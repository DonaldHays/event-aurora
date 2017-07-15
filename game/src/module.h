#ifndef module_h
#define module_h

#include <gb/gb.h>

typedef struct {
    /**
     * Called on program launch.
     * 
     * When called, the screen will be disabled. The order that different
     * modules will be initialized in is undefined, therefore it is not safe to
     * rely on the state of any other modules.
     */
    void (*init)();
    
    /**
     * Called when a module becomes the active module.
     *
     * When called, the screen will be enabled. It's possible for the active
     * module to transition to itself, so this function may be called on an
     * already-active module.
     */
    void (*wake)();
    
    /**
     * Called once per frame. Update logic, but not video RAM, here.
     */
    void (*update)();
    
    /**
     * Called once per frame. Update video RAM here. Try to be quick.
     */
    void (*updateGraphics)();
    
    /**
     * The ROM bank in which the functions `self`'s pointers point to can be
     * found. Set to 0 if they're in the home bank.
     */
    GBUInt8 romBank;
} Module;

extern const Module mainMenuModule;

void modulesInit();
void modulesCurrentSet(Module * module);
void modulesUpdate();
void modulesUpdateGraphics();

#endif
