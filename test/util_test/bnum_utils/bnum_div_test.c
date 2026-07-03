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
#define CASE_CNT 60
#define MAX_SIZE_T 12
typedef struct { const bigInt in; uint64_t optional_val; const bigInt exp; } div_case_t;
//* =============== GLOBAL ARRAY OF CASES =============== *//
static const div_case_t div3_cases[CASE_CNT] = {
    { .in = { .limbs = NULL, .n = 0 }, .optional_val = 0, .exp = { .limbs = NULL, .n = 0 } }, // 0
    { .in = { .limbs = (limb_t[]){3}, .n = 1 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){1}, .n = 1 } }, // 1
    { .in = { .limbs = (limb_t[]){6}, .n = 1 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){2}, .n = 1 } }, // 2
    { .in = { .limbs = (limb_t[]){9}, .n = 1 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){3}, .n = 1 } }, // 3
    { .in = { .limbs = (limb_t[]){12}, .n = 1 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){4}, .n = 1 } }, // 4
    { .in = { .limbs = (limb_t[]){15}, .n = 1 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){5}, .n = 1 } }, // 5
    { .in = { .limbs = (limb_t[]){18}, .n = 1 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){6}, .n = 1 } }, // 6
    { .in = { .limbs = (limb_t[]){21}, .n = 1 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){7}, .n = 1 } }, // 7
    { .in = { .limbs = (limb_t[]){24}, .n = 1 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){8}, .n = 1 } }, // 8
    { .in = { .limbs = (limb_t[]){27}, .n = 1 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){9}, .n = 1 } }, // 9
    { .in = { .limbs = (limb_t[]){30}, .n = 1 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){10}, .n = 1 } }, // 10
    { .in = { .limbs = (limb_t[]){33}, .n = 1 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){11}, .n = 1 } }, // 11
    { .in = { .limbs = (limb_t[]){36}, .n = 1 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){12}, .n = 1 } }, // 12
    { .in = { .limbs = (limb_t[]){39}, .n = 1 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){13}, .n = 1 } }, // 13
    { .in = { .limbs = (limb_t[]){42}, .n = 1 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){14}, .n = 1 } }, // 14
    { .in = { .limbs = (limb_t[]){45}, .n = 1 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){15}, .n = 1 } }, // 15
    { .in = { .limbs = (limb_t[]){48}, .n = 1 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){16}, .n = 1 } }, // 16
    { .in = { .limbs = (limb_t[]){51}, .n = 1 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){17}, .n = 1 } }, // 17
    { .in = { .limbs = (limb_t[]){54}, .n = 1 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){18}, .n = 1 } }, // 18
    { .in = { .limbs = (limb_t[]){57}, .n = 1 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){19}, .n = 1 } }, // 19
    { .in = { .limbs = (limb_t[]){60}, .n = 1 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){20}, .n = 1 } }, // 20
    { .in = { .limbs = (limb_t[]){63}, .n = 1 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){21}, .n = 1 } }, // 21
    { .in = { .limbs = (limb_t[]){66}, .n = 1 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){22}, .n = 1 } }, // 22
    { .in = { .limbs = (limb_t[]){69}, .n = 1 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){23}, .n = 1 } }, // 23
    { .in = { .limbs = (limb_t[]){72}, .n = 1 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){24}, .n = 1 } }, // 24
    { .in = { .limbs = (limb_t[]){75}, .n = 1 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){25}, .n = 1 } }, // 25
    { .in = { .limbs = (limb_t[]){78}, .n = 1 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){26}, .n = 1 } }, // 26
    { .in = { .limbs = (limb_t[]){81}, .n = 1 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){27}, .n = 1 } }, // 27
    { .in = { .limbs = (limb_t[]){84}, .n = 1 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){28}, .n = 1 } }, // 28
    { .in = { .limbs = (limb_t[]){87}, .n = 1 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){29}, .n = 1 } }, // 29
    { .in = { .limbs = (limb_t[]){0, 3}, .n = 2 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){0, 1}, .n = 2 } }, // 30
    { .in = { .limbs = (limb_t[]){3, 3}, .n = 2 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){1, 1}, .n = 2 } }, // 31
    { .in = { .limbs = (limb_t[]){6, 3}, .n = 2 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){2, 1}, .n = 2 } }, // 32
    { .in = { .limbs = (limb_t[]){9, 3}, .n = 2 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){3, 1}, .n = 2 } }, // 33
    { .in = { .limbs = (limb_t[]){12, 3}, .n = 2 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){4, 1}, .n = 2 } }, // 34
    { .in = { .limbs = (limb_t[]){15, 3}, .n = 2 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){5, 1}, .n = 2 } }, // 35
    { .in = { .limbs = (limb_t[]){18, 3}, .n = 2 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){6, 1}, .n = 2 } }, // 36
    { .in = { .limbs = (limb_t[]){21, 3}, .n = 2 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){7, 1}, .n = 2 } }, // 37
    { .in = { .limbs = (limb_t[]){24, 3}, .n = 2 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){8, 1}, .n = 2 } }, // 38
    { .in = { .limbs = (limb_t[]){27, 3}, .n = 2 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){9, 1}, .n = 2 } }, // 39
    { .in = { .limbs = (limb_t[]){30, 3}, .n = 2 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){10, 1}, .n = 2 } }, // 40
    { .in = { .limbs = (limb_t[]){33, 3}, .n = 2 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){11, 1}, .n = 2 } }, // 41
    { .in = { .limbs = (limb_t[]){36, 3}, .n = 2 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){12, 1}, .n = 2 } }, // 42
    { .in = { .limbs = (limb_t[]){39, 3}, .n = 2 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){13, 1}, .n = 2 } }, // 43
    { .in = { .limbs = (limb_t[]){42, 3}, .n = 2 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){14, 1}, .n = 2 } }, // 44
    { .in = { .limbs = (limb_t[]){45, 3}, .n = 2 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){15, 1}, .n = 2 } }, // 45
    { .in = { .limbs = (limb_t[]){48, 3}, .n = 2 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){16, 1}, .n = 2 } }, // 46
    { .in = { .limbs = (limb_t[]){51, 3}, .n = 2 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){17, 1}, .n = 2 } }, // 47
    { .in = { .limbs = (limb_t[]){54, 3}, .n = 2 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){18, 1}, .n = 2 } }, // 48
    { .in = { .limbs = (limb_t[]){57, 3}, .n = 2 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){19, 1}, .n = 2 } }, // 49
    { .in = { .limbs = (limb_t[]){0, 0, 3}, .n = 3 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){0, 0, 1}, .n = 3 } }, // 50
    { .in = { .limbs = (limb_t[]){3, 0, 3}, .n = 3 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){1, 0, 1}, .n = 3 } }, // 51
    { .in = { .limbs = (limb_t[]){6, 0, 3}, .n = 3 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){2, 0, 1}, .n = 3 } }, // 52
    { .in = { .limbs = (limb_t[]){9, 0, 3}, .n = 3 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){3, 0, 1}, .n = 3 } }, // 53
    { .in = { .limbs = (limb_t[]){12, 0, 3}, .n = 3 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){4, 0, 1}, .n = 3 } }, // 54
    { .in = { .limbs = (limb_t[]){15, 0, 3}, .n = 3 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){5, 0, 1}, .n = 3 } }, // 55
    { .in = { .limbs = (limb_t[]){18, 0, 3}, .n = 3 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){6, 0, 1}, .n = 3 } }, // 56
    { .in = { .limbs = (limb_t[]){21, 0, 3}, .n = 3 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){7, 0, 1}, .n = 3 } }, // 57
    { .in = { .limbs = (limb_t[]){24, 0, 3}, .n = 3 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){8, 0, 1}, .n = 3 } }, // 58
    { .in = { .limbs = (limb_t[]){27, 0, 3}, .n = 3 }, .optional_val = 0, .exp = { .limbs = (limb_t[]){9, 0, 1}, .n = 3 } } // 59
};
static const div_case_t divmod_cases[CASE_CNT] = {
    { .in = { .limbs = NULL, .n = 0 }, .optional_val = 1, .exp = { .limbs = NULL, .n = 0 } }, // 0
    { .in = { .limbs = (limb_t[]){10}, .n = 1 }, .optional_val = 1, .exp = { .limbs = (limb_t[]){10}, .n = 1 } }, // 1
    { .in = { .limbs = NULL, .n = 0 }, .optional_val = 2, .exp = { .limbs = NULL, .n = 0 } }, // 2
    { .in = { .limbs = (limb_t[]){5}, .n = 1 }, .optional_val = 2, .exp = { .limbs = (limb_t[]){2}, .n = 1 } }, // 3
    { .in = { .limbs = (limb_t[]){100}, .n = 1 }, .optional_val = 2, .exp = { .limbs = (limb_t[]){50}, .n = 1 } }, // 4
    { .in = { .limbs = (limb_t[]){0, 1}, .n = 2 }, .optional_val = 2, .exp = { .limbs = (limb_t[]){SIGN_BIT_MASK}, .n = 1 } }, // 5
    { .in = { .limbs = (limb_t[]){0, 2}, .n = 2 }, .optional_val = 2, .exp = { .limbs = (limb_t[]){0, 1}, .n = 2 } }, // 6
    { .in = { .limbs = NULL, .n = 0 }, .optional_val = 4, .exp = { .limbs = NULL, .n = 0 } }, // 7
    { .in = { .limbs = (limb_t[]){13}, .n = 1 }, .optional_val = 4, .exp = { .limbs = (limb_t[]){3}, .n = 1 } }, // 8
    { .in = { .limbs = (limb_t[]){0, 1}, .n = 2 }, .optional_val = 4, .exp = { .limbs = (limb_t[]){SIGN_BIT_MASK >> 1}, .n = 1 } }, // 9
    { .in = { .limbs = NULL, .n = 0 }, .optional_val = 5, .exp = { .limbs = NULL, .n = 0 } }, // 10
    { .in = { .limbs = (limb_t[]){25}, .n = 1 }, .optional_val = 5, .exp = { .limbs = (limb_t[]){5}, .n = 1 } }, // 11
    { .in = { .limbs = (limb_t[]){12345}, .n = 1 }, .optional_val = 5, .exp = { .limbs = (limb_t[]){2469}, .n = 1 } }, // 12
    { .in = { .limbs = (limb_t[]){0, 5}, .n = 2 }, .optional_val = 5, .exp = { .limbs = (limb_t[]){0, 1}, .n = 2 } }, // 13
    { .in = { .limbs = (limb_t[]){5, 5}, .n = 2 }, .optional_val = 5, .exp = { .limbs = (limb_t[]){1, 1}, .n = 2 } }, // 14
    { .in = { .limbs = (limb_t[]){100}, .n = 1 }, .optional_val = 3, .exp = { .limbs = (limb_t[]){33}, .n = 1 } }, // 15
    { .in = { .limbs = (limb_t[]){100}, .n = 1 }, .optional_val = 6, .exp = { .limbs = (limb_t[]){16}, .n = 1 } }, // 16
    { .in = { .limbs = (limb_t[]){100}, .n = 1 }, .optional_val = 7, .exp = { .limbs = (limb_t[]){14}, .n = 1 } }, // 17
    { .in = { .limbs = (limb_t[]){100}, .n = 1 }, .optional_val = 8, .exp = { .limbs = (limb_t[]){12}, .n = 1 } }, // 18
    { .in = { .limbs = (limb_t[]){100}, .n = 1 }, .optional_val = 9, .exp = { .limbs = (limb_t[]){11}, .n = 1 } }, // 19
    { .in = { .limbs = (limb_t[]){1000}, .n = 1 }, .optional_val = 10, .exp = { .limbs = (limb_t[]){100}, .n = 1 } }, // 20
    { .in = { .limbs = (limb_t[]){1000}, .n = 1 }, .optional_val = 11, .exp = { .limbs = (limb_t[]){90}, .n = 1 } }, // 21
    { .in = { .limbs = (limb_t[]){1000}, .n = 1 }, .optional_val = 12, .exp = { .limbs = (limb_t[]){83}, .n = 1 } }, // 22
    { .in = { .limbs = (limb_t[]){1000}, .n = 1 }, .optional_val = 13, .exp = { .limbs = (limb_t[]){76}, .n = 1 } }, // 23
    { .in = { .limbs = (limb_t[]){1000}, .n = 1 }, .optional_val = 25, .exp = { .limbs = (limb_t[]){40}, .n = 1 } }, // 24
    { .in = { .limbs = (limb_t[]){10000}, .n = 1 }, .optional_val = 100, .exp = { .limbs = (limb_t[]){100}, .n = 1 } }, // 25
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL}, .n = 1 }, .optional_val = 3, .exp = { .limbs = (limb_t[]){0x5555555555555555ULL}, .n = 1 } }, // 26
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL}, .n = 1 }, .optional_val = 5, .exp = { .limbs = (limb_t[]){0x3333333333333333ULL}, .n = 1 } }, // 27
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL}, .n = 1 }, .optional_val = 17, .exp = { .limbs = (limb_t[]){0x0F0F0F0F0F0F0F0FULL}, .n = 1 } }, // 28
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL}, .n = 1 }, .optional_val = 257, .exp = { .limbs = (limb_t[]){0x00FF00FF00FF00FFULL}, .n = 1 } }, // 29
    { .in = { .limbs = (limb_t[]){0, 4}, .n = 2 }, .optional_val = 2, .exp = { .limbs = (limb_t[]){0, 2}, .n = 2 } }, // 30
    { .in = { .limbs = (limb_t[]){1, 4}, .n = 2 }, .optional_val = 2, .exp = { .limbs = (limb_t[]){0, 2}, .n = 2 } }, // 31
    { .in = { .limbs = (limb_t[]){2, 4}, .n = 2 }, .optional_val = 2, .exp = { .limbs = (limb_t[]){1, 2}, .n = 2 } }, // 32
    { .in = { .limbs = (limb_t[]){3, 4}, .n = 2 }, .optional_val = 2, .exp = { .limbs = (limb_t[]){1, 2}, .n = 2 } }, // 33
    { .in = { .limbs = (limb_t[]){0, 3}, .n = 2 }, .optional_val = 3, .exp = { .limbs = (limb_t[]){0, 1}, .n = 2 } }, // 34
    { .in = { .limbs = (limb_t[]){3, 6}, .n = 2 }, .optional_val = 3, .exp = { .limbs = (limb_t[]){1, 2}, .n = 2 } }, // 35
    { .in = { .limbs = (limb_t[]){1, 3}, .n = 2 }, .optional_val = 3, .exp = { .limbs = (limb_t[]){0, 1}, .n = 2 } }, // 36
    { .in = { .limbs = (limb_t[]){2, 3}, .n = 2 }, .optional_val = 3, .exp = { .limbs = (limb_t[]){0, 1}, .n = 2 } }, // 37
    { .in = { .limbs = (limb_t[]){0, 10}, .n = 2 }, .optional_val = 10, .exp = { .limbs = (limb_t[]){0, 1}, .n = 2 } }, // 38
    { .in = { .limbs = (limb_t[]){10, 20}, .n = 2 }, .optional_val = 10, .exp = { .limbs = (limb_t[]){1, 2}, .n = 2 } }, // 39
    { .in = { .limbs = (limb_t[]){0, 1}, .n = 2 }, .optional_val = 3, .exp = { .limbs = (limb_t[]){0x5555555555555555ULL}, .n = 1 } }, // 40
    { .in = { .limbs = (limb_t[]){0, 2}, .n = 2 }, .optional_val = 3, .exp = { .limbs = (limb_t[]){0xAAAAAAAAAAAAAAAAULL}, .n = 1 } }, // 41
    { .in = { .limbs = (limb_t[]){0, 1}, .n = 2 }, .optional_val = 5, .exp = { .limbs = (limb_t[]){0x3333333333333333ULL}, .n = 1 } }, // 42
    { .in = { .limbs = (limb_t[]){0, 2}, .n = 2 }, .optional_val = 5, .exp = { .limbs = (limb_t[]){0x6666666666666666ULL}, .n = 1 } }, // 43
    { .in = { .limbs = (limb_t[]){0, 3}, .n = 2 }, .optional_val = 5, .exp = { .limbs = (limb_t[]){0x9999999999999999ULL}, .n = 1 } }, // 44
    { .in = { .limbs = (limb_t[]){0, 4}, .n = 2 }, .optional_val = 5, .exp = { .limbs = (limb_t[]){0xCCCCCCCCCCCCCCCCULL}, .n = 1 } }, // 45
    { .in = { .limbs = (limb_t[]){0, 1}, .n = 2 }, .optional_val = 10, .exp = { .limbs = (limb_t[]){0x1999999999999999ULL}, .n = 1 } }, // 46
    { .in = { .limbs = (limb_t[]){0, 1}, .n = 2 }, .optional_val = 16, .exp = { .limbs = (limb_t[]){1ULL << 60}, .n = 1 } }, // 47
    { .in = { .limbs = (limb_t[]){0, 1}, .n = 2 }, .optional_val = 64, .exp = { .limbs = (limb_t[]){1ULL << 58}, .n = 1 } }, // 48
    { .in = { .limbs = (limb_t[]){0, 1}, .n = 2 }, .optional_val = 256, .exp = { .limbs = (limb_t[]){1ULL << 56}, .n = 1 } }, // 49
    { .in = { .limbs = (limb_t[]){0, 0, 1}, .n = 3 }, .optional_val = 2, .exp = { .limbs = (limb_t[]){0, SIGN_BIT_MASK}, .n = 2 } }, // 50
    { .in = { .limbs = (limb_t[]){0, 0, 2}, .n = 3 }, .optional_val = 2, .exp = { .limbs = (limb_t[]){0, 0, 1}, .n = 3 } }, // 51
    { .in = { .limbs = (limb_t[]){0, 0, 3}, .n = 3 }, .optional_val = 3, .exp = { .limbs = (limb_t[]){0, 0, 1}, .n = 3 } }, // 52
    { .in = { .limbs = (limb_t[]){0, 0, 1}, .n = 3 }, .optional_val = 4, .exp = { .limbs = (limb_t[]){0, SIGN_BIT_MASK >> 1}, .n = 2 } }, // 53
    { .in = { .limbs = (limb_t[]){0, 0, 1}, .n = 3 }, .optional_val = 16, .exp = { .limbs = (limb_t[]){0, 1ULL << 60}, .n = 2 } }, // 54
    { .in = { .limbs = (limb_t[]){0, 0, 1}, .n = 3 }, .optional_val = 3, .exp = { .limbs = (limb_t[]){0x5555555555555555ULL, 0x5555555555555555ULL}, .n = 2 } }, // 55
    { .in = { .limbs = (limb_t[]){0, 0, 2}, .n = 3 }, .optional_val = 3, .exp = { .limbs = (limb_t[]){0xAAAAAAAAAAAAAAAAULL, 0xAAAAAAAAAAAAAAAAULL}, .n = 2 } }, // 56
    { .in = { .limbs = (limb_t[]){1, 2, 3}, .n = 3 }, .optional_val = 1, .exp = { .limbs = (limb_t[]){1, 2, 3}, .n = 3 } }, // 57
    { .in = { .limbs = (limb_t[]){2, 4, 6}, .n = 3 }, .optional_val = 2, .exp = { .limbs = (limb_t[]){1, 2, 3}, .n = 3 } }, // 58
    { .in = { .limbs = (limb_t[]){3, 6, 9}, .n = 3 }, .optional_val = 3, .exp = { .limbs = (limb_t[]){1, 2, 3}, .n = 3 } } // 59
};


int main(void) { _libdnml_init();
    int total_tests = 0, passed_tests = 0;
    struct timespec start, end; clock_gettime(CLOCK_MONOTONIC, &start);
    limb_t *ret_buf = (limb_t *)malloc(MAX_SIZE_T * U64_BYTES); assert(ret_buf != NULL);
    fputs("====================================================================\n", stdout);
    fputs("      RUNNING INTEGRATED UNIT TESTS - BIGNUM DIVISION UTILITIES     \n", stdout);
    fputs("====================================================================\n", stdout);
    fputs("---- __BIGINT_DIV3__ -----\n", stdout);
    for (int i = 0; i < CASE_CNT; i++) { total_tests++; 
        memset(ret_buf, 0, MAX_SIZE_T * U64_BYTES);
        if (div3_cases[i].in.limbs && div3_cases[i].in.n > 0) {
            memcpy(ret_buf, div3_cases[i].in.limbs, div3_cases[i].in.n * U64_BYTES);
        }
        bigInt test_x; test_x.limbs = ret_buf;
        test_x.n = div3_cases[i].in.n; test_x.cap = MAX_SIZE_T;
        __BIGINT_DIV3__(&test_x);
        int match = (test_x.n == div3_cases[i].exp.n);
        if (match) match = memcmp(test_x.limbs, div3_cases[i].exp.limbs, div3_cases[i].exp.n * U64_BYTES) == 0;
        if (match) passed_tests++;
        else printf("[FAIL] DIV3 Case %2d: Missed structural matching requirements.\n", i);
    }
    fputs("---- __BIGINT_INTERNAL_DIVMOD_UI64__ -----\n", stdout);
    for (int i = 0; i < CASE_CNT; i++) { total_tests++; 
        memset(ret_buf, 0, MAX_SIZE_T * U64_BYTES);
        if (divmod_cases[i].in.limbs && divmod_cases[i].in.n > 0) {
            memcpy(ret_buf, divmod_cases[i].in.limbs, divmod_cases[i].in.n * U64_BYTES);
        }
        bigInt test_x; test_x.limbs = ret_buf;
        test_x.n = divmod_cases[i].in.n; test_x.cap = MAX_SIZE_T;
        __BIGINT_INTERNAL_DIVMOD_UI64__(&test_x, divmod_cases[i].optional_val);
        int match = (test_x.n == divmod_cases[i].exp.n);
        if (match) match = memcmp(test_x.limbs, divmod_cases[i].exp.limbs, divmod_cases[i].exp.n * U64_BYTES) == 0;
        if (match) passed_tests++;
        else printf("[FAIL] DIVMOD Case %2d: Missed structural matching requirements.\n", i);
    }

    /* Summary output block */
    #undef MAX_SIZE_T
    #undef CASE_CNT
    clock_gettime(CLOCK_MONOTONIC, &end); free(ret_buf);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    fputs( "=========================================================\n", stdout);
    fputs( "TEST SUMMARY:\n", stdout);
    printf("+) Passed %-4d out of %-4d total compiled checks.\n", passed_tests, total_tests);
    printf("+) Success rate: %.2f%%\n", (total_tests > 0) ? ((passed_tests * 100.0) / total_tests) : 0.0);
    printf("+) Total Runtime: %lf ms\n", elapsed_time * 1000.0);
    fputs( "=========================================================\n", stdout);
    _libdnml_cleanup() ;return (passed_tests == total_tests) ? 0 : 1;
}
