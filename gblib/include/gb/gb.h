#ifndef gb_h
#define gb_h

#include "types.h"
#include "registers.h"
#include "interrupts.h"
#include "joypad.h"

#define gbTileMemory ((volatile void *)0x8000)
#define gbTileMap0 ((volatile void *)0x9800)
#define gbTileMap1 ((volatile void *)0x9C00)
#define gbSRAM ((volatile void *)0xA000)

#define gbTileMapWidth 32
#define gbTileMapHeight 32

#define gbShadeBlack 3
#define gbShadeDarkGray 2
#define gbShadeLightGray 1
#define gbShadeWhite 0

#define gbActiveInterruptFlagVBlank 0x01
#define gbActiveInterruptFlagLCD 0x02
#define gbActiveInterruptFlagTimer 0x04
#define gbActiveInterruptFlagSerial 0x08
#define gbActiveInterruptFlagJoypad 0x10

#define gbInterruptsEnable() { __asm__("ei"); }
#define gbInterruptsDisable() { __asm__("di"); }
#define gbHalt() { __asm__("halt\nnop"); }

#define _gbMergeImpl(x,y) x##y
#define _gbMerge(x,y) _gbMergeImpl(x,y)
#define _gbStrImpl(x) #x
#define _gbStr(x) _gbStrImpl(x)

#define _gbLog(message, counter) { \
    __asm \
    ld d, d \
    jr _gbMerge(counter, $) \
    .dw 0x6464 \
    .dw 0x0000 \
    .strz message \
_gbMerge(counter, $): \
    __endasm; \
}

/** Triggers a source-code breakpoint in supported emulators. */
#define gbBreak() { __asm__("ld b, b"); }
#define gbLog(message) _gbLog(message, __COUNTER__)
#define gbFatalError(message) {\
    gbLog("ERROR:") \
    gbLog(__FILE__) \
    gbLog(_gbStr(__LINE__)) \
    gbLog(message) \
    gbBreak() \
}

#ifdef DEBUG
    #define gbAssert(expression, message) {\
        if((expression) == false) { \
            gbFatalError(message); \
        } \
    }
#else
    #define gbAssert(expression, message)
#endif

#define gbBackgroundScrollSet(x, y) { gbBackgroundScrollXRegister = (x); gbBackgroundScrollYRegister = (y); }
#define gbWindowScrollSet(x, y) { gbWindowScrollXRegister = (x); gbWindowScrollYRegister = (y); }
#define gbPaletteMake(black, dark, light, white) (((black) << 6) | ((dark) << 4) | ((light) << 2) | (white))

#define gbROMBankSet(bank) { gbROMSelectRegister = bank; }

#define gbSRAMEnable() { gbSRAMEnableRegister = 0x0A; }
#define gbSRAMDisable() { gbSRAMEnableRegister = 0x00; }

void gbLCDDisable();
void gbLCDEnable();

void gbSpritesDisable();
void gbSpritesEnable();

void gbLogUInt8(GBUInt8 value);
void gbLogUInt16(GBUInt16 value);

#endif /* gb_h */
