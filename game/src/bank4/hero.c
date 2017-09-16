#include "hero.h"
#include "../sprites.h"
#include "game.h"

#pragma bank 4

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

void _heroMapEdgeBonk() {
    if(_heroX < 32 * 16) {
        _heroX = 32 * 16;
    }
    
    if(_heroX > (32 + 160 - 16) * 16) {
        _heroX = (32 + 160 - 16) * 16;
    }
}

void _heroWallBonk() {
    GBUInt8 sensorY;
    GBUInt8 sensorX;
    GBUInt8 tileX;
    GBUInt8 tileY;
    GBUInt8 attributes;
    
    sensorY = (_heroY >> 4) - 32;
    tileY = sensorY >> 4;
    sensorX = (_heroX >> 4) - (32 - 4);
    tileX = sensorX >> 4;
    
    attributes = mapAttributes[tileX + tileY * 10];
    if(attributes & 0x01) {
        _heroX = ((((GBUInt16)tileX + 1) << 4) + 32 - 4) << 4;
    }
    
    sensorX = (_heroX >> 4) - (32 - 12);
    tileX = sensorX >> 4;
    attributes = mapAttributes[tileX + tileY * 10];
    if(attributes & 0x01) {
        _heroX = ((((GBUInt16)tileX) << 4) + 32 - 12) << 4;
    }
    
    sensorY += 15;
    if((sensorY >> 4) != tileY) {
        tileY = sensorY >> 4;
        sensorX = (_heroX >> 4) - (32 - 4);
        tileX = sensorX >> 4;
        
        attributes = mapAttributes[tileX + tileY * 10];
        if(attributes & 0x01) {
            _heroX = ((((GBUInt16)tileX + 1) << 4) + 32 - 4) << 4;
        }
        
        sensorX = (_heroX >> 4) - (32 - 12);
        tileX = sensorX >> 4;
        attributes = mapAttributes[tileX + tileY * 10];
        if(attributes & 0x01) {
            _heroX = ((((GBUInt16)tileX) << 4) + 32 - 12) << 4;
        }
    }
}

void _heroFallCheck() {
    GBUInt8 sensorY;
    GBUInt8 sensorX;
    GBUInt8 tileX;
    GBUInt8 tileY;
    GBUInt8 attributes;
    GBBool hasFootStanding;
    
    hasFootStanding = false;
    
    sensorY = (_heroY >> 4) - (32 - 16);
    tileY = sensorY >> 4;
    sensorX = (_heroX >> 4) - (32 - 4);
    tileX = sensorX >> 4;
    
    attributes = mapAttributes[tileX + tileY * 10];
    if(attributes & 0x01) {
        hasFootStanding = true;
    } else {
        sensorX = (_heroX >> 4) - (32 - 11);
        tileX = sensorX >> 4;
        attributes = mapAttributes[tileX + tileY * 10];
        if(attributes & 0x01) {
            hasFootStanding = true;
        }
    }
    
    if(hasFootStanding == false) {
        _heroVelocityY = 0;
        _heroState = heroStateJumping;
    }
}

void _heroHitHeadCheck() {
    GBUInt8 sensorY;
    GBUInt8 sensorX;
    GBUInt8 tileX;
    GBUInt8 tileY;
    GBUInt8 attributes;
    
    if(_heroY < (32 * 16)) {
        _heroY = 32 * 16;
        _heroVelocityY = 0;
    }
    
    if(_heroVelocityY > 0) {
        return;
    }
    
    sensorY = (_heroY >> 4) - (32);
    tileY = sensorY >> 4;
    sensorX = (_heroX >> 4) - (32 - 4);
    tileX = sensorX >> 4;
    
    attributes = mapAttributes[tileX + tileY * 10];
    if(attributes & 0x01) {
        _heroY = ((((GBUInt16)tileY + 1) << 4) + 32) << 4;
        _heroVelocityY = 0;
    } else {
        sensorX = (_heroX >> 4) - (32 - 11);
        tileX = sensorX >> 4;
        attributes = mapAttributes[tileX + tileY * 10];
        if(attributes & 0x01) {
            _heroY = ((((GBUInt16)tileY + 1) << 4) + 32) << 4;
            _heroVelocityY = 0;
        }
    }
}

void _heroStandCheck() {
    GBUInt8 sensorY;
    GBUInt8 sensorX;
    GBUInt8 tileX;
    GBUInt8 tileY;
    GBUInt8 attributes;
    
    if(_heroVelocityY < 0) {
        return;
    }
    
    sensorY = (_heroY >> 4) - (32 - 15);
    tileY = sensorY >> 4;
    sensorX = (_heroX >> 4) - (32 - 4);
    tileX = sensorX >> 4;
    
    attributes = mapAttributes[tileX + tileY * 10];
    if(attributes & 0x01) {
        _heroY = ((((GBUInt16)tileY - 1) << 4) + 32) << 4;
        _heroState = heroStateStanding;
    } else {
        sensorX = (_heroX >> 4) - (32 - 11);
        tileX = sensorX >> 4;
        attributes = mapAttributes[tileX + tileY * 10];
        if(attributes & 0x01) {
            _heroY = ((((GBUInt16)tileY - 1) << 4) + 32) << 4;
            _heroState = heroStateStanding;
        }
    }
}

void _heroUpdateStandingState() {
    if(gbJoypadState & gbJoypadRight) {
        _heroX += 16;
    }
    
    if(gbJoypadState & gbJoypadLeft) {
        _heroX -= 16;
    }
    
    _heroMapEdgeBonk();
    _heroWallBonk();
    
    if(_heroHasReleasedA && (gbJoypadState & gbJoypadA)) {
        _heroJump();
    } else {
        _heroFallCheck();
    }
}

void _heroUpdateJumpingState() {
    if(!(gbJoypadState & gbJoypadA) || _heroVelocityY > 0) {
        _heroIsRisingSlowly = false;
    }
    
    _heroY += _heroVelocityY;
    _heroVelocityY += _heroIsRisingSlowly ? 2 : 4;
    if(_heroVelocityY > 128) {
        _heroVelocityY = 128;
    }
    
    _heroHitHeadCheck();
    _heroStandCheck();
    
    if(gbJoypadState & gbJoypadRight) {
        _heroX += 16;
    }
    
    if(gbJoypadState & gbJoypadLeft) {
        _heroX -= 16;
    }
    
    _heroMapEdgeBonk();
    _heroWallBonk();
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
