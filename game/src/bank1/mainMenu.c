#include "mainMenu.h"
#include "../memory.h"
#include "../data/gfx_font.h"

#pragma bank 1

// ===
// Public API
// ===
void mainMenuInit() {
    gbLog("Initialize Main Menu!");
    
    memoryCopy(gbTileMemory, font, fontLength);
    memorySet(gbTileMap0, 0x54, gbTileMapWidth * gbTileMapHeight);
}

void mainMenuWake() {
    gbLog("Wake main menu!");
}

void mainMenuUpdate() {
    if(gbJoypadPressedSinceLastUpdate & gbJoypadStart) {
        gbLog("Start!");
    }
}

void mainMenuUpdateGraphics() {
    
}
