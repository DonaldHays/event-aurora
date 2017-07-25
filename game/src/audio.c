#include "audio.h"
#include "banks.h"

// ===
// Private Types
// ===
typedef struct {
    AudioComposition const * composition;
    AudioLayer layer;
    GBUInt8 romBank;
    GBUInt8 priority;
    GBUInt8 tempo;
    GBUInt8 tempoTimer;
    GBUInt8 square1ChainIndex;
    GBUInt8 square1PatternIndex;
} AudioPlaybackState;

// ===
// Public Constant Data
// ===

// At 144 bytes of constant data, this might be a reasonable candidate to push
// into a ROM bank if the HOME bank gets tight on space.

const GBUInt16 audioNoteTable[72] = {
    0x002C, // 0  - C2  - 65.40639132514954
    0x009D, // 1  - C#2 - 69.2956577442179
    0x0107, // 2  - D2  - 73.41619197935177
    0x016B, // 3  - D#2 - 77.7817459305201
    0x01C9, // 4  - E2  - 82.40688922821735
    0x0223, // 5  - F2  - 87.30705785825084
    0x0277, // 6  - F#2 - 92.49860567790847
    0x02C7, // 7  - G2  - 97.99885899543719
    0x0312, // 8  - G#2 - 103.82617439498614
    0x0358, // 9  - A2  - 109.99999999999986
    0x039B, // 10 - A#2 - 116.54094037952234
    0x03DA, // 11 - B2  - 123.47082531403089
    0x0416, // 12 - C3  - 130.81278265029917
    0x044E, // 13 - C#3 - 138.5913154884359
    0x0483, // 14 - D3  - 146.83238395870362
    0x04B5, // 15 - D#3 - 155.5634918610403
    0x04E5, // 16 - E3  - 164.8137784564348
    0x0511, // 17 - F3  - 174.61411571650177
    0x053B, // 18 - F#3 - 184.99721135581703
    0x0563, // 19 - G3  - 195.99771799087446
    0x0589, // 20 - G#3 - 207.6523487899724
    0x05AC, // 21 - A3  - 219.99999999999983
    0x05CE, // 22 - A#3 - 233.0818807590448
    0x05ED, // 23 - B3  - 246.9416506280619
    0x060B, // 24 - C4  - 261.62556530059845
    0x0627, // 25 - C#4 - 277.1826309768719
    0x0642, // 26 - D4  - 293.66476791740735
    0x065B, // 27 - D#4 - 311.1269837220807
    0x0672, // 28 - E4  - 329.6275569128697
    0x0689, // 29 - F4  - 349.22823143300366
    0x069E, // 30 - F#4 - 369.9944227116342
    0x06B2, // 31 - G4  - 391.9954359817491
    0x06C4, // 32 - G#4 - 415.30469757994496
    0x06D6, // 33 - A4  - 439.99999999999983
    0x06E7, // 34 - A#4 - 466.16376151808976
    0x06F7, // 35 - B4  - 493.88330125612396
    0x0706, // 36 - C5  - 523.2511306011971
    0x0714, // 37 - C#5 - 554.365261953744
    0x0721, // 38 - D5  - 587.329535834815
    0x072D, // 39 - D#5 - 622.2539674441617
    0x0739, // 40 - E5  - 659.2551138257398
    0x0744, // 41 - F5  - 698.4564628660078
    0x074F, // 42 - F#5 - 739.9888454232688
    0x0759, // 43 - G5  - 783.9908719634986
    0x0762, // 44 - G#5 - 830.6093951598904
    0x076B, // 45 - A5  - 880.0000000000001
    0x0773, // 46 - A#5 - 932.32752303618
    0x077B, // 47 - B5  - 987.7666025122484
    0x0783, // 48 - C6  - 1046.5022612023947
    0x078A, // 49 - C#6 - 1108.7305239074885
    0x0790, // 50 - D6  - 1174.6590716696305
    0x0797, // 51 - D#6 - 1244.507934888324
    0x079D, // 52 - E6  - 1318.5102276514801
    0x07A2, // 53 - F6  - 1396.912925732016
    0x07A7, // 54 - F#6 - 1479.977690846538
    0x07AC, // 55 - G6  - 1567.9817439269978
    0x07B1, // 56 - G#6 - 1661.2187903197812
    0x07B6, // 57 - A6  - 1760.0000000000007
    0x07BA, // 58 - A#6 - 1864.6550460723604
    0x07BE, // 59 - B6  - 1975.5332050244972
    0x07C1, // 60 - C7  - 2093.00452240479
    0x07C5, // 61 - C#7 - 2217.4610478149775
    0x07C8, // 62 - D7  - 2349.3181433392615
    0x07CB, // 63 - D#7 - 2489.0158697766483
    0x07CE, // 64 - E7  - 2637.0204553029607
    0x07D1, // 65 - F7  - 2793.8258514640324
    0x07D4, // 66 - F#7 - 2959.9553816930766
    0x07D6, // 67 - G7  - 3135.963487853996
    0x07D9, // 68 - G#7 - 3322.437580639563
    0x07DB, // 69 - A7  - 3520.000000000002
    0x07DD, // 70 - A#7 - 3729.3100921447212
    0x07DF  // 71 - B7  - 3951.0664100489953
};

// ===
// Private Variables
// ===
AudioPlaybackState _soundPlaybackState;
AudioPlaybackState _musicPlaybackState;

// ===
// Private API
// ===
void _audioInitPlaybackState(AudioPlaybackState * state) {
    state->composition = null;
    state->priority = 0;
}

void _audioPlayComposition(AudioComposition const * composition, GBUInt8 romBank, AudioPlaybackState * state, GBUInt8 priority) {
    GBUInt8 originalBank;
    
    if(state->composition != null && state->priority > priority) {
        return;
    }
    
    originalBank = banksROMGet();
    banksROMSet(romBank); {
        state->composition = composition;
        state->romBank = romBank;
        state->priority = priority;
        state->tempo = composition->initialTempo;
        state->tempoTimer = 0;
        state->square1ChainIndex = 0;
        state->square1PatternIndex = 0;
    } banksROMSet(originalBank);
}

void _audioStatePlayNotes(AudioPlaybackState * state) {
    GBUInt8 patternTableIndex;
    AudioPatternRow const * patternRow;
    SquareInstrument const * instrument;
    GBUInt16 note;
    
    patternTableIndex = state->composition->square1Chain.rows[state->square1ChainIndex].pattern;
    patternRow = &(state->composition->patterns[patternTableIndex][state->square1PatternIndex]);
    instrument = &(state->composition->squareInstruments[patternRow->instrument]);
    note = audioNoteTable[patternRow->note];
    
    if(patternRow->note == 0) {
        return;
    }
    
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

void _audioStateIncrement(AudioPlaybackState * state) {
    state->tempoTimer = state->tempo;
    
    state->square1PatternIndex++;
    if(state->square1PatternIndex == 16) {
        state->square1PatternIndex = 0;
        state->square1ChainIndex++;
    }
    
    if(state->square1ChainIndex == state->composition->square1Chain.numberOfRows) {
        state->composition = null;
    }
}

void _audioUpdatePlaybackState(AudioPlaybackState * state) {
    GBUInt8 originalBank;
    
    // If there's no composition, this state is idle.
    if(state->composition == null) {
        return;
    }
    
    // If we're between ticks, decrease tempo timer and return.
    if(state->tempoTimer != 0) {
        state->tempoTimer--;
        return;
    }
    
    originalBank = banksROMGet();
    banksROMSet(state->romBank); {
        _audioStatePlayNotes(state);
        _audioStateIncrement(state);
    } banksROMSet(originalBank);
}

// ===
// Public API
// ===
void audioInit() {
    gbAudioTerminalRegister = 0xFF;
    
    _audioInitPlaybackState(&_soundPlaybackState);
    _audioInitPlaybackState(&_musicPlaybackState);
    
    _soundPlaybackState.layer = audioLayerSound;
    _musicPlaybackState.layer = audioLayerMusic;
}

void audioUpdate() {
    _audioUpdatePlaybackState(&_soundPlaybackState);
    _audioUpdatePlaybackState(&_musicPlaybackState);
}

void audioPlayComposition(AudioComposition const * composition, GBUInt8 romBank, AudioLayer layer, GBUInt8 priority) {
    if(layer == audioLayerSound) {
        _audioPlayComposition(composition, romBank, &_soundPlaybackState, priority);
    } else if(layer == audioLayerMusic) {
        _audioPlayComposition(composition, romBank, &_musicPlaybackState, priority);
    } else {
        gbLogUInt8(layer);
        gbFatalError("unrecognized audio layer");
    }
}
