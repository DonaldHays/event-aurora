#include "game.h"
#include "../data/gfx_castleTiles.h"
#include "../data/gfx_heroTiles.h"
#include "../data/gfx_particleTiles.h"
#include "../data/meta_castleMetatiles.h"
#include "../memory.h"
#include "../palette.h"
#include "../sprites.h"
#include "../audio.h"
#include "metamap.h"
#include "hero.h"
#include "heroBullets.h"
#include "particles.h"

#pragma bank 4

// ===
// Types
// ===
typedef enum {
    gameLoadingStageGameplay,
    gameLoadingStageCopyingHero,
    gameLoadingStageCopyingTiles,
    gameLoadingStageWritingBG,
    gameLoadingStageCopyingParticles
} GameLoadingStage;

// ===
// Private Defines
// ===
#define gameTileMapStagingLength (gbTileMapWidth * 18)
#define gameFadeStageIn 0x04
#define gameFadeStageOut 0x08
#define gameFadeStageMask 0x03
#define gameFadeStageTimerDuration 1

// ===
// Private Variables
// ===
GBUInt8 _mapMetatiles[80];
GBUInt8 _mapObjects[80];
GBUInt8 _tileMapStaging[gameTileMapStagingLength];
MetatileIndices _metatileIndices[256];
MetatileAttributes _metatileAttributes[256];
GBUInt8 const * _gameLoadedTiles;

GameLoadingStage _gameLoadingStage;
GBUInt16 _gameLoadingOffset;
GBUInt8 _gameFade;
GBUInt8 _gameFadeStageTimer;

// ===
// Public Variables
// ===
GBUInt8 mapAttributes[gameMapAttributesLength];
GBUInt8 metamapX;
GBUInt8 metamapY;
GBBool shouldTransitionToNewMap;

// ===
// Public API
// ===
void gameInit() {
    metamapX = 1;
    metamapY = 1;
    heroShouldSpawnAtSpawnPoint = true;
    heroSpawnFaceLeft = false;
    shouldTransitionToNewMap = false;
    _gameFade = 0;
    _gameFadeStageTimer = gameFadeStageTimerDuration;
    _gameLoadedTiles = null;
    _gameLoadingStage = gameLoadingStageCopyingHero;
    _gameLoadingOffset = 0;
    memorySet(_tileMapStaging, 0x7F, gameTileMapStagingLength);
    memorySet(mapAttributes, 0, gameMapAttributesLength);
}

void gameWake() {
    GBUInt8 lcdYTrackerOld, lcdYTrackerNew;
    GBUInt8 x, y, index;
    GBUInt8 metatileIndex;
    GBUInt8 attributesIndex;
    GBUInt8 objectIndex;
    MetamapTile const * metamapTile;
    
    lcdYTrackerOld = gbLCDYCoordinateRegister;
    
    metamapTile = metamapTileAt(metamapX, metamapY);
    
    gbBackgroundScrollXRegister = 0;
    
    spritesClear();
    
    memoryCopyBanked(_mapMetatiles, metamapTile->indices, 80, metamapTile->bank);
    memoryCopyBanked(_mapObjects, metamapTile->objects, 80, metamapTile->bank);
    memoryCopyBanked(_metatileIndices, castleMetatilesIndices, castleMetatilesCount * sizeof(MetatileIndices), castleMetatilesBank);
    memoryCopyBanked(_metatileAttributes, castleMetatilesAttributes, castleMetatilesCount * sizeof(MetatileAttributes), castleMetatilesBank);
    
    lcdYTrackerNew = gbLCDYCoordinateRegister;
    if(lcdYTrackerNew < lcdYTrackerOld) {
        audioUpdate();
    }
    lcdYTrackerOld = lcdYTrackerNew;
    
    _gameLoadingStage = gameLoadingStageCopyingHero;
    _gameLoadingOffset = 0;
    
    spritesShouldSuppressOAMTransfer = true;
    
    memorySet(_tileMapStaging, 0x7F, gbTileMapWidth * 2);
    
    lcdYTrackerNew = gbLCDYCoordinateRegister;
    if(lcdYTrackerNew < lcdYTrackerOld) {
        audioUpdate();
    }
    lcdYTrackerOld = lcdYTrackerNew;
    
    x = 0;
    y = 0;
    for(index = 0; index != 80; index++) {
        metatileIndex = _mapMetatiles[index];
        objectIndex = _mapObjects[index];
        attributesIndex = (x + 2) + (y + 2) * gameMapAttributesWidth;
        
        mapAttributes[attributesIndex] = _metatileAttributes[metatileIndex];
        
        _tileMapStaging[(x * 2) + (y * 2 * gbTileMapWidth) + gbTileMapWidth * 2] = _metatileIndices[metatileIndex][0];
        _tileMapStaging[(x * 2) + 1 + (y * 2 * gbTileMapWidth) + gbTileMapWidth * 2] = _metatileIndices[metatileIndex][1];
        _tileMapStaging[(x * 2) + (y * 2 * gbTileMapWidth) + gbTileMapWidth + gbTileMapWidth * 2] = _metatileIndices[metatileIndex][2];
        _tileMapStaging[(x * 2) + 1 + (y * 2 * gbTileMapWidth) + gbTileMapWidth + gbTileMapWidth * 2] = _metatileIndices[metatileIndex][3];
        
        if(objectIndex == 1 && heroShouldSpawnAtSpawnPoint) {
            heroShouldSpawnAtSpawnPoint = false;
            heroSpawnX = x * 16;
            heroSpawnY = y * 16;
        }
        
        x++;
        if(x == 10) {
            x = 0;
            y++;
            
            lcdYTrackerNew = gbLCDYCoordinateRegister;
            if(lcdYTrackerNew < lcdYTrackerOld) {
                audioUpdate();
            }
            lcdYTrackerOld = lcdYTrackerNew;
        }
    }
    
    particlesReset();
    heroBulletsReset();
    heroSpawn();
    
    _gameFade = gameFadeStageIn;
    _gameFadeStageTimer = gameFadeStageTimerDuration;
}

void gameSuspend() {
    
}

void gameUpdate() {
    backgroundPalette = gbPaletteMake(gbShadeWhite, gbShadeWhite, gbShadeWhite, gbShadeWhite);
    object0Palette = gbPaletteMake(gbShadeWhite, gbShadeWhite, gbShadeWhite, gbShadeWhite);
    object1Palette = gbPaletteMake(gbShadeWhite, gbShadeWhite, gbShadeWhite, gbShadeWhite);
    
    if(shouldTransitionToNewMap && _gameFade == (gameFadeStageOut | 0x02)) {
        gameWake();
        shouldTransitionToNewMap = false;
    } else if(_gameLoadingStage == gameLoadingStageGameplay) {
        if(shouldTransitionToNewMap && _gameFade == 0) {
            _gameFade = gameFadeStageOut;
            _gameFadeStageTimer = gameFadeStageTimerDuration;
        }
        
        if(_gameFade & (gameFadeStageIn | gameFadeStageOut)) {
            switch(_gameFade & gameFadeStageMask) {
            case 0:
                if(_gameFade & gameFadeStageIn) {
                    backgroundPalette = gbPaletteMake(gbShadeLightGray, gbShadeWhite, gbShadeWhite, gbShadeWhite);
                    object0Palette = gbPaletteMake(gbShadeLightGray, gbShadeWhite, gbShadeWhite, gbShadeWhite);
                    object1Palette = gbPaletteMake(gbShadeLightGray, gbShadeWhite, gbShadeWhite, gbShadeWhite);
                } else {
                    backgroundPalette = gbPaletteMake(gbShadeDarkGray, gbShadeLightGray, gbShadeWhite, gbShadeWhite);
                    object0Palette = gbPaletteMake(gbShadeDarkGray, gbShadeWhite, gbShadeWhite, gbShadeWhite);
                    object1Palette = gbPaletteMake(gbShadeDarkGray, gbShadeLightGray, gbShadeWhite, gbShadeWhite);
                }
                break;
            case 1:
                if(_gameFade & gameFadeStageIn) {
                    backgroundPalette = gbPaletteMake(gbShadeDarkGray, gbShadeLightGray, gbShadeWhite, gbShadeWhite);
                    object0Palette = gbPaletteMake(gbShadeDarkGray, gbShadeWhite, gbShadeWhite, gbShadeWhite);
                    object1Palette = gbPaletteMake(gbShadeDarkGray, gbShadeLightGray, gbShadeWhite, gbShadeWhite);
                } else {
                    backgroundPalette = gbPaletteMake(gbShadeLightGray, gbShadeWhite, gbShadeWhite, gbShadeWhite);
                    object0Palette = gbPaletteMake(gbShadeLightGray, gbShadeWhite, gbShadeWhite, gbShadeWhite);
                    object1Palette = gbPaletteMake(gbShadeLightGray, gbShadeWhite, gbShadeWhite, gbShadeWhite);
                }
                break;
            }
            
            _gameFadeStageTimer--;
            if(_gameFadeStageTimer == 0) {
                _gameFadeStageTimer = gameFadeStageTimerDuration;
                _gameFade++;
                if((_gameFade & gameFadeStageMask) == 2 && (_gameFade & gameFadeStageOut) == 0) {
                    _gameFade = 0;
                }
            }
        } else {
            backgroundPalette = gbPaletteMake(gbShadeBlack, gbShadeDarkGray, gbShadeLightGray, gbShadeWhite);
            object0Palette = gbPaletteMake(gbShadeBlack, gbShadeLightGray, gbShadeWhite, gbShadeWhite);
            object1Palette = gbPaletteMake(gbShadeBlack, gbShadeDarkGray, gbShadeWhite, gbShadeWhite);
        }
        
        if(_gameFade & gameFadeStageOut) {
            return;
        }
        
        particlesUpdate();
        heroBulletsUpdate();
        heroUpdate();
    }
}

void gameUpdateGraphics() {
    switch(_gameLoadingStage) {
    case gameLoadingStageGameplay:
        break;
    case gameLoadingStageCopyingHero:
        memoryCopy64Banked(gbTileMemory + _gameLoadingOffset, heroTiles + _gameLoadingOffset, heroTilesBank);
        _gameLoadingOffset += 64;
        if(_gameLoadingOffset >= heroTilesLength) {
            _gameLoadingOffset = 0;
            if(_gameLoadedTiles != castleTiles) {
                _gameLoadedTiles = castleTiles;
                _gameLoadingStage = gameLoadingStageCopyingTiles;
            } else {
                _gameLoadingStage = gameLoadingStageWritingBG;
            }
        }
        break;
    case gameLoadingStageCopyingTiles:
        memoryCopy64Banked(gbTileMemory + 256 * 16 + _gameLoadingOffset, castleTiles + _gameLoadingOffset, castleTilesBank);
        _gameLoadingOffset += 64;
        if(_gameLoadingOffset >= castleTilesLength) {
            _gameLoadingOffset = 0;
            _gameLoadingStage = gameLoadingStageWritingBG;
        }
        break;
    case gameLoadingStageWritingBG:
        memoryCopy64(gbTileMap0 + _gameLoadingOffset, _tileMapStaging + _gameLoadingOffset);
        _gameLoadingOffset += 64;
        if(_gameLoadingOffset >= gameTileMapStagingLength) {
            _gameLoadingOffset = 0;
            _gameLoadingStage = gameLoadingStageCopyingParticles;
            _gameFade = gameFadeStageIn;
        }
        break;
    case gameLoadingStageCopyingParticles:
        memoryCopy64Banked(gbTileMemory + 240 * 16 + _gameLoadingOffset, particleTiles + _gameLoadingOffset, particleTilesBank);
        _gameLoadingOffset += 64;
        if(_gameLoadingOffset >= particleTilesLength) {
            _gameLoadingOffset = 0;
            _gameLoadingStage = gameLoadingStageGameplay;
            spritesShouldSuppressOAMTransfer = false;
        }
        break;
    }
}

GBUInt8 gameAttributesAt(GBUInt8 x, GBUInt8 y) {
    return mapAttributes[(x / 16) + (y / 16) * gameMapAttributesWidth];
}
