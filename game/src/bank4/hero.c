#include "hero.h"
#include "../sprites.h"
#include "game.h"
#include "metamap.h"
#include "../data/gfx_heroTiles.h"

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
GBBool _heroIsFacingLeft;
HeroState _heroState;
SpriteAnimationState _heroAnimationState;

// ===
// Public Variables
// ===
GBUInt8 heroSpawnX;
GBUInt8 heroSpawnY;
GBUInt8 heroSpawnFaceLeft;

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
    if(_heroX < (32 - 8) * 16) {
        _heroX = (32 - 8) * 16;
    }
    
    if(_heroX > (32 + 8 + 160 - 16) * 16) {
        _heroX = (32 + 8 + 160 - 16) * 16;
    }
}

void _heroWallBonk() {
    GBUInt8 sensorY;
    GBUInt8 sensorX;
    GBUInt8 attributes;
    
    // Check against top of hero
    sensorY = (_heroY / 16);
    
    // Top-Left
    sensorX = (_heroX / 16) + 4;
    attributes = gameAttributesAt(sensorX, sensorY);
    if(attributes & 0x01) {
        _heroX = ((sensorX & 0xF0) + 12) * 16;
    }
    
    // Top-Right
    sensorX = (_heroX / 16) + 12;
    attributes = gameAttributesAt(sensorX, sensorY);
    if(attributes & 0x01) {
        _heroX = ((sensorX & 0xF0) - 12) * 16;
    }
    
    // Check against bottom of hero
    sensorY = (_heroY / 16) + 15;
    
    // Bottom-Left
    sensorX = (_heroX / 16) + 4;
    attributes = gameAttributesAt(sensorX, sensorY);
    if(attributes & 0x01) {
        _heroX = ((sensorX & 0xF0) + 12) * 16;
    }
    
    // Bottom-Right
    sensorX = (_heroX / 16) + 12;
    attributes = gameAttributesAt(sensorX, sensorY);
    if(attributes & 0x01) {
        _heroX = ((sensorX & 0xF0) - 12) * 16;
    }
}

void _heroFallCheck() {
    GBUInt8 sensorY;
    GBUInt8 sensorX;
    GBUInt8 attributes;
    GBBool hasFootStanding;
    
    hasFootStanding = false;
    
    sensorY = (_heroY / 16) + 16;
    sensorX = (_heroX / 16) + 4;
    
    attributes = gameAttributesAt(sensorX, sensorY);
    if(attributes & 0x01) {
        hasFootStanding = true;
    } else {
        sensorX = (_heroX / 16) + 11;
        attributes = gameAttributesAt(sensorX, sensorY);
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
    GBUInt8 attributes;
    
    if(_heroY < (24 * 16)) {
        _heroY = 24 * 16;
        _heroVelocityY = 0;
    }
    
    if(_heroVelocityY > 0) {
        return;
    }
    
    sensorY = (_heroY / 16);
    sensorX = (_heroX / 16) + 4;
    
    attributes = gameAttributesAt(sensorX, sensorY);
    if(attributes & 0x01) {
        _heroY = ((sensorY & 0xF0) + 16) * 16;
        _heroVelocityY = 0;
    } else {
        sensorX = (_heroX / 16) + 11;
        attributes = gameAttributesAt(sensorX, sensorY);
        if(attributes & 0x01) {
            _heroY = ((sensorY & 0xF0) + 16) * 16;
            _heroVelocityY = 0;
        }
    }
}

void _heroStandCheck() {
    GBUInt8 sensorY;
    GBUInt8 sensorX;
    GBUInt8 attributes;
    
    if(_heroVelocityY < 0) {
        return;
    }
    
    sensorY = (_heroY / 16) + 16;
    sensorX = (_heroX / 16) + 4;
    
    attributes = gameAttributesAt(sensorX, sensorY);
    if(attributes & 0x01) {
        _heroY = ((sensorY & 0xF0) - 16) * 16;
        _heroState = heroStateStanding;
    } else {
        sensorX = (_heroX / 16) + 11;
        attributes = gameAttributesAt(sensorX, sensorY);
        if(attributes & 0x01) {
            _heroY = ((sensorY & 0xF0) - 16) * 16;
            _heroState = heroStateStanding;
        }
    }
}

void _heroWallJumpCheck() {
    GBUInt8 sensorY;
    GBUInt8 sensorX;
    GBUInt8 attributes;
    
    if((gbJoypadPressedSinceLastUpdate & gbJoypadA) == 0) {
        return;
    }
    
    // Wall-jump requires bottom of sprite to be near wall
    sensorY = (_heroY / 16) + 15;
    
    if(gbJoypadState & gbJoypadRight) {
        sensorX = _heroX / 16;
    } else if(gbJoypadState & gbJoypadLeft) {
        sensorX = (_heroX / 16) + 16;
    } else {
        return;
    }
    
    attributes = gameAttributesAt(sensorX, sensorY);
    if(attributes & 0x01) {
        _heroJump();
    }
}

void _heroTransitionCheck() {
    if((_heroX / 16) < 28) {
        if(canNavigateLeft(metamapX, metamapY)) {
            metamapX--;
            heroSpawnY = (_heroY / 16) - 32;
            heroSpawnX = 144;
            heroSpawnFaceLeft = _heroIsFacingLeft;
            shouldTransitionToNewMap = true;
        }
    }
    
    if((_heroX / 16) > 180) {
        if(canNavigateRight(metamapX, metamapY)) {
            metamapX++;
            heroSpawnY = (_heroY / 16) - 32;
            heroSpawnX = 0;
            heroSpawnFaceLeft = _heroIsFacingLeft;
            shouldTransitionToNewMap = true;
        }
    }
    
    if((_heroY / 16) < 28) {
        if(canNavigateUp(metamapX, metamapY)) {
            metamapY--;
            heroSpawnY = 112;
            heroSpawnX = (_heroX / 16) - 32;
            heroSpawnFaceLeft = _heroIsFacingLeft;
            shouldTransitionToNewMap = true;
        }
    }
    
    if((_heroY / 16) > 148) {
        if(canNavigateDown(metamapX, metamapY)) {
            metamapY++;
            heroSpawnY = 0;
            heroSpawnX = (_heroX / 16) - 32;
            heroSpawnFaceLeft = _heroIsFacingLeft;
            shouldTransitionToNewMap = true;
        }
    }
}

void _heroUpdateStandingState() {
    if(gbJoypadState & gbJoypadRight) {
        _heroIsFacingLeft = false;
        _heroX += 16;
        
        if(_heroAnimationState.animation != &heroTilesAnimation_run) {
            spritesBeginAnimation(&_heroAnimationState, &heroTilesAnimation_run, heroTilesBank);
        }
    } else if(gbJoypadState & gbJoypadLeft) {
        _heroIsFacingLeft = true;
        _heroX -= 16;
        
        if(_heroAnimationState.animation != &heroTilesAnimation_run) {
            spritesBeginAnimation(&_heroAnimationState, &heroTilesAnimation_run, heroTilesBank);
        }
    } else if(_heroAnimationState.animation != &heroTilesAnimation_idle) {
        spritesBeginAnimation(&_heroAnimationState, &heroTilesAnimation_idle, heroTilesBank);
    }
    
    _heroMapEdgeBonk();
    _heroWallBonk();
    
    if(_heroHasReleasedA && (gbJoypadState & gbJoypadA)) {
        _heroJump();
    } else {
        _heroFallCheck();
    }
    
    _heroTransitionCheck();
}

void _heroUpdateJumpingState() {
    _heroWallJumpCheck();
    
    if(!(gbJoypadState & gbJoypadA) || _heroVelocityY > 0) {
        _heroIsRisingSlowly = false;
    }
    
    _heroY += _heroVelocityY;
    _heroVelocityY += _heroIsRisingSlowly ? 2 : 4;
    if(_heroVelocityY > 96) {
        _heroVelocityY = 96;
    }
    
    _heroHitHeadCheck();
    _heroStandCheck();
    
    if(gbJoypadState & gbJoypadRight) {
        _heroIsFacingLeft = false;
        _heroX += 16;
    }
    
    if(gbJoypadState & gbJoypadLeft) {
        _heroIsFacingLeft = true;
        _heroX -= 16;
    }
    
    _heroMapEdgeBonk();
    _heroWallBonk();
    
    _heroTransitionCheck();
}

void _heroUpdateSpriteAttributes() {
    GBUInt8 heroX, heroY;
    
    heroX = (_heroX >> 4) - 24;
    heroY = (_heroY >> 4);
    
    spritesAnimationUpdate(&_heroAnimationState);
    spritesWriteFrame2x2(&heroTilesFrames[spritesAnimationCurrentFrameIndex(&_heroAnimationState)], &spriteAttributes[0], 0, heroTilesBank, 0, heroX, heroY, spriteAttributesMake(_heroIsFacingLeft, 0));
}

// ===
// Public API
// ===
void heroSpawn() {
    spritesBeginAnimation(&_heroAnimationState, &heroTilesAnimation_idle, heroTilesBank);
    _heroX = (32 + (GBUInt16)heroSpawnX) * 16;
    _heroY = (32 + (GBUInt16)heroSpawnY) * 16;
    if(heroSpawnY != 0) {
        _heroVelocityY = 0;
        _heroState = heroStateStanding;
    }
    _heroIsRisingSlowly = false;
    _heroHasReleasedA = false;
    _heroIsFacingLeft = heroSpawnFaceLeft;
    
    if(heroSpawnY > 104) {
        _heroJump();
        _heroVelocityY = -40;
    }
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
