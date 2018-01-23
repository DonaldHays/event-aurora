#include "banks.h"

// ===
// Private Variables
// ===
volatile GBUInt8 _banksROMCurrent;

// ===
// Public API
// ===
void banksInit() {
    _banksROMCurrent = 0;
}
