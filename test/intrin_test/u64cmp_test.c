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
typedef struct { uint64_t x, y; uint8_t lt_exp, gt_exp, leq_exp, geq_exp; } test_case;
static const test_case global_bank[100] = {
    {UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), 0, 0, 1, 1}, // Case 1
    {UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000001), 0, 0, 1, 1}, // Case 2
    {UINT64_C(0x8000000000000000), UINT64_C(0x8000000000000000), 0, 0, 1, 1}, // Case 3
    {UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), 0, 0, 1, 1}, // Case 4
    {UINT64_C(0x0123456789ABCDEF), UINT64_C(0x0123456789ABCDEF), 0, 0, 1, 1}, // Case 5
    {UINT64_C(0xFEDCBA9876543210), UINT64_C(0xFEDCBA9876543210), 0, 0, 1, 1}, // Case 6
    {UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000001), 1, 0, 1, 0}, // Case 7
    {UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000002), 1, 0, 1, 0}, // Case 8
    {UINT64_C(0x7FFFFFFFFFFFFFFF), UINT64_C(0x8000000000000000), 1, 0, 1, 0}, // Case 9
    {UINT64_C(0xFFFFFFFFFFFFFFFE), UINT64_C(0xFFFFFFFFFFFFFFFF), 1, 0, 1, 0}, // Case 10
    {UINT64_C(0x0000000000000000), UINT64_C(0x8000000000000000), 1, 0, 1, 0}, // Case 11
    {UINT64_C(0x00000000FFFFFFFF), UINT64_C(0x0000000100000000), 1, 0, 1, 0}, // Case 12
    {UINT64_C(0x0100000000000000), UINT64_C(0x0200000000000000), 1, 0, 1, 0}, // Case 13
    {UINT64_C(0x00FFFFFFFFFFFFFF), UINT64_C(0x0100000000000000), 1, 0, 1, 0}, // Case 14
    {UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0xBBBBBBBBBBBBBBBB), 1, 0, 1, 0}, // Case 15
    {UINT64_C(0x5555555555555555), UINT64_C(0xAAAAAAAAAAAAAAAA), 1, 0, 1, 0}, // Case 16
    {UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000000), 0, 1, 0, 1}, // Case 17
    {UINT64_C(0x0000000000000002), UINT64_C(0x0000000000000001), 0, 1, 0, 1}, // Case 18
    {UINT64_C(0x8000000000000000), UINT64_C(0x7FFFFFFFFFFFFFFF), 0, 1, 0, 1}, // Case 19
    {UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFE), 0, 1, 0, 1}, // Case 20
    {UINT64_C(0x8000000000000000), UINT64_C(0x0000000000000000), 0, 1, 0, 1}, // Case 21
    {UINT64_C(0x0000000100000000), UINT64_C(0x00000000FFFFFFFF), 0, 1, 0, 1}, // Case 22
    {UINT64_C(0x0200000000000000), UINT64_C(0x0100000000000000), 0, 1, 0, 1}, // Case 23
    {UINT64_C(0x0100000000000000), UINT64_C(0x00FFFFFFFFFFFFFF), 0, 1, 0, 1}, // Case 24
    {UINT64_C(0xBBBBBBBBBBBBBBBB), UINT64_C(0xAAAAAAAAAAAAAAAA), 0, 1, 0, 1}, // Case 25
    {UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0x5555555555555555), 0, 1, 0, 1}, // Case 26
    {UINT64_C(0x0000000000000000), UINT64_C(0xFFFFFFFFFFFFFFFF), 1, 0, 1, 0}, // Case 27
    {UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000000), 0, 1, 0, 1}, // Case 28
    {UINT64_C(0x0000000000000001), UINT64_C(0xFFFFFFFFFFFFFFFF), 1, 0, 1, 0}, // Case 29
    {UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000001), 0, 1, 0, 1}, // Case 30
    {UINT64_C(0x7FFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), 1, 0, 1, 0}, // Case 31
    {UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x7FFFFFFFFFFFFFFF), 0, 1, 0, 1}, // Case 32
    {UINT64_C(0x8000000000000000), UINT64_C(0xFFFFFFFFFFFFFFFF), 1, 0, 1, 0}, // Case 33
    {UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x8000000000000000), 0, 1, 0, 1}, // Case 34
    {UINT64_C(0x0000000000000001), UINT64_C(0x8000000000000000), 1, 0, 1, 0}, // Case 35
    {UINT64_C(0x8000000000000000), UINT64_C(0x0000000000000001), 0, 1, 0, 1}, // Case 36
    {UINT64_C(0x0000000000000002), UINT64_C(0x0000000000000004), 1, 0, 1, 0}, // Case 37
    {UINT64_C(0x0000000000000004), UINT64_C(0x0000000000000002), 0, 1, 0, 1}, // Case 38
    {UINT64_C(0x0000000000000008), UINT64_C(0x0000000000000010), 1, 0, 1, 0}, // Case 39
    {UINT64_C(0x0000000000000010), UINT64_C(0x0000000000000008), 0, 1, 0, 1}, // Case 40
    {UINT64_C(0x0000000000000020), UINT64_C(0x0000000000000040), 1, 0, 1, 0}, // Case 41
    {UINT64_C(0x0000000000000040), UINT64_C(0x0000000000000020), 0, 1, 0, 1}, // Case 42
    {UINT64_C(0x0000000000000080), UINT64_C(0x0000000000000100), 1, 0, 1, 0}, // Case 43
    {UINT64_C(0x0000000000000100), UINT64_C(0x0000000000000080), 0, 1, 0, 1}, // Case 44
    {UINT64_C(0x0000000000000200), UINT64_C(0x0000000000000400), 1, 0, 1, 0}, // Case 45
    {UINT64_C(0x0000000000000400), UINT64_C(0x0000000000000200), 0, 1, 0, 1}, // Case 46
    {UINT64_C(0x0000000000000800), UINT64_C(0x0000000000001000), 1, 0, 1, 0}, // Case 47
    {UINT64_C(0x0000000000001000), UINT64_C(0x0000000000000800), 0, 1, 0, 1}, // Case 48
    {UINT64_C(0x0000000000002000), UINT64_C(0x0000000000004000), 1, 0, 1, 0}, // Case 49
    {UINT64_C(0x0000000000004000), UINT64_C(0x0000000000002000), 0, 1, 0, 1}, // Case 50
    {UINT64_C(0x0000000000008000), UINT64_C(0x0000000000010000), 1, 0, 1, 0}, // Case 51
    {UINT64_C(0x0000000000010000), UINT64_C(0x0000000000008000), 0, 1, 0, 1}, // Case 52
    {UINT64_C(0x0000000000020000), UINT64_C(0x0000000000040000), 1, 0, 1, 0}, // Case 53
    {UINT64_C(0x0000000000040000), UINT64_C(0x0000000000020000), 0, 1, 0, 1}, // Case 54
    {UINT64_C(0x0000000000080000), UINT64_C(0x0000000000100000), 1, 0, 1, 0}, // Case 55
    {UINT64_C(0x0000000000100000), UINT64_C(0x0000000000080000), 0, 1, 0, 1}, // Case 56
    {UINT64_C(0x0000000000200000), UINT64_C(0x0000000000400000), 1, 0, 1, 0}, // Case 57
    {UINT64_C(0x0000000000400000), UINT64_C(0x0000000000200000), 0, 1, 0, 1}, // Case 58
    {UINT64_C(0x0000000000800000), UINT64_C(0x0000000001000000), 1, 0, 1, 0}, // Case 59
    {UINT64_C(0x0000000001000000), UINT64_C(0x0000000000800000), 0, 1, 0, 1}, // Case 60
    {UINT64_C(0x0000000002000000), UINT64_C(0x0000000004000000), 1, 0, 1, 0}, // Case 61
    {UINT64_C(0x0000000004000000), UINT64_C(0x0000000002000000), 0, 1, 0, 1}, // Case 62
    {UINT64_C(0x0000000008000000), UINT64_C(0x0000000010000000), 1, 0, 1, 0}, // Case 63
    {UINT64_C(0x0000000010000000), UINT64_C(0x0000000008000000), 0, 1, 0, 1}, // Case 64
    {UINT64_C(0x0000000020000000), UINT64_C(0x0000000040000000), 1, 0, 1, 0}, // Case 65
    {UINT64_C(0x0000000040000000), UINT64_C(0x0000000020000000), 0, 1, 0, 1}, // Case 66
    {UINT64_C(0x0000000080000000), UINT64_C(0x0000000100000000), 1, 0, 1, 0}, // Case 67
    {UINT64_C(0x0000000100000000), UINT64_C(0x0000000080000000), 0, 1, 0, 1}, // Case 68
    {UINT64_C(0x0000000200000000), UINT64_C(0x0000000400000000), 1, 0, 1, 0}, // Case 69
    {UINT64_C(0x0000000400000000), UINT64_C(0x0000000200000000), 0, 1, 0, 1}, // Case 70
    {UINT64_C(0x0000000800000000), UINT64_C(0x0000001000000000), 1, 0, 1, 0}, // Case 71
    {UINT64_C(0x0000001000000000), UINT64_C(0x0000000800000000), 0, 1, 0, 1}, // Case 72
    {UINT64_C(0x0000002000000000), UINT64_C(0x0000004000000000), 1, 0, 1, 0}, // Case 73
    {UINT64_C(0x0000004000000000), UINT64_C(0x0000002000000000), 0, 1, 0, 1}, // Case 74
    {UINT64_C(0x0000008000000000), UINT64_C(0x0000010000000000), 1, 0, 1, 0}, // Case 75
    {UINT64_C(0x0000010000000000), UINT64_C(0x0000008000000000), 0, 1, 0, 1}, // Case 76
    {UINT64_C(0x0000020000000000), UINT64_C(0x0000040000000000), 1, 0, 1, 0}, // Case 77
    {UINT64_C(0x0000040000000000), UINT64_C(0x0000020000000000), 0, 1, 0, 1}, // Case 78
    {UINT64_C(0x0000080000000000), UINT64_C(0x0000100000000000), 1, 0, 1, 0}, // Case 79
    {UINT64_C(0x0000100000000000), UINT64_C(0x0000080000000000), 0, 1, 0, 1}, // Case 80
    {UINT64_C(0x0000200000000000), UINT64_C(0x0000400000000000), 1, 0, 1, 0}, // Case 81
    {UINT64_C(0x0000400000000000), UINT64_C(0x0000200000000000), 0, 1, 0, 1}, // Case 82
    {UINT64_C(0x0000800000000000), UINT64_C(0x0001000000000000), 1, 0, 1, 0}, // Case 83
    {UINT64_C(0x0001000000000000), UINT64_C(0x0000800000000000), 0, 1, 0, 1}, // Case 84
    {UINT64_C(0x0002000000000000), UINT64_C(0x0004000000000000), 1, 0, 1, 0}, // Case 85
    {UINT64_C(0x0004000000000000), UINT64_C(0x0002000000000000), 0, 1, 0, 1}, // Case 86
    {UINT64_C(0x0008000000000000), UINT64_C(0x0010000000000000), 1, 0, 1, 0}, // Case 87
    {UINT64_C(0x0010000000000000), UINT64_C(0x0008000000000000), 0, 1, 0, 1}, // Case 88
    {UINT64_C(0x0020000000000000), UINT64_C(0x0040000000000000), 1, 0, 1, 0}, // Case 89
    {UINT64_C(0x0040000000000000), UINT64_C(0x0020000000000000), 0, 1, 0, 1}, // Case 90
    {UINT64_C(0x0080000000000000), UINT64_C(0x0100000000000000), 1, 0, 1, 0}, // Case 91
    {UINT64_C(0x0100000000000000), UINT64_C(0x0080000000000000), 0, 1, 0, 1}, // Case 92
    {UINT64_C(0x0200000000000000), UINT64_C(0x0400000000000000), 1, 0, 1, 0}, // Case 93
    {UINT64_C(0x0400000000000000), UINT64_C(0x0200000000000000), 0, 1, 0, 1}, // Case 94
    {UINT64_C(0x0800000000000000), UINT64_C(0x1000000000000000), 1, 0, 1, 0}, // Case 95
    {UINT64_C(0x1000000000000000), UINT64_C(0x0800000000000000), 0, 1, 0, 1}, // Case 96
    {UINT64_C(0x2000000000000000), UINT64_C(0x4000000000000000), 1, 0, 1, 0}, // Case 97
    {UINT64_C(0x4000000000000000), UINT64_C(0x2000000000000000), 0, 1, 0, 1}, // Case 98
    {UINT64_C(0x4000000000000000), UINT64_C(0xFFFFFFFFFFFFFFFF), 1, 0, 1, 0}, // Case 99
    {UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x4000000000000000), 0, 1, 0, 1}  // Case 100
};

int main(void) {
    size_t passed_tests = 0, total_tests = 0; uint8_t res;
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    printf("=========================================================\n");
    printf("     RUNNING INTEGRATED UNIT TESTS - UNSIGNED COMP       \n");
    printf("=========================================================\n");
    /* =============== Test: x < y =============== */
    printf("--- Testing LT (<) Architectures ---\n");
    for (size_t i = 0; i < 100; ++i) {
        uint64_t x = global_bank[i].x;
        uint64_t y = global_bank[i].y;
        uint8_t lt_exp = global_bank[i].lt_exp;
        #if __ARCH_ARM64__
            res = _arm64_crt_lt(x, y);
            total_tests++; if (res == lt_exp) passed_tests++;
            else printf(
                "[FAIL] _arm64_crt_lt       | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, lt_exp, res
            );
            res = _vanillc_crt_lt(x, y);
            total_tests++; if (res == lt_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_lt     | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, lt_exp, res
            );
        #elif __ARCH_X86_64__
            res = _x86_crt_lt(x, y);
            total_tests++; if (res == lt_exp) passed_tests++;
            else printf(
                "[FAIL] _x86_crt_lt         | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, lt_exp, res
            );
            res = _vanillc_crt_lt(x, y);
            total_tests++; if (res == lt_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_lt     | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, lt_exp, res
            );
        #elif __ARCH_RVI64__
            res = _rv64_crt_lt(x, y);
            total_tests++; if (res == lt_exp) passed_tests++;
            else printf(
                "[FAIL] _rv64_crt_lt        | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, lt_exp, res
            );
            res = _vanillc_crt_lt(x, y);
            total_tests++; if (res == lt_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_lt     | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, lt_exp, res
            );
        #else
            res = _vanillc_crt_lt(x, y);
            total_tests++; if (res == lt_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_lt     | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, lt_exp, res
            );
        #endif
    }
    /* =============== Test: x > y =============== */
    printf("--- Testing GT (>) Architectures ---\n");
    for (size_t i = 0; i < 100; ++i) {
        uint64_t x = global_bank[i].x;
        uint64_t y = global_bank[i].y;
        uint8_t gt_exp = global_bank[i].gt_exp;
        #if __ARCH_ARM64__
            res = _arm64_crt_gt(x, y);
            total_tests++; if (res == gt_exp) passed_tests++;
            else printf(
                "[FAIL] _arm64_crt_gt       | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, gt_exp, res
            );
            res = _vanillc_crt_gt(x, y);
            total_tests++; if (res == gt_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_gt     | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, gt_exp, res
            );
        #elif __ARCH_X86_64__
            res = _x86_crt_gt(x, y);
            total_tests++; if (res == gt_exp) passed_tests++;
            else printf(
                "[FAIL] _x86_crt_gt         | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, gt_exp, res
            );
            res = _vanillc_crt_gt(x, y);
            total_tests++; if (res == gt_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_gt     | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, gt_exp, res
            );
        #elif __ARCH_RVI64__
            res = _rv64_crt_gt(x, y);
            total_tests++; if (res == gt_exp) passed_tests++;
            else printf(
                "[FAIL] _rv64_crt_gt        | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, gt_exp, res
            );
            res = _vanillc_crt_gt(x, y);
            total_tests++; if (res == gt_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_gt     | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, gt_exp, res
            );
        #else
            res = _vanillc_crt_gt(x, y);
            total_tests++; if (res == gt_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_gt     | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, gt_exp, res
            );
        #endif
    }
    /* =============== Test: x <= y =============== */
    printf("--- Testing LEQ (<=) Architectures ---\n");
    for (size_t i = 0; i < 100; ++i) {
        uint64_t x = global_bank[i].x;
        uint64_t y = global_bank[i].y;
        uint8_t leq_exp = global_bank[i].leq_exp;
        #if __ARCH_ARM64__
            res = _arm64_crt_leq(x, y);
            total_tests++; if (res == leq_exp) passed_tests++;
            else printf(
                "[FAIL] _arm64_crt_leq      | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, leq_exp, res
            );
            res = _vanillc_crt_leq(x, y);
            total_tests++; if (res == leq_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_leq    | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, leq_exp, res
            );
        #elif __ARCH_X86_64__
            res = _x86_crt_leq(x, y);
            total_tests++; if (res == leq_exp) passed_tests++;
            else printf(
                "[FAIL] _x86_crt_leq        | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, leq_exp, res
            );
            res = _vanillc_crt_leq(x, y);
            total_tests++; if (res == leq_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_leq    | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, leq_exp, res
            );
        #elif __ARCH_RVI64__
            res = _rv64_crt_leq(x, y);
            total_tests++; if (res == leq_exp) passed_tests++;
            else printf(
                "[FAIL] _rv64_crt_leq       | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, leq_exp, res
            );
            res = _vanillc_crt_leq(x, y);
            total_tests++; if (res == leq_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_leq    | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, leq_exp, res
            );
        #else
            res = _vanillc_crt_leq(x, y);
            total_tests++; if (res == leq_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_leq    | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, leq_exp, res
            );
        #endif
    }
    /* =============== Test: x >= y =============== */
    printf("--- Testing GEQ (>=) Architectures ---\n");
    for (size_t i = 0; i < 100; ++i) {
        uint64_t x = global_bank[i].x;
        uint64_t y = global_bank[i].y;
        uint8_t geq_exp = global_bank[i].geq_exp;
        #if __ARCH_ARM64__
            res = _arm64_crt_geq(x, y);
            total_tests++; if (res == geq_exp) passed_tests++;
            else printf(
                "[FAIL] _arm64_crt_geq      | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, geq_exp, res
            );
            res = _vanillc_crt_geq(x, y);
            total_tests++; if (res == geq_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_geq    | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, geq_exp, res
            );
        #elif __ARCH_X86_64__
            res = _x86_crt_geq(x, y);
            total_tests++; if (res == geq_exp) passed_tests++;
            else printf(
                "[FAIL] _x86_crt_geq        | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, geq_exp, res
            );
            res = _vanillc_crt_geq(x, y);
            total_tests++; if (res == geq_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_geq    | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, geq_exp, res
            );
        #elif __ARCH_RVI64__
            res = _rv64_crt_geq(x, y);
            total_tests++; if (res == geq_exp) passed_tests++;
            else printf(
                "[FAIL] _rv64_crt_geq       | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, geq_exp, res
            );
            res = _vanillc_crt_geq(x, y);
            total_tests++; if (res == geq_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_geq    | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, geq_exp, res
            );
        #else
            res = _vanillc_crt_geq(x, y);
            total_tests++; if (res == geq_exp) passed_tests++;
            else printf(
                "[FAIL] _vanillc_crt_geq    | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
                " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, geq_exp, res
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
