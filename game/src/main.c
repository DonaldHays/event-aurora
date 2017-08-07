#include "main.h"
#include "module.h"
#include "audio.h"
#include "banks.h"
#include "rand.h"
#include "data/music_titleSong.h"

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
    gbVBlankInterruptHandler = &_vblank;
    gbActiveInterruptsRegister = gbActiveInterruptFlagVBlank;
    
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
        randomInit();
        banksInit();
        audioInit();
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
    audioPlayComposition(&titleSong, titleSongBank, audioLayerMusic, 0);
    
    while(true) {
        _waitForVBlank();
        
        modulesUpdateGraphics();
        audioUpdate();
        gbJoypadStateUpdate();
        modulesUpdate();
    }
}
