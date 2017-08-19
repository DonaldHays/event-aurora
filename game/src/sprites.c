#include "sprites.h"
#include "memory.h"

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
