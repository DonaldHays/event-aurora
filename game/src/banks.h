#ifndef banks_h
#define banks_h

#include <gb/gb.h>

extern volatile GBUInt8 _banksROMCurrent;

void banksInit();

#define banksROMGet() (_banksROMCurrent)
#define banksROMSet(bank) { _banksROMCurrent = (bank); gbROMBankSet(bank); }

#endif
