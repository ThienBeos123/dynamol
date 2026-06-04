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
size_t __BIGINT_BIN_MODEXP_WS__(size_t base_size, size_t mod_size, size_t pow_size) {
    size_t raw_size = base_size + 2*mod_size + pow_size;
    size_t fcall_size = max(__BIGINT_MOD_WS__(base_size, mod_size), 
                            __BIGINT_CMODMUL_WS__(mod_size, mod_size, mod_size));
    return raw_size + fcall_size;
}
size_t __BIGINT_MBIN_MODEXP_WS__(size_t base_size, size_t mod_size, size_t pow_size) {
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
void __BIGINT_CMODMUL__(const bigInt *a, const bigInt *b, const bigInt *modulus, bigInt *res, calc_ctx modmul_ctx) {
    dnml_status err_check, end_stat = 0;
    size_t cmodmul_mark = scratch_mark(&modmul_ctx);
    BIGINT_TEMP(prod, (a->n + b->n), modmul_ctx, err_check, end_stat);
    BIGINT_TEMP(tmp, (a->n + b->n), modmul_ctx, err_check, end_stat);
    __BIGINT_MUL_DISPATCH__(a, b, &prod, modmul_ctx);
    __BIGINT_MOD_DISPATCH__(&prod, modulus, res, &tmp, modmul_ctx
    ); scratch_reset(&modmul_ctx, cmodmul_mark);
}
void __BIGINT_MONTMUL__(const bigInt *a, const bigInt *b, mont_ctx ctx, bigInt *res, calc_ctx montmul_ctx) {
    dnml_status err_check, end_stat;
    size_t montmul_mark = scratch_mark(&montmul_ctx);
    BIGINT_TEMP(t, (2*ctx.k + 1), montmul_ctx, err_check, end_stat);
    __BIGINT_MUL_DISPATCH__(a, b, &t, montmul_ctx);
    __BIGINT_MONT_REDC__(&t, ctx, res);
    scratch_reset(&montmul_ctx, montmul_mark); 
}
void __BIGINT_BIN_MODEXP__(
    const bigInt *base, const bigInt *power, 
    const bigInt *modulus, bigInt *res, calc_ctx binexp_ctx
) {
    dnml_status err_check, end_stat = 0;
    size_t binexp_mark = scratch_mark(&binexp_ctx);
    BIGINT_TEMP(buf, base->n, binexp_ctx, err_check, end_stat);
    BIGINT_TEMP(tmp_res, modulus->n, binexp_ctx, err_check, end_stat);
    BIGINT_TEMP(tmp_exp, power->n, binexp_ctx, err_check, end_stat);
    BIGINT_TEMP(tmp_base, modulus->n, binexp_ctx, err_check, end_stat);
    tmp_res.limbs[0] = 1; memcpy(tmp_exp.limbs, power->limbs, power->n * U64_BYTES);
    __BIGINT_MOD_DISPATCH__(base, modulus, &tmp_base, &buf, binexp_ctx);
    while (tmp_exp.n > 0) {
        if (tmp_exp.limbs[0] & 1) {
            __BIGINT_CMODMUL__(&tmp_res, &tmp_base, modulus, &tmp_res, binexp_ctx);
        } __BIGINT_CMODMUL__(&tmp_base, &tmp_base, modulus, &tmp_base, binexp_ctx);
        __BIGINT_INTERNAL_RSHIFT__(&tmp_exp, 1);
    } __BIGINT_INTERNAL_COPY__(res, &tmp_res); 
    scratch_reset(&binexp_ctx, binexp_mark);

}
void __BIGINT_MBIN_MODEXP__(
    const bigInt *base, const bigInt *power, 
    const bigInt *modulus, bigInt *res, calc_ctx binexp_ctx
) {
    //* --- 1. SETUP ---- *//
    mont_ctx modexp_contx = { 
        .n      = modulus,
        .nprime = __MODINV_UI64__(modulus->limbs[0]),
        .k      = modulus->n
    }; size_t binexp_mark = scratch_mark(&binexp_ctx), max_tsize = max((modulus->n << 1), max(base->n, power->n));
    dnml_status err_check, end_stat = 0;
    BIGINT_TEMP(r, max_tsize, binexp_ctx, err_check, end_stat); r.n = modulus->n + 1;
    BIGINT_TEMP(r_mod_n, modulus->n, binexp_ctx, err_check, end_stat);
    BIGINT_TEMP(tmp, max_tsize, binexp_ctx, err_check, end_stat);
    r.limbs[modulus->n] = 1; __BIGINT_MOD_DISPATCH__(&r, modulus, &r_mod_n, &tmp, binexp_ctx);
    __BIGINT_MUL_DISPATCH__(&r_mod_n, &r_mod_n, &tmp, binexp_ctx);
    __BIGINT_MOD_DISPATCH__(&tmp, modulus, &tmp, &r, binexp_ctx);
    modexp_contx.r2 = &tmp;

    //* ----- 2. MAIN LOOP ----- *//
    BIGINT_TEMP(tmp_res, modulus->n, binexp_ctx, err_check, end_stat);
    BIGINT_TEMP(tmp_exp, power->n, binexp_ctx, err_check, end_stat);
    BIGINT_TEMP(tmp_base, modulus->n, binexp_ctx, err_check, end_stat);
    tmp_res.limbs[0] = 1; memcpy(tmp_exp.limbs, power->limbs, power->n * U64_BYTES);
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
    if (modulus->n <= BIGINT_CLASSICAL) __BIGINT_CMODMUL__(a, b, modulus, res, modmul_ctx);
    else {
        dnml_status err_check, end_stat = 0;
        mont_ctx modmul_dispatch_ctx = {
            .n      = modulus,
            .nprime = __MODINV_UI64__(modulus->limbs[0]),
            .k      = modulus->n
        }; size_t modmul_dispatch_mark = scratch_mark(&modmul_ctx);
        BIGINT_TEMP(r, modulus->n + 1, modmul_ctx, err_check, end_stat); r.n = modulus->n + 1;
        BIGINT_TEMP(r_mod_n, modulus->n, modmul_ctx, err_check, end_stat);
        BIGINT_TEMP(tmp, modulus->n << 1, modmul_ctx, err_check, end_stat);
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