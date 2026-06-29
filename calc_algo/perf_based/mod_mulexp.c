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



#include "mod_mulexp.h"
#include <debug_util.h>
#include "../../util/aconv_macros.h"
/** ----------- General BigInt Modular Multiplication And Exponentiation -----------
 * THIS FILE CONTAINS THE FOLLOWING ALGORITHMS + OPERATIONS:
 *
 *  Operations:
 *      - Modular Multiplication
 *      - Modular Exponentiation
 *  Algorithms:
 *      - Montgomery Modular Multiplication (Modular)
 *      - Binary Exponentiation (Modular)
 *      - Fixed-Window/k-ary Exponentiation (Modular)
 *      - Sliding-window Exponentiation (Modular)
 *
 * This file is generally the main and only algorithm file for bigInt modular multiplication and 
 * exponentiation, containing the modular multiplication and exponentiation algorithm dispatcher, 
 * as well as the workspace sizing function dispatcher.
 */
/* ---------- WORKSPACE FUNCTIONS ------ */
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
    size_t rsize_tmpsize = max_tsize << 1, rmodn_size = mod_size;
    size_t tmpexp_size = pow_size;
    // Low-level Function Stackframe
    size_t max_frame = max(
        __BIGINT_MONTMUL_WS__(mod_size, mod_size, (mont_ctx){.k = mod_size}), max(
            __BIGINT_MUL_WS__(rmodn_size, rmodn_size), __BIGINT_MOD_WS__(max_tsize, mod_size)
        )
    ); return rsize_tmpsize + rmodn_size + base_size + tmpexp_size + max_frame;
}
size_t __BIGINT_FIX_MODEXP_WS__(size_t base_size, size_t mod_size, size_t pow_size, uint8_t k) {
    // Montgomery Domain Setup + Precomputation Setup
    size_t max_tsize = max(mod_size << 1, max(base_size, pow_size));
    size_t rsize_tmpsize = max_tsize << 1, rmodn_size = mod_size;
    size_t table_pows = mod_size * (UINT64_C(1) << (k - 1));
    size_t x2mod_size = mod_size;
    // Function calls
    size_t montmul_max = __BIGINT_MONTMUL_WS__(mod_size, mod_size, (mont_ctx){.k = mod_size});
    size_t mod_max = max(__BIGINT_MOD_WS__(mod_size << 1, mod_size), __BIGINT_MOD_WS__(base_size, mod_size));
    size_t mul_max = __BIGINT_MUL_WS__(mod_size, mod_size);
    size_t fcall_max = max(montmul_max, max(mod_max, mul_max));
    return rsize_tmpsize + rmodn_size + table_pows + x2mod_size + fcall_max;
}
size_t __BIGINT_SLIDE_MODEXP_WS__(size_t base_size, size_t mod_size, size_t pow_size, uint8_t k) {
    // Montgomery Domain Setup + Precomputation Setup
    size_t max_tsize = max(mod_size << 1, max(base_size, pow_size));
    size_t rsize_tmpsize = max_tsize << 1, rmodn_size = mod_size;
    size_t table_pows = mod_size * (UINT64_C(1) << (k - 1));
    size_t x2mod_size = mod_size;
    // Function calls
    size_t montmul_max = __BIGINT_MONTMUL_WS__(mod_size, mod_size, (mont_ctx){.k = mod_size});
    size_t mod_max = max(__BIGINT_MOD_WS__(mod_size << 1, mod_size), __BIGINT_MOD_WS__(base_size, mod_size));
    size_t mul_max = __BIGINT_MUL_WS__(mod_size, mod_size);
    size_t fcall_max = max(montmul_max, max(mod_max, mul_max));
    return rsize_tmpsize + rmodn_size + table_pows + x2mod_size + fcall_max;
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
    else if (mod_size <= BIGINT_MOD_FIXED) return __BIGINT_FIX_MODEXP_WS__(base_size, mod_size, pow_size, 6);
    else return __BIGINT_SLIDE_MODEXP_WS__(base_size, mod_size, pow_size, 7);
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
    r_mod_n.limbs[0] = 1; r_mod_n.n = 1; r_mod_n.sign = 1; memcpy(tmp_exp.limbs, exp->limbs, exp->n * U64_BYTES);
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
    SCRATCH_OVF(echeck, bin_ctx, binexp_mark, err,); __BIGINT_INTERNAL_COPY__(res, &r_mod_n);
    scratch_rewind(&bin_ctx, binexp_mark); *err = BIGINT_SUCCESS;
}
void __BIGINT_FIX_MODEXP__(PCONST_BIGINT base, PCONST_BIGINT exp, PCONST_BIGINT mod, P_BIGINT res, uint8_t k, calc_ctx fix_ctx, dnml_status *err) {
    /* --- 1. MONTGOMERY DOMAIN SETUP ---- */ dnml_status echeck = BIGINT_SUCCESS;
    mont_ctx modexp_contx = { .n = mod, .nprime = __MODINV_UI64__(mod->limbs[0]), .k = mod->n }; 
    size_t fixexp_mark = scratch_mark(&fix_ctx), max_tsize = max((mod->n << 1), max(base->n, exp->n));
    int8_t cmp_res = __BIGINT_INTERNAL_COMP__(base, mod);
    BIGINT_TEMP(r, max_tsize, fix_ctx, fixexp_mark, echeck, err,); r.n = mod->n + 1; r.limbs[mod->n] = 1; 
    BIGINT_TEMP(r_mod_n, mod->n, fix_ctx, fixexp_mark, echeck, err,);
    BIGINT_TEMP(tmp, max_tsize, fix_ctx, fixexp_mark, echeck, err,);
    __BIGINT_MOD_DISP__(&r, mod, &r_mod_n, &tmp, fix_ctx, &echeck); SCRATCH_OVF(echeck, fix_ctx, fixexp_mark, err,);
    __BIGINT_MUL_DISP__(&r_mod_n, &r_mod_n, &tmp, fix_ctx, &echeck); SCRATCH_OVF(echeck, fix_ctx, fixexp_mark, err,);
    __BIGINT_MOD_DISP__(&tmp, mod, &tmp, &r, fix_ctx, &echeck); SCRATCH_OVF(echeck, fix_ctx, fixexp_mark, err,);
    modexp_contx.r2 = &tmp;

    //* -------- 2. PRECOMPUTATION TABLE SETUP -------- *//
    size_t table_size = UINT64_C(1) << (k - 1); bigInt table[table_size];
    __BIGINT_MOD_DISP__(base, mod, &r_mod_n, &r, fix_ctx, &echeck); SCRATCH_OVF(echeck, fix_ctx, fixexp_mark, err,); 
    table[0] = r; /**/ BIGINT_TEMP(x2_mod, mod->n, fix_ctx, fixexp_mark, echeck, err,);
    __BIGINT_MONTMUL__(&r_mod_n, &r_mod_n, modexp_contx, &x2_mod, fix_ctx, &echeck);
    for (size_t i = 1; i < table_size; ++i) {
        table[i].limbs = scratch_alloc(&fix_ctx, mod->n, &echeck); SCRATCH_OVF(echeck, fix_ctx, fixexp_mark, err,);
        table[i].cap = mod->n; table[i].sign = 1; table[i].n = 0;
        __BIGINT_MONTMUL__(&table[i-1], &x2_mod, modexp_contx, &table[i], fix_ctx, &echeck);
        SCRATCH_OVF(echeck, fix_ctx, fixexp_mark, err,);
    }

    //* ------------- 3. MAIN LOOP ------------- *//
    // Quick setup before the loop
    const bigInt *const r2 = modexp_contx.r2; // r_mod_n we will be used inplace for tmp_res
    memset(&r_mod_n.limbs[1], 0, r_mod_n.n * U64_BYTES); // in which both bigInts have the same cap as mod->n
    r_mod_n.limbs[0] = 1; r_mod_n.n = 1; r_mod_n.sign = 1;
    __BIGINT_MONTMUL__(&r_mod_n, r2, modexp_contx, &r_mod_n, fix_ctx, &echeck); SCRATCH_OVF(echeck, fix_ctx, fixexp_mark, err,);
    size_t total_bits = (exp->n << 6) - __CLZ_UI64__(exp->limbs[exp->n - 1]); /**/ size_t bit_idx = total_bits - 1;
    while (bit_idx - 1) {
        // Square tmp_res curr_k times to reserve space for multiplication of potentially odd powers later
        uint8_t curr_k = (bit_idx < k) ? (uint8_t)(bit_idx) : k; /**/ bit_idx -= curr_k;
        for (uint8_t s = 0; s < curr_k; ++s) {
            __BIGINT_MONTMUL__(&r_mod_n, &r_mod_n, modexp_contx, &r_mod_n, fix_ctx, &echeck);
            SCRATCH_OVF(echeck, fix_ctx, fixexp_mark, err,);
        }

        // Extracting the k-sized window from exp
        size_t limb_idx = (size_t)(bit_idx - 1) >> 6;
        uint8_t bit_offset = (uint8_t)((bit_idx - 1) & 63);
        uint64_t winval = exp->limbs[limb_idx] >> bit_offset;
        if (bit_offset + curr_k > U64_BITS && limb_idx + 1 < exp->n) {
            winval |= exp->limbs[limb_idx + 1] << (U64_BITS - bit_offset);
        } winval &= ((UINT64_C(1) << curr_k) - 1);

        // Perform table-lookup multiplication
        if (winval) { // If the winow value is zero, then our squaring already does its job
            // Optimize by skipping trailing zeros
            uint8_t winval_ctz = __CTZ_UI64__(winval); /**/ winval >> winval_ctz;
            __BIGINT_MONTMUL__(&r_mod_n, &table[winval >> 1], modexp_contx, &r_mod_n, fix_ctx, &echeck);
            SCRATCH_OVF(echeck, fix_ctx, fixexp_mark, err,);    
            // Re-apply the trailign zeros factors back (each 0 indicates a power of 2)
            for (uint8_t s = 0; s < winval_ctz; ++s) { 
                __BIGINT_MONTMUL__(&r_mod_n, &r_mod_n, modexp_contx, &r_mod_n, fix_ctx, &echeck);
                SCRATCH_OVF(echeck, fix_ctx, fixexp_mark, err,);
            }
        }
    } uint64_t a[1] = {1};
    __BIGINT_MONTMUL__(&r_mod_n, &(bigInt){.limbs = a, .n = 1, .cap = 1, .sign = 1}, modexp_contx, res, fix_ctx, &echeck);
    SCRATCH_OVF(echeck, fix_ctx, fixexp_mark, err,); __BIGINT_INTERNAL_COPY__(res, &r_mod_n);
    scratch_rewind(&fix_ctx, fixexp_mark); *err = BIGINT_SUCCESS;
}
void __BIGINT_SLIDE_MODEXP__(PCONST_BIGINT base, PCONST_BIGINT exp, PCONST_BIGINT mod, P_BIGINT res, uint8_t k, calc_ctx slide_ctx, dnml_status *err) {
    /* --- 1. MONTGOMERY DOMAIN SETUP ---- */ dnml_status echeck = BIGINT_SUCCESS;
    mont_ctx modexp_contx = { .n = mod, .nprime = __MODINV_UI64__(mod->limbs[0]), .k = mod->n }; 
    size_t slide_mark = scratch_mark(&slide_ctx), max_tsize = max((mod->n << 1), max(base->n, exp->n));
    int8_t cmp_res = __BIGINT_INTERNAL_COMP__(base, mod);
    BIGINT_TEMP(r, max_tsize, slide_ctx, slide_mark, echeck, err,); r.n = mod->n + 1;
    BIGINT_TEMP(r_mod_n, mod->n, slide_ctx, slide_mark, echeck, err,);
    BIGINT_TEMP(tmp, max_tsize, slide_ctx, slide_mark, echeck, err,); r.limbs[mod->n] = 1; 
    __BIGINT_MOD_DISP__(&r, mod, &r_mod_n, &tmp, slide_ctx, &echeck); SCRATCH_OVF(echeck, slide_ctx, slide_mark, err,);
    __BIGINT_MUL_DISP__(&r_mod_n, &r_mod_n, &tmp, slide_ctx, &echeck); SCRATCH_OVF(echeck, slide_ctx, slide_mark, err,);
    __BIGINT_MOD_DISP__(&tmp, mod, &tmp, &r, slide_ctx, &echeck); SCRATCH_OVF(echeck, slide_ctx, slide_mark, err,);
    modexp_contx.r2 = &tmp;

    //* -------- 2. PRECOMPUTATION TABLE SETUP -------- *//
    size_t table_size = UINT64_C(1) << (k - 1); bigInt table[table_size];
    __BIGINT_MOD_DISP__(base, mod, &r_mod_n, &r, slide_ctx, &echeck); SCRATCH_OVF(echeck, slide_ctx, slide_mark, err,); 
    table[0] = r; /**/ BIGINT_TEMP(x2_mod, mod->n, slide_ctx, slide_mark, echeck, err,);
    __BIGINT_MONTMUL__(&r_mod_n, &r_mod_n, modexp_contx, &x2_mod, slide_ctx, &echeck);
    for (size_t i = 1; i < table_size; ++i) {
        table[i].limbs = scratch_alloc(&slide_ctx, mod->n, &echeck); SCRATCH_OVF(echeck, slide_ctx, slide_mark, err,);
        table[i].cap = mod->n; table[i].sign = 1; table[i].n = 0;
        __BIGINT_MONTMUL__(&table[i-1], &x2_mod, modexp_contx, &table[i], slide_ctx, &echeck);
        SCRATCH_OVF(echeck, slide_ctx, slide_mark, err,);
    }

    //* --------------- 3. MAIN LOOP --------------- *//
    const bigInt *const r2 = modexp_contx.r2; // r_mod_n we will be used inplace for tmp_res
    memset(&r_mod_n.limbs[1], 0, r_mod_n.n * U64_BYTES); // in which both bigInts have the same cap as mod->n
    r_mod_n.limbs[0] = 1; r_mod_n.n = 1; r_mod_n.sign = 1;
    __BIGINT_MONTMUL__(&r_mod_n, r2, modexp_contx, &r_mod_n, slide_ctx, &echeck); SCRATCH_OVF(echeck, slide_ctx, slide_mark, err,);
    size_t total_bits = (exp->n << 6) - __CLZ_UI64__(exp->limbs[exp->n - 1]); /**/ size_t bit_idx = total_bits - 1;
    while (bit_idx) {
        size_t limb_idx = (size_t)(bit_idx - 1) >> 6;
        uint8_t bit_offset = (uint8_t)(bit_idx - 1 & 63);
        uint8_t curr_bit = exp->limbs[limb_idx] & (UINT64_C(1) << (bit_offset - 1));
        if (!curr_bit) { // Bit is 0 ---> Square (because 0-bits indicates powers of 2)
            __BIGINT_MONTMUL__(&r_mod_n, &r_mod_n, modexp_contx, &r_mod_n, slide_ctx, &echeck);
            SCRATCH_OVF(echeck, slide_ctx, slide_mark, err,); /**/ --bit_idx;
        } else { // Bit is 1 --> Odd power
            // Extracting the current window
            size_t s = max(((int8_t)bit_offset - (int8_t)k + 1), 0); // Max window size
            // Scan ahead to locate the lowest 1-bit within range [bit_idx : bit_idx-s]
            size_t r = bit_idx - 2, l = bit_idx - 1;
            while (r >= s) {
                size_t l_limb = (size_t)r >> 6;
                uint8_t l_boff = (uint8_t)(r & 63);
                if ((exp->limbs[l_limb] >> l_boff) & 1) l = r;
                --r;
            }
            
            // Squaring (this is actually to reserve space)
            size_t winlen = bit_idx - l;
            for (size_t s = 0; s < winlen; ++s) {
                __BIGINT_MONTMUL__(&r_mod_n, &r_mod_n, modexp_contx, &r_mod_n, slide_ctx, &echeck);
                SCRATCH_OVF(echeck, slide_ctx, slide_mark, err,);
            }

            // Multiplying the odd power by the lookup table entry
            size_t limb_idx = (size_t)(l) >> 6;
            size_t bit_offset = (uint8_t)((l) & 63);
            uint64_t winval = exp->limbs[limb_idx] >> bit_offset;
            if (bit_offset + winlen > U64_BITS && limb_idx + 1 < exp->n) {
                winval |= exp->limbs[limb_idx + 1] << (U64_BITS - bit_offset);
            } winval &= ((UINT64_C(1) << winlen) - 1);
            __BIGINT_MONTMUL__(&r_mod_n, &table[winval >> 1], modexp_contx, &r_mod_n, slide_ctx, &echeck);
            SCRATCH_OVF(echeck, slide_ctx, slide_mark, err,); /**/ if (!l) break; /**/ bit_idx = l - 1;
        }
    } uint64_t a[1] = {1};
    __BIGINT_MONTMUL__(&r_mod_n, &(bigInt){.limbs = a, .n = 1, .cap = 1, .sign = 1}, modexp_contx, res, slide_ctx, &echeck);
    SCRATCH_OVF(echeck, slide_ctx, slide_mark, err,); __BIGINT_INTERNAL_COPY__(res, &r_mod_n);
    scratch_rewind(&slide_ctx, slide_mark); *err = BIGINT_SUCCESS;
}
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
    else if (mod->n < BIGINT_MOD_FIXED) __BIGINT_FIX_MODEXP__(base, exp, mod, res, 7, modexp_ctx, err);
    else __BIGINT_SLIDE_MODEXP__(base, exp, mod, res, 6, modexp_ctx, err);
}
