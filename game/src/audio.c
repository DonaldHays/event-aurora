#include "audio.h"
#include "banks.h"

// ===
// Private Defines
// ===
#define _audioRepeatStackCount 4

// ===
// Private Types
// ===

typedef struct {
    GBUInt8 audioChainRowIndex;
    GBUInt8 repeatCount;
} AudioRepeatStackFrame;

typedef AudioRepeatStackFrame AudioRepeatStack[_audioRepeatStackCount];

typedef struct {
    GBUInt8 chainIndex;
    GBUInt8 patternIndex;
    GBUInt8 repeatStackIndex;
    AudioRepeatStack repeatStack;
} AudioChannelPlaybackState;

typedef struct {
    AudioComposition const * composition;
    AudioLayer layer;
    GBUInt8 romBank;
    GBUInt8 priority;
    GBUInt8 tempo;
    GBUInt8 tempoTimer;
    AudioChannelPlaybackState square1State;
    AudioChannelPlaybackState square2State;
    AudioChannelPlaybackState noiseState;
} AudioPlaybackState;

typedef struct {
    /**
     * 0x00 -> default
     * 0x01 -> vibratoUp
     * 0x02 -> vibratoDown
     * 0x03 -> arpeggio
     * 0x04 -> terminate
     */
    GBUInt8 mode;
    GBUInt8 vibratoConfiguration;
    GBUInt8 tickCounter;
    GBUInt16 frequency;
    GBUInt8 note;
    GBUInt8 arpeggioStepsSecond;
    GBUInt8 arpeggioStepsThird;
} SquareFrameTickState;

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

SquareFrameTickState _square1FrameTickState;
SquareFrameTickState _square2FrameTickState;

AudioPlaybackState * _updatingState;
AudioComposition const * _updatingComposition;

// ===
// Private API
// ===
void _audioInitPlaybackState(AudioPlaybackState * state) {
    state->composition = null;
    state->priority = 0;
    state->square1State.repeatStackIndex = 0;
    state->square2State.repeatStackIndex = 0;
    state->noiseState.repeatStackIndex = 0;
}

void _audioPlayComposition(AudioComposition const * composition, GBUInt8 romBank, AudioPlaybackState * state, GBUInt8 priority) {
    GBUInt8 originalBank;
    
    if(composition == null) {
        _audioInitPlaybackState(state);
        gbLog("TODO: Silence any channels owned by this layer");
        return;
    }
    
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
        state->square1State.chainIndex = 0;
        state->square1State.patternIndex = 0;
        state->square1State.repeatStackIndex = 0;
        state->square2State.chainIndex = 0;
        state->square2State.patternIndex = 0;
        state->square2State.repeatStackIndex = 0;
        state->noiseState.chainIndex = 0;
        state->noiseState.patternIndex = 0;
        state->noiseState.repeatStackIndex = 0;
    } banksROMSet(originalBank);
}

void _audioStatePlaySquareNote(AudioChannelPlaybackState * channelState, AudioChain const * chain, GBUInt8 * registers, GBUInt8 leftVolume, GBUInt8 rightVolume, SquareFrameTickState * tickState) {
    GBUInt8 patternTableIndex;
    AudioPatternRow const * patternRow;
    SquareInstrument const * instrument;
    GBUInt16 note;
    GBUInt8 upperCommand;
    GBUInt8 lowerCommand;
    GBUInt8 chainIndex;
    GBUInt8 patternIndex;
    
    chainIndex = channelState->chainIndex;
    
    if(chainIndex == chain->numberOfRows) {
        return;
    }
    
    patternIndex = channelState->patternIndex;
    
    patternTableIndex = chain->rows[chainIndex].pattern;
    patternRow = &(_updatingComposition->patterns[patternTableIndex][patternIndex]);
    note = audioNoteTable[patternRow->note];
    instrument = &(_updatingComposition->squareInstruments[patternRow->instrument]);
    upperCommand = (patternRow->command) >> 8;
    lowerCommand = (patternRow->command) & 0xFF;
    
    if(patternRow->note != 0) {
        gbAudioTerminalRegister = (instrument->flags & 0x02) ? gbAudioTerminalRegister | leftVolume : gbAudioTerminalRegister & ~leftVolume;
        gbAudioTerminalRegister = (instrument->flags & 0x01) ? gbAudioTerminalRegister | rightVolume : gbAudioTerminalRegister & ~rightVolume;
        
        registers[0] = instrument->sweep;
        registers[1] = instrument->pattern;
        registers[2] = instrument->volume;
        registers[3] = note & 0xFF;
        registers[4] = 0x80 | (note >> 8);
        
        tickState->mode = 0;
        tickState->frequency = note;
        tickState->note = patternRow->note;
    }
    
    if((upperCommand & 0xF0) == 0xA0) {
        // Vibratto
        tickState->mode = 1;
        tickState->vibratoConfiguration = lowerCommand;
        tickState->tickCounter = lowerCommand >> 5; // upper nibble (>> 4) divided by 2 (>> 1)
    } else if((upperCommand & 0xF0) == 0xB0) {
        // Arpeggio
        tickState->mode = 3;
        tickState->arpeggioStepsSecond = (lowerCommand & 0xF0) >> 4;
        tickState->arpeggioStepsThird = (lowerCommand & 0x0F);
        tickState->tickCounter = 0;
    } else if((upperCommand & 0xF0) == 0xC0) {
        // Terminate Phrase
        channelState->patternIndex = 15;
    }
}

void _audioStatePlayNoiseNote(GBUInt8 chainIndex, GBUInt8 patternIndex, AudioChain const * chain, GBUInt8 leftVolume, GBUInt8 rightVolume) {
    GBUInt8 patternTableIndex;
    AudioPatternRow const * patternRow;
    NoiseInstrument const * instrument;
    
    if(chainIndex == chain->numberOfRows) {
        return;
    }
    
    patternTableIndex = chain->rows[chainIndex].pattern;
    patternRow = &(_updatingComposition->patterns[patternTableIndex][patternIndex]);
    instrument = &(_updatingComposition->noiseInstruments[patternRow->instrument]);
    
    if(patternRow->note != 0) {
        gbAudioTerminalRegister = (instrument->flags & 0x02) ? gbAudioTerminalRegister | leftVolume : gbAudioTerminalRegister & ~leftVolume;
        gbAudioTerminalRegister = (instrument->flags & 0x01) ? gbAudioTerminalRegister | rightVolume : gbAudioTerminalRegister & ~rightVolume;
        
        gbNoiseLengthRegister = instrument->length;
        gbNoiseVolumeRegister = instrument->volume;
        gbNoisePolynomialRegister = instrument->polynomial;
        gbNoiseTriggerRegister = 0x80;
    }
}

void _audioStatePlayNotes() {
    AudioPlaybackState * state = _updatingState;
    
    _audioStatePlaySquareNote(&state->square1State, &(state->composition->square1Chain), &gbTone1SweepRegister, 0x10, 0x01, &_square1FrameTickState);
    _audioStatePlaySquareNote(&state->square2State, &(state->composition->square2Chain), &gbTone2UnusedRegister, 0x20, 0x02, &_square2FrameTickState);
    _audioStatePlayNoiseNote(state->noiseState.chainIndex, state->noiseState.patternIndex, &(state->composition->noiseChain), 0x80, 0x08);
}

void _audioIncrementChannelState(AudioChannelPlaybackState * channelState, AudioChain const * chain) {
    GBUInt8 repeatCommand;
    GBUInt8 repeatCount;
    GBUInt8 repeatIndex;
    
    if(channelState->chainIndex != chain->numberOfRows) {
        channelState->patternIndex++;
        if(channelState->patternIndex == 16) {
            channelState->patternIndex = 0;
            
            repeatCommand = chain->rows[channelState->chainIndex].repeatCommand;
            if(repeatCommand & 0x80) {
                // We have a repeat command to evaluate.
                repeatCount = (repeatCommand >> 4) & 0x03;
                repeatIndex = repeatCommand & 0x0F;
                
                if(channelState->repeatStackIndex == 0 || channelState->repeatStack[channelState->repeatStackIndex - 1].audioChainRowIndex != channelState->chainIndex) {
                    // Either the repeat stack is empty, or the top of the stack does not contain this row.
                    
                    // Make an entry in the stack.
                    channelState->repeatStack[channelState->repeatStackIndex].audioChainRowIndex = channelState->chainIndex;
                    channelState->repeatStack[channelState->repeatStackIndex].repeatCount = repeatCount;
                    channelState->repeatStackIndex++;
                    
                    // Jump to the label.
                    channelState->chainIndex = chain->labels[repeatIndex];
                    
                    // Sanity assertion.
                    if(channelState->repeatStackIndex == _audioRepeatStackCount) {
                        gbFatalError("repeat stack overflow");
                    }
                } else {
                    // We are already the top of the stack.
                    
                    // Decrease the repeat count by 1.
                    channelState->repeatStack[channelState->repeatStackIndex - 1].repeatCount--;
                    
                    if(channelState->repeatStack[channelState->repeatStackIndex - 1].repeatCount == 0) {
                        // We have finished our final repeat, so move forward.
                        channelState->chainIndex++;
                        
                        // Also pop the stack element
                        channelState->repeatStackIndex--;
                    } else {
                        // We have another repeat to do, jump to the label.
                        channelState->chainIndex = chain->labels[repeatIndex];
                    }
                }
            } else {
                // There is no repeat command, so just move forward
                channelState->chainIndex++;
            }
        }
    }
    
    if(channelState->chainIndex == chain->numberOfRows) {
        // We have reached the end of the chain.
        
        if(chain->infiniteRepeat & 0x80) {
            // We're repeating infinitely, find the label index.
            repeatIndex = chain->infiniteRepeat & 0x0F;
            
            // Jump to the label.
            channelState->chainIndex = chain->labels[repeatIndex];
        }
    }
}

void _audioStateIncrement() {
    AudioPlaybackState * state = _updatingState;
    
    state->tempoTimer = state->tempo;
    
    _audioIncrementChannelState(&state->square1State, &_updatingComposition->square1Chain);
    _audioIncrementChannelState(&state->square2State, &_updatingComposition->square2Chain);
    _audioIncrementChannelState(&state->noiseState, &_updatingComposition->noiseChain);
    
    if(state->square1State.chainIndex == _updatingComposition->square1Chain.numberOfRows && state->square2State.chainIndex == _updatingComposition->square2Chain.numberOfRows && state->noiseState.chainIndex == _updatingComposition->noiseChain.numberOfRows) {
        state->composition = null;
    }
}

void _audioUpdatePlaybackState() {
    GBUInt8 originalBank;
    
    // If there's no composition, this state is idle.
    if(_updatingComposition == null) {
        return;
    }
    
    // If we're between ticks, decrease tempo timer and return.
    if(_updatingState->tempoTimer != 0) {
        _updatingState->tempoTimer--;
        return;
    }
    
    originalBank = banksROMGet();
    banksROMSet(_updatingState->romBank); {
        _audioStatePlayNotes();
        _audioStateIncrement();
    } banksROMSet(originalBank);
}

void _updateSquareFrameTickState(SquareFrameTickState * state, GBUInt8 * frequencyRegisters) {
    GBUInt16 frequency;
    
    if(state->mode == 1) {
        // Vibratto Increment
        state->tickCounter--;
        if(state->tickCounter == 0) {
            state->tickCounter = state->vibratoConfiguration >> 4;
            state->mode = 2;
        } else {
            state->frequency += state->vibratoConfiguration & 0x0F;
            frequencyRegisters[0] = state->frequency & 0xFF;
            frequencyRegisters[1] = state->frequency >> 8;
        }
    } else if(state->mode == 2) {
        // Vibratto Decrement
        state->tickCounter--;
        if(state->tickCounter == 0) {
            state->tickCounter = state->vibratoConfiguration >> 4;
            state->mode = 1;
        } else {
            state->frequency -= state->vibratoConfiguration & 0x0F;
            frequencyRegisters[0] = state->frequency & 0xFF;
            frequencyRegisters[1] = state->frequency >> 8;
        }
    } else if(state->mode == 3) {
        // Arpeggio
        state->tickCounter++;
        if(state->tickCounter == 3) {
            state->tickCounter = 0;
        }
        
        if((state->tickCounter) == 0) {
            frequency = audioNoteTable[state->note];
        } else if((state->tickCounter) == 1) {
            frequency = audioNoteTable[state->note + state->arpeggioStepsSecond];
        } else {
            frequency = audioNoteTable[state->note + state->arpeggioStepsSecond + state->arpeggioStepsThird];
        }
        
        frequencyRegisters[0] = frequency & 0xFF;
        frequencyRegisters[1] = (frequency >> 8);
    }
}

// ===
// Public API
// ===
void audioInit() {
    gbAudioTerminalRegister = 0xFF;
    
    _audioInitPlaybackState(&_soundPlaybackState);
    _audioInitPlaybackState(&_musicPlaybackState);
    
    _square1FrameTickState.mode = 0;
    _square2FrameTickState.mode = 0;
    
    _soundPlaybackState.layer = audioLayerSound;
    _musicPlaybackState.layer = audioLayerMusic;
}

void audioUpdate() {
    _updateSquareFrameTickState(&_square1FrameTickState, &gbTone1FrequencyLowRegister);
    _updateSquareFrameTickState(&_square2FrameTickState, &gbTone2FrequencyLowRegister);
    
    _updatingState = &_soundPlaybackState;
    _updatingComposition = _updatingState->composition;
    _audioUpdatePlaybackState();
    
    _updatingState = &_musicPlaybackState;
    _updatingComposition = _updatingState->composition;
    _audioUpdatePlaybackState();
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
