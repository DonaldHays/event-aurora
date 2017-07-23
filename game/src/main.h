#ifndef main_h
#define main_h

#include <gb/gb.h>

/**
 * The program entry-point.
 *
 * This function does not return. It should never be called by program code
 * unless the entire function stack is first reset, otherwise a gradual stack
 * overflow is risked.
 */
void main();

#endif
