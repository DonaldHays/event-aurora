#ifndef sprites_h
#define sprites_h

#include <gb/gb.h>

typedef struct {
    GBUInt8 y;
    GBUInt8 x;
    GBUInt8 tileIndex;
    GBUInt8 attributes;
} SpriteAttributes;

extern volatile SpriteAttributes * spriteAttributes;

void spritesInit();
void spritesClear();
void spritesWriteToOAM();

#endif
