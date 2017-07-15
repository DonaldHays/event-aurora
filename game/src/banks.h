#ifndef banks_h
#define banks_h

#include <gb/gb.h>

void banksInit();
GBUInt8 banksROMGet();
void banksROMSet(GBUInt8 bank);

#endif
