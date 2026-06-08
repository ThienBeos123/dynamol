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

#include <include.h>
#include <dnml_sys/sys.h>
#include <_libdnml_config/numeric_config.h>
#include "../../intrinsics/arm64/__arm64_conn__.h"
#include "../../intrinsics/risc-v64/__rv64_conn__.h"
#include "../../intrinsics/x86_64/__x86_conn__.h"
#include "../../intrinsics/crt_vanillc/__crt_vanillc_con__.h"
/* ============================== TEST STRUCT TYPES ============================== */
typedef struct { int64_t x; uint8_t ispos_exp, isneg_exp; } sign_test_case;
typedef struct { uint64_t x, y; uint8_t eq_exp, neq_exp; } equality_test_case;
typedef struct { uint8_t cond; uint64_t a, b; uint64_t select_exp; } selection_test_case;

/* ============================== 99 SIGN TEST CASES ============================== */
static const sign_test_case sign_bank[99] = {
    { INT64_C(0x0000000000000000), 1, 0 }, { INT64_C(0x0000000000000001), 1, 0 }, { INT64_C(0xFFFFFFFFFFFFFFFF), 0, 1 }, // 1
    { INT64_C(0x7FFFFFFFFFFFFFFF), 1, 0 }, { INT64_C(0x8000000000000000), 0, 1 }, { INT64_C(0x00000000075BCD15), 1, 0 }, // 2
    { INT64_C(0xFFFFFFFFFA6432EB), 0, 1 }, { INT64_C(0x0010000000000000), 1, 0 }, { INT64_C(0xFFF0000000000000), 0, 1 }, // 3
    { INT64_C(0x000000000000002A), 1, 0 }, { INT64_C(0xFFFFFFFFFFFFFFD6), 0, 1 }, { INT64_C(0x7FFFFFFFFFFFF000), 1, 0 }, // 4
    { INT64_C(0x8000000000001000), 0, 1 }, { INT64_C(0x0000FFFFFFFFFFFF), 1, 0 }, { INT64_C(0xFFFF000000000001), 0, 1 }, // 5
    { INT64_C(0x00000000000000FF), 1, 0 }, { INT64_C(0xFFFFFFFFFFFFFF01), 0, 1 }, { INT64_C(0x0000000000007FFF), 1, 0 }, // 6
    { INT64_C(0xFFFFFFFFFFFF8000), 0, 1 }, { INT64_C(0x000000007FFFFFFF), 1, 0 }, { INT64_C(0xFFFFFFFF80000000), 0, 1 }, // 7
    { INT64_C(0x000000003B9AC9FF), 1, 0 }, { INT64_C(0xFFFFFFFFC4653601), 0, 1 }, { INT64_C(0x0000000012345678), 1, 0 }, // 8
    { INT64_C(0xFFFFFFFFEDCBA988), 0, 1 }, { INT64_C(0x5555555555555555), 1, 0 }, { INT64_C(0xAAAAAAAAAAAAAAAA), 0, 1 }, // 9
    { INT64_C(0x2A2A2A2A2A2A2A2A), 1, 0 }, { INT64_C(0xD5D5D5D5D5D5D5D6), 0, 1 }, { INT64_C(0x0101010101010101), 1, 0 }, // 10
    { INT64_C(0xFEFEFEFEFEFEFEFF), 0, 1 }, { INT64_C(0x7F7F7F7F7F7F7F7F), 1, 0 }, { INT64_C(0x8080808080808081), 0, 1 }, // 11
    { INT64_C(0x0000000000000003), 1, 0 }, { INT64_C(0xFFFFFFFFFFFFFFFD), 0, 1 }, { INT64_C(0x0000000000000004), 1, 0 }, // 12
    { INT64_C(0xFFFFFFFFFFFFFFFC), 0, 1 }, { INT64_C(0x0000000000000005), 1, 0 }, { INT64_C(0xFFFFFFFFFFFFFFFB), 0, 1 }, // 13
    { INT64_C(0x0000000000000006), 1, 0 }, { INT64_C(0xFFFFFFFFFFFFFFFA), 0, 1 }, { INT64_C(0x0000000000000007), 1, 0 }, // 14
    { INT64_C(0xFFFFFFFFFFFFFFF9), 0, 1 }, { INT64_C(0x0000000000000008), 1, 0 }, { INT64_C(0xFFFFFFFFFFFFFFF8), 0, 1 }, // 15
    { INT64_C(0x0000000000000009), 1, 0 }, { INT64_C(0xFFFFFFFFFFFFFFF7), 0, 1 }, { INT64_C(0x000000000000000A), 1, 0 }, // 16
    { INT64_C(0xFFFFFFFFFFFFFFF6), 0, 1 }, { INT64_C(0x000000000000000B), 1, 0 }, { INT64_C(0xFFFFFFFFFFFFFFF5), 0, 1 }, // 17
    { INT64_C(0x000000000000000C), 1, 0 }, { INT64_C(0xFFFFFFFFFFFFFFF4), 0, 1 }, { INT64_C(0x000000000000000D), 1, 0 }, // 18
    { INT64_C(0xFFFFFFFFFFFFFFF3), 0, 1 }, { INT64_C(0x000000000000000E), 1, 0 }, { INT64_C(0xFFFFFFFFFFFFFFF2), 0, 1 }, // 19
    { INT64_C(0x000000000000000F), 1, 0 }, { INT64_C(0xFFFFFFFFFFFFFFF1), 0, 1 }, { INT64_C(0x0000000000000010), 1, 0 }, // 20
    { INT64_C(0xFFFFFFFFFFFFFFF0), 0, 1 }, { INT64_C(0x0000000000000011), 1, 0 }, { INT64_C(0xFFFFFFFFFFFFFFEF), 0, 1 }, // 21
    { INT64_C(0x0000000000000012), 1, 0 }, { INT64_C(0xFFFFFFFFFFFFFFEE), 0, 1 }, { INT64_C(0x0000000000000013), 1, 0 }, // 22
    { INT64_C(0xFFFFFFFFFFFFFFED), 0, 1 }, { INT64_C(0x0000000000000014), 1, 0 }, { INT64_C(0xFFFFFFFFFFFFFFEC), 0, 1 }, // 23
    { INT64_C(0x000000000000001E), 1, 0 }, { INT64_C(0xFFFFFFFFFFFFFFE2), 0, 1 }, { INT64_C(0x0000000000000028), 1, 0 }, // 24
    { INT64_C(0xFFFFFFFFFFFFFFD8), 0, 1 }, { INT64_C(0x0000000000000032), 1, 0 }, { INT64_C(0xFFFFFFFFFFFFFFCE), 0, 1 }, // 25
    { INT64_C(0x000000000000003C), 1, 0 }, { INT64_C(0xFFFFFFFFFFFFFFC4), 0, 1 }, { INT64_C(0x0000000000000046), 1, 0 }, // 26
    { INT64_C(0xFFFFFFFFFFFFFFBA), 0, 1 }, { INT64_C(0x0000000000000050), 1, 0 }, { INT64_C(0xFFFFFFFFFFFFFFB0), 0, 1 }, // 27
    { INT64_C(0x000000000000005A), 1, 0 }, { INT64_C(0xFFFFFFFFFFFFFFA6), 0, 1 }, { INT64_C(0x0000000000000064), 1, 0 }, // 28
    { INT64_C(0xFFFFFFFFFFFFFF9C), 0, 1 }, { INT64_C(0x00000000000001F4), 1, 0 }, { INT64_C(0xFFFFFFFFFFFFFEE0), 0, 1 }, // 29
    { INT64_C(0x00000000000003E8), 1, 0 }, { INT64_C(0xFFFFFFFFFFFFF618), 0, 1 }, { INT64_C(0x0000000000001388), 1, 0 }, // 30
    { INT64_C(0xFFFFFFFFFFFFEC78), 0, 1 }, { INT64_C(0x0000000000002710), 1, 0 }, { INT64_C(0xFFFFFFFFFFFFD8F0), 0, 1 }, // 31
    { INT64_C(0x0000000000004E20), 1, 0 }, { INT64_C(0xFFFFFFFFFFFFB1E0), 0, 1 }, { INT64_C(0x000000000000C350), 1, 0 }, // 32
    { INT64_C(0xFFFFFFFFFFFF3CB0), 0, 1 }, { INT64_C(0x000000000001E240), 1, 0 }, { INT64_C(0xFFFFFFFFFFFE1DC0), 0, 1 }  // 33
};

/* ============================== 100 EQUALITY TEST CASES ============================== */
static const equality_test_case equality_bank[100] = {
/* 1 */   { 0x0000000000000000, 0x0000000000000000, 1, 0 },
/* 2 */   { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 1, 0 },
/* 3 */   { 0x7FFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF, 1, 0 },
/* 4 */   { 0x8000000000000000, 0x8000000000000000, 1, 0 },
/* 5 */   { 0x5555555555555555, 0x5555555555555555, 1, 0 },
/* 6 */   { 0xAAAAAAAAAAAAAAAA, 0xAAAAAAAAAAAAAAAA, 1, 0 },
/* 7 */   { 0x0000000000000001, 0x0000000000000001, 1, 0 },
/* 8 */   { 0x8000000000000001, 0x8000000000000001, 1, 0 },
/* 9 */   { 0xFFFFFFFFFFFFFFFE, 0xFFFFFFFFFFFFFFFE, 1, 0 },
/* 10 */  { 0x123456789ABCDEF0, 0x123456789ABCDEF0, 1, 0 },
/* 11 */  { 0x0000000000000000, 0x0000000000000001, 0, 1 },
/* 12 */  { 0x0000000000000001, 0x0000000000000000, 0, 1 },
/* 13 */  { 0xFFFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF, 0, 1 },
/* 14 */  { 0x7FFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0, 1 },
/* 15 */  { 0x8000000000000000, 0x0000000000000000, 0, 1 },
/* 16 */  { 0x0000000000000000, 0x8000000000000000, 0, 1 },
/* 17 */  { 0x5555555555555555, 0xAAAAAAAAAAAAAAAA, 0, 1 },
/* 18 */  { 0xAAAAAAAAAAAAAAAA, 0x5555555555555555, 0, 1 },
/* 19 */  { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFE, 0, 1 },
/* 20 */  { 0xFFFFFFFFFFFFFFFE, 0xFFFFFFFFFFFFFFFF, 0, 1 },
/* 21 */  { 0x00000000000000FF, 0x0000000000000000, 0, 1 },
/* 22 */  { 0x000000000000FF00, 0x0000000000000000, 0, 1 },
/* 23 */  { 0x0000000000FF0000, 0x0000000000000000, 0, 1 },
/* 24 */  { 0x00000000FF000000, 0x0000000000000000, 0, 1 },
/* 25 */  { 0x000000FF00000000, 0x0000000000000000, 0, 1 },
/* 26 */  { 0x0000FF0000000000, 0x0000000000000000, 0, 1 },
/* 27 */  { 0x00FF000000000000, 0x0000000000000000, 0, 1 },
/* 28 */  { 0xFF00000000000000, 0x0000000000000000, 0, 1 },
/* 29 */  { 0x0000000000000000, 0x00000000000000FF, 0, 1 },
/* 30 */  { 0x0000000000000000, 0x000000000000FF00, 0, 1 },
/* 31 */  { 0x0000000000000000, 0x0000000000FF0000, 0, 1 },
/* 32 */  { 0x0000000000000000, 0x00000000FF000000, 0, 1 },
/* 33 */  { 0x0000000000000000, 0x000000FF00000000, 0, 1 },
/* 34 */  { 0x0000000000000000, 0x0000FF0000000000, 0, 1 },
/* 35 */  { 0x0000000000000000, 0x00FF000000000000, 0, 1 },
/* 36 */  { 0x0000000000000000, 0xFF00000000000000, 0, 1 },
/* 37 */  { 0x0000000000000001, 0x8000000000000001, 0, 1 },
/* 38 */  { 0x8000000000000001, 0x0000000000000001, 0, 1 },
/* 39 */  { 0x7FFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFE, 0, 1 },
/* 40 */  { 0x7FFFFFFFFFFFFFFE, 0x7FFFFFFFFFFFFFFF, 0, 1 },
/* 41 */  { 0x8000000000000000, 0x8000000000000001, 0, 1 },
/* 42 */  { 0x8000000000000001, 0x8000000000000000, 0, 1 },
/* 43 */  { 0x9ABCDEF012345678, 0x9ABCDEF012345678, 1, 0 },
/* 44 */  { 0x9ABCDEF012345678, 0x9ABCDEF012345679, 0, 1 },
/* 45 */  { 0x1111111111111111, 0x1111111111111111, 1, 0 },
/* 46 */  { 0x2222222222222222, 0x2222222222222222, 1, 0 },
/* 47 */  { 0x3333333333333333, 0x3333333333333333, 1, 0 },
/* 48 */  { 0x4444444444444444, 0x4444444444444444, 1, 0 },
/* 49 */  { 0x6666666666666666, 0x6666666666666666, 1, 0 },
/* 50 */  { 0x7777777777777777, 0x7777777777777777, 1, 0 },
/* 51 */  { 0x8888888888888888, 0x8888888888888888, 1, 0 },
/* 52 */  { 0x9999999999999999, 0x9999999999999999, 1, 0 },
/* 53 */  { 0xBBBBBBBBBBBBBBBB, 0xBBBBBBBBBBBBBBBB, 1, 0 },
/* 54 */  { 0xCCCCCCCCCCCCCCCC, 0xCCCCCCCCCCCCCCCC, 1, 0 },
/* 55 */  { 0xDDDDDDDDDDDDDDDD, 0xDDDDDDDDDDDDDDDD, 1, 0 },
/* 56 */  { 0xEEEEEEEEEEEEEEEE, 0xEEEEEEEEEEEEEEEE, 1, 0 },
/* 57 */  { 0x0000000100000000, 0x0000000100000000, 1, 0 },
/* 58 */  { 0x0000000000000001, 0x0000000100000000, 0, 1 },
/* 59 */  { 0x0000000100000000, 0x0000000000000001, 0, 1 },
/* 60 */  { 0x1000000000000000, 0x1000000000000000, 1, 0 },
/* 61 */  { 0x0000000000000001, 0x1000000000000000, 0, 1 },
/* 62 */  { 0x1000000000000000, 0x0000000000000001, 0, 1 },
/* 63 */  { 0x0000000000000002, 0x0000000000000002, 1, 0 },
/* 64 */  { 0x0000000000000002, 0x0000000000000003, 0, 1 },
/* 65 */  { 0x0000000000000003, 0x0000000000000002, 0, 1 },
/* 66 */  { 0x7FFFFFFFFFFFF000, 0x7FFFFFFFFFFFF000, 1, 0 },
/* 67 */  { 0x7FFFFFFFFFFFF000, 0x7FFFFFFFFFFFF001, 0, 1 },
/* 68 */  { 0x8000000000000FFF, 0x8000000000000FFF, 1, 0 },
/* 69 */  { 0x8000000000000FFF, 0x8000000000000FFE, 0, 1 },
/* 70 */  { 0x0123456789ABCDEF, 0x0123456789ABCDEF, 1, 0 },
/* 71 */  { 0xFEDCBA9876543210, 0xFEDCBA9876543210, 1, 0 },
/* 72 */  { 0x0123456789ABCDEF, 0xFEDCBA9876543210, 0, 1 },
/* 73 */  { 0xFEDCBA9876543210, 0x0123456789ABCDEF, 0, 1 },
/* 74 */  { 0x0000000000000000, 0xFFFFFFFFFFFFFF00, 0, 1 },
/* 75 */  { 0xFFFFFFFFFFFFFF00, 0x0000000000000000, 0, 1 },
/* 76 */  { 0x000000000000000A, 0x000000000000000A, 1, 0 },
/* 77 */  { 0x000000000000000A, 0x000000000000000B, 0, 1 },
/* 78 */  { 0x000000000000000B, 0x000000000000000A, 0, 1 },
/* 79 */  { 0x0000000000000064, 0x0000000000000064, 1, 0 },
/* 80 */  { 0x00000000000003E8, 0x00000000000003E8, 1, 0 },
/* 81 */  { 0x00000000000003E8, 0x00000000000003E9, 0, 1 },
/* 82 */  { 0x00000000000186A0, 0x00000000000186A0, 1, 0 },
/* 83 */  { 0x00000000000F4240, 0x00000000000F4240, 1, 0 },
/* 84 */  { 0x0000000006989680, 0x0000000006989680, 1, 0 },
/* 85 */  { 0x000000003B9ACA00, 0x000000003B9ACA00, 1, 0 },
/* 86 */  { 0x0000000DE0B6B3A7, 0x0000000DE0B6B3A7, 1, 0 },
/* 87 */  { 0x0000000DE0B6B3A7, 0x0000000DE0B6B3A8, 0, 1 },
/* 88 */  { 0x0000000DE0B6B3A8, 0x0000000DE0B6B3A7, 0, 1 },
/* 89 */  { 0x0000038D7EA4C680, 0x0000038D7EA4C680, 1, 0 },
/* 90 */  { 0x0000E8D4A5100000, 0x0000E8D4A5100000, 1, 0 },
/* 91 */  { 0x0009184E72A00000, 0x0009184E72A00000, 1, 0 },
/* 92 */  { 0x005AF3107A400000, 0x005AF3107A400000, 1, 0 },
/* 93 */  { 0x038D7EA4C6800000, 0x038D7EA4C6800000, 1, 0 },
/* 94 */  { 0x2386F26FC1000000, 0x2386F26FC1000000, 1, 0 },
/* 95 */  { 0xDE0B6B3A76400000, 0xDE0B6B3A76400000, 1, 0 },
/* 96 */  { 0xDE0B6B3A76400000, 0xDE0B6B3A76400001, 0, 1 },
/* 97 */  { 0x8AC7230489E80000, 0x8AC7230489E80000, 1, 0 },
/* 98 */  { 0x8AC7230489E80000, 0x8AC7230489E7FFFF, 0, 1 },
/* 99 */  { 0xFFFFFFFFFFFFFFFE, 0x0000000000000000, 0, 1 },
/* 100 */ { 0x0000000000000000, 0xFFFFFFFFFFFFFFFE, 0, 1 }
};

/* ============================== 100 MULTIPLEXING TEST CASES ============================== */
static const selection_test_case selection_bank[100] = {
/* 1 */   { 0, 0x0000000000000000, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF },
/* 2 */   { 1, 0x0000000000000000, 0xFFFFFFFFFFFFFFFF, 0x0000000000000000 },
/* 3 */   { 5, 0x0000000000000000, 0xFFFFFFFFFFFFFFFF, 0x0000000000000000 },
/* 4 */   { 0, 0x123456789ABCDEF0, 0xFEDCBA9876543210, 0xFEDCBA9876543210 },
/* 5 */   { 1, 0x123456789ABCDEF0, 0xFEDCBA9876543210, 0x123456789ABCDEF0 },
/* 6 */   { 255, 0x123456789ABCDEF0, 0xFEDCBA9876543210, 0x123456789ABCDEF0 },
/* 7 */   { 0, 0x7FFFFFFFFFFFFFFF, 0x8000000000000000, 0x8000000000000000 },
/* 8 */   { 1, 0x7FFFFFFFFFFFFFFF, 0x8000000000000000, 0x7FFFFFFFFFFFFFFF },
/* 9 */   { 0, 0x5555555555555555, 0xAAAAAAAAAAAAAAAA, 0xAAAAAAAAAAAAAAAA },
/* 10 */  { 1, 0x5555555555555555, 0xAAAAAAAAAAAAAAAA, 0x5555555555555555 },
/* 11 */  { 0, 0x0000000000000001, 0x0000000000000000, 0x0000000000000000 },
/* 12 */  { 1, 0x0000000000000001, 0x0000000000000000, 0x0000000000000001 },
/* 13 */  { 0, 0x0000000000000000, 0x0000000000000001, 0x0000000000000001 },
/* 14 */  { 1, 0x0000000000000000, 0x0000000000000001, 0x0000000000000000 },
/* 15 */  { 0, 0xFFFFFFFFFFFFFFFE, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF },
/* 16 */  { 1, 0xFFFFFFFFFFFFFFFE, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFE },
/* 17 */  { 0, 0x1111111111111111, 0x2222222222222222, 0x2222222222222222 },
/* 18 */  { 1, 0x1111111111111111, 0x2222222222222222, 0x1111111111111111 },
/* 19 */  { 0, 0x3333333333333333, 0x4444444444444444, 0x4444444444444444 },
/* 20 */  { 1, 0x3333333333333333, 0x4444444444444444, 0x3333333333333333 },
/* 21 */  { 0, 0x6666666666666666, 0x7777777777777777, 0x7777777777777777 },
/* 22 */  { 1, 0x6666666666666666, 0x7777777777777777, 0x6666666666666666 },
/* 23 */  { 0, 0x8888888888888888, 0x9999999999999999, 0x9999999999999999 },
/* 24 */  { 1, 0x8888888888888888, 0x9999999999999999, 0x8888888888888888 },
/* 25 */  { 0, 0xBBBBBBBBBBBBBBBB, 0xCCCCCCCCCCCCCCCC, 0xCCCCCCCCCCCCCCCC },
/* 26 */  { 1, 0xBBBBBBBBBBBBBBBB, 0xCCCCCCCCCCCCCCCC, 0xBBBBBBBBBBBBBBBB },
/* 27 */  { 0, 0xDDDDDDDDDDDDDDDD, 0xEEEEEEEEEEEEEEEE, 0xEEEEEEEEEEEEEEEE },
/* 28 */  { 1, 0xDDDDDDDDDDDDDDDD, 0xEEEEEEEEEEEEEEEE, 0xDDDDDDDDDDDDDDDD },
/* 29 */  { 0, 0x0000000100000000, 0x0000000000000001, 0x0000000000000001 },
/* 30 */  { 1, 0x0000000100000000, 0x0000000000000001, 0x0000000100000000 },
/* 31 */  { 0, 0x1000000000000000, 0x0000000000000001, 0x0000000000000001 },
/* 32 */  { 1, 0x1000000000000000, 0x0000000000000001, 0x1000000000000000 },
/* 33 */  { 2, 0x1000000000000000, 0x0000000000000001, 0x1000000000000000 },
/* 34 */  { 0, 0x0000000000000002, 0x0000000000000003, 0x0000000000000003 },
/* 35 */  { 1, 0x0000000000000002, 0x0000000000000003, 0x0000000000000002 },
/* 36 */  { 0, 0x7FFFFFFFFFFFF000, 0x7FFFFFFFFFFFF001, 0x7FFFFFFFFFFFF001 },
/* 37 */  { 1, 0x7FFFFFFFFFFFF000, 0x7FFFFFFFFFFFF001, 0x7FFFFFFFFFFFF000 },
/* 38 */  { 0, 0x8000000000000FFF, 0x8000000000000FFE, 0x8000000000000FFE },
/* 39 */  { 1, 0x8000000000000FFF, 0x8000000000000FFE, 0x8000000000000FFF },
/* 40 */  { 0, 0x0000000000000000, 0xFFFFFFFFFFFFFF00, 0xFFFFFFFFFFFFFF00 },
/* 41 */  { 1, 0x0000000000000000, 0xFFFFFFFFFFFFFF00, 0x0000000000000000 },
/* 42 */  { 0, 0x000000000000000A, 0x000000000000000B, 0x000000000000000B },
/* 43 */  { 1, 0x000000000000000A, 0x000000000000000B, 0x000000000000000A },
/* 44 */  { 0, 0x0000000000000064, 0x00000000000003E8, 0x00000000000003E8 },
/* 45 */  { 1, 0x0000000000000064, 0x00000000000003E8, 0x0000000000000064 },
/* 46 */  { 0, 0x00000000000186A0, 0x00000000000F4240, 0x00000000000F4240 },
/* 47 */  { 1, 0x00000000000186A0, 0x00000000000F4240, 0x00000000000186A0 },
/* 48 */  { 0, 0x0000000006989680, 0x000000003B9ACA00, 0x000000003B9ACA00 },
/* 49 */  { 1, 0x0000000006989680, 0x000000003B9ACA00, 0x0000000006989680 },
/* 50 */  { 0, 0x0000000DE0B6B3A7, 0x0000038D7EA4C680, 0x0000038D7EA4C680 },
/* 51 */  { 1, 0x0000000DE0B6B3A7, 0x0000038D7EA4C680, 0x0000000DE0B6B3A7 },
/* 52 */  { 0, 0x0000E8D4A5100000, 0x0009184E72A00000, 0x0009184E72A00000 },
/* 53 */  { 1, 0x0000E8D4A5100000, 0x0009184E72A00000, 0x0000E8D4A5100000 },
/* 54 */  { 0, 0x005AF3107A400000, 0x038D7EA4C6800000, 0x038D7EA4C6800000 },
/* 55 */  { 1, 0x005AF3107A400000, 0x038D7EA4C6800000, 0x005AF3107A400000 },
/* 56 */  { 0, 0x2386F26FC1000000, 0xDE0B6B3A76400000, 0xDE0B6B3A76400000 },
/* 57 */  { 1, 0x2386F26FC1000000, 0xDE0B6B3A76400000, 0x2386F26FC1000000 },
/* 58 */  { 0, 0x8AC7230489E80000, 0xFFFFFFFFFFFFFFFE, 0xFFFFFFFFFFFFFFFE },
/* 59 */  { 1, 0x8AC7230489E80000, 0xFFFFFFFFFFFFFFFE, 0x8AC7230489E80000 },
/* 60 */  { 0, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000 },
/* 61 */  { 1, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000 },
/* 62 */  { 0, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF },
/* 63 */  { 1, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF },
/* 64 */  { 0, 0x0000000000000005, 0x000000000000000A, 0x000000000000000A },
/* 65 */  { 1, 0x0000000000000005, 0x000000000000000A, 0x0000000000000005 },
/* 66 */  { 0, 0x000000000000000A, 0x0000000000000005, 0x0000000000000005 },
/* 67 */  { 1, 0x000000000000000A, 0x0000000000000005, 0x000000000000000A },
/* 68 */  { 0, 0x0000000000000001, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF },
/* 69 */  { 1, 0x0000000000000001, 0xFFFFFFFFFFFFFFFF, 0x0000000000000001 },
/* 70 */  { 0, 0xFFFFFFFFFFFFFFFF, 0x0000000000000001, 0x0000000000000001 },
/* 71 */  { 1, 0xFFFFFFFFFFFFFFFF, 0x0000000000000001, 0xFFFFFFFFFFFFFFFF },
/* 72 */  { 0, 0x0000000000000000, 0x0000000000000002, 0x0000000000000002 },
/* 73 */  { 1, 0x0000000000000000, 0x0000000000000002, 0x0000000000000000 },
/* 74 */  { 0, 0x0000000000000002, 0x0000000000000000, 0x0000000000000000 },
/* 75 */  { 1, 0x0000000000000002, 0x0000000000000000, 0x0000000000000002 },
/* 76 */  { 0, 0x000000000000000C, 0x000000000000000D, 0x000000000000000D },
/* 77 */  { 1, 0x000000000000000C, 0x000000000000000D, 0x000000000000000C },
/* 78 */  { 0, 0x000000000000000D, 0x000000000000000C, 0x000000000000000C },
/* 79 */  { 1, 0x000000000000000D, 0x000000000000000C, 0x000000000000000D },
/* 80 */  { 0, 0x0000000000000014, 0x000000000000001E, 0x000000000000001E },
/* 81 */  { 1, 0x0000000000000014, 0x000000000000001E, 0x0000000000000014 },
/* 82 */  { 0, 0x000000000000001E, 0x0000000000000014, 0x0000000000000014 },
/* 83 */  { 1, 0x000000000000001E, 0x0000000000000014, 0x000000000000001E },
/* 84 */  { 0, 0x0000000000000028, 0x0000000000000032, 0x0000000000000032 },
/* 85 */  { 1, 0x0000000000000028, 0x0000000000000032, 0x0000000000000028 },
/* 86 */  { 0, 0x0000000000000032, 0x0000000000000028, 0x0000000000000028 },
/* 87 */  { 1, 0x0000000000000032, 0x0000000000000028, 0x0000000000000032 },
/* 88 */  { 0, 0x0000000000000046, 0x0000000000000050, 0x0000000000000050 },
/* 89 */  { 1, 0x0000000000000046, 0x0000000000000050, 0x0000000000000046 },
/* 90 */  { 0, 0x0000000000000050, 0x0000000000000046, 0x0000000000000046 },
/* 91 */  { 1, 0x0000000000000050, 0x0000000000000046, 0x0000000000000050 },
/* 92 */  { 0, 0x000000000000005A, 0x0000000000000064, 0x0000000000000064 },
/* 93 */  { 1, 0x000000000000005A, 0x0000000000000064, 0x000000000000005A },
/* 94 */  { 0, 0x0000000000000064, 0x000000000000005A, 0x000000000000005A },
/* 95 */  { 1, 0x0000000000000064, 0x000000000000005A, 0x0000000000000064 },
/* 96 */  { 10, 0x000000000000000A, 0x0000000000000005, 0x000000000000000A },
/* 97 */  { 20, 0x0000000000000005, 0x000000000000000A, 0x0000000000000005 },
/* 98 */  { 30, 0xFFFFFFFFFFFFFFFF, 0x0000000000000001, 0xFFFFFFFFFFFFFFFF },
/* 99 */  { 40, 0x0000000000000001, 0xFFFFFFFFFFFFFFFF, 0x0000000000000001 },
/* 100 */ { 50, 0x0000000000000002, 0x0000000000000000, 0x0000000000000002 }
};

int main(void) {
    size_t passed_tests = 0;
    size_t total_tests = 0;
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    printf("=========================================================\n");
    printf("   RUNNING INTEGRATED UNIT TESTS - GENERAL EQUALITIES    \n");
    printf("=========================================================\n");


    /* ---------------------------------------------------------------------- */
    /* 1. SIGN TESTS (ispos, isneg)                                           */
    /* ---------------------------------------------------------------------- */
    printf("--- Testing ISPOS/ISNEG Architectures ---\n");
    for (size_t i = 0; i < 99; i++) {
        int64_t x = sign_bank[i].x;
        uint8_t ispos_exp = sign_bank[i].ispos_exp;
        uint8_t isneg_exp = sign_bank[i].isneg_exp;
        uint8_t res;

        #if __ARCH_ARM64__
            res = _arm64_crt_ispos(x);
            total_tests++; if (res == ispos_exp) passed_tests++;
            else printf(
              "[FAIL] _arm64_crt_ispos    | Case %3zu | x = 0x%016" PRIX64 
              " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, ispos_exp, res
            );
            res = _arm64_crt_isneg(x);
            total_tests++; if (res == isneg_exp) passed_tests++;
            else printf(
              "[FAIL] _arm64_crt_isneg    | Case %3zu | x = 0x%016" PRIX64 
              " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, isneg_exp, res
            );
        #elif __ARCH_RVI64__
            res = _rv64_crt_ispos(x);
            total_tests++; if (res == ispos_exp) passed_tests++;
            else printf(
              "[FAIL] _rv64_crt_ispos     | Case %3zu | x = 0x%016" PRIX64 
              " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, ispos_exp, res
            );
            res = _rv64_crt_isneg(x);
            total_tests++; if (res == isneg_exp) passed_tests++;
            else printf(
              "[FAIL] _rv64_crt_isneg     | Case %3zu | x = 0x%016" PRIX64 
              " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, isneg_exp, res
            );
        #elif __ARCH_X86_64__
            res = _x86_crt_ispos(x);
            total_tests++; if (res == ispos_exp) passed_tests++;
            else printf(
              "[FAIL] _x86_crt_ispos      | Case %3zu | x = 0x%016" PRIX64 
              " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, ispos_exp, res
            );
            res = _x86_crt_isneg(x);
            total_tests++; if (res == isneg_exp) passed_tests++;
            else printf(
              "[FAIL] _x86_crt_isneg      | Case %3zu | x = 0x%016" PRIX64 
              " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, isneg_exp, res
            );
        #endif

        // C Engine Verification Fallback
        res = _vanillc_crt_ispos(x);
        total_tests++; if (res == ispos_exp) passed_tests++;
        else printf(
          "[FAIL] _vanillc_crt_ispos  | Case %3zu | x = 0x%016" PRIX64 
          " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, ispos_exp, res
        );

        res = _vanillc_crt_isneg(x);
        total_tests++; if (res == isneg_exp) passed_tests++;
        else printf(
          "[FAIL] _vanillc_crt_isneg  | Case %3zu | x = 0x%016" PRIX64 
          " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, (uint64_t)x, isneg_exp, res
        );
    }
    /* ---------------------------------------------------------------------- */
    /* 2. EQUALITY TESTS (eq, neq)                                            */
    /* ---------------------------------------------------------------------- */
    printf("--- Testing EQ/NEG Architectures ---\n");
    for (size_t i = 0; i < 100; i++) {
        uint64_t x = equality_bank[i].x; uint64_t y = equality_bank[i].y;
        uint8_t eq_exp = equality_bank[i].eq_exp;
        uint8_t neq_exp = equality_bank[i].neq_exp;
        uint8_t res;

        #if __ARCH_ARM64__
            res = _arm64_crt_eq(x, y);
            total_tests++; if (res == eq_exp) passed_tests++;
            else printf(
              "[FAIL] _arm64_crt_eq       | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
              " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, eq_exp, res
            );
            res = _arm64_crt_neq(x, y);
            total_tests++; if (res == neq_exp) passed_tests++;
            else printf(
              "[FAIL] _arm64_crt_neq      | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
              " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, neq_exp, res
            );
        #elif __ARCH_RVI64__
            res = _rv64_crt_eq(x, y);
            total_tests++; if (res == eq_exp) passed_tests++;
            else printf(
              "[FAIL] _rv64_crt_eq        | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
              " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, eq_exp, res
            );
            res = _rv64_crt_neq(x, y);
            total_tests++; if (res == neq_exp) passed_tests++;
            else printf(
              "[FAIL] _rv64_crt_neq       | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
              " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, neq_exp, res
            );
        #elif __ARCH_X86_64__
            res = _x86_crt_eq(x, y);
            total_tests++; if (res == eq_exp) passed_tests++;
            else printf(
              "[FAIL] _x86_crt_eq         | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
              " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, eq_exp, res
            );
            res = _x86_crt_neq(x, y);
            total_tests++; if (res == neq_exp) passed_tests++;
            else printf(
              "[FAIL] _x86_crt_neq        | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
              " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, neq_exp, res
            );
        #endif

        // C Engine Verification Fallback
        res = _vanillc_crt_eq(x, y);
        total_tests++; if (res == eq_exp) passed_tests++;
        else printf(
          "[FAIL] _vanillc_crt_eq     | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
          " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, eq_exp, res
        );

        res = _vanillc_crt_neq(x, y);
        total_tests++; if (res == neq_exp) passed_tests++;
        else printf(
          "[FAIL] _vanillc_crt_neq    | Case %3zu | x = 0x%016" PRIX64 " y = 0x%016" PRIX64 
          " | Exp: %" PRIu8 " | Got: %" PRIu8 "\n", i+1, x, y, neq_exp, res
        );
    }
    /* ---------------------------------------------------------------------- */
    /* 3. SELECTION TESTS (select)                                            */
    /* ---------------------------------------------------------------------- */
    printf("--- Testing SELECT Architectures ---\n");
    for (size_t i = 0; i < 100; i++) {
        uint8_t cond = selection_bank[i].cond;
        uint64_t a = selection_bank[i].a;
        uint64_t b = selection_bank[i].b;
        uint64_t select_exp = selection_bank[i].select_exp;
        uint64_t res;

        #if __ARCH_ARM64__
            res = _arm64_crt_select(cond, a, b);
            total_tests++; if (res == select_exp) passed_tests++;
            else printf(
                "[FAIL] _arm64_crt_select   | Case %3zu | c = %" PRIu8 
                " | Exp: 0x%016" PRIX64 " | Got: 0x%016" PRIX64 "\n", i+1, cond, select_exp, res
            );
        #elif __ARCH_RVI64__
            res = _rv64_crt_select(cond, a, b);
            total_tests++; if (res == select_exp) passed_tests++;
            else printf(
                "[FAIL] _rv64_crt_select    | Case %3zu | c = %" PRIu8 
                " | Exp: 0x%016" PRIX64 " | Got: 0x%016" PRIX64 "\n", i+1, cond, select_exp, res
            );
        #elif __ARCH_X86_64__
            res = _x86_crt_select(cond, a, b);
            total_tests++; if (res == select_exp) passed_tests++;
            else printf(
                "[FAIL] _x86_crt_select     | Case %3zu | c = %" PRIu8 
                " | Exp: 0x%016" PRIX64 " | Got: 0x%016" PRIX64 "\n", i+1, cond, select_exp, res
            );
        #endif

        // C Engine Verification Fallback
        res = _vanillc_crt_select(cond, a, b);
        total_tests++; if (res == select_exp) passed_tests++;
        else printf(
            "[FAIL] _vanillc_crt_select | Case %3zu | c = %" PRIu8 
            " | Exp: 0x%016" PRIX64 " | Got: 0x%016" PRIX64 "\n", i+1, cond, select_exp, res
        );
    }


    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("\n==============================================================================\n");
    printf("TEST SUITE EXECUTION SUMMARY\n");
    printf("==============================================================================\n");
    printf("Total Internal Evaluations Run : %3zu\n", total_tests);
    printf("Total Mathematical Assertions Passed: %3zu\n", passed_tests);
    printf("Total Execution Operational Window  : %lf ms\n", elapsed_time);
    printf("==============================================================================\n");

    return (passed_tests == total_tests) ? 0 : 1;
}
