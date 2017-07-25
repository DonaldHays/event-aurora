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

const AudioChainRow testRows[] = {
    { 0, 0 },
    { 0, 0 }
};

const AudioComposition testComposition = {
    testInstruments,
    &testPattern,
    12,
    { testRows, 2, 0 }
};
