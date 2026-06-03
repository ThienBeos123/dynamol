#include "__vanillc_conn__.h"

// I argue this file is for filler, meaningless operations
// in Vanilla C just to ensure cross-platform compatibility can still runs


// Meaningless, STDLIB Shallow RNG
uint64_t _cintrin_shallow_rng(int *err) {
    srand(time(NULL));
    *err = 0; return rand();
}

// Meaningless, NOP Shallow Halt
void _cintrin_nop_halt(void) {
    int a = 10, b = 23;
    int c = a + b;
    c += 0 - 0 + 0 * 10;
    return;
}