#include "heroBullets.h"
#include "../sprites.h"
#include "game.h"
#include "particles.h"
#include "../data/music_jumpLandSound.h"

#pragma bank 4

// ===
// Public API
// ===
void heroBulletsReset() {
    
}

void heroBulletsUpdate() {
    
}

void heroBulletsSpawn(GBUInt8 x, GBUInt8 y, HeroBulletsDirection direction) {
    switch(direction) {
    case heroBulletsDirectionLeft:
        particlesSpawnSmoke(x, y - 3, -1);
        break;
    case heroBulletsDirectionRight:
    particlesSpawnSmoke(x, y - 3, 1);
        break;
    default:
        gbFatalError("unrecognized direction");
    }
    
    audioPlayComposition(&jumpLandSound, jumpLandSoundBank, audioLayerSound, 5);
}
