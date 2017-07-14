#include <gb/gb.h>

// ===
// Public Variables
// ===
volatile void (*gbVBlankInterruptHandler)();
volatile void (*gbLCDInterruptHandler)();
volatile void (*gbTimerInterruptHandler)();
volatile void (*gbSerialInterruptHandler)();
volatile void (*gbJoypadInterruptHandler)();

GBJoypadState gbJoypadState;
GBJoypadState gbJoypadPressedSinceLastUpdate;
GBJoypadState gbJoypadReleasedSinceLastUpdate;

// ===
// Public API
// ===
void gbLCDDisable() {
    __asm
    
    push af
    
    ; busy wait for vblank
.lcdDisableLoop:
    ld a, (#0xff44)
    cp a, #144
    jr nz, .lcdDisableLoop
    
    ; perform the disable
    ld hl, #0xff40
    res #7, (hl)
    
    pop af
    
    __endasm;
}

void gbLCDEnable() {
    __asm
    ld hl, #0xff40
    set #7, (hl)
    __endasm;
}

void gbSpritesDisable() {
    __asm
    ld hl, #0xff40
    res #1, (hl)
    __endasm;
}

void gbSpritesEnable() {
    __asm
    ld hl, #0xff40
    set #1, (hl)
    __endasm;
}

void gbLogUInt8(GBUInt8 value) {
    (void)(value); // Suppresses unused variable warning
    
    __asm
    push af
    push hl
    ldhl sp, #3
    ld a, (hl)
    ld d, d
    jr .gbLogUInt8End
    .dw 0x6464
    .dw 0x0000
    .strz "0x%a%"
.gbLogUInt8End:
    pop hl
    pop af
    __endasm;
}

void gbLogUInt16(GBUInt16 value) {
    (void)(value); // Suppresses unused variable warning
    
    __asm
    push af
    push bc
    push hl
    ldhl sp, #8
    ld a, (hl+)
    ld c, a
    ld b, (hl)
    ld d, d
    jr .gbLogUInt16End
    .dw 0x6464
    .dw 0x0000
    .strz "0x%bc%"
.gbLogUInt16End:
    pop hl
    pop bc
    pop af
    __endasm;
}
