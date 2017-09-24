#ifndef game_h
#define game_h

#include <gb/gb.h>

#define gameMapAttributesWidth 16
#define gameMapAttributesHeight 12
#define gameMapAttributesLength (gameMapAttributesWidth * gameMapAttributesHeight)

extern GBUInt8 mapAttributes[gameMapAttributesLength];
extern GBUInt8 metamapX;
extern GBUInt8 metamapY;

void gameInit();
void gameWake();
void gameSuspend();
void gameUpdate();
void gameUpdateGraphics();
GBUInt8 gameAttributesAt(GBUInt8 x, GBUInt8 y);

#endif
