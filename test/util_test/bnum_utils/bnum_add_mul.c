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
typedef struct { const bigInt in; uint64_t prim_op; const bigInt exp; } case_t;
//* ============= GLOBAL ARRAY OF CASES ============= *//
static const case_t add_case[60] = {
    { .in = { .limbs = NULL, .n = 0 }, .prim_op = 0, .exp = { .limbs = NULL, .n = 0 } }, // 0
    { .in = { .limbs = NULL, .n = 0 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){1}, .n = 1 } }, // 1
    { .in = { .limbs = (limb_t[]){1}, .n = 1 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){2}, .n = 1 } }, // 2
    { .in = { .limbs = (limb_t[]){5}, .n = 1 }, .prim_op = 3, .exp = { .limbs = (limb_t[]){8}, .n = 1 } }, // 3
    { .in = { .limbs = (limb_t[]){10}, .n = 1 }, .prim_op = 2, .exp = { .limbs = (limb_t[]){12}, .n = 1 } }, // 4
    { .in = { .limbs = (limb_t[]){100}, .n = 1 }, .prim_op = 7, .exp = { .limbs = (limb_t[]){107}, .n = 1 } }, // 5
    { .in = { .limbs = (limb_t[]){12345}, .n = 1 }, .prim_op = 54321, .exp = { .limbs = (limb_t[]){66666}, .n = 1 } }, // 6
    { .in = { .limbs = (limb_t[]){0x1000}, .n = 1 }, .prim_op = 0x2000, .exp = { .limbs = (limb_t[]){0x3000}, .n = 1 } }, // 7
    { .in = { .limbs = (limb_t[]){1ULL<<32}, .n = 1 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){(1ULL<<32)+1}, .n = 1 } }, // 8
    { .in = { .limbs = (limb_t[]){(1ULL<<32)-1}, .n = 1 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){1ULL<<32}, .n = 1 } }, // 9
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL}, .n = 1 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){0, 1}, .n = 2 } }, // 10
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL}, .n = 1 }, .prim_op = 2, .exp = { .limbs = (limb_t[]){1, 1}, .n = 2 } }, // 11
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFAULL}, .n = 1 }, .prim_op = 6, .exp = { .limbs = (limb_t[]){0, 1}, .n = 2 } }, // 12
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL}, .n = 1 }, .prim_op = 0xFFFFFFFFFFFFFFFFULL, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFEULL, 1}, .n = 2 } }, // 13
    { .in = { .limbs = (limb_t[]){0x7FFFFFFFFFFFFFFFULL}, .n = 1 }, .prim_op = 0x7FFFFFFFFFFFFFFFULL, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFEULL}, .n = 1 } }, // 14
    { .in = { .limbs = (limb_t[]){1}, .n = 1 }, .prim_op = 0xFFFFFFFFFFFFFFFFULL, .exp = { .limbs = (limb_t[]){0, 1}, .n = 2 } }, // 15
    { .in = { .limbs = (limb_t[]){2}, .n = 1 }, .prim_op = 0xFFFFFFFFFFFFFFFFULL, .exp = { .limbs = (limb_t[]){1, 1}, .n = 2 } }, // 16
    { .in = { .limbs = (limb_t[]){10}, .n = 1 }, .prim_op = 0xFFFFFFFFFFFFFFFFULL, .exp = { .limbs = (limb_t[]){9, 1}, .n = 2 } }, // 17
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFF0ULL}, .n = 1 }, .prim_op = 0x10, .exp = { .limbs = (limb_t[]){0, 1}, .n = 2 } }, // 18
    { .in = { .limbs = (limb_t[]){0x1000000000000000ULL}, .n = 1 }, .prim_op = 0x10, .exp = { .limbs = (limb_t[]){0x1000000000000010ULL}, .n = 1 } }, // 19
    { .in = { .limbs = (limb_t[]){0, 1}, .n = 2 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){1, 1}, .n = 2 } }, // 20
    { .in = { .limbs = (limb_t[]){0, 1}, .n = 2 }, .prim_op = 2, .exp = { .limbs = (limb_t[]){2, 1}, .n = 2 } }, // 21
    { .in = { .limbs = (limb_t[]){5, 5}, .n = 2 }, .prim_op = 5, .exp = { .limbs = (limb_t[]){10, 5}, .n = 2 } }, // 22
    { .in = { .limbs = (limb_t[]){0x10, 0x20}, .n = 2 }, .prim_op = 5, .exp = { .limbs = (limb_t[]){0x15, 0x20}, .n = 2 } }, // 23
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 1}, .n = 2 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){0, 2}, .n = 2 } }, // 24
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 1}, .n = 2 }, .prim_op = 2, .exp = { .limbs = (limb_t[]){1, 2}, .n = 2 } }, // 25
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL}, .n = 2 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){0, 0, 1}, .n = 3 } }, // 26
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL}, .n = 2 }, .prim_op = 2, .exp = { .limbs = (limb_t[]){1, 0, 1}, .n = 3 } }, // 27
    { .in = { .limbs = (limb_t[]){1, 1}, .n = 2 }, .prim_op = 0xFFFFFFFFFFFFFFFFULL, .exp = { .limbs = (limb_t[]){0, 2}, .n = 2 } }, // 28
    { .in = { .limbs = (limb_t[]){2, 2}, .n = 2 }, .prim_op = 0xFFFFFFFFFFFFFFFFULL, .exp = { .limbs = (limb_t[]){1, 3}, .n = 2 } }, // 29
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL}, .n = 2 }, .prim_op = 3, .exp = { .limbs = (limb_t[]){2, 0, 1}, .n = 3 } }, // 30
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL}, .n = 2 }, .prim_op = 10, .exp = { .limbs = (limb_t[]){9, 0, 1}, .n = 3 } }, // 31
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFF0ULL, 0xFFFFFFFFFFFFFFFFULL}, .n = 2 }, .prim_op = 0x10, .exp = { .limbs = (limb_t[]){0, 0, 1}, .n = 3 } }, // 32
    { .in = { .limbs = (limb_t[]){1, 0xFFFFFFFFFFFFFFFFULL}, .n = 2 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){2, 0xFFFFFFFFFFFFFFFFULL}, .n = 2 } }, // 5
    { .in = { .limbs = (limb_t[]){1, 0xFFFFFFFFFFFFFFFFULL}, .n = 2 }, .prim_op = 2, .exp = { .limbs = (limb_t[]){3, 0xFFFFFFFFFFFFFFFFULL}, .n = 2 } }, // 34
    { .in = { .limbs = (limb_t[]){0, 0x7FFFFFFFFFFFFFFFULL}, .n = 2 }, .prim_op = 2, .exp = { .limbs = (limb_t[]){2, 0x7FFFFFFFFFFFFFFFULL}, .n = 2 } }, // 35
    { .in = { .limbs = (limb_t[]){0, 0xFFFFFFFFFFFFFFFFULL}, .n = 2 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){1, 0xFFFFFFFFFFFFFFFFULL}, .n = 2 } }, // 36
    { .in = { .limbs = (limb_t[]){0, 0xFFFFFFFFFFFFFFFFULL}, .n = 2 }, .prim_op = 0xFFFFFFFFFFFFFFFFULL, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL}, .n = 2 } }, // 37
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0x1000ULL}, .n = 2 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){0, 0x1001ULL}, .n = 2 } }, // 38
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0x1000ULL}, .n = 2 }, .prim_op = 2, .exp = { .limbs = (limb_t[]){1, 0x1001ULL}, .n = 2 } }, // 39
    { .in = { .limbs = (limb_t[]){0, 0, 1}, .n = 3 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){1, 0, 1}, .n = 3 } }, // 40
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL}, .n = 3 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){0, 0, 0, 1}, .n = 4 } }, // 41
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL}, .n = 3 }, .prim_op = 2, .exp = { .limbs = (limb_t[]){1, 0, 0, 1}, .n = 4 } }, // 42
    { .in = { .limbs = (limb_t[]){1, 2, 3}, .n = 3 }, .prim_op = 4, .exp = { .limbs = (limb_t[]){5, 2, 3}, .n = 3 } }, // 43
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0, 0xFFFFFFFFFFFFFFFFULL}, .n = 3 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){0, 1, 0xFFFFFFFFFFFFFFFFULL}, .n = 3 } }, // 44
    { .in = { .limbs = (limb_t[]){0, 0xFFFFFFFFFFFFFFFFULL, 0}, .n = 2 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){1, 0xFFFFFFFFFFFFFFFFULL}, .n = 2 } }, // 45
    { .in = { .limbs = (limb_t[]){0, 0, 0xFFFFFFFFFFFFFFFFULL}, .n = 3 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){1, 0, 0xFFFFFFFFFFFFFFFFULL}, .n = 3 } }, // 46
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL, 0}, .n = 2 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){0, 0, 1}, .n = 3 } }, // 47
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFF0ULL, 0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL}, .n = 3 }, .prim_op = 0x10, .exp = { .limbs = (limb_t[]){0, 0, 0, 1}, .n = 4 } }, // 48
    { .in = { .limbs = (limb_t[]){1, 1, 1}, .n = 3 }, .prim_op = 0xFFFFFFFFFFFFFFFFULL, .exp = { .limbs = (limb_t[]){0, 2, 1}, .n = 3 } }, // 49
    { .in = { .limbs = (limb_t[]){1}, .n = 1 }, .prim_op = 0, .exp = { .limbs = (limb_t[]){1}, .n = 1 } }, // 50
    { .in = { .limbs = (limb_t[]){1, 1}, .n = 2 }, .prim_op = 0, .exp = { .limbs = (limb_t[]){1, 1}, .n = 2 } }, // 51
    { .in = { .limbs = (limb_t[]){0x5555555555555555ULL}, .n = 1 }, .prim_op = 3, .exp = { .limbs = (limb_t[]){0x5555555555555558ULL}, .n = 1 } }, // 52
    { .in = { .limbs = (limb_t[]){0xAAAAAAAAAAAAAAAAULL}, .n = 1 }, .prim_op = 3, .exp = { .limbs = (limb_t[]){0xAAAAAAAAAAAAAAADULL}, .n = 1 } }, // 53
    { .in = { .limbs = (limb_t[]){0x3333333333333333ULL}, .n = 1 }, .prim_op = 5, .exp = { .limbs = (limb_t[]){0x3333333333333338ULL}, .n = 1 } }, // 54
    { .in = { .limbs = (limb_t[]){0x1C71C71C71C71C71ULL}, .n = 1 }, .prim_op = 9, .exp = { .limbs = (limb_t[]){0x1C71C71C71C71C7AULL}, .n = 1 } }, // 55
    { .in = { .limbs = (limb_t[]){1ULL << 63}, .n = 1 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){(1ULL << 63) + 1}, .n = 1 } }, // 56
    { .in = { .limbs = (limb_t[]){1ULL << 63}, .n = 1 }, .prim_op = 2, .exp = { .limbs = (limb_t[]){(1ULL << 63) + 2}, .n = 1 } }, // 57
    { .in = { .limbs = (limb_t[]){0, 1ULL << 63}, .n = 2 }, .prim_op = 2, .exp = { .limbs = (limb_t[]){2, 1ULL << 63}, .n = 2 } }, // 58
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL}, .n = 4 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){0, 0, 0, 0, 1}, .n = 5 } } // 59
};
static const case_t mul_case[60] = {
    { .in = { .limbs = NULL, .n = 0 }, .prim_op = 0, .exp = { .limbs = NULL, .n = 0 } }, // 0
    { .in = { .limbs = NULL, .n = 0 }, .prim_op = 1, .exp = { .limbs = NULL, .n = 0 } }, // 1
    { .in = { .limbs = (limb_t[]){1}, .n = 1 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){1}, .n = 1 } }, // 2
    { .in = { .limbs = (limb_t[]){5}, .n = 1 }, .prim_op = 3, .exp = { .limbs = (limb_t[]){15}, .n = 1 } }, // 3
    { .in = { .limbs = (limb_t[]){10}, .n = 1 }, .prim_op = 2, .exp = { .limbs = (limb_t[]){20}, .n = 1 } }, // 4
    { .in = { .limbs = (limb_t[]){100}, .n = 1 }, .prim_op = 7, .exp = { .limbs = (limb_t[]){700}, .n = 1 } }, // 5
    { .in = { .limbs = (limb_t[]){12345}, .n = 1 }, .prim_op = 54321, .exp = { .limbs = (limb_t[]){670592745}, .n = 1 } }, // 6
    { .in = { .limbs = (limb_t[]){0x1000}, .n = 1 }, .prim_op = 0x2000, .exp = { .limbs = (limb_t[]){0x2000000}, .n = 1 } }, // 7
    { .in = { .limbs = (limb_t[]){1ULL<<32}, .n = 1 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){1ULL<<32}, .n = 1 } }, // 8
    { .in = { .limbs = (limb_t[]){(1ULL<<32)-1}, .n = 1 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){(1ULL<<32)-1}, .n = 1 } }, // 9
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL}, .n = 1 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL}, .n = 1 } }, // 10
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL}, .n = 1 }, .prim_op = 2, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFEULL, 1}, .n = 2 } }, // 11
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFAULL}, .n = 1 }, .prim_op = 6, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFDCULL, 5}, .n = 2 } }, // 12
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL}, .n = 1 }, .prim_op = 0xFFFFFFFFFFFFFFFFULL, .exp = { .limbs = (limb_t[]){1, 0xFFFFFFFFFFFFFFFEULL}, .n = 2 } }, // 13
    { .in = { .limbs = (limb_t[]){0x7FFFFFFFFFFFFFFFULL}, .n = 1 }, .prim_op = 0x7FFFFFFFFFFFFFFFULL, .exp = { .limbs = (limb_t[]){1, 0x3FFFFFFFFFFFFFFFULL}, .n = 2 } }, // 14
    { .in = { .limbs = (limb_t[]){1}, .n = 1 }, .prim_op = 0xFFFFFFFFFFFFFFFFULL, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL}, .n = 1 } }, // 15
    { .in = { .limbs = (limb_t[]){2}, .n = 1 }, .prim_op = 0xFFFFFFFFFFFFFFFFULL, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFEULL, 1}, .n = 2 } }, // 16
    { .in = { .limbs = (limb_t[]){10}, .n = 1 }, .prim_op = 0xFFFFFFFFFFFFFFFFULL, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFF6ULL, 9}, .n = 2 } }, // 17
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFF0ULL}, .n = 1 }, .prim_op = 0x10, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFF00ULL, 0xF}, .n = 2 } }, // 18
    { .in = { .limbs = (limb_t[]){0x1000000000000000ULL}, .n = 1 }, .prim_op = 0x10, .exp = { .limbs = (limb_t[]){0, 1}, .n = 2 } }, // 19
    { .in = { .limbs = (limb_t[]){0, 1}, .n = 2 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){0, 1}, .n = 2 } }, // 20
    { .in = { .limbs = (limb_t[]){0, 1}, .n = 2 }, .prim_op = 2, .exp = { .limbs = (limb_t[]){0, 2}, .n = 2 } }, // 21
    { .in = { .limbs = (limb_t[]){5, 5}, .n = 2 }, .prim_op = 5, .exp = { .limbs = (limb_t[]){25, 25}, .n = 2 } }, // 22
    { .in = { .limbs = (limb_t[]){0x10, 0x20}, .n = 2 }, .prim_op = 5, .exp = { .limbs = (limb_t[]){0x50, 0xA0}, .n = 2 } }, // 23
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 1}, .n = 2 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 1}, .n = 2 } }, // 24
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 1}, .n = 2 }, .prim_op = 2, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFEULL, 3}, .n = 2 } }, // 25
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL}, .n = 2 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL}, .n = 2 } }, // 26
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL}, .n = 2 }, .prim_op = 2, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFEULL, 0xFFFFFFFFFFFFFFFFULL, 1}, .n = 3 } }, // 27
    { .in = { .limbs = (limb_t[]){1, 1}, .n = 2 }, .prim_op = 0xFFFFFFFFFFFFFFFFULL, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL}, .n = 2 } }, // 28
    { .in = { .limbs = (limb_t[]){2, 2}, .n = 2 }, .prim_op = 0xFFFFFFFFFFFFFFFFULL, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFEULL, 0xFFFFFFFFFFFFFFFFULL, 1}, .n = 3 } }, // 29
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL}, .n = 2 }, .prim_op = 3, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFDULL, 0xFFFFFFFFFFFFFFFFULL, 2}, .n = 3 } }, // 30
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL}, .n = 2 }, .prim_op = 10, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFF6ULL, 0xFFFFFFFFFFFFFFFFULL, 9}, .n = 3 } }, // 31
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFF0ULL, 0xFFFFFFFFFFFFFFFFULL}, .n = 2 }, .prim_op = 0x10, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFF00ULL, 0xFFFFFFFFFFFFFFFFULL, 0xFULL}, .n = 3 } }, // 32
    { .in = { .limbs = (limb_t[]){1, 0xFFFFFFFFFFFFFFFFULL}, .n = 2 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){1, 0xFFFFFFFFFFFFFFFFULL}, .n = 2 } }, // 5
    { .in = { .limbs = (limb_t[]){1, 0xFFFFFFFFFFFFFFFFULL}, .n = 2 }, .prim_op = 2, .exp = { .limbs = (limb_t[]){2, 0xFFFFFFFFFFFFFFFEULL, 1}, .n = 3 } }, // 34
    { .in = { .limbs = (limb_t[]){0, 0x7FFFFFFFFFFFFFFFULL}, .n = 2 }, .prim_op = 2, .exp = { .limbs = (limb_t[]){0, 0xFFFFFFFFFFFFFFFEULL}, .n = 2 } }, // 35
    { .in = { .limbs = (limb_t[]){0, 0xFFFFFFFFFFFFFFFFULL}, .n = 2 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){0, 0xFFFFFFFFFFFFFFFFULL}, .n = 2 } }, // 36
    { .in = { .limbs = (limb_t[]){0, 0xFFFFFFFFFFFFFFFFULL}, .n = 2 }, .prim_op = 0xFFFFFFFFFFFFFFFFULL, .exp = { .limbs = (limb_t[]){0, 1, 0xFFFFFFFFFFFFFFFEULL}, .n = 3 } }, // 37
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0x1000ULL}, .n = 2 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0x1000ULL}, .n = 2 } }, // 38
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0x1000ULL}, .n = 2 }, .prim_op = 2, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFEULL, 0x2001ULL}, .n = 2 } }, // 39
    { .in = { .limbs = (limb_t[]){0, 0, 1}, .n = 3 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){0, 0, 1}, .n = 3 } }, // 40
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL}, .n = 3 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL}, .n = 3 } }, // 41
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL}, .n = 3 }, .prim_op = 2, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFEULL, 0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL, 1}, .n = 4 } }, // 42
    { .in = { .limbs = (limb_t[]){1, 2, 3}, .n = 3 }, .prim_op = 4, .exp = { .limbs = (limb_t[]){4, 8, 12}, .n = 3 } }, // 43
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0, 0xFFFFFFFFFFFFFFFFULL}, .n = 3 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0, 0xFFFFFFFFFFFFFFFFULL}, .n = 3 } }, // 44
    { .in = { .limbs = (limb_t[]){0, 0xFFFFFFFFFFFFFFFFULL, 0}, .n = 2 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){0, 0xFFFFFFFFFFFFFFFFULL}, .n = 2 } }, // 45
    { .in = { .limbs = (limb_t[]){0, 0, 0xFFFFFFFFFFFFFFFFULL}, .n = 3 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){0, 0, 0xFFFFFFFFFFFFFFFFULL}, .n = 3 } }, // 46
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL, 0}, .n = 2 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL}, .n = 2 } }, // 47
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFF0ULL, 0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL}, .n = 3 }, .prim_op = 0x10, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFF00ULL, 0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL, 15}, .n = 4 } }, // 48
    { .in = { .limbs = (limb_t[]){1, 1, 1}, .n = 3 }, .prim_op = 0xFFFFFFFFFFFFFFFFULL, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL}, .n = 3 } }, // 49
    { .in = { .limbs = (limb_t[]){1}, .n = 1 }, .prim_op = 0, .exp = { .limbs = NULL, .n = 0 } }, // 50
    { .in = { .limbs = (limb_t[]){1, 1}, .n = 2 }, .prim_op = 0, .exp = { .limbs = NULL, .n = 0 } }, // 51
    { .in = { .limbs = (limb_t[]){0x5555555555555555ULL}, .n = 1 }, .prim_op = 3, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL}, .n = 1 } }, // 52
    { .in = { .limbs = (limb_t[]){0xAAAAAAAAAAAAAAAAULL}, .n = 1 }, .prim_op = 3, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFEULL, 1}, .n = 2 } }, // 53
    { .in = { .limbs = (limb_t[]){0x3333333333333333ULL}, .n = 1 }, .prim_op = 5, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL}, .n = 1 } }, // 54
    { .in = { .limbs = (limb_t[]){0x1C71C71C71C71C71ULL}, .n = 1 }, .prim_op = 9, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFF9ULL}, .n = 1 } }, // 55
    { .in = { .limbs = (limb_t[]){1ULL << 63}, .n = 1 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){1ULL << 63}, .n = 1 } }, // 56
    { .in = { .limbs = (limb_t[]){1ULL << 63}, .n = 1 }, .prim_op = 2, .exp = { .limbs = (limb_t[]){0, 1}, .n = 2 } }, // 57
    { .in = { .limbs = (limb_t[]){0, 1ULL << 63}, .n = 2 }, .prim_op = 2, .exp = { .limbs = (limb_t[]){0, 0, 1}, .n = 3 } }, // 58
    { .in = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL}, .n = 4 }, .prim_op = 1, .exp = { .limbs = (limb_t[]){0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL}, .n = 4 } } // 59
};


int main(void) {
    _libdnml_init();
    int total_tests = 0, passed_tests = 0;
    struct timespec start, end; clock_gettime(CLOCK_MONOTONIC, &start);
    limb_t *ret_buf = (limb_t *)malloc(12 * U64_BYTES); 
    if (!ret_buf) return 1;
    printf("====================================================================\n");
    printf("     RUNNING INTEGRATED UNIT TESTS - BIGNUM LIMB-SHIFT UTILITIES    \n");
    printf("====================================================================\n");
    printf("---- __BIGINT_INTERNAL_ADD_UI64__ -----\n");
    for (int i = 0; i < 60; i++) { total_tests++; 
        memset(ret_buf, 0, 12 * U64_BYTES);
        if (add_case[i].in.limbs && add_case[i].in.n > 0) {
            memcpy(ret_buf, add_case[i].in.limbs, add_case[i].in.n * U64_BYTES);
        }
        bigInt test_x; test_x.limbs = ret_buf;
        test_x.n = add_case[i].in.n; test_x.cap = 12;
        __BIGINT_INTERNAL_ADD_UI64__(&test_x, add_case[i].prim_op);
        int match = (test_x.n == add_case[i].exp.n);
        if (match) match = memcmp(test_x.limbs, add_case[i].exp.limbs, add_case[i].exp.n * U64_BYTES) == 0;
        if (match) passed_tests++;
        else printf("[FAIL] ADD Case %2d: Missed structural matching requirements.\n", i);
    }
    printf("---- __BIGINT_INTERNAL_MUL_UI64__ -----\n");
    for (int i = 0; i < 60; i++) { total_tests++; 
        memset(ret_buf, 0, 12 * U64_BYTES);
        if (mul_case[i].in.limbs && mul_case[i].in.n > 0) {
            memcpy(ret_buf, mul_case[i].in.limbs, mul_case[i].in.n * U64_BYTES);
        }
        bigInt test_x; test_x.limbs = ret_buf;
        test_x.n = mul_case[i].in.n; test_x.cap = 12;
        __BIGINT_INTERNAL_MUL_UI64__(&test_x, mul_case[i].prim_op);
        int match = (test_x.n == mul_case[i].exp.n);
        if (match) match = memcmp(test_x.limbs, mul_case[i].exp.limbs, mul_case[i].exp.n * U64_BYTES) == 0;
        if (match) passed_tests++;
        else printf("[FAIL] MUL Case %2d: Missed structural matching requirements.\n", i);
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
