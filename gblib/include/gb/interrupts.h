#ifndef gb_interrupts_h
#define gb_interrupts_h

extern volatile void (*vblankInterruptHandler)();
extern volatile void (*lcdInterruptHandler)();
extern volatile void (*timerInterruptHandler)();
extern volatile void (*serialInterruptHandler)();
extern volatile void (*joypadInterruptHandler)();

#endif
