#include "module.h"

#include "bank1/mainMenu.h"
#include "banks.h"

// ===
// Public Constant Data
// ===
const Module mainMenuModule = { &mainMenuInit, &mainMenuWake, &mainMenuUpdate, &mainMenuUpdateGraphics, 1 };

// ===
// Private Variables
// ===
Module * _currentModule;

// ===
// Public API
// ===
void modulesInit() {
    banksROMSet(mainMenuModule.romBank);
    (*mainMenuModule.init)();
}

void modulesCurrentSet(Module * module) {
    _currentModule = module;
    
    banksROMSet(module->romBank);
    
    if(module->wake != null) {
        (*module->wake)();
    }
}

void modulesUpdate() {
    if(_currentModule->update != null) {
        banksROMSet(_currentModule->romBank);
        
        _currentModule->update();
    }
}

void modulesUpdateGraphics() {
    if(_currentModule->updateGraphics != null) {
        banksROMSet(_currentModule->romBank);
        
        _currentModule->updateGraphics();
    }
}
