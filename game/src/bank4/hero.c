#include "hero.h"
#include "../sprites.h"

// ===
// Types
// ===
typedef enum {
    heroStateStanding,
    heroStateJumping
} HeroState;

// ===
// Private Variables
// ===
GBUInt16 _heroX;
GBUInt16 _heroY;
GBInt16 _heroVelocityY;
GBBool _heroIsRisingSlowly;
GBBool _heroHasReleasedA;
HeroState _heroState;

// ===
// Private API
// ===
void _heroJump() {
    _heroState = heroStateJumping;
    _heroVelocityY = -48;
    _heroIsRisingSlowly = true;
    _heroHasReleasedA = false;
}

void _heroUpdateStandingState() {
    if(gbJoypadState & gbJoypadRight) {
        _heroX += 16;
    }
    
    if(gbJoypadState & gbJoypadLeft) {
        _heroX -= 16;
    }
    
    if(_heroHasReleasedA && (gbJoypadState & gbJoypadA)) {
        _heroJump();
    }
}

void _heroUpdateJumpingState() {
    if(gbJoypadState & gbJoypadRight) {
        _heroX += 16;
    }
    
    if(gbJoypadState & gbJoypadLeft) {
        _heroX -= 16;
    }
    
    if(!(gbJoypadState & gbJoypadA) || _heroVelocityY > 0) {
        _heroIsRisingSlowly = false;
    }
    
    _heroY += _heroVelocityY;
    _heroVelocityY += _heroIsRisingSlowly ? 2 : 4;
    if(_heroVelocityY > 128) {
        _heroVelocityY = 128;
    }
    
    if(_heroY >= 2048) {
        _heroY = 2048;
        _heroState = heroStateStanding;
    }
}

void _heroUpdateSpriteAttributes() {
    GBUInt8 heroX, heroY;
    
    heroX = (_heroX >> 4) - 24;
    heroY = (_heroY >> 4);
    
    spriteAttributes[0].x = heroX;
    spriteAttributes[0].y = heroY;
    spriteAttributes[0].tileIndex = 0;
    spriteAttributes[0].attributes = 0;
    
    spriteAttributes[1].x = heroX + 8;
    spriteAttributes[1].y = heroY;
    spriteAttributes[1].tileIndex = 1;
    spriteAttributes[1].attributes = 0;
    
    spriteAttributes[2].x = heroX;
    spriteAttributes[2].y = heroY + 8;
    spriteAttributes[2].tileIndex = 2;
    spriteAttributes[2].attributes = 0;
    
    spriteAttributes[3].x = heroX + 8;
    spriteAttributes[3].y = heroY + 8;
    spriteAttributes[3].tileIndex = 3;
    spriteAttributes[3].attributes = 0;
}

// ===
// Public API
// ===
void heroSpawn() {
    _heroX = (32 + 16) * 16;
    _heroY = (32 + 64) * 16;
    _heroState = heroStateStanding;
    _heroHasReleasedA = false;
}

void heroUpdate() {
    if(!(gbJoypadState & gbJoypadA)) {
        _heroHasReleasedA = true;
    }
    
    switch(_heroState) {
    case heroStateStanding:
        _heroUpdateStandingState();
        break;
    case heroStateJumping:
        _heroUpdateJumpingState();
        break;
    default:
        gbFatalError("unrecognized state");
    }
    
    _heroUpdateSpriteAttributes();
}
