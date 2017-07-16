#ifndef memory_h
#define memory_h

#include <gb/gb.h>

/**
 * Sets `length` bytes starting at `destination` to `value`.
 */
void memorySet(void * destination, GBUInt8 value, GBUInt16 length);

/**
 * Copies 16 bytes from `source` to `destination`.
 *
 * This function takes about 40% of the time that `memoryCopy` does when
 * copying 16 bytes.
 */
void memoryCopy16(void * destination, const void * source);

/**
 * Copies `length` bytes from `source` to `destination`.
 *
 * This function takes about 30% of the time that `memcpy` does when copying
 * 16 bytes.
 */
void memoryCopy(void * destination, const void * source, GBUInt16 length);

/**
 * Copies `length` bytes from `source` in `sourceROMBank` to `destination`, and
 * then restores the current ROM bank.
 */
void memoryCopyBanked(void * destination, const void * source, GBUInt16 length, GBUInt8 sourceROMBank);

#endif
