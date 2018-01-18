#ifndef hero_bullets_h
#define hero_bullets_h

#include <gb/gb.h>

typedef enum {
    heroBulletsDirectionLeft,
    heroBulletsDirectionRight
} HeroBulletsDirection;

void heroBulletsReset();
void heroBulletsUpdate();
void heroBulletsSpawn(GBUInt8 x, GBUInt8 y, HeroBulletsDirection direction);

#endif
