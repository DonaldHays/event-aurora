#include "audio.h"
#include "banks.h"
#include "memory.h"

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

/// An AudioHardwareChannelState manages the program-level view of one of the
/// audio channels for the device. There is one instance of this struct per
/// hardware channel, _not_ per program-level layer.
typedef struct {
    AudioLayer ownershipClaims;
    GBUInt8 * registers;
    
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
} AudioHardwareChannelState;

/// An AudioLayerChannelState represents the memory for a single channel of
/// a layer.
typedef struct {
    GBUInt8 chainIndex;
    GBUInt8 patternIndex;
    GBUInt8 repeatStackIndex;
    GBUInt8 leftVolumeFlag;
    GBUInt8 rightVolumeFlag;
    GBBool isNoise;
    AudioRepeatStack repeatStack;
} AudioLayerChannelState;

/// An AudioLayerState represents the memory for an audio layer.
typedef struct {
    AudioLayer layer;
    AudioComposition const * composition;
    GBUInt8 priority;
    GBUInt8 romBank;
    GBUInt8 tempo;
    GBUInt8 tempoTimer;
    AudioLayerChannelState square1;
    AudioLayerChannelState square2;
    AudioLayerChannelState noise;
} AudioLayerState;

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
AudioHardwareChannelState _square1State;
AudioHardwareChannelState _square2State;
AudioHardwareChannelState _noiseState;

AudioLayerState _musicState;
AudioLayerState _soundState;

AudioChain const * _currentAudioChain;
AudioHardwareChannelState * _currentHardwareChannelState;
AudioLayerChannelState * _currentLayerChannelState;
AudioLayerState * _currentLayerState;

// ===
// Private API
// ===

/// Initializes the values of `_currentHardwareChannelState`.
void _audioInitHardwareChannelState() {
    _currentHardwareChannelState->ownershipClaims = 0;
    memorySet(_currentHardwareChannelState->registers, 0, 5);
    _currentHardwareChannelState->mode = 0;
}

/// Initializes the values of `_currentLayerState`.
void _audioInitAudioLayerState() {
    AudioLayerState * state = _currentLayerState;
    
    state->composition = null;
    
    state->square1.leftVolumeFlag = 0x10;
    state->square1.rightVolumeFlag = 0x01;
    state->square1.isNoise = false;
    state->square2.leftVolumeFlag = 0x20;
    state->square2.rightVolumeFlag = 0x02;
    state->square2.isNoise = false;
    state->noise.leftVolumeFlag = 0x80;
    state->noise.rightVolumeFlag = 0x08;
    state->noise.isNoise = true;
}

GBBool _audioHardwareChannelIsCurrentOwner() {
    GBUInt8 layer, layerMask;
    
    layer = _currentLayerState->layer;
    layerMask = ~layer;
    
    return (_currentHardwareChannelState->ownershipClaims & layer) && (layer > (_currentHardwareChannelState->ownershipClaims & layerMask));
}

/// Releases any ownership claims by `_currentLayerState` on 
/// `_currentHardwareChannelState`.
void _audioHardwareChannelReleaseOwner() {
    GBUInt8 layer, layerMask;
    
    layer = _currentLayerState->layer;
    layerMask = ~layer;
    
    // If layer is in the current ownershipClaims value and is the most
    // significant bit, then there was audio being actively played by the
    // layer, so we need to trigger silence.
    if((_currentHardwareChannelState->ownershipClaims & layer) && (layer > (_currentHardwareChannelState->ownershipClaims & layerMask))) {
        gbAudioTerminalRegister = gbAudioTerminalRegister & ~(_currentLayerChannelState->leftVolumeFlag | _currentLayerChannelState->rightVolumeFlag);
        _currentHardwareChannelState->mode = 0;
    }
    
    // Remove the current layer from the ownership claims
    _currentHardwareChannelState->ownershipClaims &= layerMask;
}

void _audioHardwareChannelGainOwner() {
    GBUInt8 layer;
    
    layer = _currentLayerState->layer;
    
    // If layer is greater than the current ownershipClaims value, then it'll
    // be taking control of the hardware channel, so we need to trigger
    // silence.
    if(layer > _currentHardwareChannelState->ownershipClaims) {
        gbAudioTerminalRegister = gbAudioTerminalRegister & ~(_currentLayerChannelState->leftVolumeFlag | _currentLayerChannelState->rightVolumeFlag);
        _currentHardwareChannelState->mode = 0;
    }
    
    _currentHardwareChannelState->ownershipClaims |= layer;
}

/// Ticks _currentHardwareChannelState.
void _audioHardwareChannelStateUpdate() {
    GBUInt16 frequency;
    AudioHardwareChannelState * state;
    
    state = _currentHardwareChannelState;
    
    if(state->mode == 1) {
        // Vibratto Increment
        state->tickCounter--;
        if(state->tickCounter == 0) {
            state->tickCounter = state->vibratoConfiguration >> 4;
            state->mode = 2;
        } else {
            state->frequency += state->vibratoConfiguration & 0x0F;
            
            state->registers[3] = state->frequency & 0xFF;
            state->registers[4] = state->frequency >> 8;
        }
    } else if(state->mode == 2) {
        // Vibratto Decrement
        state->tickCounter--;
        if(state->tickCounter == 0) {
            state->tickCounter = state->vibratoConfiguration >> 4;
            state->mode = 1;
        } else {
            state->frequency -= state->vibratoConfiguration & 0x0F;
            
            state->registers[3] = state->frequency & 0xFF;
            state->registers[4] = state->frequency >> 8;
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
        
        state->registers[3] = frequency & 0xFF;
        state->registers[4] = (frequency >> 8);
    }
}

void _audioLayerChannelStatePlayNote() {
    AudioComposition const * composition;
    GBUInt8 patternTableIndex, patternIndex;
    AudioPatternRow const * patternRow;
    GBUInt8 const * instrument;
    GBUInt8 * registers;
    GBUInt16 note;
    GBUInt8 upperCommand;
    GBUInt8 lowerCommand;
    GBBool isHardwareOwner;
    
    AudioLayerChannelState * currentLayerChannelState;
    AudioHardwareChannelState * currentHardwareChannelState;
    
    currentLayerChannelState = _currentLayerChannelState;
    currentHardwareChannelState = _currentHardwareChannelState;
    
    composition = _currentLayerState->composition;
    patternIndex = currentLayerChannelState->patternIndex;
    patternTableIndex = _currentAudioChain->rows[currentLayerChannelState->chainIndex].pattern;
    patternRow = &(composition->patterns[patternTableIndex][patternIndex]);
    if(currentLayerChannelState->isNoise) {
        instrument = (GBUInt8 *)&(composition->noiseInstruments[patternRow->instrument]);
        registers = currentHardwareChannelState->registers + 1;
    } else {
        instrument = (GBUInt8 *)&(composition->squareInstruments[patternRow->instrument]);
        registers = currentHardwareChannelState->registers;
    }
    upperCommand = (patternRow->command) >> 8;
    lowerCommand = (patternRow->command) & 0xFF;
    isHardwareOwner = _audioHardwareChannelIsCurrentOwner();
    
    if(patternRow->note != 0 && isHardwareOwner) {
        *(registers++) = *(instrument++);
        *(registers++) = *(instrument++);
        *(registers++) = *(instrument++);
        
        gbAudioTerminalRegister = ((*instrument) & 0x02) ? gbAudioTerminalRegister | currentLayerChannelState->leftVolumeFlag : gbAudioTerminalRegister & ~currentLayerChannelState->leftVolumeFlag;
        gbAudioTerminalRegister = ((*instrument) & 0x01) ? gbAudioTerminalRegister | currentLayerChannelState->rightVolumeFlag : gbAudioTerminalRegister & ~currentLayerChannelState->rightVolumeFlag;
        
        if(currentLayerChannelState->isNoise) {
            *registers = 0x80;
        } else {
            note = audioNoteTable[patternRow->note];
            
            *(registers++) = note & 0xFF;
            *registers = 0x80 | (note >> 8);
            
            currentHardwareChannelState->frequency = note;
        }
        
        currentHardwareChannelState->mode = 0;
        currentHardwareChannelState->note = patternRow->note;
    }
    
    if((upperCommand & 0xF0) == 0xA0 && isHardwareOwner) {
        // Vibratto
        currentHardwareChannelState->mode = 1;
        currentHardwareChannelState->vibratoConfiguration = lowerCommand;
        currentHardwareChannelState->tickCounter = lowerCommand >> 5; // upper nibble (>> 4) divided by 2 (>> 1)
    } else if((upperCommand & 0xF0) == 0xB0 && isHardwareOwner) {
        // Arpeggio
        currentHardwareChannelState->mode = 3;
        currentHardwareChannelState->arpeggioStepsSecond = (lowerCommand & 0xF0) >> 4;
        currentHardwareChannelState->arpeggioStepsThird = (lowerCommand & 0x0F);
        currentHardwareChannelState->tickCounter = 0;
    } else if((upperCommand & 0xF0) == 0xC0) {
        // Terminate Phrase
        currentLayerChannelState->patternIndex = 15;
    }
}

void _audioLayerChannelStateIncrement() {
    GBUInt8 repeatCommand;
    GBUInt8 repeatCount;
    GBUInt8 repeatIndex;
    
    if((++(_currentLayerChannelState->patternIndex)) == 16) {
        _currentLayerChannelState->patternIndex = 0;
        
        repeatCommand = _currentAudioChain->rows[_currentLayerChannelState->chainIndex].repeatCommand;
        
        if(repeatCommand & 0x80) {
            // We have a repeat command to evaluate.
            repeatCount = (repeatCommand >> 4) & 0x03;
            repeatIndex = repeatCommand & 0x0F;
            
            if(_currentLayerChannelState->repeatStackIndex == 0 || _currentLayerChannelState->repeatStack[_currentLayerChannelState->repeatStackIndex - 1].audioChainRowIndex != _currentLayerChannelState->chainIndex) {
                // Either the repeat stack is empty, or the top of the stack does not contain this row.
                
                // Make an entry in the stack.
                _currentLayerChannelState->repeatStack[_currentLayerChannelState->repeatStackIndex].audioChainRowIndex = _currentLayerChannelState->chainIndex;
                _currentLayerChannelState->repeatStack[_currentLayerChannelState->repeatStackIndex].repeatCount = repeatCount;
                _currentLayerChannelState->repeatStackIndex++;
                
                // Jump to the label.
                _currentLayerChannelState->chainIndex = _currentAudioChain->labels[repeatIndex];
                
                // Sanity assertion.
                if(_currentLayerChannelState->repeatStackIndex == _audioRepeatStackCount) {
                    gbFatalError("repeat stack overflow");
                }
            } else {
                // We are already the top of the stack.
                
                // Decrease the repeat count by 1.
                _currentLayerChannelState->repeatStack[_currentLayerChannelState->repeatStackIndex - 1].repeatCount--;
                
                if(_currentLayerChannelState->repeatStack[_currentLayerChannelState->repeatStackIndex - 1].repeatCount == 0) {
                    // We have finished our final repeat, so move forward.
                    _currentLayerChannelState->chainIndex++;
                    
                    // Also pop the stack element
                    _currentLayerChannelState->repeatStackIndex--;
                } else {
                    // We have another repeat to do, jump to the label.
                    _currentLayerChannelState->chainIndex = _currentAudioChain->labels[repeatIndex];
                }
            }
        } else {
            // There is no repeat command, so just move forward
            _currentLayerChannelState->chainIndex++;
        }
        
        if(_currentLayerChannelState->chainIndex == _currentAudioChain->numberOfRows) {
            // We have reached the end of the chain.
        
            if(_currentAudioChain->infiniteRepeat & 0x80) {
                // We're repeating infinitely, find the label index.
                repeatIndex = _currentAudioChain->infiniteRepeat & 0x0F;
                
                // Jump to the label.
                _currentLayerChannelState->chainIndex = _currentAudioChain->labels[repeatIndex];
            } else {
                _audioHardwareChannelReleaseOwner();
            }
        }
    }
}

/// Ticks _currentLayerChannelState. This should only happen when the tempo
/// wraps.
void _audioLayerChannelStateUpdate() {
    if(_currentLayerChannelState->chainIndex == _currentAudioChain->numberOfRows) {
        return;
    }
    
    _audioLayerChannelStatePlayNote();
    _audioLayerChannelStateIncrement();
}

/// Ticks _currentLayerState if it has a composition.
void _audioLayerStateUpdate() {
    GBUInt8 originalBank;
    AudioLayerState * state = _currentLayerState;
    AudioComposition const * composition;
    
    if((composition = state->composition) == null) {
        return;
    }
    
    if(state->tempoTimer != 0) {
        state->tempoTimer--;
        return;
    }
    
    state->tempoTimer = state->tempo;
    
    // This function assumes square1, square2, and noise AudioLayerChannelState
    // instances follow each other in AudioLayerState.
    
    originalBank = banksROMGet();
    banksROMSet(state->romBank); {
        _currentHardwareChannelState = &_square1State;
        _currentLayerChannelState = &(state->square1);
        _currentAudioChain = &(composition->square1Chain);
        _audioLayerChannelStateUpdate();
        
        _currentHardwareChannelState = &_square2State;
        _currentLayerChannelState++;
        _currentAudioChain++;
        _audioLayerChannelStateUpdate();
        
        _currentHardwareChannelState = &_noiseState;
        _currentLayerChannelState++;
        _currentAudioChain++;
        _audioLayerChannelStateUpdate();
        
        if(state->square1.chainIndex == composition->square1Chain.numberOfRows && state->square2.chainIndex == composition->square2Chain.numberOfRows && state->noise.chainIndex == composition->noiseChain.numberOfRows) {
            state->composition = null;
        }
    }; banksROMSet(originalBank);
}

// ===
// Public API
// ===
void audioInit() {
    gbAudioTerminalRegister = 0xFF;
    
    // Initialize hardware channel states.
    
    _square1State.registers = &gbTone1SweepRegister;
    _square2State.registers = &gbTone2UnusedRegister;
    _noiseState.registers = &gbNoiseUnusedRegister;
    
    _currentHardwareChannelState = &_square1State;
    _audioInitHardwareChannelState();
    
    _currentHardwareChannelState = &_square2State;
    _audioInitHardwareChannelState();
    
    _currentHardwareChannelState = &_noiseState;
    _audioInitHardwareChannelState();
    
    // Initialize audio layer states.
    
    _musicState.layer = audioLayerMusic;
    _soundState.layer = audioLayerSound;
    
    _currentLayerState = &_musicState;
    _audioInitAudioLayerState();
    
    _currentLayerState = &_soundState;
    _audioInitAudioLayerState();
}

void audioUpdate() {
    // Update the layers.
    
    // We update sound before music because sound overrides music, but if it
    // releases a hardware channel in the same frame that music will play a new
    // sound, we should play that sound.
    _currentLayerState = &_soundState;
    _audioLayerStateUpdate();
    
    _currentLayerState = &_musicState;
    _audioLayerStateUpdate();
    
    // Update the hardware channel states.
    
    _currentHardwareChannelState = &_square1State;
    _audioHardwareChannelStateUpdate();
    
    _currentHardwareChannelState = &_square2State;
    _audioHardwareChannelStateUpdate();
    
    _currentHardwareChannelState = &_noiseState;
    _audioHardwareChannelStateUpdate();
}

void audioPlayComposition(AudioComposition const * composition, GBUInt8 romBank, AudioLayer layer, GBUInt8 priority) {
    AudioLayerState * state;
    GBUInt8 originalBank;
    GBUInt8 * statePointer;
    
    // Get the state for the layer we'll be working with
    if(layer == audioLayerSound) {
        state = &_soundState;
    } else if(layer == audioLayerMusic) {
        state = &_musicState;
    } else {
        gbLogUInt8(layer);
        gbFatalError("unrecognized audio layer");
        state = null;
    }
    
    // Don't play the composition if it has a lower priority than the
    // active composition.
    if(state->composition != null && state->priority > priority) {
        return;
    }
    
    // Release any claims the current layer has on hardware channels.
    
    _currentLayerState = state;
    
    _currentHardwareChannelState = &_square1State;
    _currentLayerChannelState = &(_currentLayerState->square1);
    _audioHardwareChannelReleaseOwner();
    
    _currentHardwareChannelState = &_square2State;
    _currentLayerChannelState = &(_currentLayerState->square2);
    _audioHardwareChannelReleaseOwner();
    
    _currentHardwareChannelState = &_noiseState;
    _currentLayerChannelState = &(_currentLayerState->noise);
    _audioHardwareChannelReleaseOwner();
    
    if(composition == null) {
        // There's no composition, so we're done here.
        state->composition = null;
    } else {
        // Assign the composition
        originalBank = banksROMGet();
        banksROMSet(romBank); {
            state->composition = composition;
            
            statePointer = &(state->priority);
            *(statePointer++) = priority;
            *(statePointer++) = romBank;
            *(statePointer++) = composition->initialTempo;
            *(statePointer++) = 0;
            
            state->square1.chainIndex = 0;
            state->square1.patternIndex = 0;
            state->square1.repeatStackIndex = 0;
            
            state->square2.chainIndex = 0;
            state->square2.patternIndex = 0;
            state->square2.repeatStackIndex = 0;
            
            state->noise.chainIndex = 0;
            state->noise.patternIndex = 0;
            state->noise.repeatStackIndex = 0;
            
            if(composition->square1Chain.numberOfRows != 0) {
                _currentHardwareChannelState = &_square1State;
                _currentLayerChannelState = &(_currentLayerState->square1);
                _audioHardwareChannelGainOwner();
            }
            
            if(composition->square2Chain.numberOfRows != 0) {
                _currentHardwareChannelState = &_square2State;
                _currentLayerChannelState = &(_currentLayerState->square2);
                _audioHardwareChannelGainOwner();
            }
            
            if(composition->noiseChain.numberOfRows != 0) {
                _currentHardwareChannelState = &_noiseState;
                _currentLayerChannelState = &(_currentLayerState->noise);
                _audioHardwareChannelGainOwner();
            }
        } banksROMSet(originalBank);
    }
}
