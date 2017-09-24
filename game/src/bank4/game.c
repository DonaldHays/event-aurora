#include "game.h"
#include "../data/gfx_castleTiles.h"
#include "../data/gfx_heroTiles.h"
#include "../data/meta_castleMetatiles.h"
#include "../data/map_sample.h"
#include "../data/map_walljump.h"
#include "../memory.h"
#include "../palette.h"
#include "../sprites.h"
#include "metamap.h"
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
GBUInt8 mapAttributes[gameMapAttributesLength];
GBUInt8 metamapX;
GBUInt8 metamapY;

// ===
// Public API
// ===
void gameInit() {
    metamapX = 1;
    metamapY = 0;
    memorySet(mapAttributes, 0, gameMapAttributesLength);
}

void gameWake() {
    GBUInt8 x, y, index;
    GBUInt8 metatileIndex;
    GBUInt8 attributesIndex;
    MetamapTile const * metamapTile;
    
    gbLCDDisable(); {
        metamapTile = metamapTileAt(metamapX, metamapY);
        
        backgroundPalette = gbPaletteMake(gbShadeBlack, gbShadeDarkGray, gbShadeLightGray, gbShadeWhite);
        object0Palette = gbPaletteMake(gbShadeBlack, gbShadeLightGray, gbShadeWhite, gbShadeWhite);
        object1Palette = gbPaletteMake(gbShadeBlack, gbShadeDarkGray, gbShadeWhite, gbShadeWhite);
        
        memoryCopyBanked(gbTileMemory, heroTiles, heroTilesLength, heroTilesBank);
        
        memoryCopyBanked(gbTileMemory + 256 * 16, castleTiles, castleTilesLength, castleTilesBank);
        memoryCopyBanked(_mapMetatiles, metamapTile->indices, 80, metamapTile->bank);
        memoryCopyBanked(_metatileIndices, castleMetatilesIndices, castleMetatilesCount * sizeof(MetatileIndices), castleMetatilesBank);
        memoryCopyBanked(_metatileAttributes, castleMetatilesAttributes, castleMetatilesCount * sizeof(MetatileAttributes), castleMetatilesBank);
        
        memorySet(gbTileMap0, 0x7F, gbTileMapWidth * 2);
        
        x = 0;
        y = 0;
        for(index = 0; index < 80; index++) {
            metatileIndex = _mapMetatiles[index];
            attributesIndex = (x + 2) + (y + 2) * gameMapAttributesWidth;
            
            mapAttributes[attributesIndex] = _metatileAttributes[metatileIndex];
            
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

GBUInt8 gameAttributesAt(GBUInt8 x, GBUInt8 y) {
    return mapAttributes[(x / 16) + (y / 16) * gameMapAttributesWidth];
}
