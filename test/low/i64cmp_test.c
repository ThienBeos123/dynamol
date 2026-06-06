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

#include <include.h>
#include <stdio.h>
#include <dnml_sys/sys.h>
#include <_libdnml_config/numeric_config.h>
#include "../../intrinsics/arm64/__arm64_conn__.h"
#include "../../intrinsics/risc-v64/__rv64_conn__.h"
#include "../../intrinsics/x86_64/__x86_conn__.h"
#include "../../intrinsics/crt_vanillc/__crt_vanillc_con__.h"

/* ============================== TEST STRUCT TYPE & TEST ARRAY ============================== */
typedef struct { int64_t x, y; uint8_t lt_exp, gt_exp, leq_exp, geq_exp; } test_case;
static const test_case global_bank[100] = {
    {INT64_C(0x0000000000000000), INT64_C(0x0000000000000000), 0, 0, 1, 1}, // Case 1
    {INT64_C(0x0000000000000001), INT64_C(0x0000000000000001), 0, 0, 1, 1}, // Case 2
    {INT64_C(0xFFFFFFFFFFFFFFFF), INT64_C(0xFFFFFFFFFFFFFFFF), 0, 0, 1, 1}, // Case 3
    {INT64_C(0x7FFFFFFFFFFFFFFF), INT64_C(0x7FFFFFFFFFFFFFFF), 0, 0, 1, 1}, // Case 4
    {INT64_C(0x8000000000000000), INT64_C(0x8000000000000000), 0, 0, 1, 1}, // Case 5
    {INT64_C(0x00000000075BCD15), INT64_C(0x00000000075BCD15), 0, 0, 1, 1}, // Case 6
    {INT64_C(0xFFFFFFFFFA6432EB), INT64_C(0xFFFFFFFFFA6432EB), 0, 0, 1, 1}, // Case 7
    {INT64_C(0x0000000000000000), INT64_C(0x0000000000000001), 1, 0, 1, 0}, // Case 8
    {INT64_C(0x0000000000000001), INT64_C(0x0000000000000002), 1, 0, 1, 0}, // Case 9
    {INT64_C(0x00000000000F4240), INT64_C(0x00000000001E8480), 1, 0, 1, 0}, // Case 10
    {INT64_C(0x7FFFFFFFFFFFFFFE), INT64_C(0x7FFFFFFFFFFFFFFF), 1, 0, 1, 0}, // Case 11
    {INT64_C(0x0000000000000001), INT64_C(0x7FFFFFFFFFFFFFFF), 1, 0, 1, 0}, // Case 12
    {INT64_C(0x0000000000000001), INT64_C(0x0000000000000000), 0, 1, 0, 1}, // Case 13
    {INT64_C(0x0000000000000002), INT64_C(0x0000000000000001), 0, 1, 0, 1}, // Case 14
    {INT64_C(0x00000000001E8480), INT64_C(0x00000000000F4240), 0, 1, 0, 1}, // Case 15
    {INT64_C(0x7FFFFFFFFFFFFFFF), INT64_C(0x7FFFFFFFFFFFFFFE), 0, 1, 0, 1}, // Case 16
    {INT64_C(0x7FFFFFFFFFFFFFFF), INT64_C(0x0000000000000001), 0, 1, 0, 1}, // Case 17
    {INT64_C(0xFFFFFFFFFFFFFFFF), INT64_C(0x0000000000000000), 1, 0, 1, 0}, // Case 18
    {INT64_C(0xFFFFFFFFFFFFFFFE), INT64_C(0xFFFFFFFFFFFFFFFF), 1, 0, 1, 0}, // Case 19
    {INT64_C(0xFFFFFFFFFFE17B80), INT64_C(0xFFFFFFFFFFF0BDC0), 1, 0, 1, 0}, // Case 20
    {INT64_C(0x8000000000000000), INT64_C(0xFFFFFFFFFFFFFFFF), 1, 0, 1, 0}, // Case 21
    {INT64_C(0x8000000000000000), INT64_C(0x0000000000000000), 1, 0, 1, 0}, // Case 22
    {INT64_C(0x0000000000000000), INT64_C(0xFFFFFFFFFFFFFFFF), 0, 1, 0, 1}, // Case 23
    {INT64_C(0xFFFFFFFFFFFFFFFF), INT64_C(0xFFFFFFFFFFFFFFFE), 0, 1, 0, 1}, // Case 24
    {INT64_C(0xFFFFFFFFFFF0BDC0), INT64_C(0xFFFFFFFFFFE17B80), 0, 1, 0, 1}, // Case 25
    {INT64_C(0xFFFFFFFFFFFFFFFF), INT64_C(0x8000000000000000), 0, 1, 0, 1}, // Case 26
    {INT64_C(0x0000000000000000), INT64_C(0x8000000000000000), 0, 1, 0, 1}, // Case 27
    {INT64_C(0xFFFFFFFFFFFFFFFF), INT64_C(0x0000000000000001), 1, 0, 1, 0}, // Case 28
    {INT64_C(0xFFFFFFFFFFFFFFFF), INT64_C(0x0000000000000000), 1, 0, 1, 0}, // Case 29
    {INT64_C(0x0000000000000000), INT64_C(0x0000000000000001), 1, 0, 1, 0}, // Case 30
    {INT64_C(0xFFFFFFFFFFFFFFF6), INT64_C(0x000000000000000A), 1, 0, 1, 0}, // Case 31
    {INT64_C(0xFFFFFFFFFFFFFC18), INT64_C(0x0000000000000001), 1, 0, 1, 0}, // Case 32
    {INT64_C(0x0000000000000001), INT64_C(0xFFFFFFFFFFFFFFFF), 0, 1, 0, 1}, // Case 33
    {INT64_C(0x0000000000000001), INT64_C(0xFFFFFFFFFFFFFC18), 0, 1, 0, 1}, // Case 34
    {INT64_C(0x0000000000000000), INT64_C(0xFFFFFFFFFFFFFFFF), 0, 1, 0, 1}, // Case 35
    {INT64_C(0x000000000000000A), INT64_C(0xFFFFFFFFFFFFFFF6), 0, 1, 0, 1}, // Case 36
    {INT64_C(0x0000000000000001), INT64_C(0x0000000000000000), 0, 1, 0, 1}, // Case 37
    {INT64_C(0x8000000000000000), INT64_C(0x7FFFFFFFFFFFFFFF), 1, 0, 1, 0}, // Case 38
    {INT64_C(0x8000000000000001), INT64_C(0x7FFFFFFFFFFFFFFF), 1, 0, 1, 0}, // Case 39
    {INT64_C(0x8000000000000000), INT64_C(0x7FFFFFFFFFFFFFFE), 1, 0, 1, 0}, // Case 40
    {INT64_C(0x7FFFFFFFFFFFFFFF), INT64_C(0x8000000000000000), 0, 1, 0, 1}, // Case 41
    {INT64_C(0x7FFFFFFFFFFFFFFF), INT64_C(0x8000000000000001), 0, 1, 0, 1}, // Case 42
    {INT64_C(0x7FFFFFFFFFFFFFFE), INT64_C(0x8000000000000000), 0, 1, 0, 1}, // Case 43
    {INT64_C(0x0000000000000005), INT64_C(0x000000000000000A), 1, 0, 1, 0}, // Case 44
    {INT64_C(0x000000000000000A), INT64_C(0x0000000000000005), 0, 1, 0, 1}, // Case 45
    {INT64_C(0xFFFFFFFFFFFFFFFB), INT64_C(0xFFFFFFFFFFFFFFF6), 0, 1, 0, 1}, // Case 46
    {INT64_C(0xFFFFFFFFFFFFFFF6), INT64_C(0xFFFFFFFFFFFFFFFB), 1, 0, 1, 0}, // Case 47
    {INT64_C(0x0000000012345678), INT64_C(0x0000000012345679), 1, 0, 1, 0}, // Case 48
    {INT64_C(0x0000000012345679), INT64_C(0x0000000012345678), 0, 1, 0, 1}, // Case 49
    {INT64_C(0xFFFFFFFFEDCBA988), INT64_C(0xFFFFFFFFEDCBA987), 0, 1, 0, 1}, // Case 50
    {INT64_C(0xFFFFFFFFEDCBA987), INT64_C(0xFFFFFFFFEDCBA988), 1, 0, 1, 0}, // Case 51
    {INT64_C(0x123456789ABCDEF0), INT64_C(0x123456789ABCDEF1), 1, 0, 1, 0}, // Case 52
    {INT64_C(0x123456789ABCDEF1), INT64_C(0x123456789ABCDEF0), 0, 1, 0, 1}, // Case 53
    {INT64_C(0xEDCBA9876543210F), INT64_C(0xEDCBA9876543210E), 0, 1, 0, 1}, // Case 54
    {INT64_C(0xEDCBA9876543210E), INT64_C(0xEDCBA9876543210F), 1, 0, 1, 0}, // Case 55
    {INT64_C(0x0000000000000000), INT64_C(0x7FFFFFFFFFFFFFFF), 1, 0, 1, 0}, // Case 56
    {INT64_C(0x7FFFFFFFFFFFFFFF), INT64_C(0x0000000000000000), 0, 1, 0, 1}, // Case 57
    {INT64_C(0x0000000000000000), INT64_C(0x8000000000000000), 0, 1, 0, 1}, // Case 58
    {INT64_C(0x8000000000000000), INT64_C(0x0000000000000000), 1, 0, 1, 0}, // Case 59
    {INT64_C(0xFFFFFFFFFFFFFFFF), INT64_C(0x7FFFFFFFFFFFFFFF), 1, 0, 1, 0}, // Case 60
    {INT64_C(0x7FFFFFFFFFFFFFFF), INT64_C(0xFFFFFFFFFFFFFFFF), 0, 1, 0, 1}, // Case 61
    {INT64_C(0xFFFFFFFFFFFFFFFF), INT64_C(0x8000000000000000), 0, 1, 0, 1}, // Case 62
    {INT64_C(0x8000000000000000), INT64_C(0xFFFFFFFFFFFFFFFF), 1, 0, 1, 0}, // Case 63
    {INT64_C(0x3FFFFFFFFFFFFFFF), INT64_C(0x4000000000000000), 1, 0, 1, 0}, // Case 64
    {INT64_C(0x4000000000000000), INT64_C(0x3FFFFFFFFFFFFFFF), 0, 1, 0, 1}, // Case 65
    {INT64_C(0xC000000000000000), INT64_C(0xC000000000000001), 1, 0, 1, 0}, // Case 66
    {INT64_C(0xC000000000000001), INT64_C(0xC000000000000000), 0, 1, 0, 1}, // Case 67
    {INT64_C(0xC000000000000000), INT64_C(0x4000000000000000), 1, 0, 1, 0}, // Case 68
    {INT64_C(0x4000000000000000), INT64_C(0xC000000000000000), 0, 1, 0, 1}, // Case 69
    {INT64_C(0x000000000000FFFF), INT64_C(0x0000000000010000), 1, 0, 1, 0}, // Case 70
    {INT64_C(0x0000000000010000), INT64_C(0x000000000000FFFF), 0, 1, 0, 1}, // Case 71
    {INT64_C(0xFFFFFFFFFFF00000), INT64_C(0xFFFFFFFFFFF00001), 1, 0, 1, 0}, // Case 72
    {INT64_C(0xFFFFFFFFFFF00001), INT64_C(0xFFFFFFFFFFF00000), 0, 1, 0, 1}, // Case 73
    {INT64_C(0x0000000000000001), INT64_C(0x1000000000000000), 1, 0, 1, 0}, // Case 74
    {INT64_C(0x1000000000000000), INT64_C(0x0000000000000001), 0, 1, 0, 1}, // Case 75
    {INT64_C(0xFFFFFFFFFFFFFFFE), INT64_C(0x7FFFFFFFFFFFFFFF), 1, 0, 1, 0}, // Case 76
    {INT64_C(0x7FFFFFFFFFFFFFFF), INT64_C(0xFFFFFFFFFFFFFFFE), 0, 1, 0, 1}, // Case 77
    {INT64_C(0x8000000000000001), INT64_C(0xFFFFFFFFFFFFFFFE), 1, 0, 1, 0}, // Case 78
    {INT64_C(0xFFFFFFFFFFFFFFFE), INT64_C(0x8000000000000001), 0, 1, 0, 1}, // Case 79
    {INT64_C(0x0000000011111111), INT64_C(0x0000000022222222), 1, 0, 1, 0}, // Case 80
    {INT64_C(0x0000000022222222), INT64_C(0x0000000011111111), 0, 1, 0, 1}, // Case 81
    {INT64_C(0xEEEEEEEEEEEEEEEE), INT64_C(0xFFFFFFFFFFFFFFFF), 1, 0, 1, 0}, // Case 82
    {INT64_C(0xFFFFFFFFFFFFFFFF), INT64_C(0xEEEEEEEEEEEEEEEE), 0, 1, 0, 1}, // Case 83
    {INT64_C(0x7555555555555555), INT64_C(0x7666666666666666), 1, 0, 1, 0}, // Case 84
    {INT64_C(0x7666666666666666), INT64_C(0x7555555555555555), 0, 1, 0, 1}, // Case 85
    {INT64_C(0x9999999999999999), INT64_C(0x8888888888888888), 0, 1, 0, 1}, // Case 86
    {INT64_C(0x8888888888888888), INT64_C(0x9999999999999999), 1, 0, 1, 0}, // Case 87
    {INT64_C(0x0000000000000002), INT64_C(0x0000000000000004), 1, 0, 1, 0}, // Case 88
    {INT64_C(0x0000000000000004), INT64_C(0x0000000000000002), 0, 1, 0, 1}, // Case 89
    {INT64_C(0xFFFFFFFFFFFFFFFC), INT64_C(0xFFFFFFFFFFFFFFFE), 1, 0, 1, 0}, // Case 90
    {INT64_C(0xFFFFFFFFFFFFFFFE), INT64_C(0xFFFFFFFFFFFFFFFC), 0, 1, 0, 1}, // Case 91
    {INT64_C(0x2222222222222222), INT64_C(0x3333333333333333), 1, 0, 1, 0}, // Case 92
    {INT64_C(0x3333333333333333), INT64_C(0x2222222222222222), 0, 1, 0, 1}, // Case 93
    {INT64_C(0xDDDDDDDDDDDDDDDD), INT64_C(0xCCCCCCCCCCCCCCCC), 0, 1, 0, 1}, // Case 94
    {INT64_C(0xCCCCCCCCCCCCCCCC), INT64_C(0xDDDDDDDDDDDDDDDD), 1, 0, 1, 0}, // Case 95
    {INT64_C(0x0000000000000000), INT64_C(0xFFFFFFFFFFFFFFFE), 0, 1, 0, 1}, // Case 96
    {INT64_C(0xFFFFFFFFFFFFFFFE), INT64_C(0x0000000000000000), 1, 0, 1, 0}, // Case 97
    {INT64_C(0x7FFFFFFFFFFFFFFF), INT64_C(0xFFFFFFFFFFFFFFFE), 0, 1, 0, 1}, // Case 98
    {INT64_C(0xFFFFFFFFFFFFFFFE), INT64_C(0x7FFFFFFFFFFFFFFF), 1, 0, 1, 0}, // Case 99
    {INT64_C(0x8000000000000000), INT64_C(0xFFFFFFFFFFFFFFFE), 1, 0, 1, 0}  // Case 100
};

int main(void) {
    size_t passed_tests = 0, total_tests = 0; uint8_t res;
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    printf("=========================================================\n");
    printf("      RUNNING INTEGRATED UNIT TESTS - SIGNED COMP        \n");
    printf("=========================================================\n");
    /* =============== Test: x < y =============== */
    printf("--- Testing LTI64 (<) Architectures ---\n");
    for (size_t i = 0; i < 100; ++i) {
        int64_t x = global_bank[i].x;
        int64_t y = global_bank[i].y;
        uint8_t lt_exp = global_bank[i].lt_exp;
        #if __ARCH_ARM64__
            res = _arm64_crt_lti64(x, y);
            total_tests++; if (res == lt_exp) passed_tests++;
            else printf(
                "[FAIL] _arm64_crt_lti64    | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, (uint64_t)y, lt_exp, res
            );
            res = _vanillc_crt_lti64(x, y);
            total_tests++; if (res == lt_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_lti64  | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, (uint64_t)y, lt_exp, res
            );
        #elif __ARCH_X86_64__
            res = _x86_crt_lti64(x, y);
            total_tests++; if (res == lt_exp) passed_tests++;
            else printf(
                "[FAIL] _x86_crt_lti64      | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, (uint64_t)y, lt_exp, res
            );
            res = _vanillc_crt_lti64(x, y);
            total_tests++; if (res == lt_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_lti64  | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, (uint64_t)y, lt_exp, res
            );
        #elif __ARCH_RVI64__
            res = _rv64_crt_lti64(x, y);
            total_tests++; if (res == lt_exp) passed_tests++;
            else printf(
                "[FAIL] _rv64_crt_lti64     | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, (uint64_t)y, lt_exp, res
            );
            res = _vanillc_crt_lti64(x, y);
            total_tests++; if (res == lt_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_lti64  | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, (uint64_t)y, lt_exp, res
            );
        #else
            res = _vanillc_crt_lti64(x, y);
            total_tests++; if (res == lt_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_lti64  | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, (uint64_t)y, lt_exp, res
            );
        #endif
    }
    /* =============== Test: x > y =============== */
    printf("\n--- Testing GTI64 (>) Architectures ---\n");
    for (size_t i = 0; i < 100; ++i) {
        int64_t x = global_bank[i].x;
        int64_t y = global_bank[i].y;
        uint8_t gt_exp = global_bank[i].gt_exp;
        #if __ARCH_ARM64__
            res = _arm64_crt_gti64(x, y);
            total_tests++; if (res == gt_exp) passed_tests++;
            else printf(
                "[FAIL] _arm64_crt_gti64    | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, (uint64_t)y, gt_exp, res
            );
            res = _vanillc_crt_gti64(x, y);
            total_tests++; if (res == gt_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_gti64  | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, (uint64_t)y, gt_exp, res
            );
        #elif __ARCH_X86_64__
            res = _x86_crt_gti64(x, y);
            total_tests++; if (res == gt_exp) passed_tests++;
            else printf(
                "[FAIL] _x86_crt_gti64      | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, (uint64_t)y, gt_exp, res
            );
            res = _vanillc_crt_gti64(x, y);
            total_tests++; if (res == gt_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_gti64  | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, (uint64_t)y, gt_exp, res
            );
        #elif __ARCH_RVI64__
            res = _rv64_crt_gti64(x, y);
            total_tests++; if (res == gt_exp) passed_tests++;
            else printf(
                "[FAIL] _rv64_crt_gti64     | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, (uint64_t)y, gt_exp, res
            );
            res = _vanillc_crt_gti64(x, y);
            total_tests++; if (res == gt_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_gti64  | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, (uint64_t)y, gt_exp, res
            );
        #else
            res = _vanillc_crt_gti64(x, y);
            total_tests++; if (res == gt_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_gti64  | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, (uint64_t)y, gt_exp, res
            );
        #endif
    }
    /* =============== Test: x <= y =============== */
    printf("\n--- Testing LEQI64 (<=) Architectures ---\n");
    for (size_t i = 0; i < 100; ++i) {
        int64_t x = global_bank[i].x;
        int64_t y = global_bank[i].y;
        uint8_t leq_exp = global_bank[i].leq_exp;
        #if __ARCH_ARM64__
            res = _arm64_crt_leqi64(x, y);
            total_tests++; if (res == leq_exp) passed_tests++;
            else printf(
                "[FAIL] _arm64_crt_leqi64   | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, (uint64_t)y, leq_exp, res                            
            );
            res = _vanillc_crt_leqi64(x, y);
            total_tests++; if (res == leq_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_leqi64 | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, (uint64_t)y, leq_exp, res
            );
        #elif __ARCH_X86_64__
            res = _x86_crt_leqi64(x, y);
            total_tests++; if (res == leq_exp) passed_tests++;
            else printf(
                "[FAIL] _x86_crt_leqi64     | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, (uint64_t)y, leq_exp, res
            );
            res = _vanillc_crt_leqi64(x, y);
            total_tests++; if (res == leq_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_leqi64 | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, (uint64_t)y, leq_exp, res
            );
        #elif __ARCH_RVI64__
            res = _rv64_crt_leqi64(x, y);
            total_tests++; if (res == leq_exp) passed_tests++;
            else printf(
                "[FAIL] _rv64_crt_leqi64    | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, (uint64_t)y, leq_exp, res
            );
            res = _vanillc_crt_leqi64(x, y);
            total_tests++; if (res == leq_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_leqi64 | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, (uint64_t)y, leq_exp, res
            );
        #else
            res = _vanillc_crt_leqi64(x, y);
            total_tests++; if (res == leq_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_leqi64 | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, (uint64_t)y, leq_exp, res
            );
        #endif
    }
    /* =============== Test: x >= y =============== */
    printf("\n--- Testing GEQ (>=) Architectures ---\n");
    for (size_t i = 0; i < 100; ++i) {
        int64_t x = global_bank[i].x;
        int64_t y = global_bank[i].y;
        uint8_t geq_exp = global_bank[i].geq_exp;
        #if __ARCH_ARM64__
            res = _arm64_crt_geqi64(x, y);
            total_tests++; if (res == geq_exp) passed_tests++;
            else printf(
                "[FAIL] _arm64_crt_geqi64   | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, (uint64_t)y, geq_exp, res
            );
            res = _vanillc_crt_geqi64(x, y);
            total_tests++; if (res == geq_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_geqi64 | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, (uint64_t)y, geq_exp, res
            );
        #elif __ARCH_X86_64__
            res = _x86_crt_geqi64(x, y);
            total_tests++; if (res == geq_exp) passed_tests++;
            else printf(
                "[FAIL] _x86_crt_geqi64     | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, (uint64_t)y, geq_exp, res
            );
            res = _vanillc_crt_geqi64(x, y);
            total_tests++; if (res == geq_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_geqi64 | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, (uint64_t)y, geq_exp, res
            );
        #elif __ARCH_RVI64__
            res = _rv64_crt_geqi64(x, y);
            total_tests++; if (res == geq_exp) passed_tests++;
            else printf(
                "[FAIL] _rv64_crt_geqi64    | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, (uint64_t)y, geq_exp, res
            );
            res = _vanillc_crt_geqi64(x, y);
            total_tests++; if (res == geq_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_geqi64 | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, (uint64_t)y, geq_exp, res
            );
        #else
            res = _vanillc_crt_geqi64(x, y);
            total_tests++; if (res == geq_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_geqi64 | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, (uint64_t)y, geq_exp, res
            );
        #endif
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("\n=============== Test Summary ===============\n");
    printf("Total tests: %3zu\n", total_tests);
    printf("Passed: %3zu\n", passed_tests);
    printf("Success rate: %.2f%%\n", (passed_tests * 100.0) / total_tests);
    printf("Total Runtime: %lf ms\n", elapsed_time);
    return (passed_tests == total_tests) ? 0 : 1;
}
