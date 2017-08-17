#include "mainMenu.h"
#include "../memory.h"
#include "../data/gfx_titleTiles.h"

#pragma bank 1

// ===
// Public API
// ===
void mainMenuInit() {
    memoryCopyBanked(gbTileMemory, titleTiles, titleTilesLength, titleTilesBank);
}

void mainMenuWake() {
    // gbLog("Wake main menu!");
}

void mainMenuUpdate() {
    if(gbJoypadPressedSinceLastUpdate & gbJoypadStart) {
        gbLog("Start!");
    }
}

void mainMenuUpdateGraphics() {
    
}
