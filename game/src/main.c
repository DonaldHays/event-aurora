#include "main.h"
#include "module.h"

// ===
// Public API
// ===
void main() {
    gbLCDDisable();
    modulesInit();
    gbLCDEnable();
    
    while(true) {
        gbHalt();
    }
}
