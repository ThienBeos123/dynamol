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
#include <include.h>
#include <_libdnml_config/numeric_config.h>
#include "../../intrinsics/crt_vanillc/__crt_vanillc_con__.h"
#include "../../intrinsics/zvanillc/__vanillc_conn__.h"
#include "../../intrinsics/arm64/__arm64_conn__.h"
#include "../../intrinsics/x86_64/__x86_conn__.h"
#include "../../intrinsics/risc-v64/__rv64_conn__.h"
// Case Struct
typedef struct {
    uint64_t lo, hi; uint64_t div;
    uint64_t quot, rem; uint8_t ovf_flag;
} wdiv_case_t;
#define ylw "\033[1;33m"
#define esc "\033[0m"

static const wdiv_case_t perf_cases[32] = {
    { UINT64_C(0x000000000000000A), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000002), UINT64_C(0x0000000000000005), UINT64_C(0x0000000000000000), 0 }, // 1
    { UINT64_C(0x000000000000000B), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000003), UINT64_C(0x0000000000000003), UINT64_C(0x0000000000000002), 0 }, // 2
    { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x000000000000FFFF), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), 0 }, // 3
    { UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000000), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000000), 0 }, // 4
    { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000002), UINT64_C(0x8000000000000000), UINT64_C(0x0000000000000000), 0 }, // 5
    { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000004), UINT64_C(0x0000000000000008), UINT64_C(0x8000000000000000), UINT64_C(0x0000000000000000), 0 }, // 6
    { UINT64_C(0x0000000000000000), UINT64_C(0x0000000010000000), UINT64_C(0x0000000020000000), UINT64_C(0x8000000000000000), UINT64_C(0x0000000000000000), 0 }, // 7
    { UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000002), UINT64_C(0x8000000000000000), UINT64_C(0x0000000000000001), 0 }, // 8
    { UINT64_C(0xFFFFFFFFFFFFFFFE), UINT64_C(0x7FFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x8000000000000000), UINT64_C(0x7FFFFFFFFFFFFFFE), 0 }, // 9
    { UINT64_C(0x5555555555555555), UINT64_C(0x1111111111111111), UINT64_C(0x2222222222222222), UINT64_C(0x8000000000000002), UINT64_C(0x1111111111111111), 0 }, // 10
    { UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0x3AAAAAAAAAAAAAAA), UINT64_C(0x7AAAAAAAAAAAAAAA), UINT64_C(0x7A6F4DE9BD37A6F5), UINT64_C(0x419F89467E2519F8), 0 }, // 11
    { UINT64_C(0x0000000000000000), UINT64_C(0x000000000000FFFF), UINT64_C(0x0000000000010000), UINT64_C(0xFFFF000000000000), UINT64_C(0x0000000000000000), 0 }, // 12
    { UINT64_C(0xFFFFFFFFFFFFFFDB), UINT64_C(0x0000000000000000), UINT64_C(0x000000000000000D), UINT64_C(0x13B13B13B13B13AE), UINT64_C(0x0000000000000005), 0 }, // 13
    { UINT64_C(0x0000000000000000), UINT64_C(0x0000FFFFFFFFFFFF), UINT64_C(0x0001000000000000), UINT64_C(0xFFFFFFFFFFFF0000), UINT64_C(0x0000000000000000), 0 }, // 14
    { UINT64_C(0x123456789ABCDEF0), UINT64_C(0x0000000000001234), UINT64_C(0x0000000000005678), UINT64_C(0x35E4A3CF0A2F1FB2), UINT64_C(0x0000000000003780), 0 }, // 15
    { UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFE), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFE), 0 }, // 16
    { UINT64_C(0xFFFFFFFFFFFFFFFE), UINT64_C(0xFFFFFFFFFFFFFFFE), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFD), 0 }, // 17
    { UINT64_C(0x8000000000000000), UINT64_C(0x3FFFFFFFFFFFFFFF), UINT64_C(0x7FFFFFFFFFFFFFFF), UINT64_C(0x8000000000000000), UINT64_C(0x0000000000000000), 0 }, // 18
    { UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x7FFFFFFFFFFFFFFF), UINT64_C(0x8000000000000000), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x7FFFFFFFFFFFFFFF), 0 }, // 19
    { UINT64_C(0x0000000000000007), UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000003), UINT64_C(0x5555555555555557), UINT64_C(0x0000000000000002), 0 }, // 20
    { UINT64_C(0x0F0F0F0F0F0F0F0F), UINT64_C(0x00F0F0F0F0F0F0F0), UINT64_C(0x0FFFFFFFFFFFFFFF), UINT64_C(0x0F0F0F0F0F0F0F01), UINT64_C(0x0E1E1E1E1E1E1E10), 0 }, // 21
    { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000080), UINT64_C(0x0000000000000100), UINT64_C(0x8000000000000000), UINT64_C(0x0000000000000000), 0 }, // 22
    { UINT64_C(0x0000000000000010), UINT64_C(0x0000000000000005), UINT64_C(0x000000000000000B), UINT64_C(0x745D1745D1745D18), UINT64_C(0x0000000000000008), 0 }, // 23
    { UINT64_C(0xFFFFFFFFFFFFFFFC), UINT64_C(0x3FFFFFFFFFFFFFFF), UINT64_C(0x3FFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000000), 1 }, // 24
    { UINT64_C(0x1111111111111111), UINT64_C(0x0000000000000001), UINT64_C(0x0000000011111111), UINT64_C(0x0000001000000010), UINT64_C(0x0000000000000001), 0 }, // 25
    { UINT64_C(0x123456789ABCDEF0), UINT64_C(0x0000000000000055), UINT64_C(0x0000000000000000), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000000), 1 }, // 26
    { UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000000), 1 }, // 27
    { UINT64_C(0x0000000000000005), UINT64_C(0x0000000000000010), UINT64_C(0x0000000000000010), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000000), 1 }, // 28
    { UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000020), UINT64_C(0x0000000000000010), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000000), 1 }, // 29
    { UINT64_C(0x0000000000000000), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000000), 1 }, // 30
    { UINT64_C(0x5555555555555555), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x5555555555555555), 1 }, // 31
    { UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000002), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000001), 1 } // 32
};
static const wdiv_case_t crt_cases[32] = {
    { UINT64_C(0x000000000000000A), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000002), UINT64_C(0x0000000000000005), UINT64_C(0x0000000000000000), 0 }, // 1
    { UINT64_C(0x000000000000000B), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000003), UINT64_C(0x0000000000000003), UINT64_C(0x0000000000000002), 0 }, // 2
    { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x000000000000FFFF), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), 0 }, // 3
    { UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000000), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000000), 0 }, // 4
    { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000002), UINT64_C(0x8000000000000000), UINT64_C(0x0000000000000000), 0 }, // 5
    { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000004), UINT64_C(0x0000000000000008), UINT64_C(0x8000000000000000), UINT64_C(0x0000000000000000), 0 }, // 6
    { UINT64_C(0x0000000000000000), UINT64_C(0x0000000010000000), UINT64_C(0x0000000020000000), UINT64_C(0x8000000000000000), UINT64_C(0x0000000000000000), 0 }, // 7
    { UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000002), UINT64_C(0x8000000000000000), UINT64_C(0x0000000000000001), 0 }, // 8
    { UINT64_C(0xFFFFFFFFFFFFFFFE), UINT64_C(0x7FFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x8000000000000000), UINT64_C(0x7FFFFFFFFFFFFFFE), 0 }, // 9
    { UINT64_C(0x5555555555555555), UINT64_C(0x1111111111111111), UINT64_C(0x2222222222222222), UINT64_C(0x8000000000000002), UINT64_C(0x1111111111111111), 0 }, // 10
    { UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0x3AAAAAAAAAAAAAAA), UINT64_C(0x7AAAAAAAAAAAAAAA), UINT64_C(0x7A6F4DE9BD37A6F5), UINT64_C(0x419F89467E2519F8), 0 }, // 11
    { UINT64_C(0x0000000000000000), UINT64_C(0x000000000000FFFF), UINT64_C(0x0000000000010000), UINT64_C(0xFFFF000000000000), UINT64_C(0x0000000000000000), 0 }, // 12
    { UINT64_C(0xFFFFFFFFFFFFFFDB), UINT64_C(0x0000000000000000), UINT64_C(0x000000000000000D), UINT64_C(0x13B13B13B13B13AE), UINT64_C(0x0000000000000005), 0 }, // 13
    { UINT64_C(0x0000000000000000), UINT64_C(0x0000FFFFFFFFFFFF), UINT64_C(0x0001000000000000), UINT64_C(0xFFFFFFFFFFFF0000), UINT64_C(0x0000000000000000), 0 }, // 14
    { UINT64_C(0x123456789ABCDEF0), UINT64_C(0x0000000000001234), UINT64_C(0x0000000000005678), UINT64_C(0x35E4A3CF0A2F1FB2), UINT64_C(0x0000000000003780), 0 }, // 15
    { UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFE), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFE), 0 }, // 16
    { UINT64_C(0xFFFFFFFFFFFFFFFE), UINT64_C(0xFFFFFFFFFFFFFFFE), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFD), 0 }, // 17
    { UINT64_C(0x8000000000000000), UINT64_C(0x3FFFFFFFFFFFFFFF), UINT64_C(0x7FFFFFFFFFFFFFFF), UINT64_C(0x8000000000000000), UINT64_C(0x0000000000000000), 0 }, // 18
    { UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x7FFFFFFFFFFFFFFF), UINT64_C(0x8000000000000000), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x7FFFFFFFFFFFFFFF), 0 }, // 10
    { UINT64_C(0x0000000000000007), UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000003), UINT64_C(0x5555555555555557), UINT64_C(0x0000000000000002), 0 }, // 20
    { UINT64_C(0x0F0F0F0F0F0F0F0F), UINT64_C(0x00F0F0F0F0F0F0F0), UINT64_C(0x0FFFFFFFFFFFFFFF), UINT64_C(0x0F0F0F0F0F0F0F01), UINT64_C(0x0E1E1E1E1E1E1E10), 0 }, // 21
    { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000080), UINT64_C(0x0000000000000100), UINT64_C(0x8000000000000000), UINT64_C(0x0000000000000000), 0 }, // 22
    { UINT64_C(0x0000000000000010), UINT64_C(0x0000000000000005), UINT64_C(0x000000000000000B), UINT64_C(0x745D1745D1745D18), UINT64_C(0x0000000000000008), 0 }, // 23
    { UINT64_C(0xFFFFFFFFFFFFFFFC), UINT64_C(0x3FFFFFFFFFFFFFFF), UINT64_C(0x3FFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000000), 1 }, // 24
    { UINT64_C(0x1111111111111111), UINT64_C(0x0000000000000001), UINT64_C(0x0000000011111111), UINT64_C(0x0000001000000010), UINT64_C(0x0000000000000001), 0 }, // 25
    { UINT64_C(0x123456789ABCDEF0), UINT64_C(0x0000000000000055), UINT64_C(0x0000000000000000), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000000), 1 }, // 26
    { UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000000), 1 }, // 27
    { UINT64_C(0x0000000000000005), UINT64_C(0x0000000000000010), UINT64_C(0x0000000000000010), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000000), 1 }, // 28
    { UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000020), UINT64_C(0x0000000000000010), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000000), 1 }, // 29
    { UINT64_C(0x0000000000000000), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000000), 1 }, // 30
    { UINT64_C(0x5555555555555555), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000000), 1 }, // 31
    { UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000002), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000000), 1 } // 32
};


/*
// Portable _crtintrin baseline
quot = _crtintrin_wdiv128(lo, hi, div, &rem, &ovf_check); total_tests++;
if (quot == exp_quot && rem == exp_rem && ovf_check == exp_ovf) passed_tests++;
else printf(
    "[FAIL] _crtintrin_wdiv128 "ylw"|"esc" Case %3d "ylw"|"esc" Lo: 0x%016" PRIX64 " Hi: 0x%016" PRIX64 " Div: 0x%016" PRIX64
    " "ylw"|||"esc" Equot: 0x%016" PRIX64 " Erem: 0x%016" PRIX64 " Eovf: %1" PRIu8
    " "ylw"|||"esc" Oquot: 0x%016" PRIX64 " Orem: 0x%016" PRIX64 " Oovf: %1" PRIu8 "\n", 
    i, lo, hi, div, exp_quot, exp_rem, exp_ovf, quot, rem, ovf_check
);
*/


int main(void) {
    struct timespec start, end;
    int total_tests = 0; int passed_tests = 0;
    clock_gettime(CLOCK_MONOTONIC, &start);
    printf("=========================================================\n");
    printf("     RUNNING INTEGRATED UNIT TESTS - WIDE DIVISION       \n");
    printf("=========================================================\n");
    // 1. TEST WIDE DIVISION - PERFORMANCE-BASED
    printf("--- Testing WDIV128 (PERF) Architectures ---\n");
    for (int i = 0; i < 32; i++) {
        uint64_t lo = perf_cases[i].lo;
        uint64_t hi = perf_cases[i].hi; 
        uint64_t div = perf_cases[i].div;
        uint64_t exp_quot = perf_cases[i].quot;
        uint64_t exp_rem = perf_cases[i].rem;
        uint8_t exp_ovf = perf_cases[i].ovf_flag;
        uint64_t quot; uint64_t rem; uint8_t ovf_check;

        // Portable _cintrin baseline
        quot = _cintrin_wdiv128(lo, hi, div, &rem, &ovf_check); total_tests++;
        if (quot == exp_quot && rem == exp_rem && ovf_check == exp_ovf) passed_tests++;
        else printf(
            "[FAIL] _cintrin_wdiv128   "ylw"|"esc" Case %3d "ylw"|"esc" Lo: 0x%016" PRIX64 " Hi: 0x%016" PRIX64 " Div: 0x%016" PRIX64
            " "ylw"|||"esc" Equot: 0x%016" PRIX64 " Erem: 0x%016" PRIX64 " Eovf: %1" PRIu8
            " "ylw"|||"esc" Oquot: 0x%016" PRIX64 " Orem: 0x%016" PRIX64 " Oovf: %1" PRIu8 "\n", 
            i + 1, lo, hi, div, exp_quot, exp_rem, exp_ovf, quot, rem, ovf_check
        );

        // Architecture Dispatches
        #if __ARCH_ARM64__
            quot = _arm64_wdiv128(lo, hi, div, &rem, &ovf_check); total_tests++;
            if (quot == exp_quot && rem == exp_rem && ovf_check == exp_ovf) passed_tests++;
            else printf(
                "[FAIL] _arm64_wdiv128     "ylw"|"esc" Case %3d "ylw"|"esc" Lo: 0x%016" PRIX64 " Hi: 0x%016" PRIX64 " Div: 0x%016" PRIX64
                " "ylw"|||"esc" Equot: 0x%016" PRIX64 " Erem: 0x%016" PRIX64 " Eovf: %1" PRIu8
                " "ylw"|||"esc" Oquot: 0x%016" PRIX64 " Orem: 0x%016" PRIX64 " Oovf: %1" PRIu8 "\n", 
                i + 1, lo, hi, div, exp_quot, exp_rem, exp_ovf, quot, rem, ovf_check
            );
        #elif __ARCH_X86_64__
            quot = _x86_wdiv128(lo, hi, div, &rem, &ovf_check); total_tests++;
            if (quot == exp_quot && rem == exp_rem && ovf_check == exp_ovf) passed_tests++;
            else printf(
                "[FAIL] _x86_wdiv128       "ylw"|"esc" Case %3d "ylw"|"esc" Lo: 0x%016" PRIX64 " Hi: 0x%016" PRIX64 " Div: 0x%016" PRIX64
                " "ylw"|||"esc" Equot: 0x%016" PRIX64 " Erem: 0x%016" PRIX64 " Eovf: %1" PRIu8
                " "ylw"|||"esc" Oquot: 0x%016" PRIX64 " Orem: 0x%016" PRIX64 " Oovf: %1" PRIu8 "\n", 
                i + 1, lo, hi, div, exp_quot, exp_rem, exp_ovf, quot, rem, ovf_check
            );
        #elif __ARCH_RVI64__
            quot = _rv64_wdiv128(lo, hi, div, &rem, &ovf_check); total_tests++;
            if (quot == exp_quot && rem == exp_rem && ovf_check == exp_ovf) passed_tests++;
            else printf(
                "[FAIL] _rv64_wdiv128      "ylw"|"esc" Case %3d "ylw"|"esc" Lo: 0x%016" PRIX64 " Hi: 0x%016" PRIX64 " Div: 0x%016" PRIX64
                " "ylw"|||"esc" Equot: 0x%016" PRIX64 " Erem: 0x%016" PRIX64 " Eovf: %1" PRIu8 "\n"
                " "ylw"|||"esc" Oquot: 0x%016" PRIX64 " Orem: 0x%016" PRIX64 " Oovf: %1" PRIu8 "\n", 
                i + 1, lo, hi, div, exp_quot, exp_rem, exp_ovf, quot, rem, ovf_check
            );
        #endif
    }


    // 1. TEST WIDE DIVISION - PERFORMANCE-BASED
    printf("--- Testing WDIV128 (CRYPTO) Architectures ---\n");
    for (int i = 0; i < 32; i++) {
        uint64_t lo = crt_cases[i].lo;
        uint64_t hi = crt_cases[i].hi; 
        uint64_t div = crt_cases[i].div;
        uint64_t exp_quot = crt_cases[i].quot;
        uint64_t exp_rem = crt_cases[i].rem;
        uint8_t exp_ovf = crt_cases[i].ovf_flag;
        uint64_t quot; uint64_t rem; uint8_t ovf_check;

        // Portable _cintrin baseline
        quot = _crtintrin_wdiv128(lo, hi, div, &rem, &ovf_check); total_tests++;
        if (quot == exp_quot && rem == exp_rem && ovf_check == exp_ovf) passed_tests++;
        else printf(
            "[FAIL] _crtintrin_wdiv128 "ylw"|"esc" Case %3d "ylw"|"esc" Lo: 0x%016" PRIX64 " Hi: 0x%016" PRIX64 " Div: 0x%016" PRIX64
            " "ylw"|||"esc" Equot: 0x%016" PRIX64 " Erem: 0x%016" PRIX64 " Eovf: %1" PRIu8
            " "ylw"|||"esc" Oquot: 0x%016" PRIX64 " Orem: 0x%016" PRIX64 " Oovf: %1" PRIu8 "\n", 
            i + 1, lo, hi, div, exp_quot, exp_rem, exp_ovf, quot, rem, ovf_check
        );

        // Architecture Dispatches
        #if __ARCH_ARM64__
            quot = _arm64_crt_wdiv128(lo, hi, div, &rem, &ovf_check); total_tests++;
            if (quot == exp_quot && rem == exp_rem && ovf_check == exp_ovf) passed_tests++;
            else printf(
                "[FAIL] _arm64_crt_wdiv128 "ylw"|"esc" Case %3d "ylw"|"esc" Lo: 0x%016" PRIX64 " Hi: 0x%016" PRIX64 " Div: 0x%016" PRIX64
                " "ylw"|||"esc" Equot: 0x%016" PRIX64 " Erem: 0x%016" PRIX64 " Eovf: %1" PRIu8
                " "ylw"|||"esc" Oquot: 0x%016" PRIX64 " Orem: 0x%016" PRIX64 " Oovf: %1" PRIu8 "\n", 
                i + 1, lo, hi, div, exp_quot, exp_rem, exp_ovf, quot, rem, ovf_check
            );
        #elif __ARCH_X86_64__
            quot = _x86_crt_wdiv128(lo, hi, div, &rem, &ovf_check); total_tests++;
            if (quot == exp_quot && rem == exp_rem && ovf_check == exp_ovf) passed_tests++;
            else printf(
                "[FAIL] _x86_crt_wdiv128   "ylw"|"esc" Case %3d "ylw"|"esc" Lo: 0x%016" PRIX64 " Hi: 0x%016" PRIX64 " Div: 0x%016" PRIX64
                " "ylw"|||"esc" Equot: 0x%016" PRIX64 " Erem: 0x%016" PRIX64 " Eovf: %1" PRIu8
                " "ylw"|||"esc" Oquot: 0x%016" PRIX64 " Orem: 0x%016" PRIX64 " Oovf: %1" PRIu8 "\n", 
                i + 1, lo, hi, div, exp_quot, exp_rem, exp_ovf, quot, rem, ovf_check
            );
        #elif __ARCH_RVI64__
            quot = _rv64_crt_wdiv128(lo, hi, div, &rem, &ovf_check); total_tests++;
            if (quot == exp_quot && rem == exp_rem && ovf_check == exp_ovf) passed_tests++;
            else printf(
                "[FAIL] _rv64_crt_wdiv128  "ylw"|"esc" Case %3d "ylw"|"esc" Lo: 0x%016" PRIX64 " Hi: 0x%016" PRIX64 " Div: 0x%016" PRIX64
                " "ylw"|||"esc" Equot: 0x%016" PRIX64 " Erem: 0x%016" PRIX64 " Eovf: %1" PRIu8 "\n"
                " "ylw"|||"esc" Oquot: 0x%016" PRIX64 " Orem: 0x%016" PRIX64 " Oovf: %1" PRIu8 "\n", 
                i + 1, lo, hi, div, exp_quot, exp_rem, exp_ovf, quot, rem, ovf_check
            );
        #endif
    }


    // Summary output block
    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("=========================================================\n");
    printf("TEST SUMMARY:\n");
    printf("+) Passed %-4d out of %-4d total compiled checks.\n", passed_tests, total_tests);
    printf("+) Success rate: %.2f%%\n", (passed_tests * 100.0) / total_tests);
    printf("+) Total Runtime: %lf ms\n", elapsed_time);
    printf("=========================================================\n");
    return (passed_tests == total_tests) ? 0 : 1;
}