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



#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>

#define U64_BITS ((sizeof(uint64_t)) * CHAR_BIT)

uint8_t crt_lt(uint64_t x, uint64_t y) { return (((x >> 63) ^ 1) & (y >> 63)) | (~((x >> 63) ^ (y >> 63)) & ((x - y) >> 63)); }
uint8_t crt_gt(uint64_t x, uint64_t y) { return crt_lt(y, x); }
uint8_t crt_eq(uint64_t x, uint64_t y) { return ~((x ^ y) | (-(x ^ y))) >> 63; }


// 1. Input Array X (32 elements)
const uint64_t test_x[32] = {
    0, 0, 5, 10, 10, 20, UINT64_MAX, UINT64_MAX, 0, UINT64_MAX, UINT64_MAX - 1,
    1ULL << 63, 0, (1ULL << 63) - 1, 1ULL << 63, 1ULL << 32, (1ULL << 32) - 1,
    1234567890ULL, 1234567890ULL, 9876543210ULL, UINT64_MAX >> 1, UINT64_MAX >> 1,
    (UINT64_MAX >> 1) + 1,0xAAAAAAAAAAAAAAAAULL, 0xAAAAAAAAAAAAAAAAULL,0x5555555555555555ULL,
    15, 16, UINT64_MAX, 1, 0x7FFFFFFFFFFFFFFFULL,0x8000000000000000ULL
};

// 2. Input Array Y (32 elements)
const uint64_t test_y[32] = {
    0, 5, 0, 10, 20, 10, UINT64_MAX, 0, UINT64_MAX, UINT64_MAX - 1, UINT64_MAX,
    0, 1ULL << 63, 1ULL << 63, (1ULL << 63) - 1, 1ULL << 32, 1ULL << 32, 1234567890ULL,
    9876543210ULL, 1234567890ULL, UINT64_MAX >> 1, (UINT64_MAX >> 1) + 1, UINT64_MAX >> 1,
    0xAAAAAAAAAAAAAAAAULL, 0x5555555555555555ULL, 0xAAAAAAAAAAAAAAAAULL, 16, 15,
    1, UINT64_MAX, 0x8000000000000000ULL, 0x7FFFFFFFFFFFFFFFULL
};

// 3. Expected Outputs for Less Than (x < y)
const bool expected_lt[32] = {
    false, true,  false, false, true,  false, false, false,
    true,  false, true,  false, true,  true,  false, false,
    true,  false, true,  false, false, true,  false, false,
    false, true,  true,  false, false, true,  true,  false
};

// 4. Expected Outputs for Greater Than (x > y)
const bool expected_gt[32] = {
    false, false, true,  false, false, true,  false, true,
    false, true,  false, true, false, false, true,  false,
    false, false, false, true, false, false, true,  false,
    true,  false, false, true, true,  false, false, true
};

// 5. Expected Outputs for Equal To (x == y)
const bool expected_eq[32] = {
    true,  false, false, true, false, false, true,  false,
    false, false, false, false, false, false, false, true,
    false, true,  false, false, true,  false, false, true,
    false, false, false, false, false, false, false, false
};


int main(void) {
    int passed = 0;
    for (int i = 0; i < 32; i++) {
        uint64_t x = test_x[i], y = test_y[i];
        bool lt_result = crt_lt(x, y);
        bool gt_result = crt_gt(x, y);
        bool eq_result = crt_eq(x, y);
        if (lt_result != expected_lt[i] || gt_result != expected_gt[i] || eq_result != expected_eq[i]) {
            printf("❌ Test Case %d Failed! Inputs: x=0x%llX, y=0x%llX\n", i + 1, (unsigned long long)x, (unsigned long long)y);
            printf("   LT Expected: %d, Got: %d\n", expected_lt[i], lt_result);
            printf("   GT Expected: %d, Got: %d\n", expected_gt[i], gt_result);
            printf("   EQ Expected: %d, Got: %d\n", expected_eq[i], eq_result);
        } else passed++;
    }
    printf("Validation complete: %d/32 constant-time cases passed.\n", passed);
    return (passed == 32) ? 0 : 1;
}
