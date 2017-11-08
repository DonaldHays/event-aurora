#ifndef sprites_h
#define sprites_h

#include <gb/gb.h>

typedef struct {
    GBUInt8 y;
    GBUInt8 x;
    GBUInt8 tileIndex;
    GBUInt8 attributes;
} SpriteAttributes;

typedef struct {
    GBUInt8 topLeftIndex;
    GBUInt8 topRightIndex;
    GBUInt8 bottomLeftIndex;
    GBUInt8 bottomRightIndex;
} SpriteFrame2x2;

typedef struct {
    GBUInt8 frameIndex;
    GBUInt8 duration;
} SpriteAnimationFrame;

typedef struct SpriteAnimation SpriteAnimation;

struct SpriteAnimation {
    SpriteAnimation * next;
    GBUInt8 numberOfFrames;
    SpriteAnimationFrame * frames;
};

typedef struct {
    SpriteAnimation * animation;
    GBUInt8 frameIndex;
    GBUInt8 timeUntilNextFrame;
    GBUInt8 bank;
} SpriteAnimationState;

#define spriteAttributesMake(flipX, palette) (((flipX) << 5) | ((palette) << 4))

extern volatile SpriteAttributes * spriteAttributes;

void spritesInit();
void spritesClear();
void spritesWriteToOAM();
void spritesWriteFrame2x2(SpriteFrame2x2 * frames, SpriteAttributes * spriteAttributes, GBUInt8 frameIndex, GBUInt8 bank, GBUInt8 tileOffset, GBUInt8 x, GBUInt8 y, GBBool flipX);

void spritesBeginAnimation(SpriteAnimationState * state, SpriteAnimation * animation, GBUInt8 bank);
void spritesAnimationUpdate(SpriteAnimationState * state);
GBUInt8 spritesAnimationCurrentFrameIndex(SpriteAnimationState * state);

#endif
