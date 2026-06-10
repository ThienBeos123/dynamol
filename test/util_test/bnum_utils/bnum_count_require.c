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
typedef struct { size_t bits; size_t limb_out; } limbs_needed_case;
typedef struct { const bigInt x; uint64_t threshold; uint8_t out; } will_overflow_case;
/* ========== GLOBAL ARRAY OF LIMBS_NEEDED CASES ========== */
static const limbs_needed_case global_lncases[100] = {
    {0, 0},   {1, 1},   {2, 1},   {3, 1},   {4, 1},   {5, 1},   {6, 1},   {7, 1},   {8, 1},   {9, 1},
    {10, 1},  {11, 1},  {12, 1},  {13, 1},  {14, 1},  {15, 1},  {16, 1},  {17, 1},  {18, 1},  {19, 1},
    {20, 1},  {21, 1},  {22, 1},  {23, 1},  {24, 1},  {25, 1},  {26, 1},  {27, 1},  {28, 1},  {29, 1},
    {30, 1},  {31, 1},  {32, 1},  {33, 1},  {34, 1},  {35, 1},  {36, 1},  {37, 1},  {38, 1},  {39, 1},
    {40, 1},  {41, 1},  {42, 1},  {43, 1},  {44, 1},  {45, 1},  {46, 1},  {47, 1},  {48, 1},  {49, 1},
    {50, 1},  {51, 1},  {52, 1},  {53, 1},  {54, 1},  {55, 1},  {56, 1},  {57, 1},  {58, 1},  {59, 1},
    {60, 1},  {61, 1},  {62, 1},  {63, 1},  {64, 1},  {65, 2},  {66, 2},  {67, 2},  {68, 2},  {69, 2},
    {70, 2},  {71, 2},  {72, 2},  {73, 2},  {74, 2},  {75, 2},  {76, 2},  {77, 2},  {78, 2},  {79, 2},
    {80, 2},  {81, 2},  {82, 2},  {83, 2},  {84, 2},  {85, 2},  {86, 2},  {87, 2},  {88, 2},  {89, 2},
    {90, 2},  {91, 2},  {92, 2},  {93, 2},  {94, 2},  {95, 2},  {96, 2},  {97, 2},  {98, 2},  {99, 2}
};
/* ========== GLOBAL ARRAY OF WILL_OVERFLOW CASES ========== */
static const limb_t limbs_zero[] = {0};
static const limb_t limbs_one[] = {5ULL};
static const limb_t limbs_two[] = {0x1111111111111111ULL, 0x2222222222222222ULL};
static const limb_t limbs_max[] = {0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL};
static const limb_t limbs_12b[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0xAAAAAAAAAAAAAAAAULL};
static const will_overflow_case global_wocases[40] = {
    // Group 1: Size n == 0 Cases (Should always return 0 to prevent underflow crashes)
    { { 0, 1, (limb_t *)limbs_zero, }, 0x0000000000000000ULL, 0 },
    { { 0, 1, (limb_t *)limbs_zero, }, 0x0000000000000001ULL, 0 },
    { { 0, 1, (limb_t *)limbs_zero, }, 0x00000000000000FFULL, 0 },
    { { 0, 1, (limb_t *)limbs_zero, }, 0x000000000000FFFFULL, 0 },
    { { 0, 1, (limb_t *)limbs_zero, }, 0x00000000FFFFFFFFULL, 0 },
    { { 0, 2, (limb_t *)limbs_zero, }, 0x1000000000000000ULL, 0 },
    { { 0, 4, (limb_t *)limbs_zero, }, 0x7FFFFFFFFFFFFFFFULL, 0 },
    { { 0, 8, (limb_t *)limbs_zero, }, 0xFFFFFFFFFFFFFFFEULL, 0 },
    { { 0, 12, (limb_t *)limbs_zero, }, 0xFFFFFFFFFFFFFFFFULL, 0 },
    { { 0, 0, NULL }, 0x123456789ABCDEF0ULL, 0 },
    // Group 2: Size n < Cap Cases (Should always return 0 as headroom exists)
    { { 1, 2, (limb_t *)limbs_one },  0x0000000000000000ULL, 0 },
    { { 1, 2, (limb_t *)limbs_one },  0x0000000000000004ULL, 0 },
    { { 1, 2, (limb_t *)limbs_one },  0x0000000000000005ULL, 0 },
    { { 1, 2, (limb_t *)limbs_one },  0xFFFFFFFFFFFFFFFFULL, 0 },
    { { 1, 2, (limb_t *)limbs_two, },  0x0000000000000000ULL, 0 },
    { { 1, 3, (limb_t *)limbs_two, },  0x1111111111111110ULL, 0 },
    { { 1, 2, (limb_t *)limbs_max },  0x0000000000000000ULL, 0 },
    { { 5, 12, (limb_t *)limbs_12b }, 0x0000000000000000ULL, 0 },
    { { 11, 12, (limb_t *)limbs_12b },0x0000000000000000ULL, 0 },
    { { 1, 4, (limb_t *)limbs_max },  0xFFFFFFFFFFFFFFFEULL, 0 },
    // Group 3: Size n == Cap Cases where Highest Limb <= Threshold (No Overflow expected)
    { { 1, 1, (limb_t *)limbs_zero, },  0x0000000000000000ULL, 0 },
    { { 1, 1, (limb_t *)limbs_zero, },  0x0000000000000001ULL, 0 },
    { { 1, 1, (limb_t *)limbs_one },  0x0000000000000005ULL, 0 },
    { { 1, 1, (limb_t *)limbs_one },  0x0000000000000006ULL, 0 },
    { { 2, 2, (limb_t *)limbs_two, },  0x2222222222222222ULL, 0 },
    { { 2, 2, (limb_t *)limbs_two, },  0x2222222222222223ULL, 0 },
    { { 2, 2, (limb_t *)limbs_two, },  0xFFFFFFFFFFFFFFFFULL, 0 },
    { { 2, 2, (limb_t *)limbs_max, },  0xFFFFFFFFFFFFFFFFULL, 0 },
    { { 12, 12, (limb_t *)limbs_12b, },0xAAAAAAAAAAAAAAAAULL, 0 },
    { { 12, 12, (limb_t *)limbs_12b, },0xFFFFFFFFFFFFFFFFULL, 0 },
    // Group 4: Size n == Cap Cases where Highest Limb > Threshold (Overflow will trigger)
    { { 1, 1, (limb_t *)limbs_one },  0x0000000000000000ULL, 1 },
    { { 1, 1, (limb_t *)limbs_one },  0x0000000000000004ULL, 1 },
    { { 2, 2, (limb_t *)limbs_two, },  0x0000000000000000ULL, 1 },
    { { 2, 2, (limb_t *)limbs_two, },  0x2222222222222221ULL, 1 },
    { { 2, 2, (limb_t *)limbs_max, },  0x0000000000000000ULL, 1 },
    { { 2, 2, (limb_t *)limbs_max, },  0xFFFFFFFFFFFFFFFEULL, 1 },
    { { 12, 12, (limb_t *)limbs_12b, },0x0000000000000000ULL, 1 },
    { { 12, 12, (limb_t *)limbs_12b, },0xAAAAAAAAAAAAAAAAULL - 1, 1 },
    { { 1, 1, (limb_t *)limbs_max },  0x7FFFFFFFFFFFFFFFULL, 1 },
    { { 2, 2, (limb_t *)limbs_two, },  0x1111111111111111ULL, 1 }
};




int main(void) {
    int total_tests = 0, passed_tests = 0;
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    limb_t *l_buf = (limb_t *)malloc(12 * sizeof(limb_t));
    printf("===================================================================\n");
    printf("     RUNNING INTEGRATED UNIT TESTS - BIGNUM DIGIT/TZ UTILITIES     \n");
    printf("===================================================================\n");
    printf("---- __BIGINT_LIMBS_NEEDED__ -----\n");
    for (int i = 0; i < 100; i++) { total_tests++;
        size_t res = __BIGINT_LIMBS_NEEDED__(global_lncases[i].bits);
        if (res == global_lncases[i].limb_out) passed_tests++;
        else printf(
            "[FAIL] Case %-2d: Bits = %-4zu | Expected Limbs = %-2zu, Got = %-2zu\n", 
            i, global_lncases[i].bits, global_lncases[i].limb_out, res
        );
    }
    printf("---- __BIGINT_WILL_OVERFLOW__ -----\n");
    for (int i = 0; i < 40; i++) { total_tests++;
        size_t limbs_to_copy = global_wocases[i].x.n < 12 ? global_wocases[i].x.n : 12;
        if (global_wocases[i].x.limbs && limbs_to_copy > 0) {
            memcpy(l_buf, global_wocases[i].x.limbs, limbs_to_copy * sizeof(limb_t));
        }
        bigInt test_x = global_wocases[i].x;
        if (limbs_to_copy > 0) test_x.limbs = l_buf;
        uint8_t res = __BIGINT_WILL_OVERFLOW__(&test_x, global_wocases[i].threshold);
        if (res == global_wocases[i].out) passed_tests++;
        else printf(
            "[FAIL] Case %-2d: Size = %-2zu, Cap = %-2zu"
            ", Threshold = 0x%016" PRIu64 " | Expected = %u, Got = %u\n",
            i, global_wocases[i].x.n, global_wocases[i].x.cap, 
            global_wocases[i].threshold, global_wocases[i].out, res
        );
    }

    free(l_buf); clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("=========================================================\n");
    printf("TEST SUMMARY:\n");
    printf("+) Passed %-4d out of %-4d total compiled checks.\n", passed_tests, total_tests);
    printf("+) Success rate: %.2f%%\n", (passed_tests * 100.0) / total_tests);
    printf("+) Total Runtime: %lf ms\n", elapsed_time * 1000.0);
    printf("=========================================================\n");
    return (passed_tests == total_tests) ? 0 : 1;
    return 0;
}
