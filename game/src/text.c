#include "text.h"
#include "memory.h"
#include "data/gfx_font.h"

// ===
// Public API
// ===
void textLoadFont(void * destination, GBBool shouldInvert) {
    GBUInt16 index;
    
    memoryCopyBanked(destination, font, fontLength, fontBank);
    
    if(shouldInvert) {
        for(index = 0; index < fontLength; index++) {
            ((GBUInt8 *)destination)[index] ^= 0xFF;
        }
    }
}
