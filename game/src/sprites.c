#include "sprites.h"
#include "memory.h"
#include "banks.h"

// ===
// Public Variables
// ===

volatile SpriteAttributes * spriteAttributes;
GBBool spritesShouldSuppressOAMTransfer;

// ===
// Public API
// ===
void spritesInit() {
    spriteAttributes = (SpriteAttributes *)gbShadowOAMMemory;
    spritesShouldSuppressOAMTransfer = false;
    
    spritesClear();
}

void spritesClear() {
    memorySet(spriteAttributes, 0, sizeof(SpriteAttributes) * gbSpritesCount);
}

void spritesWriteToOAM() {
    if(spritesShouldSuppressOAMTransfer) {
        return;
    }
    
    __asm
    call #0xFF80
    __endasm;
}

void spritesWriteFrame2x2(SpriteFrame2x2 * frames, SpriteAttributes * spriteAttributes, GBUInt8 frameIndex, GBUInt8 bank, GBUInt8 tileOffset, GBUInt8 x, GBUInt8 y, GBUInt8 attributes) {
    SpriteFrame2x2 * frame;
    GBUInt8 * attributePointer;
    GBUInt8 topLeft, topRight, bottomLeft, bottomRight;
    GBUInt8 currentBank = banksROMGet();
    
    frame = &frames[frameIndex];
    attributePointer = (GBUInt8 *)spriteAttributes;
    
    banksROMSet(bank); {
        if(attributes & 0x20) {
            topLeft = frame->topRightIndex + tileOffset;
            topRight = frame->topLeftIndex + tileOffset;
            bottomLeft = frame->bottomRightIndex + tileOffset;
            bottomRight = frame->bottomLeftIndex + tileOffset;
        } else {
            topLeft = frame->topLeftIndex + tileOffset;
            topRight = frame->topRightIndex + tileOffset;
            bottomLeft = frame->bottomLeftIndex + tileOffset;
            bottomRight = frame->bottomRightIndex + tileOffset;
        }
        
        *(attributePointer++) = y;
        *(attributePointer++) = x;
        *(attributePointer++) = topLeft;
        *(attributePointer++) = attributes;
        
        *(attributePointer++) = y;
        *(attributePointer++) = x + 8;
        *(attributePointer++) = topRight;
        *(attributePointer++) = attributes;
        
        *(attributePointer++) = y + 8;
        *(attributePointer++) = x;
        *(attributePointer++) = bottomLeft;
        *(attributePointer++) = attributes;
        
        *(attributePointer++) = y + 8;
        *(attributePointer++) = x + 8;
        *(attributePointer++) = bottomRight;
        *(attributePointer++) = attributes;
    } banksROMSet(currentBank);
}

void spritesBeginAnimation(SpriteAnimationState * state, SpriteAnimation * animation, GBUInt8 bank) {
    GBUInt8 currentBank;
    
    currentBank = banksROMGet();
    
    banksROMSet(bank); {
        state->animation = animation;
        state->frameIndex = 0;
        state->timeUntilNextFrame = animation->frames[0].duration;
        state->bank = bank;
    } banksROMSet(currentBank);
}

void spritesAnimationUpdate(SpriteAnimationState * state) {
    GBUInt8 currentBank;
    
    if(state->animation == null) {
        return;
    }
    
    currentBank = banksROMGet();
    
    banksROMSet(state->bank); {
        if(state->timeUntilNextFrame != 0) {
            state->timeUntilNextFrame -= 1;
            
            if(state->timeUntilNextFrame == 0) {
                if(state->frameIndex + 1 != state->animation->numberOfFrames) {
                    state->frameIndex += 1;
                    state->timeUntilNextFrame = state->animation->frames[state->frameIndex].duration;
                } else if(state->animation->next != null) {
                    state->animation = state->animation->next;
                    state->frameIndex = 0;
                    state->timeUntilNextFrame = state->animation->frames[0].duration;
                }
            }
        }
    } banksROMSet(currentBank);
}

GBUInt8 spritesAnimationCurrentFrameIndex(SpriteAnimationState * state) {
    GBUInt8 currentBank;
    GBUInt8 frameIndex;
    
    currentBank = banksROMGet();
    
    banksROMSet(state->bank); {
        frameIndex = state->animation->frames[state->frameIndex].frameIndex;
    } banksROMSet(currentBank);
    
    return frameIndex;
}
