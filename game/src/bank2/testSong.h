#ifndef testsong_h
#define testsong_h

#include <gb/gb.h>

typedef struct {
    GBUInt8 note;
    GBUInt8 instrument;
    GBUInt16 command;
} PatternRow;

void testSongInit();
void testSongUpdate();

#endif
