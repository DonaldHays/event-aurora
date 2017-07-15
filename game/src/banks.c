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

GBUInt8 banksROMGet() {
    return _banksROMCurrent;
}

void banksROMSet(GBUInt8 bank) {
    _banksROMCurrent = bank;
    gbROMBankSet(bank);
}
