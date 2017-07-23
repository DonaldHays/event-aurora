#include "testSong.h"

#pragma bank 2

const SquareInstrument testInstruments[2] = {
    { 0x00, 0x40, 0xF3, 0x03 },
    { 0x00, 0x80, 0x73, 0x03 }
};

const AudioPattern testPattern = {
    { 24, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 28, 1, 0 },
    
    { 28, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 31, 1, 0 },
    
    { 31, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 28, 1, 0 },
    
    { 28, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
};

const AudioComposition testComposition = {
    testInstruments,
    &testPattern,
    { null, 0, 0 }
};

AudioPattern const * currentPattern;
GBUInt8 patternIndex;
GBUInt8 tempo;
GBUInt8 tempoCount;
GBUInt16 currentNote;
GBUInt8 vibrattoCycle;

void play(GBUInt16 note, SquareInstrument const * instrument) {
    currentNote = note;
    
    gbTone1SweepRegister = instrument->sweep;
    gbTone1PatternRegister = instrument->pattern;
    gbTone1VolumeRegister = instrument->volume;
    
    if(instrument->flags & 0x02) {
        gbAudioTerminalRegister |= 0x10;
    } else {
        gbAudioTerminalRegister &= ~0x10;
    }
    
    if(instrument->flags & 0x01) {
        gbAudioTerminalRegister |= 0x01;
    } else {
        gbAudioTerminalRegister &= ~0x01;
    }
    
    gbTone1FrequencyLowRegister = note & 0xFF;
    gbTone1TriggerRegister = 0x80 | (note >> 8);
}

void testSongInit() {
    gbAudioTerminalRegister = 0xFF;
    
    currentPattern = &testPattern;
    patternIndex = 0;
    tempo = 12;
    tempoCount = tempo;
    vibrattoCycle = 0;
    
    if(currentPattern[0][0].note) {
        play(audioNoteTable[currentPattern[0][0].note], &testInstruments[currentPattern[0][0].instrument]);
    }
}

void testSongUpdate() {
    AudioPatternRow const * currentRow;
    
    if(currentPattern == null) {
        return;
    }
    
    tempoCount--;
    if(tempoCount == 0) {
        tempoCount = tempo;
        
        patternIndex++;
        if(patternIndex == 16) {
            currentPattern = null;
            return;
        }
        
        currentRow = &currentPattern[0][patternIndex];
        
        if(currentRow->note) {
            play(audioNoteTable[currentRow->note], &testInstruments[currentRow->instrument]);
        }
    }
    
    // ===
    // Arpeggio
    // ===
    
    // vibrattoCycle++;
    // if(vibrattoCycle == 3) {
    //     vibrattoCycle = 0;
    // }
    
    // gbTone1FrequencyLowRegister = (currentNote + 104 * vibrattoCycle) & 0xFF;
    // gbTone1TriggerRegister = ((currentNote + 104 * vibrattoCycle) >> 8);
    
    // ===
    // Vibratto
    // ===
    
    // vibrattoCycle++;
    // if(vibrattoCycle == 16) {
    //     vibrattoCycle = 0;
    // }
    
    // if(vibrattoCycle < 8) {
    //     currentNote++;
    // } else {
    //     currentNote--;
    // }
    // gbTone1FrequencyLowRegister = currentNote & 0xFF;
    // gbTone1TriggerRegister = (currentNote >> 8);
}
