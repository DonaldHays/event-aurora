#include "heroBullets.h"
#include "../sprites.h"
#include "game.h"
#include "particles.h"
#include "../data/music_bulletFireSound.h"

#pragma bank 4

// ===
// Private Defines
// ===
typedef struct {
    GBUInt8 x;
    GBUInt8 y;
    GBUInt8 flags;
    GBUInt8 padding;
} PlayerBullet;

#define heroBulletsFlagActive 0x01
#define heroBulletsFlagLeft 0x02
#define heroBulletsFlagRight 0x04

#define heroBulletsSpriteOAMStartIndex 33
#define heroBulletsMax 3

// ===
// Private Variables
// ===
PlayerBullet _playerBullets[heroBulletsMax];

// ===
// Public API
// ===
void heroBulletsReset() {
    GBUInt8 index;
    
    for(index = 0; index != heroBulletsMax; index++) {
        _playerBullets[index].flags = 0;
    }
}

void heroBulletsUpdate() {
    GBUInt8 index;
    PlayerBullet * bullet;
    GBUInt8 * attributes;
    
    bullet = _playerBullets;
    attributes = (GBUInt8 *)(&spriteAttributes[heroBulletsSpriteOAMStartIndex]);
    
    for(index = 0; index != heroBulletsMax; index++, bullet++) {
        if((bullet->flags & heroBulletsFlagActive) == 0 || (bullet->x < 8) || (bullet->x > 160)) {
            bullet->flags = 0;
            
            *(attributes) = 0;
            attributes = (GBUInt8 *)(&spriteAttributes[heroBulletsSpriteOAMStartIndex + index + 1]);
            
            continue;
        }
        
        if(bullet->flags & heroBulletsFlagRight) {
            bullet->x += 4;
        } else {
            bullet->x -= 4;
        }
        
        *(attributes++) = bullet->y - 3;
        *(attributes++) = bullet->x;
        *(attributes++) = 0xF3;
        *(attributes++) = spriteAttributesMake((bullet->flags & heroBulletsFlagLeft) == heroBulletsFlagLeft, 0, 0);
    }
}

void heroBulletsSpawn(GBUInt8 x, GBUInt8 y, HeroBulletsDirection direction) {
    GBUInt8 flags;
    GBUInt8 seekIndex, foundIndex;
    
    flags = heroBulletsFlagActive;
    
    foundIndex = heroBulletsMax + 1;
    for(seekIndex = 0; seekIndex != heroBulletsMax; seekIndex++) {
        if(_playerBullets[seekIndex].flags == 0) {
            foundIndex = seekIndex;
            break;
        }
    }
    
    if(foundIndex == heroBulletsMax + 1) {
        return;
    }
    
    switch(direction) {
    case heroBulletsDirectionLeft:
        flags |= heroBulletsFlagLeft;
        // particlesSpawnSmoke(x, y - 3, -1);
        break;
    case heroBulletsDirectionRight:
        flags |= heroBulletsFlagRight;
        // particlesSpawnSmoke(x, y - 3, 1);
        break;
    default:
        gbFatalError("unrecognized direction");
    }
    
    _playerBullets[foundIndex].x = x;
    _playerBullets[foundIndex].y = y;
    _playerBullets[foundIndex].flags = flags;
    
    audioPlayComposition(&bulletFireSound, bulletFireSoundBank, audioLayerSound, 5);
}
