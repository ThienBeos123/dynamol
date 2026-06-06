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
// De-bruijin Magic Sequences
static const int debruijn_clz_table[64] = {
    63, 62, 15, 61, 6, 14, 35, 60, 2, 5, 13, 21, 25, 34, 46, 59, 1, 
    8, 4, 27, 10, 12, 20, 41, 18, 24, 30, 33, 39, 45, 51, 58, 0, 
    16, 7, 36, 3, 22, 26, 47, 9, 28, 11, 42, 19, 31, 40, 52, 17, 
    37, 23, 48, 29, 43, 32, 53, 38, 49, 44, 54, 50, 55, 56, 57
};

static const int debruijn_ctz_table[64] = {
    0, 1, 48, 2, 57, 49, 28, 3, 61, 58, 50, 42, 38, 29, 17, 4, 62, 
    55, 59, 36, 53, 51, 43, 22, 45, 39, 33, 30, 24, 18, 12, 5, 63, 
    47, 56, 27, 60, 41, 37, 16, 54, 35, 52, 21, 44, 32, 23, 11, 46, 
    26, 40, 15, 34, 20, 31, 10, 25, 14, 19, 9, 13, 8, 7, 6
};

uint8_t _cintrin_clz64(uint64_t x) {
    if (!x) return 64; // Handle zero input safely
    if (x & UINT64_C(0x8000000000000000)) return 0;
    x |= x >> 1; x |= x >> 2;
    x |= x >> 4; x |= x >> 8;
    x |= x >> 16; x |= x >> 32;
    return debruijn_clz_table[(((x >> 1) + 1) * UINT64_C(0x03F79D71B4CB0A89)) >> 58];
}

uint8_t _cintrin_ctz64(uint64_t x) {
    if (!x) return 64; // Handle zero input safely
    if (x & UINT64_C(0x0000000000000001)) return 0;
    return debruijn_ctz_table[((x & -x) * UINT64_C(0x03F79D71B4CB0A89)) >> 58];
}

// 64 bit Byte Swapping
uint64_t _cintrin_bswap64(uint64_t x) {
    // Stage 1: Swap adjacent 8-bit bytes
    // Mask isolates bytes 0, 2, 4, 6.
    // Shifting grabs bytes 1, 3, 5, 7.
    x = ((x >> 8)  & UINT64_C(0x00FF00FF00FF00FF)) |
        ((x & UINT64_C(0x00FF00FF00FF00FF)) << 8);

    // Stage 2: Swap adjacent 16-bit words
    // Mask isolates bytes [0,1] and [4,5].
    // Shifting grabs bytes [2,3] and [6,7].
    x = ((x >> 16) & UINT64_C(0x0000FFFF0000FFFF)) |
        ((x & UINT64_C(0x0000FFFF0000FFFF)) << 16);

    // Stage 3: Swap the 32-bit halves (No mask required!)
    return (x >> 32) | (x << 32);
}

// 64 bit Popoulation Counting
uint8_t _cintrin_pcnt64(uint64_t x) {
    x = x - ((x >> 1) & UINT64_C(0x5555555555555555));
    x = (x & UINT64_C(0x3333333333333333)) + ((x >> 2) & UINT64_C(0x3333333333333333));
    x = (x + (x >> 4)) & UINT64_C(0x0F0F0F0F0F0F0F0F);
    x = x * UINT64_C(0x0101010101010101);
    return (uint8_t)(x >> 56);
}
