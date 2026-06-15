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
typedef struct { const bigInt in; size_t klimbs; const bigInt exp; } limb_shift_case;
/* ========== STATIC LIMB DATA REGISTRY ========== */
static const limb_t limbs_c1[1]   = {1};
static const limb_t limbs_c2[2]   = {1, 2};
static const limb_t limbs_c3[3]   = {1, 2, 3};
static const limb_t limbs_c4[4]   = {1, 2, 3, 4};
static const limb_t limbs_c5[5]   = {1, 2, 3, 4, 5};
static const limb_t limbs_c6[6]   = {1, 2, 3, 4, 5, 6};
static const limb_t limbs_c7[7]   = {1, 2, 3, 4, 5, 6, 7};
static const limb_t limbs_c8[8]   = {1, 2, 3, 4, 5, 6, 7, 8};
static const limb_t limbs_c9[9]   = {1, 2, 3, 4, 5, 6, 7, 8, 9};
static const limb_t limbs_c10[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
static const limb_t limbs_c11[11] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
static const limb_t limbs_c12[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
static const limb_t limbs_c13[13] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
static const limb_t limbs_c14[14] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
static const limb_t limbs_c15[15] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
static const limb_t limbs_c16[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
static const limb_t limbs_c17[17] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};
static const limb_t limbs_c18[18] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};
static const limb_t limbs_c19[19] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
static const limb_t limbs_c20[20] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
static const limb_t limbs_c21[21] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
static const limb_t limbs_c22[22] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22};
static const limb_t limbs_c23[23] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};
static const limb_t limbs_c24[24] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};
static const limb_t limbs_c25[25] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25};
static const limb_t limbs_c26[26] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26};
static const limb_t limbs_c27[27] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27};
static const limb_t limbs_c28[28] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28};
static const limb_t limbs_c29[29] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29};
static const limb_t limbs_c30[30] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30};
static const limb_t limbs_c31[31] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};

static const limb_t limbs_rl_out2[1]  = {2};
static const limb_t limbs_rl_out3[2]  = {2, 3};
static const limb_t limbs_rl_out4[3]  = {2, 3, 4};
static const limb_t limbs_rl_out5[4]  = {2, 3, 4, 5};
static const limb_t limbs_rl_out6[5]  = {2, 3, 4, 5, 6};
static const limb_t limbs_rl_out7[6]  = {2, 3, 4, 5, 6, 7};
static const limb_t limbs_rl_out8[7]  = {2, 3, 4, 5, 6, 7, 8};
static const limb_t limbs_rl_out9[8]  = {2, 3, 4, 5, 6, 7, 8, 9};
static const limb_t limbs_rl_out10[9] = {2, 3, 4, 5, 6, 7, 8, 9, 10};
static const limb_t limbs_rl_out11[10] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
static const limb_t limbs_rl_out12[11] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
static const limb_t limbs_rl_out13[12] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
static const limb_t limbs_rl_out14[13] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
static const limb_t limbs_rl_out15[14] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
static const limb_t limbs_rl_out16[15] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
static const limb_t limbs_rl_out17[16] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};
static const limb_t limbs_rl_out18[17] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};
static const limb_t limbs_rl_out19[18] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
static const limb_t limbs_rl_out20[19] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
static const limb_t limbs_rl_out21[20] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
static const limb_t limbs_rl_out22[21] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22};
static const limb_t limbs_rl_out23[22] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};
static const limb_t limbs_rl_out24[23] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};
static const limb_t limbs_rl_out25[24] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25};
static const limb_t limbs_rl_out26[25] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26};
static const limb_t limbs_rl_out27[26] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27};
static const limb_t limbs_rl_out28[27] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28};
static const limb_t limbs_rl_out29[28] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29};
static const limb_t limbs_rl_out30[29] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30};
static const limb_t limbs_rl_out31[30] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};

static const limb_t limbs_ll_out1[2]  = {1, 1};
static const limb_t limbs_ll_out2[3]  = {1, 1, 2};
static const limb_t limbs_ll_out3[4]  = {1, 1, 2, 3};
static const limb_t limbs_ll_out4[5]  = {1, 1, 2, 3, 4};
static const limb_t limbs_ll_out5[6]  = {1, 1, 2, 3, 4, 5};
static const limb_t limbs_ll_out6[7]  = {1, 1, 2, 3, 4, 5, 6};
static const limb_t limbs_ll_out7[8]  = {1, 1, 2, 3, 4, 5, 6, 7};
static const limb_t limbs_ll_out8[9]  = {1, 1, 2, 3, 4, 5, 6, 7, 8};
static const limb_t limbs_ll_out9[10] = {1, 1, 2, 3, 4, 5, 6, 7, 8, 9};
static const limb_t limbs_ll_out10[11] = {1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
static const limb_t limbs_ll_out11[12] = {1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
static const limb_t limbs_ll_out12[13] = {1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
static const limb_t limbs_ll_out13[14] = {1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
static const limb_t limbs_ll_out14[15] = {1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
static const limb_t limbs_ll_out15[16] = {1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
static const limb_t limbs_ll_out16[17] = {1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
static const limb_t limbs_ll_out17[18] = {1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};
static const limb_t limbs_ll_out18[19] = {1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};
static const limb_t limbs_ll_out19[20] = {1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
static const limb_t limbs_ll_out20[21] = {1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
static const limb_t limbs_ll_out21[22] = {1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
static const limb_t limbs_ll_out22[23] = {1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22};
static const limb_t limbs_ll_out23[24] = {1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};
static const limb_t limbs_ll_out24[25] = {1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};
static const limb_t limbs_ll_out25[26] = {1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25};
static const limb_t limbs_ll_out26[27] = {1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26};
static const limb_t limbs_ll_out27[28] = {1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27};
static const limb_t limbs_ll_out28[29] = {1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28};
static const limb_t limbs_ll_out29[30] = {1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29};
static const limb_t limbs_ll_out30[31] = {1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30};

//* ============== GLOBAL ARENA OF CASES ============== *//
static const limb_shift_case rlshift_cases[60] = {
    { { 1, 1, (limb_t *)limbs_c1 }, 0, { 1, 1, (limb_t *)limbs_c1 } },
    { { 2, 2, (limb_t *)limbs_c2 }, 0, { 2, 2, (limb_t *)limbs_c2 } },
    { { 3, 3, (limb_t *)limbs_c3 }, 0, { 3, 3, (limb_t *)limbs_c3 } },
    { { 4, 4, (limb_t *)limbs_c4 }, 0, { 4, 4, (limb_t *)limbs_c4 } },
    { { 5, 5, (limb_t *)limbs_c5 }, 0, { 5, 5, (limb_t *)limbs_c5 } },
    { { 6, 6, (limb_t *)limbs_c6 }, 0, { 6, 6, (limb_t *)limbs_c6 } },
    { { 7, 7, (limb_t *)limbs_c7 }, 0, { 7, 7, (limb_t *)limbs_c7 } },
    { { 8, 8, (limb_t *)limbs_c8 }, 0, { 8, 8, (limb_t *)limbs_c8 } },
    { { 9, 9, (limb_t *)limbs_c9 }, 0, { 9, 9, (limb_t *)limbs_c9 } },
    { { 10, 10, (limb_t *)limbs_c10 }, 0, { 10, 10, (limb_t *)limbs_c10 } },
    { { 11, 11, (limb_t *)limbs_c11 }, 0, { 11, 11, (limb_t *)limbs_c11 } },
    { { 12, 12, (limb_t *)limbs_c12 }, 0, { 12, 12, (limb_t *)limbs_c12 } },
    { { 13, 13, (limb_t *)limbs_c13 }, 0, { 13, 13, (limb_t *)limbs_c13 } },
    { { 14, 14, (limb_t *)limbs_c14 }, 0, { 14, 14, (limb_t *)limbs_c14 } },
    { { 15, 15, (limb_t *)limbs_c15 }, 0, { 15, 15, (limb_t *)limbs_c15 } },
    { { 16, 16, (limb_t *)limbs_c16 }, 0, { 16, 16, (limb_t *)limbs_c16 } },
    { { 17, 17, (limb_t *)limbs_c17 }, 0, { 17, 17, (limb_t *)limbs_c17 } },
    { { 18, 18, (limb_t *)limbs_c18 }, 0, { 18, 18, (limb_t *)limbs_c18 } },
    { { 19, 19, (limb_t *)limbs_c19 }, 0, { 19, 19, (limb_t *)limbs_c19 } },
    { { 20, 20, (limb_t *)limbs_c20 }, 0, { 20, 20, (limb_t *)limbs_c20 } },
    { { 21, 21, (limb_t *)limbs_c21 }, 0, { 21, 21, (limb_t *)limbs_c21 } },
    { { 22, 22, (limb_t *)limbs_c22 }, 0, { 22, 22, (limb_t *)limbs_c22 } },
    { { 23, 23, (limb_t *)limbs_c23 }, 0, { 23, 23, (limb_t *)limbs_c23 } },
    { { 24, 24, (limb_t *)limbs_c24 }, 0, { 24, 24, (limb_t *)limbs_c24 } },
    { { 25, 25, (limb_t *)limbs_c25 }, 0, { 25, 25, (limb_t *)limbs_c25 } },
    { { 26, 26, (limb_t *)limbs_c26 }, 0, { 26, 26, (limb_t *)limbs_c26 } },
    { { 27, 27, (limb_t *)limbs_c27 }, 0, { 27, 27, (limb_t *)limbs_c27 } },
    { { 28, 28, (limb_t *)limbs_c28 }, 0, { 28, 28, (limb_t *)limbs_c28 } },
    { { 29, 29, (limb_t *)limbs_c29 }, 0, { 29, 29, (limb_t *)limbs_c29 } },
    { { 30, 30, (limb_t *)limbs_c30 }, 0, { 30, 30, (limb_t *)limbs_c30 } },
    { { 2, 2, (limb_t *)limbs_c2 }, 1, { 1, 1, (limb_t *)limbs_rl_out2 } },
    { { 3, 3, (limb_t *)limbs_c3 }, 1, { 2, 2, (limb_t *)limbs_rl_out3 } },
    { { 4, 4, (limb_t *)limbs_c4 }, 1, { 3, 3, (limb_t *)limbs_rl_out4 } },
    { { 5, 5, (limb_t *)limbs_c5 }, 1, { 4, 4, (limb_t *)limbs_rl_out5 } },
    { { 6, 6, (limb_t *)limbs_c6 }, 1, { 5, 5, (limb_t *)limbs_rl_out6 } },
    { { 7, 7, (limb_t *)limbs_c7 }, 1, { 6, 6, (limb_t *)limbs_rl_out7 } },
    { { 8, 8, (limb_t *)limbs_c8 }, 1, { 7, 7, (limb_t *)limbs_rl_out8 } },
    { { 9, 9, (limb_t *)limbs_c9 }, 1, { 8, 8, (limb_t *)limbs_rl_out9 } },
    { { 10, 10, (limb_t *)limbs_c10 }, 1, { 9, 9, (limb_t *)limbs_rl_out10 } },
    { { 11, 11, (limb_t *)limbs_c11 }, 1, { 10, 10, (limb_t *)limbs_rl_out11 } },
    { { 12, 12, (limb_t *)limbs_c12 }, 1, { 11, 11, (limb_t *)limbs_rl_out12 } },
    { { 13, 13, (limb_t *)limbs_c13 }, 1, { 12, 12, (limb_t *)limbs_rl_out13 } },
    { { 14, 14, (limb_t *)limbs_c14 }, 1, { 13, 13, (limb_t *)limbs_rl_out14 } },
    { { 15, 15, (limb_t *)limbs_c15 }, 1, { 14, 14, (limb_t *)limbs_rl_out15 } },
    { { 16, 16, (limb_t *)limbs_c16 }, 1, { 15, 15, (limb_t *)limbs_rl_out16 } },
    { { 17, 17, (limb_t *)limbs_c17 }, 1, { 16, 16, (limb_t *)limbs_rl_out17 } },
    { { 18, 18, (limb_t *)limbs_c18 }, 1, { 17, 17, (limb_t *)limbs_rl_out18 } },
    { { 19, 19, (limb_t *)limbs_c19 }, 1, { 18, 18, (limb_t *)limbs_rl_out19 } },
    { { 20, 20, (limb_t *)limbs_c20 }, 1, { 19, 19, (limb_t *)limbs_rl_out20 } },
    { { 21, 21, (limb_t *)limbs_c21 }, 1, { 20, 20, (limb_t *)limbs_rl_out21 } },
    { { 22, 22, (limb_t *)limbs_c22 }, 1, { 21, 21, (limb_t *)limbs_rl_out22 } },
    { { 23, 23, (limb_t *)limbs_c23 }, 1, { 22, 22, (limb_t *)limbs_rl_out23 } },
    { { 24, 24, (limb_t *)limbs_c24 }, 1, { 23, 23, (limb_t *)limbs_rl_out24 } },
    { { 25, 25, (limb_t *)limbs_c25 }, 1, { 24, 24, (limb_t *)limbs_rl_out25 } },
    { { 26, 26, (limb_t *)limbs_c26 }, 1, { 25, 25, (limb_t *)limbs_rl_out26 } },
    { { 27, 27, (limb_t *)limbs_c27 }, 1, { 26, 26, (limb_t *)limbs_rl_out27 } },
    { { 28, 28, (limb_t *)limbs_c28 }, 1, { 27, 27, (limb_t *)limbs_rl_out28 } },
    { { 29, 29, (limb_t *)limbs_c29 }, 1, { 28, 28, (limb_t *)limbs_rl_out29 } },
    { { 30, 30, (limb_t *)limbs_c30 }, 1, { 29, 29, (limb_t *)limbs_rl_out30 } },
    { { 31, 31, (limb_t *)limbs_c31 }, 1, { 30, 30, (limb_t *)limbs_rl_out31 } }
};
static const limb_shift_case llshift_cases[60] = {
    { { 1, 1, (limb_t *)limbs_c1 }, 0, { 1, 1, (limb_t *)limbs_c1 } },
    { { 2, 2, (limb_t *)limbs_c2 }, 0, { 2, 2, (limb_t *)limbs_c2 } },
    { { 3, 3, (limb_t *)limbs_c3 }, 0, { 3, 3, (limb_t *)limbs_c3 } },
    { { 4, 4, (limb_t *)limbs_c4 }, 0, { 4, 4, (limb_t *)limbs_c4 } },
    { { 5, 5, (limb_t *)limbs_c5 }, 0, { 5, 5, (limb_t *)limbs_c5 } },
    { { 6, 6, (limb_t *)limbs_c6 }, 0, { 6, 6, (limb_t *)limbs_c6 } },
    { { 7, 7, (limb_t *)limbs_c7 }, 0, { 7, 7, (limb_t *)limbs_c7 } },
    { { 8, 8, (limb_t *)limbs_c8 }, 0, { 8, 8, (limb_t *)limbs_c8 } },
    { { 9, 9, (limb_t *)limbs_c9 }, 0, { 9, 9, (limb_t *)limbs_c9 } },
    { { 10, 10, (limb_t *)limbs_c10 }, 0, { 10, 10, (limb_t *)limbs_c10 } },
    { { 11, 11, (limb_t *)limbs_c11 }, 0, { 11, 11, (limb_t *)limbs_c11 } },
    { { 12, 12, (limb_t *)limbs_c12 }, 0, { 12, 12, (limb_t *)limbs_c12 } },
    { { 13, 13, (limb_t *)limbs_c13 }, 0, { 13, 13, (limb_t *)limbs_c13 } },
    { { 14, 14, (limb_t *)limbs_c14 }, 0, { 14, 14, (limb_t *)limbs_c14 } },
    { { 15, 15, (limb_t *)limbs_c15 }, 0, { 15, 15, (limb_t *)limbs_c15 } },
    { { 16, 16, (limb_t *)limbs_c16 }, 0, { 16, 16, (limb_t *)limbs_c16 } },
    { { 17, 17, (limb_t *)limbs_c17 }, 0, { 17, 17, (limb_t *)limbs_c17 } },
    { { 18, 18, (limb_t *)limbs_c18 }, 0, { 18, 18, (limb_t *)limbs_c18 } },
    { { 19, 19, (limb_t *)limbs_c19 }, 0, { 19, 19, (limb_t *)limbs_c19 } },
    { { 20, 20, (limb_t *)limbs_c20 }, 0, { 20, 20, (limb_t *)limbs_c20 } },
    { { 21, 21, (limb_t *)limbs_c21 }, 0, { 21, 21, (limb_t *)limbs_c21 } },
    { { 22, 22, (limb_t *)limbs_c22 }, 0, { 22, 22, (limb_t *)limbs_c22 } },
    { { 23, 23, (limb_t *)limbs_c23 }, 0, { 23, 23, (limb_t *)limbs_c23 } },
    { { 24, 24, (limb_t *)limbs_c24 }, 0, { 24, 24, (limb_t *)limbs_c24 } },
    { { 25, 25, (limb_t *)limbs_c25 }, 0, { 25, 25, (limb_t *)limbs_c25 } },
    { { 26, 26, (limb_t *)limbs_c26 }, 0, { 26, 26, (limb_t *)limbs_c26 } },
    { { 27, 27, (limb_t *)limbs_c27 }, 0, { 27, 27, (limb_t *)limbs_c27 } },
    { { 28, 28, (limb_t *)limbs_c28 }, 0, { 28, 28, (limb_t *)limbs_c28 } },
    { { 29, 29, (limb_t *)limbs_c29 }, 0, { 29, 29, (limb_t *)limbs_c29 } },
    { { 30, 30, (limb_t *)limbs_c30 }, 0, { 30, 30, (limb_t *)limbs_c30 } },
    { { 1, 1, (limb_t *)limbs_c1 }, 1, { 2, 2, (limb_t *)limbs_ll_out1 } },
    { { 2, 2, (limb_t *)limbs_c2 }, 1, { 3, 3, (limb_t *)limbs_ll_out2 } },
    { { 3, 3, (limb_t *)limbs_c3 }, 1, { 4, 4, (limb_t *)limbs_ll_out3 } },
    { { 4, 4, (limb_t *)limbs_c4 }, 1, { 5, 5, (limb_t *)limbs_ll_out4 } },
    { { 5, 5, (limb_t *)limbs_c5 }, 1, { 6, 6, (limb_t *)limbs_ll_out5 } },
    { { 6, 6, (limb_t *)limbs_c6 }, 1, { 7, 7, (limb_t *)limbs_ll_out6 } },
    { { 7, 7, (limb_t *)limbs_c7 }, 1, { 8, 8, (limb_t *)limbs_ll_out7 } },
    { { 8, 8, (limb_t *)limbs_c8 }, 1, { 9, 9, (limb_t *)limbs_ll_out8 } },
    { { 9, 9, (limb_t *)limbs_c9 }, 1, { 10, 10, (limb_t *)limbs_ll_out9 } },
    { { 10, 10, (limb_t *)limbs_c10 }, 1, { 11, 11, (limb_t *)limbs_ll_out10 } },
    { { 11, 11, (limb_t *)limbs_c11 }, 1, { 12, 12, (limb_t *)limbs_ll_out11 } },
    { { 12, 12, (limb_t *)limbs_c12 }, 1, { 13, 13, (limb_t *)limbs_ll_out12 } },
    { { 13, 13, (limb_t *)limbs_c13 }, 1, { 14, 14, (limb_t *)limbs_ll_out13 } },
    { { 14, 14, (limb_t *)limbs_c14 }, 1, { 15, 15, (limb_t *)limbs_ll_out14 } },
    { { 15, 15, (limb_t *)limbs_c15 }, 1, { 16, 16, (limb_t *)limbs_ll_out15 } },
    { { 16, 16, (limb_t *)limbs_c16 }, 1, { 17, 17, (limb_t *)limbs_ll_out16 } },
    { { 17, 17, (limb_t *)limbs_c17 }, 1, { 18, 18, (limb_t *)limbs_ll_out17 } },
    { { 18, 18, (limb_t *)limbs_c18 }, 1, { 19, 19, (limb_t *)limbs_ll_out18 } },
    { { 19, 19, (limb_t *)limbs_c19 }, 1, { 20, 20, (limb_t *)limbs_ll_out19 } },
    { { 20, 20, (limb_t *)limbs_c20 }, 1, { 21, 21, (limb_t *)limbs_ll_out20 } },
    { { 21, 21, (limb_t *)limbs_c21 }, 1, { 22, 22, (limb_t *)limbs_ll_out21 } },
    { { 22, 22, (limb_t *)limbs_c22 }, 1, { 23, 23, (limb_t *)limbs_ll_out22 } },
    { { 23, 23, (limb_t *)limbs_c23 }, 1, { 24, 24, (limb_t *)limbs_ll_out23 } },
    { { 24, 24, (limb_t *)limbs_c24 }, 1, { 25, 25, (limb_t *)limbs_ll_out24 } },
    { { 25, 25, (limb_t *)limbs_c25 }, 1, { 26, 26, (limb_t *)limbs_ll_out25 } },
    { { 26, 26, (limb_t *)limbs_c26 }, 1, { 27, 27, (limb_t *)limbs_ll_out26 } },
    { { 27, 27, (limb_t *)limbs_c27 }, 1, { 28, 28, (limb_t *)limbs_ll_out27 } },
    { { 28, 28, (limb_t *)limbs_c28 }, 1, { 29, 29, (limb_t *)limbs_ll_out28 } },
    { { 29, 29, (limb_t *)limbs_c29 }, 1, { 30, 30, (limb_t *)limbs_ll_out29 } },
    { { 30, 30, (limb_t *)limbs_c30 }, 1, { 31, 31, (limb_t *)limbs_ll_out30 } }
};

int main(void) { _libdnml_init();
    int total_tests = 0, passed_tests = 0;
    struct timespec start, end; clock_gettime(CLOCK_MONOTONIC, &start);
    limb_t *ret_buf = (limb_t *)malloc(128 * sizeof(limb_t)); 
    if (!ret_buf) return 1;
    printf("====================================================================\n");
    printf("     RUNNING INTEGRATED UNIT TESTS - BIGNUM LIMB-SHIFT UTILITIES    \n");
    printf("====================================================================\n");
    printf("---- __BIGINT_INTERNAL_RLSHIFT -----\n");
    for (int i = 0; i < 60; i++) { total_tests++; 
        memset(ret_buf, 0, 128 * sizeof(limb_t));
        if (rlshift_cases[i].in.limbs && rlshift_cases[i].in.n > 0) {
            memcpy(ret_buf, rlshift_cases[i].in.limbs, rlshift_cases[i].in.n * sizeof(limb_t));
        }
        bigInt test_x; test_x.limbs = ret_buf;
        test_x.n = rlshift_cases[i].in.n; test_x.cap = 128;
        __BIGINT_INTERNAL_RLSHIFT__(&test_x, rlshift_cases[i].klimbs);
        int match = (test_x.n == rlshift_cases[i].exp.n);
        if (match) match = memcmp(test_x.limbs, rlshift_cases[i].exp.limbs, rlshift_cases[i].exp.n * U64_BYTES) == 0;
        if (match) passed_tests++;
        else printf("[FAIL] RLSHIFT Case %2d: Missed structural matching requirements.\n", i);
    }
    printf("---- __BIGINT_INTERNAL_LLSHIFT -----\n");
    for (int i = 0; i < 60; i++) { total_tests++; 
        memset(ret_buf, 0, 128 * sizeof(limb_t));
        if (llshift_cases[i].in.limbs && llshift_cases[i].in.n > 0) {
            memcpy(ret_buf, llshift_cases[i].in.limbs, llshift_cases[i].in.n * sizeof(limb_t));
        }
        bigInt test_x; test_x.limbs = ret_buf;
        test_x.n = llshift_cases[i].in.n; test_x.cap = 128;
        __BIGINT_INTERNAL_LLSHIFT__(&test_x, llshift_cases[i].klimbs);
        int match = (test_x.n == llshift_cases[i].exp.n);
        if (match) match = memcmp(test_x.limbs, llshift_cases[i].exp.limbs, llshift_cases[i].exp.n * U64_BYTES) == 0;
        if (match) passed_tests++;
        else printf("[FAIL] LLSHIFT Case %2d: Missed structural matching requirements.\n", i);
    }

    /* Summary output block */
    clock_gettime(CLOCK_MONOTONIC, &end); free(ret_buf);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("=========================================================\n");
    printf("TEST SUMMARY:\n");
    printf("+) Passed %-4d out of %-4d total compiled checks.\n", passed_tests, total_tests);
    printf("+) Success rate: %.2f%%\n", (total_tests > 0) ? ((passed_tests * 100.0) / total_tests) : 0.0);
    printf("+) Total Runtime: %lf ms\n", elapsed_time * 1000.0);
    printf("=========================================================\n");
    _libdnml_cleanup(); return (passed_tests == total_tests) ? 0 : 1;
}
