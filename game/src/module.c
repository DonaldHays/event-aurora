#include "module.h"

void blargh();

const Module mainMenuModule = { &blargh, null, null, null, 0 };

void modulesInit() {
    (*mainMenuModule.init)();
}

void blargh() {
    
}

void modulesUpdate() {
    gbJoypadStateUpdate();
    
    
}
