#include "main.h"
#include "module.h"
#include "banks.h"

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
    banksInit();
    modulesInit();
    gbLCDEnable();
    
    gbInterruptsEnable();
    
    modulesCurrentSet(&mainMenuModule);
    
    while(true) {
        hasEnteredVBlank = false;
        gbHalt();
        if(hasEnteredVBlank == false) {
            continue;
        }
        
        modulesUpdateGraphics();
        
        gbJoypadStateUpdate();
        modulesUpdate();
    }
}
