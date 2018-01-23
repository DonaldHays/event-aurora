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

/// An AudioHardwareChannelState manages the program-level view of one of the
/// audio channels for the device. There is one instance of this struct per
/// hardware channel, _not_ per program-level layer.
typedef struct {
    GBUInt8 padding;
} AudioHardwareChannelState;

/// An AudioLayerChannelState represents the memory for a single channel of
/// a layer.
typedef struct {
    GBUInt8 padding;
} AudioLayerChannelState;

/// An AudioLayerState represents the memory for an audio layer.
typedef struct {
    AudioLayer layer;
    AudioComposition const * composition;
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

AudioHardwareChannelState * _currentHardwareChannelState;
AudioLayerChannelState * _currentLayerChannelState;
AudioLayerState * _currentLayerState;

// ===
// Private API
// ===

/// Initializes the values of `_currentHardwareChannelState`.
void _audioInitHardwareChannelState() {
    _currentHardwareChannelState->padding = 0;
}

/// Initializes the values of `_currentLayerChannelState`.
void _audioInitAudioLayerChannelState() {
    _currentLayerChannelState->padding = 0;
}

/// Initializes the values of `_currentLayerState`.
void _audioInitAudioLayerState() {
    AudioLayerState * state = _currentLayerState;
    
    state->composition = null;
    
    // This function assumes square1, square2, and noise AudioLayerChannelState
    // instances follow each other in AudioLayerState.
    
    _currentLayerChannelState = &(state->square1);
    _audioInitAudioLayerChannelState();
    
    _currentLayerChannelState += sizeof(AudioLayerChannelState);
    _audioInitAudioLayerChannelState();
    
    _currentLayerChannelState += sizeof(AudioLayerChannelState);
    _audioInitAudioLayerChannelState();
}

/// Ticks _currentHardwareChannelState.
void _audioHardwareChannelStateUpdate() {
    
}

/// Ticks _currentLayerChannelState.
void _audioLayerChannelStateUpdate() {
    
}

/// Ticks _currentLayerState if it has a composition.
void _audioLayerStateUpdate() {
    AudioLayerState * state = _currentLayerState;
    
    if(state->composition == null) {
        return;
    }
    
    // This function assumes square1, square2, and noise AudioLayerChannelState
    // instances follow each other in AudioLayerState.
    
    _currentLayerChannelState = &(state->square1);
    _audioLayerChannelStateUpdate();
    
    _currentLayerChannelState += sizeof(AudioLayerChannelState);
    _audioLayerChannelStateUpdate();
    
    _currentLayerChannelState += sizeof(AudioLayerChannelState);
    _audioLayerChannelStateUpdate();
}

// ===
// Public API
// ===
void audioInit() {
    gbAudioTerminalRegister = 0xFF;
    
    // Initialize hardware channel states.
    
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
    (void)romBank;
    (void)priority;
    
    if(layer == audioLayerSound) {
        _soundState.composition = composition;
    } else if(layer == audioLayerMusic) {
        _musicState.composition = composition;
    } else {
        gbLogUInt8(layer);
        gbFatalError("unrecognized audio layer");
    }
}
