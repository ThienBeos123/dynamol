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
typedef struct { const bigInt x; uint8_t base; size_t out; } countdb_case_t;
typedef struct { size_t lcnt; uint8_t base; size_t out; } maxcdb_case_t;
typedef struct { const bigInt x; size_t out; } ctz_case_t;
/* ========== GLOBAL ARRAYS OF CASES ========== */
/* --- CountDB Limb Storage --- */
static limb_t cdb_l_0[1] = { UINT64_C(0x0000000000000000) }, cdb_l_1[1] = { UINT64_C(0x0000000000000001) };
static limb_t cdb_l_2[1] = { UINT64_C(0x0000000000000002) }, cdb_l_3[1] = { UINT64_C(0x0000000000000004) };
static limb_t cdb_l_4[1] = { UINT64_C(0x0000000000000008) }, cdb_l_5[1] = { UINT64_C(0x000000000000000A) };
static limb_t cdb_l_6[1] = { UINT64_C(0x000000000000000F) }, cdb_l_7[1] = { UINT64_C(0x0000000000000010) };
static limb_t cdb_l_8[1] = { UINT64_C(0x0000000000000020) }, cdb_l_9[1] = { UINT64_C(0x0000000000000040) };
static limb_t cdb_l_10[1] = { UINT64_C(0x0000000000000064) }, cdb_l_11[1] = { UINT64_C(0x0000000000000080) };
static limb_t cdb_l_12[1] = { UINT64_C(0x00000000000000FF) }, cdb_l_13[1] = { UINT64_C(0x0000000000000100) };
static limb_t cdb_l_14[1] = { UINT64_C(0x0000000000000200) }, cdb_l_15[1] = { UINT64_C(0x00000000000003E8) };
static limb_t cdb_l_16[1] = { UINT64_C(0x0000000000000400) }, cdb_l_17[1] = { UINT64_C(0x0000000000000800) };
static limb_t cdb_l_18[1] = { UINT64_C(0x0000000000000FFF) }, cdb_l_19[1] = { UINT64_C(0x0000000000001000) };
static limb_t cdb_l_20[1] = { UINT64_C(0x0000000000002000) }, cdb_l_21[1] = { UINT64_C(0x0000000000002710) };
static limb_t cdb_l_22[1] = { UINT64_C(0x0000000000004000) }, cdb_l_23[1] = { UINT64_C(0x0000000000008000) };
static limb_t cdb_l_24[1] = { UINT64_C(0x000000000000FFFF) }, cdb_l_25[1] = { UINT64_C(0x0000000000010000) };
static limb_t cdb_l_26[1] = { UINT64_C(0x00000000000186A0) }, cdb_l_27[1] = { UINT64_C(0x0000000000020000) };
static limb_t cdb_l_28[1] = { UINT64_C(0x0000000000040000) }, cdb_l_29[1] = { UINT64_C(0x0000000000080000) };
static limb_t cdb_l_30[1] = { UINT64_C(0x00000000000F4240) }, cdb_l_31[1] = { UINT64_C(0x00000000000FFFFF) };
static limb_t cdb_l_32[1] = { UINT64_C(0x0000000000100000) }, cdb_l_33[1] = { UINT64_C(0x0000000000200000) };
static limb_t cdb_l_34[1] = { UINT64_C(0x0000000000400000) }, cdb_l_35[1] = { UINT64_C(0x0000000000800000) };
static limb_t cdb_l_36[1] = { UINT64_C(0x0000000000989680) }, cdb_l_37[1] = { UINT64_C(0x0000000000FFFFFF) };
static limb_t cdb_l_38[1] = { UINT64_C(0x0000000001000000) }, cdb_l_39[1] = { UINT64_C(0x0000000002000000) };
static limb_t cdb_l_40[1] = { UINT64_C(0x0000000004000000) }, cdb_l_41[1] = { UINT64_C(0x0000000005F5E100) };
static limb_t cdb_l_42[1] = { UINT64_C(0x0000000008000000) }, cdb_l_43[1] = { UINT64_C(0x000000000FFFFFFF) };
static limb_t cdb_l_44[1] = { UINT64_C(0x0000000010000000) }, cdb_l_45[1] = { UINT64_C(0x0000000020000000) };
static limb_t cdb_l_46[1] = { UINT64_C(0x000000003B9ACA00) }, cdb_l_47[1] = { UINT64_C(0x0000000040000000) };
static limb_t cdb_l_48[1] = { UINT64_C(0x0000000080000000) }, cdb_l_49[1] = { UINT64_C(0x00000000FFFFFFFF) };
static limb_t cdb_l_50[1] = { UINT64_C(0x0000000100000000) }, cdb_l_51[1] = { UINT64_C(0x0000000200000000) };
static limb_t cdb_l_52[1] = { UINT64_C(0x00000002540BE400) }, cdb_l_53[1] = { UINT64_C(0x0000000400000000) };
static limb_t cdb_l_54[1] = { UINT64_C(0x0000000800000000) }, cdb_l_55[1] = { UINT64_C(0x0000000FFFFFFFFF) };
static limb_t cdb_l_56[1] = { UINT64_C(0x0000001000000000) }, cdb_l_57[1] = { UINT64_C(0x000000174876E800) };
static limb_t cdb_l_58[1] = { UINT64_C(0x0000002000000000) }, cdb_l_59[1] = { UINT64_C(0x0000004000000000) };
static limb_t cdb_l_60[1] = { UINT64_C(0x0000008000000000) }, cdb_l_61[1] = { UINT64_C(0x000000E8D4A51000) };
static limb_t cdb_l_62[1] = { UINT64_C(0x000000FFFFFFFFFF) }, cdb_l_63[1] = { UINT64_C(0x0000010000000000) };
static limb_t cdb_l_64[1] = { UINT64_C(0x0000020000000000) }, cdb_l_65[1] = { UINT64_C(0x0000040000000000) };
static limb_t cdb_l_66[1] = { UINT64_C(0x0000080000000000) }, cdb_l_67[1] = { UINT64_C(0x000009184E72A000) };
static limb_t cdb_l_68[1] = { UINT64_C(0x00000FFFFFFFFFFF) }, cdb_l_69[1] = { UINT64_C(0x0000100000000000) };
static const countdb_case_t countdb_cases[70] = {
    { { 0, 1, cdb_l_0, 1 }, 2, 0 }, /* 0 */ { { 1, 1, cdb_l_1, 1 }, 8, 1 }, /* 1 */
    { { 1, 1, cdb_l_2, 1 }, 10, 1 }, /* 2 */ { { 1, 1, cdb_l_3, 1 }, 16, 1 }, /* 3 */
    { { 1, 1, cdb_l_4, 1 }, 3, 2 }, /* 4 */ { { 1, 1, cdb_l_5, 1 }, 5, 2 }, /* 5 */
    { { 1, 1, cdb_l_6, 1 }, 7, 2 }, /* 6 */ { { 1, 1, cdb_l_7, 1 }, 2, 5 }, /* 7 */
    { { 1, 1, cdb_l_8, 1 }, 8, 2 }, /* 8 */ { { 1, 1, cdb_l_9, 1 }, 10, 2 }, /* 9 */
    { { 1, 1, cdb_l_10, 1 }, 16, 2 }, /* 10 */ { { 1, 1, cdb_l_11, 1 }, 3, 5 }, /* 11 */
    { { 1, 1, cdb_l_12, 1 }, 5, 4 }, /* 12 */ { { 1, 1, cdb_l_13, 1 }, 7, 3 }, /* 13 */
    { { 1, 1, cdb_l_14, 1 }, 2, 10 }, /* 14 */ { { 1, 1, cdb_l_15, 1 }, 8, 4 }, /* 15 */
    { { 1, 1, cdb_l_16, 1 }, 10, 4 }, /* 16 */ { { 1, 1, cdb_l_17, 1 }, 16, 3 }, /* 17 */
    { { 1, 1, cdb_l_18, 1 }, 3, 8 }, /* 18 */ { { 1, 1, cdb_l_19, 1 }, 5, 6 }, /* 19 */
    { { 1, 1, cdb_l_20, 1 }, 7, 5 }, /* 20 */ { { 1, 1, cdb_l_21, 1 }, 2, 14 }, /* 21 */
    { { 1, 1, cdb_l_22, 1 }, 8, 5 }, /* 22 */ { { 1, 1, cdb_l_23, 1 }, 10, 5 }, /* 23 */
    { { 1, 1, cdb_l_24, 1 }, 16, 4 }, /* 24 */ { { 1, 1, cdb_l_25, 1 }, 3, 11 }, /* 25 */
    { { 1, 1, cdb_l_26, 1 }, 5, 8 }, /* 26 */ { { 1, 1, cdb_l_27, 1 }, 7, 7 }, /* 27 */
    { { 1, 1, cdb_l_28, 1 }, 2, 19 }, /* 28 */ { { 1, 1, cdb_l_29, 1 }, 8, 7 }, /* 29 */
    { { 1, 1, cdb_l_30, 1 }, 10, 7 }, /* 30 */ { { 1, 1, cdb_l_31, 1 }, 16, 5 }, /* 31 */
    { { 1, 1, cdb_l_32, 1 }, 3, 13 }, /* 32 */ { { 1, 1, cdb_l_33, 1 }, 5, 10 }, /* 33 */
    { { 1, 1, cdb_l_34, 1 }, 7, 8 }, /* 34 */ { { 1, 1, cdb_l_35, 1 }, 2, 24 }, /* 35 */
    { { 1, 1, cdb_l_36, 1 }, 8, 8 }, /* 36 */ { { 1, 1, cdb_l_37, 1 }, 10, 8 }, /* 37 */
    { { 1, 1, cdb_l_38, 1 }, 16, 7 }, /* 38 */ { { 1, 1, cdb_l_39, 1 }, 3, 16 }, /* 39 */
    { { 1, 1, cdb_l_40, 1 }, 5, 12 }, /* 40 */ { { 1, 1, cdb_l_41, 1 }, 7, 10 }, /* 41 */
    { { 1, 1, cdb_l_42, 1 }, 2, 28 }, /* 42 */ { { 1, 1, cdb_l_43, 1 }, 8, 10 }, /* 43 */
    { { 1, 1, cdb_l_44, 1 }, 10, 9 }, /* 44 */ { { 1, 1, cdb_l_45, 1 }, 16, 8 }, /* 45 */
    { { 1, 1, cdb_l_46, 1 }, 3, 19 }, /* 46 */ { { 1, 1, cdb_l_47, 1 }, 5, 13 }, /* 47 */
    { { 1, 1, cdb_l_48, 1 }, 7, 12 }, /* 48 */ { { 1, 1, cdb_l_49, 1 }, 2, 32 }, /* 49 */
    { { 1, 1, cdb_l_50, 1 }, 8, 11 }, /* 50 */ { { 1, 1, cdb_l_51, 1 }, 10, 10 }, /* 51 */
    { { 1, 1, cdb_l_52, 1 }, 16, 9 }, /* 52 */ { { 1, 1, cdb_l_53, 1 }, 3, 22 }, /* 53 */
    { { 1, 1, cdb_l_54, 1 }, 5, 16 }, /* 54 */ { { 1, 1, cdb_l_55, 1 }, 7, 13 }, /* 55 */
    { { 1, 1, cdb_l_56, 1 }, 2, 37 }, /* 56 */ { { 1, 1, cdb_l_57, 1 }, 8, 13 }, /* 57 */
    { { 1, 1, cdb_l_58, 1 }, 10, 12 }, /* 58 */ { { 1, 1, cdb_l_59, 1 }, 16, 10 }, /* 59 */
    { { 1, 1, cdb_l_60, 1 }, 3, 25 }, /* 60 */ { { 1, 1, cdb_l_61, 1 }, 5, 18 }, /* 61 */
    { { 1, 1, cdb_l_62, 1 }, 7, 15 }, /* 62 */ { { 1, 1, cdb_l_63, 1 }, 2, 41 }, /* 63 */
    { { 1, 1, cdb_l_64, 1 }, 8, 14 }, /* 64 */ { { 1, 1, cdb_l_65, 1 }, 10, 13 }, /* 65 */
    { { 1, 1, cdb_l_66, 1 }, 16, 11 }, /* 66 */ { { 1, 1, cdb_l_67, 1 }, 3, 28 }, /* 67 */
    { { 1, 1, cdb_l_68, 1 }, 5, 19 }, /* 68 */ { { 1, 1, cdb_l_69, 1 }, 7, 16 }, /* 69 */
};
/* --- MaxCDB cases --- */
static const maxcdb_case_t maxcdb_cases[70] = {
    { 1, 2, 65 }, /* 0 */ { 2, 8, 43 }, /* 1 */ { 3, 10, 58 }, /* 2 */
    { 4, 16, 65 }, /* 3 */ { 5, 3, 202 }, /* 4 */ { 6, 5, 166 }, /* 5 */
    { 1, 7, 23 }, /* 6 */ { 2, 2, 129 }, /* 7 */ { 3, 8, 65 }, /* 8 */
    { 4, 10, 78 }, /* 9 */ { 5, 16, 81 }, /* 10 */ { 6, 3, 243 }, /* 11 */
    { 1, 5, 28 }, /* 12 */ { 2, 7, 46 }, /* 13 */ { 3, 2, 193 }, /* 14 */
    { 4, 8, 86 }, /* 15 */ { 5, 10, 97 }, /* 16 */ { 6, 16, 97 }, /* 17 */
    { 1, 3, 41 }, /* 18 */ { 2, 5, 56 }, /* 19 */ { 3, 7, 69 }, /* 20 */
    { 4, 2, 257 }, /* 21 */ { 5, 8, 107 }, /* 22 */ { 6, 10, 116 }, /* 23 */
    { 1, 16, 17 }, /* 24 */ { 2, 3, 81 }, /* 25 */ { 3, 5, 83 }, /* 26 */
    { 4, 7, 92 }, /* 27 */ { 5, 2, 321 }, /* 28 */ { 6, 8, 129 }, /* 29 */
    { 1, 10, 20 }, /* 30 */ { 2, 16, 33 }, /* 31 */ { 3, 3, 122 }, /* 32 */
    { 4, 5, 111 }, /* 33 */ { 5, 7, 114 }, /* 34 */ { 6, 2, 385 }, /* 35 */
    { 1, 8, 22 }, /* 36 */ { 2, 10, 39 }, /* 37 */ { 3, 16, 49 }, /* 38 */
    { 4, 3, 162 }, /* 39 */ { 5, 5, 138 }, /* 40 */ { 6, 7, 137 }, /* 41 */
    { 1, 2, 65 }, /* 42 */ { 2, 8, 43 }, /* 43 */ { 3, 10, 58 }, /* 44 */
    { 4, 16, 65 }, /* 45 */ { 5, 3, 202 }, /* 46 */ { 6, 5, 166 }, /* 47 */
    { 1, 7, 23 }, /* 48 */ { 2, 2, 129 }, /* 49 */ { 3, 8, 65 }, /* 50 */
    { 4, 10, 78 }, /* 51 */ { 5, 16, 81 }, /* 52 */ { 6, 3, 243 }, /* 53 */
    { 1, 5, 28 }, /* 54 */ { 2, 7, 46 }, /* 55 */ { 3, 2, 193 }, /* 56 */
    { 4, 8, 86 }, /* 57 */ { 5, 10, 97 }, /* 58 */ { 6, 16, 97 }, /* 59 */
    { 1, 3, 41 }, /* 60 */ { 2, 5, 56 }, /* 61 */ { 3, 7, 69 }, /* 62 */
    { 4, 2, 257 }, /* 63 */ { 5, 8, 107 }, /* 64 */ { 6, 10, 116 }, /* 65 */
    { 1, 16, 17 }, /* 66 */ { 2, 3, 81 }, /* 67 */ { 3, 5, 83 }, /* 68 */
    { 4, 7, 92 }, /* 69 */
};
/* --- CTZ Limb Storage --- */
static limb_t ctz_l_0[1] = { UINT64_C(0x0000000000000001) }, ctz_l_1[1] = { UINT64_C(0x0000000000000020) };
static limb_t ctz_l_2[1] = { UINT64_C(0x0000000000000400) }, ctz_l_3[1] = { UINT64_C(0x0000000000008000) };
static limb_t ctz_l_4[1] = { UINT64_C(0x0000000000100000) }, ctz_l_5[1] = { UINT64_C(0x0000000002000000) };
static limb_t ctz_l_6[1] = { UINT64_C(0x0000000040000000) }, ctz_l_7[1] = { UINT64_C(0x0000000800000000) };
static limb_t ctz_l_8[1] = { UINT64_C(0x0000010000000000) }, ctz_l_9[1] = { UINT64_C(0x0000200000000000) };
static limb_t ctz_l_10[1] = { UINT64_C(0x0004000000000000) }, ctz_l_11[1] = { UINT64_C(0x0080000000000000) };
static limb_t ctz_l_12[1] = { UINT64_C(0x1000000000000000) };
static limb_t ctz_l_13[2] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000002) };
static limb_t ctz_l_14[2] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000040) };
static limb_t ctz_l_15[2] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000800) };
static limb_t ctz_l_16[2] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000010000) };
static limb_t ctz_l_17[2] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000200000) };
static limb_t ctz_l_18[2] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000004000000) };
static limb_t ctz_l_19[2] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000080000000) };
static limb_t ctz_l_20[2] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000001000000000) };
static limb_t ctz_l_21[2] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000020000000000) };
static limb_t ctz_l_22[2] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000400000000000) };
static limb_t ctz_l_23[2] = { UINT64_C(0x0000000000000000), UINT64_C(0x0008000000000000) };
static limb_t ctz_l_24[2] = { UINT64_C(0x0000000000000000), UINT64_C(0x0100000000000000) };
static limb_t ctz_l_25[2] = { UINT64_C(0x0000000000000000), UINT64_C(0x2000000000000000) };
static limb_t ctz_l_26[3] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000004) };
static limb_t ctz_l_27[3] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000080) };
static limb_t ctz_l_28[3] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000001000) };
static limb_t ctz_l_29[3] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000020000) };
static limb_t ctz_l_30[3] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000400000) };
static limb_t ctz_l_31[3] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000008000000) };
static limb_t ctz_l_32[3] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000100000000) };
static limb_t ctz_l_33[3] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000002000000000) };
static limb_t ctz_l_34[3] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000040000000000) };
static limb_t ctz_l_35[3] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000800000000000) };
static limb_t ctz_l_36[3] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0010000000000000) };
static limb_t ctz_l_37[3] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0200000000000000) };
static limb_t ctz_l_38[3] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x4000000000000000) };
static limb_t ctz_l_39[4] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000008) };
static limb_t ctz_l_40[4] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000100) };
static limb_t ctz_l_41[4] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000002000) };
static limb_t ctz_l_42[4] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000040000) };
static limb_t ctz_l_43[4] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000800000) };
static limb_t ctz_l_44[4] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000010000000) };
static limb_t ctz_l_45[4] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000200000000) };
static limb_t ctz_l_46[4] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000004000000000) };
static limb_t ctz_l_47[4] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000080000000000) };
static limb_t ctz_l_48[4] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0001000000000000) };
static limb_t ctz_l_49[4] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0020000000000000) };
static limb_t ctz_l_50[4] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0400000000000000) };
static limb_t ctz_l_51[4] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x8000000000000000) };
static limb_t ctz_l_52[5] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000010) };
static limb_t ctz_l_53[5] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000200) };
static limb_t ctz_l_54[5] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000004000) };
static limb_t ctz_l_55[5] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000080000) };
static limb_t ctz_l_56[5] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000001000000) };
static limb_t ctz_l_57[5] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000020000000) };
static limb_t ctz_l_58[5] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000400000000) };
static limb_t ctz_l_59[5] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000008000000000) };
static limb_t ctz_l_60[5] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000100000000000) };
static limb_t ctz_l_61[5] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0002000000000000) };
static limb_t ctz_l_62[5] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0040000000000000) };
static limb_t ctz_l_63[5] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0800000000000000) };
static limb_t ctz_l_64[6] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000001) };
static limb_t ctz_l_65[6] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000020) };
static limb_t ctz_l_66[6] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000400) };
static limb_t ctz_l_67[6] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000008000) };
static limb_t ctz_l_68[6] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000100000) };
static limb_t ctz_l_69[6] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000002000000) };

static const ctz_case_t ctz_cases[70] = {
    { { 1, 1, ctz_l_0, 1 }, 0 }, /* 0 */ { { 1, 1, ctz_l_1, 1 }, 5 }, /* 1 */ { { 1, 1, ctz_l_2, 1 }, 10 }, /* 2 */
    { { 1, 1, ctz_l_3, 1 }, 15 }, /* 3 */ { { 1, 1, ctz_l_4, 1 }, 20 }, /* 4 */ { { 1, 1, ctz_l_5, 1 }, 25 }, /* 5 */
    { { 1, 1, ctz_l_6, 1 }, 30 }, /* 6 */ { { 1, 1, ctz_l_7, 1 }, 35 }, /* 7 */ { { 1, 1, ctz_l_8, 1 }, 40 }, /* 8 */
    { { 1, 1, ctz_l_9, 1 }, 45 }, /* 9 */ { { 1, 1, ctz_l_10, 1 }, 50 }, /* 10 */ { { 1, 1, ctz_l_11, 1 }, 55 }, /* 11 */
    { { 1, 1, ctz_l_12, 1 }, 60 }, /* 12 */ { { 2, 2, ctz_l_13, 1 }, 65 }, /* 13 */
    { { 2, 2, ctz_l_14, 1 }, 70 }, /* 14 */ { { 2, 2, ctz_l_15, 1 }, 75 }, /* 15 */
    { { 2, 2, ctz_l_16, 1 }, 80 }, /* 16 */ { { 2, 2, ctz_l_17, 1 }, 85 }, /* 17 */
    { { 2, 2, ctz_l_18, 1 }, 90 }, /* 18 */ { { 2, 2, ctz_l_19, 1 }, 95 }, /* 19 */
    { { 2, 2, ctz_l_20, 1 }, 100 }, /* 20 */ { { 2, 2, ctz_l_21, 1 }, 105 }, /* 21 */
    { { 2, 2, ctz_l_22, 1 }, 110 }, /* 22 */ { { 2, 2, ctz_l_23, 1 }, 115 }, /* 23 */
    { { 2, 2, ctz_l_24, 1 }, 120 }, /* 24 */ { { 2, 2, ctz_l_25, 1 }, 125 }, /* 25 */
    { { 3, 3, ctz_l_26, 1 }, 130 }, /* 26 */ { { 3, 3, ctz_l_27, 1 }, 135 }, /* 27 */
    { { 3, 3, ctz_l_28, 1 }, 140 }, /* 28 */ { { 3, 3, ctz_l_29, 1 }, 145 }, /* 29 */
    { { 3, 3, ctz_l_30, 1 }, 150 }, /* 30 */ { { 3, 3, ctz_l_31, 1 }, 155 }, /* 31 */
    { { 3, 3, ctz_l_32, 1 }, 160 }, /* 32 */ { { 3, 3, ctz_l_33, 1 }, 165 }, /* 33 */
    { { 3, 3, ctz_l_34, 1 }, 170 }, /* 34 */ { { 3, 3, ctz_l_35, 1 }, 175 }, /* 35 */ 
    { { 3, 3, ctz_l_36, 1 }, 180 }, /* 36 */ { { 3, 3, ctz_l_37, 1 }, 185 }, /* 37 */
    { { 3, 3, ctz_l_38, 1 }, 190 }, /* 38 */ { { 4, 4, ctz_l_39, 1 }, 195 }, /* 39 */
    { { 4, 4, ctz_l_40, 1 }, 200 }, /* 40 */ { { 4, 4, ctz_l_41, 1 }, 205 }, /* 41 */
    { { 4, 4, ctz_l_42, 1 }, 210 }, /* 42 */ { { 4, 4, ctz_l_43, 1 }, 215 }, /* 43 */
    { { 4, 4, ctz_l_44, 1 }, 220 }, /* 44 */ { { 4, 4, ctz_l_45, 1 }, 225 }, /* 45 */
    { { 4, 4, ctz_l_46, 1 }, 230 }, /* 46 */ { { 4, 4, ctz_l_47, 1 }, 235 }, /* 47 */
    { { 4, 4, ctz_l_48, 1 }, 240 }, /* 48 */ { { 4, 4, ctz_l_49, 1 }, 245 }, /* 49 */
    { { 4, 4, ctz_l_50, 1 }, 250 }, /* 50 */ { { 4, 4, ctz_l_51, 1 }, 255 }, /* 51 */
    { { 5, 5, ctz_l_52, 1 }, 260 }, /* 52 */ { { 5, 5, ctz_l_53, 1 }, 265 }, /* 53 */
    { { 5, 5, ctz_l_54, 1 }, 270 }, /* 54 */ { { 5, 5, ctz_l_55, 1 }, 275 }, /* 55 */
    { { 5, 5, ctz_l_56, 1 }, 280 }, /* 56 */ { { 5, 5, ctz_l_57, 1 }, 285 }, /* 57 */
    { { 5, 5, ctz_l_58, 1 }, 290 }, /* 58 */ { { 5, 5, ctz_l_59, 1 }, 295 }, /* 59 */
    { { 5, 5, ctz_l_60, 1 }, 300 }, /* 60 */ { { 5, 5, ctz_l_61, 1 }, 305 }, /* 61 */
    { { 5, 5, ctz_l_62, 1 }, 310 }, /* 62 */ { { 5, 5, ctz_l_63, 1 }, 315 }, /* 63 */ 
    { { 6, 6, ctz_l_64, 1 }, 320 }, /* 64 */ { { 6, 6, ctz_l_65, 1 }, 325 }, /* 65 */
    { { 6, 6, ctz_l_66, 1 }, 330 }, /* 66 */ { { 6, 6, ctz_l_67, 1 }, 335 }, /* 67 */
    { { 6, 6, ctz_l_68, 1 }, 340 }, /* 68 */ { { 6, 6, ctz_l_69, 1 }, 345 }, /* 69 */
};

int main(void) {
    _libdnml_init();
    int total_tests = 0, passed_tests = 0;
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    limb_t *l_buf = (limb_t *)malloc(6 * sizeof(limb_t));
    printf("===================================================================\n");
    printf("     RUNNING INTEGRATED UNIT TESTS - BIGNUM DIGIT/TZ UTILITIES     \n");
    printf("===================================================================\n");
    printf("---- __BIGINT_COUNTDB__ -----\n");
    for (int i = 0; i < 70; i++) { total_tests++;
        bigInt tx = countdb_cases[i].x; tx.limbs = l_buf;
        memcpy(l_buf, countdb_cases[i].x.limbs, tx.n * sizeof(limb_t));
        size_t res = __BIGINT_COUNTDB__(&tx, countdb_cases[i].base);
        if (res == countdb_cases[i].out) passed_tests++;
        else printf(
            "Case: %2d | Input struct = { .limbs = SEE case_%02d, "
            ".n = %zu } | exp: %zu | ret: %zu\n", 
            i, i, tx.n, countdb_cases[i].out, res
        );
    }
    printf("---- __BIGINT_MAXCDB__ -----\n");
    for (int i = 0; i < 70; i++) { total_tests++;
        size_t res = __BIGINT_MAXCDB__(maxcdb_cases[i].lcnt, maxcdb_cases[i].base);
        if (res == maxcdb_cases[i].out) passed_tests++;
        else printf(
            "Case: %2d | Input lcnt = %zu, base = %u | exp: %zu | ret: %zu\n", 
            i, maxcdb_cases[i].lcnt, maxcdb_cases[i].base, maxcdb_cases[i].out, res
        );
    }
    printf("---- __BIGINT_CTZ__ -----\n");
    for (int i = 0; i < 70; i++) { total_tests++;
        bigInt tx = ctz_cases[i].x; tx.limbs = l_buf;
        memcpy(l_buf, ctz_cases[i].x.limbs, tx.n * sizeof(limb_t));
        size_t res = __BIGINT_CTZ__(&tx);
        if (res == ctz_cases[i].out) passed_tests++;
        else printf(
            "Case: %2d | Input struct = { .limbs = SEE case_%02d,"
            " .n = %zu } | exp: %zu | ret: %zu\n", 
            i, i, tx.n, ctz_cases[i].out, res
        );
    }

    free(l_buf); clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("=========================================================\n");
    printf("TEST SUMMARY:\n");
    printf("+) Passed %-4d out of %-4d total compiled checks.\n", passed_tests, total_tests);
    printf("+) Success rate: %.2f%%\n", (passed_tests * 100.0) / total_tests);
    printf("+) Total Runtime: %lf ms\n", elapsed_time * 1000.0);
    printf("=========================================================\n");
    _libdnml_cleanup(); return (passed_tests == total_tests) ? 0 : 1;
}
