#ifndef metamap_h
#define metamap_h

#include <gb/gb.h>

typedef struct {
    GBUInt8 * indices;
    GBUInt8 * objects;
    GBUInt8 bank;
    GBUInt8 padding0;
    GBUInt8 padding1;
    GBUInt8 padding2;
} MetamapTile;

#define metamapWidth 3
#define metamapHeight 2

extern const MetamapTile metamapTiles[metamapWidth * metamapHeight];

MetamapTile const * metamapTileAt(GBUInt8 x, GBUInt8 y);

GBBool canNavigateLeft(GBUInt8 x, GBUInt8 y);
GBBool canNavigateRight(GBUInt8 x, GBUInt8 y);
GBBool canNavigateDown(GBUInt8 x, GBUInt8 y);
GBBool canNavigateUp(GBUInt8 x, GBUInt8 y);

#endif