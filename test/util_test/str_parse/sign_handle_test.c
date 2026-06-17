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
typedef struct { const char *str; int8_t exp_sign; size_t exp_pos; uint8_t exp_err; } sign_case_t;
typedef struct { const char *str; size_t len; int8_t exp_sign; size_t exp_pos; uint8_t exp_err; } sign_nlen_case;
/* ----- Out-of-line Strings (> 20 chars) ----- */
#define long_neg_valid "-123456789012345678901234567890"
#define long_pos_valid "+987654321098765432109876543210"
#define long_no_sign "555555555555555555555555555555"
#define long_invalid_chr "-12345678901234567890X123456789"
#define long_naked_plus "+                                                 "
#define long_naked_minus "-                                                 "
//* ================= GLOBAL ARRAY OF CASES ================= *//
static const sign_case_t sign_cases[CASE_CNT] = {
    // 1-10: Basic functional checks (Implicitly positive, explicit signs)
    {"123", 1, 0, 0}, {"-456", -1, 1, 0}, {"+789", 1, 1, 0}, {"0", 1, 0, 0}, {"-0", -1, 1, 0},
    {"+0", 1, 1, 0}, {"5", 1, 0, 0}, {"-9", -1, 1, 0}, {"+1", 1, 1, 0}, {"99999", 1, 0, 0},
    // 11-20: Error code 3 validations (Naked operators leading to '\0')
    {"-", -1, 1, 3}, {"+", 1, 1, 3}, {"-\0", -1, 1, 3}, {"+\0", 1, 1, 3}, {"-", -1, 1, 3},
    {"+", 1, 1, 3}, {"-", -1, 1, 3}, {"+", 1, 1, 3}, {"-", -1, 1, 3}, {"+", 1, 1, 3},
    // 21-30: Error code 4 validations (Non-digit following optional sign or initial position)
    {"a", 1, 0, 4}, {"-a", -1, 1, 4}, {"+b", 1, 1, 4}, {"- ", -1, 1, 4}, {"+ ", 1, 1, 4},
    {"xyz", 1, 0, 4}, {"-.", -1, 1, 4}, {"+*", 1, 1, 4}, {"@", 1, 0, 4}, {"-#", -1, 1, 4},
    // 31-40: Out-of-line allocations (> 20 chars) and compound validations
    {long_neg_valid, -1, 1, 0}, {long_pos_valid, 1, 1, 0}, {long_no_sign, 1, 0, 0}, 
    {long_invalid_chr, -1, 1, 0}, {long_naked_plus, 1, 1, 4}, {long_naked_minus, -1, 1, 4},
    {"1234567890123456789", 1, 0, 0}, {"-123456789012345678", -1, 1, 0}, 
    {"+123456789012345678", 1, 1, 0}, {"9", 1, 0, 0},
    // 41-80: Structured value pads to complete structural alignment
    {"1", 1, 0, 0}, {"2", 1, 0, 0}, {"3", 1, 0, 0}, {"4", 1, 0, 0}, {"5", 1, 0, 0},
    {"6", 1, 0, 0}, {"7", 1, 0, 0}, {"8", 1, 0, 0}, {"9", 1, 0, 0}, {"0", 1, 0, 0},
    {"-1", -1, 1, 0}, {"-2", -1, 1, 0}, {"-3", -1, 1, 0}, {"-4", -1, 1, 0}, {"-5", -1, 1, 0},
    {"-6", -1, 1, 0}, {"-7", -1, 1, 0}, {"-8", -1, 1, 0}, {"-9", -1, 1, 0}, {"-0", -1, 1, 0},
    {"+1", 1, 1, 0}, {"+2", 1, 1, 0}, {"+3", 1, 1, 0}, {"+4", 1, 1, 0}, {"+5", 1, 1, 0},
    {"+6", 1, 1, 0}, {"+7", 1, 1, 0}, {"+8", 1, 1, 0}, {"+9", 1, 1, 0}, {"+0", 1, 1, 0},
    {"1", 1, 0, 0}, {"2", 1, 0, 0}, {"3", 1, 0, 0}, {"4", 1, 0, 0}, {"5", 1, 0, 0},
    {"6", 1, 0, 0}, {"7", 1, 0, 0}, {"8", 1, 0, 0}, {"9", 1, 0, 0}, {"0", 1, 0, 0}
};
static const sign_nlen_case sign_nlen_cases[CASE_CNT] = {
    // 1-10: Normal processing within strict slice lengths
    {"123", 3, 1, 0, 0}, {"-456", 4, -1, 1, 0}, {"+789", 4, 1, 1, 0}, {"0", 1, 1, 0, 0}, {"-0", 2, -1, 1, 0},
    {"+0", 2, 1, 1, 0}, {"555", 2, 1, 0, 0}, {"-99", 2, -1, 1, 0}, {"+11", 2, 1, 1, 0}, {"98765", 3, 1, 0, 0},
    // 11-20: Length-bound terminations creating bare-operator errors (Error code 3)
    {"-456", 1, -1, 1, 3}, {"+789", 1, 1, 1, 3}, {"-", 1, -1, 1, 3}, {"+", 1, 1, 1, 3}, {"-0", 1, -1, 1, 3},
    {"+0", 1, 1, 1, 3}, {"-12", 1, -1, 1, 3}, {"+12", 1, 1, 1, 3}, {"-abc", 1, -1, 1, 3}, {"+abc", 1, 1, 1, 3},
    // 21-30: Character constraints (Error code 4) within limited parsing windows
    {"a", 1, 1, 0, 4}, {"-a", 2, -1, 1, 4}, {"+b", 2, 1, 1, 4}, {"12a3", 4, 1, 0, 0}, {"-12b3", 5, -1, 1, 0},
    {"xyz", 3, 1, 0, 4}, {"-.", 2, -1, 1, 4}, {"+*", 2, 1, 1, 4}, {"12a3", 3, 1, 0, 0}, {"-1a", 2, -1, 1, 0},
    // 31-40: Out-of-line allocations and strict length truncations
    {long_neg_valid, 31, -1, 1, 0}, {long_pos_valid, 31, 1, 1, 0}, {long_no_sign, 30, 1, 0, 0},
    {long_neg_valid, 1, -1, 1, 3}, {long_pos_valid, 1, 1, 1, 3}, {long_invalid_chr, 31, -1, 1, 0},
    {long_invalid_chr, 2, -1, 1, 0}, {"1234567890123456789", 19, 1, 0, 0},
    {"-123456789012345678", 19, -1, 1, 0}, {"+", 1, 1, 1, 3},
    // 41-80: Balanced uniform fillers satisfying CASE_CNT
    {"1", 1, 1, 0, 0}, {"2", 1, 1, 0, 0}, {"3", 1, 1, 0, 0}, {"4", 1, 1, 0, 0}, {"5", 1, 1, 0, 0},
    {"6", 1, 1, 0, 0}, {"7", 1, 1, 0, 0}, {"8", 1, 1, 0, 0}, {"9", 1, 1, 0, 0}, {"0", 1, 1, 0, 0},
    {"-1", 2, -1, 1, 0}, {"-2", 2, -1, 1, 0}, {"-3", 2, -1, 1, 0}, {"-4", 2, -1, 1, 0}, {"-5", 2, -1, 1, 0},
    {"-6", 2, -1, 1, 0}, {"-7", 2, -1, 1, 0}, {"-8", 2, -1, 1, 0}, {"-9", 2, -1, 1, 0}, {"-0", 2, -1, 1, 0},
    {"+1", 2, 1, 1, 0}, {"+2", 2, 1, 1, 0}, {"+3", 2, 1, 1, 0}, {"+4", 2, 1, 1, 0}, {"+5", 2, 1, 1, 0},
    {"+6", 2, 1, 1, 0}, {"+7", 2, 1, 1, 0}, {"+8", 2, 1, 1, 0}, {"+9", 2, 1, 1, 0}, {"+0", 2, 1, 1, 0},
    {"1", 1, 1, 0, 0}, {"2", 1, 1, 0, 0}, {"3", 1, 1, 0, 0}, {"4", 1, 1, 0, 0}, {"5", 1, 1, 0, 0},
    {"6", 1, 1, 0, 0}, {"7", 1, 1, 0, 0}, {"8", 1, 1, 0, 0}, {"9", 1, 1, 0, 0}, {"0", 1, 1, 0, 0}
};



int main(void) { _libdnml_init();
    int total_tests = 0, passed_tests = 0;
    struct timespec start, end; clock_gettime(CLOCK_MONOTONIC, &start);
    fputs("===============================================================\n", stdout);
    fputs("      RUNNING INTEGRATED UNIT TESTS - STRING SIGN HANDLING     \n", stdout);
    fputs("===============================================================\n", stdout);
    fputs("\033[1;34m[NOTE]: Res and Exp output is in the form of {sign, pos, err}\033[0m\n", stdout);
    fputs("---- _sign_handle_ -----\n", stdout);
    for (int i = 0; i < CASE_CNT; ++i) { ++total_tests;
        const char *str = sign_cases[i].str; int8_t exp_sign = sign_cases[i].exp_sign;
        size_t exp_pos = sign_cases[i].exp_pos; uint8_t exp_err = sign_cases[i].exp_err;
        int8_t ret_sign = 1; size_t ret_pos = 0;
        uint8_t ret_err = _sign_handle_(str, &ret_pos, &ret_sign);
        if (ret_sign == exp_sign && ret_pos == exp_pos && ret_err == exp_err) ++passed_tests;
        else printf("[FAIL] _sign_handle_ | Case %-2d "
            "| Res: {%-2" PRId8 ", %-2zu, %-1" PRIu8 "} "
            "| Exp: {%-2" PRId8 ", %-2zu, %-1" PRIu8 "} "
            "| STRING: %s\n",
            i + 1, ret_sign, ret_pos, ret_err, exp_sign, exp_pos, exp_err, str
        );
    }
    fputs("---- _sign_handle_nlen_ -----\n", stdout);
    for (int i = 0; i < CASE_CNT; ++i) { ++total_tests;
        const char *str = sign_nlen_cases[i].str; size_t len = sign_nlen_cases[i].len;
        int8_t exp_sign = sign_nlen_cases[i].exp_sign; size_t exp_pos = sign_nlen_cases[i].exp_pos;
        uint8_t exp_err = sign_nlen_cases[i].exp_err; int8_t ret_sign = 1; size_t ret_pos = 0;
        uint8_t ret_err = _sign_handle_nlen_(str, &ret_pos, &ret_sign, len);
        if (ret_sign == exp_sign && ret_pos == exp_pos && ret_err == exp_err) ++passed_tests;
        else printf("[FAIL] _sign_handle_nlen_ | Case %-2d "
            "| Res: {%-2" PRId8 ", %-2zu, %-1" PRIu8 "} "
            "| Exp: {%-2" PRId8 ", %-2zu, %-1" PRIu8 "} "
            "| STRING (len = %-3zu): %s\n",
            i + 1, ret_sign, ret_pos, ret_err, exp_sign, exp_pos, exp_err, len, str
        );
    }

    #undef long_neg_valid
    #undef long_pos_valid
    #undef long_no_sign
    #undef long_invalid_chr
    #undef long_naked_plus
    #undef long_naked_minus
    #undef CASE_CNT
    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    fputs( "=========================================================\n", stdout);
    fputs( "TEST SUMMARY:\n", stdout);
    printf("+) Passed %-4d out of %-4d total compiled checks.\n", passed_tests, total_tests);
    printf("+) Success rate: %.2f%%\n", (passed_tests * 100.0) / total_tests);
    printf("+) Total Runtime: %lf ms\n", elapsed_time * 1000.0);
    fputs("=========================================================\n", stdout);
    _libdnml_cleanup(); return (passed_tests == total_tests) ? 0 : 1;
}
