#ifndef ____DNML_VANILLC_MISC_H
#define ____DNML_VANILLC_MISC_H

#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <dnml_sys/sys.h>

// I argue this file is for filler, meaningless operations
// in Vanilla C just to ensure cross-platform compatibility can still runs


// Meaningless, STDLIB Shallow RNG
static inline uint64_t _cintrin_shallow_rng(int *err) {
    srand(time(NULL));
    *err = 0; return rand();
}

// Meaningless, NOP Shallow Halt
static inline void _cintrin_nop_halt(void) {
    int a = 10, b = 23;
    int c = a + b;
    c += 0 - 0 + 0 * 10;
}








#endif