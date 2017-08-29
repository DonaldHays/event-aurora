#include "mainMenu.h"
#include "../memory.h"
#include "../sprites.h"
#include "../palette.h"
#include "../data/gfx_titleTiles.h"
#include "../data/music_titleSong.h"
#include "../data/music_testBoing.h"

#pragma bank 1

// ===
// Private Defines
// ===
#define _mainMenuPressStartY 136
#define _mainMenuPressStartCycleDuration 120

#define _mainMenuFadeStateFadingIn 1
#define _mainMenuFadeStateIdle 2
#define _mainMenuFadeStateFadingOut 3

#define _mainMenuMakeFadeState(state, timer) (((timer) << 4) | (state))
#define _mainMenuFadeStateTimer() (_mainMenuFadeRegister >> 4)
#define _mainMenuFadeStateState() (_mainMenuFadeRegister & 0x0F)

// ===
// Private Variables
// ===
GBUInt8 _mainMenuPressStartCycle;

/**
 * TTTTSSSS
 * TTTT - Timer for current state
 * SSSS - Current state
 */
GBUInt8 _mainMenuFadeRegister;

// ===
// Private API
// ===
void _mainMenuBeginFadeIn() {
    _mainMenuFadeRegister = _mainMenuMakeFadeState(_mainMenuFadeStateFadingIn, 15);
}

void _mainMenuBeginFadeOut() {
    _mainMenuFadeRegister = _mainMenuMakeFadeState(_mainMenuFadeStateFadingOut, 15);
}

void _mainMenuUpdateFadeState() {
    GBUInt8 state, timer;
    
    state = _mainMenuFadeStateState();
    timer = _mainMenuFadeStateTimer();
    
    switch(state) {
    case _mainMenuFadeStateFadingIn:
        if(timer > 8) {
            backgroundPalette = gbPaletteMake(gbShadeLightGray, gbShadeWhite, gbShadeWhite, gbShadeWhite);
            object0Palette = gbPaletteMake(gbShadeLightGray, gbShadeWhite, gbShadeWhite, gbShadeWhite);
            object1Palette = gbPaletteMake(gbShadeLightGray, gbShadeWhite, gbShadeWhite, gbShadeWhite);
        } else {
            backgroundPalette = gbPaletteMake(gbShadeDarkGray, gbShadeLightGray, gbShadeWhite, gbShadeWhite);
            object0Palette = gbPaletteMake(gbShadeDarkGray, gbShadeLightGray, gbShadeWhite, gbShadeWhite);
            object1Palette = gbPaletteMake(gbShadeDarkGray, gbShadeWhite, gbShadeWhite, gbShadeWhite);
        }
        
        timer--;
        
        if(timer == 0) {
            _mainMenuFadeRegister = _mainMenuMakeFadeState(_mainMenuFadeStateIdle, 0);
        } else {
            _mainMenuFadeRegister = _mainMenuMakeFadeState(_mainMenuFadeStateFadingIn, timer);
        }
        
        break;
    case _mainMenuFadeStateIdle:
        backgroundPalette = gbPaletteMake(gbShadeBlack, gbShadeDarkGray, gbShadeLightGray, gbShadeWhite);
        object0Palette = gbPaletteMake(gbShadeBlack, gbShadeDarkGray, gbShadeWhite, gbShadeWhite);
        object1Palette = gbPaletteMake(gbShadeBlack, gbShadeLightGray, gbShadeWhite, gbShadeWhite);
        break;
    case _mainMenuFadeStateFadingOut:
        if(timer > 8) {
            backgroundPalette = gbPaletteMake(gbShadeDarkGray, gbShadeLightGray, gbShadeWhite, gbShadeWhite);
            object0Palette = gbPaletteMake(gbShadeDarkGray, gbShadeLightGray, gbShadeWhite, gbShadeWhite);
            object1Palette = gbPaletteMake(gbShadeDarkGray, gbShadeWhite, gbShadeWhite, gbShadeWhite);
        } else if(timer != 0) {
            backgroundPalette = gbPaletteMake(gbShadeLightGray, gbShadeWhite, gbShadeWhite, gbShadeWhite);
            object0Palette = gbPaletteMake(gbShadeLightGray, gbShadeWhite, gbShadeWhite, gbShadeWhite);
            object1Palette = gbPaletteMake(gbShadeLightGray, gbShadeWhite, gbShadeWhite, gbShadeWhite);
        } else {
            backgroundPalette = gbPaletteMake(gbShadeWhite, gbShadeWhite, gbShadeWhite, gbShadeWhite);
            object0Palette = gbPaletteMake(gbShadeWhite, gbShadeWhite, gbShadeWhite, gbShadeWhite);
            object1Palette = gbPaletteMake(gbShadeWhite, gbShadeWhite, gbShadeWhite, gbShadeWhite);
        }
        
        if(timer > 0) {
            timer--;
        }
        
        _mainMenuFadeRegister = _mainMenuMakeFadeState(_mainMenuFadeStateFadingOut, timer);
        if(timer == 0) {
            // TODO: Exit main menu module
        }
        
        break;
    default:
        gbFatalError("unrecognized fade state");
    }
}

void _mainMenuUpdatePressStartCycle() {
    GBUInt8 x;
    
    _mainMenuPressStartCycle--;
    if(_mainMenuPressStartCycle == 0) {
        _mainMenuPressStartCycle = _mainMenuPressStartCycleDuration;
    }
    
    if(_mainMenuPressStartCycle < 30) {
        for(x = 0; x != 8; x++) {
            spriteAttributes[x].y = 0;
        }
    } else {
        for(x = 0; x != 8; x++) {
            spriteAttributes[x].y = _mainMenuPressStartY;
        }
    }
}

// ===
// Public API
// ===
void mainMenuInit() {
    _mainMenuPressStartCycle = _mainMenuPressStartCycleDuration;
}

void mainMenuWake() {
    GBUInt8 x, c;
    
    gbLCDDisable();
    
    memoryCopyBanked(gbTileMemory + 0x0800, titleTiles, titleTilesLength, titleTilesBank);
    
    c = 0;
    for(x = 0; x != gbTileMapWidth; x++) {
        gbTileMap0[x + 0 * gbTileMapWidth] = 128 + 15;
        gbTileMap0[x + 1 * gbTileMapWidth] = 128 + 15;
        gbTileMap0[x + 2 * gbTileMapWidth] = 128 + c;
        gbTileMap0[x + 3 * gbTileMapWidth] = 128 + 16 + c;
        gbTileMap0[x + 4 * gbTileMapWidth] = 128 + 4 + c;
        gbTileMap0[x + 5 * gbTileMapWidth] = 128 + 20 + c;
        
        c++;
        if(c == 4) {
            c = 0;
        }
    }
    
    for(x = 0; x != 20; x++) {
        gbTileMap0[x + 6 * gbTileMapWidth] = 128 + 13;
        gbTileMap0[x + 7 * gbTileMapWidth] = 128 + 13;
        gbTileMap0[x + 8 * gbTileMapWidth] = 128 + 13;
        gbTileMap0[x + 9 * gbTileMapWidth] = 128 + 13;
        
        gbTileMap0[x + 10 * gbTileMapWidth] = 128 + 64 + c;
        gbTileMap0[x + 11 * gbTileMapWidth] = 128 + 80 + c;
        gbTileMap0[x + 12 * gbTileMapWidth] = 128 + 68 + c;
        gbTileMap0[x + 13 * gbTileMapWidth] = 128 + 84 + c;
        gbTileMap0[x + 14 * gbTileMapWidth] = 128 + 72 + c;
        gbTileMap0[x + 15 * gbTileMapWidth] = 128 + 88 + c;
        
        gbTileMap0[x + 16 * gbTileMapWidth] = 128 + 14;
        gbTileMap0[x + 17 * gbTileMapWidth] = 128 + 14;
        
        c++;
        if(c == 4) {
            c = 0;
        }
    }
    
    for(x = 0; x != 12; x++) {
        gbTileMap0[x + 7 * gbTileMapWidth + 4] = 128 + 32 + x;
        gbTileMap0[x + 8 * gbTileMapWidth + 4] = 128 + 48 + x;
    }
    
    gbTileMap0[7 * gbTileMapWidth + 16] = 128 + 12;
    for(x = 0; x != 4; x++) {
        gbTileMap0[x + 8 * gbTileMapWidth + 16] = 128 + 28 + x;
        gbTileMap0[x + 9 * gbTileMapWidth + 16] = 128 + 44 + x;
        gbTileMap0[x + 10 * gbTileMapWidth + 16] = 128 + 60 + x;
        gbTileMap0[x + 11 * gbTileMapWidth + 16] = 128 + 76 + x;
        gbTileMap0[x + 12 * gbTileMapWidth + 16] = 128 + 92 + x;
        gbTileMap0[x + 13 * gbTileMapWidth + 16] = 128 + 108 + x;
    }
    
    for(x = 0; x != 8; x++) {
        spriteAttributes[x].x = 56 + x * 8;
        spriteAttributes[x].y = _mainMenuPressStartY;
        spriteAttributes[x].tileIndex = 0xE0 + x;
    }
    
    backgroundPalette = gbPaletteMake(gbShadeWhite, gbShadeWhite, gbShadeWhite, gbShadeWhite);
    object0Palette = gbPaletteMake(gbShadeWhite, gbShadeWhite, gbShadeWhite, gbShadeWhite);
    object1Palette = gbPaletteMake(gbShadeWhite, gbShadeWhite, gbShadeWhite, gbShadeWhite);
    
    audioPlayComposition(&titleSong, titleSongBank, audioLayerMusic, 0);
    
    _mainMenuBeginFadeIn();
    
    gbLCDEnable();
}

void mainMenuUpdate() {
    _mainMenuUpdatePressStartCycle();
    _mainMenuUpdateFadeState();
    
    if(gbJoypadPressedSinceLastUpdate & gbJoypadStart) {
        if(_mainMenuFadeStateState() == _mainMenuFadeStateIdle) {
            gbLog("TODO: Play fade out tone");
            audioPlayComposition(null, 0, audioLayerMusic, 0);
            _mainMenuBeginFadeOut();
        }
    }
    
    if(gbJoypadPressedSinceLastUpdate & gbJoypadA) {
        audioPlayComposition(&testBoing, testBoingBank, audioLayerSound, 0);
    }
}

void mainMenuUpdateGraphics() {
    
}
