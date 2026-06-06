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
    uint8_t count = 0, is_zero = (!x);
    count   += _vanillc_crt_select((!(x & UINT64_C(0xFFFFFFFF00000000))), 32, 0);
    x       <<= _vanillc_crt_select((!(x & UINT64_C(0xFFFFFFFF00000000))), 32, 0);
    count   += _vanillc_crt_select((!(x & UINT64_C(0xFFFF000000000000))), 16, 0);
    x       <<= _vanillc_crt_select((!(x & UINT64_C(0xFFFF000000000000))), 16, 0);
    count   += _vanillc_crt_select((!(x & UINT64_C(0xFF00000000000000))), 8, 0);
    x       <<= _vanillc_crt_select((!(x & UINT64_C(0xFF00000000000000))), 8, 0);
    count   += _vanillc_crt_select((!(x & UINT64_C(0xF000000000000000))), 4, 0);
    x       <<= _vanillc_crt_select((!(x & UINT64_C(0xF000000000000000))), 4, 0);
    count   += _vanillc_crt_select((!(x & UINT64_C(0xC000000000000000))), 2, 0);
    x       <<= _vanillc_crt_select((!(x & UINT64_C(0xC000000000000000))), 2, 0);
    count += (!(x & UINT64_C(0x8000000000000000))); // MSB
    return _vanillc_crt_select(is_zero, 64, count);
}

uint8_t _crtintrin_ctz64(uint64_t x) {
    uint8_t count = 0, is_zero = (!x);
    count   += _vanillc_crt_select((!(x & UINT64_C(0x00000000FFFFFFFF))), 32, 0);
    x       >>= _vanillc_crt_select((!(x & UINT64_C(0x00000000FFFFFFFF))), 32, 0);
    count   += _vanillc_crt_select((!(x & UINT64_C(0x000000000000FFFF))), 16, 0);
    x       >>= _vanillc_crt_select((!(x & UINT64_C(0x000000000000FFFF))), 16, 0);
    count   += _vanillc_crt_select((!(x & UINT64_C(0x00000000000000FF))), 8, 0);
    x       >>= _vanillc_crt_select((!(x & UINT64_C(0x00000000000000FF))), 8, 0);
    count   += _vanillc_crt_select((!(x & UINT64_C(0x000000000000000F))), 4, 0);
    x       >>= _vanillc_crt_select((!(x & UINT64_C(0x000000000000000F))), 4, 0);
    count   += _vanillc_crt_select((!(x & UINT64_C(0x0000000000000003))), 2, 0);
    x       >>= _vanillc_crt_select((!(x & UINT64_C(0x0000000000000003))), 2, 0);
    count += (!(x & UINT64_C(0x0000000000000001))); // MSB
    return _vanillc_crt_select(is_zero, 64, count);
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
