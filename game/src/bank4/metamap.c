#include "metamap.h"

#include "../data/map_sample.h"
#include "../data/map_walljump.h"
#include "../data/map_testright.h"

#pragma bank 4

// ===
// Public Constant Data
// ===

const MetamapTile metamapTiles[metamapWidth * metamapHeight] = {
    { walljumpMapIndices, walljumpMapBank, 0 },
    { sampleMapIndices, sampleMapBank, 0 },
    { testrightMapIndices, testrightMapBank, 0 },
};

// ===
// Public API
// ===

MetamapTile const * metamapTileAt(GBUInt8 x, GBUInt8 y) {
    return &metamapTiles[x + y * metamapWidth];
}

GBBool canNavigateLeft(GBUInt8 x, GBUInt8 y) {
    if(x == 0) {
        return false;
    }
    
    if(metamapTileAt(x - 1, y)->indices == null) {
        return false;
    }
    
    return true;
}

GBBool canNavigateRight(GBUInt8 x, GBUInt8 y) {
    if(x == (metamapWidth - 1)) {
        return false;
    }
    
    if(metamapTileAt(x + 1, y)->indices == null) {
        return false;
    }
    
    return true;
}

GBBool canNavigateDown(GBUInt8 x, GBUInt8 y) {
    if(y == (metamapHeight - 1)) {
        return false;
    }
    
    if(metamapTileAt(x, y + 1)->indices == null) {
        return false;
    }
    
    return true;
}

GBBool canNavigateUp(GBUInt8 x, GBUInt8 y) {
    if(y == 0) {
        return false;
    }
    
    if(metamapTileAt(x, y - 1)->indices == null) {
        return false;
    }
    
    return true;
}
