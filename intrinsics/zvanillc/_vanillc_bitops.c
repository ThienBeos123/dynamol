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
// De Bruijn Magic lookup maps for 64-bit hashing keys
static const uint8_t debruijn_clz_table[64] = {
    63,  0, 58,  1, 59, 47, 53,  2, 60, 39, 48, 27, 54, 33, 42,  3,
    61, 51, 37, 40, 49, 18, 28, 20, 55, 30, 34, 11, 43, 14, 22,  4,
    62, 57, 46, 52, 38, 26, 32, 41, 50, 36, 17, 19, 29, 10, 13, 21,
    56, 45, 25, 31, 35, 16,  9, 12, 44, 24, 15,  8, 23,  7,  6,  5
};
static const uint8_t debruijn_ctz_table[64] = {
     0,  1,  2, 53,  3,  7, 54, 27,  4, 38, 41,  8, 34, 55, 48, 28,
    62,  5, 39, 46, 44, 42, 22,  9, 24, 35, 59, 56, 49, 18, 29, 11,
    63, 52,  6, 26, 37, 40, 33, 47, 61, 45, 43, 21, 23, 58, 17, 10,
    51, 25, 36, 32, 60, 44, 57, 16, 50, 31, 15, 14, 13, 12, 50, 30
};

// 64 bit Count Leading Zeros
uint8_t _cintrin_clz64(uint64_t x) {
    if (!x) return 64;
    // Smear bits to the right to fill all positions below highest bit
    x |= x >> 1;  x |= x >> 2;
    x |= x >> 4;  x |= x >> 8;
    x |= x >> 16; x |= x >> 32;
    // Multiply by De Bruijn Constant and fetch via index mapping
    return debruijn_clz_table[(x * UINT64_C(0x07C4ACDDFFB40F2D)) >> 58];
}

// 64 bit Count Trailing Zeros
uint8_t _cintrin_ctz64(uint64_t x) {
    if (!x) return 64;
    // Isolate lowest set bit and multiply by De Bruijn factor
    return debruijn_ctz_table[((x & -x) * UINT64_C(0x022FDD63CC95386D)) >> 58];
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