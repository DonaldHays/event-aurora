#ifndef palette_h
#define palette_h

#include <gb/gb.h>

extern GBUInt8 backgroundPalette;
extern GBUInt8 object0Palette;
extern GBUInt8 object1Palette;

void paletteInit();
void paletteUpdateGraphics();

#endif
