#include "module.h"

#include "bank1/mainMenu.h"
#include "bank4/game.h"
#include "banks.h"
#include "sprites.h"

// ===
// Public Constant Data
// ===
const Module mainMenuModule = { &mainMenuInit, &mainMenuWake, &mainMenuUpdate, &mainMenuUpdateGraphics, 1, 1 };
const Module gameModule = { &gameInit, &gameWake, &gameUpdate, &gameUpdateGraphics, 4, 0 };

// ===
// Private Variables
// ===
GBUInt8 _lcdInterruptMode;
Module * _currentModule;
Module * _nextModule;

GBUInt8 _modulesCloud0Scroll;
GBUInt8 _modulesCloud0ScrollTimer;
GBUInt8 _modulesCloud1Scroll;
GBUInt8 _modulesCloud1ScrollTimer;
GBUInt8 _modulesCloud2Scroll;
GBUInt8 _modulesCloud2ScrollTimer;

// ===
// Private API
// ===
void _modulesLCDInterruptHandler() {
    if(_lcdInterruptMode == 1) {
        if(gbLCDYCoordinateRegister == 15) {
            gbBackgroundScrollXRegister = _modulesCloud0Scroll;
            gbLCDYCoordinateCompareRegister = 31;
        } else if(gbLCDYCoordinateRegister == 31) {
            gbBackgroundScrollXRegister = _modulesCloud1Scroll;
            gbLCDYCoordinateCompareRegister = 39;
        } else if(gbLCDYCoordinateRegister == 39) {
            gbBackgroundScrollXRegister = _modulesCloud2Scroll;
            gbLCDYCoordinateCompareRegister = 47;
        } else if(gbLCDYCoordinateRegister == 47) {
            gbBackgroundScrollXRegister = 0;
            gbLCDYCoordinateCompareRegister = 15;
        }
    }
}

// ===
// Public API
// ===
void modulesInit() {
    _lcdInterruptMode = 0;
    
    _modulesCloud0Scroll = 0;
    _modulesCloud0ScrollTimer = 4;
    _modulesCloud1Scroll = 0;
    _modulesCloud1ScrollTimer = 6;
    _modulesCloud2Scroll = 0;
    _modulesCloud2ScrollTimer = 7;
    
    _currentModule = null;
    _nextModule = null;
    
    banksROMSet(mainMenuModule.romBank);
    (*mainMenuModule.init)();
    
    banksROMSet(gameModule.romBank);
    (*gameModule.init)();
}

void modulesCurrentSet(Module * module) {
    _currentModule = module;
    
    gbInterruptsDisable(); {
        _lcdInterruptMode = module->lcdInterruptMode;
        
        if(_lcdInterruptMode == 1) {
            gbLCDStatusRegister = 0x44;
            gbLCDYCoordinateCompareRegister = 15;
            gbLCDInterruptHandler = &_modulesLCDInterruptHandler;
        } else {
            gbLCDStatusRegister = 0x00;
            gbLCDInterruptHandler = null;
        }
    } gbInterruptsEnable();
    
    spritesClear();
    
    banksROMSet(module->romBank);
    
    if(module->wake != null) {
        (*module->wake)();
    }
}

void modulesSetNext(Module * module) {
    _nextModule = module;
}

void modulesUpdate() {
    if(_nextModule != null) {
        modulesCurrentSet(_nextModule);
        _nextModule = null;
    }
    
    if(_lcdInterruptMode == 1) {
        _modulesCloud0ScrollTimer--;
        if(_modulesCloud0ScrollTimer == 0) {
            _modulesCloud0ScrollTimer = 2;
            _modulesCloud0Scroll++;
            if(_modulesCloud0Scroll == 32) {
                _modulesCloud0Scroll = 0;
            }
        }
        
        _modulesCloud1ScrollTimer--;
        if(_modulesCloud1ScrollTimer == 0) {
            _modulesCloud1ScrollTimer = 4;
            _modulesCloud1Scroll++;
            if(_modulesCloud1Scroll == 32) {
                _modulesCloud1Scroll = 0;
            }
        }
        
        _modulesCloud2ScrollTimer--;
        if(_modulesCloud2ScrollTimer == 0) {
            _modulesCloud2ScrollTimer = 12;
            _modulesCloud2Scroll++;
            if(_modulesCloud2Scroll == 32) {
                _modulesCloud2Scroll = 0;
            }
        }
    }
    
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
