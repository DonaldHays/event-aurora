#ifndef gb_joypad_h
#define gb_joypad_h

#include "types.h"

#define gbJoypadStart 0x80
#define gbJoypadSelect 0x40
#define gbJoypadB 0x20
#define gbJoypadA 0x10
#define gbJoypadDown 0x08
#define gbJoypadUp 0x04
#define gbJoypadLeft 0x02
#define gbJoypadRight 0x01

typedef GBUInt8 GBJoypadState;

extern GBJoypadState gbJoypadState;
extern GBJoypadState gbJoypadPressedSinceLastUpdate;
extern GBJoypadState gbJoypadReleasedSinceLastUpdate;

void gbJoypadStateUpdate();

#endif