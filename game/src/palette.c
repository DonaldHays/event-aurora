#include "palette.h"

// ===
// Public Variables
// ===
GBUInt8 backgroundPalette;
GBUInt8 object0Palette;
GBUInt8 object1Palette;

// ===
// Public API
// ===
void paletteInit() {
    backgroundPalette = gbPaletteMake(gbShadeBlack, gbShadeDarkGray, gbShadeLightGray, gbShadeWhite);
    object0Palette = gbPaletteMake(gbShadeBlack, gbShadeDarkGray, gbShadeWhite, gbShadeWhite);
    object1Palette = gbPaletteMake(gbShadeBlack, gbShadeLightGray, gbShadeWhite, gbShadeWhite);
}

void paletteUpdateGraphics() {
    gbBackgroundPaletteRegister = backgroundPalette;
    gbObject0PaletteRegister = object0Palette;
    gbObject1PaletteRegister = object1Palette;
}
