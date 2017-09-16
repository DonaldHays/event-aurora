#include "game.h"
#include "../data/gfx_castleTiles.h"
#include "../data/gfx_heroTiles.h"
#include "../data/meta_castleMetatiles.h"
#include "../data/map_sample.h"
#include "../memory.h"
#include "../palette.h"
#include "../sprites.h"
#include "hero.h"

#pragma bank 4

// ===
// Private Variables
// ===
GBUInt8 _mapMetatiles[80];
MetatileIndices _metatileIndices[256];
MetatileAttributes _metatileAttributes[256];

// ===
// Public Variables
// ===
GBUInt8 mapAttributes[80];

// ===
// Public API
// ===
void gameInit() {
    
}

void gameWake() {
    GBUInt8 x, y, index;
    GBUInt8 metatileIndex;
    
    gbLCDDisable(); {
        backgroundPalette = gbPaletteMake(gbShadeBlack, gbShadeDarkGray, gbShadeLightGray, gbShadeWhite);
        object0Palette = gbPaletteMake(gbShadeBlack, gbShadeLightGray, gbShadeWhite, gbShadeWhite);
        object1Palette = gbPaletteMake(gbShadeBlack, gbShadeDarkGray, gbShadeWhite, gbShadeWhite);
        
        memoryCopyBanked(gbTileMemory, heroTiles, heroTilesLength, heroTilesBank);
        
        memoryCopyBanked(gbTileMemory + 256 * 16, castleTiles, castleTilesLength, castleTilesBank);
        memoryCopyBanked(_mapMetatiles, sampleMapIndices, 80, sampleMapBank);
        memoryCopyBanked(_metatileIndices, castleMetatilesIndices, castleMetatilesCount * sizeof(MetatileIndices), castleMetatilesBank);
        memoryCopyBanked(_metatileAttributes, castleMetatilesAttributes, castleMetatilesCount * sizeof(MetatileAttributes), castleMetatilesBank);
        
        memorySet(gbTileMap0, 0x7F, gbTileMapWidth * 2);
        
        x = 0;
        y = 0;
        for(index = 0; index < 80; index++) {
            metatileIndex = _mapMetatiles[index];
            
            mapAttributes[index] = _metatileAttributes[metatileIndex];
            
            gbTileMap0[(x * 2) + (y * 2 * gbTileMapWidth) + gbTileMapWidth * 2] = _metatileIndices[metatileIndex][0];
            gbTileMap0[(x * 2) + 1 + (y * 2 * gbTileMapWidth) + gbTileMapWidth * 2] = _metatileIndices[metatileIndex][1];
            gbTileMap0[(x * 2) + (y * 2 * gbTileMapWidth) + gbTileMapWidth + gbTileMapWidth * 2] = _metatileIndices[metatileIndex][2];
            gbTileMap0[(x * 2) + 1 + (y * 2 * gbTileMapWidth) + gbTileMapWidth + gbTileMapWidth * 2] = _metatileIndices[metatileIndex][3];
            
            x++;
            if(x == 10) {
                x = 0;
                y++;
            }
        }
        
        heroSpawn();
    } gbLCDEnable();
}

void gameSuspend() {
    
}

void gameUpdate() {
    heroUpdate();
}

void gameUpdateGraphics() {
    // gbLogUInt8(gbLCDYCoordinateRegister);
}
