#ifndef audio_h
#define audio_h

#include <gb/gb.h>

#define audioLayerSound 1
#define audioLayerMusic 2

typedef struct {
    GBUInt8 padding;
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
