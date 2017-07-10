#include "main.h"

// ===
// Public API
// ===
void main() {
    gbLCDDisable();
    gbLCDEnable();
    
    while(true) {
        gbHalt();
    }
}
