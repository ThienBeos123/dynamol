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



// Core Types & Utilities
#include <include.h>
#include <dnml_status.h>
#include <dnml_sys/sys.h>
#include <libdnml_types.h>
// Memory Management and Algorithmic core
#include <_libdnml_config/numeric_config.h>
#include "../../../util/util.h"

typedef struct { bigInt x; bigInt y; int8_t expected; } cmp_case_t;

/* Static limbs for test cases */
static limb_t c_zero[1] = { UINT64_C(0x0000000000000000) };
static limb_t c_one[1]  = { UINT64_C(0x0000000000000001) };
static limb_t c_two[1]  = { UINT64_C(0x0000000000000002) };
static limb_t c_max[1]  = { UINT64_C(0xFFFFFFFFFFFFFFFF) };
static limb_t c_1_0[2]  = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000001) };
static limb_t c_1_1[2]  = { UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000001) };
static limb_t c_f_f[2]  = { UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF) };
static limb_t c_seq[3]  = { UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000002), UINT64_C(0x0000000000000003) };
static limb_t c_alt[3]  = { UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0x5555555555555555), UINT64_C(0xAAAAAAAAAAAAAAAA) };
static const cmp_case_t cases[] = {
    { { 0, 1, c_zero, 1 },  { 0, 1, c_zero, 1 },  0 }, // 0
    { { 0, 1, c_zero, 1 },  { 1, 1, c_one, 1 },  -1 }, // 1
    { { 1, 1, c_one, 1 },   { 0, 1, c_zero, 1 },   1 }, // 2
    { { 1, 1, c_one, 1 },   { 1, 1, c_one, 1 },    0 }, // 3
    { { 1, 1, c_one, 1 },   { 1, 1, c_two, 1 },   -1 }, // 4
    { { 1, 1, c_two, 1 },   { 1, 1, c_one, 1 },    1 }, // 5
    { { 1, 1, c_max, 1 },   { 1, 1, c_one, 1 },    1 }, // 6
    { { 2, 2, c_1_0, 1 },   { 1, 1, c_max, 1 },    1 }, // 7
    { { 1, 1, c_max, 1 },   { 2, 2, c_1_0, -1 }, -1 }, // 8
    { { 2, 2, c_1_1, 1 },   { 2, 2, c_1_0, 1 },    1 }, // 9
    { { 2, 2, c_1_0, 1 },   { 2, 2, c_1_1, 1 },   -1 }, // 10
    { { 2, 2, c_f_f, 1 },   { 2, 2, c_f_f, -1 },   0 }, // 11
    { { 3, 3, c_seq, 1 },   { 3, 3, c_seq, 1 },    0 }, // 12
    { { 3, 3, c_seq, 1 },   { 2, 2, c_f_f, 1 },    1 }, // 13
    { { 0, 1, c_zero, -1 }, { 0, 1, c_zero, 1 },   0 }, // 14
    { { 1, 1, c_one, -1 },  { 1, 1, c_one, 1 },    0 }, // 15
    { { 3, 3, c_alt, 1 },   { 3, 3, c_seq, 1 },    1 }, // 16
    { { 3, 3, c_seq, 1 },   { 3, 3, c_alt, 1 },   -1 }, // 17
    { { 1, 1, c_one, 1 },   { 1, 1, c_two, -1 }, -1 }, // 18
    { { 2, 2, c_1_0, 1 },   { 2, 2, c_1_0, 1 },    0 }, // 19
    { { 0, 1, c_zero, 1 },  { 0, 1, c_zero, -1 },  0 }, // 20
    { { 1, 1, c_max, 1 },   { 1, 1, c_max, 1 },    0 }, // 21
    { { 2, 2, c_f_f, 1 },   { 1, 1, c_max, 1 },    1 }, // 22
    { { 1, 1, c_max, 1 },   { 2, 2, c_f_f, 1 },   -1 }, // 23
    { { 3, 3, c_alt, 1 },   { 3, 3, c_alt, 1 },    0 }, // 24
    { { 0, 4, c_zero, 1 },  { 0, 4, c_zero, 1 },   0 }, // 25
    { { 1, 1, c_one, 1 },   { 0, 1, c_zero, 1 },   1 }, // 26
    { { 1, 1, c_two, 1 },   { 1, 1, c_two, 1 },    0 }, // 27
    { { 2, 2, c_1_1, 1 },   { 2, 2, c_1_1, 1 },    0 }, // 28
    { { 2, 2, c_1_1, 1 },   { 3, 3, c_seq, 1 },   -1 }, // 29
    { { 3, 3, c_seq, 1 },   { 2, 2, c_1_1, 1 },    1 }, // 30
    { { 1, 1, c_max, 1 },   { 1, 1, c_two, 1 },    1 }, // 31
    { { 1, 1, c_two, 1 },   { 1, 1, c_max, 1 },   -1 }, // 32
    { { 2, 2, c_f_f, 1 },   { 3, 3, c_alt, 1 },   -1 }, // 33
    { { 3, 3, c_alt, 1 },   { 2, 2, c_f_f, 1 },    1 }, // 34
    { { 0, 1, c_zero, 1 },  { 1, 1, c_max, 1 },   -1 }, // 35
    { { 1, 1, c_max, 1 },   { 0, 1, c_zero, 1 },   1 }, // 36
    { { 2, 2, c_1_0, 1 },   { 2, 2, c_f_f, 1 },   -1 }, // 37
    { { 2, 2, c_f_f, 1 },   { 2, 2, c_1_0, 1 },    1 }, // 38
    { { 1, 1, c_one, 1 },   { 1, 1, c_one, 1 },    0 }, // 39
    { { 1, 1, c_two, -1 },  { 1, 1, c_one, 1 },    1 }, // 40
    { { 1, 1, c_one, 1 },   { 1, 1, c_two, -1 }, -1 }, // 41
    { { 0, 1, c_zero, 1 },  { 0, 1, c_zero, 1 },   0 }, // 42
    { { 3, 3, c_seq, 1 },   { 3, 3, c_seq, 1 },    0 }, // 43
    { { 3, 3, c_seq, -1 },  { 3, 3, c_alt, 1 },   -1 }, // 44
    { { 3, 3, c_alt, 1 },   { 3, 3, c_seq, -1 },   1 }, // 45
    { { 1, 1, c_max, 1 },   { 1, 1, c_max, -1 },   0 }, // 46
    { { 2, 2, c_1_0, 1 },   { 1, 1, c_one, 1 },    1 }, // 47
    { { 1, 1, c_one, 1 },   { 2, 2, c_1_0, 1 },   -1 }, // 48
    { { 2, 2, c_1_1, 1 },   { 2, 2, c_1_1, 1 },    0 }, // 49
    { { 3, 3, c_alt, 1 },   { 3, 3, c_alt, 1 },    0 }, // 50
    { { 0, 2, c_zero, 1 },  { 1, 2, c_one, 1 },   -1 }, // 51
    { { 1, 2, c_one, 1 },   { 0, 2, c_zero, 1 },   1 }, // 52
    { { 2, 2, c_f_f, 1 },   { 2, 2, c_f_f, 1 },    0 }, // 53
    { { 1, 1, c_one, 1 },   { 1, 1, c_max, 1 },   -1 }, // 54
    { { 1, 1, c_max, 1 },   { 1, 1, c_one, 1 },    1 }, // 55
    { { 2, 2, c_1_0, 1 },   { 2, 2, c_1_0, 1 },    0 }, // 56
    { { 3, 3, c_seq, 1 },   { 0, 1, c_zero, 1 },   1 }, // 57
    { { 0, 1, c_zero, 1 },  { 3, 3, c_seq, 1 },   -1 }, // 58
    { { 1, 1, c_two, 1 },   { 1, 1, c_two, 1 },    0 }  // 59
};

int main(void) {
    int total_tests = 0, passed_tests = 0;
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    /* Allocate heap buffers for comparison operands */
    const size_t max_limbs = 16;
    limb_t *x_buf = (limb_t *)malloc(max_limbs * sizeof(limb_t));
    limb_t *y_buf = (limb_t *)malloc(max_limbs * sizeof(limb_t));

    printf("===================================================================\n");
    printf("     RUNNING INTEGRATED UNIT TESTS - BIGNUM MAGNITUDE COMPARISON   \n");
    printf("===================================================================\n");
    printf("---- __BIGINT_INTERNAL_COMP__ -----\n");
    int num_cases = sizeof(cases) / sizeof(cmp_case_t);
    for (int i = 0; i < num_cases; i++) { total_tests++;
        const cmp_case_t *c = &cases[i];
        /* Setup local bigInts with heap-backed memory */
        bigInt tx = c->x; tx.limbs = x_buf; tx.cap = max_limbs;
        bigInt ty = c->y; ty.limbs = y_buf; ty.cap = max_limbs;
        if (c->x.n > 0) memcpy(x_buf, c->x.limbs, c->x.n * sizeof(limb_t));
        if (c->y.n > 0) memcpy(y_buf, c->y.limbs, c->y.n * sizeof(limb_t));
        int8_t res = __BIGINT_INTERNAL_COMP__(&tx, &ty);
        if (res == c->expected) passed_tests++;
        else printf(
            "Case: %2d | x = { .n = %zu, .sign = %2d }, "
            "y = { .n = %zu, .sign = %2d } | exp: %2d | ret: %2d\n",
            i, c->x.n, c->x.sign, c->y.n, c->y.sign, c->expected, res
        );
    }

    free(x_buf); free(y_buf); clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("=========================================================\n");
    printf("TEST SUMMARY:\n");
    printf("+) Passed %-4d out of %-4d total compiled checks.\n", passed_tests, total_tests);
    printf("+) Success rate: %.2f%%\n", (passed_tests * 100.0) / total_tests);
    printf("+) Total Runtime: %lf ms\n", elapsed_time * 1000.0);
    printf("=========================================================\n");
    return (passed_tests == total_tests) ? 0 : 1;
}
