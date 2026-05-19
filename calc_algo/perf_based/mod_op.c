#include "mod_op.h"

//* ------------------------------ *//
//* ----- WORKSPACE FUNCTIONS ---- *//
//* ------------------------------ *//
size_t __BIGINT_CMODMUL_WS__(size_t a_size, size_t b_size, size_t mod_size) {
    size_t raw_size = (a_size + b_size) << 1;
    size_t fcall_size = max(
        __BIGINT_MUL_WS__(a_size, b_size),
        __BIGINT_MOD_WS__((a_size + b_size), mod_size)
    ); return raw_size + fcall_size;
}
size_t __BIGINT_MONTMUL_WS__(size_t a_size, size_t b_size, mont_ctx ctx) {
    size_t raw_size = 2*ctx.k + 1;
    size_t mul_size = __BIGINT_MUL_WS__(a_size, b_size);
    return raw_size + mul_size;
}
static size_t __BIGINT_BIN_MODEXP_WS__(size_t base_size, size_t mod_size, size_t pow_size) {
    size_t raw_size = base_size + 2*mod_size + pow_size;
    size_t fcall_size = max(__BIGINT_MOD_WS__(base_size, mod_size), 
                            __BIGINT_CMODMUL_WS__(mod_size, mod_size, mod_size));
    return raw_size + fcall_size;
}
static size_t __BIGINT_MBIN_MODEXP_WS__(size_t base_size, size_t mod_size, size_t pow_size) {
    // Binary ModExp's objects
    size_t max_tsize = max(2*mod_size, max(base_size, pow_size));
    size_t rsize_tmpsize = max_tsize, rmodn_size = mod_size;
    size_t ressize_basesize = 2*mod_size, tmpexp_size = pow_size;
    // Low-level Function Stackframe
    size_t max_frame = max(__BIGINT_MONTMUL_WS__(mod_size, mod_size, (mont_ctx){.k = mod_size}), 
                           max(__BIGINT_MUL_WS__(rmodn_size, rmodn_size),
                               max(__BIGINT_MOD_WS__(max_tsize, mod_size),
                                   __BIGINT_MOD_WS__(max_tsize, mod_size))));
    return rsize_tmpsize + rmodn_size + ressize_basesize + tmpexp_size + max_frame;
}
size_t __BIGINT_MODMUL_WS__(size_t a_size, size_t b_size, size_t mod_size) {
    if (mod_size <= BIGINT_CLASSICAL) return __BIGINT_CMODMUL_WS__(a_size, b_size, mod_size);
    else { size_t montmul_internal = __BIGINT_MONTMUL_WS__(a_size, b_size, (mont_ctx){.k = mod_size});
        size_t setup_size = 4*mod_size + 1;
        size_t setup_fcall = max(
            __BIGINT_MOD_WS__(mod_size + 1, mod_size), 
            __BIGINT_MUL_WS__(mod_size, mod_size)
        ); return montmul_internal + setup_size + setup_fcall;
    }
}
size_t __BIGINT_MODEXP_WS__(size_t base_size, size_t mod_size, size_t pow_size) {
    if (mod_size <= BIGINT_MOD_BINARY) return __BIGINT_BIN_MODEXP_WS__(base_size, mod_size, pow_size);
    else if (mod_size <= BIGINT_MONT_BINARY) return __BIGINT_MBIN_MODEXP_WS__(base_size, mod_size, pow_size);
    else ; // FOR SLIDING WINDOW/2-ARY EXPONENTIATION
}




//* ------------------------------ *//
//* --------- ALGORITHMS --------- *//
//* ------------------------------ *//
void __BIGINT_CLASSICAL_MODMUL__(
    const bigInt *a, const bigInt *b, 
    const bigInt *modulus, bigInt *res, calc_ctx modmul_ctx
) {
    size_t cmodmul_mark = scratch_mark(&modmul_ctx);
    limb_t *prodlimbs = scratch_alloc(&modmul_ctx, (a->n + b->n) * BYTES_IN_UINT64_T);
    limb_t *tmplimbs = scratch_alloc(&modmul_ctx, (a->n + b->n) * BYTES_IN_UINT64_T);
    bigInt prod = {.limbs = prodlimbs, .sign = 1,   /**/    .n = 0, .cap = a->n + b->n};
    __BIGINT_MUL_DISPATCH__(a, b, &prod, modmul_ctx);
    __BIGINT_MOD_DISPATCH__(
        &prod, modulus, res, 
        &(bigInt){
            .limbs=tmplimbs, .sign=1, 
            .n=0, .cap=(a->n + b->n)}, 
        modmul_ctx
    ); scratch_reset(&modmul_ctx, cmodmul_mark);
}
void __BIGINT_MONTMUL__(
    const bigInt *a, const bigInt *b, 
    mont_ctx ctx, bigInt *res, calc_ctx montmul_ctx
) {
    size_t montmul_mark = scratch_mark(&montmul_ctx);
    limb_t *tlimbs = scratch_alloc(&montmul_ctx, (2*ctx.k + 1) * BYTES_IN_UINT64_T);
    bigInt t = {.limbs = tlimbs, .sign = 1,  /**/  .n = 0, .cap = 2*ctx.k + 1};
    __BIGINT_MUL_DISPATCH__(a, b, &t, montmul_ctx);
    __BIGINT_MONT_REDC__(&t, ctx, res, montmul_ctx);
    scratch_reset(&montmul_ctx, montmul_mark); 
}
static void __BIGINT_BIN_MODEXP__(
    const bigInt *base, const bigInt *power, 
    const bigInt *modulus, bigInt *res, calc_ctx binexp_ctx
) {
    size_t binexp_mark = scratch_mark(&binexp_ctx);
    limb_t *buflimbs = scratch_alloc(&binexp_ctx, base->n * BYTES_IN_UINT64_T);
    limb_t *tmpres_limbs = scratch_alloc(&binexp_ctx, modulus->n * BYTES_IN_UINT64_T); tmpres_limbs[0] = 1;
    limb_t *tmpexp_limbs = scratch_alloc(&binexp_ctx, power->n * BYTES_IN_UINT64_T);
    limb_t *tmpbase_limbs = scratch_alloc(&binexp_ctx, modulus->n * BYTES_IN_UINT64_T);
    bigInt buf = {.limbs = buflimbs, .sign = 1,             /**/    .n = 0, .cap = base->n};
    bigInt tmp_res = {.limbs = tmpres_limbs, .sign = 1,     /**/    .n = 1, .cap = modulus->n};
    bigInt tmp_exp = {.limbs = tmpexp_limbs, .sign = 1,     /**/    .n = power->n, .cap = power->n};
    bigInt tmp_base = {.limbs = tmpbase_limbs, .sign = 1,   /**/    .n = 0, .cap = modulus->n};
    memcpy(tmp_exp.limbs, power->limbs, power->n * BYTES_IN_UINT64_T);
    __BIGINT_MOD_DISPATCH__(base, modulus, &tmp_base, &buf, binexp_ctx);
    while (tmp_exp.n > 0) {
        if (tmp_exp.limbs[0] & 1) {
            __BIGINT_CLASSICAL_MODMUL__(&tmp_res, &tmp_base, modulus, &tmp_res, binexp_ctx);
        } __BIGINT_CLASSICAL_MODMUL__(&tmp_base, &tmp_base, modulus, &tmp_base, binexp_ctx);
        __BIGINT_INTERNAL_RSHIFT__(&tmp_exp, 1);
    } __BIGINT_INTERNAL_COPY__(res, &tmp_res); 
    scratch_reset(&binexp_ctx, binexp_mark);

}
static void __BIGINT_MBIN_MODEXP__(
    const bigInt *base, const bigInt *power, 
    const bigInt *modulus, bigInt *res, calc_ctx binexp_ctx
) {
    //* --- 1. SETUP ---- *//
    mont_ctx modexp_contx = { 
        .n      = modulus,
        .nprime = __MODINV_UI64__(modulus->limbs[0]),
        .k      = modulus->n
    }; size_t binexp_mark = scratch_mark(&binexp_ctx), max_tsize = max(2*modulus->n, max(base->n, power->n));
    limb_t *rlimbs = scratch_alloc(&binexp_ctx,         max_tsize * BYTES_IN_UINT64_T);
    limb_t *rmodn_limbs = scratch_alloc(&binexp_ctx,    modulus->n * BYTES_IN_UINT64_T);
    limb_t *tmp_limbs = scratch_alloc(&binexp_ctx,      max_tsize * BYTES_IN_UINT64_T);
    bigInt r = {.limbs = rlimbs, .sign = 1,               /**/    .n = modulus->n + 1, .cap = modulus->n + 1};
    bigInt r_mod_n = {.limbs = rmodn_limbs, .sign = 1,    /**/    .n = 0, .cap = modulus->n};
    bigInt tmp = {.limbs = tmp_limbs, .sign = 1,          /**/    .n = 0, .cap = modulus->n + 1};
    r.limbs[modulus->n] = 1; __BIGINT_MOD_DISPATCH__(&r, modulus, &r_mod_n, &tmp, binexp_ctx);
    __BIGINT_MUL_DISPATCH__(&r_mod_n, &r_mod_n, &tmp, binexp_ctx);
    __BIGINT_MOD_DISPATCH__(&tmp, modulus, &tmp, &r, binexp_ctx);
    modexp_contx.r2 = &tmp;

    //* ----- 2. MAIN LOOP ----- *//
    limb_t *tmpres_limbs = scratch_alloc(&binexp_ctx, modulus->n * BYTES_IN_UINT64_T); tmpres_limbs[0] = 1;
    limb_t *tmpexp_limbs = scratch_alloc(&binexp_ctx, power->n * BYTES_IN_UINT64_T);
    limb_t *tmpbase_limbs = scratch_alloc(&binexp_ctx, modulus->n * BYTES_IN_UINT64_T);
    bigInt tmp_res = {.limbs = tmpres_limbs, .sign = 1,     /**/    .n = 1, .cap = modulus->n};
    bigInt tmp_exp = {.limbs = tmpexp_limbs, .sign = 1,     /**/    .n = power->n, .cap = power->n};
    bigInt tmp_base = {.limbs = tmpbase_limbs, .sign = 1,   /**/    .n = 0, .cap = modulus->n};
    memcpy(tmp_exp.limbs, power->limbs, power->n * BYTES_IN_UINT64_T);
    __BIGINT_MOD_DISPATCH__(base, modulus, &tmp_base, &tmp, binexp_ctx);
    __BIGINT_MONTMUL__(&tmp_res, modexp_contx.r2, modexp_contx, &tmp_res, binexp_ctx);
    __BIGINT_MONTMUL__(&tmp_base, modexp_contx.r2, modexp_contx, &tmp_base, binexp_ctx);
    while (tmp_exp.n > 0) {
        if (tmp_exp.limbs[0] & 1) {
            __BIGINT_MONTMUL__(&tmp_res, &tmp_base, modexp_contx, &tmp_res, binexp_ctx);
        } __BIGINT_MONTMUL__(&tmp_base, &tmp_base, modexp_contx, &tmp_base, binexp_ctx);
        __BIGINT_INTERNAL_RSHIFT__(&tmp_exp, 1);
    } uint64_t a[1] = {1}; 
    __BIGINT_MONTMUL__(
        &tmp_res, &(bigInt){.limbs = a, .n = 1, .cap = 1, .sign = 1}, 
        modexp_contx, res, binexp_ctx
    ); scratch_reset(&binexp_ctx, binexp_mark);
}
void __BIGINT_MODMUL_DISPATCH__(
    const bigInt *a, const bigInt *b, 
    const bigInt *modulus, bigInt *res, calc_ctx modmul_ctx
) {
    if (modulus->n <= BIGINT_CLASSICAL) __BIGINT_CLASSICAL_MODMUL__(a, b, modulus, res, modmul_ctx);
    else { mont_ctx modmul_dispatch_ctx = { 
            .n      = modulus,
            .nprime = __MODINV_UI64__(modulus->limbs[0]),
            .k      = modulus->n
        }; size_t modmul_dispatch_mark = scratch_mark(&modmul_ctx);
        limb_t *rlimbs = scratch_alloc(&modmul_ctx, (modulus->n + 1) * BYTES_IN_UINT64_T);
        limb_t *rmodn_limbs = scratch_alloc(&modmul_ctx, modulus->n * BYTES_IN_UINT64_T);
        limb_t *tmp_limbs = scratch_alloc(&modmul_ctx, (2*modulus->n) * BYTES_IN_UINT64_T);
        bigInt r = {.limbs = rlimbs, .sign = 1,               /**/    .n = modulus->n + 1, .cap = modulus->n + 1};
        bigInt r_mod_n = {.limbs = rmodn_limbs, .sign = 1,    /**/    .n = 0, .cap = modulus->n};
        bigInt tmp = {.limbs = tmp_limbs, .sign = 1,          /**/    .n = 0, .cap = modulus->n + 1};
        r.limbs[modulus->n] = 1; __BIGINT_MOD_DISPATCH__(&r, modulus, &r_mod_n, &tmp, modmul_ctx);
        __BIGINT_MUL_DISPATCH__(&r_mod_n, &r_mod_n, &tmp, modmul_ctx); modmul_dispatch_ctx.r2 = &tmp;
        __BIGINT_MOD_DISPATCH__(&tmp, modulus, &tmp, &r, modmul_ctx);
        modmul_dispatch_ctx.r2 = &tmp;
        __BIGINT_MONTMUL__(a, b, modmul_dispatch_ctx, res, modmul_ctx);
        scratch_reset(&modmul_ctx, modmul_dispatch_mark);
    }
}
void __BIGINT_MODEXP_DISPATCH__(
    const bigInt *base, const bigInt *power, 
    const bigInt *modulus, bigInt *res, calc_ctx binexp_ctx
) {
    if (modulus->n < BIGINT_MOD_BINARY) __BIGINT_BIN_MODEXP__(base, power, modulus, res, binexp_ctx);
    else if (modulus->n < BIGINT_MONT_BINARY) __BIGINT_MBIN_MODEXP__(base, power, modulus, res, binexp_ctx);
    else ; // FOR SLIDING WINDOW/2-ARY EXPONENTIATION
}