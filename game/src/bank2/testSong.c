#include "testSong.h"

#pragma bank 2

const SquareInstrument testInstruments[] = {
    { 0x00, 0x40, 0xF3, 0x03 },
    { 0x00, 0x80, 0x73, 0x03 },
    { 0x00, 0x00, 0xF1, 0x03 },
    { 0x00, 0x00, 0xF1, 0x02 },
    { 0x00, 0x00, 0xF1, 0x01 }
};

const NoiseInstrument testNoiseInstruments[] = {
    { 0x00, 0xF3, 0x33, 0x03 }
};

const AudioPattern testPatterns[] = {
    {
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
    },
    {
        { 12, 2, 0 },
        { 0, 0, 0 },
        { 12, 3, 0 },
        { 16, 4, 0 },
        
        { 12, 2, 0 },
        { 0, 0, 0 },
        { 12, 3, 0 },
        { 16, 4, 0 },
        
        { 12, 2, 0 },
        { 0, 0, 0 },
        { 12, 3, 0 },
        { 16, 4, 0 },
        
        { 12, 2, 0 },
        { 0, 0, 0 },
        { 16, 3, 0 },
        { 12, 4, 0 },
    },
    {
        { 1, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 },
        
        { 1, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 },
        
        { 1, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 },
        
        { 1, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 },
        { 0, 0, 0 }
    }
};

const AudioChainRow testRowsSquare1[] = {
    { 0, 0 },
    { 0, 0xB0 }
};

const AudioChainRow testRowsSquare2[] = {
    { 1, 0 },
    { 1, 0 }
};

const AudioChainRow testRowsNoise[] = {
    { 2, 0 },
    { 2, 0 }
};

const AudioComposition testComposition = {
    testInstruments,
    testNoiseInstruments,
    testPatterns,
    8,
    { testRowsSquare1, 2, 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { testRowsSquare2, 2, 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { testRowsNoise, 2, 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
};
