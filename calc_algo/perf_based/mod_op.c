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


//* ----- WORKSPACE FUNCTIONS ---- *//
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
    dnml_status echeck;
    size_t cmodmul_mark = scratch_mark(&modmul_ctx);
    BIGINT_TEMP(prod, (a->n + b->n), modmul_ctx, cmodmul_mark, echeck, err,);
    BIGINT_TEMP(tmp, (a->n + b->n), modmul_ctx, cmodmul_mark, echeck, err,);
    __BIGINT_MUL_DISPATCH__(a, b, &prod, modmul_ctx, &echeck); SCRATCH_OVF(echeck, modmul_ctx, cmodmul_mark, err,);
    __BIGINT_MOD_DISPATCH__(&prod, mod, res, &tmp, modmul_ctx, &echeck); 
    SCRATCH_OVF(echeck, modmul_ctx, cmodmul_mark, err,); 
    scratch_rewind(&modmul_ctx, cmodmul_mark); *err = BIGINT_SUCCESS;
}
void __BIGINT_MONTMUL__(PCONST_BIGINT a, PCONST_BIGINT b, mont_ctx ctx, P_BIGINT res, calc_ctx montmul_ctx, dnml_status *err) {
    dnml_status echeck;
    size_t montmul_mark = scratch_mark(&montmul_ctx);
    BIGINT_TEMP(t, (2*ctx.k + 1), montmul_ctx, montmul_mark, echeck, err,);
    __BIGINT_MUL_DISPATCH__(a, b, &t, montmul_ctx, &echeck); SCRATCH_OVF(echeck, montmul_ctx, montmul_mark, err,);
    __BIGINT_MONT_REDC__(&t, ctx, res); scratch_rewind(&montmul_ctx, montmul_mark); *err = BIGINT_SUCCESS;
}
void __BIGINT_BIN_MODEXP__(PCONST_BIGINT base, PCONST_BIGINT exp, PCONST_BIGINT mod, P_BIGINT res, calc_ctx bin_ctx, dnml_status *err) {
    dnml_status echeck;
    size_t binexp_mark = scratch_mark(&bin_ctx);
    BIGINT_TEMP(buf, base->n, bin_ctx, binexp_mark, echeck, err,);
    BIGINT_TEMP(tmp_res, mod->n, bin_ctx, binexp_mark, echeck, err,);
    BIGINT_TEMP(tmp_exp, exp->n, bin_ctx, binexp_mark, echeck, err,);
    BIGINT_TEMP(tmp_base, mod->n, bin_ctx, binexp_mark, echeck, err,);
    tmp_res.limbs[0] = 1; memcpy(tmp_exp.limbs, exp->limbs, exp->n * U64_BYTES);
    __BIGINT_MOD_DISPATCH__(base, mod, &tmp_base, &buf, bin_ctx, &echeck); 
    SCRATCH_OVF(echeck, bin_ctx, binexp_mark, err,);
    while (tmp_exp.n > 0) {
        if (tmp_exp.limbs[0] & 1) {
            __BIGINT_CMODMUL__(&tmp_res, &tmp_base, mod, &tmp_res, bin_ctx, &echeck); 
            SCRATCH_OVF(echeck, bin_ctx, binexp_mark, err,);
        } 
        __BIGINT_CMODMUL__(&tmp_base, &tmp_base, mod, &tmp_base, bin_ctx, &echeck); 
        SCRATCH_OVF(echeck, bin_ctx, binexp_mark, err,); __BIGINT_INTERNAL_RSHIFT__(&tmp_exp, 1);
    } __BIGINT_INTERNAL_COPY__(res, &tmp_res); scratch_rewind(&bin_ctx, binexp_mark); *err = BIGINT_SUCCESS;
}
void __BIGINT_MBIN_MODEXP__(PCONST_BIGINT base, PCONST_BIGINT exp, PCONST_BIGINT mod, P_BIGINT res, calc_ctx bin_ctx, dnml_status *err) {
    /* --- 1. SETUP ---- */ dnml_status echeck;
    mont_ctx modexp_contx = { .n = mod, .nprime = __MODINV_UI64__(mod->limbs[0]), .k = mod->n }; 
    size_t binexp_mark = scratch_mark(&bin_ctx), max_tsize = max((mod->n << 1), max(base->n, exp->n));
    BIGINT_TEMP(r, max_tsize, bin_ctx, binexp_mark, echeck, err,); r.n = mod->n + 1;
    BIGINT_TEMP(r_mod_n, mod->n, bin_ctx, binexp_mark, echeck, err,);
    BIGINT_TEMP(tmp, max_tsize, bin_ctx, binexp_mark, echeck, err,); r.limbs[mod->n] = 1; 
    __BIGINT_MOD_DISPATCH__(&r, mod, &r_mod_n, &tmp, bin_ctx, &echeck); SCRATCH_OVF(echeck, bin_ctx, binexp_mark, err,);
    __BIGINT_MUL_DISPATCH__(&r_mod_n, &r_mod_n, &tmp, bin_ctx, &echeck); SCRATCH_OVF(echeck, bin_ctx, binexp_mark, err,);
    __BIGINT_MOD_DISPATCH__(&tmp, mod, &tmp, &r, bin_ctx, &echeck); SCRATCH_OVF(echeck, bin_ctx, binexp_mark, err,);
    modexp_contx.r2 = &tmp;

    //* ----- 2. MAIN LOOP ----- *//
    PCONST_BIGINT r2 = modexp_contx.r2;
    BIGINT_TEMP(tmp_res, mod->n, bin_ctx, binexp_mark, echeck, err,);
    BIGINT_TEMP(tmp_exp, exp->n, bin_ctx, binexp_mark, echeck, err,);
    BIGINT_TEMP(tmp_base, mod->n, bin_ctx, binexp_mark, echeck, err,);
    tmp_res.limbs[0] = 1; memcpy(tmp_exp.limbs, exp->limbs, exp->n * U64_BYTES);
    __BIGINT_MOD_DISPATCH__(base, mod, &tmp_base, &r, bin_ctx, &echeck); SCRATCH_OVF(echeck, bin_ctx, binexp_mark, err,);
    __BIGINT_MONTMUL__(&tmp_res, r2, modexp_contx, &tmp_res, bin_ctx, &echeck); SCRATCH_OVF(echeck, bin_ctx, binexp_mark, err,);
    __BIGINT_MONTMUL__(&tmp_base, r2, modexp_contx, &tmp_base, bin_ctx, &echeck); SCRATCH_OVF(echeck, bin_ctx, binexp_mark, err,);
    while (tmp_exp.n > 0) {
        if (tmp_exp.limbs[0] & 1) {
            __BIGINT_MONTMUL__(&tmp_res, &tmp_base, modexp_contx, &tmp_res, bin_ctx, &echeck); 
            SCRATCH_OVF(echeck, bin_ctx, binexp_mark, err,);
        } 
        __BIGINT_MONTMUL__(&tmp_base, &tmp_base, modexp_contx, &tmp_base, bin_ctx, &echeck);
        SCRATCH_OVF(echeck, bin_ctx, binexp_mark, err,); __BIGINT_INTERNAL_RSHIFT__(&tmp_exp, 1);
    } uint64_t a[1] = {1};
    __BIGINT_MONTMUL__(&tmp_res, &(bigInt){.limbs = a, .n = 1, .cap = 1, .sign = 1}, modexp_contx, res, bin_ctx, &echeck); 
    SCRATCH_OVF(echeck, bin_ctx, binexp_mark, err,); scratch_rewind(&bin_ctx, binexp_mark); *err = BIGINT_SUCCESS;
}
void __BIGINT_FIX_MODEXP__(PCONST_BIGINT base, PCONST_BIGINT exp, PCONST_BIGINT mod, P_BIGINT res, calc_ctx fix_ctx, dnml_status *err) {}
void __BIGINT_SLIDE_MODEXP__(PCONST_BIGINT base, PCONST_BIGINT exp, PCONST_BIGINT mod, P_BIGINT res, calc_ctx slide_ctx, dnml_status *err) {}
void __BIGINT_MODMUL_DISPATCH__(PCONST_BIGINT a, PCONST_BIGINT b, PCONST_BIGINT mod, P_BIGINT res, calc_ctx modmul_ctx, dnml_status *err) {
    if (mod->n <= BIGINT_CLASSICAL) __BIGINT_CMODMUL__(a, b, mod, res, modmul_ctx, err);
    else { dnml_status echeck;
        mont_ctx modmul_disp_ctx = { .n = mod, .nprime = __MODINV_UI64__(mod->limbs[0]), .k = mod->n };
        size_t modmul_disp_mark = scratch_mark(&modmul_ctx);
        BIGINT_TEMP(r, mod->n + 1, modmul_ctx, modmul_disp_mark, echeck, err,); r.n = mod->n + 1;
        BIGINT_TEMP(r_mod_n, mod->n, modmul_ctx, modmul_disp_mark, echeck, err,);
        BIGINT_TEMP(tmp, mod->n << 1, modmul_ctx, modmul_disp_mark, echeck, err,); r.limbs[mod->n] = 1; 
        __BIGINT_MOD_DISPATCH__(&r, mod, &r_mod_n, &tmp, modmul_ctx, &echeck); SCRATCH_OVF(echeck, modmul_ctx, modmul_disp_mark, err,);
        __BIGINT_MUL_DISPATCH__(&r_mod_n, &r_mod_n, &tmp, modmul_ctx, &echeck); SCRATCH_OVF(echeck, modmul_ctx, modmul_disp_mark, err,);
        __BIGINT_MOD_DISPATCH__(&tmp, mod, &tmp, &r, modmul_ctx, &echeck); SCRATCH_OVF(echeck, modmul_ctx, modmul_disp_mark, err,);
        modmul_disp_ctx.r2 = &tmp; __BIGINT_MONTMUL__(a, b, modmul_disp_ctx, res, modmul_ctx, &echeck);
        SCRATCH_OVF(echeck, modmul_ctx, modmul_disp_mark, err,); scratch_rewind(&modmul_ctx, modmul_disp_mark); 
        *err = BIGINT_SUCCESS;
    }
}
void __BIGINT_MODEXP_DISPATCH__(PCONST_BIGINT base, PCONST_BIGINT exp, PCONST_BIGINT mod, P_BIGINT res, calc_ctx modexp_ctx, dnml_status *err) {
    if (mod->n < BIGINT_MOD_BINARY) __BIGINT_BIN_MODEXP__(base, exp, mod, res, modexp_ctx, err);
    else if (mod->n < BIGINT_MONT_BINARY) __BIGINT_MBIN_MODEXP__(base, exp, mod, res, modexp_ctx, err);
    else if (mod->n < BIGINT_MOD_FIXED) __BIGINT_FIX_MODEXP__(base, exp, mod, res, modexp_ctx, err);
    else __BIGINT_SLIDE_MODEXP__(base, exp, mod, res, modexp_ctx, err);
}
