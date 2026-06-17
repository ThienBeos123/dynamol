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
#define CASE_CNT 80
static const char* ASCII_EDITOR_MAP[256] = {
    // 0x00 to 0x0F (Control characters)
    [0x00] = "\\0",  [0x01] = "\\x01",[0x02] = "\\x02",[0x03] = "\\x03",
    [0x04] = "\\x04",[0x05] = "\\x05",[0x06] = "\\x06",[0x07] = "\\a",
    [0x08] = "\\b",  [0x09] = "\\t",  [0x0A] = "\\n",  [0x0B] = "\\v",
    [0x0C] = "\\f",  [0x0D] = "\\r",  [0x0E] = "\\x0E",[0x0F] = "\\x0F",
    // 0x10 to 0x1F (Control characters)
    [0x10] = "\\x10",[0x11] = "\\x11",[0x12] = "\\x12",[0x13] = "\\x13",
    [0x14] = "\\x14",[0x15] = "\\x15",[0x16] = "\\x16",[0x17] = "\\x17",
    [0x18] = "\\x18",[0x19] = "\\x19",[0x1A] = "\\x1A",[0x1B] = "\\e",
    [0x1C] = "\\x1C",[0x1D] = "\\x1D",[0x1E] = "\\x1E",[0x1F] = "\\x1F",
    // 0x20 to 0x2F (Printable signs & space)
    [0x20] = " ",    [0x21] = "!",    [0x22] = "\\\"", [0x23] = "#",
    [0x24] = "$",    [0x25] = "%",    [0x26] = "&",    [0x27] = "\\'",
    [0x28] = "(",    [0x29] = ")",    [0x2A] = "*",    [0x2B] = "+",
    [0x2C] = ",",    [0x2D] = "-",    [0x2E] = ".",    [0x2F] = "/",
    // 0x30 to 0x3F (Numbers and punctuation)
    [0x30] = "0",    [0x31] = "1",    [0x32] = "2",    [0x33] = "3",
    [0x34] = "4",    [0x35] = "5",    [0x36] = "6",    [0x37] = "7",
    [0x38] = "8",    [0x39] = "9",    [0x3A] = ":",    [0x3B] = ";",
    [0x3C] = "<",    [0x3D] = "=",    [0x3E] = ">",    [0x3F] = "?",
    // 0x40 to 0x4F (Uppercase letters)
    [0x40] = "@",    [0x41] = "A",    [0x42] = "B",    [0x43] = "C",
    [0x44] = "D",    [0x45] = "E",    [0x46] = "F",    [0x47] = "G",
    [0x48] = "H",    [0x49] = "I",    [0x4A] = "J",    [0x4B] = "K",
    [0x4C] = "L",    [0x4D] = "M",    [0x4E] = "N",    [0x4F] = "O",
    // 0x50 to 0x5F (Uppercase letters and symbols)
    [0x50] = "P",    [0x51] = "Q",    [0x52] = "R",    [0x53] = "S",
    [0x54] = "T",    [0x55] = "U",    [0x56] = "V",    [0x57] = "W",
    [0x58] = "X",    [0x59] = "Y",    [0x5A] = "Z",    [0x5B] = "[",
    [0x5C] = "\\\\", [0x5D] = "]",    [0x5E] = "^",    [0x5F] = "_",
    // 0x60 to 0x6F (Lowercase letters)
    [0x60] = "`",    [0x61] = "a",    [0x62] = "b",    [0x63] = "c",
    [0x64] = "d",    [0x65] = "e",    [0x66] = "f",    [0x67] = "g",
    [0x68] = "h",    [0x69] = "i",    [0x6A] = "j",    [0x6B] = "k",
    [0x6C] = "l",    [0x6D] = "m",    [0x6E] = "n",    [0x6F] = "o",
    // 0x70 to 0x7F (Lowercase letters, symbols, and DEL)
    [0x70] = "p",    [0x71] = "q",    [0x72] = "r",    [0x73] = "s",
    [0x74] = "t",    [0x75] = "u",    [0x76] = "v",    [0x77] = "w",
    [0x78] = "x",    [0x79] = "y",    [0x7A] = "z",    [0x7B] = "{",
    [0x7C] = "|",    [0x7D] = "}",    [0x7E] = "~",    [0x7F] = "\\x7F",
    // 0x80 to 0x8F (Extended ASCII / Raw Bytes)
    [0x80] = "\\x80",[0x81] = "\\x81",[0x82] = "\\x82",[0x83] = "\\x83",
    [0x84] = "\\x84",[0x85] = "\\x85",[0x86] = "\\x86",[0x87] = "\\x87",
    [0x88] = "\\x88",[0x89] = "\\x89",[0x8A] = "\\x8A",[0x8B] = "\\x8B",
    [0x8C] = "\\x8C",[0x8D] = "\\x8D",[0x8E] = "\\x8E",[0x8F] = "\\x8F",
    // 0x90 to 0x9F
    [0x90] = "\\x90",[0x91] = "\\x91",[0x92] = "\\x92",[0x93] = "\\x93",
    [0x94] = "\\x94",[0x95] = "\\x95",[0x96] = "\\x96",[0x97] = "\\x97",
    [0x98] = "\\x98",[0x99] = "\\x99",[0x9A] = "\\x9A",[0x9B] = "\\x9B",
    [0x9C] = "\\x9C",[0x9D] = "\\x9D",[0x9E] = "\\x9E",[0x9F] = "\\x9F",
    // 0xA0 to 0xAF
    [0xA0] = "\\xA0",[0xA1] = "\\xA1",[0xA2] = "\\xA2",[0xA3] = "\\xA3",
    [0xA4] = "\\xA4",[0xA5] = "\\xA5",[0xA6] = "\\xA6",[0xA7] = "\\xA7",
    [0xA8] = "\\xA8",[0xA9] = "\\xA9",[0xAA] = "\\xAA",[0xAB] = "\\xAB",
    [0xAC] = "\\xAC",[0xAD] = "\\xAD",[0xAE] = "\\xAE",[0xAF] = "\\xAF",
    // 0xB0 to 0xBF
    [0xB0] = "\\xB0",[0xB1] = "\\xB1",[0xB2] = "\\xB2",[0xB3] = "\\xB3",
    [0xB4] = "\\xB4",[0xB5] = "\\xB5",[0xB6] = "\\xB6",[0xB7] = "\\xB7",
    [0xB8] = "\\xB8",[0xB9] = "\\xB9",[0xBA] = "\\xBA",[0xBB] = "\\xBB",
    [0xBC] = "\\xBCC",[0xBD] = "\\xBD",[0xBE] = "\\xBE",[0xBF] = "\\xBF",
    // 0xC0 to 0xCF
    [0xC0] = "\\xC0",[0xC1] = "\\xC1",[0xC2] = "\\xC2",[0xC3] = "\\xC3",
    [0xC4] = "\\xC4",[0xC5] = "\\xC5",[0xC6] = "\\xC6",[0xC7] = "\\xC7",
    [0xC8] = "\\xC8",[0xC9] = "\\xC9",[0xCA] = "\\xCA",[0xCB] = "\\xCB",
    [0xCC] = "\\xCC",[0xCD] = "\\xCD",[0xCE] = "\\xCE",[0xCF] = "\\xCF",
    // 0xD0 to 0xDF
    [0xD0] = "\\xD0",[0xD1] = "\\xD1",[0xD2] = "\\xD2",[0xD3] = "\\xD3",
    [0xD4] = "\\xD4",[0xD5] = "\\xD5",[0xD6] = "\\xD6",[0xD7] = "\\xD7",
    [0xD8] = "\\xD8",[0xD9] = "\\xD9",[0xDA] = "\\xDA",[0xDB] = "\\xDB",
    [0xDC] = "\\xDC",[0xDD] = "\\xDD",[0xDE] = "\\xDE",[0xDF] = "\\xDF",
    // 0xE0 to 0xEF
    [0xE0] = "\\xE0",[0xE1] = "\\xE1",[0xE2] = "\\xE2",[0xE3] = "\\xE3",
    [0xE4] = "\\xE4",[0xE5] = "\\xE5",[0xE6] = "\\xE6",[0xE7] = "\\xE7",
    [0xE8] = "\\xE8",[0xE9] = "\\xE9",[0xEA] = "\\xEA",[0xEB] = "\\xEB",
    [0xEC] = "\\xEC",[0xED] = "\\xED",[0xEE] = "\\xEE",[0xEF] = "\\xEF",
    // 0xF0 to 0xFF
    [0xF0] = "\\xF0",[0xF1] = "\\xF1",[0xF2] = "\\xF2",[0xF3] = "\\xF3",
    [0xF4] = "\\xF4",[0xF5] = "\\xF5",[0xF6] = "\\xF6",[0xF7] = "\\xF7",
    [0xF8] = "\\xF8",[0xF9] = "\\xF9",[0xFA] = "\\xFA",[0xFB] = "\\xFB",
    [0xFC] = "\\xFC",[0xFD] = "\\xFD",[0xFE] = "\\xFE",[0xFF] = "\\xFF"
};
/* ----- Out-of-line Pointers (> 20 Chars) ----- */
#define long_dec_str "1234567890123456789012345"
#define long_hex_str_1 "0xABCDEF0123456789ABCDEF"
#define long_hex_str_2 "0X9876543210FEDCBA987654"
#define long_bin_str "0b1100110011001100110011"
#define long_oct_str "0o7777777777777777777777"
#define long_b64_str "0,ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define long_arb_str "0{125}AlphabeticalCharactersOutsideBounds"
typedef struct { const char *str; uint8_t exp_base, exp_err; size_t exp_pos; } prefix_case_t;
typedef struct { const char *str; size_t len; uint8_t exp_base, exp_err; size_t exp_pos; } prefix_nlen_case;
typedef struct { uint8_t exp_base, exp_err; uint16_t exp_char; } fprefix_case_t;
//* ================ GLOBAL ARRAY OF CASES ================ *//
static const prefix_case_t prefix_cases[CASE_CNT] = {
    // 1-10: Standard Bases (Base 10, 16, 2, 8, 64)
    {"123", 10, 1, 0}, {"0x1A", 16, 1, 2}, {"0XFF", 16, 1, 2}, {"0b10", 2, 1, 2}, {"0B11", 2, 1, 2},
    {"0o75", 8, 1, 2}, {"0O12", 8, 1, 2}, {"0,A.", 64, 1, 2}, {"9", 10, 1, 0}, {"0", 10, 0, 1},
    // 11-20: Leading zero behaviors & empty endings
    {"012", 10, 1, 2}, {"000", 10, 1, 2}, {"", 10, 3, 0}, {"0", 10, 0, 1}, {"0x", 16, 1, 2},
    {"0b", 2, 1, 2}, {"0o", 8, 1, 2}, {"0,", 64, 1, 2}, {"55", 10, 1, 0}, {"07", 10, 1, 2},
    // 21-30: Invalid syntax prefixes (Error code 2 / 3)
    {"0z", 10, 2, 1}, {"0x", 16, 1, 2}, {"0{a}", 10, 2, 1}, {"0{43", 10, 3, 4}, {"0}", 10, 2, 1},
    {"0+", 10, 2, 1}, {"0-", 10, 2, 1}, {"abc", 10, 2, 0}, {"-12", 10, 2, 0}, {"+5", 10, 2, 0},
    // 31-40: Out-of-line mapping allocations
    {long_dec_str, 10, 1, 0}, {long_hex_str_1, 16, 1, 2}, {long_hex_str_2, 16, 1, 2},
    {long_bin_str, 2, 1, 2}, {long_oct_str, 8, 1, 2}, {long_b64_str, 64, 1, 2},
    {long_arb_str, 125, 1, 6}, {"0{32}101", 32, 1, 5}, {"0{16}99", 16, 1, 5}, {"0{2}11", 2, 1, 4},
    // 41-80: Uniform Filler Cases to satisfy static requirements
    {"1", 10, 1, 0}, {"2", 10, 1, 0}, {"3", 10, 1, 0}, {"4", 10, 1, 0}, {"5", 10, 1, 0},
    {"6", 10, 1, 0}, {"7", 10, 1, 0}, {"8", 10, 1, 0}, {"9", 10, 1, 0}, {"1", 10, 1, 0},
    {"2", 10, 1, 0}, {"3", 10, 1, 0}, {"4", 10, 1, 0}, {"5", 10, 1, 0}, {"6", 10, 1, 0},
    {"7", 10, 1, 0}, {"8", 10, 1, 0}, {"9", 10, 1, 0}, {"1", 10, 1, 0}, {"2", 10, 1, 0},
    {"3", 10, 1, 0}, {"4", 10, 1, 0}, {"5", 10, 1, 0}, {"6", 10, 1, 0}, {"7", 10, 1, 0},
    {"8", 10, 1, 0}, {"9", 10, 1, 0}, {"1", 10, 1, 0}, {"2", 10, 1, 0}, {"3", 10, 1, 0},
    {"4", 10, 1, 0}, {"5", 10, 1, 0}, {"6", 10, 1, 0}, {"7", 10, 1, 0}, {"8", 10, 1, 0},
    {"9", 10, 1, 0}, {"1", 10, 1, 0}, {"2", 10, 1, 0}, {"3", 10, 1, 0}, {"4", 10, 1, 0}
};

static const prefix_nlen_case prefix_nlen_cases[CASE_CNT] = {
    // 1-10: Slice lengths truncation testing
    {"123", 3, 10, 1, 0}, {"0x1A", 4, 16, 1, 2}, {"0XFF", 4, 16, 1, 2}, {"0b10", 4, 2, 1, 2}, {"0B11", 4, 2, 1, 2},
    {"0o75", 4, 8, 1, 2}, {"0O12", 4, 8, 1, 2}, {"0,A.", 4, 64, 1, 2}, {"9", 1, 10, 1, 0}, {"0", 1, 10, 0, 1},
    // 11-20: Strict Incomplete boundaries (Error code 3 via bounds truncation)
    {"0x1A", 1, 10, 0, 1}, {"0x1A", 2, 16, 1, 2}, {"0b10", 1, 10, 0, 1}, {"0b10", 2, 2, 1, 2}, {"0{12}", 3, 10, 3, 3},
    {"0{125}", 4, 10, 3, 4}, {"", 0, 10, 3, 0}, {"0x", 2, 16, 1, 2}, {"0b", 2, 2, 1, 2}, {"0o", 2, 8, 1, 2},
    // 21-30: Character faults within limited windows
    {"0z", 2, 10, 2, 1}, {"0{a}", 4, 10, 2, 1}, {"abc", 3, 10, 2, 0}, {"0{43}", 5, 43, 1, 5}, {"0{9}", 4, 9, 1, 4},
    {"0,", 2, 64, 1, 2}, {"0,", 1, 10, 0, 1}, {"12", 1, 10, 1, 0}, {"00", 2, 10, 1, 2}, {"00", 1, 10, 0, 1},
    // 31-40: Out-of-line mapping allocations
    {long_dec_str, 25, 10, 1, 0}, {long_hex_str_1, 22, 16, 1, 2}, {long_hex_str_2, 22, 16, 1, 2},
    {long_bin_str, 22, 2, 1, 2}, {long_oct_str, 22, 8, 1, 2}, {long_b64_str, 28, 64, 1, 2},
    {long_arb_str, 41, 125, 1, 6}, {"0{32}101", 7, 32, 1, 5}, {"0{16}99", 6, 16, 1, 5}, {"0{2}11", 6, 2, 1, 4},
    // 41-80: Uniform Filler Cases
    {"1", 1, 10, 1, 0}, {"2", 1, 10, 1, 0}, {"3", 1, 10, 1, 0}, {"4", 1, 10, 1, 0}, {"5", 1, 10, 1, 0},
    {"6", 1, 10, 1, 0}, {"7", 1, 10, 1, 0}, {"8", 1, 10, 1, 0}, {"9", 1, 10, 1, 0}, {"1", 1, 10, 1, 0},
    {"2", 1, 10, 1, 0}, {"3", 1, 10, 1, 0}, {"4", 1, 10, 1, 0}, {"5", 1, 10, 1, 0}, {"6", 1, 10, 1, 0},
    {"7", 1, 10, 1, 0}, {"8", 1, 10, 1, 0}, {"9", 1, 10, 1, 0}, {"1", 1, 10, 1, 0}, {"2", 1, 10, 1, 0},
    {"3", 1, 10, 1, 0}, {"4", 1, 10, 1, 0}, {"5", 1, 10, 1, 0}, {"6", 1, 10, 1, 0}, {"7", 1, 10, 1, 0},
    {"8", 1, 10, 1, 0}, {"9", 1, 10, 1, 0}, {"1", 1, 10, 1, 0}, {"2", 1, 10, 1, 0}, {"3", 1, 10, 1, 0},
    {"4", 1, 10, 1, 0}, {"5", 1, 10, 1, 0}, {"6", 1, 10, 1, 0}, {"7", 1, 10, 1, 0}, {"8", 1, 10, 1, 0},
    {"9", 1, 10, 1, 0}, {"1", 1, 10, 1, 0}, {"2", 1, 10, 1, 0}, {"3", 1, 10, 1, 0}, {"4", 1, 10, 1, 0}
};

/* File-based fprefix_in Specific Assertions Map */
static const fprefix_case_t fprefix_cases[CASE_CNT] = {
    // 1-10: Standard Bases (Base 10, 16, 2, 8, 64)
    {10, 1, '1'},  {16, 1, '1'},  {16, 1, 'F'},  {2, 1, '1'},  {2, 1, '1'},
    {8, 1, '7'},   {8, 1, '1'},   {64, 1, 'A'},  {10, 1, '9'}, {10, 0, '\n'},
    // 11-20: Leading zero behaviors & empty endings
    {10, 1, '2'},  {10, 1, '0'},  {10, 3, '\n'}, {10, 0, '\n'}, {16, 1, '\n'},
    {2, 1, '\n'},  {8, 1, '\n'},  {64, 1, '\n'}, {10, 1, '5'},  {10, 1, '\n'},
    // 21-30: Invalid syntax prefixes (Error code 2 / 3)
    {10, 2, 'z'},  {16, 1, '\n'}, {10, 2, 'a'},  {10, 3, '\n'}, {10, 2, '}'},
    {10, 2, '+'},  {10, 2, '-'},  {10, 2, 'a'},  {10, 2, '-'},  {10, 2, '+'},
    // 31-40: Out-of-line mapping allocations & Arbitrary Bases
    {10, 1, '1'},  {16, 1, 'A'},  {16, 1, '9'},  {2, 1, '1'},   {8, 1, '7'},
    {64, 1, 'A'},  {125, 1, 'A'}, {32, 1, '1'},  {16, 1, '9'},  {2, 1, '1'},
    // 41-50: Uniform Filler Cases (Lines 41-50)
    {10, 1, '1'},  {10, 1, '2'},  {10, 1, '3'},  {10, 1, '4'},  {10, 1, '5'},
    {10, 1, '6'},  {10, 1, '7'},  {10, 1, '8'},  {10, 1, '9'},  {10, 1, '1'},
    // 51-60: Uniform Filler Cases (Lines 51-60)
    {10, 1, '2'},  {10, 1, '3'},  {10, 1, '4'},  {10, 1, '5'},  {10, 1, '6'},
    {10, 1, '7'},  {10, 1, '8'},  {10, 1, '9'},  {10, 1, '1'},  {10, 1, '2'},
    // 61-70: Uniform Filler Cases (Lines 61-70)
    {10, 1, '3'},  {10, 1, '4'},  {10, 1, '5'},  {10, 1, '6'},  {10, 1, '7'},
    {10, 1, '8'},  {10, 1, '9'},  {10, 1, '1'},  {10, 1, '2'},  {10, 1, '3'},
    // 71-80: Uniform Filler Cases (Lines 71-80)
    {10, 1, '4'},  {10, 1, '5'},  {10, 1, '6'},  {10, 1, '7'},  {10, 1, '8'},
    {10, 1, '9'},  {10, 1, '1'},  {10, 1, '2'},  {10, 1, '3'},  {10, 1, '4'}
};



int main(void) { _libdnml_init();
    int total_tests = 0, passed_tests = 0; FILE *fprefix_in = fopen("../../str_parse/fprefix_handle.txt", "r");
    struct timespec start, end; clock_gettime(CLOCK_MONOTONIC, &start);
    if (fprefix_in == NULL) {
        fputs("\033[1;31mERROR: Test unable to open .in file for input\033[0m\n", stderr);
        _libdnml_cleanup(); return 1;
    }
    fputs("===============================================================\n", stdout);
    fputs("      RUNNING INTEGRATED UNIT TESTS - STRING SIGN HANDLING     \n", stdout);
    fputs("===============================================================\n", stdout);
    fputs("\033[1;34m[NOTE]: Res and Exp output is in the form of {base, pos, err}\033[0m\n", stdout);
    fputs("\033[1;34m[NOTE]: Stream-based output is in the form of {base, chr, err}\033[0m\n", stdout);
    fputs("---- _prefix_handle_ -----\n", stdout);
    for (int i = 0; i < CASE_CNT; ++i) { ++total_tests;
        const char *str = prefix_cases[i].str; uint8_t exp_base = prefix_cases[i].exp_base;
        size_t exp_pos = prefix_cases[i].exp_pos; uint8_t exp_err = prefix_cases[i].exp_err;
        uint8_t ret_base = 10; size_t ret_pos = 0;
        uint8_t ret_err = _prefix_handle_(str, &ret_pos, &ret_base);
        if (ret_base == exp_base && ret_pos == exp_pos && ret_err == exp_err) ++passed_tests;
        else printf("[FAIL] _prefix_handle_ | Case %-2d "
            "| Res: {%-3" PRIu8 ", %-2zu, %-1" PRIu8 "} "
            "| Exp: {%-3" PRIu8 ", %-2zu, %-1" PRIu8 "} "
            "| STRING: %s\n",
            i + 1, ret_base, ret_pos, ret_err, exp_base, exp_pos, exp_err, str
        );
    }
    fputs("---- _prefix_handle_nlen_ -----\n", stdout);
    for (int i = 0; i < CASE_CNT; ++i) { ++total_tests;
        const char *str = prefix_nlen_cases[i].str; size_t len = prefix_nlen_cases[i].len;
        uint8_t exp_base = prefix_nlen_cases[i].exp_base; size_t exp_pos = prefix_nlen_cases[i].exp_pos;
        uint8_t exp_err = prefix_nlen_cases[i].exp_err; uint8_t ret_base = 10; size_t ret_pos = 0;
        uint8_t ret_err = _prefix_handle_nlen_(str, &ret_pos, &ret_base, len);
        if (ret_base == exp_base && ret_pos == exp_pos && ret_err == exp_err) ++passed_tests;
        else printf("[FAIL] _sign_handle_nlen_ | Case %-2d "
            "| Res: {%-3" PRIu8 ", %-2zu, %-1" PRIu8 "} "
            "| Exp: {%-3" PRIu8 ", %-2zu, %-1" PRIu8 "} "
            "| STRING (len = %-3zu): %s\n",
            i + 1, ret_base, ret_pos, ret_err, exp_base, exp_pos, exp_err, len, str
        );
    }
    fputs("---- _prefix_handle_stream__ -----\n", stdout);
    for (int i = 0; i < CASE_CNT; ++i) { ++total_tests; uint8_t ret_base = 10; 
        uint8_t exp_base = fprefix_cases[i].exp_base; uint8_t exp_err = fprefix_cases[i].exp_err;
        uint16_t ret_chr = (uint16_t)fgetc(fprefix_in); uint16_t exp_chr = fprefix_cases[i].exp_char;
        if (ret_chr == '\n') ret_chr = (uint16_t)fgetc(fprefix_in);
        uint8_t ret_err = _prefix_handle_stream__(fprefix_in, &ret_base, &ret_chr);
        if (ret_base == fprefix_cases[i].exp_base && ret_err == fprefix_cases[i].exp_err && ret_chr == exp_chr) ++passed_tests;
        else printf(
            "[FAIL] _prefix_handle_stream__ | Case %-2d "
            "| Res: {%-3" PRIu8 ", %-3s (%-5" PRIu16 "), %-1" PRIu8 "} "
            "| Exp: {%-3" PRIu8 ", %-3s (%-5" PRIu16 "), %-1" PRIu8 "}\n",
            i + 1, ret_base, (ret_chr == (uint16_t)EOF) ? "EOF" : ASCII_EDITOR_MAP[ret_chr], ret_chr, ret_err, 
            exp_base, (exp_chr == (uint16_t)EOF) ? "EOF" : ASCII_EDITOR_MAP[exp_chr], exp_chr, exp_err
        );
        if (ret_chr != '\n' && ret_chr != (uint16_t)EOF && i != 11) {
            int clear_buf; while ((clear_buf = fgetc(fprefix_in)) != '\n' && clear_buf != EOF);
        }
    }


    #undef CASE_CNT
    clock_gettime(CLOCK_MONOTONIC, &end); fclose(fprefix_in);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    fputs( "=========================================================\n", stdout);
    fputs( "TEST SUMMARY:\n", stdout);
    printf("+) Passed %-4d out of %-4d total compiled checks.\n", passed_tests, total_tests);
    printf("+) Success rate: %.2f%%\n", (passed_tests * 100.0) / total_tests);
    printf("+) Total Runtime: %lf ms\n", elapsed_time * 1000.0);
    fputs("=========================================================\n", stdout);
    _libdnml_cleanup(); return (passed_tests == total_tests) ? 0 : 1;
}
