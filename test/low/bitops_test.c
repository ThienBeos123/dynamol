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

// ==========================================================
// TEST CASES POPULATION (60 elements each: 30 base + 30 extra)
// ==========================================================
typedef struct { uint64_t input; uint64_t expected; } bswap_case;
typedef struct { uint64_t input; uint8_t clz; uint8_t ctz; uint8_t pcnt; } other_case;
bswap_case bswap_cases[60] = {
    // --- Base 30 Cases ---
    {0x0000000000000000ULL, 0x0000000000000000ULL}, {0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL},
    {0x0123456789ABCDEFULL, 0xEFCDAB8967452301ULL}, {0xEFCDAB8967452301ULL, 0x0123456789ABCDEFULL},
    {0xAA55AA55AA55AA55ULL, 0x55AA55AA55AA55AAULL}, {0x55AA55AA55AA55AAULL, 0xAA55AA55AA55AA55ULL},
    {0x00000000000000FFULL, 0xFF00000000000000ULL}, {0xFF00000000000000ULL, 0x00000000000000FFULL},
    {0x000000000000FF00ULL, 0x00FF000000000000ULL}, {0x00FF000000000000ULL, 0x000000000000FF00ULL},
    {0x0000000000FF0000ULL, 0x0000FF0000000000ULL}, {0x0000FF0000000000ULL, 0x0000000000FF0000ULL},
    {0x00000000FF000000ULL, 0x000000FF00000000ULL}, {0x000000FF00000000ULL, 0x00000000FF000000ULL},
    {0x1122334455667788ULL, 0x8877665544332211ULL}, {0x8877665544332211ULL, 0x1122334455667788ULL},
    {0xDEADC0DEDEADC0DEULL, 0xDEC0ADDEDEC0ADDEULL}, {0x00F000F000F000F0ULL, 0xF000F000F000F000ULL},
    {0xF000F000F000F000ULL, 0x00F000F000F000F0ULL}, {0x1234567890ABCDEFULL, 0xEFCDAB9078563412ULL},
    {0x1000000000000001ULL, 0x0100000000000010ULL}, {0x0001000000001000ULL, 0x0010000000000100ULL},
    {0xFFFFFF0000000000ULL, 0x0000000000FFFFFFULL}, {0x0000000000FFFFFFULL, 0xFFFFFF0000000000ULL},
    {0xCCCCCCCCCCCCCCCCULL, 0xCCCCCCCCCCCCCCCCULL}, {0x3333333333333333ULL, 0x3333333333333333ULL},
    {0x0102030405060708ULL, 0x0807060504030201ULL}, {0x0F0F0F0F0F0F0F0FULL, 0x0F0F0F0F0F0F0F0FULL},
    {0xF0F0F0F0F0F0F0F0ULL, 0xF0F0F0F0F0F0F0F0ULL}, {0xABCDEF0123456789ULL, 0x8967452301EFCDABULL},

    // --- Extra 30 Cases ---
    {0x0000000000000001ULL, 0x0100000000000000ULL}, {0x8000000000000000ULL, 0x0000000000000080ULL},
    {0x0000000000000100ULL, 0x0001000000000000ULL}, {0x0000000000010000ULL, 0x0000010000000000ULL},
    {0x0000000000100000ULL, 0x0000100000000000ULL}, {0x0000000001000000ULL, 0x0000000100000000ULL},
    {0x0000000010000000ULL, 0x0000001000000000ULL}, {0x0000000100000000ULL, 0x0000000001000000ULL},
    {0x0000001000000000ULL, 0x0000000010000000ULL}, {0x0000010000000000ULL, 0x0000000000010000ULL},
    {0x0000100000000000ULL, 0x0000000000100000ULL}, {0x0001000000000000ULL, 0x0000000000000100ULL},
    {0x0010000000000000ULL, 0x0000000000001000ULL}, {0x0100000000000000ULL, 0x0000000000000001ULL},
    {0xA5A5A5A5A5A5A5A5ULL, 0xA5A5A5A5A5A5A5A5ULL}, {0x5A5A5A5A5A5A5A5AULL, 0x5A5A5A5A5A5A5A5AULL},
    {0x1234567812345678ULL, 0x7856341278563412ULL}, {0x8765432187654321ULL, 0x2143658721436587ULL},
    {0x0011223344556677ULL, 0x7766554433221100ULL}, {0x7766554433221100ULL, 0x0011223344556677ULL},
    {0x9999999999999999ULL, 0x9999999999999999ULL}, {0x6666666666666666ULL, 0x6666666666666666ULL},
    {0xBD2394F01A4E7C35ULL, 0x357C4E1AF09423BDULL}, {0x357C4E1AF09423BDULL, 0xBD2394F01A4E7C35ULL},
    {0xFFFFFFFFFFFFFF00ULL, 0x00FFFFFFFFFFFFFFULL}, {0x00FFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFF00ULL},
    {0x7F7F7F7F7F7F7F7FULL, 0x7F7F7F7F7F7F7F7FULL}, {0xE0E0E0E0E0E0E0E0ULL, 0xE0E0E0E0E0E0E0E0ULL},
    {0x0707070707070707ULL, 0x0707070707070707ULL}, {0xDEADBEEFFEEDFACEULL, 0xCEFAEDFECEBEADDEULL}
};
other_case other_cases[60] = {
    // --- Base 30 Cases ---
    {0x0000000000000000ULL, 64, 64, 0},  {0xFFFFFFFFFFFFFFFFULL, 0,  0,  64},
    {0x0000000000000001ULL, 63, 0,  1},  {0x8000000000000000ULL, 0,  63, 1},
    {0x00000000FFFFFFFFULL, 32, 0,  32}, {0xFFFFFFFF00000000ULL, 0,  32, 32},
    {0x0000FFFFFFFF0000ULL, 16, 16, 32}, {0x000000000000000FULL, 60, 0,  4},
    {0xF000000000000000ULL, 0,  60, 4},  {0x5555555555555555ULL, 1,  0,  32},
    {0xAAAAAAAAAAAAAAAAULL, 0,  1,  32}, {0x0000000080000000ULL, 32, 31, 1},
    {0x0000000100000000ULL, 31, 32, 1},  {0x7FFFFFFFFFFFFFFFULL, 1,  0,  63},
    {0xFFFFFFFFFFFFFFFEULL, 0,  1,  63}, {0x8000000000000001ULL, 0,  0,  2},
    {0x4000000000000002ULL, 1,  1,  2},  {0x2000000000000004ULL, 2,  2,  2},
    {0x1000000000000008ULL, 3,  3,  2},  {0x0800000000000010ULL, 4,  4,  2},
    {0x0400000000000020ULL, 5,  5,  2},  {0x0200000000000040ULL, 6,  6,  2},
    {0x0100000000000080ULL, 7,  7,  2},  {0x000000000000FF00ULL, 48, 8,  8},
    {0x00FF000000000000ULL, 8,  48, 8},  {0x0000000007E00000ULL, 37, 21, 6},
    {0x0123456789ABCDEFULL, 7,  0,  32}, {0xFEDCBA9876543210ULL, 0,  4,  32},
    {0x00000003C0000000ULL, 30, 30, 4},  {0x0000000000000007ULL, 61, 0,  3},

    // --- Extra 30 Cases ---
    {0x0000000000000010ULL, 60, 4,  1},  {0x0000000000000100ULL, 56, 8,  1},
    {0x0000000000001000ULL, 52, 12, 1},  {0x0000000000010000ULL, 48, 16, 1},
    {0x0000000000100000ULL, 44, 20, 1},  {0x0000000001000000ULL, 40, 24, 1},
    {0x0000000010000000ULL, 36, 28, 1},  {0x0000000100000000ULL, 31, 32, 1},
    {0x0000001000000000ULL, 27, 36, 1},  {0x0000010000000000ULL, 23, 40, 1},
    {0x0000100000000000ULL, 19, 44, 1},  {0x0001000000000000ULL, 15, 48, 1},
    {0x0010000000000000ULL, 11, 52, 1},  {0x0100000000000000ULL, 7,  56, 1},
    {0x1000000000000000ULL, 3,  60, 1},  {0x0000000000000002ULL, 62, 1,  1},
    {0x0000000000000004ULL, 61, 2,  1},  {0x0000000000000008ULL, 60, 3,  1},
    {0x3000000000000000ULL, 2,  60, 2},  {0x6000000000000000ULL, 1,  61, 2},
    {0x0000000000000003ULL, 62, 0,  2},  {0x0000000000000006ULL, 61, 1,  2},
    {0x000000000000000CULL, 60, 2,  2},  {0x0000000000000018ULL, 59, 3,  2},
    {0x000F000000000000ULL, 12, 48, 4},  {0x00F0000000000000ULL, 8,  52, 4},
    {0x0F00000000000000ULL, 4,  56, 4},  {0xFF00000000000000ULL, 0,  56, 8},
    {0x8000000000000001ULL, 0,  0,  2},  {0xC000000000000003ULL, 0,  0,  4}
};

// ==========================================================
// TEST EXECUTION ENGINE
// ==========================================================
int main(void) {
    int total_tests = 0, passed_tests = 0;
    printf("=========================================================\n");
    printf("            RUNNING INTEGRATED UNIT TESTS                \n");
    printf("=========================================================\n\n");
    // ------------------------------------------------------
    // 1. TEST BSWAP64 VARIANTS
    // ------------------------------------------------------
    printf("--- Testing BSWAP64 Architectures ---\n");
    for (int i = 0; i < 60; i++) {
        uint64_t input = bswap_cases[i].input;
        uint64_t expected = bswap_cases[i].expected;
        uint64_t res;
        // Verify crt_vanillc
        res = _crtintrin_bswap64(input);
        total_tests++; if (res == expected) passed_tests++;
        else printf("[FAIL] _crtintrin_bswap64 | Case %d | Input: 0x%016llx | Exp: 0x%016llx | Got: 0x%016llx\n", i, input, expected, res);
        // Verify zvanillc
        res = _cintrin_bswap64(input);
        total_tests++; if (res == expected) passed_tests++;
        else printf("[FAIL] _cintrin_bswap64    | Case %d | Input: 0x%016llx | Exp: 0x%016llx | Got: 0x%016llx\n", i, input, expected, res);

        // Target Specific Hardware / ABI Layer Dispatches
        #if __ARCH_ARM64__
            res = _arm64_bswap64(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _arm64_bswap64      | Case %d | Input: 0x%016llx | Exp: 0x%016llx | Got: 0x%016llx\n", i, input, expected, res);
        #elif __ARCH_X86_64__
            res = _x86_bswap64(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _x86_bswap64        | Case %d | Input: 0x%016llx | Exp: 0x%016llx | Got: 0x%016llx\n", i, input, expected, res);
        #elif __ARCH_RVI64__
            res = _rv64_bswap64(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _rv64_bswap64       | Case %d | Input: 0x%016llx | Exp: 0x%016llx | Got: 0x%016llx\n", i, input, expected, res);

            res = _rv64_bswap64_port(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _rv64_bswap64_port  | Case %d | Input: 0x%016llx | Exp: 0x%016llx | Got: 0x%016llx\n", i, input, expected, res);
        #endif
    }

    // ------------------------------------------------------
    // 2. TEST CLZ64 VARIANTS
    // ------------------------------------------------------
    printf("--- Testing CLZ64 Architectures ---\n");
    for (int i = 0; i < 60; i++) {
        uint64_t input = other_cases[i].input;
        uint8_t expected = other_cases[i].clz;
        uint8_t res;

        res = _crtintrin_clz64(input);
        total_tests++; if (res == expected) passed_tests++;
        else printf("[FAIL] _crtintrin_clz64   | Case %d | Input: 0x%016llx | Exp: %u | Got: %u\n", i, input, expected, res);

        res = _cintrin_clz64(input);
        total_tests++; if (res == expected) passed_tests++;
        else printf("[FAIL] _cintrin_clz64      | Case %d | Input: 0x%016llx | Exp: %u | Got: %u\n", i, input, expected, res);

        #if __ARCH_ARM64__
            res = _arm64_clz64(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _arm64_clz64        | Case %d | Input: 0x%016llx | Exp: %u | Got: %u\n", i, input, expected, res);
        #elif __ARCH_X86_64__
            res = _x86_clz64e(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _x86_clz64e         | Case %d | Input: 0x%016llx | Exp: %u | Got: %u\n", i, input, expected, res);

            res = _x86_clz64s(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _x86_clz64s         | Case %d | Input: 0x%016llx | Exp: %u | Got: %u\n", i, input, expected, res);
        #elif __ARCH_RVI64__
            res = _rv64_clz64(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _rv64_clz64         | Case %d | Input: 0x%016llx | Exp: %u | Got: %u\n", i, input, expected, res);

            res = _rv64_clz64p(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _rv64_clz64p        | Case %d | Input: 0x%016llx | Exp: %u | Got: %u\n", i, input, expected, res);

            res = _rv64_clz64c(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _rv64_clz64c        | Case %d | Input: 0x%016llx | Exp: %u | Got: %u\n", i, input, expected, res);
        #endif
    }

    // ------------------------------------------------------
    // 3. TEST CTZ64 VARIANTS
    // ------------------------------------------------------
    printf("--- Testing CTZ64 Architectures ---\n");
    for (int i = 0; i < 60; i++) {
        uint64_t input = other_cases[i].input;
        uint8_t expected = other_cases[i].ctz;
        uint8_t res;

        res = _crtintrin_ctz64(input);
        total_tests++; if (res == expected) passed_tests++;
        else printf("[FAIL] _crtintrin_ctz64   | Case %d | Input: 0x%016llx | Exp: %u | Got: %u\n", i, input, expected, res);

        res = _cintrin_ctz64(input);
        total_tests++; if (res == expected) passed_tests++;
        else printf("[FAIL] _cintrin_ctz64      | Case %d | Input: 0x%016llx | Exp: %u | Got: %u\n", i, input, expected, res);

        #if __ARCH_ARM64__
            res = _arm64_ctz64(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _arm64_ctz64        | Case %d | Input: 0x%016llx | Exp: %u | Got: %u\n", i, input, expected, res);
        #elif __ARCH_X86_64__
            res = _x86_ctz64e(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _x86_ctz64e         | Case %d | Input: 0x%016llx | Exp: %u | Got: %u\n", i, input, expected, res);

            res = _x86_ctz64s(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _x86_ctz64s         | Case %d | Input: 0x%016llx | Exp: %u | Got: %u\n", i, input, expected, res);
        #elif __ARCH_RVI64__
            res = _rv64_ctz64(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _rv64_ctz64         | Case %d | Input: 0x%016llx | Exp: %u | Got: %u\n", i, input, expected, res);

            res = _rv64_ctz64p(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _rv64_ctz64p        | Case %d | Input: 0x%016llx | Exp: %u | Got: %u\n", i, input, expected, res);

            res = _rv64_ctz64c(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _rv64_ctz64c        | Case %d | Input: 0x%016llx | Exp: %u | Got: %u\n", i, input, expected, res);
        #endif
    }

    // ------------------------------------------------------
    // 4. TEST PCNT64 VARIANTS
    // ------------------------------------------------------
    printf("--- Testing PCNT64 Architectures ---\n");
    for (int i = 0; i < 60; i++) {
        uint64_t input = other_cases[i].input;
        uint8_t expected = other_cases[i].pcnt;
        uint8_t res;

        res = _crtintrin_pcnt64(input);
        total_tests++; if (res == expected) passed_tests++;
        else printf("[FAIL] _crtintrin_pcnt64   | Case %d | Input: 0x%016llx | Exp: %u | Got: %u\n", i, input, expected, res);

        res = _cintrin_pcnt64(input);
        total_tests++; if (res == expected) passed_tests++;
        else printf("[FAIL] _cintrin_pcnt64      | Case %d | Input: 0x%016llx | Exp: %u | Got: %u\n", i, input, expected, res);

        #if __ARCH_ARM64__
            res = _arm64_pcnt64(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _arm64_pcnt64        | Case %d | Input: 0x%016llx | Exp: %u | Got: %u\n", i, input, expected, res);
        #elif __ARCH_X86_64__
            res = _x86_pcnt64e(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _x86_pcnt64e         | Case %d | Input: 0x%016llx | Exp: %u | Got: %u\n", i, input, expected, res);

            res = _x86_pcnt64s(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _x86_pcnt64s         | Case %d | Input: 0x%016llx | Exp: %u | Got: %u\n", i, input, expected, res);
        #elif __ARCH_RVI64__
            res = _rv64_pcnt64(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _rv64_pcnt64         | Case %d | Input: 0x%016llx | Exp: %u | Got: %u\n", i, input, expected, res);

            res = _rv64_pcnt64_port(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _rv64_pcnt64_port    | Case %d | Input: 0x%016llx | Exp: %u | Got: %u\n", i, input, expected, res);
        #endif
    }

    // Summary output block
    printf("=========================================================\n");
    printf("SUMMARY: Passed %-4d out of %-4d total compiled checks.\n", passed_tests, total_tests);
    printf("=========================================================\n");
    return (passed_tests == total_tests) ? 0 : 1;
}
