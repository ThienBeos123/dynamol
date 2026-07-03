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



#include "heap_modmulexp.h"
#include <debug_util.h>
#include "../../util/aconv_macros.h"
limb_t a[1] = {1}; // NO FUNCTION SHOULD MODIFY THIS ARRAY, EVER

/* BigInt Modular Multiplication AND Exponentiation Algorithms */
void __BIHEAP_CMODMUL__(PCONST_BIGINT a, PCONST_BIGINT b, PCONST_BIGINT mod, P_BIGINT res, dnml_status *err) {
    dnml_status echeck; bigInt *alloc_list[3], *early_free[3]; uint8_t alloc_cnt = 0, early_cnt = 0;
    BIHEAP_TEMP(prod, (a->n + b->n), echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    const bigInt *chosen_a = a, *chosen_b = b; bigInt a_mod_n = {0}, b_mod_n = {0};
    if (a->n > mod->n << 1) {
        echeck = __BIGINT_INTERNAL_LINIT__(&a_mod_n, mod->n);
        if (echeck == DNML_ALLOC_OOM) { _free_alloc_list(early_free, early_cnt); *err = DNML_ALLOC_OOM; return; }
        early_free[early_cnt++] = &a_mod_n; alloc_list[alloc_cnt++] = &a_mod_n;
        __BIHEAP_MOD_DISP__(a, mod, &a_mod_n, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
        chosen_a = &a_mod_n;
    }
    if (b->n > mod->n << 1) {
        echeck = __BIGINT_INTERNAL_LINIT__(&b_mod_n, mod->n);
        if (echeck == DNML_ALLOC_OOM) { _free_alloc_list(early_free, early_cnt); *err = DNML_ALLOC_OOM; return; }
        early_free[early_cnt++] = &b_mod_n; alloc_list[alloc_cnt++] = &b_mod_n;
        __BIHEAP_MOD_DISP__(b, mod, &b_mod_n, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
        chosen_a = &b_mod_n;
    }
    __BIHEAP_MUL_DISP__(chosen_a, chosen_b, &prod, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    __BIHEAP_MOD_DISP__(&prod, mod, res, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    _free_alloc_list(alloc_list, alloc_cnt); *err = BIGINT_SUCCESS;
}
void __BIHEAP_MONTMUL__(PCONST_BIGINT a, PCONST_BIGINT b, mont_ctx ctx, P_BIGINT res, dnml_status *err) {
    dnml_status echeck; bigInt *early_free[1]; uint8_t early_cnt = 0;
    BIHEAP_RET(t, (2*ctx.k + 1), echeck, err, early_free, early_cnt,);
    __BIHEAP_MUL_DISP__(a, b, &t, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    __BIHEAP_MONT_REDC__(&t, ctx, res); *err = BIGINT_SUCCESS;
}
void __BIHEAP_BIN_MODEXP__(PCONST_BIGINT base, PCONST_BIGINT exp, PCONST_BIGINT mod, P_BIGINT res, dnml_status *err) {
    int8_t cmp_res = __BIGINT_INTERNAL_COMP__(base, mod);
    dnml_status echeck; bigInt *alloc_list[2], *early_free[3]; uint8_t alloc_cnt = 0, early_cnt = 0;
    BIHEAP_RET(tmp_res, mod->n, echeck, err, early_free, early_cnt,);
    BIHEAP_TEMP(tmp_exp, exp->n, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    BIHEAP_TEMP(tmp_base, mod->n, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    tmp_res.limbs[0] = 1; memcpy(tmp_exp.limbs, exp->limbs, exp->n * U64_BYTES);
    if (cmp_res < 0) memcpy(tmp_base.limbs, base->limbs, base->n * U64_BYTES);
    else if (cmp_res > 0) { 
        __BIHEAP_MOD_DISP__(base, mod, &tmp_base, &echeck); 
        HEAP_OOM(echeck, err, early_free, early_cnt,);
    }
    while (tmp_exp.n > 0) {
        if (tmp_exp.limbs[0] & 1) {
            __BIHEAP_CMODMUL__(&tmp_res, &tmp_base, mod, &tmp_res, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
        } __BIHEAP_CMODMUL__(&tmp_base, &tmp_base, mod, &tmp_base, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
        __BIGINT_INTERNAL_RSHIFT__(&tmp_exp, 1);
    } __BIGINT_INTERNAL_MOVE__(res, &tmp_res); _free_alloc_list(alloc_list, alloc_cnt); *err = BIGINT_SUCCESS;
}
void __BIHEAP_MBIN_MODEXP__(PCONST_BIGINT base, PCONST_BIGINT exp, PCONST_BIGINT mod, P_BIGINT res, dnml_status *err) {
    //* ----------- 1. SETUP ----------- *//
    dnml_status echeck; bigInt *alloc_list[6], *early_free[6]; uint8_t alloc_cnt = 0, early_cnt = 0;
    mont_ctx modexp_contx = { .n = mod, .nprime = __MODINV_UI64__(mod->limbs[0]), .k = mod->n }; 
    size_t max_tsize = max((mod->n << 1), max(base->n, exp->n));
    BIHEAP_TEMP(r, mod->n + 1, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,); r.n = mod->n + 1;
    BIHEAP_TEMP(r_mod_n, mod->n, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    BIHEAP_TEMP(tmp, max_tsize, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,); r.limbs[mod->n] = 1; 
    __BIHEAP_MOD_DISP__(&r, mod, &r_mod_n, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    __BIHEAP_MUL_DISP__(&r_mod_n, &r_mod_n, &tmp, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    __BIHEAP_MOD_DISP__(&tmp, mod, &tmp, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    modexp_contx.r2 = &tmp;

    //* ----- 2. MAIN LOOP ----- *//
    int8_t cmp_res = __BIGINT_INTERNAL_COMP__(base, mod);
    const bigInt *const r2 = modexp_contx.r2; // We will now re-use r_mod_n in place for tmp_res, in which
    memset(&r_mod_n.limbs[1], 0, r_mod_n.n * U64_BYTES); // both bigints have the same capacity requirement of mod->n
    BIHEAP_TEMP(tmp_exp, exp->n, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    BIHEAP_TEMP(tmp_base, mod->n, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    r_mod_n.limbs[0] = 1; memcpy(tmp_exp.limbs, exp->limbs, exp->n * U64_BYTES);
    if (cmp_res < 0) memcpy(tmp_base.limbs, base->limbs, base->n * U64_BYTES);
    else if (cmp_res > 0) {__BIHEAP_MOD_DISP__(base, mod, &tmp_base, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,); }  
    __BIHEAP_MONTMUL__(&r_mod_n, r2, modexp_contx, &r_mod_n, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    __BIHEAP_MONTMUL__(&tmp_base, r2, modexp_contx, &tmp_base, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    while (tmp_exp.n > 0) {
        if (tmp_exp.limbs[0] & 1) {
            __BIHEAP_MONTMUL__(&r_mod_n, &tmp_base, modexp_contx, &r_mod_n, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
        } __BIHEAP_MONTMUL__(&tmp_base, &tmp_base, modexp_contx, &tmp_base, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,); 
        __BIGINT_INTERNAL_RSHIFT__(&tmp_exp, 1);
    }
    __BIHEAP_MONTMUL__(&r_mod_n, &(bigInt){.limbs = a, .n = 1, .cap = 1, .sign = 1}, modexp_contx, res, &echeck); 
    HEAP_OOM(echeck, err, early_free, early_cnt,); _free_alloc_list(alloc_list, alloc_cnt) ;*err = BIGINT_SUCCESS;
}



/* BigInt Modular Exponentiation - Windows Algorithms */
void __BIHEAP_FIX_MODEXP__(PCONST_BIGINT base, PCONST_BIGINT exp, PCONST_BIGINT mod, uint8_t k, P_BIGINT res, dnml_status *err) {
    /* --- 1. MONTGOMERY DOMAIN SETUP ---- */ dnml_status echeck = BIGINT_SUCCESS;
    bigInt *alloc_list[3] = {0}, *early_free[4] = {0}; uint8_t alloc_cnt = 0, early_cnt = 0;
    mont_ctx modexp_contx = { .n = mod, .nprime = __MODINV_UI64__(mod->limbs[0]), .k = mod->n }; 
    size_t max_tsize = max((mod->n << 1), max(base->n, exp->n)); /**/ int8_t cmp_res = __BIGINT_INTERNAL_COMP__(base, mod);
    BIHEAP_TEMP(r, mod->n + 1, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    BIHEAP_RET(r_mod_n, mod->n, echeck, err, early_free, early_cnt,); /**/ r.n = mod->n + 1; 
    BIHEAP_TEMP(tmp, max_tsize, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,); r.limbs[mod->n] = 1; 
    __BIHEAP_MOD_DISP__(&r, mod, &r_mod_n, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    __BIHEAP_MUL_DISP__(&r_mod_n, &r_mod_n, &tmp, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    __BIHEAP_MOD_DISP__(&tmp, mod, &tmp, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    modexp_contx.r2 = &tmp;

    //* -------- 2. PRECOMPUTATION TABLE SETUP -------- *//
    size_t table_size = UINT64_C(1) << (k - 1); bigInt table[table_size];
    __BIHEAP_MOD_DISP__(base, mod, &r, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,); 
    table[0] = r; /**/ BIHEAP_TEMP(x2_mod, mod->n, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    __BIHEAP_MONTMUL__(&r, &r, modexp_contx, &x2_mod, &echeck);
    for (size_t i = 1; i < table_size; ++i) {
        echeck = __BIGINT_INTERNAL_LINIT__(&table[i], mod->n); HEAP_OOM(echeck, err, early_free, early_cnt,);
        __BIHEAP_MONTMUL__(&table[i-1], &x2_mod, modexp_contx, &table[i], &echeck);
        HEAP_OOM(echeck, err, early_free, early_cnt,);
    }

    //* ------------- 3. MAIN LOOP ------------- *//
    // Quick setup before the loop
    const bigInt *const r2 = modexp_contx.r2; // r_mod_n we will be used inplace for tmp_res
    memset(&r_mod_n.limbs[1], 0, r_mod_n.n * U64_BYTES); // in which both bigInts have the same cap as mod->n
    r_mod_n.limbs[0] = 1; r_mod_n.n = 1; r_mod_n.sign = 1;
    __BIHEAP_MONTMUL__(&r_mod_n, r2, modexp_contx, &r_mod_n, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    size_t total_bits = (exp->n << 6) - __CLZ_UI64__(exp->limbs[exp->n - 1]); /**/ size_t bit_idx = total_bits - 1;
    while (bit_idx - 1) {
        // Square tmp_res curr_k times to reserve space for multiplication of potentially odd powers later
        uint8_t curr_k = (bit_idx < k) ? (uint8_t)(bit_idx) : k; /**/ bit_idx -= curr_k;
        for (uint8_t s = 0; s < curr_k; ++s) {
            __BIHEAP_MONTMUL__(&r_mod_n, &r_mod_n, modexp_contx, &r_mod_n, &echeck);
            HEAP_OOM(echeck, err, early_free, early_cnt,);
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
            __BIHEAP_MONTMUL__(&r_mod_n, &table[winval >> 1], modexp_contx, &r_mod_n, &echeck);
            HEAP_OOM(echeck, err, early_free, early_cnt,);    
            // Re-apply the trailign zeros factors back (each 0 indicates a power of 2)
            for (uint8_t s = 0; s < winval_ctz; ++s) { 
                __BIHEAP_MONTMUL__(&r_mod_n, &r_mod_n, modexp_contx, &r_mod_n, &echeck);
                HEAP_OOM(echeck, err, early_free, early_cnt,);
            }
        }
    } uint64_t a[1] = {1};
    __BIHEAP_MONTMUL__(&r_mod_n, &(bigInt){.limbs = a, .n = 1, .cap = 1, .sign = 1}, modexp_contx, res, &echeck);
    HEAP_OOM(echeck, err, early_free, early_cnt,); __BIGINT_INTERNAL_MOVE__(res, &r_mod_n);
    _free_alloc_list(alloc_list, alloc_cnt); *err = BIGINT_SUCCESS;
}
void __BIHEAP_SLIDE_MODEXP__(PCONST_BIGINT base, PCONST_BIGINT exp, PCONST_BIGINT mod, uint8_t k, P_BIGINT res, dnml_status *err) {
    /* --- 1. MONTGOMERY DOMAIN SETUP ---- */ dnml_status echeck = BIGINT_SUCCESS;
    bigInt *alloc_list[3] = {0}, *early_free[4] = {0}; uint8_t alloc_cnt = 0, early_cnt = 0;
    mont_ctx modexp_contx = { .n = mod, .nprime = __MODINV_UI64__(mod->limbs[0]), .k = mod->n }; 
    size_t max_tsize = max((mod->n << 1), max(base->n, exp->n)); /**/ int8_t cmp_res = __BIGINT_INTERNAL_COMP__(base, mod);
    BIHEAP_TEMP(r, mod->n + 1, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,); 
    BIHEAP_RET(r_mod_n, mod->n, echeck, err, early_free, early_cnt,); /**/ r.n = mod->n + 1;
    BIHEAP_TEMP(tmp, max_tsize, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,); r.limbs[mod->n] = 1; 
    __BIHEAP_MOD_DISP__(&r, mod, &r_mod_n, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    __BIHEAP_MUL_DISP__(&r_mod_n, &r_mod_n, &tmp, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    __BIHEAP_MOD_DISP__(&tmp, mod, &tmp, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    modexp_contx.r2 = &tmp;

    //* -------- 2. PRECOMPUTATION TABLE SETUP -------- *//
    size_t table_size = UINT64_C(1) << (k - 1); bigInt table[table_size];
    __BIHEAP_MOD_DISP__(base, mod, &r, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,); 
    table[0] = r; /**/ BIHEAP_TEMP(x2_mod, mod->n, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    __BIHEAP_MONTMUL__(&r, &r, modexp_contx, &x2_mod, &echeck);
    for (size_t i = 1; i < table_size; ++i) {
        echeck = __BIGINT_INTERNAL_LINIT__(&table[i], mod->n); HEAP_OOM(echeck, err, early_free, early_cnt,);
        __BIHEAP_MONTMUL__(&table[i-1], &x2_mod, modexp_contx, &table[i], &echeck);
        HEAP_OOM(echeck, err, early_free, early_cnt,);
    }

    //* --------------- 3. MAIN LOOP --------------- *//
    const bigInt *const r2 = modexp_contx.r2; // r_mod_n we will be used inplace for tmp_res
    memset(&r_mod_n.limbs[1], 0, r_mod_n.n * U64_BYTES); // in which both bigInts have the same cap as mod->n
    r_mod_n.limbs[0] = 1; r_mod_n.n = 1; r_mod_n.sign = 1;
    __BIHEAP_MONTMUL__(&r_mod_n, r2, modexp_contx, &r_mod_n, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    size_t total_bits = (exp->n << 6) - __CLZ_UI64__(exp->limbs[exp->n - 1]); /**/ size_t bit_idx = total_bits - 1;
    while (bit_idx) {
        size_t limb_idx = (size_t)(bit_idx - 1) >> 6;
        uint8_t bit_offset = (uint8_t)(bit_idx - 1 & 63);
        uint8_t curr_bit = exp->limbs[limb_idx] & (UINT64_C(1) << (bit_offset - 1));
        if (!curr_bit) { // Bit is 0 ---> Square (because 0-bits indicates powers of 2)
            __BIHEAP_MONTMUL__(&r_mod_n, &r_mod_n, modexp_contx, &r_mod_n, &echeck);
            HEAP_OOM(echeck, err, early_free, early_cnt,); /**/ --bit_idx;
        } else { // Bit is 1 --> Odd power
            // Extracting the current window
            size_t s = max(((int8_t)bit_offset - (int8_t)k + 1), 0); // Max window size
            // Scan ahead to locate the lowest 1-bit within range [bit_idx : bit_idx-s]
            size_t ri = bit_idx - 2, l = bit_idx - 1;
            while (ri >= s) {
                size_t l_limb = (size_t)ri >> 6;
                uint8_t l_boff = (uint8_t)(ri & 63);
                if ((exp->limbs[l_limb] >> l_boff) & 1) l = ri;
                --ri;
            }
            
            // Squaring (this is actually to reserve space)
            size_t winlen = bit_idx - l;
            for (size_t s = 0; s < winlen; ++s) {
                __BIHEAP_MONTMUL__(&r_mod_n, &r_mod_n, modexp_contx, &r_mod_n, &echeck);
                HEAP_OOM(echeck, err, early_free, early_cnt,);
            }

            // Multiplying the odd power by the lookup table entry
            size_t limb_idx = (size_t)(l) >> 6;
            size_t bit_offset = (uint8_t)((l) & 63);
            uint64_t winval = exp->limbs[limb_idx] >> bit_offset;
            if (bit_offset + winlen > U64_BITS && limb_idx + 1 < exp->n) {
                winval |= exp->limbs[limb_idx + 1] << (U64_BITS - bit_offset);
            } winval &= ((UINT64_C(1) << winlen) - 1);
            __BIHEAP_MONTMUL__(&r_mod_n, &table[winval >> 1], modexp_contx, &r_mod_n, &echeck);
            HEAP_OOM(echeck, err, early_free, early_cnt,); /**/ if (!l) break; /**/ bit_idx = l - 1;
        }
    } uint64_t a[1] = {1};
    __BIHEAP_MONTMUL__(&r_mod_n, &(bigInt){.limbs = a, .n = 1, .cap = 1, .sign = 1}, modexp_contx, res, &echeck);
    HEAP_OOM(echeck, err, early_free, early_cnt,); __BIGINT_INTERNAL_MOVE__(res, &r_mod_n);
    _free_alloc_list(alloc_list, alloc_cnt); *err = BIGINT_SUCCESS;
}




/* BigInt Algorithm Dispatchers */
void __BIHEAP_MODMUL_DISP__(PCONST_BIGINT a, PCONST_BIGINT b, PCONST_BIGINT mod, P_BIGINT res, dnml_status *err) {
    if (mod->n <= BIGINT_CLASSICAL) __BIHEAP_CMODMUL__(a, b, mod, res, err);
    else { dnml_status echeck; bigInt *alloc_list[3], *early_free[3]; uint8_t alloc_cnt = 0, early_cnt = 0;
        mont_ctx modmul_disp_ctx = { .n = mod, .nprime = __MODINV_UI64__(mod->limbs[0]), .k = mod->n };
        BIHEAP_TEMP(r, mod->n + 1, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
        BIHEAP_TEMP(r_mod_n, mod->n, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
        BIHEAP_TEMP(tmp, mod->n << 1, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
        __BIHEAP_MOD_DISP__(&r, mod, &r_mod_n, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
        __BIHEAP_MUL_DISP__(&r_mod_n, &r_mod_n, &tmp, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
        __BIHEAP_MOD_DISP__(&tmp, mod, &tmp, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
        modmul_disp_ctx.r2 = &tmp; __BIHEAP_MONTMUL__(a, b, modmul_disp_ctx, res, &echeck);
        _free_alloc_list(alloc_list, alloc_cnt); *err = (echeck == DNML_ALLOC_OOM) ? DNML_ALLOC_OOM : BIGINT_SUCCESS;
    }
}
void __BIHEAP_MODEXP_DISP__(PCONST_BIGINT base, PCONST_BIGINT exp, PCONST_BIGINT mod, P_BIGINT res, dnml_status *err) {           
    if (mod->n < BIGINT_MOD_BINARY) __BIHEAP_BIN_MODEXP__(base, exp, mod, res, err);
    else if (mod->n < BIGINT_MONT_BINARY) __BIHEAP_MBIN_MODEXP__(base, exp, mod, res, err);
    else if (mod->n < BIGINT_MOD_FIXED) __BIHEAP_FIX_MODEXP__(base, exp, mod, 6, res, err);
    else __BIHEAP_SLIDE_MODEXP__(base, exp, mod, 7, res, err);
}
