#ifndef _LOW_SETUP_H
#define _LOW_SETUP_H


#include "../../test_ui/lowui.h"
// STDLIB utilities
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define case _libdnml_case
#define pair _dnml_pair
#define u64 uint64_t
#define u32 uint32_t
#define u16 uint16_t
#define u8 uint8_t

#define U8m UINT8_MAX
#define U16m UINT16_MAX
#define U32m UINT32_MAX
#define U31m (U32m >> 1)

#define U64m UINT64_MAX
#define U63m (U64m >> 1)
#define U62m (U64m >> 2)

#define alt64_1 0xAAAAAAAAAAAAAAAA
#define alt64_2 0x5555555555555555
#define dalt    0x3333333333333333
#define half64_1 0x00000000FFFFFFFF
#define half64_2 0xFFFFFFFF00000000


//* ======================================= *//
//* ========= GENERIC SETUP MACRO ========= *//
//* ======================================= *//
#define _DNML_INFER_STYLEE(fn) _Generic((fn),   \
    _fn1o_t: DNML_OCALL,                        \
    _fn2o_t: DNML_OCALL,                        \
    _fn3o_t: DNML_OCALL,                        \
    default: DNML_CALL                          \
)

#define _DNML_SUITE_SETUP( \
    suite, name,                \
    ecount, rcount, rin,        \
    edge, rand, ribuf, resbuf,  \
    test_fn, ref_fn, logp       \
) do { create_lsuite( \
    suite, name,            \
    ecount, rcount, rin,    \
    edge, rand,             \
    ribuf, resbuf, logp     \
); (suite)->fn_test = (void*)(test_fn);  \
(suite)->fn_ref    = (void*)(ref_fn);   \
(suite)->call_style = _DNML_INFER_STYLE(test_fn); \
} while (0); \

u64 barebone_rand(void) {
    u64 res = 0;
    for (int i = 0; i < 5; i++) {
        res = (res << 15) | (rand() & 0x7FFF);
    } return res;
}
void _suite_slices(
    void** slices, void* storage, 
    size_t elements_per_suite, size_t element_size, 
    u8 suite_count
) {
    for (size_t i = 0; i < suite_count; ++i) {
        size_t increments = i * elements_per_suite * element_size;
        slices[i] = (void*)((char*)storage + increments);
    } return;
}




//* ======================================= *//
//* ======= ARITHMETIC SUITE SETUP ======== *//
//* ======================================= *//
static inline void addc_setup(
    _libdnml_lsuite *s, const char *name,
    case *ecases, case *rcases, u32 rcount, 
    u64 **ribuf, pair *resbuf, const char *addclp,
    u64 (*test_fn)(u64, u64, u8*),
    u64 (*ref_fn)(u64, u64, u8*)
) { 
    srand(time(NULL)); static u8 addc_in = 2;
    // Basic Tests
    ecases[0] = (case){ .in = {0, 0, 0}, .exp = {0, 0}, .inc = addc_in };
    ecases[1] = (case){ .in = {0, 0, 1}, .exp = {1, 0}, .inc = addc_in };
    ecases[2] = (case){ .in = {1, 2, 0}, .exp = {3, 0}, .inc = addc_in };
    ecases[3] = (case){ .in = {1, 2, 1}, .exp = {4, 0}, .inc = addc_in };
    // Rollover/Edge tests
    ecases[4] = (case){ .in = {U64m, 0, 0}, .exp = {U64m, 0}, .inc = addc_in };
    ecases[5] = (case){ .in = {U64m, 1, 0}, .exp = {0, 1}, .inc = addc_in };
    ecases[6] = (case){ .in = {U64m, 0, 1}, .exp = {0, 1}, .inc = addc_in };
    ecases[7] = (case){ .in = {U64m-1, 0, 0}, .exp = {U64m, 0}, .inc = addc_in };
    ecases[8] = (case){ .in = {U64m, 1, 1}, .exp = {1, 1}, .inc = addc_in };
    ecases[9] = (case){ .in = {1, U64m, 1}, .exp = {1, 1}, .inc = addc_in };
    ecases[10] = (case){ .in = {U64m, U64m, 0}, .exp = {U64m-1, 1}, .inc = addc_in };
    ecases[11] = (case){ .in = {U64m, U64m, 1}, .exp = {U64m,   1}, .inc = addc_in };
    // Bitwise Activity tests
    ecases[12] = (case){ .in = {alt64_1, alt64_2, 0},   .exp = {U64m, 0}, .inc = addc_in };
    ecases[13] = (case){ .in = {alt64_1, alt64_2, 1},   .exp = {0, 1}, .inc = addc_in };
    ecases[14] = (case){ .in = {half64_1, half64_2, 0}, .exp = {U64m, 0}, .inc = addc_in };
    ecases[15] = (case){ .in = {half64_1, half64_2, 1}, .exp = {0, 1}, .inc = addc_in };
    ecases[16] = (case){ .in = {U64m>>1, U64m>>1, 0},       .exp = {0, 1}, .inc = addc_in };
    // Signed Arithmetic overlap / ALU tests
    ecases[17] = (case){ .in = {alt64_1, alt64_2, 1}, .exp = {0, 1}, .inc = addc_in };
    ecases[18] = (case){ .in = {5,       U64m-4,  0}, .exp = {0, 1}, .inc = addc_in };
    ecases[19] = (case){ .in = {U64m>>1, 0, 1}, .exp = {U64m>>1+1, 0}, .inc = addc_in };
    // Randomly Generated Cases filling
    for (u32 i = 0; i < rcount; ++i) {
        u64 in3 = (barebone_rand() & 1) ? 1 : 0;
        rcases[i] = (case){ 
            .in = {barebone_rand(), barebone_rand(), in3}, 
            .inc = addc_in 
        };
    } _DNML_SUITE_SETUP(s, name, 20, rcount, 
        addc_in, ecases, rcases, ribuf,
        resbuf, test_fn, ref_fn, addclp
    ); return;
}
static inline void subb_setup(
    _libdnml_lsuite *s, const char *name,
    case *ecases, case *rcases, u32 rcount, 
    u64 **ribuf, pair *resbuf, const char *subblp,
    u64 (*test_fn)(u64, u64, u8*),
    u64 (*ref_fn)(u64, u64, u8*)
) { 
    srand(time(NULL)); static u8 subb_in = 2;
    // Basic Tests
    ecases[0] = (case){ .in = {0, 0, 0}, .exp = {0, 0}, .inc = subb_in };
    ecases[1] = (case){ .in = {1, 0, 0}, .exp = {1, 0}, .inc = subb_in };
    ecases[2] = (case){ .in = {5, 5, 0}, .exp = {0, 0}, .inc = subb_in };
    ecases[3] = (case){ .in = {9, 4, 1}, .exp = {4, 0}, .inc = subb_in };
    ecases[4] = (case){ .in = {9, 8, 1}, .exp = {0, 0}, .inc = subb_in };
    // Rollover/Underflow tests
    ecases[5] = (case){ .in = {16, 20, 0},     .exp = {U64m - 4, 1}, .inc = subb_in };
    ecases[6] = (case){ .in = {0,  1,  0},     .exp = {U64m, 1}, .inc = subb_in };
    ecases[7] = (case){ .in = {15, U64m, 1},   .exp = {14, 1}, .inc = subb_in };
    ecases[8] = (case){ .in = {0,  0, 1},      .exp = {U64m, 1}, .inc = subb_in };
    ecases[9] = (case){ .in = {10, 10, 1},     .exp = {U64m, 1}, .inc = subb_in };
    // Near the edge tests
    ecases[10] = (case){ .in = {U64m, U64m, 0}, .exp = {U64m-1, 1}, .inc = subb_in };
    ecases[11] = (case){ .in = {U64m, U64m, 1}, .exp = {U64m, 1}, .inc = subb_in };
    ecases[12] = (case){ .in = {U64m, 0, 0},    .exp = {U64m, 1}, .inc = subb_in };
    ecases[13] = (case){ .in = {U64m, 0, 1},    .exp = {U64m-1, 0}, .inc = subb_in };
    ecases[14] = (case){ .in = {0,    U64m, 0}, .exp = {1, 1}, .inc = subb_in };
    ecases[15] = (case){ .in = {0,    U64m, 1}, .exp = {0, 1}, .inc = subb_in };
    // Bitwise Stress Test
    ecases[16] = (case){ .in = {alt64_1, alt64_2, 0}, .exp = {alt64_2, 0}, .inc = subb_in };
    ecases[17] = (case){ .in = {alt64_2, alt64_1, 1}, .exp = {alt64_1+1, 1}, .inc = subb_in };
    ecases[18] = (case){ .in = {U64m>>1+1, U64m>>1, 1},   .exp = {0, 0}, .inc = subb_in };
    // API/Function Logic test
    ecases[19] = (case){ .in = {5, U64m, 1}, .exp = {5, 1}, .inc = subb_in };
    // Randomly Generated Cases filling
    for (u32 i = 0; i < rcount; ++i) {
        u64 in3 = (barebone_rand() & 1) ? 0 : 1;
        rcases[i] = (case){ 
            .in = {barebone_rand(), barebone_rand(), in3}, 
            .inc = subb_in
        };
    } _DNML_SUITE_SETUP(s, name, 20, rcount,
        subb_in, ecases, rcases, ribuf,
        resbuf, test_fn, ref_fn, subblp
    ); return;
}
static inline void wmul_setup(
    _libdnml_lsuite *s, const char *name,
    case *ecases, case *rcases, u32 rcount, 
    u64 **ribuf, pair *resbuf, const char *wmullp,
    u64 (*test_fn)(u64, u64, u64*),
    u64 (*ref_fn)(u64, u64, u64*)
) { 
    srand(time(NULL)); static u8 wmul_in = 2;
    // Basic Tests
    ecases[0] = (case){ .in = {0, 0}, .exp = {0, 0}, .inc = wmul_in };
    ecases[1] = (case){ .in = {1, 1}, .exp = {1, 0}, .inc = wmul_in };
    ecases[2] = (case){ .in = {1, 0}, .exp = {0, 0}, .inc = wmul_in };
    ecases[3] = (case){ .in = {9, 8}, .exp = {72, 0}, .inc = wmul_in };
    // Extreme Casees
    ecases[4] = (case){ .in = {U64m, U64m},     .exp = {1, U64m-2}, .inc = wmul_in };
    ecases[5] = (case){ .in = {U63m+1, 2},      .exp = {0, 1},      .inc = wmul_in };
    ecases[6] = (case){ .in = {U63m+1, U63m+1}, .exp = {0, U62m+1}, .inc = wmul_in };
    ecases[7] = (case){ .in = {U32m, U32m}, .exp = {0xFFFFFFFE00000001, 0}, .inc = wmul_in };
    ecases[8] = (case){ .in = {U32m+1, U32m+1}, .exp = {0, 1}, .inc = wmul_in };
    ecases[9] = (case){ .in = {half64_2, half64_1}, .exp = {U32m+1, half64_1-1}, .inc = wmul_in };
    ecases[10] = (case){ 
        .in =  {alt64_1,          alt64_2         }, 
        .exp = {0x1C71C71C71C71C72, 0x38E38E38E38E38E3}, 
        .inc = wmul_in 
    };
    ecases[11] = (case){ .in = {U64m, U64m-1}, .exp = {2, U64m-2}, .inc = wmul_in };
    ecases[12] = (case){ 
        .in = {U64m,                0x0001000100010001}, 
        .exp = {0xFFEFFFEFFFEFFFFF, 0x0001000100010000},
        .inc = wmul_in 
    };
    ecases[13] = (case){ .in = {U32m, U64m}, .exp = {half64_2+1, half64_1-1}, .inc = wmul_in };
    ecases[14] = (case){ 
        .in =  {0xFFFFFFFFFFFFFFC5, 3}, 
        .exp = {0xFFFFFFFFFFFFFF4F, 2}, 
        .inc = wmul_in 
    };
    // Randomly Generated Cases filling
    for (u32 i = 0; i < rcount; ++i) {
        rcases[i] = (case){ 
            .in = {barebone_rand(), barebone_rand(), 0},
            .inc = wmul_in
        };
    } _DNML_SUITE_SETUP(s, name, 15, rcount,
        wmul_in, ecases, rcases, ribuf,
        resbuf, test_fn, ref_fn, wmullp
    ); return;
}
static inline void wdiv_setup(
    _libdnml_lsuite *s, const char *name,
    case *ecases, case *rcases, u32 rcount, 
    u64 **ribuf, pair *resbuf, const char *wmullp,
    u64 (*test_fn)(u64, u64, u64*),
    u64 (*ref_fn)(u64, u64, u64*)
) {
    srand(time(NULL)); static u8 wdiv_in = 3;
    // Upper Limits
    ecases[0] = (case){ .in = {U64m, U64m-1, U64m}, .exp = {U64m, U64m-1}, .inc = wdiv_in };
    ecases[1] = (case){ .in = {U64m-1, 1, U64m},    .exp = {2, 0}, .inc = wdiv_in };
    ecases[2] = (case){ .in = {U64m,   1, U64m},    .exp = {2, 1}, .inc = wdiv_in };
    ecases[3] = (case){ .in = {U64m-1, 0, U64m},    .exp = {0, U64m-1}, .inc = wdiv_in };
    /// Boundary Cases
    ecases[4] = (case){ .in = {0, 1, 2}, .exp = {U63m+1, 0}, .inc = wdiv_in };
    ecases[5] = (case){ .in = {1, 1, 2}, .exp = {U63m+1, 1}, .inc = wdiv_in };
    ecases[6] = (case){ 
        .in = {half64_2,  half64_1, U32m+1}, 
        .exp = {U64m,       0}, 
        .inc = wdiv_in 
    }; 
    ecases[7] = (case){ .in = {U63m, U64m, U63m+1}, .exp = {U64m, U63m}, .inc = wdiv_in };
    // Assymertical Tests
    ecases[8] = (case){ .in = {U64m, 0, 1},        .exp = {U64m, 0}, .inc = wdiv_in };
    ecases[9] = (case){ .in = {U32m, 0, U64m},     .exp = {0, U32m}, .inc = wdiv_in };
    ecases[10] = (case){ .in =  {U64m, 0, U64m},   .exp = {1, 0}, .inc = wdiv_in };
    ecases[11] = (case){ .in =  {U64m-1, 0, U63m}, .exp = {1, U63m}, .inc = wdiv_in };
    // Bitwise Handling Tests
    ecases[12] = (case){ 
        .in = {half64_1-1,  half64_2+1, U32m}, 
        .exp = {half64_2,   1},
        .inc = wdiv_in 
    };
    ecases[13] = (case){ 
        .in = {alt64_1, alt64_2, alt64_1}, 
        .exp = {U63m+1, alt64_2}, 
        .inc = wdiv_in
    };
    ecases[14] = (case){ 
        .in = {0x13579BDF2468ACE0, 0x123456789ABCDEF0, U63m+1},
        .exp = {0x2468ACF13579BDE0, 0x13579BDF2468ACE0},
        .inc = wdiv_in 
    };
    // Randomly Generated Cases filling
    for (u32 i = 0; i < rcount; ++i) {
        u64 in2 = barebone_rand();
        while (!in2) in2 = barebone_rand();
        rcases[i] = (case) {
            .in = {barebone_rand(), in2, 0},
            .inc = wdiv_in
        };
    } _DNML_SUITE_SETUP(s, name, 15, rcount,
        wdiv_in, ecases, rcases, ribuf,
        resbuf, test_fn, ref_fn, wmullp
    ); return;
}



//* ========================================== *//
//* ===== MODULAR ARITHMETIC SUITE SETUP ===== *//
//* ========================================== *//
static inline void modinv_setup(
    _libdnml_lsuite *s, const char *name,
    case *ecases, case *rcases, u32 rcount, 
    u64 **ribuf, pair *resbuf, const char *wmullp,
    u64 (*test_fn)(u64, u64, u64*),
    u64 (*ref_fn)(u64, u64, u64*)
) {
    srand(time(NULL)); static u8 modinv_in = 1;
    // Core, Basic cases
    ecases[0] = (case){ .in = {1},     .exp = {1},     .inc = modinv_in };
    ecases[1] = (case){ .in = {U64m},  .exp = {U64m},  .inc = modinv_in };
    ecases[2] = (case){ .in = {3},     .exp = {0xAAAAAAAAAAAAAAAB}, .inc = modinv_in };
    ecases[3] = (case){ .in = {5},     .exp = {0xCCCCCCCCCCCCCCCD}, .inc = modinv_in };
    ecases[4] = (case){ .in = {7},     .exp = {0xB6DB6DB6DB6DB6DB}, .inc = modinv_in };
    // Self-Inverting cases
    ecases[5] = (case){ 
        .in = {0x7FFFFFFFFFFFFFFF}, 
        .exp = {0x7FFFFFFFFFFFFFFF}, 
        .inc = modinv_in 
    };
    ecases[6] = (case){ 
        .in = {0x8000000000000001}, 
        .exp = {0x8000000000000001},
        .inc = modinv_in 
    }; 
    ecases[7] = (case){ .in = {U64m-2}, .exp = {alt64_2}, .inc = modinv_in };
    ecases[8] = (case){ .in = {U32m},   .exp = {0xFFFFFFFEFFFFFFFF}, .inc = modinv_in };
    // Specific Patterns and Cases
    ecases[9] = (case){ .in = {17},    .exp = {0xF0F0F0F0F0F0F0F1}, .inc = modinv_in };
    ecases[10] = (case){ .in = {15},   .exp = {0xEEEEEEEEEEEEEEEF}, .inc = modinv_in };
    ecases[11] = (case){ .in = {alt64_2}, .exp = {U64m-2},          .inc = modinv_in };
    ecases[12] = (case){ .in = {dalt}, .exp = {dalt},               .inc = modinv_in };
    ecases[13] = (case){ .in = {19},   .exp = {0x79435E50D79435E3}, .inc = modinv_in };
    ecases[14] = (case){ 
        .in = {0x1234567890ABCDEF},
        .exp = {0x3196E67A831E018F},
        .inc = modinv_in 
    };
    // Randomly Generated Cases filling
    for (u32 i = 0; i < rcount; ++i) {
        u64 in = barebone_rand();
        while (!(in & 1)) in = barebone_rand();
        rcases[i] = (case) {
            .in = {in},
            .inc = modinv_in
        };
    } _DNML_SUITE_SETUP(s, name, 15, rcount,
        modinv_in, ecases, rcases, ribuf,
        resbuf, test_fn, ref_fn, wmullp
    ); return;
}



//* ========================================== *//
//* ===== BITWISE OPERATIONS SUITE SETUP ===== *//
//* ========================================== *//
static inline void clz_setup(
    _libdnml_lsuite *s, const char *name,
    case *ecases, case *rcases, u32 rcount, 
    u64 **ribuf, pair *resbuf, const char *wmullp,
    u64 (*test_fn)(u64, u64, u64*),
    u64 (*ref_fn)(u64, u64, u64*)
) {
    srand(time(NULL)); static u8 clz_in = 1;
    // Core, Basic cases
    ecases[0] = (case){ .in = {0},      .exp = {64}, .inc = clz_in };
    ecases[1] = (case){ .in = {U64m},   .exp = {0},  .inc = clz_in };
    ecases[2] = (case){ .in = {U63m+1}, .exp = {1}, .inc = clz_in };
    ecases[3] = (case){ .in = {1},      .exp = {63}, .inc = clz_in };
    // Off-by-1 Cases
    ecases[4] = (case){ .in = {U62m+1}, .exp = {1},  .inc = clz_in };
    ecases[5] = (case){ .in = {2},      .exp = {62}, .inc = clz_in };
    ecases[6] = (case){ .in = {0x0010000000000001}, .exp = {11}, .inc = clz_in }; 
    ecases[7] = (case){ .in = {U31m+1},    .exp = {32}, .inc = clz_in };
    ecases[8] = (case){ .in = {U32m+1<<1}, .exp = {30}, .inc = clz_in };
    // Block Boundaries
    ecases[9] = (case){ .in = {half64_1},      .exp = {32}, .inc = clz_in };
    ecases[10] = (case){ .in = {half64_1+1},   .exp = {31}, .inc = clz_in };
    ecases[11] = (case){ .in = {U8m},          .exp = {56}, .inc = clz_in };
    ecases[12] = (case){ .in = {U16m},         .exp = {48}, .inc = clz_in };
    ecases[13] = (case){ .in = {U64m << 16},   .exp = {16}, .inc = clz_in };
    // Pattern Cases
    ecases[14] = (case){ .in = {0x0F00000000000000}, .exp = {4}, .inc = clz_in };
    ecases[15] = (case){ .in = {0x00F0000000000000}, .exp = {8}, .inc = clz_in };
    ecases[16] = (case){ .in = {alt64_1}, .exp = {0}, .inc = clz_in };
    ecases[17] = (case){ .in = {alt64_2}, .exp = {1}, .inc = clz_in };
    ecases[18] = (case){ .in = {0x0000800000000001}, .exp = {16}, .inc = clz_in };
    ecases[19] = (case){ .in = {0x0FFFFFFFFFFFFFFF}, .exp = {4},  .inc = clz_in };
    // Randomly Generated Cases filling
    for (u32 i = 0; i < rcount; ++i) {
        u64 in = barebone_rand();
        rcases[i] = (case) { .in = {in}, .inc = clz_in };
    } _DNML_SUITE_SETUP(s, name, 15, rcount,
        clz_in, ecases, rcases, ribuf,
        resbuf, test_fn, ref_fn, wmullp
    ); return;
}
static inline void ctz_setup(
    _libdnml_lsuite *s, const char *name,
    case *ecases, case *rcases, u32 rcount, 
    u64 **ribuf, pair *resbuf, const char *wmullp,
    u64 (*test_fn)(u64, u64, u64*),
    u64 (*ref_fn)(u64, u64, u64*)
) {
    srand(time(NULL)); static u8 ctz_in = 1;
    // Core, Basic cases
    ecases[0] = (case){ .in = {0},      .exp = {64}, .inc = ctz_in };
    ecases[1] = (case){ .in = {U64m},   .exp = {0},  .inc = ctz_in };
    ecases[2] = (case){ .in = {U63m+1}, .exp = {63}, .inc = ctz_in };
    ecases[3] = (case){ .in = {1},      .exp = {0},  .inc = ctz_in };
    // Off-by-1 Cases
    ecases[4] = (case){ .in = {U62m+1}, .exp = {62},   .inc = ctz_in };
    ecases[5] = (case){ .in = {2},      .exp = {1},    .inc = ctz_in };
    ecases[6] = (case){ .in = {U63m+1}, .exp = {15},   .inc = ctz_in }; 
    ecases[7] = (case){ .in = {U31m+1},    .exp = {31}, .inc = ctz_in };
    ecases[8] = (case){ .in = {U32m+1<<1}, .exp = {33}, .inc = ctz_in };
    // Block Boundaries
    ecases[9] = (case){ .in = {half64_2},      .exp = {32}, .inc = ctz_in };
    ecases[10] = (case){ .in = {half64_1+1},   .exp = {31}, .inc = ctz_in };
    ecases[11] = (case){ .in = {half64_2+1},   .exp = {0},  .inc = ctz_in };
    ecases[12] = (case){ .in = {U8m},          .exp = {8},  .inc = ctz_in };
    ecases[13] = (case){ .in = {U16m},         .exp = {16}, .inc = ctz_in };
    ecases[14] = (case){ .in = {U64m << 16},   .exp = {48}, .inc = ctz_in };
    // Pattern Cases
    ecases[15] = (case){ .in = {0x00000000000000F0}, .exp = {4}, .inc = ctz_in };
    ecases[16] = (case){ .in = {0x0000000000000F00}, .exp = {8}, .inc = ctz_in };
    ecases[17] = (case){ .in = {alt64_1}, .exp = {0}, .inc = ctz_in };
    ecases[18] = (case){ .in = {alt64_2}, .exp = {1}, .inc = ctz_in };
    ecases[19] = (case){ .in = {0xFFFFFFFFFFFFFFF0}, .exp = {4},  .inc = ctz_in };
    // Randomly Generated Cases filling
    for (u32 i = 0; i < rcount; ++i) {
        u64 in = barebone_rand();
        rcases[i] = (case) { .in = {in}, .inc = ctz_in };
    } _DNML_SUITE_SETUP(s, name, 15, rcount,
        ctz_in, ecases, rcases, ribuf,
        resbuf, test_fn, ref_fn, wmullp
    ); return;
}
static inline void bswap_setup(
    _libdnml_lsuite *s, const char *name,
    case *ecases, case *rcases, u32 rcount, 
    u64 **ribuf, pair *resbuf, const char *wmullp,
    u64 (*test_fn)(u64, u64, u64*),
    u64 (*ref_fn)(u64, u64, u64*)
) {
    srand(time(NULL)); static u8 ctz_in = 1;
    // Core, Basic cases
    ecases[0] = (case){ .in = {0},      .exp = {0},    .inc = ctz_in };
    ecases[1] = (case){ .in = {U64m},   .exp = {U64m}, .inc = ctz_in };
    ecases[2] = (case){ .in = {0x0102030405060708}, .exp = {0x0807060504030201}, .inc = ctz_in };
    ecases[3] = (case){ .in = {0x1122334444332211}, .exp = {0x1122334444332211}, .inc = ctz_in };
    ecases[4] = (case){ .in = {0xDEADBEEFCAFEBABE}, .exp = {0xBEBAFECAEFBEADDE}, .inc = ctz_in };
    // Symmetry
    ecases[5] = (case){ .in = {0x00000000000000FF}, .exp = {0xFF00000000000000}, .inc = ctz_in };
    ecases[6] = (case){ .in = {0xFF00000000000000}, .exp = {0x00000000000000FF}, .inc = ctz_in };
    ecases[7] = (case){ .in = {0xFF000000000000FF}, .exp = {0xFF000000000000FF}, .inc = ctz_in }; 
    ecases[8] = (case){ .in = {0x1200000000000034}, .exp = {0x3400000000000012}, .inc = ctz_in };
    ecases[9] = (case){ .in = {0x000000FFFF000000}, .exp = {0x000000FFFF000000}, .inc = ctz_in };
    // Blocks
    ecases[10] = (case){ .in = {half64_1}, .exp = {half64_2}, .inc = ctz_in };
    ecases[11] = (case){ .in = {0x000000FF00000000}, .exp = {0x00000000FF000000}, .inc = ctz_in };
    ecases[12] = (case){ .in = {0x0000000000FF0000}, .exp = {0x0000FF0000000000}, .inc = ctz_in };
    ecases[13] = (case){ .in = {0x0000FF0000000000}, .exp = {0x0000000000FF0000}, .inc = ctz_in };
    // Pattern & Traps
    ecases[14] = (case){ .in = {0x0A0B0C0D0E0F1A1B}, .exp = {0x1B1A0F0E0D0C0B0A}, .inc = ctz_in };
    ecases[15] = (case){ .in = {0xAA55AA55AA55AA55}, .exp = {0x55AA55AA55AA55AA}, .inc = ctz_in };
    ecases[16] = (case){ .in = {0xF0F0F0F0F0F0F0F0}, .exp = {0xF0F0F0F0F0F0F0F0}, .inc = ctz_in };
    ecases[17] = (case){ .in = {0x8000000000000001}, .exp = {0x0100000000000080}, .inc = ctz_in };
    ecases[18] = (case){ .in = {0x0804020110204080}, .exp = {0x0804020110204080}, .inc = ctz_in };
    ecases[19] = (case){ .in = {0x13579BDF02468ACE}, .exp = {0xCE8A4602DF9B5713}, .inc = ctz_in };
    // Randomly Generated Cases filling
    for (u32 i = 0; i < rcount; ++i) {
        u64 in = barebone_rand();
        rcases[i] = (case) { .in = {in}, .inc = ctz_in };
    } _DNML_SUITE_SETUP(s, name, 15, rcount,
        ctz_in, ecases, rcases, ribuf,
        resbuf, test_fn, ref_fn, wmullp
    ); return;
}
static inline void pcnt_setup(
    _libdnml_lsuite *s, const char *name,
    case *ecases, case *rcases, u32 rcount, 
    u64 **ribuf, pair *resbuf, const char *wmullp,
    u64 (*test_fn)(u64, u64, u64*),
    u64 (*ref_fn)(u64, u64, u64*)
) {
    srand(time(NULL)); static u8 ctz_in = 1;
    // Core, Basic cases
    ecases[0] = (case){ .in = {0},      .exp = {0},  .inc = ctz_in };
    ecases[1] = (case){ .in = {U64m},   .exp = {64}, .inc = ctz_in };
    ecases[2] = (case){ .in = {1},      .exp = {1},  .inc = ctz_in };
    ecases[3] = (case){ .in = {U63m+1}, .exp = {1},  .inc = ctz_in };
    // Off-by-slightly ig
    ecases[4] = (case){ .in = {U64m-1}, .exp = {63},   .inc = ctz_in };
    ecases[5] = (case){ .in = {U63m},   .exp = {63},   .inc = ctz_in };
    ecases[6] = (case){ .in = {0xFFFFFFFFEFFFFFFF}, .exp = {63},   .inc = ctz_in }; 
    ecases[7] = (case){ .in = {U63m+2}, .exp = {2},    .inc = ctz_in };
    // Half Blocks
    ecases[8] = (case){ .in = {half64_2},              .exp = {32}, .inc = ctz_in };
    ecases[9] = (case){ .in = {half64_1},              .exp = {32}, .inc = ctz_in };
    ecases[10] = (case){ .in = {0x0000FFFFFFFF0000},   .exp = {32}, .inc = ctz_in };
    ecases[11] = (case){ .in = {0xFF00FF00FF00FF00},   .exp = {32}, .inc = ctz_in };
    // Pattern Cases
    ecases[12] = (case){ .in = {alt64_1}, .exp = {32}, .inc = ctz_in };
    ecases[13] = (case){ .in = {alt64_2}, .exp = {32}, .inc = ctz_in };
    ecases[14] = (case){ .in = {0x1111111111111111}, .exp = {16}, .inc = ctz_in };
    ecases[15] = (case){ .in = {0x3333333333333333}, .exp = {32}, .inc = ctz_in };
    ecases[16] = (case){ .in = {0x5555555555555555}, .exp = {32},  .inc = ctz_in };
    ecases[17] = (case){ .in = {0x0707070707070707}, .exp = {24},  .inc = ctz_in };
    // Traps
    ecases[18] = (case){ .in = {0x0103070F1F3F7FFF}, .exp = {36},  .inc = ctz_in };
    ecases[19] = (case){ .in = {0x0001000200040008}, .exp = {4},   .inc = ctz_in };
    // Randomly Generated Cases filling
    for (u32 i = 0; i < rcount; ++i) {
        u64 in = barebone_rand();
        rcases[i] = (case) { .in = {in}, .inc = ctz_in };
    } _DNML_SUITE_SETUP(s, name, 15, rcount,
        ctz_in, ecases, rcases, ribuf,
        resbuf, test_fn, ref_fn, wmullp
    ); return;
}


#endif