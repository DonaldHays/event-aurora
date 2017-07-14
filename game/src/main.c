#include "main.h"
#include "module.h"

// ===
// Private Variables
// ===
volatile GBBool hasEnteredVBlank;

// ===
// Private API
// ===
void _vblank() {
    hasEnteredVBlank = true;
}

// ===
// Public API
// ===
void main() {
    hasEnteredVBlank = false;
    
    gbVBlankInterruptHandler = &_vblank;
    gbActiveInterruptsRegister = gbActiveInterruptFlagVBlank;
    
    gbLCDDisable();
    modulesInit();
    gbLCDEnable();
    
    gbInterruptsEnable();
    
    while(true) {
        hasEnteredVBlank = false;
        gbHalt();
        if(hasEnteredVBlank == false) {
            continue;
        }
        
        modulesUpdate();
    }
}
