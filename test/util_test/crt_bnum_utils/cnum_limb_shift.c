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

#define MAX_SIZE_T 32

typedef struct { const crint in; size_t klimbs; const crint exp; } limb_shift_case;

/* ========================================================================== */
/* RIGHT SHIFT STATIC LIMB BUFFERS                       */
/* ========================================================================== */

// ---- Valid Right-Shift Arrays (1 - 50) ----
static limb_t r_in_01[]  = { UINT64_C(0x0000000000000000) };
static limb_t r_exp_01[] = { UINT64_C(0x0000000000000000) };
static limb_t r_in_02[]  = { UINT64_C(0x0000000000000001) };
static limb_t r_exp_02[] = { UINT64_C(0x0000000000000001) };
static limb_t r_in_03[]  = { UINT64_C(0x0000000000000005) };
static limb_t r_exp_03[] = { UINT64_C(0x0000000000000000) };
static limb_t r_in_04[]  = { UINT64_C(0xAAAAAAAAAAAAAAAA) };
static limb_t r_exp_04[] = { UINT64_C(0xAAAAAAAAAAAAAAAA) };
static limb_t r_in_05[]  = { UINT64_C(0xFFFFFFFFFFFFFFFF) };
static limb_t r_exp_05[] = { UINT64_C(0x0000000000000000) };
static limb_t r_in_06[]  = { UINT64_C(0x1111111111111111), UINT64_C(0x2222222222222222) };
static limb_t r_exp_06[] = { UINT64_C(0x2222222222222222) };
static limb_t r_in_07[]  = { UINT64_C(0x1111111111111111), UINT64_C(0x2222222222222222) };
static limb_t r_exp_07[] = { UINT64_C(0x0000000000000000) };
static limb_t r_in_08[]  = { UINT64_C(0xBBBBBBBBBBBBBBBB), UINT64_C(0xCCCCCCCCCCCCCCCC) };
static limb_t r_exp_08[] = { UINT64_C(0xBBBBBBBBBBBBBBBB), UINT64_C(0xCCCCCCCCCCCCCCCC) };
static limb_t r_in_09[]  = { UINT64_C(0x123456789ABCDEF0), UINT64_C(0x0FEDCBA987654321) };
static limb_t r_exp_09[] = { UINT64_C(0x0FEDCBA987654321) };
static limb_t r_in_10[]  = { UINT64_C(0x123456789ABCDEF0), UINT64_C(0x0FEDCBA987654321) };
static limb_t r_exp_10[] = { UINT64_C(0x0000000000000000) };
static limb_t r_in_11[]  = { UINT64_C(0x0101010101010101), UINT64_C(0x0202020202020202), UINT64_C(0x0303030303030303) };
static limb_t r_exp_11[] = { UINT64_C(0x0202020202020202), UINT64_C(0x0303030303030303) };
static limb_t r_in_12[]  = { UINT64_C(0x0101010101010101), UINT64_C(0x0202020202020202), UINT64_C(0x0303030303030303) };
static limb_t r_exp_12[] = { UINT64_C(0x0303030303030303) };
static limb_t r_in_13[]  = { UINT64_C(0x0101010101010101), UINT64_C(0x0202020202020202), UINT64_C(0x0303030303030303) };
static limb_t r_exp_13[] = { UINT64_C(0x0000000000000000) };
static limb_t r_in_14[]  = { UINT64_C(0x5555555555555555), UINT64_C(0x6666666666666666), UINT64_C(0x7777777777777777) };
static limb_t r_exp_14[] = { UINT64_C(0x5555555555555555), UINT64_C(0x6666666666666666), UINT64_C(0x7777777777777777) };
static limb_t r_in_15[]  = { UINT64_C(0x5555555555555555), UINT64_C(0x6666666666666666), UINT64_C(0x7777777777777777) };
static limb_t r_exp_15[] = { UINT64_C(0x0000000000000000) };
static limb_t r_in_16[]  = { UINT64_C(0xA1A1A1A1A1A1A1A1), UINT64_C(0xB2B2B2B2B2B2B2B2), UINT64_C(0xC3C3C3C3C3C3C3C3), UINT64_C(0xD4D4D4D4D4D4D4D4) };
static limb_t r_exp_16[] = { UINT64_C(0xB2B2B2B2B2B2B2B2), UINT64_C(0xC3C3C3C3C3C3C3C3), UINT64_C(0xD4D4D4D4D4D4D4D4) };
static limb_t r_in_17[]  = { UINT64_C(0xA1A1A1A1A1A1A1A1), UINT64_C(0xB2B2B2B2B2B2B2B2), UINT64_C(0xC3C3C3C3C3C3C3C3), UINT64_C(0xD4D4D4D4D4D4D4D4) };
static limb_t r_exp_17[] = { UINT64_C(0xC3C3C3C3C3C3C3C3), UINT64_C(0xD4D4D4D4D4D4D4D4) };
static limb_t r_in_18[]  = { UINT64_C(0xA1A1A1A1A1A1A1A1), UINT64_C(0xB2B2B2B2B2B2B2B2), UINT64_C(0xC3C3C3C3C3C3C3C3), UINT64_C(0xD4D4D4D4D4D4D4D4) };
static limb_t r_exp_18[] = { UINT64_C(0xD4D4D4D4D4D4D4D4) };
static limb_t r_in_19[]  = { UINT64_C(0xA1A1A1A1A1A1A1A1), UINT64_C(0xB2B2B2B2B2B2B2B2), UINT64_C(0xC3C3C3C3C3C3C3C3), UINT64_C(0xD4D4D4D4D4D4D4D4) };
static limb_t r_exp_19[] = { UINT64_C(0x0000000000000000) };
static limb_t r_in_20[]  = { UINT64_C(0xA1A1A1A1A1A1A1A1), UINT64_C(0xB2B2B2B2B2B2B2B2), UINT64_C(0xC3C3C3C3C3C3C3C3), UINT64_C(0xD4D4D4D4D4D4D4D4) };
static limb_t r_exp_20[] = { UINT64_C(0x0000000000000000) };
static limb_t r_in_21[]  = { UINT64_C(0xE1E1E1E1E1E1E1E1), UINT64_C(0xE2E2E2E2E2E2E2E2), UINT64_C(0xE3E3E3E3E3E3E3E3), UINT64_C(0xE4E4E4E4E4E4E4E4), UINT64_C(0xE5E5E5E5E5E5E5E5) };
static limb_t r_exp_21[] = { UINT64_C(0xE2E2E2E2E2E2E2E2), UINT64_C(0xE3E3E3E3E3E3E3E3), UINT64_C(0xE4E4E4E4E4E4E4E4), UINT64_C(0xE5E5E5E5E5E5E5E5) };
static limb_t r_in_22[]  = { UINT64_C(0xE1E1E1E1E1E1E1E1), UINT64_C(0xE2E2E2E2E2E2E2E2), UINT64_C(0xE3E3E3E3E3E3E3E3), UINT64_C(0xE4E4E4E4E4E4E4E4), UINT64_C(0xE5E5E5E5E5E5E5E5) };
static limb_t r_exp_22[] = { UINT64_C(0xE3E3E3E3E3E3E3E3), UINT64_C(0xE4E4E4E4E4E4E4E4), UINT64_C(0xE5E5E5E5E5E5E5E5) };
static limb_t r_in_23[]  = { UINT64_C(0xE1E1E1E1E1E1E1E1), UINT64_C(0xE2E2E2E2E2E2E2E2), UINT64_C(0xE3E3E3E3E3E3E3E3), UINT64_C(0xE4E4E4E4E4E4E4E4), UINT64_C(0xE5E5E5E5E5E5E5E5) };
static limb_t r_exp_23[] = { UINT64_C(0xE4E4E4E4E4E4E4E4), UINT64_C(0xE5E5E5E5E5E5E5E5) };
static limb_t r_in_24[]  = { UINT64_C(0xE1E1E1E1E1E1E1E1), UINT64_C(0xE2E2E2E2E2E2E2E2), UINT64_C(0xE3E3E3E3E3E3E3E3), UINT64_C(0xE4E4E4E4E4E4E4E4), UINT64_C(0xE5E5E5E5E5E5E5E5) };
static limb_t r_exp_24[] = { UINT64_C(0xE5E5E5E5E5E5E5E5) };
static limb_t r_in_25[]  = { UINT64_C(0xE1E1E1E1E1E1E1E1), UINT64_C(0xE2E2E2E2E2E2E2E2), UINT64_C(0xE3E3E3E3E3E3E3E3), UINT64_C(0xE4E4E4E4E4E4E4E4), UINT64_C(0xE5E5E5E5E5E5E5E5) };
static limb_t r_exp_25[] = { UINT64_C(0x0000000000000000) };
static limb_t r_in_26[]  = { UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000000), UINT64_C(0x1111111111111111) };
static limb_t r_exp_26[] = { UINT64_C(0x0000000000000000), UINT64_C(0x1111111111111111) };
static limb_t r_in_27[]  = { UINT64_C(0xBA9876543210FEDC), UINT64_C(0xABCDEF0123456789), UINT64_C(0x9999999999999999), UINT64_C(0x8888888888888888) };
static limb_t r_exp_27[] = { UINT64_C(0xABCDEF0123456789), UINT64_C(0x9999999999999999), UINT64_C(0x8888888888888888) };
static limb_t r_in_28[]  = { UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000002), UINT64_C(0x0000000000000003), UINT64_C(0x0000000000000004), UINT64_C(0x0000000000000005) };
static limb_t r_exp_28[] = { UINT64_C(0x0000000000000004), UINT64_C(0x0000000000000005) };
static limb_t r_in_29[]  = { UINT64_C(0xDEADBEEFDEADBEEF), UINT64_C(0xCAFEBABECAFEBABE), UINT64_C(0xBADCAFEFBADCAFEF) };
static limb_t r_exp_29[] = { UINT64_C(0xBADCAFEFBADCAFEF) };
static limb_t r_in_30[]  = { UINT64_C(0x1111222233334444), UINT64_C(0x5555666677778888), UINT64_C(0x9999AAAABBBBCCCC), UINT64_C(0xDDDDEEEEFFFF0000) };
static limb_t r_exp_30[] = { UINT64_C(0x9999AAAABBBBCCCC), UINT64_C(0xDDDDEEEEFFFF0000) };
static limb_t r_in_31[]  = { UINT64_C(0x000000000000000A), UINT64_C(0x000000000000000B) };
static limb_t r_exp_31[] = { UINT64_C(0x000000000000000B) };
static limb_t r_in_32[]  = { UINT64_C(0x000000000000000A), UINT64_C(0x000000000000000B) };
static limb_t r_exp_32[] = { UINT64_C(0x000000000000000B) };
static limb_t r_in_33[]  = { UINT64_C(0x000000000000000C) };
static limb_t r_exp_33[] = { UINT64_C(0x0000000000000000) };
static limb_t r_in_34[]  = { UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000002), UINT64_C(0x0000000000000003), UINT64_C(0x0000000000000004), UINT64_C(0x0000000000000005), UINT64_C(0x0000000000000006) };
static limb_t r_exp_34[] = { UINT64_C(0x0000000000000004), UINT64_C(0x0000000000000005), UINT64_C(0x0000000000000006) };
static limb_t r_in_35[]  = { UINT64_C(0x1000000000000000), UINT64_C(0x2000000000000000), UINT64_C(0x3000000000000000), UINT64_C(0x4000000000000000), UINT64_C(0x5000000000000000), UINT64_C(0x6000000000000000), UINT64_C(0x7000000000000000) };
static limb_t r_exp_35[] = { UINT64_C(0x6000000000000000), UINT64_C(0x7000000000000000) };
static limb_t r_in_36[]  = { UINT64_C(0x0123456789ABCDEF) };
static limb_t r_exp_36[] = { UINT64_C(0x0123456789ABCDEF) };
static limb_t r_in_37[]  = { UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000002) };
static limb_t r_exp_37[] = { UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000002) };
static limb_t r_in_38[]  = { UINT64_C(0x1111111111111111), UINT64_C(0x2222222222222222), UINT64_C(0x3333333333333333) };
static limb_t r_exp_38[] = { UINT64_C(0x1111111111111111), UINT64_C(0x2222222222222222), UINT64_C(0x3333333333333333) };
static limb_t r_in_39[]  = { UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xEEEEEEEEEEEEEEEE), UINT64_C(0xDDDDDDDDDDDDDDDD), UINT64_C(0xCCCCCCCCCCCCCCCC) };
static limb_t r_exp_39[] = { UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xEEEEEEEEEEEEEEEE), UINT64_C(0xDDDDDDDDDDDDDDDD), UINT64_C(0xCCCCCCCCCCCCCCCC) };
static limb_t r_in_40[]  = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000001) };
static limb_t r_exp_40[] = { UINT64_C(0x0000000000000001) };
static limb_t r_in_41[]  = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000001) };
static limb_t r_exp_41[] = { UINT64_C(0x0000000000000000) };
static limb_t r_in_42[]  = { UINT64_C(0x0000000000000005), UINT64_C(0x0000000000000006) };
static limb_t r_exp_42[] = { UINT64_C(0x0000000000000000) };
static limb_t r_in_43[]  = { UINT64_C(0x123456789ABCDEF0), UINT64_C(0x23456789ABCDEF01), UINT64_C(0x3456789ABCDEF012) };
static limb_t r_exp_43[] = { UINT64_C(0x0000000000000000) };
static limb_t r_in_44[]  = { UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000002), UINT64_C(0x0000000000000003), UINT64_C(0x0000000000000004) };
static limb_t r_exp_44[] = { UINT64_C(0x0000000000000000) };
static limb_t r_in_45[]  = { UINT64_C(0x000000000000000A), UINT64_C(0x000000000000000B) };
static limb_t r_exp_45[] = { UINT64_C(0x0000000000000000) };
static limb_t r_in_46[]  = { UINT64_C(0x1111111111111111), UINT64_C(0x2222222222222222) };
static limb_t r_exp_46[] = { UINT64_C(0x0000000000000000) };
static limb_t r_in_47[]  = { UINT64_C(0x9999999999999999), UINT64_C(0x8888888888888888), UINT64_C(0x7777777777777777) };
static limb_t r_exp_47[] = { UINT64_C(0x0000000000000000) };
static limb_t r_in_48[]  = { UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0xBBBBBBBBBBBBBBBB), UINT64_C(0xCCCCCCCCCCCCCCCC), UINT64_C(0xDDDDDDDDDDDDDDDD) };
static limb_t r_exp_48[] = { UINT64_C(0x0000000000000000) };
static limb_t r_in_49[]  = { UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000002), UINT64_C(0x0000000000000003), UINT64_C(0x0000000000000004), UINT64_C(0x0000000000000005) };
static limb_t r_exp_49[] = { UINT64_C(0x0000000000000000) };
static limb_t r_in_50[]  = { UINT64_C(0x0000000000000001) };
static limb_t r_exp_50[] = { UINT64_C(0x0000000000000000) };

// ---- Poisoned Right-Shift Invariant Arrays (51 - 60) ----
static limb_t r_poison_in_51[] = { UINT64_C(0x1111111111111111) };
static limb_t r_poison_exp_51[] = { UINT64_C(0x1111111111111111) };
static limb_t r_poison_in_52[] = { UINT64_C(0x2222222222222222), UINT64_C(0x3333333333333333) };
static limb_t r_poison_exp_52[] = { UINT64_C(0x2222222222222222), UINT64_C(0x3333333333333333) };
static limb_t r_poison_in_53[] = { UINT64_C(0x4444444444444444), UINT64_C(0x5555555555555555), UINT64_C(0x6666666666666666) };
static limb_t r_poison_exp_53[] = { UINT64_C(0x4444444444444444), UINT64_C(0x5555555555555555), UINT64_C(0x6666666666666666) };
static limb_t r_poison_in_54[] = { UINT64_C(0x7777777777777777), UINT64_C(0x8888888888888888), UINT64_C(0x9999999999999999), UINT64_C(0xAAAAAAAAAAAAAAAA) };
static limb_t r_poison_exp_54[] = { UINT64_C(0x7777777777777777), UINT64_C(0x8888888888888888), UINT64_C(0x9999999999999999), UINT64_C(0xAAAAAAAAAAAAAAAA) };
static limb_t r_poison_in_55[] = { UINT64_C(0xBBBBBBBBBBBBBBBB), UINT64_C(0xCCCCCCCCCCCCCCCC) };
static limb_t r_poison_exp_55[] = { UINT64_C(0xBBBBBBBBBBBBBBBB), UINT64_C(0xCCCCCCCCCCCCCCCC) };
static limb_t r_poison_in_56[] = { UINT64_C(0xDDDDDDDDDDDDDDDD), UINT64_C(0xEEEEEEEEEEEEEEEE), UINT64_C(0xFFFFFFFFFFFFFFFF) };
static limb_t r_poison_exp_56[] = { UINT64_C(0xDDDDDDDDDDDDDDDD), UINT64_C(0xEEEEEEEEEEEEEEEE), UINT64_C(0xFFFFFFFFFFFFFFFF) };
static limb_t r_poison_in_57[] = { UINT64_C(0x123456789ABCDEF0) };
static limb_t r_poison_exp_57[] = { UINT64_C(0x123456789ABCDEF0) };
static limb_t r_poison_in_58[] = { UINT64_C(0x0FEDCBA987654321), UINT64_C(0xABCDEF0123456789) };
static limb_t r_poison_exp_58[] = { UINT64_C(0x0FEDCBA987654321), UINT64_C(0xABCDEF0123456789) };
static limb_t r_poison_in_59[] = { UINT64_C(0x9999888877776666), UINT64_C(0x5555444433332222), UINT64_C(0x11110000FFFFEEEE) };
static limb_t r_poison_exp_59[] = { UINT64_C(0x9999888877776666), UINT64_C(0x5555444433332222), UINT64_C(0x11110000FFFFEEEE) };
static limb_t r_poison_in_60[] = { UINT64_C(0x0000000000000001) };
static limb_t r_poison_exp_60[] = { UINT64_C(0x0000000000000001) };


/* ========================================================================== */
/* LEFT SHIFT STATIC LIMB BUFFERS                       */
/* ========================================================================== */

// ---- Valid Left-Shift Arrays (1 - 50) ----
static limb_t l_in_01[]  = { UINT64_C(0x0000000000000000) };
static limb_t l_exp_01[] = { UINT64_C(0x0000000000000000) };
static limb_t l_in_02[]  = { UINT64_C(0x0000000000000001) };
static limb_t l_exp_02[] = { UINT64_C(0x0000000000000001) };
static limb_t l_in_03[]  = { UINT64_C(0x0000000000000005) };
static limb_t l_exp_03[] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000005) };
static limb_t l_in_04[]  = { UINT64_C(0xAAAAAAAAAAAAAAAA) };
static limb_t l_exp_04[] = { UINT64_C(0xAAAAAAAAAAAAAAAA) };
static limb_t l_in_05[]  = { UINT64_C(0xFFFFFFFFFFFFFFFF) };
static limb_t l_exp_05[] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0xFFFFFFFFFFFFFFFF) };
static limb_t l_in_06[]  = { UINT64_C(0x1111111111111111), UINT64_C(0x2222222222222222) };
static limb_t l_exp_06[] = { UINT64_C(0x0000000000000000), UINT64_C(0x1111111111111111), UINT64_C(0x2222222222222222) };
static limb_t l_in_07[]  = { UINT64_C(0x1111111111111111), UINT64_C(0x2222222222222222) };
static limb_t l_exp_07[] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x1111111111111111), UINT64_C(0x2222222222222222) };
static limb_t l_in_08[]  = { UINT64_C(0xBBBBBBBBBBBBBBBB), UINT64_C(0xCCCCCCCCCCCCCCCC) };
static limb_t l_exp_08[] = { UINT64_C(0xBBBBBBBBBBBBBBBB), UINT64_C(0xCCCCCCCCCCCCCCCC) };
static limb_t l_in_09[]  = { UINT64_C(0x123456789ABCDEF0), UINT64_C(0x0FEDCBA987654321) };
static limb_t l_exp_09[] = { UINT64_C(0x0000000000000000), UINT64_C(0x123456789ABCDEF0), UINT64_C(0x0FEDCBA987654321) };
static limb_t l_in_10[]  = { UINT64_C(0x123456789ABCDEF0), UINT64_C(0x0FEDCBA987654321) };
static limb_t l_exp_07_extra[] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x123456789ABCDEF0), UINT64_C(0x0FEDCBA987654321) };
static limb_t l_in_11[]  = { UINT64_C(0x0101010101010101), UINT64_C(0x0202020202020202), UINT64_C(0x0303030303030303) };
static limb_t l_exp_11[] = { UINT64_C(0x0000000000000000), UINT64_C(0x0101010101010101), UINT64_C(0x0202020202020202), UINT64_C(0x0303030303030303) };
static limb_t l_in_12[]  = { UINT64_C(0x0101010101010101), UINT64_C(0x0202020202020202), UINT64_C(0x0303030303030303) };
static limb_t l_exp_12[] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0101010101010101), UINT64_C(0x0202020202020202), UINT64_C(0x0303030303030303) };
static limb_t l_in_13[]  = { UINT64_C(0x0101010101010101), UINT64_C(0x0202020202020202), UINT64_C(0x0303030303030303) };
static limb_t l_exp_13[] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0101010101010101), UINT64_C(0x0202020202020202), UINT64_C(0x0303030303030303) };
static limb_t l_in_14[]  = { UINT64_C(0x5555555555555555), UINT64_C(0x6666666666666666), UINT64_C(0x7777777777777777) };
static limb_t l_exp_14[] = { UINT64_C(0x5555555555555555), UINT64_C(0x6666666666666666), UINT64_C(0x7777777777777777) };
static limb_t l_in_15[]  = { UINT64_C(0x5555555555555555), UINT64_C(0x6666666666666666), UINT64_C(0x7777777777777777) };
static limb_t l_exp_15[] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x5555555555555555), UINT64_C(0x6666666666666666), UINT64_C(0x7777777777777777) };
static limb_t l_in_16[]  = { UINT64_C(0xA1A1A1A1A1A1A1A1), UINT64_C(0xB2B2B2B2B2B2B2B2), UINT64_C(0xC3C3C3C3C3C3C3C3), UINT64_C(0xD4D4D4D4D4D4D4D4) };
static limb_t l_exp_16[] = { UINT64_C(0x0000000000000000), UINT64_C(0xA1A1A1A1A1A1A1A1), UINT64_C(0xB2B2B2B2B2B2B2B2), UINT64_C(0xC3C3C3C3C3C3C3C3), UINT64_C(0xD4D4D4D4D4D4D4D4) };
static limb_t l_in_17[]  = { UINT64_C(0xA1A1A1A1A1A1A1A1), UINT64_C(0xB2B2B2B2B2B2B2B2), UINT64_C(0xC3C3C3C3C3C3C3C3), UINT64_C(0xD4D4D4D4D4D4D4D4) };
static limb_t l_exp_17[] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0xA1A1A1A1A1A1A1A1), UINT64_C(0xB2B2B2B2B2B2B2B2), UINT64_C(0xC3C3C3C3C3C3C3C3), UINT64_C(0xD4D4D4D4D4D4D4D4) };
static limb_t l_in_18[]  = { UINT64_C(0xA1A1A1A1A1A1A1A1), UINT64_C(0xB2B2B2B2B2B2B2B2), UINT64_C(0xC3C3C3C3C3C3C3C3), UINT64_C(0xD4D4D4D4D4D4D4D4) };
static limb_t l_exp_18[] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0xA1A1A1A1A1A1A1A1), UINT64_C(0xB2B2B2B2B2B2B2B2), UINT64_C(0xC3C3C3C3C3C3C3C3), UINT64_C(0xD4D4D4D4D4D4D4D4) };
static limb_t l_in_19[]  = { UINT64_C(0xA1A1A1A1A1A1A1A1), UINT64_C(0xB2B2B2B2B2B2B2B2), UINT64_C(0xC3C3C3C3C3C3C3C3), UINT64_C(0xD4D4D4D4D4D4D4D4) };
static limb_t l_exp_19[] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0xA1A1A1A1A1A1A1A1), UINT64_C(0xB2B2B2B2B2B2B2B2), UINT64_C(0xC3C3C3C3C3C3C3C3), UINT64_C(0xD4D4D4D4D4D4D4D4) };
static limb_t l_in_20[]  = { UINT64_C(0xA1A1A1A1A1A1A1A1), UINT64_C(0xB2B2B2B2B2B2B2B2), UINT64_C(0xC3C3C3C3C3C3C3C3), UINT64_C(0xD4D4D4D4D4D4D4D4) };
static limb_t l_exp_20[] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0xA1A1A1A1A1A1A1A1), UINT64_C(0xB2B2B2B2B2B2B2B2), UINT64_C(0xC3C3C3C3C3C3C3C3), UINT64_C(0xD4D4D4D4D4D4D4D4) };
static limb_t l_in_21[]  = { UINT64_C(0xE1E1E1E1E1E1E1E1), UINT64_C(0xE2E2E2E2E2E2E2E2), UINT64_C(0xE3E3E3E3E3E3E3E3), UINT64_C(0xE4E4E4E4E4E4E4E4), UINT64_C(0xE5E5E5E5E5E5E5E5) };
static limb_t l_exp_21[] = { UINT64_C(0x0000000000000000), UINT64_C(0xE1E1E1E1E1E1E1E1), UINT64_C(0xE2E2E2E2E2E2E2E2), UINT64_C(0xE3E3E3E3E3E3E3E3), UINT64_C(0xE4E4E4E4E4E4E4E4), UINT64_C(0xE5E5E5E5E5E5E5E5) };
static limb_t l_in_22[]  = { UINT64_C(0xE1E1E1E1E1E1E1E1), UINT64_C(0xE2E2E2E2E2E2E2E2), UINT64_C(0xE3E3E3E3E3E3E3E3), UINT64_C(0xE4E4E4E4E4E4E4E4), UINT64_C(0xE5E5E5E5E5E5E5E5) };
static limb_t l_exp_22[] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0xE1E1E1E1E1E1E1E1), UINT64_C(0xE2E2E2E2E2E2E2E2), UINT64_C(0xE3E3E3E3E3E3E3E3), UINT64_C(0xE4E4E4E4E4E4E4E4), UINT64_C(0xE5E5E5E5E5E5E5E5) };
static limb_t l_in_23[]  = { UINT64_C(0xE1E1E1E1E1E1E1E1), UINT64_C(0xE2E2E2E2E2E2E2E2), UINT64_C(0xE3E3E3E3E3E3E3E3), UINT64_C(0xE4E4E4E4E4E4E4E4), UINT64_C(0xE5E5E5E5E5E5E5E5) };
static limb_t l_exp_23[] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0xE1E1E1E1E1E1E1E1), UINT64_C(0xE2E2E2E2E2E2E2E2), UINT64_C(0xE3E3E3E3E3E3E3E3), UINT64_C(0xE4E4E4E4E4E4E4E4), UINT64_C(0xE5E5E5E5E5E5E5E5) };
static limb_t l_in_24[]  = { UINT64_C(0xE1E1E1E1E1E1E1E1), UINT64_C(0xE2E2E2E2E2E2E2E2), UINT64_C(0xE3E3E3E3E3E3E3E3), UINT64_C(0xE4E4E4E4E4E4E4E4), UINT64_C(0xE5E5E5E5E5E5E5E5) };
static limb_t l_exp_24[] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0xE1E1E1E1E1E1E1E1), UINT64_C(0xE2E2E2E2E2E2E2E2), UINT64_C(0xE3E3E3E3E3E3E3E3), UINT64_C(0xE4E4E4E4E4E4E4E4), UINT64_C(0xE5E5E5E5E5E5E5E5) };
static limb_t l_in_25[]  = { UINT64_C(0xE1E1E1E1E1E1E1E1), UINT64_C(0xE2E2E2E2E2E2E2E2), UINT64_C(0xE3E3E3E3E3E3E3E3), UINT64_C(0xE4E4E4E4E4E4E4E4), UINT64_C(0xE5E5E5E5E5E5E5E5) };
static limb_t l_exp_25[] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0xE1E1E1E1E1E1E1E1), UINT64_C(0xE2E2E2E2E2E2E2E2), UINT64_C(0xE3E3E3E3E3E3E3E3), UINT64_C(0xE4E4E4E4E4E4E4E4), UINT64_C(0xE5E5E5E5E5E5E5E5) };
static limb_t l_in_26[]  = { UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000000), UINT64_C(0x1111111111111111) };
static limb_t l_exp_26[] = { UINT64_C(0x0000000000000000), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000000), UINT64_C(0x1111111111111111) };
static limb_t l_in_27[]  = { UINT64_C(0xBA9876543210FEDC), UINT64_C(0xABCDEF0123456789) };
static limb_t l_exp_27[] = { UINT64_C(0x0000000000000000), UINT64_C(0xBA9876543210FEDC), UINT64_C(0xABCDEF0123456789) };
static limb_t l_in_28[]  = { UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000002) };
static limb_t l_exp_28[] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000002) };
static limb_t l_in_29[]  = { UINT64_C(0xDEADBEEFDEADBEEF), UINT64_C(0xCAFEBABECAFEBABE) };
static limb_t l_exp_29[] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0xDEADBEEFDEADBEEF), UINT64_C(0xCAFEBABECAFEBABE) };
static limb_t l_in_30[]  = { UINT64_C(0x1111222233334444), UINT64_C(0x5555666677778888) };
static limb_t l_exp_30[] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x1111222233334444), UINT64_C(0x5555666677778888) };
static limb_t l_in_31[]  = { UINT64_C(0x000000000000000A), UINT64_C(0x000000000000000B) };
static limb_t l_exp_31[] = { UINT64_C(0x0000000000000000), UINT64_C(0x000000000000000A), UINT64_C(0x000000000000000B) };
static limb_t l_in_32[]  = { UINT64_C(0x000000000000000A) };
static limb_t l_exp_32[] = { UINT64_C(0x0000000000000000), UINT64_C(0x000000000000000A) };
static limb_t l_in_33[]  = { UINT64_C(0x000000000000000C) };
static limb_t l_exp_33[] = { UINT64_C(0x0000000000000000), UINT64_C(0x000000000000000C) };
static limb_t l_in_34[]  = { UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000002) };
static limb_t l_exp_34[] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000002) };
static limb_t l_in_35[]  = { UINT64_C(0x1000000000000000), UINT64_C(0x2000000000000000) };
static limb_t l_exp_35[] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000000), UINT64_C(0x1000000000000000), UINT64_C(0x2000000000000000) };
static limb_t l_in_36[]  = { UINT64_C(0x0123456789ABCDEF) };
static limb_t l_exp_36[] = { UINT64_C(0x0123456789ABCDEF) };
static limb_t l_in_37[]  = { UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000002) };
static limb_t l_exp_37[] = { UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000002) };
static limb_t l_in_38[]  = { UINT64_C(0x1111111111111111), UINT64_C(0x2222222222222222), UINT64_C(0x3333333333333333) };
static limb_t l_exp_38[] = { UINT64_C(0x1111111111111111), UINT64_C(0x2222222222222222), UINT64_C(0x3333333333333333) };
static limb_t l_in_39[]  = { UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xEEEEEEEEEEEEEEEE) };
static limb_t l_exp_39[] = { UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xEEEEEEEEEEEEEEEE) };
static limb_t l_in_40[]  = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000001) };
static limb_t l_exp_40[] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000001) };
static limb_t l_in_41[]  = { UINT64_C(0x0000000000000001) };
static limb_t l_exp_41[] = { UINT64_C(0x0000000000000000) };
static limb_t l_in_42[]  = { UINT64_C(0x0000000000000005), UINT64_C(0x0000000000000006) };
static limb_t l_exp_42[] = { UINT64_C(0x0000000000000000) };
static limb_t l_in_43[]  = { UINT64_C(0x123456789ABCDEF0), UINT64_C(0x23456789ABCDEF01), UINT64_C(0x3456789ABCDEF012) };
static limb_t l_exp_43[] = { UINT64_C(0x0000000000000000) };
static limb_t l_in_44[]  = { UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000002), UINT64_C(0x0000000000000003), UINT64_C(0x0000000000000004) };
static limb_t l_exp_44[] = { UINT64_C(0x0000000000000000) };
static limb_t l_in_45[]  = { UINT64_C(0x000000000000000A), UINT64_C(0x000000000000000B) };
static limb_t l_exp_45[] = { UINT64_C(0x0000000000000000) };
static limb_t l_in_46[]  = { UINT64_C(0x1111111111111111), UINT64_C(0x2222222222222222) };
static limb_t l_exp_46[] = { UINT64_C(0x0000000000000000) };
static limb_t l_in_47[]  = { UINT64_C(0x9999999999999999), UINT64_C(0x8888888888888888), UINT64_C(0x7777777777777777) };
static limb_t l_exp_47[] = { UINT64_C(0x0000000000000000) };
static limb_t l_in_48[]  = { UINT64_C(0xAAAAAAAAAAAAAAAA), UINT64_C(0xBBBBBBBBBBBBBBBB), UINT64_C(0xCCCCCCCCCCCCCCCC), UINT64_C(0xDDDDDDDDDDDDDDDD) };
static limb_t l_exp_48[] = { UINT64_C(0x0000000000000000) };
static limb_t l_in_49[]  = { UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000002), UINT64_C(0x0000000000000003), UINT64_C(0x0000000000000004), UINT64_C(0x0000000000000005) };
static limb_t l_exp_49[] = { UINT64_C(0x0000000000000000) };
static limb_t l_in_50[]  = { UINT64_C(0x0000000000000001) };
static limb_t l_exp_50[] = { UINT64_C(0x0000000000000000) };

// ---- Poisoned Left-Shift Invariant Arrays (51 - 60) ----
static limb_t l_poison_in_51[] = { UINT64_C(0x1111111111111111) };
static limb_t l_poison_exp_51[] = { UINT64_C(0x1111111111111111) };
static limb_t l_poison_in_52[] = { UINT64_C(0x2222222222222222), UINT64_C(0x3333333333333333) };
static limb_t l_poison_exp_52[] = { UINT64_C(0x2222222222222222), UINT64_C(0x3333333333333333) };
static limb_t l_poison_in_53[] = { UINT64_C(0x4444444444444444), UINT64_C(0x5555555555555555), UINT64_C(0x6666666666666666) };
static limb_t l_poison_exp_53[] = { UINT64_C(0x4444444444444444), UINT64_C(0x5555555555555555), UINT64_C(0x6666666666666666) };
static limb_t l_poison_in_54[] = { UINT64_C(0x7777777777777777), UINT64_C(0x8888888888888888), UINT64_C(0x9999999999999999), UINT64_C(0xAAAAAAAAAAAAAAAA) };
static limb_t l_poison_exp_54[] = { UINT64_C(0x7777777777777777), UINT64_C(0x8888888888888888), UINT64_C(0x9999999999999999), UINT64_C(0xAAAAAAAAAAAAAAAA) };
static limb_t l_poison_in_55[] = { UINT64_C(0xBBBBBBBBBBBBBBBB), UINT64_C(0xCCCCCCCCCCCCCCCC) };
static limb_t l_poison_exp_55[] = { UINT64_C(0xBBBBBBBBBBBBBBBB), UINT64_C(0xCCCCCCCCCCCCCCCC) };
static limb_t l_poison_in_56[] = { UINT64_C(0xDDDDDDDDDDDDDDDD), UINT64_C(0xEEEEEEEEEEEEEEEE), UINT64_C(0xFFFFFFFFFFFFFFFF) };
static limb_t l_poison_exp_56[] = { UINT64_C(0xDDDDDDDDDDDDDDDD), UINT64_C(0xEEEEEEEEEEEEEEEE), UINT64_C(0xFFFFFFFFFFFFFFFF) };
static limb_t l_poison_in_57[] = { UINT64_C(0x123456789ABCDEF0) };
static limb_t r_poison_exp_57_extra[] = { UINT64_C(0x123456789ABCDEF0) };
static limb_t l_poison_in_58[] = { UINT64_C(0x0FEDCBA987654321), UINT64_C(0xABCDEF0123456789) };
static limb_t r_poison_exp_58_extra[] = { UINT64_C(0x0FEDCBA987654321), UINT64_C(0xABCDEF0123456789) };
static limb_t l_poison_in_59[] = { UINT64_C(0x9999888877776666), UINT64_C(0x5555444433332222), UINT64_C(0x11110000FFFFEEEE) };
static limb_t r_poison_exp_59_extra[] = { UINT64_C(0x9999888877776666), UINT64_C(0x5555444433332222), UINT64_C(0x11110000FFFFEEEE) };
static limb_t l_poison_in_60[] = { UINT64_C(0x0000000000000001) };
static limb_t r_poison_exp_60_extra[] = { UINT64_C(0x0000000000000001) };


/* ========================================================================== */
/* GLOBAL TEST CASE REGISTRIES                         */
/* ========================================================================== */

static const limb_shift_case rlshift_cases[60] = {
    // ---- Group 1: Valid Right-Shift Structural Mapping Operations (1 - 50) ----
    { { 1, 4, r_in_01, 1, false }, 0, { 1, 4, r_exp_01, 1, false } },
    { { 1, 4, r_in_02, 1, false }, 0, { 1, 4, r_exp_02, 1, false } },
    { { 1, 4, r_in_03, 1, false }, 1, { 0, 4, r_exp_03, 1, false } },
    { { 1, 4, r_in_04, 1, false }, 0, { 1, 4, r_exp_04, 1, false } },
    { { 1, 4, r_in_05, 1, false }, 3, { 0, 4, r_exp_05, 1, false } },
    { { 2, 4, r_in_06, 1, false }, 1, { 1, 4, r_exp_06, 1, false } },
    { { 2, 4, r_in_07, 1, false }, 2, { 0, 4, r_exp_07, 1, false } },
    { { 2, 4, r_in_08, 1, false }, 0, { 2, 4, r_exp_08, 1, false } },
    { { 2, 4, r_in_09, 1, false }, 1, { 1, 4, r_exp_09, 1, false } },
    { { 2, 4, r_in_10, 1, false }, 5, { 0, 4, r_exp_10, 1, false } },
    { { 3, 6, r_in_11, 1, false }, 1, { 2, 6, r_exp_11, 1, false } },
    { { 3, 6, r_in_12, 1, false }, 2, { 1, 6, r_exp_12, 1, false } },
    { { 3, 6, r_in_13, 1, false }, 3, { 0, 6, r_exp_13, 1, false } },
    { { 3, 6, r_in_14, 1, false }, 0, { 3, 6, r_exp_14, 1, false } },
    { { 3, 6, r_in_15, 1, false }, 4, { 0, 6, r_exp_15, 1, false } },
    { { 4, 8, r_in_16, 1, false }, 1, { 3, 8, r_exp_16, 1, false } },
    { { 4, 8, r_in_17, 1, false }, 2, { 2, 8, r_exp_17, 1, false } },
    { { 4, 8, r_in_18, 1, false }, 3, { 1, 8, r_exp_18, 1, false } },
    { { 4, 8, r_in_19, 1, false }, 4, { 0, 8, r_exp_19, 1, false } },
    { { 4, 8, r_in_20, 1, false }, 8, { 0, 8, r_exp_20, 1, false } },
    { { 5, 10, r_in_21, 1, false }, 1, { 4, 10, r_exp_21, 1, false } },
    { { 5, 10, r_in_22, 1, false }, 2, { 3, 10, r_exp_22, 1, false } },
    { { 5, 10, r_in_23, 1, false }, 3, { 2, 10, r_exp_23, 1, false } },
    { { 5, 10, r_in_24, 1, false }, 4, { 1, 10, r_exp_24, 1, false } },
    { { 5, 10, r_in_25, 1, false }, 5, { 0, 10, r_exp_25, 1, false } },
    { { 3, 8, r_in_26, 1, false }, 1, { 2, 8, r_exp_26, 1, false } },
    { { 4, 8, r_in_27, 1, false }, 1, { 3, 8, r_exp_27, 1, false } },
    { { 5, 8, r_in_28, 1, false }, 3, { 2, 8, r_exp_28, 1, false } },
    { { 3, 8, r_in_29, 1, false }, 2, { 1, 8, r_exp_29, 1, false } },
    { { 4, 8, r_in_30, 1, false }, 2, { 2, 8, r_exp_30, 1, false } },
    { { 2, 4, r_in_31, 1, false }, 1, { 1, 4, r_exp_31, 1, false } },
    { { 2, 4, r_in_32, 1, false }, 1, { 1, 4, r_exp_32, 1, false } },
    { { 1, 4, r_in_33, 1, false }, 1, { 0, 4, r_exp_33, 1, false } },
    { { 6, 8, r_in_34, 1, false }, 3, { 3, 8, r_exp_34, 1, false } },
    { { 7, 12, r_in_35, 1, false }, 5, { 2, 12, r_exp_35, 1, false } },
    { { 1, 32, r_in_36, 1, false }, 0, { 1, 32, r_exp_36, 1, false } },
    { { 2, 32, r_in_37, 1, false }, 0, { 2, 32, r_exp_37, 1, false } },
    { { 3, 32, r_in_38, 1, false }, 0, { 3, 32, r_exp_38, 1, false } },
    { { 4, 32, r_in_39, 1, false }, 0, { 4, 32, r_exp_39, 1, false } },
    { { 3, 32, r_in_40, 1, false }, 2, { 1, 32, r_exp_40, 1, false } },
    { { 3, 32, r_in_41, 1, false }, 3, { 0, 32, r_exp_41, 1, false } },
    { { 2, 32, r_in_42, 1, false }, 4, { 0, 32, r_exp_42, 1, false } },
    { { 3, 32, r_in_43, 1, false }, 10, { 0, 32, r_exp_43, 1, false } },
    { { 4, 32, r_in_44, 1, false }, 32, { 0, 32, r_exp_44, 1, false } },
    { { 2, 32, r_in_45, 1, false }, 100, { 0, 32, r_exp_45, 1, false } },
    { { 2, 2, r_in_46, 1, false }, 2, { 0, 2, r_exp_46, 1, false } },
    { { 3, 3, r_in_47, 1, false }, 3, { 0, 3, r_exp_47, 1, false } },
    { { 4, 4, r_in_48, 1, false }, 4, { 0, 4, r_exp_48, 1, false } },
    { { 5, 5, r_in_49, 1, false }, 5, { 0, 5, r_exp_49, 1, false } },
    { { 1, 1, r_in_50, 1, false }, 1, { 0, 1, r_exp_50, 1, false } },

    // ---- Group 2: Poisoned Right-Shift Fixed/Immutable Invariant Limits (51 - 60) ----
    { { 1, 4, r_poison_in_51, 1, true }, 0, { 1, 4, r_poison_exp_51, 1, true } },
    { { 2, 4, r_poison_in_52, 1, true }, 1, { 2, 4, r_poison_exp_52, 1, true } },
    { { 3, 6, r_poison_in_53, 1, true }, 2, { 3, 6, r_poison_exp_53, 1, true } },
    { { 4, 8, r_poison_in_54, 1, true }, 3, { 4, 8, r_poison_exp_54, 1, true } },
    { { 2, 4, r_poison_in_55, 1, true }, 10, { 2, 4, r_poison_exp_55, 1, true } },
    { { 3, 6, r_poison_in_56, 1, true }, 0, { 3, 6, r_poison_exp_56, 1, true } },
    { { 1, 32, r_poison_in_57, 1, true }, 5, { 1, 32, r_poison_exp_57, 1, true } },
    { { 2, 32, r_poison_in_58, 1, true }, 1, { 2, 32, r_poison_exp_58, 1, true } },
    { { 3, 32, r_poison_in_59, 1, true }, 2, { 3, 32, r_poison_exp_59, 1, true } },
    { { 1, 1, r_poison_in_60, 1, true }, 1, { 1, 1, r_poison_exp_60, 1, true } }
};

static const limb_shift_case llshift_cases[60] = {
    // ---- Group 3: Valid Left-Shift Structural Mapping Operations (1 - 50) ----
    { { 1, 4, l_in_01, 1, false }, 0, { 1, 4, l_exp_01, 1, false } },
    { { 1, 4, l_in_02, 1, false }, 0, { 1, 4, l_exp_02, 1, false } },
    { { 1, 4, l_in_03, 1, false }, 1, { 2, 4, l_exp_03, 1, false } },
    { { 1, 4, l_in_04, 1, false }, 0, { 1, 4, l_exp_04, 1, false } },
    { { 1, 4, l_in_05, 1, false }, 2, { 3, 4, l_exp_05, 1, false } },
    { { 2, 4, l_in_06, 1, false }, 1, { 3, 4, l_exp_06, 1, false } },
    { { 2, 4, l_in_07, 1, false }, 2, { 4, 4, l_exp_07, 1, false } },
    { { 2, 4, l_in_08, 1, false }, 0, { 2, 4, l_exp_08, 1, false } },
    { { 2, 5, l_in_09, 1, false }, 1, { 3, 5, l_exp_09, 1, false } },
    { { 2, 5, l_in_10, 1, false }, 3, { 5, 5, l_exp_07_extra, 1, false } },
    { { 3, 6, l_in_11, 1, false }, 1, { 4, 6, l_exp_11, 1, false } },
    { { 3, 6, l_in_12, 1, false }, 2, { 5, 6, l_exp_12, 1, false } },
    { { 3, 6, l_in_13, 1, false }, 3, { 6, 6, l_exp_13, 1, false } },
    { { 3, 6, l_in_14, 1, false }, 0, { 3, 6, l_exp_14, 1, false } },
    { { 3, 6, l_in_15, 1, false }, 2, { 5, 6, l_exp_15, 1, false } },
    { { 4, 8, l_in_16, 1, false }, 1, { 5, 8, l_exp_16, 1, false } },
    { { 4, 8, l_in_17, 1, false }, 2, { 6, 8, l_exp_17, 1, false } },
    { { 4, 8, l_in_18, 1, false }, 3, { 7, 8, l_exp_18, 1, false } },
    { { 4, 8, l_in_19, 1, false }, 4, { 8, 8, l_exp_19, 1, false } },
    { { 4, 8, l_in_20, 1, false }, 4, { 8, 8, l_exp_20, 1, false } },
    { { 5, 10, l_in_21, 1, false }, 1, { 6, 10, l_exp_21, 1, false } },
    { { 5, 10, l_in_22, 1, false }, 2, { 7, 10, l_exp_22, 1, false } },
    { { 5, 10, l_in_23, 1, false }, 3, { 8, 10, l_exp_23, 1, false } },
    { { 5, 10, l_in_24, 1, false }, 4, { 9, 10, l_exp_24, 1, false } },
    { { 5, 10, l_in_25, 1, false }, 5, { 10, 10, l_exp_25, 1, false } },
    { { 3, 8, l_in_26, 1, false }, 1, { 4, 8, l_exp_26, 1, false } },
    { { 2, 8, l_in_27, 1, false }, 1, { 3, 8, l_exp_27, 1, false } },
    { { 2, 8, l_in_28, 1, false }, 2, { 4, 8, l_exp_28, 1, false } },
    { { 2, 8, l_in_29, 1, false }, 3, { 5, 8, l_exp_29, 1, false } },
    { { 2, 8, l_in_30, 1, false }, 4, { 6, 8, l_exp_30, 1, false } },
    { { 2, 4, l_in_31, 1, false }, 1, { 3, 4, l_exp_31, 1, false } },
    { { 1, 4, l_in_32, 1, false }, 1, { 2, 4, l_exp_32, 1, false } },
    { { 1, 4, l_in_33, 1, false }, 1, { 2, 4, l_exp_33, 1, false } },
    { { 2, 8, l_in_34, 1, false }, 1, { 3, 8, l_exp_34, 1, false } },
    { { 2, 12, l_in_35, 1, false }, 2, { 4, 12, l_exp_35, 1, false } },
    { { 1, 32, l_in_36, 1, false }, 0, { 1, 32, l_exp_36, 1, false } },
    { { 2, 32, l_in_37, 1, false }, 0, { 2, 32, l_exp_37, 1, false } },
    { { 3, 32, l_in_38, 1, false }, 0, { 3, 32, l_exp_38, 1, false } },
    { { 2, 32, l_in_39, 1, false }, 0, { 2, 32, l_exp_39, 1, false } },
    { { 2, 32, l_in_40, 1, false }, 0, { 2, 32, l_exp_40, 1, false } },
    { { 1, 32, l_in_41, 1, false }, 32, { 0, 32, l_exp_41, 1, false } },
    { { 2, 32, l_in_42, 1, false }, 32, { 0, 32, l_exp_42, 1, false } },
    { { 3, 32, l_in_43, 1, false }, 35, { 0, 32, l_exp_43, 1, false } },
    { { 4, 32, l_in_44, 1, false }, 100, { 0, 32, l_exp_44, 1, false } },
    { { 2, 32, l_in_45, 1, false }, 40, { 0, 32, l_exp_45, 1, false } },
    { { 2, 2, l_in_46, 1, false }, 2, { 0, 2, l_exp_46, 1, false } },
    { { 3, 3, l_in_47, 1, false }, 3, { 0, 3, l_exp_47, 1, false } },
    { { 4, 4, l_in_48, 1, false }, 4, { 0, 4, l_exp_48, 1, false } },
    { { 5, 5, l_in_49, 1, false }, 5, { 0, 5, l_exp_49, 1, false } },
    { { 1, 1, l_in_50, 1, false }, 1, { 0, 1, l_exp_50, 1, false } },

    // ---- Group 4: Poisoned Left-Shift Fixed/Immutable Invariant Limits (51 - 60) ----
    { { 1, 4, l_poison_in_51, 1, true }, 0, { 1, 4, l_poison_exp_51, 1, true } },
    { { 2, 4, l_poison_in_52, 1, true }, 1, { 2, 4, l_poison_exp_52, 1, true } },
    { { 3, 6, l_poison_in_53, 1, true }, 2, { 3, 6, l_poison_exp_53, 1, true } },
    { { 4, 8, l_poison_in_54, 1, true }, 3, { 4, 8, l_poison_exp_54, 1, true } },
    { { 2, 4, l_poison_in_55, 1, true }, 10, { 2, 4, l_poison_exp_55, 1, true } },
    { { 3, 6, l_poison_in_56, 1, true }, 0, { 3, 6, l_poison_exp_56, 1, true } },
    { { 1, 32, l_poison_in_57, 1, true }, 5, { 1, 32, r_poison_exp_57_extra, 1, true } },
    { { 2, 32, l_poison_in_58, 1, true }, 1, { 2, 32, r_poison_exp_58_extra, 1, true } },
    { { 3, 32, l_poison_in_59, 1, true }, 2, { 3, 32, r_poison_exp_59_extra, 1, true } },
    { { 1, 1, l_poison_in_60, 1, true }, 1, { 1, 1, r_poison_exp_60_extra, 1, true } }
};

int main(void) { 
    _libdnml_init();
    int total_tests = 0, passed_tests = 0;
    struct timespec start, end; 
    clock_gettime(CLOCK_MONOTONIC, &start);
    limb_t *ret_buf = (limb_t *)malloc(MAX_SIZE_T * sizeof(limb_t));
    printf("====================================================================\n");
    printf("   RUNNING INTEGRATED UNIT TESTS - CRYPT-NUM LIMB-SHIFT UTILITIES   \n");
    printf("====================================================================\n");
    printf("---- __CRINT_INTERNAL_RLSHIFT -----\n");
    for (int i = 0; i < 60; i++) { 
        total_tests++; 
        memset(ret_buf, 0, MAX_SIZE_T * sizeof(limb_t));
        if (rlshift_cases[i].in.limbs && rlshift_cases[i].in.n > 0) {
            memcpy(ret_buf, rlshift_cases[i].in.limbs, rlshift_cases[i].in.n * sizeof(limb_t));
        }
        crint test_x; test_x.limbs = ret_buf;
        test_x.n = rlshift_cases[i].in.n; 
        test_x.cap = rlshift_cases[i].in.cap;
        test_x.poisoned = rlshift_cases[i].in.poisoned;
        
        __CRINT_INTERNAL_RLSHIFT__(&test_x, test_x.cap, rlshift_cases[i].klimbs);
        int match = (test_x.n == rlshift_cases[i].exp.n);
        if (match && test_x.n > 0) {
            match = (memcmp(test_x.limbs, rlshift_cases[i].exp.limbs, test_x.n * sizeof(limb_t)) == 0);
        }
        if (match) passed_tests++;
        else printf("[FAIL] RLSHIFT Case %2d: Missed structural matching requirements.\n", i);
    }
    printf("---- __CRINT_INTERNAL_LLSHIFT -----\n");
    for (int i = 0; i < 60; i++) { 
        total_tests++; 
        memset(ret_buf, 0, MAX_SIZE_T * sizeof(limb_t));
        if (llshift_cases[i].in.limbs && llshift_cases[i].in.n > 0) {
            memcpy(ret_buf, llshift_cases[i].in.limbs, llshift_cases[i].in.n * sizeof(limb_t));
        }
        crint test_x; test_x.limbs = ret_buf;
        test_x.n = llshift_cases[i].in.n; 
        test_x.cap = llshift_cases[i].in.cap;
        test_x.poisoned = llshift_cases[i].in.poisoned;

        __CRINT_INTERNAL_LLSHIFT__(&test_x, test_x.cap, llshift_cases[i].klimbs);
        int match = (test_x.n == llshift_cases[i].exp.n);
        if (match && test_x.n > 0) {
            match = (memcmp(test_x.limbs, llshift_cases[i].exp.limbs, test_x.n * sizeof(limb_t)) == 0);
        }
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
