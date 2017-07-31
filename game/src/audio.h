#ifndef audio_h
#define audio_h

#include <gb/gb.h>

#define audioLayerMusic 1
#define audioLayerSound 2

typedef struct {
    /**
     * -PPPNSSS
     *
     * This value is only used by the first square channel.
     *
     * PPP - Update rate. 0 to disable sweeping.
     * N - 1 to decrease, 0 to increase.
     * SSS - Shift amount. Larger values yield smaller shifts. 0 to disable.
     */
    GBUInt8 sweep;
    
    /**
     * DDLLLLLL
     *
     * DD - Duty. 00 for 12.5%, 01 for 25%, 10 for 50%, 11 for 75%.
     * LLLLLL - Length. From 0 to 0.25 seconds. Larger values yield shorter
     *   lengths.
     */
    GBUInt8 pattern;
    
    /**
     * VVVVAPPP
     *
     * VVVV - Initial volume. Larger values yield higher volumes.
     * A - Envelope direction. 0 to decrease, 1 to increase.
     * PPP - Shift amount. Larger values yield smaller shifts. 0 to disable.
     */
    GBUInt8 volume;
    
    /**
     * ------LR
     *
     * L - 1 to output to left channel.
     * R - 1 to output to right channel.
     */
    GBUInt8 flags;
} SquareInstrument;

typedef struct {
    /**
     * --LLLLLL
     *
     * LLLLLL - Length. From 0 to 0.25 seconds. Larger values yield shorter
     *   lengths.
     */
    GBUInt8 length;
    
    /**
     * VVVVAPPP
     *
     * VVVV - Initial volume. Larger values yield higher volumes.
     * A - Envelope direction. 0 to decrease, 1 to increase.
     * PPP - Shift amount. Larger values yield smaller shifts. 0 to disable.
     */
    GBUInt8 volume;
    
    /**
     * SSSSWDDD
     *
     * SSSS - Clock shift. Larger values are "dirtier". Too large a value
     *   prevents noise from working.
     * W - Width mode of LFSR. 0 for noise, 1 for buzz.
     * DDD - Divisor code. Larger values yield lower frequencies.
     */
    GBUInt8 polynomial;
    
    /**
     * ------LR
     *
     * L - 1 to output to left channel.
     * R - 1 to output to right channel.
     */
    GBUInt8 flags;
} NoiseInstrument;

typedef struct {
    /**
     * Index into `audioNoteTable`. 0 is C2, 71 is B7.
     */
    GBUInt8 note;
    
    /**
     * Index into the channel's instrument table.
     */
    GBUInt8 instrument;
    
    /**
     * A command.
     *
     * The first nibble defines the command.
     *
     * A - Set note vibrato.
     *   A-TS
     *   T - Time. A full oscillation will last 2 * T frames.
     *   S - Steps. Larger values produce larger amplitudes.
     */
    GBUInt16 command;
} AudioPatternRow;

typedef AudioPatternRow AudioPattern[16];

typedef struct {
    /**
     * Index into the composition's pattern table.
     */
    GBUInt8 pattern;
    
    /**
     * 0------- - No command
     * 1-CCLLLL - Repeat label
     *
     * LLLL - Label (0x00 - 0x0F). The index in the chain's row table to
     *   perform a jump to. An audio chain can have up to 16 locations to
     *   target by jumps.
     * CC - Repeat count (0x01 - 0x03). The repeat operation will be performed
     *   CC + 1 times (in other words, you can repeat from 1 to 3 times, for 2
     *   to 4 playback cycles).
     *
     * A jump command will jump to LLLL _after_ playing the current pattern.
     */
    GBUInt8 repeatCommand;
} AudioChainRow;

typedef struct {
    /**
     * Pointer to a list of `AudioChainRow` values.
     */
    AudioChainRow rows[64];
    
    /**
     * The count of `rows`. The chain terminates after playing the final row if
     * no repeats are triggered (whether via a row's `repeatCommand` or the
     * chain's `infiniteRepeat`).
     */
    GBUInt8 numberOfRows;
    
    /**
     * 0------- - Terminate the chain after it finishes.
     * 1---LLLL - After the chain finishes, repeat starting from label LLLL.
     */
    GBUInt8 infiniteRepeat;
    
    GBUInt8 labels[16];
} AudioChain;

typedef struct {
    SquareInstrument squareInstruments[16];
    NoiseInstrument noiseInstruments[16];
    GBUInt8 initialTempo;
    AudioChain square1Chain;
    AudioChain square2Chain;
    AudioChain noiseChain;
    AudioPattern patterns[];
} AudioComposition;

typedef GBUInt8 AudioLayer;

extern const GBUInt16 audioNoteTable[72];

/**
 * Initializes the audio system.
 */
void audioInit();

/**
 * Ticks the audio system.
 *
 * Should be called at a rate of 60 Hz. Not only should it be called 60 times
 * per second, but the interval between invocations should be relatively
 * stable. If the timings between invocations are variable by more than a few
 * milliseconds the inconsistency will be audible. The human ear is annoyingly
 * good at detecting that.
 */
void audioUpdate();

/**
 * Plays `composition` in `romBank` on `layer`, so long as either there is
 * nothing currently playing on `layer`, or the current `AudioComposition` on
 * `layer` has a priority that is less than or equal to `priority`.
 */
void audioPlayComposition(AudioComposition const * composition, GBUInt8 romBank, AudioLayer layer, GBUInt8 priority);

#endif
