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

#include "heap_ptest.h"
#include <debug_util.h>
#include "../../util/aconv_macros.h"
static const uint32_t dmr_bases[7] = { 2, 325, 9375, 28178, 450775, 9780504, 1794265022 };
/** ----------- Heap-based BigInt Primality Testing -----------
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

 
//* ======== Primality Testing - ALGORITHMS ======== *//
// Helper functions
static void _randbase_fill(P_BIGINT x, xoshiro256_state *state, size_t upper_size) {
    x->n = __rng_range(state, 1, upper_size); 
    size_t i = 0; while (i < x->n) {
        uint64_t rand = xoshiro256pp_next(state);
        if (i == x->n - 1 && !rand) continue; /**/ ++i;
    }
}
// Main Algorithm Functions
uint8_t __BIHEAP_TRIAL_DIV__(uint64_t x) {
    if (x <= 1) return 0;
    else if (x == 2 || x == 3 || x == 5) return 1;
    else if (!(x & 1) || !(x % 3) || !(x % 5)) return 0;
    uint8_t steps[8] = {6, 4, 2, 4, 2, 4, 6, 2}; uint8_t steps_i = 1;
    for (uint64_t i = 7; i <= (uint64_t)(sqrtl((long double)x)) + 1; i += steps[steps_i]) {
        if (!(x % i)) return 0; /**/ steps_i = (steps_i < 7) ? steps_i + 1 : 0;
    } return 1;
}
uint8_t __BIHEAP_SMALL_MRABIN__(uint64_t n) {
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
uint8_t __BIHEAP_MILLER_RABIN__(PCONST_BIGINT n, PCONST_BIGINT base, dnml_status *err) {
    if (n->sign == -1) return 0; /**/ if (!n->n || (n->n == 1 && n->limbs[1] == 1)) return 0;
    dnml_status echeck = BIGINT_SUCCESS; uint8_t prim_status = 0; uint64_t a[1] = {1};
    bigInt *alloc_list[6], *early_free[6]; uint8_t alloc_cnt = 0, early_cnt = 0;

    BIHEAP_TEMP(n_min1, n->n, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt, 0); n_min1.n = n->n;
    memcpy(n_min1.limbs, n->limbs, n->n * U64_BYTES);
    bigInt constant_one = {.limbs = a, .n = 1, .cap = 1, .sign = 1 };
    __BIGINT_SUB_WB__(&n_min1, &n_min1, &constant_one);
    size_t s = (uint64_t)(__BIGINT_CTZ__(&n_min1));
    BIHEAP_TEMP(d, n_min1.n, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt, 0);
    memcpy(d.limbs, n_min1.limbs, n_min1.n * U64_BYTES);
    __BIGINT_INTERNAL_RLSHIFT__(&d, (size_t)(s / U64_BITS));
    __BIGINT_INTERNAL_RSHIFT__(&d, (size_t)(s % U64_BITS));

    // 1st test: a^d mod(n)
    BIHEAP_TEMP(x, n->n, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt, 0);
    __BIHEAP_MODEXP_DISP__(base, &d, n, &x, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt, 0);
    if (x.n == 1 && x.limbs[0] == 1) prim_status = 1; // a^d mod(n) = 1
    else if (!__BIGINT_INTERNAL_COMP__(&x, &n_min1)) prim_status = 1; // a^d mod(n) = n - 1

    // 2nd test: a^(2^r * d) mod(n)
    if (unlikely(n->n <= BIGINT_CLASSICAL)) {
        for (uint64_t mrr = 1; mrr < s; ++mrr) {
            __BIHEAP_CMODMUL__(&x, &x, n, &x, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt, 0);
            if (x.n == 1 && x.limbs[0] == 1) { prim_status = 1; break; }
            else if (!__BIGINT_INTERNAL_COMP__(&x, &n_min1)) { prim_status = 1; break; }
        }
    } else {
        mont_ctx mont_ctx = {.n = n, .nprime = __MODINV_UI64__(n->limbs[0]), .k = n->n};
        BIHEAP_TEMP(r, n->n << 1, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt, 0); r.n = n->n + 1;
        BIHEAP_TEMP(r_mod_n, n->n, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt, 0);
        BIHEAP_TEMP(tmp, n->n << 1, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt, 0); r.limbs[n->n] = 1; 
        __BIHEAP_MOD_DISP__(&r, n, &r_mod_n, &tmp, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt, 0);
        __BIHEAP_MUL_DISP__(&r_mod_n, &r_mod_n, &tmp, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt, 0);
        __BIHEAP_MOD_DISP__(&tmp, n, &tmp, &r, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt, 0);
        mont_ctx.r2 = &tmp;
        // Conversions
        __BIHEAP_MONTMUL__(&x, mont_ctx.r2, mont_ctx, &x, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt, 0);
        __BIHEAP_MONTMUL__(&n_min1, mont_ctx.r2, mont_ctx, &n_min1, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt, 0);
        // 1 in Montgomery form is just R mod(N), so we reuse r_mod_n
        for (uint64_t mrr = 1; mrr < s; ++mrr) {
            __BIHEAP_MONTMUL__(&x, &x, mont_ctx, &x, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt, 0);
            if (!__BIGINT_INTERNAL_COMP__(&x, &constant_one)) { prim_status = 1; break; }
            else if (!__BIGINT_INTERNAL_COMP__(&x, &n_min1)) { prim_status = 1; break; }
        }
    } _free_alloc_list(alloc_list, alloc_cnt); *err = BIGINT_SUCCESS; return prim_status;
}
uint8_t __BIHEAP_BPSW__(PCONST_BIGINT n, dnml_status *err) { return 0; }
uint8_t __BIHEAP_ECPP__(PCONST_BIGINT n, dnml_status *err) { return 0; }
uint8_t __BIHEAP_PTEST_DISP__(PCONST_BIGINT x, dnml_status *err) {
    if (x->n < MIXED_MAIN) {
        if (x->limbs[0] <= TRIAL_DIVISION) return __BIHEAP_TRIAL_DIV__(x->limbs[0]);
        else return __BIHEAP_SMALL_MRABIN__(x->limbs[0]);
    } else {
        dnml_status echeck = BIGINT_SUCCESS; uint8_t bpsw_ret = __BIHEAP_BPSW__(x, &echeck);
        if (echeck == DARENA_OVERFLOW) { *err = DARENA_OVERFLOW; return 0; }
        if (!bpsw_ret) { *err = BIGINT_SUCCESS; return 0; }
        bigInt *alloc_list[1], *early_free[1]; uint8_t alloc_cnt = 0, early_cnt = 0; 
 
        xoshiro256_state ptmain_state = {0}; uint64_t side_mix = 0;
        __GET_ENTROPY_FAST(&side_mix, sizeof(side_mix));
        __GET_ENTROPY_FAST(ptmain_state.s, (sizeof(uint64_t)) << 2);
        seed_xoshiro256(&ptmain_state, side_mix);
        size_t uppperbound = (size_t)(sqrtl((long double)x->n)) + 1;
        size_t rand_upper = __rng_skrange(&ptmain_state, 6, uppperbound, 70.0f);
        BIHEAP_TEMP(random_base, rand_upper, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt, 0);
        for (size_t i = 0; i < _DNML_MR_ROUNDS_DYNAMOL; ++i) {
            _randbase_fill(&random_base, &ptmain_state, rand_upper);
            uint8_t mrabin_ret = __BIHEAP_MILLER_RABIN__(x, &random_base, &echeck);
            if (echeck == DNML_ALLOC_OOM) { _free_alloc_list(alloc_list, alloc_cnt); *err = DNML_ALLOC_OOM; return 0; }
            if (!mrabin_ret) { _free_alloc_list(alloc_list, alloc_cnt); *err = DNML_ALLOC_OOM; return 0; }
        } _free_alloc_list(alloc_list, alloc_cnt); *err = BIGINT_SUCCESS; return 1;
    }
}
