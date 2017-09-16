#ifndef game_h
#define game_h

#include <gb/gb.h>

extern GBUInt8 mapAttributes[80];

void gameInit();
void gameWake();
void gameSuspend();
void gameUpdate();
void gameUpdateGraphics();

#endif
