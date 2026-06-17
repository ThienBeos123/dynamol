/*
Copyright (C) 2026 @ThienBeos123/@Poly-glon

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

  http://apache.org

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/



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
