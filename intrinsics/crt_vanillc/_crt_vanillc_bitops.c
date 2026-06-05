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



#include "__crt_vanillc_con__.h"



// 64 bit Count Leading Zeros
uint8_t _crtintrin_clz64(uint64_t x) {
    uint64_t is_zero_mask = ~(x | (-x)) >> 63;
    uint64_t n = 0, mask;
    mask = -((int64_t)!(x >> 32)); n += mask & 32; x <<= mask & 32;
    mask = -((int64_t)!(x >> 48)); n += mask & 16; x <<= mask & 16;
    mask = -((int64_t)!(x >> 56)); n += mask & 8;  x <<= mask & 8;
    mask = -((int64_t)!(x >> 60)); n += mask & 4;  x <<= mask & 4;
    mask = -((int64_t)!(x >> 62)); n += mask & 2;  x <<= mask & 2;
    mask = -((int64_t)!(x >> 63)); n += mask & 1; // clang-format off
    uint8_t res = (uint8_t)(n ^ (is_zero_mask & (n ^ U64_BITS)));
    is_zero_mask = 0; n = 0; mask = 0; x = 0; return res; // clang-format on
}

// 64 bit Count Trailing Zeros
uint8_t _crtintrin_ctz64(uint64_t x) {
    uint64_t is_zero_mask = ~(x | (-x)) >> 63;
    uint64_t lowest_bit = x & -x; uint64_t n = 0, mask;
    mask = -((int64_t)!(lowest_bit >> 32)); n += mask & 32; lowest_bit <<= mask & 32;
    mask = -((int64_t)!(lowest_bit >> 48)); n += mask & 16; lowest_bit <<= mask & 16;
    mask = -((int64_t)!(lowest_bit >> 56)); n += mask & 8;  lowest_bit <<= mask & 8;
    mask = -((int64_t)!(lowest_bit >> 60)); n += mask & 4;  lowest_bit <<= mask & 4;
    mask = -((int64_t)!(lowest_bit >> 62)); n += mask & 2;  lowest_bit <<= mask & 2; // clang-format off
    uint64_t ctz_val = 63 - n; uint8_t res =  (uint8_t)(ctz_val ^ (is_zero_mask & (ctz_val ^ U64_BITS)));
    is_zero_mask = 0; lowest_bit = 0; n = 0; mask = 0; ctz_val = 0; x = 0; return res; // clang-format on
}

// 64 bit Byte Swapping
uint64_t _crtintrin_bswap64(uint64_t x) {
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
uint8_t _crtintrin_pcnt64(uint64_t x) {
    x = x - ((x >> 1) & UINT64_C(0x5555555555555555));
    x = (x & UINT64_C(0x3333333333333333)) + ((x >> 2) & UINT64_C(0x3333333333333333));
    x = (x + (x >> 4)) & UINT64_C(0x0F0F0F0F0F0F0F0F);
    x = x * UINT64_C(0x0101010101010101);
    return (uint8_t)(x >> 56);
}
