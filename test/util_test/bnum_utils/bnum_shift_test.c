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
#define MAX_SIZE_T 96
typedef struct { const bigInt in; size_t k; const bigInt exp; } shift_case_t;
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

static const limb_t limbs_r_in[1] = {0x100000000ULL};
static const limb_t limbs_r_out1[1]  = {0x80000000ULL};   static const limb_t limbs_r_out2[1]  = {0x40000000ULL};
static const limb_t limbs_r_out3[1]  = {0x20000000ULL};   static const limb_t limbs_r_out4[1]  = {0x10000000ULL};
static const limb_t limbs_r_out5[1]  = {0x08000000ULL};   static const limb_t limbs_r_out6[1]  = {0x04000000ULL};
static const limb_t limbs_r_out7[1]  = {0x02000000ULL};   static const limb_t limbs_r_out8[1]  = {0x01000000ULL};
static const limb_t limbs_r_out9[1]  = {0x00800000ULL};   static const limb_t limbs_r_out10[1] = {0x00400000ULL};
static const limb_t limbs_r_out11[1] = {0x00200000ULL};   static const limb_t limbs_r_out12[1] = {0x00100000ULL};
static const limb_t limbs_r_out13[1] = {0x00080000ULL};   static const limb_t limbs_r_out14[1] = {0x00040000ULL};
static const limb_t limbs_r_out15[1] = {0x00020000ULL};   static const limb_t limbs_r_out16[1] = {0x00010000ULL};
static const limb_t limbs_r_out17[1] = {0x00008000ULL};   static const limb_t limbs_r_out18[1] = {0x00004000ULL};
static const limb_t limbs_r_out19[1] = {0x00002000ULL};   static const limb_t limbs_r_out20[1] = {0x00001000ULL};
static const limb_t limbs_r_out21[1] = {0x00000800ULL};   static const limb_t limbs_r_out22[1] = {0x00000400ULL};
static const limb_t limbs_r_out23[1] = {0x00000200ULL};   static const limb_t limbs_r_out24[1] = {0x00000100ULL};
static const limb_t limbs_r_out25[1] = {0x00000080ULL};   static const limb_t limbs_r_out26[1] = {0x00000040ULL};
static const limb_t limbs_r_out27[1] = {0x00000020ULL};   static const limb_t limbs_r_out28[1] = {0x00000010ULL};
static const limb_t limbs_r_out29[1] = {0x00000008ULL};   static const limb_t limbs_r_out30[1] = {0x00000004ULL};

static const limb_t limbs_l_in[1] = {1ULL};
static const limb_t limbs_l_out1[1]  = {2ULL};            static const limb_t limbs_l_out2[1]  = {4ULL};
static const limb_t limbs_l_out3[1]  = {8ULL};            static const limb_t limbs_l_out4[1]  = {16ULL};
static const limb_t limbs_l_out5[1]  = {32ULL};           static const limb_t limbs_l_out6[1]  = {64ULL};
static const limb_t limbs_l_out7[1]  = {128ULL};          static const limb_t limbs_l_out8[1]  = {256ULL};
static const limb_t limbs_l_out9[1]  = {512ULL};          static const limb_t limbs_l_out10[1] = {1024ULL};
static const limb_t limbs_l_out11[1] = {2048ULL};         static const limb_t limbs_l_out12[1] = {4096ULL};
static const limb_t limbs_l_out13[1] = {8192ULL};         static const limb_t limbs_l_out14[1] = {16384ULL};
static const limb_t limbs_l_out15[1] = {32768ULL};         static const limb_t limbs_l_out16[1] = {65536ULL};
static const limb_t limbs_l_out17[1] = {131072ULL};       static const limb_t limbs_l_out18[1] = {262144ULL};
static const limb_t limbs_l_out19[1] = {524288ULL};       static const limb_t limbs_l_out20[1] = {1048576ULL};
static const limb_t limbs_l_out21[1] = {2097152ULL};       static const limb_t limbs_l_out22[1] = {4194304ULL};
static const limb_t limbs_l_out23[1] = {8388608ULL};       static const limb_t limbs_l_out24[1] = {16777216ULL};
static const limb_t limbs_l_out25[1] = {33554432ULL};      static const limb_t limbs_l_out26[1] = {67108864ULL};
static const limb_t limbs_l_out27[1] = {134217728ULL};     static const limb_t limbs_l_out28[1] = {268435456ULL};
static const limb_t limbs_l_out29[1] = {536870912ULL};     static const limb_t limbs_l_out30[1] = {1073741824ULL};

//* ============== GLOBAL ARENA OF CASES ============== *//
static const shift_case_t rshift_cases[CASE_CNT] = {
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
    { { 1, 1, (limb_t *)limbs_r_in }, 1, { 1, 1, (limb_t *)limbs_r_out1 } },
    { { 1, 1, (limb_t *)limbs_r_in }, 2, { 1, 1, (limb_t *)limbs_r_out2 } },
    { { 1, 1, (limb_t *)limbs_r_in }, 3, { 1, 1, (limb_t *)limbs_r_out3 } },
    { { 1, 1, (limb_t *)limbs_r_in }, 4, { 1, 1, (limb_t *)limbs_r_out4 } },
    { { 1, 1, (limb_t *)limbs_r_in }, 5, { 1, 1, (limb_t *)limbs_r_out5 } },
    { { 1, 1, (limb_t *)limbs_r_in }, 6, { 1, 1, (limb_t *)limbs_r_out6 } },
    { { 1, 1, (limb_t *)limbs_r_in }, 7, { 1, 1, (limb_t *)limbs_r_out7 } },
    { { 1, 1, (limb_t *)limbs_r_in }, 8, { 1, 1, (limb_t *)limbs_r_out8 } },
    { { 1, 1, (limb_t *)limbs_r_in }, 9, { 1, 1, (limb_t *)limbs_r_out9 } },
    { { 1, 1, (limb_t *)limbs_r_in }, 10, { 1, 1, (limb_t *)limbs_r_out10 } },
    { { 1, 1, (limb_t *)limbs_r_in }, 11, { 1, 1, (limb_t *)limbs_r_out11 } },
    { { 1, 1, (limb_t *)limbs_r_in }, 12, { 1, 1, (limb_t *)limbs_r_out12 } },
    { { 1, 1, (limb_t *)limbs_r_in }, 13, { 1, 1, (limb_t *)limbs_r_out13 } },
    { { 1, 1, (limb_t *)limbs_r_in }, 14, { 1, 1, (limb_t *)limbs_r_out14 } },
    { { 1, 1, (limb_t *)limbs_r_in }, 15, { 1, 1, (limb_t *)limbs_r_out15 } },
    { { 1, 1, (limb_t *)limbs_r_in }, 16, { 1, 1, (limb_t *)limbs_r_out16 } },
    { { 1, 1, (limb_t *)limbs_r_in }, 17, { 1, 1, (limb_t *)limbs_r_out17 } },
    { { 1, 1, (limb_t *)limbs_r_in }, 18, { 1, 1, (limb_t *)limbs_r_out18 } },
    { { 1, 1, (limb_t *)limbs_r_in }, 19, { 1, 1, (limb_t *)limbs_r_out19 } },
    { { 1, 1, (limb_t *)limbs_r_in }, 20, { 1, 1, (limb_t *)limbs_r_out20 } },
    { { 1, 1, (limb_t *)limbs_r_in }, 21, { 1, 1, (limb_t *)limbs_r_out21 } },
    { { 1, 1, (limb_t *)limbs_r_in }, 22, { 1, 1, (limb_t *)limbs_r_out22 } },
    { { 1, 1, (limb_t *)limbs_r_in }, 23, { 1, 1, (limb_t *)limbs_r_out23 } },
    { { 1, 1, (limb_t *)limbs_r_in }, 24, { 1, 1, (limb_t *)limbs_r_out24 } },
    { { 1, 1, (limb_t *)limbs_r_in }, 25, { 1, 1, (limb_t *)limbs_r_out25 } },
    { { 1, 1, (limb_t *)limbs_r_in }, 26, { 1, 1, (limb_t *)limbs_r_out26 } },
    { { 1, 1, (limb_t *)limbs_r_in }, 27, { 1, 1, (limb_t *)limbs_r_out27 } },
    { { 1, 1, (limb_t *)limbs_r_in }, 28, { 1, 1, (limb_t *)limbs_r_out28 } },
    { { 1, 1, (limb_t *)limbs_r_in }, 29, { 1, 1, (limb_t *)limbs_r_out29 } },
    { { 1, 1, (limb_t *)limbs_r_in }, 30, { 1, 1, (limb_t *)limbs_r_out30 } }
};
static const shift_case_t lshift_cases[CASE_CNT] = {
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
    { { 1, 1, (limb_t *)limbs_l_in }, 1, { 1, 1, (limb_t *)limbs_l_out1 } },
    { { 1, 1, (limb_t *)limbs_l_in }, 2, { 1, 1, (limb_t *)limbs_l_out2 } },
    { { 1, 1, (limb_t *)limbs_l_in }, 3, { 1, 1, (limb_t *)limbs_l_out3 } },
    { { 1, 1, (limb_t *)limbs_l_in }, 4, { 1, 1, (limb_t *)limbs_l_out4 } },
    { { 1, 1, (limb_t *)limbs_l_in }, 5, { 1, 1, (limb_t *)limbs_l_out5 } },
    { { 1, 1, (limb_t *)limbs_l_in }, 6, { 1, 1, (limb_t *)limbs_l_out6 } },
    { { 1, 1, (limb_t *)limbs_l_in }, 7, { 1, 1, (limb_t *)limbs_l_out7 } },
    { { 1, 1, (limb_t *)limbs_l_in }, 8, { 1, 1, (limb_t *)limbs_l_out8 } },
    { { 1, 1, (limb_t *)limbs_l_in }, 9, { 1, 1, (limb_t *)limbs_l_out9 } },
    { { 1, 1, (limb_t *)limbs_l_in }, 10, { 1, 1, (limb_t *)limbs_l_out10 } },
    { { 1, 1, (limb_t *)limbs_l_in }, 11, { 1, 1, (limb_t *)limbs_l_out11 } },
    { { 1, 1, (limb_t *)limbs_l_in }, 12, { 1, 1, (limb_t *)limbs_l_out12 } },
    { { 1, 1, (limb_t *)limbs_l_in }, 13, { 1, 1, (limb_t *)limbs_l_out13 } },
    { { 1, 1, (limb_t *)limbs_l_in }, 14, { 1, 1, (limb_t *)limbs_l_out14 } },
    { { 1, 1, (limb_t *)limbs_l_in }, 15, { 1, 1, (limb_t *)limbs_l_out15 } },
    { { 1, 1, (limb_t *)limbs_l_in }, 16, { 1, 1, (limb_t *)limbs_l_out16 } },
    { { 1, 1, (limb_t *)limbs_l_in }, 17, { 1, 1, (limb_t *)limbs_l_out17 } },
    { { 1, 1, (limb_t *)limbs_l_in }, 18, { 1, 1, (limb_t *)limbs_l_out18 } },
    { { 1, 1, (limb_t *)limbs_l_in }, 19, { 1, 1, (limb_t *)limbs_l_out19 } },
    { { 1, 1, (limb_t *)limbs_l_in }, 20, { 1, 1, (limb_t *)limbs_l_out20 } },
    { { 1, 1, (limb_t *)limbs_l_in }, 21, { 1, 1, (limb_t *)limbs_l_out21 } },
    { { 1, 1, (limb_t *)limbs_l_in }, 22, { 1, 1, (limb_t *)limbs_l_out22 } },
    { { 1, 1, (limb_t *)limbs_l_in }, 23, { 1, 1, (limb_t *)limbs_l_out23 } },
    { { 1, 1, (limb_t *)limbs_l_in }, 24, { 1, 1, (limb_t *)limbs_l_out24 } },
    { { 1, 1, (limb_t *)limbs_l_in }, 25, { 1, 1, (limb_t *)limbs_l_out25 } },
    { { 1, 1, (limb_t *)limbs_l_in }, 26, { 1, 1, (limb_t *)limbs_l_out26 } },
    { { 1, 1, (limb_t *)limbs_l_in }, 27, { 1, 1, (limb_t *)limbs_l_out27 } },
    { { 1, 1, (limb_t *)limbs_l_in }, 28, { 1, 1, (limb_t *)limbs_l_out28 } },
    { { 1, 1, (limb_t *)limbs_l_in }, 29, { 1, 1, (limb_t *)limbs_l_out29 } },
    { { 1, 1, (limb_t *)limbs_l_in }, 30, { 1, 1, (limb_t *)limbs_l_out30 } }
};

int main(void) { _libdnml_init();
    int total_tests = 0, passed_tests = 0;
    struct timespec start, end; clock_gettime(CLOCK_MONOTONIC, &start);
    limb_t *ret_buf = (limb_t *)malloc(MAX_SIZE_T * sizeof(limb_t)); assert(ret_buf != NULL);
    fputs("====================================================================\n", stdout);
    fputs("     RUNNING INTEGRATED UNIT TESTS - BIGNUM BIT-SHIFT UTILITIES     \n", stdout);
    fputs("====================================================================\n", stdout);
    fputs("---- __BIGINT_INTERNAL_RSHIFT__ -----\n", stdout);
    for (int i = 0; i < CASE_CNT; i++) { total_tests++; 
        memset(ret_buf, 0, MAX_SIZE_T * sizeof(limb_t));
        if (rshift_cases[i].in.limbs && rshift_cases[i].in.n > 0) {
            memcpy(ret_buf, rshift_cases[i].in.limbs, rshift_cases[i].in.n * sizeof(limb_t));
        }
        bigInt test_x; test_x.limbs = ret_buf;
        test_x.n = rshift_cases[i].in.n; test_x.cap = MAX_SIZE_T;
        __BIGINT_INTERNAL_RSHIFT__(&test_x, rshift_cases[i].k);
        int match = (test_x.n == rshift_cases[i].exp.n);
        if (match) match = memcmp(test_x.limbs, rshift_cases[i].exp.limbs, rshift_cases[i].exp.n * U64_BYTES) == 0;
        if (match) passed_tests++;
        else printf("[FAIL] RSHIFT Case %2d: Missed structural matching requirements.\n", i);
    }
    fputs("---- __BIGINT_INTERNAL_LSHIFT__ -----\n", stdout);
    for (int i = 0; i < CASE_CNT; i++) { total_tests++; 
        memset(ret_buf, 0, MAX_SIZE_T * sizeof(limb_t));
        if (lshift_cases[i].in.limbs && lshift_cases[i].in.n > 0) {
            memcpy(ret_buf, lshift_cases[i].in.limbs, lshift_cases[i].in.n * sizeof(limb_t));
        }
        bigInt test_x; test_x.limbs = ret_buf;
        test_x.n = lshift_cases[i].in.n; test_x.cap = MAX_SIZE_T;
        __BIGINT_INTERNAL_LSHIFT__(&test_x, lshift_cases[i].k);
        int match = (test_x.n == lshift_cases[i].exp.n);
        if (match) match = memcmp(test_x.limbs, lshift_cases[i].exp.limbs, lshift_cases[i].exp.n * U64_BYTES) == 0;
        if (match) passed_tests++;
        else printf("[FAIL] LSHIFT Case %2d: Missed structural matching requirements.\n", i);
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
    _libdnml_cleanup(); return (passed_tests == total_tests) ? 0 : 1;
}
