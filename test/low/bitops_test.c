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
static const bswap_case bswap_cases[60] = {
    // --- Base 30 Cases ---
    {UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000)}, {UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF)},
    {UINT64_C(0x0123456789ABCDEF), UINT64_C(0xEFCDAB8967452301)}, {UINT64_C(0xEFCDAB8967452301), UINT64_C(0x0123456789ABCDEF)},
    {UINT64_C(0xAA55AA55AA55AA55), UINT64_C(0x55AA55AA55AA55AA)}, {UINT64_C(0x55AA55AA55AA55AA), UINT64_C(0xAA55AA55AA55AA55)},
    {UINT64_C(0x00000000000000FF), UINT64_C(0xFF00000000000000)}, {UINT64_C(0xFF00000000000000), UINT64_C(0x00000000000000FF)},
    {UINT64_C(0x000000000000FF00), UINT64_C(0x00FF000000000000)}, {UINT64_C(0x00FF000000000000), UINT64_C(0x000000000000FF00)},
    {UINT64_C(0x0000000000FF0000), UINT64_C(0x0000FF0000000000)}, {UINT64_C(0x0000FF0000000000), UINT64_C(0x0000000000FF0000)},
    {UINT64_C(0x00000000FF000000), UINT64_C(0x000000FF00000000)}, {UINT64_C(0x000000FF00000000), UINT64_C(0x00000000FF000000)},
    {UINT64_C(0x1122334455667788), UINT64_C(0x8877665544332211)}, {UINT64_C(0x8877665544332211), UINT64_C(0x1122334455667788)},
    {UINT64_C(0xDEADC0DEDEADC0DE), UINT64_C(0xDEC0ADDEDEC0ADDE)}, {UINT64_C(0x00F000F000F000F0), UINT64_C(0xF000F000F000F000)},
    {UINT64_C(0xF000F000F000F000), UINT64_C(0x00F000F000F000F0)}, {UINT64_C(0x1234567890ABCDEF), UINT64_C(0xEFCDAB9078563412)},
    {UINT64_C(0x1000000000000001), UINT64_C(0x0100000000000010)}, {UINT64_C(0x0001000000001000), UINT64_C(0x0010000000000100)},
    {UINT64_C(0xFFFFFF0000000000), UINT64_C(0x0000000000FFFFFF)}, {UINT64_C(0x0000000000FFFFFF), UINT64_C(0xFFFFFF0000000000)},
    {UINT64_C(0xCCCCCCCCCCCCCCCC), UINT64_C(0xCCCCCCCCCCCCCCCC)}, {UINT64_C(0x3333333333333333), UINT64_C(0x3333333333333333)},
    {UINT64_C(0x0102030405060708), UINT64_C(0x0807060504030201)}, {UINT64_C(0x0F0F0F0F0F0F0F0F), UINT64_C(0x0F0F0F0F0F0F0F0F)},
    {UINT64_C(0xF0F0F0F0F0F0F0F0), UINT64_C(0xF0F0F0F0F0F0F0F0)}, {UINT64_C(0xABCDEF0123456789), UINT64_C(0x8967452301EFCDAB)},

    // --- Extra 30 Cases ---
    {UINT64_C(0x0000000000000001), UINT64_C(0x0100000000000000)}, {UINT64_C(0x8000000000000000), UINT64_C(0x0000000000000080)},
    {UINT64_C(0x0000000000000100), UINT64_C(0x0001000000000000)}, {UINT64_C(0x0000000000010000), UINT64_C(0x0000010000000000)},
    {UINT64_C(0x0000000000100000), UINT64_C(0x0000100000000000)}, {UINT64_C(0x0000000001000000), UINT64_C(0x0000000100000000)},
    {UINT64_C(0x0000000010000000), UINT64_C(0x0000001000000000)}, {UINT64_C(0x0000000100000000), UINT64_C(0x0000000001000000)},
    {UINT64_C(0x0000001000000000), UINT64_C(0x0000000010000000)}, {UINT64_C(0x0000010000000000), UINT64_C(0x0000000000010000)},
    {UINT64_C(0x0000100000000000), UINT64_C(0x0000000000100000)}, {UINT64_C(0x0001000000000000), UINT64_C(0x0000000000000100)},
    {UINT64_C(0x0010000000000000), UINT64_C(0x0000000000001000)}, {UINT64_C(0x0100000000000000), UINT64_C(0x0000000000000001)},
    {UINT64_C(0xA5A5A5A5A5A5A5A5), UINT64_C(0xA5A5A5A5A5A5A5A5)}, {UINT64_C(0x5A5A5A5A5A5A5A5A), UINT64_C(0x5A5A5A5A5A5A5A5A)},
    {UINT64_C(0x1234567812345678), UINT64_C(0x7856341278563412)}, {UINT64_C(0x8765432187654321), UINT64_C(0x2143658721436587)},
    {UINT64_C(0x0011223344556677), UINT64_C(0x7766554433221100)}, {UINT64_C(0x7766554433221100), UINT64_C(0x0011223344556677)},
    {UINT64_C(0x9999999999999999), UINT64_C(0x9999999999999999)}, {UINT64_C(0x6666666666666666), UINT64_C(0x6666666666666666)},
    {UINT64_C(0xBD2394F01A4E7C35), UINT64_C(0x357C4E1AF09423BD)}, {UINT64_C(0x357C4E1AF09423BD), UINT64_C(0xBD2394F01A4E7C35)},
    {UINT64_C(0xFFFFFFFFFFFFFF00), UINT64_C(0x00FFFFFFFFFFFFFF)}, {UINT64_C(0x00FFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFF00)},                              
    {UINT64_C(0x7F7F7F7F7F7F7F7F), UINT64_C(0x7F7F7F7F7F7F7F7F)}, {UINT64_C(0xE0E0E0E0E0E0E0E0), UINT64_C(0xE0E0E0E0E0E0E0E0)},
    {UINT64_C(0x0707070707070707), UINT64_C(0x0707070707070707)}, {UINT64_C(0xDEADBEEFFEEDFACE), UINT64_C(0xCEFAEDFEEFBEADDE)}                               
};
static const other_case other_cases[60] = {
    // --- Base 30 Cases ---
    {UINT64_C(0x0000000000000000), 64, 64, 0},  {UINT64_C(0xFFFFFFFFFFFFFFFF), 0,  0,  64},
    {UINT64_C(0x0000000000000001), 63, 0,  1},  {UINT64_C(0x8000000000000000), 0,  63, 1},
    {UINT64_C(0x00000000FFFFFFFF), 32, 0,  32}, {UINT64_C(0xFFFFFFFF00000000), 0,  32, 32},
    {UINT64_C(0x0000FFFFFFFF0000), 16, 16, 32}, {UINT64_C(0x000000000000000F), 60, 0,  4},
    {UINT64_C(0xF000000000000000), 0,  60, 4},  {UINT64_C(0x5555555555555555), 1,  0,  32},
    {UINT64_C(0xAAAAAAAAAAAAAAAA), 0,  1,  32}, {UINT64_C(0x0000000080000000), 32, 31, 1},
    {UINT64_C(0x0000000100000000), 31, 32, 1},  {UINT64_C(0x7FFFFFFFFFFFFFFF), 1,  0,  63},
    {UINT64_C(0xFFFFFFFFFFFFFFFE), 0,  1,  63}, {UINT64_C(0x8000000000000001), 0,  0,  2},
    {UINT64_C(0x4000000000000002), 1,  1,  2},  {UINT64_C(0x2000000000000004), 2,  2,  2},
    {UINT64_C(0x1000000000000008), 3,  3,  2},  {UINT64_C(0x0800000000000010), 4,  4,  2},
    {UINT64_C(0x0400000000000020), 5,  5,  2},  {UINT64_C(0x0200000000000040), 6,  6,  2},
    {UINT64_C(0x0100000000000080), 7,  7,  2},  {UINT64_C(0x000000000000FF00), 48, 8,  8},
    {UINT64_C(0x00FF000000000000), 8,  48, 8},  {UINT64_C(0x0000000007E00000), 37, 21, 6},
    {UINT64_C(0x0123456789ABCDEF), 7,  0,  32}, {UINT64_C(0xFEDCBA9876543210), 0,  4,  32},
    {UINT64_C(0x00000003C0000000), 30, 30, 4},  {UINT64_C(0x0000000000000007), 61, 0,  3},

    // --- Extra 30 Cases ---
    {UINT64_C(0x0000000000000010), 59, 4,  1},  {UINT64_C(0x0000000000000100), 55, 8,  1},
    {UINT64_C(0x0000000000001000), 51, 12, 1},  {UINT64_C(0x0000000000010000), 47, 16, 1},
    {UINT64_C(0x0000000000100000), 43, 20, 1},  {UINT64_C(0x0000000001000000), 39, 24, 1},
    {UINT64_C(0x0000000010000000), 35, 28, 1},  {UINT64_C(0x0000000100000000), 31, 32, 1},
    {UINT64_C(0x0000001000000000), 27, 36, 1},  {UINT64_C(0x0000010000000000), 23, 40, 1},
    {UINT64_C(0x0000100000000000), 19, 44, 1},  {UINT64_C(0x0001000000000000), 15, 48, 1},
    {UINT64_C(0x0010000000000000), 11, 52, 1},  {UINT64_C(0x0100000000000000), 7,  56, 1},
    {UINT64_C(0x1000000000000000), 3,  60, 1},  {UINT64_C(0x0000000000000002), 62, 1,  1},
    {UINT64_C(0x0000000000000004), 61, 2,  1},  {UINT64_C(0x0000000000000008), 60, 3,  1},
    {UINT64_C(0x3000000000000000), 2,  60, 2},  {UINT64_C(0x6000000000000000), 1,  61, 2},
    {UINT64_C(0x0000000000000003), 62, 0,  2},  {UINT64_C(0x0000000000000006), 61, 1,  2},
    {UINT64_C(0x000000000000000C), 60, 2,  2},  {UINT64_C(0x0000000000000018), 59, 3,  2},
    {UINT64_C(0x000F000000000000), 12, 48, 4},  {UINT64_C(0x00F0000000000000), 8,  52, 4},
    {UINT64_C(0x0F00000000000000), 4,  56, 4},  {UINT64_C(0xFF00000000000000), 0,  56, 8},
    {UINT64_C(0x8000000000000001), 0,  0,  2},  {UINT64_C(0xC000000000000003), 0,  0,  4}
};

// ==========================================================
// TEST EXECUTION ENGINE
// ==========================================================
int main(void) {
    int total_tests = 0, passed_tests = 0;
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    printf("=========================================================\n");
    printf("        RUNNING INTEGRATED UNIT TESTS - BITOPS           \n");
    printf("=========================================================\n");
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
        else printf(
            "[FAIL] _crtintrin_bswap64 | Case %2d | Input: 0x%016" PRIX64 
            " | Exp: 0x%016" PRIX64 " | Got: 0x%016" PRIX64 "\n",
            i + 1, input, expected, res
        );
        // Verify zvanillc
        res = _cintrin_bswap64(input);
        total_tests++; if (res == expected) passed_tests++;
        else printf(
            "[FAIL] _cintrin_bswap64    | Case %2d | Input: 0x%016" PRIX64 
            " | Exp: 0x%016" PRIX64 " | Got: 0x%016" PRIX64 "\n",
            i + 1, input, expected, res
        );

        // Target Specific Hardware / ABI Layer Dispatches
        #if __ARCH_ARM64__
            res = _arm64_bswap64(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf(
                "[FAIL] _arm64_bswap64       | Case %2d | Input: 0x%016" PRIX64 
                " | Exp: 0x%016" PRIX64 " | Got: 0x%016" PRIX64 "\n",
                i + 1, input, expected, res
            );
        #elif __ARCH_X86_64__
            res = _x86_bswap64(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf(
                "[FAIL] _x86_bswap64         | Case %2d | Input: 0x%016" PRIX64 
                " | Exp: 0x%016" PRIX64 " | Got: 0x%016" PRIX64 "\n",
                i + 1, input, expected, res
            );
        #elif __ARCH_RVI64__
            res = _rv64_bswap64(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf(
                "[FAIL] _rv64_bswap64        | Case %2d | Input: 0x%016" PRIX64 
                " | Exp: 0x%016" PRIX64 " | Got: 0x%016" PRIX64 "\n",
                i + 1, input, expected, res
            );

            res = _rv64_bswap64_port(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf(
                "[FAIL] _rv64_bswap64_port  | Case %2d | Input: 0x%016" PRIX64 
                " | Exp: 0x%016" PRIX64 " | Got: 0x%016" PRIX64 "\n",
                i + 1, input, expected, res
            );
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
        else printf("[FAIL] _crtintrin_clz64   "
            "| Case %2d | Input: 0x%016" PRIX64 " | Exp: %u | Got: %u\n", 
            i + 1, input, expected, res
        );

        res = _cintrin_clz64(input);
        total_tests++; if (res == expected) passed_tests++;
        else printf("[FAIL] _cintrin_clz64      "
            "| Case %2d | Input: 0x%016" PRIX64 " | Exp: %u | Got: %u\n", 
            i + 1, input, expected, res
        );

        #if __ARCH_ARM64__
            res = _arm64_clz64(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _arm64_clz64        "
                "| Case %2d | Input: 0x%016" PRIX64 " | Exp: %u | Got: %u\n", 
                i + 1, input, expected, res
            );
        #elif __ARCH_X86_64__
            res = _x86_clz64e(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _x86_clz64e         "
                "| Case %2d | Input: 0x%016" PRIX64 " | Exp: %u | Got: %u\n", 
                i + 1, input, expected, res
            );

            res = _x86_clz64s(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _x86_clz64s         "
                "| Case %2d | Input: 0x%016" PRIX64 " | Exp: %u | Got: %u\n", 
                i + 1, input, expected, res
            );
        #elif __ARCH_RVI64__
            res = _rv64_clz64(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _rv64_clz64         "
                "| Case %2d | Input: 0x%016" PRIX64 " | Exp: %u | Got: %u\n", 
                i + 1, input, expected, res
            );

            res = _rv64_clz64p(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _rv64_clz64p        "
                "| Case %2d | Input: 0x%016" PRIX64 " | Exp: %u | Got: %u\n", 
                i + 1, input, expected, res
            );

            res = _rv64_clz64c(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _rv64_clz64c        "
                "| Case %2d | Input: 0x%016" PRIX64 " | Exp: %u | Got: %u\n", 
                i + 1, input, expected, res
            );
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
        else printf("[FAIL] _crtintrin_ctz64   "
            "| Case %2d | Input: 0x%016" PRIX64 " | Exp: %u | Got: %u\n", 
            i + 1, input, expected, res
        );

        res = _cintrin_ctz64(input);
        total_tests++; if (res == expected) passed_tests++;
        else printf("[FAIL] _cintrin_ctz64      "
            "| Case %2d | Input: 0x%016" PRIX64 " | Exp: %u | Got: %u\n", 
            i + 1, input, expected, res
        );

        #if __ARCH_ARM64__
            res = _arm64_ctz64(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _arm64_ctz64        "
                "| Case %2d | Input: 0x%016" PRIX64 " | Exp: %u | Got: %u\n", 
                i + 1, input, expected, res
            );
        #elif __ARCH_X86_64__
            res = _x86_ctz64e(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _x86_ctz64e         "
                "| Case %2d | Input: 0x%016" PRIX64 " | Exp: %u | Got: %u\n", 
                i + 1, input, expected, res
            );

            res = _x86_ctz64s(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _x86_ctz64s         "
                "| Case %2d | Input: 0x%016" PRIX64 " | Exp: %u | Got: %u\n", 
                i + 1, input, expected, res
            );
        #elif __ARCH_RVI64__
            res = _rv64_ctz64(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _rv64_ctz64         "
                "| Case %2d | Input: 0x%016" PRIX64 " | Exp: %u | Got: %u\n", 
                i + 1, input, expected, res
            );

            res = _rv64_ctz64p(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _rv64_ctz64p        "
                "| Case %2d | Input: 0x%016" PRIX64 " | Exp: %u | Got: %u\n", 
                i + 1, input, expected, res
            );

            res = _rv64_ctz64c(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _rv64_ctz64c        "
                "| Case %2d | Input: 0x%016" PRIX64 " | Exp: %u | Got: %u\n", 
                i + 1, input, expected, res
            );
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
        else printf("[FAIL] _crtintrin_pcnt64   "
            "| Case %2d | Input: 0x%016" PRIX64 " | Exp: %u | Got: %u\n", 
            i + 1, input, expected, res
        );

        res = _cintrin_pcnt64(input);
        total_tests++; if (res == expected) passed_tests++;
        else printf("[FAIL] _cintrin_pcnt64      "
            "| Case %2d | Input: 0x%016" PRIX64 " | Exp: %u | Got: %u\n", 
            i + 1, input, expected, res
        );

        #if __ARCH_ARM64__
            res = _arm64_pcnt64(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _arm64_pcnt64        "
                "| Case %2d | Input: 0x%016" PRIX64 " | Exp: %u | Got: %u\n", 
                i + 1, input, expected, res
            );
        #elif __ARCH_X86_64__
            res = _x86_pcnt64e(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _x86_pcnt64e         "
                "| Case %2d | Input: 0x%016" PRIX64 " | Exp: %u | Got: %u\n", 
                i + 1, input, expected, res
            );

            res = _x86_pcnt64s(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _x86_pcnt64s         "
                "| Case %2d | Input: 0x%016" PRIX64 " | Exp: %u | Got: %u\n", 
                i + 1, input, expected, res
            );
        #elif __ARCH_RVI64__
            res = _rv64_pcnt64(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _rv64_pcnt64         "
                "| Case %2d | Input: 0x%016" PRIX64 " | Exp: %u | Got: %u\n", 
                i + 1, input, expected, res
            );

            res = _rv64_pcnt64_port(input);
            total_tests++; if (res == expected) passed_tests++;
            else printf("[FAIL] _rv64_pcnt64_port    "
                "| Case %2d | Input: 0x%016" PRIX64 " | Exp: %u | Got: %u\n", 
                i + 1, input, expected, res
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
