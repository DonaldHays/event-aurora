#include "mainMenu.h"

#pragma bank 1

// ===
// Public API
// ===
void mainMenuInit() {
    gbLog("Initialize Main Menu!");
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
