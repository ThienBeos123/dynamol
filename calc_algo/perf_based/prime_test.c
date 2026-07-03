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



#include "prime_test.h"
#include <debug_util.h>
#include <tables.h>
#include "../../util/aconv_macros.h"
static const uint32_t dmr_bases[7] = { 2, 325, 9375, 28178, 450775, 9780504, 1794265022 };
/** ----------- General BigInt Primality Testing -----------
 * THIS FILE CONTAINS THE FOLLOWING ALGORITHMS + OPERATIONS:
 *
 *      - 64-bit Trial Division (<= 207936)
 *      - 64-bit Deterministic Miller-Rabin (64-bit ONLY)
 *      - Probabilistic Miller-Rabin (General)
 *      - Unproven Probabilistic Baillie-PSW (General)
 *
 * This file is generally the main and only algorithm file for bigInt primality testing,
 * containing the primality testing algorithm dispatcher, as well as the workspace sizing function dispatcher.
 */


//* ======== Primality Testing - WORKSPACE RETURNER ======== */
size_t __BIGINT_MRABIN_WS__(size_t n_size, size_t base_size) {
    // Main, raw Miller-Rabin size
    // Obj_count also accounts for the function call workspace
    size_t additional_size = 0;
    size_t mrabin_setup_size = 2*n_size;
    size_t x_size = n_size, max_fcall;
    // Branching of Algorithm Dispatch (MODMULs)
    if (likely(n_size > BIGINT_CLASSICAL)) {
        size_t rlimbs_size = 2*n_size;
        size_t rmodn_size = n_size;
        size_t tmp_size = 2*n_size;
        additional_size = rlimbs_size + rmodn_size + tmp_size;
        size_t zdomain_funcs = max(
            __BIGINT_MOD_WS__(n_size + 1, n_size),
            max(__BIGINT_MUL_WS__(rmodn_size, rmodn_size),
                __BIGINT_MOD_WS__(tmp_size, n_size))
        ); size_t outer_montmuls = max(
            __BIGINT_MONTMUL_WS__(x_size, n_size, (mont_ctx){.k = n_size}),
            __BIGINT_MONTMUL_WS__(n_size, n_size, (mont_ctx){.k = n_size})
        ); size_t inner_montmuls = __BIGINT_MONTMUL_WS__(n_size, n_size, (mont_ctx){.k = n_size});
        max_fcall = max(zdomain_funcs, max(outer_montmuls, inner_montmuls));
    } else max_fcall = max(
        __BIGINT_CMODMUL_WS__(x_size, x_size, n_size),
        __BIGINT_CMODMUL_WS__(n_size, n_size, n_size)
    );
    max_fcall = max(max_fcall, __BIGINT_MODEXP_WS__(base_size, n_size, n_size));
    return mrabin_setup_size + x_size + additional_size + max_fcall;
}
size_t __BIGINT_ECPP_WS__(size_t n_size) { return 0; }
size_t __BIGINT_PTEST_WS__(size_t x_size) {
    if (x_size < MIXED_MAIN) return 0;
    else {
        if (!_DNML_PRIMALITY_STRATEGY) {
            if (x_size < MRABIN_ONLY) return __BIGINT_MRABIN_WS__(x_size, 2);
            else return __BIGINT_BPSW_WS__(x_size);
        } else {
            size_t random_size = (size_t)(sqrtl((long double)x_size)) + 1;
            size_t proc_calls = max(__BIGINT_BPSW_WS__(x_size), __BIGINT_MRABIN_WS__(x_size, random_size));
            return (random_size * U64_BYTES) + proc_calls;
        }
    }
}





//* ======== Primality Testing - ALGORITHMS ======== *//
// Helper functions
static void _randbase_fill(P_BIGINT x, xoshiro256_state *state, size_t upper_size) {
    x->n = __rng_skrange(state, 1, upper_size, 0.1f); 
    size_t i = 0; while (i < x->n) {
        uint64_t rand = xoshiro256pp_next(state);
        if (i == x->n - 1 && !rand) continue; /**/ ++i;
    }
}
/* Small, word-size Deterministic Tests */
uint8_t __BIGINT_TRIAL_DIV__(uint64_t x) {
    if (x <= 1) return 0;
    else if (x == 2 || x == 3 || x == 5) return 1;
    else if (!(x & 1) || !(x % 3) || !(x % 5)) return 0;
    uint8_t steps[8] = {6, 4, 2, 4, 2, 4, 6, 2}; uint8_t steps_i = 1;
    for (uint64_t i = 7; i <= (uint64_t)(sqrtl((long double)x)) + 1; i += steps[steps_i]) {
        if (!(x % i)) return 0; /**/ steps_i = (steps_i < 7) ? steps_i + 1 : 0;
    } return 1;
}
uint8_t __BIGINT_SMALL_MRABIN__(uint64_t n) {
    uint64_t s = 0, d = n - 1, x;
    uint8_t composite = 1;
    while (!(d & 1)) { ++s; d >>= 1; }
    for (uint8_t i = 0; i < 7; ++i) {
        uint32_t curr_base = dmr_bases[i]; 
        x = __MODEXP_UI64__(curr_base, d, n);
        // 1. Check a^d mod(n) = 1
        if (x == 1 || x == n - 1) continue;
        // 2. Check for a^(2^r * d) mod(n) = n - 1
        composite = 1;
        for (uint64_t r = 1; r < s; ++r) {
            x = __MODMUL_UI64__(x, x, n);
            if (x == n - 1) { composite = 0; break; }
        } if (composite) return 0;
    } return 1;
}





/* ---------- Large Probablistic Tests - Miller Rabin ---------- */
uint8_t __BIGINT_MILLER_RABIN__(PCONST_BIGINT n, PCONST_BIGINT base, calc_ctx rabin_ctx, dnml_status *err) {
    // Early exit conditions - Even/Odd + Divisibility by 5
    if (n->sign < 0) { *err = BIGINT_SUCCESS; return 0; }
    if (!n->n || (n->n == 1 && n->limbs[0] == 1)) { *err = BIGINT_SUCCESS; return 0; }
    if (!(n->limbs[0])) { *err = BIGINT_SUCCESS; return 0; }
    if (!(n->limbs[0] % 5) || n->limbs[0] % 5 == 5) { *err = BIGINT_SUCCESS; return 0; }
    // Early exit conditions - Perfect squares checking (Only lightly, Miller-Rabin still works on perfect squares)
    uint64_t limb_1 = n->limbs[0]; uint64_t _mod10_ = limb_1 % 10;
    uint64_t _mod16_ = limb_1 & 15; uint64_t _mod64_ = limb_1 & 63; 
    uint64_t _mod256_ = limb_1 & UINT8_MAX;
    if (_mod10_ == 1 || _mod10_ == 9) { *err = BIGINT_SUCCESS; return 0; } // Check in mod(10)
    if (_mod16_ == 1 || _mod16_ == 9) { *err = BIGINT_SUCCESS; return 0; } // Check in mod(16)
    for (uint8_t i = 0; i < _PFSQR_MOD64_CNT; ++i) { if (_mod64_ == pfsqr_filter_mod64[i]) { *err = BIGINT_SUCCESS; return 0; }}
    for (uint8_t i = 0; i < _PFSQR_MOD256_CNT; ++i) { if (_mod256_ == pfsqr_filter_mod256[i]) { *err = BIGINT_SUCCESS; return 0; }}



    /* ---------------- Normal Operation - Miller Rabin ---------------- */
    dnml_status echeck = BIGINT_SUCCESS;
    uint8_t prim_status = 0; uint64_t a = 1;
    size_t mrabin_mark = scratch_mark(&rabin_ctx);

    BIGINT_TEMP(n_min1, n->n, rabin_ctx, mrabin_mark, echeck, err, 0); n_min1.n = n->n;
    memcpy(n_min1.limbs, n->limbs, n->n * U64_BYTES);
    bigInt constant_one = {.limbs = &a, .n = 1, .cap = 1, .sign = 1 };
    __BIGINT_SUB_WB__(&n_min1, &n_min1, &constant_one);
    size_t s = (uint64_t)(__BIGINT_CTZ__(&n_min1));
    BIGINT_TEMP(d, n_min1.n, rabin_ctx, mrabin_mark, echeck, err, 0);
    memcpy(d.limbs, n_min1.limbs, n_min1.n * U64_BYTES);
    __BIGINT_INTERNAL_RLSHIFT__(&d, (size_t)(s / U64_BITS));
    __BIGINT_INTERNAL_RSHIFT__(&d, (size_t)(s % U64_BITS));

    // 1st test: a^d mod(n)
    BIGINT_TEMP(x, n->n, rabin_ctx, mrabin_mark, echeck, err, 0);
    __BIGINT_MODEXP_DISP__(base, &d, n, &x, rabin_ctx, &echeck); SCRATCH_OVF(echeck, rabin_ctx, mrabin_mark, err, 0);
    if (x.n == 1 && x.limbs[0] == 1) prim_status = 1; // a^d mod(n) = 1
    else if (!__BIGINT_INTERNAL_COMP__(&x, &n_min1)) prim_status = 1; // a^d mod(n) = n - 1

    // 2nd test: a^(2^r * d) mod(n)
    if (unlikely(n->n <= BIGINT_CLASSICAL) && (!prim_status)) {
        for (uint64_t mrr = 1; mrr < s; ++mrr) {
            __BIGINT_CMODMUL__(&x, &x, n, &x, rabin_ctx, &echeck); SCRATCH_OVF(echeck, rabin_ctx, mrabin_mark, err, 0);
            if (x.n == 1 && x.limbs[0] == 1) { prim_status = 1; break; }
            else if (!__BIGINT_INTERNAL_COMP__(&x, &n_min1)) { prim_status = 1; break; }
        }
    } else if (!prim_status) {
        mont_ctx mont_ctx = {.n = n, .nprime = __MODINV_UI64__(n->limbs[0]), .k = n->n};
        BIGINT_TEMP(r, n->n + 1, rabin_ctx, mrabin_mark, echeck, err, 0); r.n = n->n + 1;
        BIGINT_TEMP(r_mod_n, n->n, rabin_ctx, mrabin_mark, echeck, err, 0);
        BIGINT_TEMP(tmp, n->n << 1, rabin_ctx, mrabin_mark, echeck, err, 0); r.limbs[n->n] = 1; 
        __BIGINT_MOD_DISP__(&r, n, &r_mod_n, rabin_ctx, &echeck); SCRATCH_OVF(echeck, rabin_ctx, mrabin_mark, err, 0);
        __BIGINT_MUL_DISP__(&r_mod_n, &r_mod_n, &tmp, rabin_ctx, &echeck); SCRATCH_OVF(echeck, rabin_ctx, mrabin_mark, err, 0);
        __BIGINT_MOD_DISP__(&tmp, n, &tmp, rabin_ctx, &echeck); SCRATCH_OVF(echeck, rabin_ctx, mrabin_mark, err, 0);
        mont_ctx.r2 = &tmp;
        // Conversions
        __BIGINT_MONTMUL__(&x, mont_ctx.r2, mont_ctx, &x, rabin_ctx, &echeck); SCRATCH_OVF(echeck, rabin_ctx, mrabin_mark, err, 0);
        __BIGINT_MONTMUL__(&n_min1, mont_ctx.r2, mont_ctx, &n_min1, rabin_ctx, &echeck);
        SCRATCH_OVF(echeck, rabin_ctx, mrabin_mark, err, 0);
        // 1 in Montgomery form is just R mod(N), so we reuse r_mod_n
        for (uint64_t mrr = 1; mrr < s; ++mrr) {
            __BIGINT_MONTMUL__(&x, &x, mont_ctx, &x, rabin_ctx, &echeck); SCRATCH_OVF(echeck, rabin_ctx, mrabin_mark, err, 0);
            if (!__BIGINT_INTERNAL_COMP__(&x, &constant_one)) { prim_status = 1; break; }
            else if (!__BIGINT_INTERNAL_COMP__(&x, &n_min1)) { prim_status = 1; break; }
        }
    } scratch_rewind(&rabin_ctx, mrabin_mark); *err = BIGINT_SUCCESS; return prim_status;
}





/* Deterministic Primality Certificate Tests */
uint8_t __BIGINT_ECPP__(PCONST_BIGINT n, calc_ctx ecpp_ctx, dnml_status *err) { return 0; }






/* Algorithm Dispatcher */
uint8_t __BIGINT_PTEST_DISP__(PCONST_BIGINT x, calc_ctx ptest_ctx, dnml_status *err) {
    if (x->n < MIXED_MAIN) {
        if (x->limbs[0] <= TRIAL_DIVISION) return __BIGINT_TRIAL_DIV__(x->limbs[0]);
        else return __BIGINT_SMALL_MRABIN__(x->limbs[0]);
    } 
    else {
        if (!_DNML_PRIMALITY_STRATEGY) {
            uint64_t a = 2; bigInt two = { .limbs = &a, .n = 1, .cap = 1, .sign = 1 };
            if (x->n <= MRABIN_ONLY) return __BIGINT_MILLER_RABIN__(x, &two, ptest_ctx, err);
            else return __BIGINT_BPSW__(x, ptest_ctx, err);
        } else {
            dnml_status echeck = BIGINT_SUCCESS; uint8_t bpsw_ret = __BIGINT_BPSW__(x, ptest_ctx, &echeck);
            if (echeck == DARENA_OVERFLOW) { *err = DARENA_OVERFLOW; return 0; }
            if (!bpsw_ret) { *err = BIGINT_SUCCESS; return 0; }
    
            xoshiro256_state ptmain_state = {0}; uint64_t side_mix = 0;
            __GET_ENTROPY_FAST(&side_mix, sizeof(side_mix));
            __GET_ENTROPY_FAST(ptmain_state.s, (sizeof(uint64_t)) << 2);
            seed_xoshiro256(&ptmain_state, side_mix); size_t ptest_mark = scratch_mark(&ptest_ctx);
            size_t uppperbound = (size_t)(sqrtl((long double)x->n)) + 1;
            size_t rand_upper = __rng_skrange(&ptmain_state, 6, uppperbound, 70.0f);
            BIGINT_TEMP(random_base, rand_upper, ptest_ctx, ptest_mark, echeck, err, 0);
            for (size_t i = 0; i < _DNML_MR_ROUNDS_DYNAMOL; ++i) {
                _randbase_fill(&random_base, &ptmain_state, rand_upper);
                uint8_t mrabin_ret = __BIGINT_MILLER_RABIN__(x, &random_base, ptest_ctx, &echeck);
                if (echeck == DARENA_OVERFLOW) { scratch_rewind(&ptest_ctx, ptest_mark); *err = DARENA_OVERFLOW; return 0; }
                if (!mrabin_ret) { scratch_rewind(&ptest_ctx, ptest_mark); *err = BIGINT_SUCCESS; return 0; }
            } scratch_rewind(&ptest_ctx, ptest_mark); *err = BIGINT_SUCCESS; return 1;
        }
    }
}
