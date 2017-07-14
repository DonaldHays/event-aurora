#ifndef gb_interrupts_h
#define gb_interrupts_h

extern volatile void (*gbVBlankInterruptHandler)();
extern volatile void (*gbLCDInterruptHandler)();
extern volatile void (*gbTimerInterruptHandler)();
extern volatile void (*gbSerialInterruptHandler)();
extern volatile void (*gbJoypadInterruptHandler)();

#endif
