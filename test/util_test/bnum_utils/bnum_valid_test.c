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
#include "../../../libdnml_base.h"
typedef struct { bigInt x; uint8_t valid, pvalid, svalid; } case_t;
/* Memory-backed limbs for arithmetic/storage validation */
static limb_t case_51[1] = { UINT64_C(0x0000000000000001) };
static limb_t case_52[3] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000001) };
static limb_t case_53[1] = { UINT64_C(0x0000000000000000) };
static limb_t case_54[2] = { UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000000) };
static limb_t case_55[4] = {
    UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), 
    UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF)
};
static limb_t dummy_limb[1] = { UINT64_C(0x0000000000000000) };

/* Global Array of Edge cases */
static const case_t cases[] = {
    /* 50 Structural/Pointer Cases (No memory access required for invalidity) */
    { { 0, 0, NULL, 1 }, 0, 0, 0 }, // 0
    { { 1, 0, NULL, 1 }, 0, 0, 0 }, // 1
    { { 0, 10, NULL, 1 }, 0, 0, 0 }, // 2
    { { 5, 2, dummy_limb, 1 }, 0, 0, 1 }, // 3
    { { 1, 1, dummy_limb, 0 }, 0, 0, 1 }, // 4
    { { 1, 1, dummy_limb, 2 }, 0, 0, 1 }, // 5
    { { 1, 1, dummy_limb, -2 }, 0, 0, 1 }, // 6
    { { 1, 1, dummy_limb, 127 }, 0, 0, 1 }, // 7
    { { 0, 0, dummy_limb, 1 }, 0, 0, 0 }, // 8
    { { 10, 5, dummy_limb, -1 }, 0, 0, 1 }, // 9
    { { 1, 1, NULL, -1 }, 0, 0, 0 }, // 10
    { { 0, 1, NULL, 1 }, 0, 0, 0 }, // 11
    { { 1, 1, dummy_limb, 5 }, 0, 0, 1 }, // 12
    { { 2, 1, dummy_limb, 1 }, 0, 0, 1 }, // 13
    { { 0, 0, NULL, -1 }, 0, 0, 0 }, // 14
    { { 100, 10, dummy_limb, 1 }, 0, 0, 1 }, // 15
    { { 1, 0, dummy_limb, 1 }, 0, 0, 0 }, // 16
    { { 0, 1, dummy_limb, 0 }, 0, 0, 1 }, // 17
    { { 5, 5, NULL, 1 }, 0, 0, 0 }, // 18
    { { 0, 0, dummy_limb, -1 }, 0, 0, 0 }, // 19
    { { 1, 1, dummy_limb, 100 }, 0, 0, 1 }, // 20
    { { 1, 1, dummy_limb, -100 }, 0, 0, 1 }, // 21
    { { 2, 2, NULL, 1 }, 0, 0, 0 }, // 22
    { { 3, 3, NULL, -1 }, 0, 0, 0 }, // 23
    { { 4, 1, dummy_limb, 1 }, 0, 0, 1 }, // 24
    { { 0, 1, dummy_limb, -5 }, 0, 0, 1 }, // 25
    { { 0, 5, NULL, 1 }, 0, 0, 0 }, // 26
    { { 1, 5, NULL, -1 }, 0, 0, 0 }, // 27
    { { 5, 1, dummy_limb, 1 }, 0, 0, 1 }, // 28
    { { 1, 1, dummy_limb, 9 }, 0, 0, 1 }, // 29
    { { 0, 0, NULL, 1 }, 0, 0, 0 }, // 30
    { { 10, 1, dummy_limb, 1 }, 0, 0, 1 }, // 31
    { { 0, 1, dummy_limb, 120 }, 0, 0, 1 }, // 32
    { { 1, 2, NULL, 1 }, 0, 0, 0 }, // 33
    { { 2, 1, dummy_limb, -1 }, 0, 0, 1 }, // 34
    { { 0, 0, dummy_limb, 1 }, 0, 0, 0 }, // 35
    { { 1, 1, dummy_limb, 42 }, 0, 0, 1 }, // 36
    { { 1, 1, dummy_limb, -42 }, 0, 0, 1 }, // 37
    { { 0, 2, NULL, -1 }, 0, 0, 0 }, // 38
    { { 5, 0, dummy_limb, 1 }, 0, 0, 0 }, // 39
    { { 0, 10, NULL, 1 }, 0, 0, 0 }, // 40
    { { 1, 1, dummy_limb, 3 }, 0, 0, 1 }, // 41
    { { 1, 1, dummy_limb, -3 }, 0, 0, 1 }, // 42
    { { 8, 4, dummy_limb, 1 }, 0, 0, 1 }, // 43
    { { 0, 1, NULL, 1 }, 0, 0, 0 }, // 44
    { { 1, 0, NULL, -1 }, 0, 0, 0 }, // 45
    { { 1, 1, dummy_limb, 88 }, 0, 0, 1 }, // 46
    { { 0, 0, NULL, 1 }, 0, 0, 0 }, // 47
    { { 2, 1, dummy_limb, 1 }, 0, 0, 1 }, // 48
    { { 1, 1, dummy_limb, -7 }, 0, 0, 1 }, // 49
    { { 0, 1, dummy_limb, 1 }, 1, 1, 1 }, // 50
    
    /* 30 Arithmetic/Memory Cases */
    { { 1, 1, case_51, 1 }, 1, 1, 1 }, // 51
    { { 3, 3, case_52, 1 }, 1, 1, 1 }, // 52
    { { 1, 1, case_53, 1 }, 0, 1, 1 }, // 53
    { { 2, 2, case_54, 1 }, 0, 1, 1 }, // 54
    { { 4, 4, case_55, -1 }, 1, 1, 1 }, // 55
    { { 1, 1, case_53, -1 }, 0, 1, 1 }, // 56
    { { 0, 1, dummy_limb, -1 }, 0, 1, 1 }, // 57
    { { 0, 5, dummy_limb, 1 }, 1, 1, 1 }, // 58
    { { 1, 1, case_51, -1 }, 1, 1, 1 }, // 59
    { { 1, 1, case_53, 1 }, 0, 1, 1 }, // 60
    { { 3, 5, case_52, -1 }, 1, 1, 1 }, // 61
    { { 1, 2, case_53, -1 }, 0, 1, 1 }, // 62
    { { 2, 10, case_54, 1 }, 0, 1, 1 }, // 63
    { { 4, 4, case_55, 1 }, 1, 1, 1 }, // 64
    { { 1, 1, case_53, 1 }, 0, 1, 1 }, // 65
    { { 0, 1, dummy_limb, 1 }, 1, 1, 1 }, // 66
    { { 1, 1, case_51, 1 }, 1, 1, 1 }, // 67
    { { 2, 2, case_54, -1 }, 0, 1, 1 }, // 68
    { { 3, 3, case_52, 1 }, 1, 1, 1 }, // 69
    { { 1, 1, case_53, -1 }, 0, 1, 1 }, // 70
    { { 0, 2, dummy_limb, 1 }, 1, 1, 1 }, // 71
    { { 4, 10, case_55, -1 }, 1, 1, 1 }, // 72
    { { 1, 1, case_53, 1 }, 0, 1, 1 }, // 73
    { { 2, 2, case_54, 1 }, 0, 1, 1 }, // 74
    { { 1, 1, case_51, -1 }, 1, 1, 1 }, // 75
    { { 3, 4, case_52, -1 }, 1, 1, 1 }, // 76
    { { 1, 1, case_53, 1 }, 0, 1, 1 }, // 77
    { { 0, 1, dummy_limb, -1 }, 0, 1, 1 }, // 78
    { { 1, 1, case_51, 1 }, 1, 1, 1 }  // 79
};

int main(void) {
    _libdnml_init();
    int num_cases = sizeof(cases) / sizeof(case_t);
    int total_tests = 0, passed_tests = 0;
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    printf("===================================================================\n");
    printf("     RUNNING INTEGRATED UNIT TESTS - BIGNUM VALIDATION UTILITIES   \n");
    printf("===================================================================\n");
    printf("---- __BIGINT_INTERNAL_VALID__ -----\n");
    for (int i = 0; i < num_cases; i++) {
        total_tests++;
        uint8_t res = __BIGINT_INTERNAL_VALID__(&cases[i].x);
        if (res == cases[i].valid) passed_tests++;
        else printf(
            "Case: %2d |  Input Struct = { .limbs = SEE case_%02d, .n = %zu, "
            ".cap = %zu, .sign = %d } | exp: %" PRIu8 " | ret: %" PRIu8 "\n", 
            i, i, cases[i].x.n, cases[i].x.cap, cases[i].x.sign, cases[i].valid, res
        );
    }
    printf("---- __BIGINT_INTERNAL_PVALID__ -----\n");
    for (int i = 0; i < num_cases; i++) {
        total_tests++;
        uint8_t res = __BIGINT_INTERNAL_PVALID__(&cases[i].x);
        if (res == cases[i].pvalid) passed_tests++;
        else printf(
            "Case: %2d |  Input Struct = { .limbs = SEE case_%02d, .n = %zu, "
            ".cap = %zu, .sign = %d } | exp: %" PRIu8 " | ret: %" PRIu8 "\n", 
            i, i, cases[i].x.n, cases[i].x.cap, cases[i].x.sign, cases[i].pvalid, res
        );
    }
    printf("---- __BIGINT_INTERNAL_SVALID__ -----\n");
    for (int i = 0; i < num_cases; i++) {
        total_tests++;
        uint8_t res = __BIGINT_INTERNAL_SVALID__(&cases[i].x);
        if (res == cases[i].svalid) passed_tests++;
        else printf(
            "Case: %2d |  Input Struct = { .limbs = SEE case_%02d, .n = %zu, "
            ".cap = %zu, .sign = %d } | exp: %" PRIu8 " | ret: %" PRIu8 "\n", 
            i, i, cases[i].x.n, cases[i].x.cap, cases[i].x.sign, cases[i].svalid, res
        );
    }

    // Summary output block
    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("=========================================================\n");
    printf("TEST SUMMARY:\n");
    printf("+) Passed %-4d out of %-4d total compiled checks.\n", passed_tests, total_tests);
    printf("+) Success rate: %.2f%%\n", (passed_tests * 100.0) / total_tests);
    printf("+) Total Runtime: %lf ms\n", elapsed_time * 1000.0);
    printf("=========================================================\n");
    _libdnml_cleanup(); return (passed_tests == total_tests) ? 0 : 1;
}
