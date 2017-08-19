#include "main.h"
#include "module.h"
#include "audio.h"
#include "banks.h"
#include "rand.h"
#include "palette.h"
#include "sprites.h"

// ===
// Private Variables
// ===

/**
 * Has a vertical blank interrupt recently occurred?
 */
volatile GBBool _hasEnteredVBlank;

// ===
// Private API
// ===

/**
 * Handles the vertical blank interrupt by setting `_hasEnteredVBlank` to
 * `true`.
 */
void _vblank() {
    _hasEnteredVBlank = true;
}

/**
 * Initializes variables used by the main subsystem.
 */
void _initializeMainMemory() {
    _hasEnteredVBlank = false;
}

/**
 * Installs and activates the interrupt handlers.
 */
void _initializeInterruptHandlers() {
    gbLCDInterruptHandler = null;
    gbVBlankInterruptHandler = &_vblank;
    gbActiveInterruptsRegister = gbActiveInterruptFlagVBlank | gbActiveInterruptFlagLCD;
    
    gbInterruptsEnable();
}

/**
 * Initializes all other subsystems in the program.
 */
void _initializeSubsystems() {
    // The API contract guarantees that the LCD is disabled when subsystems
    // (especially the module subsystem) are initialized, allowing them to
    // freely write to video memory.
    gbLCDDisable(); {
        // Disables bit 4 of LCD Control Register, making background tiles begin from 0x8800.
        gbLCDControlRegister &= 0xEF;
        
        gbSpritesEnable();
        
        randomInit();
        banksInit();
        paletteInit();
        audioInit();
        spritesInit();
        modulesInit();
    } gbLCDEnable();
}

/**
 * Returns after the system enters the vertical blank period.
 *
 * @precondition interrupts must be enabled.
 */
void _waitForVBlank() {
    while(true) {
        _hasEnteredVBlank = false;
        gbHalt();
        if(_hasEnteredVBlank) {
            break;
        }
    }
}

// ===
// Public API
// ===
void main() {
    _initializeMainMemory();
    _initializeSubsystems();
    _initializeInterruptHandlers();
    
    modulesCurrentSet(&mainMenuModule);
    
    while(true) {
        _waitForVBlank();
        
        spritesWriteToOAM();
        modulesUpdateGraphics();
        paletteUpdateGraphics();
        audioUpdate();
        gbJoypadStateUpdate();
        modulesUpdate();
    }
}
