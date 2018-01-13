#ifndef particles_h
#define particles_h

#include <gb/gb.h>

void particlesReset();
void particlesUpdate();
void particlesSpawnSmoke(GBUInt8 x, GBUInt8 y, GBInt8 velocityX);

#endif