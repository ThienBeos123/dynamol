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



#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#define U64_BITS 64

uint8_t _vanillc_crt_lti64(int64_t x, int64_t y) {
    int64_t sx = (x >> (U64_BITS - 1)) & 1;
    int64_t sy = (y >> (U64_BITS - 1)) & 1;
    int64_t sd = ((x - y) >> (U64_BITS - 1)) & 1;
    return (sx & !sy) | (!(sx ^ sy) & sd);
}
uint8_t _vanillc_crt_gti64(int64_t x, int64_t y) { return _vanillc_crt_lti64(y, x); }
uint8_t _vanillc_crt_eq(uint64_t x, uint64_t y) { return ~((x ^ y) | (-(x ^ y))) >> (U64_BITS - 1); }

// 1. Input Array X (32 elements)
const int64_t test_x[32] = {
    0, 0, 5, 10, 10, 20, -5, -5, -10, -5, -20, 0,
    -1, -1, 0, 1, INT64_MAX, INT64_MIN, INT64_MAX, INT64_MIN,
    INT64_MAX - 1, INT64_MIN + 1, 1234567890LL, -1234567890LL,
    -1234567890LL, 55555LL, -55555LL, INT64_MAX,
    INT64_MIN, -1, INT64_MAX, -9223372036854775807LL
};

// 2. Input Array Y (32 elements)
const int64_t test_y[32] = {
    0, 5, 0, 10, 20, 10, -5, -10, -5, -20, -10, -1,
    0, -1, -1, -1, INT64_MAX, INT64_MIN, INT64_MIN, INT64_MAX,
    INT64_MAX, INT64_MIN, 1234567890LL, 1234567890LL,
    -1234567890LL, -55555LL, 55555LL, -1, 1, INT64_MIN, 1, INT64_MAX
};

// 3. Expected Outputs for Less Than (x < y)
const bool expected_lt[32] = {
    false, true,  false, false, true,  false, false, false,
    true,  false, true,  false, true,  false, false, false,
    false, false, false, true, true,  false, false, true,
    false, false, true,  false, true,  false, false, true
};

// 4. Expected Outputs for Greater Than (x > y)
const bool expected_gt[32] = {
    false, false, true,  false, false, true,  false, true,
    false, true,  false, true, false, false, true,  true,
    false, false, true,  false, false, true,  false, false,
    false, true,  false, true, false, true,  true,  false
};

// 5. Expected Outputs for Equal To (x == y)
const bool expected_eq[32] = {
    true,  false, false, true, false, false, true,  false,
    false, false, false, false, false, true,  false, false,
    true,  true,  false, false, false, false, true,  false,
    true,  false, false, false, false, false, false, false
};

int main(void) {
    int passed = 0;
    for (int i = 0; i < 32; i++) {
        uint64_t x = test_x[i], y = test_y[i];
        bool lt_result = _vanillc_crt_lti64(x, y);
        bool gt_result = _vanillc_crt_gti64(x, y);
        bool eq_result = _vanillc_crt_eq(x, y);
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