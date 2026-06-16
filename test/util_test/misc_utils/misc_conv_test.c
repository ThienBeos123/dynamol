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
#include "../../../util/crt_util.h"
#include "../../../libdnml_base.h"
#define CASE_CNT 100
typedef struct { size_t digit_cnt; uint8_t base; size_t exp; } bcnt_case_t;
typedef struct { uint64_t val; uint8_t base; uint8_t exp; } basen_case_t;
typedef struct { int64_t val; uint64_t exp; } mag_case_t;
//* ============== GLOBAL ARRAY OF CASES ============== *//
static const bcnt_case_t bcnt_cases[CASE_CNT] = {
    { 0, 2, 0 }, { 1, 2, 1 }, { 2, 2, 2 }, { 3, 2, 3 }, { 4, 2, 4 },
    { 8, 2, 8 }, { 16, 2, 16 }, { 32, 2, 32 }, { 64, 2, 64 }, { 128, 2, 128 },
    { 0, 8, 0 }, { 1, 8, 3 }, { 2, 8, 6 }, { 3, 8, 9 }, { 4, 8, 12 },
    { 8, 8, 24 }, { 16, 8, 48 }, { 32, 8, 96 }, { 64, 8, 192 }, { 128, 8, 384 },
    // Base 10 corrected upperbounds (ceill(cnt * log2(10)))
    { 0, 10, 0 }, { 1, 10, 4 }, { 2, 10, 7 }, { 3, 10, 10 }, { 4, 10, 14 },
    { 5, 10, 17 }, { 6, 10, 20 }, { 7, 10, 24 }, { 8, 10, 27 }, { 9, 10, 30 },
    { 10, 10, 34 }, { 11, 10, 37 }, { 12, 10, 40 }, { 13, 10, 44 }, { 14, 10, 47 },
    { 15, 10, 50 }, { 16, 10, 54 }, { 20, 10, 67 }, { 64, 10, 213 },
    { 0, 16, 0 }, { 1, 16, 4 }, { 2, 16, 8 }, { 3, 16, 12 }, { 4, 16, 16 },
    { 8, 16, 32 }, { 16, 16, 64 }, { 32, 16, 128 }, { 64, 16, 256 }, { 128, 16, 512 },
    // Arbitrary Bases Upperbounds corrected
    { 1, 3, 2 }, { 2, 3, 4 }, { 5, 3, 8 }, { 10, 3, 16 }, { 20, 3, 32 },
    { 50, 3, 80 }, { 1, 4, 2 }, { 2, 4, 4 }, { 10, 4, 20 }, { 20, 4, 40 },
    { 1, 5, 3 }, { 5, 5, 12 }, { 10, 5, 24 }, { 20, 5, 47 }, { 1, 6, 3 },
    { 10, 6, 26 }, { 20, 6, 52 }, { 1, 7, 3 }, { 10, 7, 29 }, { 20, 7, 57 },
    { 1, 9, 4 }, { 10, 9, 32 }, { 20, 9, 64 }, { 1, 11, 4 }, { 10, 11, 35 },
    { 20, 11, 70 }, { 1, 12, 4 }, { 10, 12, 36 }, { 20, 12, 72 }, { 1, 20, 5 },
    { 10, 20, 44 }, { 20, 20, 87 }, { 1, 32, 5 }, { 10, 32, 50 }, { 20, 32, 100 },
    { 1, 64, 6 }, { 10, 64, 60 }, { 20, 64, 120 }, { 100, 2, 100 }, { 100, 8, 300 },
    { 100, 16, 400 }, { 100, 10, 333 }, { 1000, 2, 1000 }, { 1000, 8, 3000 }, { 1000, 16, 4000 },
    { 1000, 10, 3322 }, { 500, 2, 500 }, { 500, 8, 1500 }, { 500, 16, 2000 }, { 500, 10, 1661 }
};
static const basen_case_t basen_cases[CASE_CNT] = {
    { UINT64_C(1), 10, 1 }, { UINT64_C(9), 10, 1 },
    { UINT64_C(10), 10, 2 }, { UINT64_C(99), 10, 2 },
    { UINT64_C(100), 10, 3 }, { UINT64_C(999), 10, 3 },
    { UINT64_C(1000), 10, 4 }, { UINT64_C(9999), 10, 4 },
    { UINT64_C(10000), 10, 5 }, { UINT64_C(99999), 10, 5 },
    { UINT64_C(100000), 10, 6 }, { UINT64_C(999999), 10, 6 },
    { UINT64_C(1000000), 10, 7 }, { UINT64_C(9999999), 10, 7 },
    { UINT64_C(10000000), 10, 8 }, { UINT64_C(99999999), 10, 8 },
    { UINT64_C(100000000), 10, 9 }, { UINT64_C(999999999), 10, 9 },
    { UINT64_C(1000000000), 10, 10 }, { UINT64_C(9999999999), 10, 10 },
    { UINT64_C(10000000000), 10, 11 }, { UINT64_C(99999999999), 10, 11 },
    { UINT64_C(100000000000), 10, 12 }, { UINT64_C(999999999999), 10, 12 },
    { UINT64_C(1000000000000), 10, 13 }, { UINT64_C(10000000004095), 10, 14 },
    { UINT64_C(100000000000000), 10, 15 }, { UINT64_C(999999999999999), 10, 15 },
    { UINT64_C(1000000000000000), 10, 16 }, { UINT64_C(9999999999999999), 10, 16 },
    { UINT64_C(10000000000000000), 10, 17 }, { UINT64_C(10000000000000000), 10, 17 },
    { UINT64_C(100000000000000000), 10, 18 }, { UINT64_C(16000000000000000000), 10, 20 },
    { UINT64_C(0xFFFFFFFFFFFFFFFF), 10, 20 }, { UINT64_C(1), 2, 1 },
    { UINT64_C(2), 2, 2 }, { UINT64_C(3), 2, 2 },
    { UINT64_C(4), 2, 3 }, { UINT64_C(7), 2, 3 },
    { UINT64_C(8), 2, 4 }, { UINT64_C(15), 2, 4 },
    { UINT64_C(16), 2, 5 }, { UINT64_C(127), 2, 7 },
    { UINT64_C(128), 2, 8 }, { UINT64_C(255), 2, 8 },
    { UINT64_C(260), 2, 9 }, { UINT64_C(0x000000000000FFFF), 2, 16 },
    { UINT64_C(0x0000000000010000), 2, 17 }, { UINT64_C(0x00000000FFFFFFFF), 2, 32 },
    { UINT64_C(0x0000000100000000), 2, 33 }, { UINT64_C(0x7FFFFFFFFFFFFFFF), 2, 63 },
    { UINT64_C(0x8000000000000000), 2, 64 }, { UINT64_C(0xFFFFFFFFFFFFFFFF), 2, 64 },
    { UINT64_C(1), 16, 1 }, { UINT64_C(15), 16, 1 },
    { UINT64_C(16), 16, 2 }, { UINT64_C(255), 16, 2 },
    { UINT64_C(256), 16, 3 }, { UINT64_C(0x000000000000FFF0), 16, 4 },
    { UINT64_C(0x000000000000FFFF), 16, 4 }, { UINT64_C(0x0000000000010000), 16, 5 },
    { UINT64_C(0x00000000FFFFFFFF), 16, 8 }, { UINT64_C(0x0000000100000000), 16, 9 },
    { UINT64_C(0xFFFFFFFFFFFFFFFF), 16, 16 }, { UINT64_C(1), 8, 1 },
    { UINT64_C(7), 8, 1 }, { UINT64_C(8), 8, 2 },
    { UINT64_C(63), 8, 2 }, { UINT64_C(64), 8, 3 },
    { UINT64_C(0x00000000000001FF), 8, 3 }, { UINT64_C(0x0000000000000200), 8, 4 },
    { UINT64_C(0xFFFFFFFFFFFFFFFF), 8, 22 }, { UINT64_C(5), 5, 2 },
    { UINT64_C(24), 5, 2 }, { UINT64_C(25), 5, 3 },
    { UINT64_C(124), 5, 3 }, { UINT64_C(125), 5, 4 },
    { UINT64_C(0xFFFFFFFFFFFFFFFF), 5, 28 }, { UINT64_C(3), 3, 2 },
    { UINT64_C(8), 3, 2 }, { UINT64_C(9), 3, 3 },
    { UINT64_C(26), 3, 3 }, { UINT64_C(27), 3, 4 },
    { UINT64_C(0xFFFFFFFFFFFFFFFF), 3, 41 }, { UINT64_C(32), 32, 2 },
    { UINT64_C(0x00000000000003FF), 32, 2 }, { UINT64_C(0x0000000000000400), 32, 3 },
    { UINT64_C(0xFFFFFFFFFFFFFFFF), 32, 13 }, { UINT64_C(64), 64, 2 },
    { UINT64_C(0x0000000000000FFF), 64, 2 }, { UINT64_C(0x0000000000001000), 64, 3 },
    { UINT64_C(0xFFFFFFFFFFFFFFFF), 64, 11 }, { UINT64_C(10), 10, 2 },
    { UINT64_C(100), 10, 3 }, { UINT64_C(1000), 10, 4 },
    { UINT64_C(10000), 10, 5 }, { UINT64_C(100000), 10, 6 },
    { UINT64_C(1000000), 10, 7 }, { UINT64_C(10000000), 10, 8 }
};
static const mag_case_t mag_cases[100] = {
    { INT64_C(0x0000000000000000), UINT64_C(0x0000000000000000) }, /* 0 */
    { INT64_C(0x0000000000000001), UINT64_C(0x0000000000000001) }, /* 1 */
    { INT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0000000000000001) }, /* 2 */
    { INT64_C(0x0000000000000002), UINT64_C(0x0000000000000002) }, /* 3 */
    { INT64_C(0xFFFFFFFFFFFFFFFE), UINT64_C(0x0000000000000002) }, /* 4 */
    { INT64_C(0x000000000000000F), UINT64_C(0x000000000000000F) }, /* 5 */
    { INT64_C(0xFFFFFFFFFFFFFFF1), UINT64_C(0x000000000000000F) }, /* 6 */
    { INT64_C(0x0000000000000010), UINT64_C(0x0000000000000010) }, /* 7 */
    { INT64_C(0xFFFFFFFFFFFFFFF0), UINT64_C(0x0000000000000010) }, /* 8 */
    { INT64_C(0x00000000000000FF), UINT64_C(0x00000000000000FF) }, /* 9 */
    { INT64_C(0xFFFFFFFFFFFFFF01), UINT64_C(0x00000000000000FF) }, /* 10 */
    { INT64_C(0x0000000000000100), UINT64_C(0x0000000000000100) }, /* 11 */
    { INT64_C(0xFFFFFFFFFFFFF900), UINT64_C(0x0000000000000700) }, /* 12 */
    { INT64_C(0x000000000000FFFF), UINT64_C(0x000000000000FFFF) }, /* 13 */
    { INT64_C(0xFFFFFFFFFFFF0001), UINT64_C(0x000000000000FFFF) }, /* 14 */
    { INT64_C(0x0000000000010000), UINT64_C(0x0000000000010000) }, /* 15 */
    { INT64_C(0xFFFFFFFFFFF0FFFF), UINT64_C(0x00000000000F0001) }, /* 16 */
    { INT64_C(0x00000000FFFFFFFF), UINT64_C(0x00000000FFFFFFFF) }, /* 17 */
    { INT64_C(0xFFFFFFFF00000001), UINT64_C(0x00000000FFFFFFFF) }, /* 18 */
    { INT64_C(0x0000000100000000), UINT64_C(0x0000000100000000) }, /* 19 */
    { INT64_C(0xFFFFFFFEFFFFFFFF), UINT64_C(0x0000000100000001) }, /* 20 */
    { INT64_C(0x7FFFFFFFFFFFFFFF), UINT64_C(0x7FFFFFFFFFFFFFFF) }, /* 21 */
    { INT64_C(0x8000000000000001), UINT64_C(0x7FFFFFFFFFFFFFFF) }, /* 22 */
    { INT64_C(0x8000000000000000), UINT64_C(0x8000000000000000) }, /* 23 */
    { INT64_C(0x0000000000000003), UINT64_C(0x0000000000000003) }, /* 24 */
    { INT64_C(0xFFFFFFFFFFFFFFFD), UINT64_C(0x0000000000000003) }, /* 25 */
    { INT64_C(0x0000000000000004), UINT64_C(0x0000000000000004) }, /* 26 */
    { INT64_C(0xFFFFFFFFFFFFFFFC), UINT64_C(0x0000000000000004) }, /* 27 */
    { INT64_C(0x0000000000000005), UINT64_C(0x0000000000000005) }, /* 28 */
    { INT64_C(0xFFFFFFFFFFFFFFFB), UINT64_C(0x0000000000000005) }, /* 29 */
    { INT64_C(0x0000000000000006), UINT64_C(0x0000000000000006) }, /* 30 */
    { INT64_C(0xFFFFFFFFFFFFFFFA), UINT64_C(0x0000000000000006) }, /* 31 */
    { INT64_C(0x0000000000000007), UINT64_C(0x0000000000000007) }, /* 32 */
    { INT64_C(0xFFFFFFFFFFFFFFF9), UINT64_C(0x0000000000000007) }, /* 33 */
    { INT64_C(0x0000000000000008), UINT64_C(0x0000000000000008) }, /* 34 */
    { INT64_C(0xFFFFFFFFFFFFFFF8), UINT64_C(0x0000000000000008) }, /* 35 */
    { INT64_C(0x0000000000000009), UINT64_C(0x0000000000000009) }, /* 36 */
    { INT64_C(0xFFFFFFFFFFFFFFF7), UINT64_C(0x0000000000000009) }, /* 37 */
    { INT64_C(0x000000000000000A), UINT64_C(0x000000000000000A) }, /* 38 */
    { INT64_C(0xFFFFFFFFFFFFFFF6), UINT64_C(0x000000000000000A) }, /* 39 */
    { INT64_C(0x000000000000000B), UINT64_C(0x000000000000000B) }, /* 40 */
    { INT64_C(0xFFFFFFFFFFFFFFF5), UINT64_C(0x000000000000000B) }, /* 41 */
    { INT64_C(0x000000000000000C), UINT64_C(0x000000000000000C) }, /* 42 */
    { INT64_C(0xFFFFFFFFFFFFFFF4), UINT64_C(0x000000000000000C) }, /* 43 */
    { INT64_C(0x000000000000000D), UINT64_C(0x000000000000000D) }, /* 44 */
    { INT64_C(0xFFFFFFFFFFFFFFF3), UINT64_C(0x000000000000000D) }, /* 45 */
    { INT64_C(0x000000000000000E), UINT64_C(0x000000000000000E) }, /* 46 */
    { INT64_C(0xFFFFFFFFFFFFFFF2), UINT64_C(0x000000000000000E) }, /* 47 */
    { INT64_C(0x0000000000000011), UINT64_C(0x0000000000000011) }, /* 48 */
    { INT64_C(0xFFFFFFFFFFFFFFEF), UINT64_C(0x0000000000000011) }, /* 49 */
    { INT64_C(0x0000000000000012), UINT64_C(0x0000000000000012) }, /* 50 */
    { INT64_C(0xFFFFFFFFFFFFFFEE), UINT64_C(0x0000000000000012) }, /* 51 */
    { INT64_C(0x0000000000000013), UINT64_C(0x0000000000000013) }, /* 52 */
    { INT64_C(0xFFFFFFFFFFFFFFED), UINT64_C(0x0000000000000013) }, /* 53 */
    { INT64_C(0x0000000000000014), UINT64_C(0x0000000000000014) }, /* 54 */
    { INT64_C(0xFFFFFFFFFFFFFFEC), UINT64_C(0x0000000000000014) }, /* 55 */
    { INT64_C(0x0000000000000015), UINT64_C(0x0000000000000015) }, /* 56 */
    { INT64_C(0xFFFFFFFFFFFFFFEB), UINT64_C(0x0000000000000015) }, /* 57 */
    { INT64_C(0x0000000000000016), UINT64_C(0x0000000000000016) }, /* 58 */
    { INT64_C(0xFFFFFFFFFFFFFFEA), UINT64_C(0x0000000000000016) }, /* 59 */
    { INT64_C(0x0000000000000017), UINT64_C(0x0000000000000017) }, /* 60 */
    { INT64_C(0xFFFFFFFFFFFFFFE9), UINT64_C(0x0000000000000017) }, /* 61 */
    { INT64_C(0x0000000000000018), UINT64_C(0x0000000000000018) }, /* 62 */
    { INT64_C(0xFFFFFFFFFFFFFFE8), UINT64_C(0x0000000000000018) }, /* 63 */
    { INT64_C(0x0000000000000019), UINT64_C(0x0000000000000019) }, /* 64 */
    { INT64_C(0xFFFFFFFFFFFFFFE7), UINT64_C(0x0000000000000019) }, /* 65 */
    { INT64_C(0x000000000000001A), UINT64_C(0x000000000000001A) }, /* 66 */
    { INT64_C(0xFFFFFFFFFFFFFFE6), UINT64_C(0x000000000000001A) }, /* 67 */
    { INT64_C(0x000000000000001B), UINT64_C(0x000000000000001B) }, /* 68 */
    { INT64_C(0xFFFFFFFFFFFFFFE5), UINT64_C(0x000000000000001B) }, /* 69 */
    { INT64_C(0x000000000000001C), UINT64_C(0x000000000000001C) }, /* 70 */
    { INT64_C(0xFFFFFFFFFFFFFFE4), UINT64_C(0x000000000000001C) }, /* 71 */
    { INT64_C(0x000000000000001D), UINT64_C(0x000000000000001D) }, /* 72 */
    { INT64_C(0xFFFFFFFFFFFFFFE3), UINT64_C(0x000000000000001D) }, /* 73 */
    { INT64_C(0x000000000000001E), UINT64_C(0x000000000000001E) }, /* 74 */
    { INT64_C(0xFFFFFFFFFFFFFFE2), UINT64_C(0x000000000000001E) }, /* 75 */
    { INT64_C(0x000000000000001F), UINT64_C(0x000000000000001F) }, /* 76 */
    { INT64_C(0xFFFFFFFFFFFFFFE1), UINT64_C(0x000000000000001F) }, /* 77 */
    { INT64_C(0x0000000000000020), UINT64_C(0x0000000000000020) }, /* 78 */
    { INT64_C(0xFFFFFFFFFFFFFFE0), UINT64_C(0x0000000000000020) }, /* 79 */
    { INT64_C(0x0000000000000064), UINT64_C(0x0000000000000064) }, /* 80 */
    { INT64_C(0xFFFFFFFFFFFFFF9C), UINT64_C(0x0000000000000064) }, /* 81 */
    { INT64_C(0x00000000000003E8), UINT64_C(0x00000000000003E8) }, /* 82 */
    { INT64_C(0xFFFFFFFFFFFFFCE0), UINT64_C(0x0000000000000320) }, /* 83 */
    { INT64_C(0x0000000000002710), UINT64_C(0x0000000000002710) }, /* 84 */
    { INT64_C(0xFFFFFFFFFFFFD8F0), UINT64_C(0x0000000000002710) }, /* 85 */
    { INT64_C(0x00000000000186A0), UINT64_C(0x00000000000186A0) }, /* 86 */
    { INT64_C(0xFFFFFFFFFFFE7960), UINT64_C(0x00000000000186A0) }, /* 87 */
    { INT64_C(0x00000000000F4240), UINT64_C(0x00000000000F4240) }, /* 88 */
    { INT64_C(0xFFFFFFFFFFF0BDC0), UINT64_C(0x00000000000F4240) }, /* 89 */
    { INT64_C(0x0000000000989680), UINT64_C(0x0000000000989680) }, /* 90 */
    { INT64_C(0xFFFFFFFFFF676980), UINT64_C(0x0000000000989680) }, /* 91 */
    { INT64_C(0x0000000005F5E100), UINT64_C(0x0000000005F5E100) }, /* 92 */
    { INT64_C(0xFFFFFFFFFA0A1F00), UINT64_C(0x0000000005F5E100) }, /* 93 */
    { INT64_C(0x000000003B9ACA00), UINT64_C(0x000000003B9ACA00) }, /* 94 */
    { INT64_C(0xFFFFFFFFC4653600), UINT64_C(0x000000003B9ACA00) }, /* 95 */
    { INT64_C(0x00000002540BE400), UINT64_C(0x00000002540BE400) }, /* 96 */
    { INT64_C(0xFFFFFFF9ABF41C00), UINT64_C(0x00000006540BE400) }, /* 97 */
    { INT64_C(0x000000174876E800), UINT64_C(0x000000174876E800) }, /* 98 */
    { INT64_C(0xFFFFFFE8B7891800), UINT64_C(0x000000174876E800) }  /* 99 */
};



int main(void) { _libdnml_init();
    int total_tests = 0, passed_tests = 0;
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    fputs("===================================================================\n", stdout);
    fputs("          RUNNING INTEGRATED UNIT TESTS - MISC CONVERSIONS         \n", stdout);
    fputs("===================================================================\n", stdout);
    fputs("---- __BITCOUNT___ -----\n", stdout);
    for (int i = 0; i < CASE_CNT; i++) { total_tests++;
        const bcnt_case_t *curr_case = &bcnt_cases[i];
        size_t res = __BITCOUNT___(curr_case->digit_cnt, curr_case->base);
        if (res == curr_case->exp) passed_tests++;
        else printf(
            "[FAIL] __BITCOUNT___    | case %-2d | digit_cnt: %-20zu base: %-2" PRIu8 " | res: %8zu | exp: %8zu\n",
            i, curr_case->digit_cnt, curr_case->base, res, curr_case->exp
        );
    }
    fputs("---- __BASEN_DCOUNT__ -----\n", stdout);
    for (int i = 0; i < CASE_CNT; i++) { total_tests++;
        const basen_case_t *curr_case = &basen_cases[i];
        uint8_t res = __BASEN_DCOUNT__(curr_case->val, curr_case->base);
        if (res == curr_case->exp) passed_tests++;
        else printf(
            "[FAIL] __BASEN_DCOUNT__ | case %-2d | val: %-20" PRIu64 " base: %-2" PRIu8 " | res: %3" PRIu8 " | exp: %3" PRIu8 "\n",
            i, curr_case->val, curr_case->base, res, curr_case->exp
        );
    }
    fputs("---- __MAG_I64__ -----\n", stdout);
    for (int i = 0; i < CASE_CNT; i++) { total_tests++;
        const mag_case_t *curr_case = &mag_cases[i];
        uint64_t res = __MAG_I64__(curr_case->val);
        if (res == curr_case->exp) passed_tests++;
        else printf(
            "[FAIL] __MAG_I64__      | case %-2d | val: %-20" PRId64 " | res: %20" PRIu64 " | exp: %20" PRIu64 "\n",
            i, curr_case->val, res, curr_case->exp
        );
    }

    #undef CASE_CNT
    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    fputs( "=========================================================\n", stdout);
    fputs( "TEST SUMMARY:\n", stdout);
    printf("+) Passed %-4d out of %-4d total compiled checks.\n", passed_tests, total_tests);
    printf("+) Success rate: %.2f%%\n", (passed_tests * 100.0) / total_tests);
    printf("+) Total Runtime: %lf ms\n", elapsed_time * 1000.0);
    fputs( "=========================================================\n", stdout);
    _libdnml_cleanup(); return (passed_tests == total_tests) ? 0 : 1;
}
