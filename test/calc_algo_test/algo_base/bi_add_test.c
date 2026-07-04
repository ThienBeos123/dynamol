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
#include <_libdnml_mem/arena.h>
#include <_libdnml_mem/_ctx.h>
#include "../../../libdnml_base.h"
#include "../../../calc_algo/algo_base/add_sub.h"
#include "../../test_utils.h"
#define CASE_CNT 60
#define MAX_SIZE 12
#define BUF_SIZE 24
typedef struct { const bigInt a; const bigInt b; const bigInt exp; } bi_add_case;
typedef struct { bigInt dst; const bigInt src; size_t limb_shift; const bigInt exp; } bi_adds_case;
//* ================= LIMB ARRAYS ================= *//
// Deterministic Input & Expected Vectors (Fully Unrolled)
static const limb_t Z_1[1] = { UINT64_C(0x0000000000000000) };
static const limb_t C_1[1] = { UINT64_C(0x0000000000000001) };
static const limb_t C_2[2] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000001) };
static const limb_t C_3[3] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000001) };
static const limb_t C_4[4] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000001) };
static const limb_t C_5[5] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000001) };
static const limb_t C_6[6] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000001) };
static const limb_t C_7[7] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000001) };
static const limb_t M_1[1] = { UINT64_C(0xFFFFFFFFFFFFFFFF) };
static const limb_t M_2[2] = { UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF) };
static const limb_t M_3[3] = { UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF) };
static const limb_t M_4[4] = { UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF) };
static const limb_t M_5[5] = { UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF) };
static const limb_t M_6[6] = { UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF) };
static const limb_t A_1[1] = { UINT64_C(0xAAAAAAAAAAAAAAAA) };
static const limb_t A_2[2] = { UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0xAAAAAAAAAAAAAAAA) };
static const limb_t A_3[3] = { UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0xAAAAAAAAAAAAAAAA) };
static const limb_t A_4[4] = { UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0xAAAAAAAAAAAAAAAA) };
static const limb_t A_5[5] = { UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0xAAAAAAAAAAAAAAAA) };
static const limb_t A_6[6] = { UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0xAAAAAAAAAAAAAAAA) };
static const limb_t F_1[1] = { UINT64_C(0x5555555555555555) };
static const limb_t F_2[2] = { UINT64_C(0x5555555555555555), UINT64_C(0x5555555555555555) };
static const limb_t F_3[3] = { UINT64_C(0x5555555555555555), UINT64_C(0x5555555555555555), UINT64_C(0x5555555555555555) };
static const limb_t F_4[4] = { UINT64_C(0x5555555555555555), UINT64_C(0x5555555555555555), UINT64_C(0x5555555555555555), UINT64_C(0x5555555555555555) };
static const limb_t F_5[5] = { UINT64_C(0x5555555555555555), UINT64_C(0x5555555555555555), UINT64_C(0x5555555555555555), UINT64_C(0x5555555555555555), UINT64_C(0x5555555555555555) };
static const limb_t F_6[6] = { UINT64_C(0x5555555555555555), UINT64_C(0x5555555555555555), UINT64_C(0x5555555555555555), UINT64_C(0x5555555555555555), UINT64_C(0x5555555555555555), UINT64_C(0x5555555555555555) };
static const limb_t Mm1_1[1] = { UINT64_C(0xFFFFFFFFFFFFFFFE) };
static const limb_t Mm1_2[2] = { UINT64_C(0xFFFFFFFFFFFFFFFE), UINT64_C(0xFFFFFFFFFFFFFFFF) };
static const limb_t Mm1_3[3] = { UINT64_C(0xFFFFFFFFFFFFFFFE), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF) };
static const limb_t Mm1_4[4] = { UINT64_C(0xFFFFFFFFFFFFFFFE), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF) };
static const limb_t Mm1_5[5] = { UINT64_C(0xFFFFFFFFFFFFFFFE), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF) };
static const limb_t Mm1_6[6] = { UINT64_C(0xFFFFFFFFFFFFFFFE), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF) };



//* =================== GLOBAL ARRAY OF CASES =================== *//
/* ---- __BIGINT_ADD_WC cases ---- */
static const bi_add_case add_cases[CASE_CNT] = {
    { { .n=1, .cap=1, .limbs=(limb_t*)M_1, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=1 }, { .n=2, .cap=2, .limbs=(limb_t*)C_2, .sign=1 } },   // 1
    { { .n=2, .cap=2, .limbs=(limb_t*)M_2, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=1 }, { .n=3, .cap=3, .limbs=(limb_t*)C_3, .sign=1 } },   // 2
    { { .n=3, .cap=3, .limbs=(limb_t*)M_3, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=1 }, { .n=4, .cap=4, .limbs=(limb_t*)C_4, .sign=1 } },   // 3
    { { .n=4, .cap=4, .limbs=(limb_t*)M_4, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=1 }, { .n=5, .cap=5, .limbs=(limb_t*)C_5, .sign=1 } },   // 4
    { { .n=5, .cap=5, .limbs=(limb_t*)M_5, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=1 }, { .n=6, .cap=6, .limbs=(limb_t*)C_6, .sign=1 } },   // 5
    { { .n=6, .cap=6, .limbs=(limb_t*)M_6, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=1 }, { .n=7, .cap=7, .limbs=(limb_t*)C_7, .sign=1 } },   // 6
    { { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)M_1, .sign=1 }, { .n=2, .cap=2, .limbs=(limb_t*)C_2, .sign=1 } },   // 7
    { { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=1 }, { .n=2, .cap=2, .limbs=(limb_t*)M_2, .sign=1 }, { .n=3, .cap=3, .limbs=(limb_t*)C_3, .sign=1 } },   // 8
    { { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=1 }, { .n=3, .cap=3, .limbs=(limb_t*)M_3, .sign=1 }, { .n=4, .cap=4, .limbs=(limb_t*)C_4, .sign=1 } },   // 9
    { { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=1 }, { .n=4, .cap=4, .limbs=(limb_t*)M_4, .sign=1 }, { .n=5, .cap=5, .limbs=(limb_t*)C_5, .sign=1 } },   // 10
    { { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=1 }, { .n=5, .cap=5, .limbs=(limb_t*)M_5, .sign=1 }, { .n=6, .cap=6, .limbs=(limb_t*)C_6, .sign=1 } },   // 11
    { { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=1 }, { .n=6, .cap=6, .limbs=(limb_t*)M_6, .sign=1 }, { .n=7, .cap=7, .limbs=(limb_t*)C_7, .sign=1 } },   // 12
    { { .n=1, .cap=1, .limbs=(limb_t*)A_1, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)F_1, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)M_1, .sign=1 } },   // 13
    { { .n=2, .cap=2, .limbs=(limb_t*)A_2, .sign=1 }, { .n=2, .cap=2, .limbs=(limb_t*)F_2, .sign=1 }, { .n=2, .cap=2, .limbs=(limb_t*)M_2, .sign=1 } },   // 14
    { { .n=3, .cap=3, .limbs=(limb_t*)A_3, .sign=1 }, { .n=3, .cap=3, .limbs=(limb_t*)F_3, .sign=1 }, { .n=3, .cap=3, .limbs=(limb_t*)M_3, .sign=1 } },   // 15
    { { .n=4, .cap=4, .limbs=(limb_t*)A_4, .sign=1 }, { .n=4, .cap=4, .limbs=(limb_t*)F_4, .sign=1 }, { .n=4, .cap=4, .limbs=(limb_t*)M_4, .sign=1 } },   // 16
    { { .n=5, .cap=5, .limbs=(limb_t*)A_5, .sign=1 }, { .n=5, .cap=5, .limbs=(limb_t*)F_5, .sign=1 }, { .n=5, .cap=5, .limbs=(limb_t*)M_5, .sign=1 } },   // 17
    { { .n=6, .cap=6, .limbs=(limb_t*)A_6, .sign=1 }, { .n=6, .cap=6, .limbs=(limb_t*)F_6, .sign=1 }, { .n=6, .cap=6, .limbs=(limb_t*)M_6, .sign=1 } },   // 18
    { { .n=1, .cap=1, .limbs=(limb_t*)F_1, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)A_1, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)M_1, .sign=1 } },   // 19
    { { .n=2, .cap=2, .limbs=(limb_t*)F_2, .sign=1 }, { .n=2, .cap=2, .limbs=(limb_t*)A_2, .sign=1 }, { .n=2, .cap=2, .limbs=(limb_t*)M_2, .sign=1 } },   // 20
    { { .n=3, .cap=3, .limbs=(limb_t*)F_3, .sign=1 }, { .n=3, .cap=3, .limbs=(limb_t*)A_3, .sign=1 }, { .n=3, .cap=3, .limbs=(limb_t*)M_3, .sign=1 } },   // 21
    { { .n=4, .cap=4, .limbs=(limb_t*)F_4, .sign=1 }, { .n=4, .cap=4, .limbs=(limb_t*)A_4, .sign=1 }, { .n=4, .cap=4, .limbs=(limb_t*)M_4, .sign=1 } },   // 22
    { { .n=5, .cap=5, .limbs=(limb_t*)F_5, .sign=1 }, { .n=5, .cap=5, .limbs=(limb_t*)A_5, .sign=1 }, { .n=5, .cap=5, .limbs=(limb_t*)M_5, .sign=1 } },   // 23
    { { .n=6, .cap=6, .limbs=(limb_t*)F_6, .sign=1 }, { .n=6, .cap=6, .limbs=(limb_t*)A_6, .sign=1 }, { .n=6, .cap=6, .limbs=(limb_t*)M_6, .sign=1 } },   // 24
    { { .n=1, .cap=1, .limbs=(limb_t*)M_1, .sign=1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)M_1, .sign=1 } },   // 25
    { { .n=2, .cap=2, .limbs=(limb_t*)M_2, .sign=1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=2, .cap=2, .limbs=(limb_t*)M_2, .sign=1 } },   // 26
    { { .n=3, .cap=3, .limbs=(limb_t*)M_3, .sign=1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=3, .cap=3, .limbs=(limb_t*)M_3, .sign=1 } },   // 27
    { { .n=4, .cap=4, .limbs=(limb_t*)M_4, .sign=1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=4, .cap=4, .limbs=(limb_t*)M_4, .sign=1 } },   // 28
    { { .n=5, .cap=5, .limbs=(limb_t*)M_5, .sign=1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=5, .cap=5, .limbs=(limb_t*)M_5, .sign=1 } },   // 29
    { { .n=6, .cap=6, .limbs=(limb_t*)M_6, .sign=1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=6, .cap=6, .limbs=(limb_t*)M_6, .sign=1 } },   // 30
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)A_1, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)A_1, .sign=1 } },   // 31
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=2, .cap=2, .limbs=(limb_t*)A_2, .sign=1 }, { .n=2, .cap=2, .limbs=(limb_t*)A_2, .sign=1 } },   // 32
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=3, .cap=3, .limbs=(limb_t*)A_3, .sign=1 }, { .n=3, .cap=3, .limbs=(limb_t*)A_3, .sign=1 } },   // 33
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=4, .cap=4, .limbs=(limb_t*)A_4, .sign=1 }, { .n=4, .cap=4, .limbs=(limb_t*)A_4, .sign=1 } },   // 34
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=5, .cap=5, .limbs=(limb_t*)A_5, .sign=1 }, { .n=5, .cap=5, .limbs=(limb_t*)A_5, .sign=1 } },   // 35
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=6, .cap=6, .limbs=(limb_t*)A_6, .sign=1 }, { .n=6, .cap=6, .limbs=(limb_t*)A_6, .sign=1 } },   // 36
    { { .n=1, .cap=1, .limbs=(limb_t*)F_1, .sign=1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)F_1, .sign=1 } },   // 37
    { { .n=2, .cap=2, .limbs=(limb_t*)F_2, .sign=1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=2, .cap=2, .limbs=(limb_t*)F_2, .sign=1 } },   // 38
    { { .n=3, .cap=3, .limbs=(limb_t*)F_3, .sign=1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=3, .cap=3, .limbs=(limb_t*)F_3, .sign=1 } },   // 39
    { { .n=4, .cap=4, .limbs=(limb_t*)F_4, .sign=1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=4, .cap=4, .limbs=(limb_t*)F_4, .sign=1 } },   // 40
    { { .n=5, .cap=5, .limbs=(limb_t*)F_5, .sign=1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=5, .cap=5, .limbs=(limb_t*)F_5, .sign=1 } },   // 41
    { { .n=6, .cap=6, .limbs=(limb_t*)F_6, .sign=1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=6, .cap=6, .limbs=(limb_t*)F_6, .sign=1 } },   // 42
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=1 } },   // 43
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=2, .cap=2, .limbs=(limb_t*)C_2, .sign=1 }, { .n=2, .cap=2, .limbs=(limb_t*)C_2, .sign=1 } },   // 44
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=3, .cap=3, .limbs=(limb_t*)C_3, .sign=1 }, { .n=3, .cap=3, .limbs=(limb_t*)C_3, .sign=1 } },   // 45
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=4, .cap=4, .limbs=(limb_t*)C_4, .sign=1 }, { .n=4, .cap=4, .limbs=(limb_t*)C_4, .sign=1 } },   // 46
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=5, .cap=5, .limbs=(limb_t*)C_5, .sign=1 }, { .n=5, .cap=5, .limbs=(limb_t*)C_5, .sign=1 } },   // 47
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=6, .cap=6, .limbs=(limb_t*)C_6, .sign=1 }, { .n=6, .cap=6, .limbs=(limb_t*)C_6, .sign=1 } },   // 48
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=7, .cap=7, .limbs=(limb_t*)C_7, .sign=1 }, { .n=7, .cap=7, .limbs=(limb_t*)C_7, .sign=1 } },   // 49
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 } },   // 50
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)Mm1_1, .sign=1}, { .n=1, .cap=1, .limbs=(limb_t*)Mm1_1, .sign=1} }, // 51
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=2, .cap=2, .limbs=(limb_t*)Mm1_2, .sign=1}, { .n=2, .cap=2, .limbs=(limb_t*)Mm1_2, .sign=1} }, // 52
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=3, .cap=3, .limbs=(limb_t*)Mm1_3, .sign=1}, { .n=3, .cap=3, .limbs=(limb_t*)Mm1_3, .sign=1} }, // 53
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=4, .cap=4, .limbs=(limb_t*)Mm1_4, .sign=1}, { .n=4, .cap=4, .limbs=(limb_t*)Mm1_4, .sign=1} }  // 54
};

/* ----- __BIGINT_ADD_SHIFT__ cases ----- */
static const bi_adds_case add_shift_cases[CASE_CNT] = {
    { { .n=1, .cap=1, .limbs=(limb_t*)M_1, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)M_1, .sign=1 }, 1, { .n=2, .cap=2, .limbs=(limb_t*)M_2, .sign=1 } },// 0
    { { .n=1, .cap=1, .limbs=(limb_t*)M_1, .sign=1 }, { .n=2, .cap=2, .limbs=(limb_t*)M_2, .sign=1 }, 1, { .n=3, .cap=3, .limbs=(limb_t*)M_3, .sign=1 } },// 1
    { { .n=1, .cap=1, .limbs=(limb_t*)M_1, .sign=1 }, { .n=3, .cap=3, .limbs=(limb_t*)M_3, .sign=1 }, 1, { .n=4, .cap=4, .limbs=(limb_t*)M_4, .sign=1 } },// 2
    { { .n=1, .cap=1, .limbs=(limb_t*)M_1, .sign=1 }, { .n=4, .cap=4, .limbs=(limb_t*)M_4, .sign=1 }, 1, { .n=5, .cap=5, .limbs=(limb_t*)M_5, .sign=1 } },// 3
    { { .n=1, .cap=1, .limbs=(limb_t*)M_1, .sign=1 }, { .n=5, .cap=5, .limbs=(limb_t*)M_5, .sign=1 }, 1, { .n=6, .cap=6, .limbs=(limb_t*)M_6, .sign=1 } },// 4
    { { .n=2, .cap=2, .limbs=(limb_t*)A_2, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)A_1, .sign=1 }, 2, { .n=3, .cap=3, .limbs=(limb_t*)A_3, .sign=1 } },// 5
    { { .n=2, .cap=2, .limbs=(limb_t*)A_2, .sign=1 }, { .n=2, .cap=2, .limbs=(limb_t*)A_2, .sign=1 }, 2, { .n=4, .cap=4, .limbs=(limb_t*)A_4, .sign=1 } },// 6
    { { .n=2, .cap=2, .limbs=(limb_t*)A_2, .sign=1 }, { .n=3, .cap=3, .limbs=(limb_t*)A_3, .sign=1 }, 2, { .n=5, .cap=5, .limbs=(limb_t*)A_5, .sign=1 } },// 7
    { { .n=2, .cap=2, .limbs=(limb_t*)A_2, .sign=1 }, { .n=4, .cap=4, .limbs=(limb_t*)A_4, .sign=1 }, 2, { .n=6, .cap=6, .limbs=(limb_t*)A_6, .sign=1 } },// 8
    { { .n=1, .cap=1, .limbs=(limb_t*)A_1, .sign=1 }, { .n=4, .cap=4, .limbs=(limb_t*)A_4, .sign=1 }, 1, { .n=5, .cap=5, .limbs=(limb_t*)A_5, .sign=1 } },// 9
    { { .n=3, .cap=3, .limbs=(limb_t*)F_3, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)F_1, .sign=1 }, 3, { .n=4, .cap=4, .limbs=(limb_t*)F_4, .sign=1 } },// 10
    { { .n=3, .cap=3, .limbs=(limb_t*)F_3, .sign=1 }, { .n=2, .cap=2, .limbs=(limb_t*)F_2, .sign=1 }, 3, { .n=5, .cap=5, .limbs=(limb_t*)F_5, .sign=1 } },// 11
    { { .n=3, .cap=3, .limbs=(limb_t*)F_3, .sign=1 }, { .n=3, .cap=3, .limbs=(limb_t*)F_3, .sign=1 }, 3, { .n=6, .cap=6, .limbs=(limb_t*)F_6, .sign=1 } },// 12
    { { .n=1, .cap=1, .limbs=(limb_t*)F_1, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)F_1, .sign=1 }, 1, { .n=2, .cap=2, .limbs=(limb_t*)F_2, .sign=1 } },// 13
    { { .n=2, .cap=2, .limbs=(limb_t*)F_2, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)F_1, .sign=1 }, 2, { .n=3, .cap=3, .limbs=(limb_t*)F_3, .sign=1 } },// 14
    { { .n=1, .cap=1, .limbs=(limb_t*)M_1, .sign=1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, 1, { .n=1, .cap=1, .limbs=(limb_t*)M_1, .sign=1 } },// 15
    { { .n=2, .cap=2, .limbs=(limb_t*)M_2, .sign=1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, 2, { .n=2, .cap=2, .limbs=(limb_t*)M_2, .sign=1 } },// 16
    { { .n=3, .cap=3, .limbs=(limb_t*)M_3, .sign=1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, 3, { .n=3, .cap=3, .limbs=(limb_t*)M_3, .sign=1 } },// 17
    { { .n=4, .cap=4, .limbs=(limb_t*)M_4, .sign=1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, 4, { .n=4, .cap=4, .limbs=(limb_t*)M_4, .sign=1 } },// 18
    { { .n=5, .cap=5, .limbs=(limb_t*)M_5, .sign=1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, 5, { .n=5, .cap=5, .limbs=(limb_t*)M_5, .sign=1 } },// 19
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)M_1, .sign=1 }, 0, { .n=1, .cap=1, .limbs=(limb_t*)M_1, .sign=1 } },// 20
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=2, .cap=2, .limbs=(limb_t*)M_2, .sign=1 }, 0, { .n=2, .cap=2, .limbs=(limb_t*)M_2, .sign=1 } },// 21
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=3, .cap=3, .limbs=(limb_t*)M_3, .sign=1 }, 0, { .n=3, .cap=3, .limbs=(limb_t*)M_3, .sign=1 } },// 22
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=4, .cap=4, .limbs=(limb_t*)M_4, .sign=1 }, 0, { .n=4, .cap=4, .limbs=(limb_t*)M_4, .sign=1 } },// 23
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=5, .cap=5, .limbs=(limb_t*)M_5, .sign=1 }, 0, { .n=5, .cap=5, .limbs=(limb_t*)M_5, .sign=1 } },// 24
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)A_1, .sign=1 }, 0, { .n=1, .cap=1, .limbs=(limb_t*)A_1, .sign=1 } },// 25
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=2, .cap=2, .limbs=(limb_t*)A_2, .sign=1 }, 0, { .n=2, .cap=2, .limbs=(limb_t*)A_2, .sign=1 } },// 26
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=3, .cap=3, .limbs=(limb_t*)A_3, .sign=1 }, 0, { .n=3, .cap=3, .limbs=(limb_t*)A_3, .sign=1 } },// 27
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=4, .cap=4, .limbs=(limb_t*)A_4, .sign=1 }, 0, { .n=4, .cap=4, .limbs=(limb_t*)A_4, .sign=1 } },// 28
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=5, .cap=5, .limbs=(limb_t*)A_5, .sign=1 }, 0, { .n=5, .cap=5, .limbs=(limb_t*)A_5, .sign=1 } },// 29
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)F_1, .sign=1 }, 0, { .n=1, .cap=1, .limbs=(limb_t*)F_1, .sign=1 } },// 30
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=2, .cap=2, .limbs=(limb_t*)F_2, .sign=1 }, 0, { .n=2, .cap=2, .limbs=(limb_t*)F_2, .sign=1 } },// 31
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=3, .cap=3, .limbs=(limb_t*)F_3, .sign=1 }, 0, { .n=3, .cap=3, .limbs=(limb_t*)F_3, .sign=1 } },// 32
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=4, .cap=4, .limbs=(limb_t*)F_4, .sign=1 }, 0, { .n=4, .cap=4, .limbs=(limb_t*)F_4, .sign=1 } },// 33
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=5, .cap=5, .limbs=(limb_t*)F_5, .sign=1 }, 0, { .n=5, .cap=5, .limbs=(limb_t*)F_5, .sign=1 } },// 34
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=1 }, 1, { .n=2, .cap=2, .limbs=(limb_t*)C_2, .sign=1 } },// 35
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=1 }, 2, { .n=3, .cap=3, .limbs=(limb_t*)C_3, .sign=1 } },// 36
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=1 }, 3, { .n=4, .cap=4, .limbs=(limb_t*)C_4, .sign=1 } },// 37
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=1 }, 4, { .n=5, .cap=5, .limbs=(limb_t*)C_5, .sign=1 } },// 38
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=1 }, 5, { .n=6, .cap=6, .limbs=(limb_t*)C_6, .sign=1 } },// 39
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)Mm1_1, .sign=1 }, 0, { .n=1, .cap=1, .limbs=(limb_t*)Mm1_1, .sign=1 } },// 40
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=2, .cap=2, .limbs=(limb_t*)Mm1_2, .sign=1 }, 0, { .n=2, .cap=2, .limbs=(limb_t*)Mm1_2, .sign=1 } },// 41
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=3, .cap=3, .limbs=(limb_t*)Mm1_3, .sign=1 }, 0, { .n=3, .cap=3, .limbs=(limb_t*)Mm1_3, .sign=1 } },// 42
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=4, .cap=4, .limbs=(limb_t*)Mm1_4, .sign=1 }, 0, { .n=4, .cap=4, .limbs=(limb_t*)Mm1_4, .sign=1 } },// 43
    { { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 }, { .n=5, .cap=5, .limbs=(limb_t*)Mm1_5, .sign=1 }, 0, { .n=5, .cap=5, .limbs=(limb_t*)Mm1_5, .sign=1 } },// 44
    { { .n=1, .cap=1, .limbs=(limb_t*)A_1, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)F_1, .sign=1 }, 0, { .n=1, .cap=1, .limbs=(limb_t*)M_1, .sign=1 } },// 45
    { { .n=2, .cap=2, .limbs=(limb_t*)A_2, .sign=1 }, { .n=2, .cap=2, .limbs=(limb_t*)F_2, .sign=1 }, 0, { .n=2, .cap=2, .limbs=(limb_t*)M_2, .sign=1 } },// 46
    { { .n=3, .cap=3, .limbs=(limb_t*)A_3, .sign=1 }, { .n=3, .cap=3, .limbs=(limb_t*)F_3, .sign=1 }, 0, { .n=3, .cap=3, .limbs=(limb_t*)M_3, .sign=1 } },// 47
    { { .n=4, .cap=4, .limbs=(limb_t*)A_4, .sign=1 }, { .n=4, .cap=4, .limbs=(limb_t*)F_4, .sign=1 }, 0, { .n=4, .cap=4, .limbs=(limb_t*)M_4, .sign=1 } },// 48
    { { .n=5, .cap=5, .limbs=(limb_t*)A_5, .sign=1 }, { .n=5, .cap=5, .limbs=(limb_t*)F_5, .sign=1 }, 0, { .n=5, .cap=5, .limbs=(limb_t*)M_5, .sign=1 } },// 49
    { { .n=2, .cap=2, .limbs=(limb_t*)M_2, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=1 }, 0, { .n=3, .cap=3, .limbs=(limb_t*)C_3, .sign=1 } },// 50
    { { .n=3, .cap=3, .limbs=(limb_t*)M_3, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=1 }, 0, { .n=4, .cap=4, .limbs=(limb_t*)C_4, .sign=1 } },// 51
    { { .n=4, .cap=4, .limbs=(limb_t*)M_4, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=1 }, 0, { .n=5, .cap=5, .limbs=(limb_t*)C_5, .sign=1 } },// 52
    { { .n=5, .cap=5, .limbs=(limb_t*)M_5, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=1 }, 0, { .n=6, .cap=6, .limbs=(limb_t*)C_6, .sign=1 } },// 53
    { { .n=6, .cap=6, .limbs=(limb_t*)M_6, .sign=1 }, { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=1 }, 0, { .n=7, .cap=7, .limbs=(limb_t*)C_7, .sign=1 } } // 54
};

/* ----- __BIGINT_ADD_SAW__ cases ----- */
static const bi_add_case add_saw_cases[CASE_CNT] = {
    { { .n=1, .cap=1, .limbs=(limb_t*)M_1, .sign=1 },  { .n=1, .cap=1, .limbs=(limb_t*)M_1, .sign=-1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 } }, // 0
    { { .n=2, .cap=2, .limbs=(limb_t*)M_2, .sign=1 },  { .n=2, .cap=2, .limbs=(limb_t*)M_2, .sign=-1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 } }, // 1
    { { .n=3, .cap=3, .limbs=(limb_t*)M_3, .sign=1 },  { .n=3, .cap=3, .limbs=(limb_t*)M_3, .sign=-1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 } }, // 2
    { { .n=4, .cap=4, .limbs=(limb_t*)M_4, .sign=1 },  { .n=4, .cap=4, .limbs=(limb_t*)M_4, .sign=-1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 } }, // 3
    { { .n=5, .cap=5, .limbs=(limb_t*)M_5, .sign=1 },  { .n=5, .cap=5, .limbs=(limb_t*)M_5, .sign=-1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 } }, // 4
    { { .n=6, .cap=6, .limbs=(limb_t*)M_6, .sign=1 },  { .n=6, .cap=6, .limbs=(limb_t*)M_6, .sign=-1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 } }, // 5
    { { .n=1, .cap=1, .limbs=(limb_t*)M_1, .sign=-1 }, { .n=1, .cap=1, .limbs=(limb_t*)M_1, .sign=1 },  { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 } }, // 6
    { { .n=2, .cap=2, .limbs=(limb_t*)M_2, .sign=-1 }, { .n=2, .cap=2, .limbs=(limb_t*)M_2, .sign=1 },  { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 } }, // 7
    { { .n=3, .cap=3, .limbs=(limb_t*)M_3, .sign=-1 }, { .n=3, .cap=3, .limbs=(limb_t*)M_3, .sign=1 },  { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 } }, // 8
    { { .n=4, .cap=4, .limbs=(limb_t*)M_4, .sign=-1 }, { .n=4, .cap=4, .limbs=(limb_t*)M_4, .sign=1 },  { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 } }, // 9
    { { .n=5, .cap=5, .limbs=(limb_t*)M_5, .sign=-1 }, { .n=5, .cap=5, .limbs=(limb_t*)M_5, .sign=1 },  { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 } }, // 10
    { { .n=6, .cap=6, .limbs=(limb_t*)M_6, .sign=-1 }, { .n=6, .cap=6, .limbs=(limb_t*)M_6, .sign=1 },  { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 } }, // 11
    { { .n=1, .cap=1, .limbs=(limb_t*)A_1, .sign=1 },  { .n=1, .cap=1, .limbs=(limb_t*)A_1, .sign=-1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 } }, // 12
    { { .n=2, .cap=2, .limbs=(limb_t*)A_2, .sign=1 },  { .n=2, .cap=2, .limbs=(limb_t*)A_2, .sign=-1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 } }, // 13
    { { .n=3, .cap=3, .limbs=(limb_t*)A_3, .sign=1 },  { .n=3, .cap=3, .limbs=(limb_t*)A_3, .sign=-1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 } }, // 14
    { { .n=4, .cap=4, .limbs=(limb_t*)A_4, .sign=1 },  { .n=4, .cap=4, .limbs=(limb_t*)A_4, .sign=-1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 } }, // 15
    { { .n=5, .cap=5, .limbs=(limb_t*)A_5, .sign=1 },  { .n=5, .cap=5, .limbs=(limb_t*)A_5, .sign=-1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 } }, // 16
    { { .n=6, .cap=6, .limbs=(limb_t*)A_6, .sign=1 },  { .n=6, .cap=6, .limbs=(limb_t*)A_6, .sign=-1 }, { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 } }, // 17
    { { .n=1, .cap=1, .limbs=(limb_t*)F_1, .sign=-1 }, { .n=1, .cap=1, .limbs=(limb_t*)F_1, .sign=1 },  { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 } }, // 18
    { { .n=2, .cap=2, .limbs=(limb_t*)F_2, .sign=-1 }, { .n=2, .cap=2, .limbs=(limb_t*)F_2, .sign=1 },  { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 } }, // 19
    { { .n=3, .cap=3, .limbs=(limb_t*)F_3, .sign=-1 }, { .n=3, .cap=3, .limbs=(limb_t*)F_3, .sign=1 },  { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 } }, // 20
    { { .n=4, .cap=4, .limbs=(limb_t*)F_4, .sign=-1 }, { .n=4, .cap=4, .limbs=(limb_t*)F_4, .sign=1 },  { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 } }, // 21
    { { .n=5, .cap=5, .limbs=(limb_t*)F_5, .sign=-1 }, { .n=5, .cap=5, .limbs=(limb_t*)F_5, .sign=1 },  { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 } }, // 22
    { { .n=6, .cap=6, .limbs=(limb_t*)F_6, .sign=-1 }, { .n=6, .cap=6, .limbs=(limb_t*)F_6, .sign=1 },  { .n=0, .cap=1, .limbs=(limb_t*)Z_1, .sign=1 } }, // 23
    { { .n=1, .cap=1, .limbs=(limb_t*)M_1, .sign=1 },  { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=-1 }, { .n=1, .cap=1, .limbs=(limb_t*)Mm1_1, .sign=1 } },   // 24
    { { .n=2, .cap=2, .limbs=(limb_t*)M_2, .sign=1 },  { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=-1 }, { .n=2, .cap=2, .limbs=(limb_t*)Mm1_2, .sign=1 } },   // 25
    { { .n=3, .cap=3, .limbs=(limb_t*)M_3, .sign=1 },  { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=-1 }, { .n=3, .cap=3, .limbs=(limb_t*)Mm1_3, .sign=1 } },   // 26
    { { .n=4, .cap=4, .limbs=(limb_t*)M_4, .sign=1 },  { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=-1 }, { .n=4, .cap=4, .limbs=(limb_t*)Mm1_4, .sign=1 } },   // 27
    { { .n=5, .cap=5, .limbs=(limb_t*)M_5, .sign=1 },  { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=-1 }, { .n=5, .cap=5, .limbs=(limb_t*)Mm1_5, .sign=1 } },   // 28
    { { .n=6, .cap=6, .limbs=(limb_t*)M_6, .sign=1 },  { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=-1 }, { .n=6, .cap=6, .limbs=(limb_t*)Mm1_6, .sign=1 } },   // 29
    { { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=1 },  { .n=1, .cap=1, .limbs=(limb_t*)M_1, .sign=-1 }, { .n=1, .cap=1, .limbs=(limb_t*)Mm1_1, .sign=-1 } },  // 30
    { { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=1 },  { .n=2, .cap=2, .limbs=(limb_t*)M_2, .sign=-1 }, { .n=2, .cap=2, .limbs=(limb_t*)Mm1_2, .sign=-1 } },  // 31
    { { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=1 },  { .n=3, .cap=3, .limbs=(limb_t*)M_3, .sign=-1 }, { .n=3, .cap=3, .limbs=(limb_t*)Mm1_3, .sign=-1 } },  // 32
    { { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=1 },  { .n=4, .cap=4, .limbs=(limb_t*)M_4, .sign=-1 }, { .n=4, .cap=4, .limbs=(limb_t*)Mm1_4, .sign=-1 } },  // 33
    { { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=1 },  { .n=5, .cap=5, .limbs=(limb_t*)M_5, .sign=-1 }, { .n=5, .cap=5, .limbs=(limb_t*)Mm1_5, .sign=-1 } },  // 34
    { { .n=1, .cap=1, .limbs=(limb_t*)C_1, .sign=1 },  { .n=6, .cap=6, .limbs=(limb_t*)M_6, .sign=-1 }, { .n=6, .cap=6, .limbs=(limb_t*)Mm1_6, .sign=-1 } },  // 35
    { { .n=1, .cap=1, .limbs=(limb_t*)A_1, .sign=1 },  { .n=1, .cap=1, .limbs=(limb_t*)F_1, .sign=1 },  { .n=1, .cap=1, .limbs=(limb_t*)M_1, .sign=1 } }, // 36
    { { .n=2, .cap=2, .limbs=(limb_t*)A_2, .sign=1 },  { .n=2, .cap=2, .limbs=(limb_t*)F_2, .sign=1 },  { .n=2, .cap=2, .limbs=(limb_t*)M_2, .sign=1 } }, // 37
    { { .n=3, .cap=3, .limbs=(limb_t*)A_3, .sign=1 },  { .n=3, .cap=3, .limbs=(limb_t*)F_3, .sign=1 },  { .n=3, .cap=3, .limbs=(limb_t*)M_3, .sign=1 } }, // 38
    { { .n=4, .cap=4, .limbs=(limb_t*)A_4, .sign=1 },  { .n=4, .cap=4, .limbs=(limb_t*)F_4, .sign=1 },  { .n=4, .cap=4, .limbs=(limb_t*)M_4, .sign=1 } }, // 39
    { { .n=5, .cap=5, .limbs=(limb_t*)A_5, .sign=1 },  { .n=5, .cap=5, .limbs=(limb_t*)F_5, .sign=1 },  { .n=5, .cap=5, .limbs=(limb_t*)M_5, .sign=1 } }, // 40
    { { .n=6, .cap=6, .limbs=(limb_t*)A_6, .sign=1 },  { .n=6, .cap=6, .limbs=(limb_t*)F_6, .sign=1 },  { .n=6, .cap=6, .limbs=(limb_t*)M_6, .sign=1 } }, // 41
    { { .n=1, .cap=1, .limbs=(limb_t*)M_1, .sign=-1 }, { .n=1, .cap=1, .limbs=(limb_t*)F_1, .sign=1 },  { .n=1, .cap=1, .limbs=(limb_t*)A_1, .sign=-1 } },// 42
    { { .n=2, .cap=2, .limbs=(limb_t*)M_2, .sign=-1 }, { .n=2, .cap=2, .limbs=(limb_t*)F_2, .sign=1 },  { .n=2, .cap=2, .limbs=(limb_t*)A_2, .sign=-1 } },// 43
    { { .n=3, .cap=3, .limbs=(limb_t*)M_3, .sign=-1 }, { .n=3, .cap=3, .limbs=(limb_t*)F_3, .sign=1 },  { .n=3, .cap=3, .limbs=(limb_t*)A_3, .sign=-1 } },// 44
    { { .n=4, .cap=4, .limbs=(limb_t*)M_4, .sign=-1 }, { .n=4, .cap=4, .limbs=(limb_t*)F_4, .sign=1 },  { .n=4, .cap=4, .limbs=(limb_t*)A_4, .sign=-1 } },// 45
    { { .n=5, .cap=5, .limbs=(limb_t*)M_5, .sign=-1 }, { .n=5, .cap=5, .limbs=(limb_t*)F_5, .sign=1 },  { .n=5, .cap=5, .limbs=(limb_t*)A_5, .sign=-1 } },// 46
    { { .n=6, .cap=6, .limbs=(limb_t*)M_6, .sign=-1 }, { .n=6, .cap=6, .limbs=(limb_t*)F_6, .sign=1 },  { .n=6, .cap=6, .limbs=(limb_t*)A_6, .sign=-1 } },// 47
    { { .n=1, .cap=1, .limbs=(limb_t*)M_1, .sign=1 },  { .n=1, .cap=1, .limbs=(limb_t*)A_1, .sign=-1 }, { .n=1, .cap=1, .limbs=(limb_t*)F_1, .sign=1 } }, // 48
    { { .n=2, .cap=2, .limbs=(limb_t*)M_2, .sign=1 },  { .n=2, .cap=2, .limbs=(limb_t*)A_2, .sign=-1 }, { .n=2, .cap=2, .limbs=(limb_t*)F_2, .sign=1 } }, // 49
    { { .n=3, .cap=3, .limbs=(limb_t*)M_3, .sign=1 },  { .n=3, .cap=3, .limbs=(limb_t*)A_3, .sign=-1 }, { .n=3, .cap=3, .limbs=(limb_t*)F_3, .sign=1 } }, // 50
    { { .n=4, .cap=4, .limbs=(limb_t*)M_4, .sign=1 },  { .n=4, .cap=4, .limbs=(limb_t*)A_4, .sign=-1 }, { .n=4, .cap=4, .limbs=(limb_t*)F_4, .sign=1 } }, // 51
    { { .n=5, .cap=5, .limbs=(limb_t*)M_5, .sign=1 },  { .n=5, .cap=5, .limbs=(limb_t*)A_5, .sign=-1 }, { .n=5, .cap=5, .limbs=(limb_t*)F_5, .sign=1 } }, // 52
    { { .n=6, .cap=6, .limbs=(limb_t*)M_6, .sign=1 },  { .n=6, .cap=6, .limbs=(limb_t*)A_6, .sign=-1 }, { .n=6, .cap=6, .limbs=(limb_t*)F_6, .sign=1 } }  // 53
};




int main(void) { _libdnml_init();
    int total_tests = 0, passed_tests = 0; FILE* log_path;
    struct timespec start, end; clock_gettime(CLOCK_MONOTONIC, &start);
    limb_t *ret_buf = (limb_t *)malloc(BUF_SIZE * U64_BYTES); assert(ret_buf != NULL);
    bigInt ret = { .limbs = ret_buf, .n = 0, .cap = BUF_SIZE, .sign = 1 };
    log_path = fopen("../../algo_base/bi_add_logs.log", "w+");
    if (log_path == NULL) { free(ret_buf); fputs("Can't open log file", stdout); return 1; }
    fputs("====================================================================\n", stdout);
    fputs("               LIB-DNML ALGORITHM TESTS - BIGINT ADDITION           \n", stdout);
    fputs("====================================================================\n", stdout);
    fputs("----- __BIGINT_ADD_WC__ -----\n", log_path);
    for (int i = 0; i < CASE_CNT; ++i) { total_tests++;
        __BIGINT_ADD_WC__(&ret, &add_cases[i].a, &add_cases[i].b);
        int8_t match = __BIGINT_INTERNAL_COMP__(&ret, &add_cases[i].exp);
        if (!match) passed_tests++;
        else {
            fprintf(log_path,
                "[FAIL] ADD_WC Case %2d "
                "| Output struct: { .n = %zu, .sign = %" PRId8 " } "
                "| Exp Struct: { .n = %zu, .sign = %" PRId8 " }\n",
                i + 1, ret.n, ret.sign, add_cases[i].exp.n, add_cases[i].exp.sign
            );
            print_bi_limbs("ret", &ret, log_path);
            print_bi_limbs("exp", &add_cases[i].exp, log_path);
        }
    } fputs("Result written to test/calc_algo_test/algo_base/bi_add_logs.log\n", stdout);



    fputs("\n\n----- __BIGINT_ADD_SHIFT__ -----\n", log_path);
    for (int i = 0; i < CASE_CNT; ++i) { total_tests++;
        if (add_shift_cases[i].dst.limbs && add_shift_cases[i].dst.n) {
            memcpy(ret_buf, add_shift_cases[i].dst.limbs, add_shift_cases[i].dst.n * U64_BYTES);
        }
        ret.n = add_shift_cases[i].dst.n;
        __BIGINT_ADD_SHIFT__(&ret, &add_shift_cases[i].src, add_shift_cases[i].limb_shift);
        int8_t match = __BIGINT_INTERNAL_COMP__(&ret, &add_shift_cases[i].exp);
        if (!match) passed_tests++;
        else {
            fprintf(log_path,
                "[FAIL] ADD_SHIFT Case %2d | Limb shift: %zu "
                "| Output struct: { .n = %zu, .sign = %" PRId8 " } "
                "| Exp Struct: { .n = %zu, .sign = %" PRId8 " }\n",
                i + 1, add_shift_cases[i].limb_shift, ret.n, ret.sign, add_shift_cases[i].exp.n, add_shift_cases[i].exp.sign
            );
            print_bi_limbs("ret", &ret, log_path);
            print_bi_limbs("exp", &add_shift_cases[i].exp, log_path);
        }
    } fputs("Result written to test/calc_algo_test/algo_base/bi_add_logs.log\n", stdout);



    fputs("\n\n----- __BIGINT_ADD_SAW__ -----\n", log_path);
    for (int i = 0; i < CASE_CNT; ++i) { total_tests++;
        __BIGINT_ADD_SAW__(&ret, &add_saw_cases[i].a, &add_saw_cases[i].b);
        int8_t match = __BIGINT_INTERNAL_COMP__(&ret, &add_saw_cases[i].exp);
        if (!match) passed_tests++;
        else {
            fprintf(log_path,
                "[FAIL] ADD_SAW Case %2d "
                "| Output struct: { .n = %zu, .sign = %" PRId8 " } "
                "| Exp Struct: { .n = %zu, .sign = %" PRId8 " }\n",
                i + 1, ret.n, ret.sign, add_saw_cases[i].exp.n, add_saw_cases[i].exp.sign
            );
            print_bi_limbs("ret", &ret, log_path);
            print_bi_limbs("exp", &add_saw_cases[i].exp, log_path);
        }
    } fputs("Result written to test/calc_algo_test/algo_base/bi_add_logs.log\n", stdout);



    /* Summary output block */
    #undef CASE_CNT
    #undef MAX_SIZE
    #undef BUF_SIZE
    clock_gettime(CLOCK_MONOTONIC, &end); free(ret_buf); fclose(log_path);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    fputs( "=========================================================\n", stdout);
    fputs( "TEST SUMMARY:\n", stdout);
    printf("+) Passed %-4d out of %-4d total compiled checks.\n", passed_tests, total_tests);
    printf("+) Success rate: %.2f%%\n", (total_tests > 0) ? ((passed_tests * 100.0) / total_tests) : 0.0);
    printf("+) Total Runtime: %lf ms\n", elapsed_time * 1000.0);
    fputs( "=========================================================\n", stdout);
    _libdnml_cleanup(); return 0;
}
