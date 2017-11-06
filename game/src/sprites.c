#include "sprites.h"
#include "memory.h"
#include "banks.h"

// ===
// Public Variables
// ===

volatile SpriteAttributes * spriteAttributes;

// ===
// Public API
// ===
void spritesInit() {
    spriteAttributes = (SpriteAttributes *)gbShadowOAMMemory;
    
    spritesClear();
}

void spritesClear() {
    memorySet(spriteAttributes, 0, sizeof(SpriteAttributes) * gbSpritesCount);
}

void spritesWriteToOAM() {
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
