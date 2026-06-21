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



#include "num_theory.h"
static const uint32_t dmr_bases[7] = { 2, 325, 9375, 28178, 450775, 9780504, 1794265022 };

//* ======== GCD - WORKSPACE RETURNER ======== */
size_t __BIGINT_STEIN_WS__(size_t u_size, size_t v_size) { return u_size + v_size; }
size_t __BIGINT_LEHMER_WS__(size_t u_size, size_t v_size) { return 0; } 
size_t __BIGINT_HALF_WS__(size_t u_size, size_t v_size) { return 0; }
size_t __BIGINT_GCD_WS__(size_t u_size, size_t v_size) {
    if (u_size == 1 && v_size == 1) return 0; // Euclid 64 bit doesn't require arena
    size_t op_size = min(u_size, v_size);
    if (op_size <= BIGINT_STEIN) return __BIGINT_STEIN_WS__(u_size, v_size);
    else if (op_size <= BIGINT_LEHMER) return __BIGINT_LEHMER_WS__(u_size, v_size);
    else return __BIGINT_HALF_WS__(u_size, v_size);
}
/* ======== GCD - ALGORITHMS ======== */
uint64_t __BIGINT_EUCLID__(uint64_t u, uint64_t v) {
    uint64_t remainder = (u < v) ? u : v;
    uint64_t dividend = (u >= v) ? u : v;
    uint64_t old_remainder;
    while (remainder) {
        old_remainder = remainder;
        remainder = dividend % remainder;
        dividend = old_remainder;
    } return dividend;
}
void __BIGINT_STEIN__(P_BIGINT res, PCONST_BIGINT u, PCONST_BIGINT v, calc_ctx stein_ctx, dnml_status *err) {
    // Base case - Identity #1 - gcd(u, 0) = u
    if (u->n == 0) { __BIGINT_INTERNAL_COPY__(res, v); return; }
    else if (v->n == 0) { __BIGINT_INTERNAL_COPY__(res, u); return; }

    // Setup - Identity #2 - gcd(2u, 2v) = gcd(u, v)
    dnml_status echeck;
    size_t stein_mark = scratch_mark(&stein_ctx), maxsize = max(u->n, v->n); // maxsize is used for SWAP
    BIGINT_TEMP(u_copy, maxsize, stein_ctx, stein_mark, echeck, err,); u_copy.n = u->n;
    BIGINT_TEMP(v_copy, maxsize, stein_ctx, stein_mark, echeck, err,); v_copy.n = v->n;
    memcpy(u_copy.limbs, u->limbs, u->n * U64_BYTES); memcpy(v_copy.limbs, v->limbs, v->n * U64_BYTES);
    size_t i = __BIGINT_CTZ__(u); __BIGINT_INTERNAL_RSHIFT__(&u_copy, i);
    size_t j = __BIGINT_CTZ__(v); __BIGINT_INTERNAL_RSHIFT__(&v_copy, j); 
    size_t k = min(i, j);

    // Procedure
    int8_t comp_res = __BIGINT_INTERNAL_COMP__(&u_copy, &v_copy);
    while (comp_res) {
        if (comp_res == -1) __BIGINT_INTERNAL_SWAP__(&u_copy, &v_copy);
        // Identity #4: gcd(u, v) = gcd(u, v - u)
        //  WHEN: +) u & v is ODD
        //        +) u <= v
        __BIGINT_SUB_WB__(&u_copy, &u_copy, &v_copy);
        // Identity #3 - gcd(u, 2v) = gcd(u, v)
        i = __BIGINT_CTZ__(&u_copy);
        __BIGINT_INTERNAL_RSHIFT__(&u_copy, i);
        comp_res = __BIGINT_INTERNAL_COMP__(&u_copy, &v_copy);
    }
    __BIGINT_INTERNAL_LSHIFT__(&u_copy, k); __BIGINT_INTERNAL_COPY__(res, &u_copy);
    scratch_rewind(&stein_ctx, stein_mark); *err = BIGINT_SUCCESS;
}
void __BIGINT_LEHMER__(P_BIGINT res, PCONST_BIGINT u, PCONST_BIGINT v, calc_ctx lehmer_ctx, dnml_status *err) {}
void __BIGINT_HALF__(P_BIGINT res, PCONST_BIGINT u, PCONST_BIGINT v, calc_ctx half_ctx, dnml_status *err) {}
void __BIGINT_GCD_DISPATCH__(P_BIGINT res, PCONST_BIGINT u, PCONST_BIGINT v, calc_ctx gcd_ctx, dnml_status *err) {
    size_t op_size = min(u->n, v->n);
    if (u->n == 1 && v->n == 1) { res->limbs[0] = __BIGINT_EUCLID__(u->limbs[0], v->limbs[0]); res->n = 1; }
    else if (op_size <= BIGINT_STEIN) __BIGINT_STEIN__(res, u, v, gcd_ctx, err);
    else if (op_size <= BIGINT_LEHMER) __BIGINT_LEHMER__(res, u, v, gcd_ctx, err);
    else __BIGINT_HALF__(res, u, v, gcd_ctx, err);
}


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
size_t __BIGINT_BPSW_WS__(size_t n_size) { return 0; }
size_t __BIGINT_ECPP_WS__(size_t n_size) { return 0; }
size_t __BIGINT_PTEST_WS__(size_t x_size) {
    if (x_size < MIXED_MAIN) return 0;
    else {
        size_t random_size = (size_t)(sqrtl((long double)x_size)) + 1;
        size_t proc_calls = max(
            __BIGINT_BPSW_WS__(x_size),
            __BIGINT_MRABIN_WS__(x_size, random_size)
        );
        return (random_size * U64_BYTES) + proc_calls;
    }
}
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
uint8_t __BIGINT_MILLER_RABIN__(PCONST_BIGINT n, PCONST_BIGINT base, calc_ctx rabin_ctx, dnml_status *err) {
    if (n->sign == -1) return 0;
    if (!n->n || (n->n == 1 && n->limbs[1] == 1)) return 0;
    dnml_status echeck;
    uint8_t prim_status = 0; uint64_t a[1] = {1};
    size_t mrabin_mark = scratch_mark(&rabin_ctx);

    BIGINT_TEMP(n_min1, n->n, rabin_ctx, mrabin_mark, echeck, err, 0); n_min1.n = n->n;
    memcpy(n_min1.limbs, n->limbs, n->n * U64_BYTES);
    bigInt constant_one = {.limbs = a, .n = 1, .cap = 1, .sign = 1 };
    __BIGINT_SUB_WB__(&n_min1, &n_min1, &constant_one);
    size_t s = (uint64_t)(__BIGINT_CTZ__(&n_min1));
    BIGINT_TEMP(d, n_min1.n, rabin_ctx, mrabin_mark, echeck, err, 0);
    memcpy(d.limbs, n_min1.limbs, n_min1.n * U64_BYTES);
    __BIGINT_INTERNAL_RLSHIFT__(&d, (size_t)(s / U64_BITS));
    __BIGINT_INTERNAL_RSHIFT__(&d, (size_t)(s % U64_BITS));

    // 1st test: a^d mod(n)
    BIGINT_TEMP(x, n->n, rabin_ctx, mrabin_mark, echeck, err, 0);
    __BIGINT_MODEXP_DISPATCH__(base, &d, n, &x, rabin_ctx, &echeck); SCRATCH_OVF(echeck, rabin_ctx, mrabin_mark, err, 0);
    if (x.n == 1 && x.limbs[0] == 1) prim_status = 1; // a^d mod(n) = 1
    else if (!__BIGINT_INTERNAL_COMP__(&x, &n_min1)) prim_status = 1; // a^d mod(n) = n - 1

    // 2nd test: a^(2^r * d) mod(n)
    if (unlikely(n->n <= BIGINT_CLASSICAL)) {
        for (uint64_t mrr = 1; mrr < s; ++mrr) {
            __BIGINT_CMODMUL__(&x, &x, n, &x, rabin_ctx, &echeck); SCRATCH_OVF(echeck, rabin_ctx, mrabin_mark, err, 0);
            if (x.n == 1 && x.limbs[0] == 1) { prim_status = 1; break; }
            else if (!__BIGINT_INTERNAL_COMP__(&x, &n_min1)) { prim_status = 1; break; }
        }
    } else {
        mont_ctx mont_ctx = {.n = n, .nprime = __MODINV_UI64__(n->limbs[0]), .k = n->n};
        BIGINT_TEMP(r, n->n << 1, rabin_ctx, mrabin_mark, echeck, err, 0); r.n = n->n + 1;
        BIGINT_TEMP(r_mod_n, n->n, rabin_ctx, mrabin_mark, echeck, err, 0);
        BIGINT_TEMP(tmp, n->n << 1, rabin_ctx, mrabin_mark, echeck, err, 0); r.limbs[n->n] = 1; 
        __BIGINT_MOD_DISPATCH__(&r, n, &r_mod_n, &tmp, rabin_ctx, &echeck); SCRATCH_OVF(echeck, rabin_ctx, mrabin_mark, err, 0);
        __BIGINT_MUL_DISPATCH__(&r_mod_n, &r_mod_n, &tmp, rabin_ctx, &echeck); SCRATCH_OVF(echeck, rabin_ctx, mrabin_mark, err, 0);
        __BIGINT_MOD_DISPATCH__(&tmp, n, &tmp, &r, rabin_ctx, &echeck); SCRATCH_OVF(echeck, rabin_ctx, mrabin_mark, err, 0);
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
uint8_t __BIGINT_BPSW__(PCONST_BIGINT n, calc_ctx bpsw_ctx, dnml_status *err) { return 0; }
uint8_t __BIGINT_ECPP__(PCONST_BIGINT n, calc_ctx ecpp_ctx, dnml_status *err) { return 0; }
uint8_t __BIGINT_PTEST_DISPATCH__(PCONST_BIGINT x, calc_ctx ptest_ctx, dnml_status *err) {
    if (x->n < MIXED_MAIN) {
        if (x->limbs[0] <= TRIAL_DIVISION) return __BIGINT_TRIAL_DIV__(x->limbs[0]);
        else return __BIGINT_SMALL_MRABIN__(x->limbs[0]);
    } else {
        dnml_status echeck; uint8_t bpsw_ret = __BIGINT_BPSW__(x, ptest_ctx, &echeck);
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
