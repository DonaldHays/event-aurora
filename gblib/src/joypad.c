#include <gb/gb.h>

void gbJoypadStateUpdate() {
    GBUInt8 lastValue;
    
    lastValue = gbJoypadState;
    
    __asm
    ; Procedure from http://www.emulatronia.com/doctec/consolas/gameboy/gameboy.txt
    ; though we swapped which set of 4 bits are high vs low to match the values from
    ; the `joypad` function from GBDK.
    
    push af
    push bc
    
    ; Read first part of joypad state
    ld a, #0x20
    ld (#0xff00), a
    
    ; Read several times to burn cycles waiting for register to update
    ld a, (#0xff00)
    ld a, (#0xff00)
    cpl ; Invert so 1=on and 0=off
    and #0x0f ; Only want 4 least significant bits
    ld b, a ; Store in b
    
    ; Read second part of joypad state
    ld a, #0x10
    ld (#0xff00), a
    
    ; Read several times to burn cycles waiting for register to update
    ld a, (#0xff00)
    ld a, (#0xff00)
    ld a, (#0xff00)
    ld a, (#0xff00)
    ld a, (#0xff00)
    ld a, (#0xff00)
    
    cpl ; invert
    and #0x0f ; only 4 least significant bits
    swap a  ; Swap nibbles
    or b ; Merge with b
    
    ld (_gbJoypadState), a
    
    pop bc
    pop af
    
    __endasm;
    
    gbJoypadPressedSinceLastUpdate = (gbJoypadState ^ lastValue) & gbJoypadState;
    gbJoypadReleasedSinceLastUpdate = (gbJoypadState ^ lastValue) & lastValue;
}