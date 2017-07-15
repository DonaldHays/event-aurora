#include "memory.h"

// ===
// Public API
// ===
void memorySet(void * destination, GBUInt8 value, GBUInt16 length) {
    // Silence unused variable warnings
    (void)destination;
    (void)value;
    (void)length;
    __asm
    
    ; bc = length
    ldhl sp, #5
    ld c, (hl)
    inc hl
    ld b, (hl)
    
    ; d = value
    ldhl sp, #4
    ld a, (hl)
    ld d, a
    
    ; hl = destination
    ldhl sp, #2
    ld a, (hl+)
    ld h, (hl)
    ld l, a
    
.memorySetLoop:
    ; if bc == 0 break
    ld a, c
    or b
    jr z, .memorySetEndLoop
    
    ; *(hl++) = d
    ld a, d
    ld (hl+), a
    
    ; bc--
    dec bc
    
    ; continue;
    jr .memorySetLoop

.memorySetEndLoop:
    
    __endasm;
}

void memoryCopy16(void * destination, const void * source) {
    // This routine takes about 114 machine cycles.
    
    // Silence unused variable warnings
    (void)destination;
    (void)source;
    
    __asm
    
    ; de = destination
    ldhl sp, #2
    ld e, (hl)
    inc hl
    ld d, (hl)
    
    ; hl = source
    inc hl
    ld a, (hl+)
    ld h, (hl)
    ld l, a
    
    ; *(de++) = *(hl++) (x16)
    ld a, (hl+)
    ld (de), a
    inc de
    
    ld a, (hl+)
    ld (de), a
    inc de
    
    ld a, (hl+)
    ld (de), a
    inc de
    
    ld a, (hl+)
    ld (de), a
    inc de
    
    ld a, (hl+)
    ld (de), a
    inc de
    
    ld a, (hl+)
    ld (de), a
    inc de
    
    ld a, (hl+)
    ld (de), a
    inc de
    
    ld a, (hl+)
    ld (de), a
    inc de
    
    ld a, (hl+)
    ld (de), a
    inc de
    
    ld a, (hl+)
    ld (de), a
    inc de
    
    ld a, (hl+)
    ld (de), a
    inc de
    
    ld a, (hl+)
    ld (de), a
    inc de
    
    ld a, (hl+)
    ld (de), a
    inc de
    
    ld a, (hl+)
    ld (de), a
    inc de
    
    ld a, (hl+)
    ld (de), a
    inc de
    
    ld a, (hl)
    ld (de), a
    
    __endasm;
}

void memoryCopy(void * destination, const void * source, GBUInt16 length) {
    // This routine takes about 271 machine cycles to copy 16 bytes, versus 962 for SDCC's memcpy.
    
    // Silence unused variable warnings
    (void)destination;
    (void)source;
    (void)length;
    
    __asm
    
    ; de = destination
    ldhl sp, #2
    ld e, (hl)
    inc hl
    ld d, (hl)
    
    ; bc = length
    ldhl sp, #6
    ld c, (hl)
    inc hl
    ld b, (hl)
    
    ; hl = source
    ldhl sp, #4
    ld a, (hl+)
    ld h, (hl)
    ld l, a
    
.loop:
    ; if bc == 0 break
    ld a, c
    or b
    jr z, .endLoop
    
    ; *(de++) = *(hl++)
    ld a, (hl+)
    ld (de), a
    inc de
    
    ; bc--
    dec bc
    
    ; continue;
    jr .loop

.endLoop:
    
    __endasm;
}
