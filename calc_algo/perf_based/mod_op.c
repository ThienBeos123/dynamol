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
#include <debug_util.h>
#include "../../util/aconv_macros.h"


//* ----- WORKSPACE FUNCTIONS ---- *//
size_t __BIGINT_CMODMUL_WS__(size_t a_size, size_t b_size, size_t mod_size) {
    size_t raw_size = (a_size + b_size) << 1;
    size_t fcall_size = max(
        __BIGINT_MUL_WS__(a_size, b_size),
        __BIGINT_MOD_WS__((a_size + b_size), mod_size)
    ); return raw_size + fcall_size;
}
size_t __BIGINT_MONTMUL_WS__(size_t a_size, size_t b_size, mont_ctx ctx) {
    size_t raw_size = (ctx.k << 1) + 1;
    size_t mul_size = __BIGINT_MUL_WS__(a_size, b_size);
    return raw_size + mul_size;
}
size_t __BIGINT_BIN_MODEXP_WS__(size_t base_size, size_t mod_size, size_t pow_size) {
    size_t tres_size = max(mod_size, base_size); // Impossible to fully dictate through purely size parameters
    size_t raw_size = tres_size + pow_size + mod_size;
    size_t fcall_size = max(__BIGINT_MOD_WS__(base_size, mod_size), __BIGINT_CMODMUL_WS__(mod_size, mod_size, mod_size));
    return raw_size + fcall_size;
}
size_t __BIGINT_MBIN_MODEXP_WS__(size_t base_size, size_t mod_size, size_t pow_size) {
    // Binary ModExp's objects
    size_t max_tsize = max(mod_size << 1, max(base_size, pow_size));
    size_t rsize_tmpsize = max_tsize, rmodn_size = mod_size;
    size_t ressize_basesize = mod_size << 1, tmpexp_size = pow_size;
    // Low-level Function Stackframe
    size_t max_frame = max(
        __BIGINT_MONTMUL_WS__(mod_size, mod_size, (mont_ctx){.k = mod_size}), max(
            __BIGINT_MUL_WS__(rmodn_size, rmodn_size), __BIGINT_MOD_WS__(max_tsize, mod_size)
        )
    ); return rsize_tmpsize + rmodn_size + ressize_basesize + tmpexp_size + max_frame;
}
size_t __BIGINT_FIX_MODEXP_WS__(size_t base_size, size_t mod_size, size_t pow_size) { return 0; }
size_t __BIGINT_SLIDE_MODEXP_WS__(size_t base_size, size_t mod_size, size_t pow_size) { return 0; }
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
    else if (mod_size <= BIGINT_MOD_FIXED) return __BIGINT_FIX_MODEXP_WS__(base_size, mod_size, pow_size);
    else return __BIGINT_SLIDE_MODEXP_WS__(base_size, mod_size, pow_size);
}


//* --------- ALGORITHMS --------- *//
void __BIGINT_CMODMUL__(PCONST_BIGINT a, PCONST_BIGINT b, PCONST_BIGINT mod, P_BIGINT res, calc_ctx modmul_ctx, dnml_status *err) {
    dnml_status echeck = BIGINT_SUCCESS; size_t cmodmul_mark = scratch_mark(&modmul_ctx);
    BIGINT_TEMP(prod, (a->n + b->n), modmul_ctx, cmodmul_mark, echeck, err,);
    BIGINT_TEMP(tmp, (a->n + b->n), modmul_ctx, cmodmul_mark, echeck, err,);
    const bigInt *chosen_a = a, *chosen_b = b; bigInt a_mod_n = {0}, b_mod_n = {0};
    if (a->n < mod->n << 1) {
        limb_t *amodn_limbs = scratch_alloc(&modmul_ctx, mod->n, &echeck);
        if (echeck != DARENA_SUCCESS) { scratch_rewind(&modmul_ctx, cmodmul_mark); *err = DARENA_OVERFLOW; return; }
        a_mod_n.limbs = amodn_limbs; a_mod_n.n = 0, a_mod_n.sign = 1; a_mod_n.cap = mod->n;
        __BIGINT_MOD_DISP__(a, mod, &a_mod_n, &tmp, modmul_ctx, &echeck); 
        SCRATCH_OVF(echeck, modmul_ctx, cmodmul_mark, err,); chosen_a = &a_mod_n;
    }
    if (b->n < mod->n << 1) {
        limb_t *bmodn_limbs = scratch_alloc(&modmul_ctx, mod->n, &echeck);
        if (echeck != DARENA_SUCCESS) { scratch_rewind(&modmul_ctx, cmodmul_mark); *err = DARENA_OVERFLOW; return; }
        b_mod_n.limbs = bmodn_limbs; b_mod_n.n = 0, b_mod_n.sign = 1; b_mod_n.cap = mod->n;
        __BIGINT_MOD_DISP__(b, mod, &b_mod_n, &tmp, modmul_ctx, &echeck); 
        SCRATCH_OVF(echeck, modmul_ctx, cmodmul_mark, err,); chosen_b = &b_mod_n;
    }
    __BIGINT_MUL_DISP__(chosen_a, chosen_b, &prod, modmul_ctx, &echeck); SCRATCH_OVF(echeck, modmul_ctx, cmodmul_mark, err,);
    __BIGINT_MOD_DISP__(&prod, mod, res, &tmp, modmul_ctx, &echeck); SCRATCH_OVF(echeck, modmul_ctx, cmodmul_mark, err,);
    scratch_rewind(&modmul_ctx, cmodmul_mark); *err = BIGINT_SUCCESS;
}
void __BIGINT_MONTMUL__(PCONST_BIGINT a, PCONST_BIGINT b, mont_ctx ctx, P_BIGINT res, calc_ctx montmul_ctx, dnml_status *err) {
    dnml_status echeck = BIGINT_SUCCESS; size_t montmul_mark = scratch_mark(&montmul_ctx);
    BIGINT_TEMP(t, ((ctx.k << 1) + 1), montmul_ctx, montmul_mark, echeck, err,);
    __BIGINT_MUL_DISP__(a, b, &t, montmul_ctx, &echeck); SCRATCH_OVF(echeck, montmul_ctx, montmul_mark, err,);
    __BIGINT_MONT_REDC__(&t, ctx, res); scratch_rewind(&montmul_ctx, montmul_mark); *err = BIGINT_SUCCESS;
}
void __BIGINT_BIN_MODEXP__(PCONST_BIGINT base, PCONST_BIGINT exp, PCONST_BIGINT mod, P_BIGINT res, calc_ctx bin_ctx, dnml_status *err) {
    dnml_status echeck = BIGINT_SUCCESS; size_t binexp_mark = scratch_mark(&bin_ctx);
    int8_t cmp_res = __BIGINT_INTERNAL_COMP__(base, mod); size_t tres_size = (cmp_res > 0) ? base->n : mod->n;
    BIGINT_TEMP(tmp_res, tres_size, bin_ctx, binexp_mark, echeck, err,);
    BIGINT_TEMP(tmp_exp, exp->n, bin_ctx, binexp_mark, echeck, err,);
    BIGINT_TEMP(tmp_base, mod->n, bin_ctx, binexp_mark, echeck, err,);
    memcpy(tmp_exp.limbs, exp->limbs, exp->n * U64_BYTES); 
    if (cmp_res < 0) memcpy(tmp_base.limbs, base->limbs, base->n * U64_BYTES);
    else if (cmp_res > 0) {
        __BIGINT_MOD_DISP__(base, mod, &tmp_base, &tmp_res, bin_ctx, &echeck); 
        SCRATCH_OVF(echeck, bin_ctx, binexp_mark, err,); memset(tmp_res.limbs, 0, tmp_res.n * U64_BYTES);
    } tmp_res.limbs[0] = 1;
    while (tmp_exp.n > 0) {
        if (tmp_exp.limbs[0] & 1) {
            __BIGINT_CMODMUL__(&tmp_res, &tmp_base, mod, &tmp_res, bin_ctx, &echeck); 
            SCRATCH_OVF(echeck, bin_ctx, binexp_mark, err,);
        } 
        __BIGINT_CMODMUL__(&tmp_base, &tmp_base, mod, &tmp_base, bin_ctx, &echeck); 
        SCRATCH_OVF(echeck, bin_ctx, binexp_mark, err,); __BIGINT_INTERNAL_RSHIFT__(&tmp_exp, 1);
    } __BIGINT_INTERNAL_TRIM_LZ__(&tmp_res); __BIGINT_INTERNAL_COPY__(res, &tmp_res);
    scratch_rewind(&bin_ctx, binexp_mark); *err = BIGINT_SUCCESS;
}
void __BIGINT_MBIN_MODEXP__(PCONST_BIGINT base, PCONST_BIGINT exp, PCONST_BIGINT mod, P_BIGINT res, calc_ctx bin_ctx, dnml_status *err) {
    /* --- 1. SETUP ---- */ dnml_status echeck = BIGINT_SUCCESS;
    mont_ctx modexp_contx = { .n = mod, .nprime = __MODINV_UI64__(mod->limbs[0]), .k = mod->n }; 
    size_t binexp_mark = scratch_mark(&bin_ctx), max_tsize = max((mod->n << 1), max(base->n, exp->n));
    int8_t cmp_res = __BIGINT_INTERNAL_COMP__(base, mod);
    BIGINT_TEMP(r, max_tsize, bin_ctx, binexp_mark, echeck, err,); r.n = mod->n + 1;
    BIGINT_TEMP(r_mod_n, mod->n, bin_ctx, binexp_mark, echeck, err,);
    BIGINT_TEMP(tmp, max_tsize, bin_ctx, binexp_mark, echeck, err,); r.limbs[mod->n] = 1; 
    __BIGINT_MOD_DISP__(&r, mod, &r_mod_n, &tmp, bin_ctx, &echeck); SCRATCH_OVF(echeck, bin_ctx, binexp_mark, err,);
    __BIGINT_MUL_DISP__(&r_mod_n, &r_mod_n, &tmp, bin_ctx, &echeck); SCRATCH_OVF(echeck, bin_ctx, binexp_mark, err,);
    __BIGINT_MOD_DISP__(&tmp, mod, &tmp, &r, bin_ctx, &echeck); SCRATCH_OVF(echeck, bin_ctx, binexp_mark, err,);
    modexp_contx.r2 = &tmp;

    //* ----- 2. MAIN LOOP ----- *//
    const bigInt *const r2 = modexp_contx.r2; // We will now re-use r_mod_n in place for tmp_res, in which
    memset(&r_mod_n.limbs[1], 0, r_mod_n.n * U64_BYTES); // both bigInts have the same capacity requirements of mod->n
    BIGINT_TEMP(tmp_exp, exp->n, bin_ctx, binexp_mark, echeck, err,);
    BIGINT_TEMP(tmp_base, mod->n, bin_ctx, binexp_mark, echeck, err,);
    r_mod_n.limbs[0] = 1; memcpy(tmp_exp.limbs, exp->limbs, exp->n * U64_BYTES);
    if (cmp_res < 0) memcpy(tmp_base.limbs, base->limbs, base->n * U64_BYTES);
    else if (cmp_res > 0) {
        __BIGINT_MOD_DISP__(base, mod, &tmp_base, &r, bin_ctx, &echeck); SCRATCH_OVF(echeck, bin_ctx, binexp_mark, err,);
    } __BIGINT_MONTMUL__(&r_mod_n, r2, modexp_contx, &r_mod_n, bin_ctx, &echeck); SCRATCH_OVF(echeck, bin_ctx, binexp_mark, err,);
    __BIGINT_MONTMUL__(&tmp_base, r2, modexp_contx, &tmp_base, bin_ctx, &echeck); SCRATCH_OVF(echeck, bin_ctx, binexp_mark, err,);
    while (tmp_exp.n > 0) {
        if (tmp_exp.limbs[0] & 1) {
            __BIGINT_MONTMUL__(&r_mod_n, &tmp_base, modexp_contx, &r_mod_n, bin_ctx, &echeck); 
            SCRATCH_OVF(echeck, bin_ctx, binexp_mark, err,);
        } 
        __BIGINT_MONTMUL__(&tmp_base, &tmp_base, modexp_contx, &tmp_base, bin_ctx, &echeck);
        SCRATCH_OVF(echeck, bin_ctx, binexp_mark, err,); __BIGINT_INTERNAL_RSHIFT__(&tmp_exp, 1);
    } uint64_t a[1] = {1};
    __BIGINT_MONTMUL__(&r_mod_n, &(bigInt){.limbs = a, .n = 1, .cap = 1, .sign = 1}, modexp_contx, res, bin_ctx, &echeck); 
    SCRATCH_OVF(echeck, bin_ctx, binexp_mark, err,); scratch_rewind(&bin_ctx, binexp_mark); *err = BIGINT_SUCCESS;
}
void __BIGINT_FIX_MODEXP__(PCONST_BIGINT base, PCONST_BIGINT exp, PCONST_BIGINT mod, P_BIGINT res, calc_ctx fix_ctx, dnml_status *err) {}
void __BIGINT_SLIDE_MODEXP__(PCONST_BIGINT base, PCONST_BIGINT exp, PCONST_BIGINT mod, P_BIGINT res, calc_ctx slide_ctx, dnml_status *err) {}
void __BIGINT_MODMUL_DISP__(PCONST_BIGINT a, PCONST_BIGINT b, PCONST_BIGINT mod, P_BIGINT res, calc_ctx modmul_ctx, dnml_status *err) {
    if (mod->n <= BIGINT_CLASSICAL) __BIGINT_CMODMUL__(a, b, mod, res, modmul_ctx, err);
    else { dnml_status echeck = BIGINT_SUCCESS;
        mont_ctx modmul_disp_ctx = { .n = mod, .nprime = __MODINV_UI64__(mod->limbs[0]), .k = mod->n };
        size_t modmul_disp_mark = scratch_mark(&modmul_ctx);
        BIGINT_TEMP(r, mod->n + 1, modmul_ctx, modmul_disp_mark, echeck, err,); r.n = mod->n + 1;
        BIGINT_TEMP(r_mod_n, mod->n, modmul_ctx, modmul_disp_mark, echeck, err,);
        BIGINT_TEMP(tmp, mod->n << 1, modmul_ctx, modmul_disp_mark, echeck, err,); r.limbs[mod->n] = 1; 
        __BIGINT_MOD_DISP__(&r, mod, &r_mod_n, &tmp, modmul_ctx, &echeck); SCRATCH_OVF(echeck, modmul_ctx, modmul_disp_mark, err,);
        __BIGINT_MUL_DISP__(&r_mod_n, &r_mod_n, &tmp, modmul_ctx, &echeck); SCRATCH_OVF(echeck, modmul_ctx, modmul_disp_mark, err,);
        __BIGINT_MOD_DISP__(&tmp, mod, &tmp, &r, modmul_ctx, &echeck); SCRATCH_OVF(echeck, modmul_ctx, modmul_disp_mark, err,);
        modmul_disp_ctx.r2 = &tmp; __BIGINT_MONTMUL__(a, b, modmul_disp_ctx, res, modmul_ctx, &echeck);
        SCRATCH_OVF(echeck, modmul_ctx, modmul_disp_mark, err,); scratch_rewind(&modmul_ctx, modmul_disp_mark); 
        *err = BIGINT_SUCCESS;
    }
}
void __BIGINT_MODEXP_DISP__(PCONST_BIGINT base, PCONST_BIGINT exp, PCONST_BIGINT mod, P_BIGINT res, calc_ctx modexp_ctx, dnml_status *err) {
    if (mod->n < BIGINT_MOD_BINARY) __BIGINT_BIN_MODEXP__(base, exp, mod, res, modexp_ctx, err);
    else if (mod->n < BIGINT_MONT_BINARY) __BIGINT_MBIN_MODEXP__(base, exp, mod, res, modexp_ctx, err);
    else if (mod->n < BIGINT_MOD_FIXED) __BIGINT_FIX_MODEXP__(base, exp, mod, res, modexp_ctx, err);
    else __BIGINT_SLIDE_MODEXP__(base, exp, mod, res, modexp_ctx, err);
}
