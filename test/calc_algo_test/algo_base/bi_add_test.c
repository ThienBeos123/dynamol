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
// Deterministic Input Limb Vectors (Size <= MAX_SIZE [12])
static const limb_t zero_l[1] = { UINT64_C(0x0000000000000000) };
static const limb_t one_l[1]  = { UINT64_C(0x0000000000000001) };
static const limb_t two_l[1]  = { UINT64_C(0x0000000000000002) };
static const limb_t max_l[1]  = { UINT64_C(0xFFFFFFFFFFFFFFFF) };
static const limb_t pat_a[4]  = { UINT64_C(0x0123456789ABCDEF), UINT64_C(0xFEDCBA9876543210), UINT64_C(0x0000000011111111), UINT64_C(0x2222222233333333) };
static const limb_t pat_b[4]  = { UINT64_C(0x9ABCDEF012345678), UINT64_C(0x76543210FEDCBA98), UINT64_C(0xEEEEEEEEFFFFFFFF), UINT64_C(0xDDDDDDDDCCCCCCCC) };
static const limb_t a_len12[12] = {
    UINT64_C(0x1111111111111111), UINT64_C(0x2222222222222222), UINT64_C(0x3333333333333333), UINT64_C(0x4444444444444444),
    UINT64_C(0x5555555555555555), UINT64_C(0x6666666666666666), UINT64_C(0x7777777777777777), UINT64_C(0x8888888888888888),
    UINT64_C(0x9999999999999999), UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0xBBBBBBBBBBBBBBBB), UINT64_C(0xCCCCCCCCCCCCCCCC)
};
static const limb_t b_len12[12] = {
    UINT64_C(0xEEEEEEEEEEEEEEEE), UINT64_C(0xDDDDDDDDDDDDDDDD), UINT64_C(0xCCCCCCCCCCCCCCCC), UINT64_C(0xBBBBBBBBBBBBBBBB),
    UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0x9999999999999999), UINT64_C(0x8888888888888888), UINT64_C(0x7777777777777777),
    UINT64_C(0x6666666666666666), UINT64_C(0x5555555555555555), UINT64_C(0x4444444444444444), UINT64_C(0x3333333333333333)
};
// Expected vectors
static const limb_t exp_zero[1] = { UINT64_C(0x0000000000000000) };
static const limb_t exp_one[1]  = { UINT64_C(0x0000000000000001) };
static const limb_t exp_two[1]  = { UINT64_C(0x0000000000000002) };
static const limb_t exp_three[1]= { UINT64_C(0x0000000000000003) };
static const limb_t exp_four[1] = { UINT64_C(0x0000000000000004) };
static const limb_t exp_max_m1[1] = { UINT64_C(0xFFFFFFFFFFFFFFFE) };
static const limb_t exp_wc_c1[2]  = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000001) };
static const limb_t exp_wc_c2[3]  = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000001) };
static const limb_t exp_wc_pat[4] = { UINT64_C(0x9BE024579BE02467), UINT64_C(0x7530ECA97530ECA8), UINT64_C(0xEEEEEEEF11111111), UINT64_C(0xFFFFFFFFFFFFFFFF) };
static const limb_t exp_wc_12[13] = {
    UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF),
    UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF),
    UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF),
    UINT64_C(0x0000000000000000)
};
static const limb_t exp_sh_c1[4] = { UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000001) };
static const limb_t exp_sh_pat[6] = {
    UINT64_C(0x0123456789ABCDEF), UINT64_C(0xFEDCBA9876543210), UINT64_C(0x9ABCDEF023456789), UINT64_C(0x98765433320FEDCB),
    UINT64_C(0xEEEEEEEEFFFFFFFF), UINT64_C(0xDDDDDDDDCCCCCCCC),
};
//* =================== GLOBAL ARRAY OF CASES =================== *//
/* ---- __BIGINT_ADD_WC cases ---- */
static const bi_add_case add_cases[CASE_CNT] = {
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } },  // 1
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_one,  .sign = 1 } },  // 2
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_one,  .sign = 1 } },  // 3
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 4
    { { .n = 1, .cap = 1, .limbs = (limb_t*)two_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_three,.sign = 1 } },  // 5
    { { .n = 1, .cap = 1, .limbs = (limb_t*)max_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 2, .cap = 2, .limbs = (limb_t*)exp_wc_c1, .sign = 1 } }, // 6
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)max_l,  .sign = 1 }, { .n = 2, .cap = 2, .limbs = (limb_t*)exp_wc_c1, .sign = 1 } }, // 7
    { { .n = 4, .cap = 4, .limbs = (limb_t*)pat_a,  .sign = 1 }, { .n = 4, .cap = 4, .limbs = (limb_t*)pat_b,  .sign = 1 }, { .n = 4, .cap = 4, .limbs = (limb_t*)exp_wc_pat,.sign = 1 } }, // 8
    { { .n = 4, .cap = 4, .limbs = (limb_t*)pat_b,  .sign = 1 }, { .n = 4, .cap = 4, .limbs = (limb_t*)pat_a,  .sign = 1 }, { .n = 4, .cap = 4, .limbs = (limb_t*)exp_wc_pat,.sign = 1 } }, // 9
    { { .n = 12,.cap = 12,.limbs = (limb_t*)a_len12,.sign = 1 }, { .n = 12,.cap = 12,.limbs = (limb_t*)b_len12,.sign = 1 }, { .n = 12,.cap = 13,.limbs = (limb_t*)exp_wc_12, .sign = 1 } }, // 10
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)two_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_three,.sign = 1 } },  // 11
    { { .n = 1, .cap = 1, .limbs = (limb_t*)two_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)two_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_four, .sign = 1 } },  // 12
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)two_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 13
    { { .n = 1, .cap = 1, .limbs = (limb_t*)two_l,  .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 14
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 15
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 16
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 17
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 18
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 19
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 20
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 21
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 22
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 23
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 24
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 25
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 26
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 27
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 28
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 29
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 30
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 31
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 32
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 33
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 34
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 35
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 36
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 37
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 38
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 39
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 40
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 41
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 42
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 43
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 44
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 45
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 46
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 47
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 48
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 49
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 50
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 51
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 52
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 53
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 54
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 55
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 56
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 57
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 58
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } },  // 59
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_two,  .sign = 1 } }   // 60
};


/* ----- __BIGINT_ADD_SHIFT__ cases ----- */
static const bi_adds_case add_shift_cases[CASE_CNT] = {
    // dst, src, limb_shift, exp (Imitating: dst = dst + (src << limb_shift))
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, 0, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_one,   .sign = 1 } },  // 1
    { { .n = 1, .cap = 1, .limbs = (limb_t*)max_l,  .sign = 1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 }, 3, { .n = 4, .cap = 4, .limbs = (limb_t*)exp_sh_c1, .sign = 1 } },  // 2
    { { .n = 4, .cap = 4, .limbs = (limb_t*)pat_a,  .sign = 1 }, { .n = 4, .cap = 4, .limbs = (limb_t*)pat_b,  .sign = 1 }, 2, { .n = 6, .cap = 6, .limbs = (limb_t*)exp_sh_pat,.sign = 1 } },  // 3
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 4
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 5
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 6
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 7
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 8
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 9
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 10
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 11
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 12
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 13
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 14
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 15
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 16
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 17
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 18
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 19
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 20
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 21
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 22
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 23
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 24
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 25
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 26
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 27
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 28
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 29
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 30
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 31
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 32
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 33
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 34
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 35
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 36
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 37
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 38
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 39
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 40
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 41
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 42
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 43
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 44
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 45
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 46
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 47
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 48
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 49
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 50
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 51
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 52
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 53
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 54
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 55
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 56
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 57
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 58
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } },  // 59
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 }, 0, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero,  .sign = 1 } }   // 60
};


/* ----- __BIGINT_ADD_SAW__ cases ----- */
static const bi_add_case add_saw_cases[CASE_CNT] = {
    // a, b, exp (Handles algebraic mutations cleanly)
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 },  { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = -1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 1
    { { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = -1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 2
    { { .n = 4, .cap = 4, .limbs = (limb_t*)pat_a,  .sign = 1 },  { .n = 4, .cap = 4, .limbs = (limb_t*)pat_a,  .sign = -1 }, { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 3
    { { .n = 1, .cap = 1, .limbs = (limb_t*)max_l,  .sign = 1 },  { .n = 1, .cap = 1, .limbs = (limb_t*)one_l,  .sign = -1 }, { .n = 1, .cap = 1, .limbs = (limb_t*)exp_max_m1, .sign = 1 } }, // 4
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 5
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 6
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 7
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 8
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 9
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 10
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 11
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 12
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 13
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 14
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 15
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 16
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 17
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 18
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 19
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 20
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 21
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 22
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 23
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 24
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 25
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 26
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 27
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 28
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 29
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 30
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 31
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 32
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 33
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 34
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 35
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 36
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 37
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 38
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 39
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 40
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 41
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 42
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 43
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 44
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 45
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 46
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 47
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 48
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 49
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 50
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 51
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 52
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 53
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 54
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 55
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 56
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 57
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 58
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } }, // 59
    { { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)zero_l, .sign = 1 },  { .n = 0, .cap = 1, .limbs = (limb_t*)exp_zero, .sign = 1 } } // 60
};




int main(void) { _libdnml_init();
    int total_tests = 0, passed_tests = 0; FILE* log_path;
    struct timespec start, end; clock_gettime(CLOCK_MONOTONIC, &start);
    limb_t *ret_buf = (limb_t *)malloc(BUF_SIZE * U64_BYTES); assert(ret_buf != NULL);
    bigInt ret = { .limbs = ret_buf, .n = 0, .cap = BUF_SIZE, .sign = 1 };
    log_path = fopen("../../algo_base/bi_add_logs.txt", "w+");
    if (log_path == NULL) { free(ret_buf); fputs("Can't open log file", stdout); return 1; }
    fputs("====================================================================\n", stdout);
    fputs("               LIB-DNML ALGORITHM TESTS - BIGINT ADDITION           \n", stdout);
    fputs("====================================================================\n", stdout);
    fputs("----- __BIGINT_ADD_WC__ -----\n", log_path);
    for (int i = 0; i < CASE_CNT; ++i) { total_tests++;
        memset(ret_buf, 0, BUF_SIZE * U64_BYTES); ret.n = 0; ret.sign = 1;
        __BIGINT_ADD_WC__(&ret, &add_cases[i].a, &add_cases[i].b);
        int8_t match = __BIGINT_INTERNAL_COMP__(&ret, &add_cases[i].exp); // Proven to be correct
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
    } fputs("Result written to test/calc_algo_test/algo_base/bi_add_logs.txt\n", stdout);



    fputs("\n\n----- __BIGINT_ADD_SHIFT__ -----\n", log_path);
    for (int i = 0; i < CASE_CNT; ++i) { total_tests++;
        memset(ret_buf, 0, BUF_SIZE * U64_BYTES); ret.n = 0; ret.sign = 1;
        if (add_shift_cases[i].dst.limbs && add_shift_cases[i].dst.n) {
            memcpy(ret_buf, add_shift_cases[i].dst.limbs, add_shift_cases[i].dst.n * U64_BYTES);
        } ret.n = add_shift_cases[i].dst.n;
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
    } fputs("Result written to test/calc_algo_test/algo_base/bi_add_logs.txt\n", stdout);



    fputs("\n\n----- __BIGINT_ADD_SAW__ -----\n", log_path);
    for (int i = 0; i < CASE_CNT; ++i) { total_tests++;
        memset(ret_buf, 0, BUF_SIZE * U64_BYTES); ret.n = 0; ret.sign = 1;
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
    } fputs("Result written to test/calc_algo_test/algo_base/bi_add_logs.txt\n", stdout);



    /* Summary output block */
    #undef CASE_CNT
    #undef MAX_SIZE
    clock_gettime(CLOCK_MONOTONIC, &end); free(ret_buf);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    fputs( "=========================================================\n", stdout);
    fputs( "TEST SUMMARY:\n", stdout);
    printf("+) Passed %-4d out of %-4d total compiled checks.\n", passed_tests, total_tests);
    printf("+) Success rate: %.2f%%\n", (total_tests > 0) ? ((passed_tests * 100.0) / total_tests) : 0.0);
    printf("+) Total Runtime: %lf ms\n", elapsed_time * 1000.0);
    fputs( "=========================================================\n", stdout);
    _libdnml_cleanup(); return 0;
}
