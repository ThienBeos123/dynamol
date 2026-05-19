#include "num_theory.h"

static const uint32_t dmr_bases[7] = {
    2, 325, 9375, 28178, 
    450775, 9780504, 
    1794265022
};

//* ======== GCD - WORKSPACE RETURNER ======== */
static size_t __BIGINT_STEIN_WS__(size_t u_size, size_t v_size) { return u_size + v_size; }
static size_t __BIGINT_LEHMER_WS__(size_t u_size, size_t v_size) {}
static size_t __BIGINT_HALF_WS__(size_t u_size, size_t v_size) {}
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
    }
    return dividend;
}
static void __BIGINT_STEIN__(bigInt *res, const bigInt *u, const bigInt *v, calc_ctx stein_ctx) {
    // Base case - Identity #1 - gcd(u, 0) = u
    if (u->n == 0) { __BIGINT_INTERNAL_COPY__(res, v); return; }
    else if (v->n == 0) { __BIGINT_INTERNAL_COPY__(res, u); return; }

    // Setup - Identity #2 - gcd(2u, 2v) = gcd(u, v)
    size_t stein_mark = scratch_mark(&stein_ctx), maxsize = max(u->n, v->n); // maxsize is used for SWAP
    limb_t *ucopy_limbs = scratch_alloc(&stein_ctx, maxsize * BYTES_IN_UINT64_T);
    limb_t *vcopy_limbs = scratch_alloc(&stein_ctx, maxsize * BYTES_IN_UINT64_T);
    memcpy(ucopy_limbs, u->limbs, u->n * BYTES_IN_UINT64_T);
    memcpy(vcopy_limbs, v->limbs, v->n * BYTES_IN_UINT64_T);
    bigInt u_copy = {.limbs = ucopy_limbs, .sign = 1,   /**/    .n = u->n, .cap = maxsize};
    bigInt v_copy = {.limbs = vcopy_limbs, .sign = 1,   /**/    .n = v->n, .cap = maxsize};
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
    __BIGINT_INTERNAL_LSHIFT__(&u_copy, k);
    __BIGINT_INTERNAL_COPY__(res, &u_copy);
    scratch_reset(&stein_ctx, stein_mark);
}
static void __BIGINT_LEHMER__(bigInt *res, const bigInt *u, const bigInt *v, calc_ctx lehmer_ctx) {}
static void __BIGINT_HALF__(bigInt *res, const bigInt *u, const bigInt *v, calc_ctx half_ctx) {}
void __BIGINT_GCD_DISPATCH__(bigInt *res, const bigInt *u, const bigInt *v, calc_ctx gcd_ctx) {
    size_t op_size = min(u->n, v->n);
    __BIGINT_INTERNAL_ENSCAP__(res, op_size);
    if (u->n == 1 && v->n == 1) {
        res->limbs[0] = __BIGINT_EUCLID__(u->limbs[0], v->limbs[0]);
        res->n        = 1;
    } else if (op_size <= BIGINT_STEIN) __BIGINT_STEIN__(res, u, v, gcd_ctx);
    else if (op_size <= BIGINT_LEHMER) __BIGINT_LEHMER__(res, u, v, gcd_ctx);
    else __BIGINT_HALF__(res, u, v, gcd_ctx);
}


//* ======== Primality Testing - WORKSPACE RETURNER ======== */
static size_t __BIGINT_MRABIN_WS__(size_t n_size, size_t base_size) {
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
static size_t __BIGINT_BPSW_WS__(size_t n_size) {}
size_t __BIGINT_ECPP_WS__(size_t n_size) {}
size_t __BIGINT_PTEST_WS__(size_t x_size) {
    if (x_size < MIXED_MAIN) return 0;
    else { //! FIGURE OUT LATER !
        size_t random_size = 2; //? Whatever it is
        size_t proc_calls = max(
            __BIGINT_BPSW_WS__(x_size),
            __BIGINT_MRABIN_WS__(x_size, random_size)
        ); return (random_size * BYTES_IN_UINT64_T) 
                 + proc_calls + (2 * alignof(max_align_t));
    }
}
/* ======== Primality Testing - ALGORITHMS ======== */
static uint8_t __BIGINT_TRIAL_DIV__(uint64_t x) {
    if (x <= 1) return 0;
    else if (x == 2 || x == 3 || x == 5) return 1;
    else if (!(x & 1) || !(x % 3) || !(x % 5)) return 0;
    uint8_t steps[8] = {6, 4, 2, 4, 2, 4, 6, 2};
    uint8_t steps_i = 1;
    for (uint64_t i = 7; 
        i <= (uint64_t)(sqrt(x)) + 1; 
        i += steps[steps_i]
    ) { 
        if (x % i == 0) return 0;
        steps_i = (steps_i < 7) ? steps_i + 1 : 0;
    } return 1;
}
static uint8_t __BIGINT_SMALL_MRABIN__(uint64_t n) {
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
static uint8_t __BIGINT_MILLER_RABIN__(const bigInt *n, const bigInt* base, calc_ctx mrabin_ctx) {
    if (n->sign == -1) return 0; uint8_t prim_status = 0; uint64_t a[1] = {1};
    size_t mrabin_mark = scratch_mark(&mrabin_ctx);
    limb_t *nmo_limbs = scratch_alloc(&mrabin_ctx, n->n);
    memcpy(nmo_limbs, n->limbs, n->n * BYTES_IN_UINT64_T);
    bigInt n_minus_one = {.limbs = nmo_limbs, .sign = 1,    /**/    .n = n->n, .cap = n->n};
    bigInt constant_one = {.limbs = a, .n = 1, .cap = 1, .sign = 1 };
    __BIGINT_SUB_WB__(&n_minus_one, &n_minus_one, &constant_one);
    size_t s = (uint64_t)(__BIGINT_CTZ__(&n_minus_one));
    limb_t *dlimbs = scratch_alloc(&mrabin_ctx, n_minus_one.n * BYTES_IN_UINT64_T);
    memcpy(dlimbs, n_minus_one.limbs, n_minus_one.n * BYTES_IN_UINT64_T);
    bigInt d = {.limbs = dlimbs, .sign = 1,     /**/    .n = n_minus_one.n, .cap = n_minus_one.n};
    __BIGINT_INTERNAL_RLSHIFT__(&d, (size_t)(s / BITS_IN_UINT64_T));
    __BIGINT_INTERNAL_RSHIFT__(&d, (size_t)(s % BITS_IN_UINT64_T));
    
    // 1st test: a^d mod(n)
    limb_t *xlimbs = scratch_alloc(&mrabin_ctx, n->n * BYTES_IN_UINT64_T);
    bigInt x = {.limbs = xlimbs, .sign = 1,     /**/    .n = 0, .cap = n->n};
    __BIGINT_MODEXP_DISPATCH__(base, &d, n, &x, mrabin_ctx);
    if (x.n == 1 && x.limbs[0] == 1) prim_status = 1; // a^d mod(n) = 1
    else if (!__BIGINT_INTERNAL_COMP__(&x, &n_minus_one)) prim_status = 1; // a^d mod(n) = n - 1

    // 2nd test: a^(2^r * d) mod(n)
    if (unlikely(n->n <= BIGINT_CLASSICAL)) {
        for (uint64_t mrr = 1; mrr < s; ++mrr) {
            __BIGINT_CLASSICAL_MODMUL__(&x, &x, n, &x, mrabin_ctx);
            if (x.n == 1 && x.limbs[0] == 1) { prim_status = 1; break; }
            else if (!__BIGINT_INTERNAL_COMP__(&x, &n_minus_one)) { prim_status = 1; break; }
        }
    } else {
        mont_ctx mrabin_mont_ctx = {.n = n, .nprime = __MODINV_UI64__(n->limbs[0]), .k = n->n}; 
        limb_t *rlimbs = scratch_alloc(&mrabin_ctx, (2*n->n) * BYTES_IN_UINT64_T);
        limb_t *rmodn_limbs = scratch_alloc(&mrabin_ctx, n->n * BYTES_IN_UINT64_T);
        limb_t *tmp_limbs = scratch_alloc(&mrabin_ctx, (2*n->n) * BYTES_IN_UINT64_T);
        bigInt r = {.limbs = rlimbs, .sign = 1,             /**/    .n = n->n + 1, .cap = 2*n->n};
        bigInt r_mod_n = {.limbs = rmodn_limbs, .sign = 1,  /**/    .n = 0, .cap = n->n};
        bigInt tmp = {.limbs = tmp_limbs, .sign = 1,        /**/    .n = 0, .cap = 2*n->n};
        r.limbs[n->n] = 1; __BIGINT_MOD_DISPATCH__(&r, n, &r_mod_n, &tmp, mrabin_ctx);
        __BIGINT_MUL_DISPATCH__(&r_mod_n, &r_mod_n, &tmp, mrabin_ctx);
        __BIGINT_MOD_DISPATCH__(&tmp, n, &tmp, &r, mrabin_ctx);
        mrabin_mont_ctx.r2 = &tmp;
        // Conversions
        __BIGINT_MONTMUL__(&x, mrabin_mont_ctx.r2, mrabin_mont_ctx, &x, mrabin_ctx);
        __BIGINT_MONTMUL__(&n_minus_one, mrabin_mont_ctx.r2, mrabin_mont_ctx, &n_minus_one, mrabin_ctx);
        // 1 in Montgomery form is just R mod(N), so we reuse r_mod_n
        for (uint64_t mrr = 1; mrr < s; ++mrr) {
            __BIGINT_MONTMUL__(&x, &x, mrabin_mont_ctx, &x, mrabin_ctx);
            if (!__BIGINT_INTERNAL_COMP__(&x, &constant_one)) { prim_status = 1; break; }
            else if (!__BIGINT_INTERNAL_COMP__(&x, &n_minus_one)) { prim_status = 1; break; }
        }
    } scratch_reset(&mrabin_ctx, mrabin_mark); 
    return prim_status;
}
static uint8_t __BIGINT_BPSW__(const bigInt *n, calc_ctx mrabin_ctx) {}
uint8_t __BIGINT_ECPP__(const bigInt *n, calc_ctx mrabin_ctx) {}
uint8_t __BIGINT_PTEST_DISPATCH__(const bigInt *x, calc_ctx ptest_ctx) {
    if (x->n < MIXED_MAIN) {
        if (x->limbs[0] <= TRIAL_DIVISION) return __BIGINT_TRIAL_DIV__(x->limbs[0]);
        else return __BIGINT_SMALL_MRABIN__(x->limbs[0]);
    } else {
        size_t ptest_mark = scratch_mark(&ptest_ctx);
        limb_t *randbase_limbs = scratch_alloc(&ptest_ctx, 2); // Whatever size
        bigInt random_base = {.limbs = randbase_limbs, .sign = 1,   /**/    .n = 0, .cap = 2}; 
        if (!__BIGINT_BPSW__(x, ptest_ctx)) { scratch_reset(&ptest_ctx, ptest_mark); return 0; } 
        for (size_t i = 0; i < MRROUNDS_DNML; ++i) {
            /*
            todo    RNG RIGHT HERE
            ?       RNG RIGHT HERE
            *       RNG RIGHT HERE
            !       RNG RIGHT HERE
            */
            if (!__BIGINT_MILLER_RABIN__(x, &random_base, ptest_ctx)) { 
                scratch_reset(&ptest_ctx, ptest_mark);
                return 0;
            }
        } scratch_reset(&ptest_ctx, ptest_mark); return 1;
    }
}
