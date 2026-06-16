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
#define CASE_CNT 80
typedef struct { const char *str; size_t buflen; size_t exp; } alen_case_t;
typedef struct { const char *str; size_t buflen; size_t mut_exp; size_t ret_exp; } str_case_t;
/* ----- Out-of-line Strings (> 20 chars) ----- */
#define long_ws_str_1 "                        \t\n  abc"
#define long_ws_str_2 "   \t\t\n\n\r\r          XYZ"
#define long_lz_str_1 "000000000000000000000000012345"
#define long_lz_str_2 "000000000000000000000000000000"
#define long_mix_str_1 "This string has no initial padding elements."
#define long_mix_str_2 "   00000Mixed spaces and zeros test case."
//* =============== GLOBAL ARRAY OF CASES =============== *//
static const alen_case_t actual_len_cases[CASE_CNT] = {
    // 1-20: Null-termination and standard lengths within different buffer allocations
    {"", 10, 0}, {"a", 10, 1}, {"abc", 10, 3}, {"abcdefghij", 5, 5}, {"abc\0def", 10, 3},
    {"12345", 5, 5}, {"12345", 4, 4}, {"", 0, 0}, {"a", 0, 0}, {"hello", 20, 5},
    {"world", 3, 3}, {"test", 4, 4}, {"cryptography", 5, 5}, {"bignum", 10, 6}, {"constant", 20, 8},
    {"time", 2, 2}, {"secure", 10, 6}, {"poison", 10, 6}, {"memory", 4, 4}, {"buffer", 6, 6},
    // 21-40: Edge values close to buffer sizes and out-of-line mappings
    {long_mix_str_1, 50, 44}, {long_mix_str_2, 50, 41}, 
    {"12345678901234567890", 20, 20}, 
    {"12345678901234567890", 25, 20}, {"a\0", 5, 1},
    {"\0b", 5, 0}, {"ab\0c", 3, 2}, {"abcdef", 6, 6}, {"abcdef", 7, 6}, {"abcdef", 3, 3},
    {"x", 1, 1}, {"xy", 1, 1}, {"xyz", 2, 2}, {"", 1, 0}, {"a", 2, 1},
    {"ab", 5, 2}, {"abc", 5, 3}, {"abcd", 5, 4}, {"abcde", 5, 5}, {"abcdef", 5, 5},
    // 41-60: Varied inputs
    {"A", 10, 1}, {"B", 10, 1}, {"C", 10, 1}, {"DE", 10, 2}, {"FGH", 10, 3},
    {"IJK", 2, 2}, {"LMNOP", 20, 5}, {"QRSTU", 4, 4}, {"VWXYZ", 10, 5}, {"0123", 10, 4},
    {"4567", 3, 3}, {"89", 10, 2}, {"+-*", 10, 3}, {"=<>", 10, 3}, {"&|^", 10, 3},
    {"~!", 10, 2}, {"@#$", 10, 3}, {"%^&*()", 20, 6}, {"_-[", 10, 3}, {"]{}", 10, 3},
    // 61-80: Uniform structure fills
    {"a", 5, 1}, {"b", 5, 1}, {"c", 5, 1}, {"d", 5, 1}, {"e", 5, 1},
    {"f", 5, 1}, {"g", 5, 1}, {"h", 5, 1}, {"i", 5, 1}, {"j", 5, 1},
    {"k", 5, 1}, {"l", 5, 1}, {"m", 5, 1}, {"n", 5, 1}, {"o", 5, 1},
    {"p", 5, 1}, {"q", 5, 1}, {"r", 5, 1}, {"s", 5, 1}, {"t", 5, 1}
};
static const str_case_t skip_whitespace_cases[CASE_CNT] = {
    // 1-20: Pure or initial white spaces (Inlined)
    {" ", 5, 1, 1}, {"  ", 5, 2, 2}, {"\t", 5, 1, 1}, {"\n", 5, 1, 1}, {" \t\n", 5, 3, 3},
    {" a", 5, 1, 1}, {"  b", 5, 2, 2}, {"\tc", 5, 1, 1}, {"\nd", 5, 1, 1}, {" \t\ne", 5, 3, 3},
    {"", 5, 0, 0}, {"a ", 5, 0, 0}, {"ab", 5, 0, 0}, {"  ", 1, 1, 1}, {" \t", 2, 2, 2},
    {"\r\nxyz", 10, 2, 2}, {" \v\f ", 10, 4, 4}, {"\t\t\t", 2, 2, 2}, {"  a  ", 6, 2, 2}, {" \t \n ", 3, 3, 3},
    // 21-40: Out-of-line pointers or edge lengths
    {long_ws_str_1, 31, 28, 28}, {long_ws_str_2, 22, 19, 19}, 
    {long_mix_str_2, 50, 3, 3}, {"   ", 0, 0, 0}, {"   ", 2, 2, 2},
    {" \0 ", 5, 1, 1}, {"\t\0\t", 5, 1, 1}, {" \n\0", 2, 2, 2}, {"     ", 3, 3, 3}, {"\n\n\n", 2, 2, 2},
    {"a  ", 3, 0, 0}, {" a ", 2, 1, 1}, {"  a", 2, 2, 2}, {"\t a", 1, 1, 1}, {"\n a", 1, 1, 1},
    {"\r a", 1, 1, 1}, {"\v a", 1, 1, 1}, {"\f a", 1, 1, 1}, {"   ", 5, 3, 3}, {"\t\t", 5, 2, 2},
    // 41-80: Structure fills
    {" ", 2, 1, 1}, {" ", 2, 1, 1}, {" ", 2, 1, 1}, {" ", 2, 1, 1}, {" ", 2, 1, 1},
    {" ", 2, 1, 1}, {" ", 2, 1, 1}, {" ", 2, 1, 1}, {" ", 2, 1, 1}, {" ", 2, 1, 1},
    {" ", 2, 1, 1}, {" ", 2, 1, 1}, {" ", 2, 1, 1}, {" ", 2, 1, 1}, {" ", 2, 1, 1},
    {" ", 2, 1, 1}, {" ", 2, 1, 1}, {" ", 2, 1, 1}, {" ", 2, 1, 1}, {" ", 2, 1, 1},
    {" ", 2, 1, 1}, {" ", 2, 1, 1}, {" ", 2, 1, 1}, {" ", 2, 1, 1}, {" ", 2, 1, 1},
    {" ", 2, 1, 1}, {" ", 2, 1, 1}, {" ", 2, 1, 1}, {" ", 2, 1, 1}, {" ", 2, 1, 1},
    {" ", 2, 1, 1}, {" ", 2, 1, 1}, {" ", 2, 1, 1}, {" ", 2, 1, 1}, {" ", 2, 1, 1},
    {" ", 2, 1, 1}, {" ", 2, 1, 1}, {" ", 2, 1, 1}, {" ", 2, 1, 1}, {" ", 2, 1, 1}
};
static const str_case_t skip_lzeros_cases[CASE_CNT] = {
    // 1-20: Pure or initial leading zeros
    {"0", 5, 1, 1}, {"00", 5, 2, 2}, {"000", 5, 3, 3}, {"0a", 5, 1, 1}, {"00b", 5, 2, 2},
    {"000c", 5, 3, 3}, {"", 5, 0, 0}, {"a0", 5, 0, 0}, {"100", 5, 0, 0}, {"00000", 3, 3, 3},
    {"00", 1, 1, 1}, {"000", 2, 2, 2}, {"0\00", 5, 1, 1}, {"00\0", 5, 2, 2}, {"0102", 5, 1, 1},
    {"0034", 10, 2, 2}, {"00056", 10, 3, 3}, {"000078", 10, 4, 4}, {"000009", 10, 5, 5}, {"000000", 4, 4, 4},
    // 21-40: Large external constants and custom thresholds
    {long_lz_str_1, 40, 25, 25}, {long_lz_str_2, 40, 30, 30}, 
    {long_mix_str_2, 50, 0, 0}, {"000", 0, 0, 0}, {"00000", 5, 5, 5},
    {"0a0", 5, 1, 1}, {"00b00", 5, 2, 2}, {"000c000", 5, 3, 3}, {"0000d", 2, 2, 2}, {"00000e", 4, 4, 4},
    {"000000f", 5, 5, 5}, {"0000000g", 6, 6, 6}, {"00h", 1, 1, 1}, {"000i", 2, 2, 2}, {"0000j", 3, 3, 3},
    {"00000k", 4, 4, 4}, {"000000l", 5, 5, 5}, {"0000000m", 6, 6, 6}, 
    {"00000000n", 7, 7, 7}, {"000000000o", 8, 8, 8},
    // 41-80: Structure fills
    {"0", 2, 1, 1}, {"0", 2, 1, 1}, {"0", 2, 1, 1}, {"0", 2, 1, 1}, {"0", 2, 1, 1},
    {"0", 2, 1, 1}, {"0", 2, 1, 1}, {"0", 2, 1, 1}, {"0", 2, 1, 1}, {"0", 2, 1, 1},
    {"0", 2, 1, 1}, {"0", 2, 1, 1}, {"0", 2, 1, 1}, {"0", 2, 1, 1}, {"0", 2, 1, 1},
    {"0", 2, 1, 1}, {"0", 2, 1, 1}, {"0", 2, 1, 1}, {"0", 2, 1, 1}, {"0", 2, 1, 1},
    {"0", 2, 1, 1}, {"0", 2, 1, 1}, {"0", 2, 1, 1}, {"0", 2, 1, 1}, {"0", 2, 1, 1},
    {"0", 2, 1, 1}, {"0", 2, 1, 1}, {"0", 2, 1, 1}, {"0", 2, 1, 1}, {"0", 2, 1, 1},
    {"0", 2, 1, 1}, {"0", 2, 1, 1}, {"0", 2, 1, 1}, {"0", 2, 1, 1}, {"0", 2, 1, 1},
    {"0", 2, 1, 1}, {"0", 2, 1, 1}, {"0", 2, 1, 1}, {"0", 2, 1, 1}, {"0", 2, 1, 1}
};


int main(void) { _libdnml_init();
    int total_tests = 0, passed_tests = 0; FILE *fskip_cases = fopen("../../str_parse/fskip_whitespace.txt", "r");
    struct timespec start, end; clock_gettime(CLOCK_MONOTONIC, &start);
    if (fskip_cases == NULL) {
        fputs("\033[1;31mERROR: Test unable to open .in file for input\033[0m\n", stderr);
        _libdnml_cleanup(); return 1;
    }
    fputs("=================================================================================\n", stdout);
    fputs("      RUNNING INTEGRATED UNIT TESTS - STRING SKIPPING & COUNTING CONVERSIONS     \n", stdout);
    fputs("=================================================================================\n", stdout);
    fputs("---- _actual_len -----\n", stdout);
    for (int i = 0; i < CASE_CNT; ++i) { ++total_tests;
        const char *str = actual_len_cases[i].str; 
        size_t buflen = actual_len_cases[i].buflen;
        size_t exp = actual_len_cases[i].exp, res = _actual_len(str, buflen);
        if (res == exp) ++passed_tests;
        else printf(
            "[FAIL] _actual_len | Case %-2d | Buflen: %-3zu | Res: %3zu | Exp: %3zu\n"
            "  STRING: %s\n", i + 1, buflen, res, exp, str
        );
    }
    fputs("---- _fskip_whitespace__ -----\n", stdout);
    for (int i = 0; i < CASE_CNT; ++i) { total_tests++;
        uint16_t res_char = _fskip_whitespace__(fskip_cases);
        // Define the predictable stopping point value
        // Case 0 expect 'A' + 0, Case 1 expects 'A' + 1, looping through ASCII increments safely
        char expected_char = 'A' + (i % 26); 
        if (res_char == (uint16_t)EOF) {
            printf("[FAIL] _fskip_whitespace__ | Case %-2d | Premature End-of-File reached.\n", i + 1); continue;
        }
        if (res_char == (uint16_t)expected_char) passed_tests++;
        else printf(
            "[FAIL] _fskip_whitespace__ | Case %-2d | Got character: '%c' (0x%02X) | Expected: '%c'\n", 
            i + 1, (char)res_char, res_char, expected_char
        );
        // CRITICAL: Consume residual text on the active line until '\n' is wiped out 
        // This repositions the cursor directly onto the next test sequence line entry.
        int clear_buf; while ((clear_buf = fgetc(fskip_cases)) != '\n' && clear_buf != EOF);
    }
    fputs("---- _skip_whitespace -----\n", stdout);
    for (int i = 0; i < CASE_CNT; ++i) { ++total_tests;
        const char *str = skip_whitespace_cases[i].str; 
        size_t buflen = skip_whitespace_cases[i].buflen;
        size_t pos = 0; size_t res = _skip_whitespace(str, buflen, &pos);
        size_t exp_pos = skip_whitespace_cases[i].mut_exp;
        size_t ws_exp = skip_whitespace_cases[i].ret_exp;
        if (pos == exp_pos && res == ws_exp) passed_tests++;
        else printf(
            "[FAIL] _skip_whitespace | Case %-2d | Buflen: %-3zu | Pos: %3zu WS: %3zu "
            "| Exp_Pos: %3zu Exp_WS: %3zu\n    STRING: %s\n", i + 1, buflen, pos, res, exp_pos, ws_exp, str
        );
    }
    fputs("---- _skip_leading_zeros -----\n", stdout);
    for (int i = 0; i < CASE_CNT; ++i) { ++total_tests;
        const char *str = skip_lzeros_cases[i].str; 
        size_t buflen = skip_lzeros_cases[i].buflen;
        size_t pos = 0; size_t res = _skip_leading_zeros(str, buflen, &pos);
        size_t exp_pos = skip_lzeros_cases[i].mut_exp;
        size_t lz_exp = skip_lzeros_cases[i].ret_exp;
        if (pos == exp_pos && res == lz_exp) passed_tests++;
        else printf(
            "[FAIL] _skip_whitespace | Case %-2d | Buflen: %-3zu | Pos: %3zu LZ: %3zu "
            "| Exp_Pos: %3zu Exp_LZ: %3zu\n    STRING: %s\n", i + 1, buflen, pos, res, exp_pos, lz_exp, str
        );
    }

    #undef long_ws_str_1
    #undef long_ws_str_2
    #undef long_lz_str_1
    #undef long_lz_str_2
    #undef long_mix_str_1
    #undef long_mix_str_2
    #undef CASE_CNT
    clock_gettime(CLOCK_MONOTONIC, &end); fclose(fskip_cases);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    fputs( "=========================================================\n", stdout);
    fputs( "TEST SUMMARY:\n", stdout);
    printf("+) Passed %-4d out of %-4d total compiled checks.\n", passed_tests, total_tests);
    printf("+) Success rate: %.2f%%\n", (passed_tests * 100.0) / total_tests);
    printf("+) Total Runtime: %lf ms\n", elapsed_time * 1000.0);
    fputs("=========================================================\n", stdout);
    _libdnml_cleanup(); return (passed_tests == total_tests) ? 0 : 1;
}
