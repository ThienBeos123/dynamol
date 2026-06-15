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
#define TEST_MAX_CAP 64
#define CASE_CNT 80
typedef struct { uint64_t val; size_t start; size_t end; bool noop; } memset_case_t;
typedef struct { size_t start; size_t end; bool noop; } memwipe_case_t;
typedef struct { uint64_t src_val; size_t src_start, src_range; size_t start, end; bool noop; } memcpy_case_t;
typedef struct { uint64_t val; size_t dst_start; size_t src_start; size_t len; bool noop; } memmove_case_t;
//* ============= GLOBAL ARRAY OF CASES ============= */
// __libdnml_smemset_u64() cases
static const memset_case_t memset_cases[CASE_CNT] = {
    // ---- Group 1: Standard Mutations (noop = false, so loop tests for 0) ----
    { 55,  0,  0, false }, { 12,  0,  5, false }, { 99,  0, 63, false }, { 42, 10, 20, false },
    { 77, 32, 63, false }, { 88, 63, 63, false }, { 11,  5,  5, false }, { 22,  1, 62, false },
    { 33,  0, 32, false }, { 44, 32, 32, false }, { 66, 15, 45, false }, { 255, 0, 10, false },
    { 128, 5, 15, false }, { 64, 20, 40, false }, { 1,  45, 55, false }, { 2,   2,  8, false },
    { 3,   9, 19, false }, { 4,  50, 60, false }, { 5,  11, 22, false }, { 6,  33, 44, false },
    
    // ---- Group 2: NOP Operations (noop = true, loop expects values to match val) ----
    { 100,  0,  0, true },  { 101,  0,  5, true },  { 102,  0, 63, true },  { 103, 10, 20, true },
    { 104, 32, 63, true },  { 105, 63, 63, true },  { 106,  5,  5, true },  { 107,  1, 62, true },
    { 108,  0, 32, true },  { 109, 32, 32, true },  { 110, 15, 45, true },  { 111,  0, 10, true },
    { 112,  5, 15, true },  { 113, 20, 40, true },  { 114, 45, 55, true },  { 115,  2,  8, true },
    { 116,  9, 19, true },  { 117, 50, 60, true },  { 118, 11, 22, true },  { 119, 33, 44, true },

    // ---- Group 3: Out of bound intervals & offsets (noop = false) ----
    { 20,   0, 64, false }, { 21,  0, 70, false }, { 22, 10, 80, false }, { 23, 60, 90, false },
    { 24,  64, 64, false }, { 25,  65, 70, false }, { 26, 30, 20, false }, { 27, 50, 10, false },
    { 28,   1,  0, false }, { 29,  63,  0, false }, { 30,  0, 100, false }, { 31, 40, 40, false },
    { 32,  12, 13, false }, { 33,  13, 12, false }, { 34, 55, 56, false }, { 35, 56, 55, false },
    { 36,   5,  6, false }, { 37,   6,  5, false }, { 38, 25, 26, false }, { 39, 26, 25, false },

    // ---- Group 4: Offsets and Mixed Constraints (noop = true) ----
    { 200,  0, 64, true },  { 201,  0, 70, true },  { 202, 10, 80, true },  { 203, 60, 90, true },
    { 204, 64, 64, true },  { 205, 65, 70, true },  { 206, 30, 20, true },  { 207, 50, 10, true },
    { 208,  1,  0, true },  { 209, 63,  0, true },  { 210,  0, 100, true }, { 211, 40, 40, true },
    { 212, 12, 13, true },  { 213, 13, 12, true },  { 214, 55, 56, true },  { 215, 56, 55, true },
    { 216,  5,  6, true },  { 217,  6,  5, true },  { 218, 25, 26, true },  { 219, 26, 25, true }
};
// __libdnml_smemwipe_u64() casees
static const memwipe_case_t memwipe_cases[CASE_CNT] = {
    // ---- Group 1: Standard wipes (noop = false) ----
    {  0,  0, false }, {  0,  5, false }, {  0, 63, false }, { 10, 20, false },
    { 32, 63, false }, { 63, 63, false }, {  5,  5, false }, {  1, 62, false },
    {  0, 32, false }, { 32, 32, false }, { 15, 45, false }, {  0, 10, false },
    {  5, 15, false }, { 20, 40, false }, { 45, 55, false }, {  2,  8, false },
    {  9, 19, false }, { 50, 60, false }, { 11, 22, false }, { 33, 44, false },

    // ---- Group 2: Safe NOP skips (noop = true) ----
    {  0,  0, true },  {  0,  5, true },  {  0, 63, true },  { 10, 20, true },
    { 32, 63, true },  { 63, 63, true },  {  5,  5, true },  {  1, 62, true },
    {  0, 32, true },  { 32, 32, true },  { 15, 45, true },  {  0, 10, true },
    {  5, 15, true },  { 20, 40, true },  { 45, 55, true },  {  2,  8, true },
    {  9, 19, true },  { 50, 60, true },  { 11, 22, true },  { 33, 44, true },

    // ---- Group 3: Upper/Lower Boundary Collisions (noop = false) ----
    {  0, 64, false }, {  0, 70, false }, { 10, 80, false }, { 60, 90, false },
    { 64, 64, false }, { 65, 70, false }, { 30, 20, false }, { 50, 10, false },
    {  1,  0, false }, { 63,  0, false }, {  0, 100, false }, { 40, 40, false },
    { 12, 13, false }, { 13, 12, false }, { 55, 56, false }, { 56, 55, false },
    {  5,  6, false }, {  6,  5, false }, { 25, 26, false }, { 26, 25, false },

    // ---- Group 4: Upper/Lower Boundary Collisions (noop = true) ----
    {  0, 64, true },  {  0, 70, true },  { 10, 80, true },  { 60, 90, true },
    { 64, 64, true },  { 65, 70, true },  { 30, 20, true },  { 50, 10, true },
    {  1,  0, true },  { 63,  0, true },  {  0, 100, true }, { 40, 40, true },
    { 12, 13, true },  { 13, 12, true },  { 55, 56, true },  { 56, 55, true },
    {  5,  6, true },  {  6,  5, true },  { 25, 26, true },  { 26, 25, true }
};
// __libdnml_smemcpy_u64() cases
static const memcpy_case_t memcpy_cases[CASE_CNT] = {
    // ---- Group 1: Matching Indices Configurations (noop = false) ----
    { 10,  0, 64,  0,  0, false }, { 11,  0, 64,  0,  5, false }, { 12,  0, 64,  0, 63, false },
    { 13,  0, 64, 10, 20, false }, { 14,  0, 64, 32, 63, false }, { 15,  0, 64, 63, 63, false },
    { 16,  0, 64,  5,  5, false }, { 17,  0, 64,  1, 62, false }, { 18,  0, 64,  0, 32, false },
    { 19,  0, 64, 32, 32, false }, { 20,  0, 64, 15, 45, false }, { 21,  0, 64,  0, 10, false },
    { 22,  0, 64,  5, 15, false }, { 23,  0, 64, 20, 40, false }, { 24,  0, 64, 45, 55, false },
    { 25,  0, 64,  2,  8, false }, { 26,  0, 64,  9, 19, false }, { 27,  0, 64, 50, 60, false },
    { 28,  0, 64, 11, 22, false }, { 29,  0, 64, 33, 44, false },

    // ---- Group 2: Matching Indices Configurations (noop = true) ----
    { 50,  0, 64,  0,  0, true },  { 51,  0, 64,  0,  5, true },  { 52,  0, 64,  0, 63, true },
    { 53,  0, 64, 10, 20, true },  { 54,  0, 64, 32, 63, true },  { 55,  0, 64, 63, 63, true },
    { 56,  0, 64,  5,  5, true },  { 57,  0, 64,  1, 62, true },  { 58,  0, 64,  0, 32, true },
    { 59,  0, 64, 32, 32, true },  { 60,  0, 64, 15, 45, true },  { 61,  0, 64,  0, 10, true },
    { 62,  0, 64,  5, 15, true },  { 63,  0, 64, 20, 40, true },  { 64,  0, 64, 45, 55, true },
    { 65,  0, 64,  2,  8, true },  { 66,  0, 64,  9, 19, true },  { 67,  0, 64, 50, 60, true },
    { 68,  0, 64, 11, 22, true },  { 69,  0, 64, 33, 44, true },

    // ---- Group 3: Sub-range Source Constraints & Truncations (noop = false) ----
    { 80,  0, 10,  0, 64, false }, { 81,  5, 10,  0, 70, false }, { 82, 20, 10, 10, 80, false },
    { 83,  0, 30, 60, 90, false }, { 84,  0, 64, 64, 64, false }, { 85,  0, 64, 65, 70, false },
    { 86,  0, 40, 30, 20, false }, { 87,  0, 50, 50, 10, false }, { 88,  0,  5,  1,  0, false },
    { 89,  0, 64, 63,  0, false }, { 90,  0, 64,  0, 100, false }, { 91,  0, 64, 40, 40, false },
    { 92,  0, 20, 12, 13, false }, { 93,  0, 20, 13, 12, false }, { 94,  0, 64, 55, 56, false },
    { 95,  0, 64, 56, 55, false }, { 96,  0, 10,  5,  6, false }, { 97,  0, 10,  6,  5, false },
    { 98,  0, 30, 25, 26, false }, { 99,  0, 30, 26, 25, false },

    // ---- Group 4: Sub-range Source Constraints & Truncations (noop = true) ----
    { 80,  0, 10,  0, 64, true },  { 81,  5, 10,  0, 70, true },  { 82, 20, 10, 10, 80, true },
    { 83,  0, 30, 60, 90, true },  { 84,  0, 64, 64, 64, true },  { 85,  0, 64, 65, 70, true },
    { 86,  0, 40, 30, 20, true },  { 87,  0, 50, 50, 10, true },  { 88,  0,  5,  1,  0, true },
    { 89,  0, 64, 63,  0, true },  { 90,  0, 64,  0, 100, true }, { 91,  0, 64, 40, 40, true },
    { 92,  0, 20, 12, 13, true },  { 93,  0, 20, 13, 12, true },  { 94,  0, 64, 55, 56, true },
    { 95,  0, 64, 56, 55, true },  { 96,  0, 10,  5,  6, true },  { 97,  0, 10,  6,  5, true },
    { 98,  0, 30, 25, 26, true },  { 99,  0, 30, 26, 25, true }
};
// __libdnml_smemmove_u64() cases
static const memmove_case_t memmove_cases[CASE_CNT] = {
    // ---- Group 1: Forward Shifts (dst_start < src_start) (noop = false) ----
    { 7,   0, 10,  5, false }, { 8,   2, 12,  8, false }, { 9,   5, 25, 15, false }, { 10, 20, 40, 10, false },
    { 11, 30, 35,  2, false }, { 12,  1,  9,  5, false }, { 13,  0, 32, 16, false }, { 14, 10, 50,  5, false },
    { 15, 40, 50,  8, false }, { 16,  5, 15,  3, false }, { 17, 12, 24,  6, false }, { 18, 15, 45, 10, false },
    { 19,  0, 60,  2, false }, { 20, 25, 50, 12, false }, { 21,  8, 16,  4, false }, { 22,  3,  9,  3, false },
    { 23, 11, 22,  5, false }, { 24, 33, 44,  6, false }, { 25,  2,  8,  4, false }, { 26, 50, 55,  2, false },

    // ---- Group 2: Backward Shifts (dst_start > src_start) (noop = false) ----
    { 77, 10,  0,  5, false }, { 78, 12,  2,  8, false }, { 79, 25,  5, 15, false }, { 80, 40, 20, 10, false },
    { 81, 35, 30,  2, false }, { 82,  9,  1,  5, false }, { 83, 32,  0, 16, false }, { 84, 50, 10,  5, false },
    { 85, 50, 40,  8, false }, { 86, 15,  5,  3, false }, { 87, 24, 12,  6, false }, { 88, 45, 15, 10, false },
    { 89, 60,  0,  2, false }, { 90, 50, 25, 12, false }, { 91, 16,  8,  4, false }, { 92,  9,  3,  3, false },
    { 93, 22, 11,  5, false }, { 94, 44, 33,  6, false }, { 95,  8,  2,  4, false }, { 96, 55, 50,  2, false },

    // ---- Group 3: Overlapping & Full Constraints (noop = true) ----
    { 1,   0, 10,  5, true },  { 2,   2, 12,  8, true },  { 3,   5, 25, 15, true },  { 4,  20, 40, 10, true },
    { 5,  30, 35,  2, true },  { 6,   1,  9,  5, true },  { 7,   0, 32, 16, true },  { 8,  10, 50,  5, true },
    { 9,  40, 50,  8, true },  { 10,  5, 15,  3, true },  { 11, 12, 24,  6, true },  { 12, 15, 45, 10, true },
    { 13,  0, 60,  2, true },  { 14, 25, 50, 12, true },  { 15,  8, 16,  4, true },  { 16,  3,  9,  3, true },
    { 17, 11, 22,  5, true },  { 18, 33, 44,  6, true },  { 19,  2,  8,  4, true },  { 20, 50, 55,  2, true },

    // ---- Group 4: Extreme Out-of-bounds Boundaries & Overlaps (noop = false) ----
    { 120, 0, 60, 20, false }, { 121,60,  0, 20, false }, { 122, 0,  0, 64, false }, { 123, 0,  0,  0, false },
    { 124,63, 63,  1, false }, { 125,64, 64,  5, false }, { 126, 0, 64,  5, false }, { 127,64,  0,  5, false },
    { 128,10, 20, 64, false }, { 129,20, 10, 64, false }, { 130,30, 30,  0, false }, { 131, 5, 10, 80, false },
    { 132, 0,  5,  5, false }, { 133, 5,  0,  5, false }, { 134,45, 40, 15, false }, { 135,40, 45, 15, false },
    { 136, 1, 63, 64, false }, { 137,63,  1, 64, false }, { 138,10, 15,  0, false }, { 139,15, 10,  0, false }
};


static void print_arr(uint64_t *arr, const char *arr_name, FILE *log_path) {
    fprintf(log_path, "%s = {\n", arr_name);
    uint64_t block1, block2, block3, block4;
    uint64_t block5, block6, block7, block8;
    for (size_t i = 0; i < TEST_MAX_CAP; i += 8) {
        block1 = arr[i]; block2 = arr[i + 1]; block3 = arr[i + 2];
        block4 = arr[i + 3]; block5 = arr[i + 4]; block6 = arr[i + 5];
        block7 = arr[i + 6]; block8 = arr[i + 7];
        fprintf(log_path,
            "    %" PRIu64 ", %" PRIu64 ", %" PRIu64 ", %" PRIu64 
            ", %" PRIu64 ", %" PRIu64 ", %" PRIu64 ", %" PRIu64 ",\n",
            block1, block2, block3, block4, block5, block6, block7, block8
        );
    } fputs("}\n", log_path);
}
static uint64_t setup_val(uint8_t pattern) {
    uint64_t ret = pattern; ret <<= U64_BYTES; ret |= pattern;
    ret <<= U64_BYTES; ret |= pattern; ret <<= U64_BYTES; ret |= pattern;
    ret <<= U64_BYTES; ret |= pattern; ret <<= U64_BYTES; ret |= pattern;
    ret <<= U64_BYTES; ret |= pattern; ret <<= U64_BYTES; ret |= pattern;
    return ret;
}


int main(void) { _libdnml_init();
    int total_tests = 0, passed_tests = 0;
    struct timespec start, end; bool pass = false; FILE* log_path;
    clock_gettime(CLOCK_MONOTONIC, &start);
    uint64_t *dst_buf = calloc(TEST_MAX_CAP, sizeof(uint64_t));
    uint64_t *src_buf = calloc(TEST_MAX_CAP, sizeof(uint64_t));
    fputs("===================================================================\n", stdout);
    fputs("       RUNNING INTEGRATED UNIT TESTS - 8-BIT MEMORY UTILITIES      \n", stdout);
    fputs("===================================================================\n", stdout);
    log_path = fopen("../../log/__libdnml_smemset_logs.txt", "w+");
    fputs("---- __libdnml_smemset_u64 -----\n", log_path);
    for (int i = 0; i < CASE_CNT; ++i) { total_tests++;
        memset(dst_buf, 0, TEST_MAX_CAP * U64_BYTES); pass = true;
        uint64_t val = memset_cases[i].val; size_t start = memset_cases[i].start;
        size_t end = memset_cases[i].end; bool noop = memset_cases[i].noop;
        __libdnml_smemset_u64(dst_buf, val, TEST_MAX_CAP, start, end, noop);
        size_t bound = min(end + 1, TEST_MAX_CAP);
        for (size_t j = start; j < bound; ++j) {
            uint64_t chosen_val = (noop) ? 0 : setup_val((uint8_t)val);
            if (dst_buf[j] != chosen_val) { pass = false; break; }
        }
        if (pass) ++passed_tests;
        else {
            fprintf(log_path,
                "[FAIL] __libdnml_smemset_u64 | Case %-2d | val: %-3" PRIu64 
                " Start: %-2zu End: %-2zu Noop: %s\n", i, val, start, end, (noop) ? "True" : "False"
            ); print_arr(dst_buf, "dst_buf", log_path); fputc('\n', log_path);
        }
    } fclose(log_path);
    fputs("__libdnml_smemset_u64: Result written to: ../../log/__libdnml_memwipe_logs.txt\n", stdout);

    log_path = fopen("../../log/__libdnml_smemwipe_logs.txt", "w+");
    fputs("---- __libdnml_smemwipe_u64 -----\n", log_path);
    for (int i = 0; i < CASE_CNT; ++i) { total_tests++;
        memset(dst_buf, UINT8_MAX, TEST_MAX_CAP * U64_BYTES); pass = true;
        size_t start = memset_cases[i].start;
        size_t end = memset_cases[i].end; bool noop = memset_cases[i].noop;
        __libdnml_smemwipe_u64(dst_buf, TEST_MAX_CAP, start, end, noop);
        size_t bound = min(end + 1, TEST_MAX_CAP);
        for (size_t j = start; j < bound; ++j) {
            uint64_t chosen_val = (noop) ? UINT64_MAX : 0;
            if (dst_buf[j] != chosen_val) { pass = false; break; }
        }
        if (pass) ++passed_tests;
        else { 
            fprintf(log_path,
                "[FAIL] __libdnml_smemwipe_u64 | Case %-2d | Start: %-2zu "
                "End: %-2zu Noop: %s\n", i, start, end, (noop) ? "True" : "False"
            ); print_arr(dst_buf, "dst_buf", log_path); fputc('\n', log_path);
        }
    } fclose(log_path);
    fputs("__libdnml_smemwipe_u64: Result written to: ../../log/__libdnml_memwipe_logs.txt\n", stdout);

    log_path = fopen("../../log/__libdnml_smemcpy_logs.txt", "w+");
    fputs("---- __libdnml_smemcpy_u64 -----\n", log_path);
    for (int i = 0; i < CASE_CNT; ++i) { total_tests++; 
        memset(src_buf, 0, TEST_MAX_CAP * U64_BYTES);
        memset(dst_buf, 0, TEST_MAX_CAP * U64_BYTES);
        size_t src_init_len = 0;
        if (memcpy_cases[i].src_start < TEST_MAX_CAP) {
            src_init_len = min(memcpy_cases[i].src_range, TEST_MAX_CAP - memcpy_cases[i].src_start);
            for (size_t k = 0; k < src_init_len; ++k) {
                src_buf[memcpy_cases[i].src_start + k] = (uint64_t)memcpy_cases[i].src_val;
            }
        }
        size_t start = memcpy_cases[i].start, end = memcpy_cases[i].end; bool noop = memcpy_cases[i].noop;
        __libdnml_smemcpy_u64(dst_buf, src_buf, TEST_MAX_CAP, TEST_MAX_CAP, start, end, noop);
        size_t bound = min(end + 1, TEST_MAX_CAP);
        for (size_t j = start; j < bound; ++j) {
            uint64_t chosen_val = (noop) ? 0 : src_buf[j];
            if (dst_buf[j] != chosen_val) { pass = false; break; }
        }
        if (pass) ++passed_tests;
        else {
            fprintf(log_path,
                "[FAIL] __libdnml_smemcpy_u64 | Case %-2d | Start: %-2zu "
                "End: %-2zu, Noop: %s\n", i, start, end, (noop) ? "True" : "False"
            ); print_arr(dst_buf, "dst_buf", log_path); print_arr(src_buf, "src_buf", log_path); fputc('\n', log_path);
        }
    } fclose(log_path);
    fputs("__libdnml_smemcpy_u64: Result written to: ../../log/__libdnml_memcpy_logs.txt\n", stdout);

    log_path = fopen("../../log/__libdnml_smemmove_logs.txt", "w+");
    fputs("---- __libdnml_smemmove_u64 -----\n", log_path);
    for (int i = 0; i < CASE_CNT; ++i) { total_tests++; memset(dst_buf, 0, TEST_MAX_CAP * U64_BYTES);
        size_t dst_start = memmove_cases[i].dst_start, src_start = memmove_cases[i].src_start;
        size_t len = memmove_cases[i].len; bool noop = memmove_cases[i].noop;
        size_t init_len = (dst_start < TEST_MAX_CAP) ? min(len, TEST_MAX_CAP - dst_start) : 0;
        if (init_len) {
            for (size_t k = 0; k < init_len; ++k) {
                dst_buf[dst_start + k] = (uint64_t)memmove_cases[i].val;
            }
        }
        memcpy(src_buf, dst_buf, TEST_MAX_CAP * U64_BYTES); // Reserved as a truth source for NOP case
        __libdnml_smemmove_u64(dst_buf, TEST_MAX_CAP, dst_start, src_start, len, noop);
        size_t moved_len = min(min(len, TEST_MAX_CAP - src_start), TEST_MAX_CAP - dst_start);
        if (noop) for (size_t j = 0; j < TEST_MAX_CAP; ++j) { if (dst_buf[j] != src_buf[j]) { pass = false; break; } }
        else for (size_t j = 0; j < moved_len; ++j) { 
            if (dst_buf[dst_start + j] != src_buf[src_start + j]) { pass = false; break; }
        }
        if (pass) ++passed_tests;
        else {
            fprintf(log_path,
                "[FAIL] __libdnml_smemmove_u64 | Case %-2d | Val: %-3" PRIu64
                " Dst_Start: %-2zu Src_Start: %-2zu Len: %-2zu Noop: %s\n", 
                i, memmove_cases[i].val, dst_start, src_start, len, noop ? "True" : "False"
            ); print_arr(dst_buf, "dst_buf", log_path); fputc('\n', log_path);
        }
    } fclose(log_path);
    fputs("__libdnml_smemmove_u64: Result written to: ../../log/__libdnml_memmove_logs.txt\n", stdout);

    
    #undef TEST_MAX_CAP
    #undef CASE_CNT
    free(dst_buf), free(src_buf); clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("=========================================================\n");
    printf("TEST SUMMARY:\n");
    printf("+) Passed %-4d out of %-4d total compiled checks.\n", passed_tests, total_tests);
    printf("+) Success rate: %.2f%%\n", (passed_tests * 100.0) / total_tests);
    printf("+) Total Runtime: %lf ms\n", elapsed_time * 1000.0);
    printf("=========================================================\n");
    _libdnml_cleanup(); return (passed_tests == total_tests) ? 0 : 1;
}
