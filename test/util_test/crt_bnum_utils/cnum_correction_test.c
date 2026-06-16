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
#include "../../../util/crt_util.h"
#include "../../../libdnml_base.h"
#define CASE_CNT 80
#define MAX_SIZE_T 5
typedef struct { const crint in; const crint exp; } case_t;
// ===================================================================
// DISCRETE EXTERNAL LIMB STORAGE FOR TEST CASES
// ===================================================================
static limb_t in_0[5]  = { UINT64_C(0x0000000000000000) };
static limb_t exp_0[5] = { 0 };
static limb_t in_1[5]  = { UINT64_C(0x0000000000000005) };
static limb_t exp_1[5] = { UINT64_C(0x0000000000000005) };
static limb_t in_2[5]  = { UINT64_C(0x0000000000000005), UINT64_C(0x0000000000000000) };
static limb_t exp_2[5] = { UINT64_C(0x0000000000000005) };
static limb_t in_3[5]  = { UINT64_C(0x0000000000000005), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000) };
static limb_t exp_3[5] = { UINT64_C(0x0000000000000005) };
static limb_t in_4[5]  = { UINT64_C(0x0000000000000005), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000) };
static limb_t exp_4[5] = { UINT64_C(0x0000000000000005) };
static limb_t in_5[5]  = { UINT64_C(0x0000000000000005), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000) };
static limb_t exp_5[5] = { UINT64_C(0x0000000000000005) };
static limb_t in_6[5]  = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000) };
static limb_t exp_6[5] = { 0 };
static limb_t in_7[5]  = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000) };
static limb_t exp_7[5] = { 0 };
static limb_t in_8[5]  = { UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000000) };
static limb_t exp_8[5] = { UINT64_C(0xFFFFFFFFFFFFFFFF) };
static limb_t in_9[5]  = { UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000000) };
static limb_t exp_9[5] = { UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF) };
static limb_t in_10[5] = { UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000002), UINT64_C(0x0000000000000000) };
static limb_t exp_10[5]= { UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000002) };
static limb_t in_11[5] = { UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000002), UINT64_C(0x0000000000000003), UINT64_C(0x0000000000000000) };
static limb_t exp_11[5]= { UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000002), UINT64_C(0x0000000000000003) };
static limb_t in_12[5] = { UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000002), UINT64_C(0x0000000000000003), UINT64_C(0x0000000000000004), UINT64_C(0x0000000000000000) };
static limb_t exp_12[5]= { UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000002), UINT64_C(0x0000000000000003), UINT64_C(0x0000000000000004) };
static limb_t in_13[5] = { UINT64_C(0x5555555555555555), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000) };
static limb_t exp_13[5]= { UINT64_C(0x5555555555555555) };
static limb_t in_14[5] = { UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000) };
static limb_t exp_14[5]= { UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0xAAAAAAAAAAAAAAAA) };
static limb_t in_15[5] = { UINT64_C(0x000000000000000A), UINT64_C(0x000000000000000B), UINT64_C(0x000000000000000C), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000) };
static limb_t exp_15[5]= { UINT64_C(0x000000000000000A), UINT64_C(0x000000000000000B), UINT64_C(0x000000000000000C) };

// Generate remaining distinct test arrays programmatically via macro-like consistency
#define DEF_CASE(id, i1, i2, i3, i4, i5, e1, e2, e3, e4, e5) \
    static limb_t in_##id[5] = { UINT64_C(i1), UINT64_C(i2), UINT64_C(i3), UINT64_C(i4), UINT64_C(i5) }; \
    static limb_t exp_##id[5] = { UINT64_C(e1), UINT64_C(e2), UINT64_C(e3), UINT64_C(e4), UINT64_C(e5) };

DEF_CASE(16, 0x1, 0x0, 0x2, 0x0, 0x0, 0x1, 0x0, 0x2, 0x0, 0x0)
DEF_CASE(17, 0x1, 0x2, 0x0, 0x3, 0x0, 0x1, 0x2, 0x0, 0x3, 0x0)
DEF_CASE(18, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0)
DEF_CASE(19, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0)
DEF_CASE(20, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0)
DEF_CASE(21, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x1)
DEF_CASE(22, 0xf, 0x0, 0xf, 0x0, 0x0, 0xf, 0x0, 0xf, 0x0, 0x0)
DEF_CASE(23, 0xf, 0xf, 0x0, 0xf, 0x0, 0xf, 0xf, 0x0, 0xf, 0x0)
DEF_CASE(24, 0x123, 0x0, 0x0, 0x0, 0x0, 0x123, 0x0, 0x0, 0x0, 0x0)
DEF_CASE(25, 0x123, 0x456, 0x0, 0x0, 0x0, 0x123, 0x456, 0x0, 0x0, 0x0)
DEF_CASE(26, 0x123, 0x456, 0x789, 0x0, 0x0, 0x123, 0x456, 0x789, 0x0, 0x0)
DEF_CASE(27, 0x123, 0x456, 0x789, 0xabc, 0x0, 0x123, 0x456, 0x789, 0xabc, 0x0)
DEF_CASE(28, 0x123, 0x456, 0x789, 0xabc, 0xdef, 0x123, 0x456, 0x789, 0xabc, 0xdef)
DEF_CASE(29, 0xabc, 0x0, 0xdef, 0x0, 0x0, 0xabc, 0x0, 0xdef, 0x0, 0x0)
DEF_CASE(30, 0x99, 0x99, 0x99, 0x0, 0x0, 0x99, 0x99, 0x99, 0x0, 0x0)
DEF_CASE(31, 0x111, 0x0, 0x222, 0x0, 0x0, 0x111, 0x0, 0x222, 0x0, 0x0)
DEF_CASE(32, 0xaaa, 0xbbb, 0xccc, 0xddd, 0x0, 0xaaa, 0xbbb, 0xccc, 0xddd, 0x0)
DEF_CASE(33, 0x10, 0x20, 0x30, 0x0, 0x0, 0x10, 0x20, 0x30, 0x0, 0x0)
DEF_CASE(34, 0x7, 0x0, 0x0, 0x0, 0x0, 0x7, 0x0, 0x0, 0x0, 0x0)
DEF_CASE(35, 0x8, 0x9, 0x0, 0x0, 0x0, 0x8, 0x9, 0x0, 0x0, 0x0)
DEF_CASE(36, 0x100, 0x200, 0x300, 0x400, 0x0, 0x100, 0x200, 0x300, 0x400, 0x0)
DEF_CASE(37, 0xff, 0x0, 0x0, 0x0, 0x0, 0xff, 0x0, 0x0, 0x0, 0x0)
DEF_CASE(38, 0xee, 0xee, 0x0, 0x0, 0x0, 0xee, 0xee, 0x0, 0x0, 0x0) /* Overwritten below cleanly */
DEF_CASE(39, 0xdd, 0xdd, 0xdd, 0x0, 0x0, 0xdd, 0xdd, 0xdd, 0x0, 0x0)
DEF_CASE(40, 0xcc, 0xcc, 0xcc, 0xcc, 0x0, 0xcc, 0xcc, 0xcc, 0xcc, 0x0)
DEF_CASE(41, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb)
DEF_CASE(42, 0x11, 0x0, 0x0, 0x0, 0x0, 0x11, 0x0, 0x0, 0x0, 0x0)
DEF_CASE(43, 0x22, 0x22, 0x0, 0x0, 0x0, 0x22, 0x22, 0x0, 0x0, 0x0)
DEF_CASE(44, 0x33, 0x33, 0x33, 0x0, 0x0, 0x33, 0x33, 0x33, 0x0, 0x0)
DEF_CASE(45, 0x44, 0x44, 0x44, 0x44, 0x0, 0x44, 0x44, 0x44, 0x44, 0x0)
DEF_CASE(46, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55)
DEF_CASE(47, 0xaa, 0x0, 0x0, 0x0, 0x0, 0xaa, 0x0, 0x0, 0x0, 0x0)
DEF_CASE(48, 0xbb, 0xcc, 0x0, 0x0, 0x0, 0xbb, 0xcc, 0x0, 0x0, 0x0)
DEF_CASE(49, 0xdd, 0xee, 0xff, 0x0, 0x0, 0xdd, 0xee, 0xff, 0x0, 0x0)
DEF_CASE(50, 0x12, 0x34, 0x56, 0x78, 0x0, 0x12, 0x34, 0x56, 0x78, 0x0)
DEF_CASE(51, 0x90, 0xab, 0xcd, 0xef, 0x12, 0x90, 0xab, 0xcd, 0xef, 0x12)
DEF_CASE(52, 0x1111, 0x0, 0x0, 0x0, 0x0, 0x1111, 0x0, 0x0, 0x0, 0x0)
DEF_CASE(53, 0x2222, 0x3333, 0x0, 0x0, 0x0, 0x2222, 0x3333, 0x0, 0x0, 0x0)
DEF_CASE(54, 0x4444, 0x5444, 0x6444, 0x0, 0x0, 0x4444, 0x5444, 0x6444, 0x0, 0x0)
DEF_CASE(55, 0x7777, 0x8888, 0x9999, 0xaaaa, 0x0, 0x7777, 0x8888, 0x9999, 0xaaaa, 0x0)
DEF_CASE(56, 0xbbbb, 0xcccc, 0xdddd, 0xeeee, 0xffff, 0xbbbb, 0xcccc, 0xdddd, 0xeeee, 0xffff)
DEF_CASE(57, 0x1a, 0x0, 0x0, 0x0, 0x0, 0x1a, 0x0, 0x0, 0x0, 0x0)
DEF_CASE(58, 0x2b, 0x3c, 0x0, 0x0, 0x0, 0x2b, 0x3c, 0x0, 0x0, 0x0)
DEF_CASE(59, 0x4d, 0x5e, 0x6f, 0x0, 0x0, 0x4d, 0x5e, 0x6f, 0x0, 0x0)
DEF_CASE(60, 0x7a, 0x8b, 0xc3, 0xd4, 0x0, 0x7a, 0x8b, 0xc3, 0xd4, 0x0)
DEF_CASE(61, 0xe5, 0xf6, 0xa7, 0xb8, 0xc9, 0xe5, 0xf6, 0xa7, 0xb8, 0xc9)
DEF_CASE(62, 0x9, 0x0, 0x0, 0x0, 0x0, 0x9, 0x0, 0x0, 0x0, 0x0)
DEF_CASE(63, 0x8, 0x7, 0x0, 0x0, 0x0, 0x8, 0x7, 0x0, 0x0, 0x0)
DEF_CASE(64, 0x6, 0x5, 0x4, 0x0, 0x0, 0x6, 0x5, 0x4, 0x0, 0x0)
DEF_CASE(65, 0x3, 0x2, 0x1, 0x0, 0x0, 0x3, 0x2, 0x1, 0x0, 0x0)
DEF_CASE(66, 0xf0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0x0, 0x0, 0x0, 0x0)
DEF_CASE(67, 0xe0, 0xd0, 0x0, 0x0, 0x0, 0xe0, 0xd0, 0x0, 0x0, 0x0)
DEF_CASE(68, 0xc0, 0xb0, 0xa0, 0x0, 0x0, 0xc0, 0xb0, 0xa0, 0x0, 0x0)
DEF_CASE(69, 0x90, 0x80, 0x70, 0x60, 0x0, 0x90, 0x80, 0x70, 0x60, 0x0)
DEF_CASE(70, 0x50, 0x40, 0x30, 0x20, 0x10, 0x50, 0x40, 0x30, 0x20, 0x10)
DEF_CASE(71, 0x135, 0x0, 0x0, 0x0, 0x0, 0x135, 0x0, 0x0, 0x0, 0x0)
DEF_CASE(72, 0x246, 0x0, 0x0, 0x0, 0x0, 0x246, 0x0, 0x0, 0x0, 0x0)
DEF_CASE(73, 0x789, 0x0, 0x0, 0x0, 0x0, 0x789, 0x0, 0x0, 0x0, 0x0)
DEF_CASE(74, 0xabc, 0x0, 0x0, 0x0, 0x0, 0xabc, 0x0, 0x0, 0x0, 0x0)
DEF_CASE(75, 0xdef, 0x0, 0x0, 0x0, 0x0, 0xdef, 0x0, 0x0, 0x0, 0x0)
DEF_CASE(76, 0x11, 0x22, 0x0, 0x0, 0x0, 0x11, 0x22, 0x0, 0x0, 0x0)
DEF_CASE(77, 0x33, 0x44, 0x0, 0x0, 0x0, 0x33, 0x44, 0x0, 0x0, 0x0)
DEF_CASE(78, 0x55, 0x66, 0x0, 0x0, 0x0, 0x55, 0x66, 0x0, 0x0, 0x0)
DEF_CASE(79, 0x77, 0x88, 0x0, 0x0, 0x0, 0x77, 0x88, 0x0, 0x0, 0x0)

/* Global Array of Unit-tested cases */
static const case_t global_bank[CASE_CNT] = {
    { { 1, 5, in_0,  1, false }, { 0, 5, exp_0,  1, false } },
    { { 1, 5, in_1,  1, false }, { 1, 5, exp_1,  1, false } },
    { { 2, 5, in_2,  1, false }, { 1, 5, exp_2,  1, false } },
    { { 3, 5, in_3,  1, false }, { 1, 5, exp_3,  1, false } },
    { { 4, 5, in_4,  1, false }, { 1, 5, exp_4,  1, false } },
    { { 5, 5, in_5,  1, false }, { 1, 5, exp_5,  1, false } },
    { { 2, 5, in_6,  1, false }, { 0, 5, exp_6,  1, false } },
    { { 5, 5, in_7,  1, false }, { 0, 5, exp_7,  1, false } },
    { { 2, 5, in_8,  1, false }, { 1, 5, exp_8,  1, false } },
    { { 3, 5, in_9,  1, false }, { 2, 5, exp_9,  1, false } },
    { { 3, 5, in_10, 1, false }, { 2, 5, exp_10, 1, false } },
    { { 4, 5, in_11, 1, false }, { 3, 5, exp_11, 1, false } },
    { { 5, 5, in_12, 1, false }, { 4, 5, exp_12, 1, false } },
    { { 3, 5, in_13, 1, false }, { 1, 5, exp_13, 1, false } },
    { { 4, 5, in_14, 1, false }, { 2, 5, exp_14, 1, false } },
    { { 5, 5, in_15, 1, false }, { 3, 5, exp_15, 1, false } },
    { { 5, 5, in_16, 1, false }, { 3, 5, exp_16, 1, false } },
    { { 5, 5, in_17, 1, false }, { 4, 5, exp_17, 1, false } },
    { { 5, 5, in_18, 1, false }, { 2, 5, exp_18, 1, false } },
    { { 5, 5, in_19, 1, false }, { 3, 5, exp_19, 1, false } },
    { { 5, 5, in_20, 1, false }, { 4, 5, exp_20, 1, false } },
    { { 5, 5, in_21, 1, false }, { 5, 5, exp_21, 1, false } },
    { { 5, 5, in_22, 1, false }, { 3, 5, exp_22, 1, false } },
    { { 5, 5, in_23, 1, false }, { 4, 5, exp_23, 1, false } },
    { { 5, 5, in_24, 1, false }, { 1, 5, exp_24, 1, false } },
    { { 5, 5, in_25, 1, false }, { 2, 5, exp_25, 1, false } },
    { { 5, 5, in_26, 1, false }, { 3, 5, exp_26, 1, false } },
    { { 5, 5, in_27, 1, false }, { 4, 5, exp_27, 1, false } },
    { { 5, 5, in_28, 1, false }, { 5, 5, exp_28, 1, false } },
    { { 5, 5, in_29, 1, false }, { 3, 5, exp_29, 1, false } },
    { { 5, 5, in_30, 1, false }, { 3, 5, exp_30, 1, false } },
    { { 5, 5, in_31, 1, false }, { 3, 5, exp_31, 1, false } },
    { { 5, 5, in_32, 1, false }, { 4, 5, exp_32, 1, false } },
    { { 5, 5, in_33, 1, false }, { 3, 5, exp_33, 1, false } },
    { { 5, 5, in_34, 1, false }, { 1, 5, exp_34, 1, false } },
    { { 5, 5, in_35, 1, false }, { 2, 5, exp_35, 1, false } },
    { { 5, 5, in_36, 1, false }, { 4, 5, exp_36, 1, false } },
    { { 5, 5, in_37, 1, false }, { 1, 5, exp_37, 1, false } },
    { { 5, 5, in_38, 1, false }, { 2, 5, exp_38, 1, false } },
    { { 5, 5, in_39, 1, false }, { 3, 5, exp_39, 1, false } },
    { { 5, 5, in_40, 1, false }, { 4, 5, exp_40, 1, false } },
    { { 5, 5, in_41, 1, false }, { 5, 5, exp_41, 1, false } },
    { { 5, 5, in_42, 1, false }, { 1, 5, exp_42, 1, false } },
    { { 5, 5, in_43, 1, false }, { 2, 5, exp_43, 1, false } },
    { { 5, 5, in_44, 1, false }, { 3, 5, exp_44, 1, false } },
    { { 5, 5, in_45, 1, false }, { 4, 5, exp_45, 1, false } },
    { { 5, 5, in_46, 1, false }, { 5, 5, exp_46, 1, false } },
    { { 5, 5, in_47, 1, false }, { 1, 5, exp_47, 1, false } },
    { { 5, 5, in_48, 1, false }, { 2, 5, exp_48, 1, false } },
    { { 5, 5, in_49, 1, false }, { 3, 5, exp_49, 1, false } },
    { { 5, 5, in_50, 1, false }, { 4, 5, exp_50, 1, false } },
    { { 5, 5, in_51, 1, false }, { 5, 5, exp_51, 1, false } },
    { { 5, 5, in_52, 1, false }, { 1, 5, exp_52, 1, false } },
    { { 5, 5, in_53, 1, false }, { 2, 5, exp_53, 1, false } },
    { { 5, 5, in_54, 1, false }, { 3, 5, exp_54, 1, false } },
    { { 5, 5, in_55, 1, false }, { 4, 5, exp_55, 1, false } },
    { { 5, 5, in_56, 1, false }, { 5, 5, exp_56, 1, false } },
    { { 5, 5, in_57, 1, false }, { 1, 5, exp_57, 1, false } },
    { { 5, 5, in_58, 1, false }, { 2, 5, exp_58, 1, false } },
    { { 5, 5, in_59, 1, false }, { 3, 5, exp_59, 1, false } },
    { { 5, 5, in_60, 1, false }, { 4, 5, exp_60, 1, false } },
    { { 5, 5, in_61, 1, false }, { 5, 5, exp_61, 1, false } },
    { { 5, 5, in_62, 1, false }, { 1, 5, exp_62, 1, false } },
    { { 5, 5, in_63, 1, false }, { 2, 5, exp_63, 1, false } },
    { { 5, 5, in_64, 1, false }, { 3, 5, exp_64, 1, false } },
    { { 5, 5, in_65, 1, false }, { 3, 5, exp_65, 1, false } },
    { { 5, 5, in_66, 1, false }, { 1, 5, exp_66, 1, false } },
    { { 5, 5, in_67, 1, false }, { 2, 5, exp_67, 1, false } },
    { { 5, 5, in_68, 1, false }, { 3, 5, exp_68, 1, false } },
    { { 5, 5, in_69, 1, false }, { 4, 5, exp_69, 1, false } },
    { { 5, 5, in_70, 1, false }, { 5, 5, exp_70, 1, false } },
    { { 5, 5, in_71, 1, false }, { 1, 5, exp_71, 1, false } },
    { { 5, 5, in_72, 1, false }, { 1, 5, exp_72, 1, false } },
    { { 5, 5, in_73, 1, false }, { 1, 5, exp_73, 1, false } },
    { { 5, 5, in_74, 1, false }, { 1, 5, exp_74, 1, false } },
    { { 5, 5, in_75, 1, false }, { 1, 5, exp_75, 1, false } },
    { { 5, 5, in_76, 1, false }, { 2, 5, exp_76, 1, false } },
    { { 5, 5, in_77, 1, false }, { 2, 5, exp_77, 1, false } },
    { { 5, 5, in_78, 1, false }, { 2, 5, exp_78, 1, false } },
    { { 5, 5, in_79, 1, false }, { 2, 5, exp_79, 1, false } }
};

int main(void) { _libdnml_init();
    int total_tests = 0, passed_tests = 0; struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start); bool match = false;
    limb_t *res_buf = (limb_t *)malloc(MAX_SIZE_T * sizeof(limb_t)); assert(res_buf != NULL);
    fputs("===================================================================\n", stdout);
    fputs("        RUNNING INTEGRATED UNIT TESTS - CRYPTNUM CORRECTIONS.      \n", stdout);
    fputs("===================================================================\n", stdout);
    fputs("---- __CRINT_INTERNAL_CMP__ -----\n", stdout);
    for (int i = 0; i < CASE_CNT; i++) { total_tests++; match = false;
        const case_t *curr_case = &global_bank[i];
        memset(res_buf, 0, MAX_SIZE_T * sizeof(limb_t));
        /* Setup local bigInts with heap-backed memory */
        crint res = curr_case->in; res.limbs = res_buf;  res.cap = MAX_SIZE_T;
        if (curr_case->in.n > 0) {
            memcpy(res_buf, curr_case->in.limbs, curr_case->in.n * sizeof(limb_t));
        }
        __CRINT_TRIM_LZ__(&res);
        if (res.n == curr_case->exp.n) {
            if (res.n == 0) match = true;
            else match = (memcmp(res.limbs, curr_case->exp.limbs, curr_case->exp.n * sizeof(limb_t)) == 0);
        }
        if (match) passed_tests++;
        else printf(
            "Case: %2d | res = { .n = %zu, .sign = %2d } | exp = { .n = %zu, .sign = %2d }\n",
            i, res.n, curr_case->in.sign, curr_case->exp.n, curr_case->exp.sign
        );
    }

    #undef CASE_CNT
    #undef MAX_SIZE_T
    free(res_buf); clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    fputs( "=========================================================\n", stdout);
    fputs( "TEST SUMMARY:\n", stdout);
    printf("+) Passed %-4d out of %-4d total compiled checks.\n", passed_tests, total_tests);
    printf("+) Success rate: %.2f%%\n", (passed_tests * 100.0) / total_tests);
    printf("+) Total Runtime: %lf ms\n", elapsed_time * 1000.0);
    fputs( "=========================================================\n", stdout);

    _libdnml_cleanup(); 
    return (passed_tests == total_tests) ? 0 : 1;
}
