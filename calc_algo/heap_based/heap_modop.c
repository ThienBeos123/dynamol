#include "heap_modop.h"
limb_t a[1] = {1}; // NO FUNCTION SHOULD MODIFY THIS ARRAY, EVER

/* BigInt Modular Multiplication AND Exponentiation Algorithms */
void __BIHEAP_CMODMUL__(const bigInt *a, const bigInt *b, const bigInt *mod, bigInt *res, dnml_status *err) {
    dnml_status echeck; bigInt *alloc_list[4], *early_free[4]; uint8_t alloc_cnt = 0, early_cnt = 0;
    BIHEAP_TEMP(prod, (a->n + b->n), echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    BIHEAP_TEMP(tmp, (a->n + b->n), echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    const bigInt *chosen_a = a, *chosen_b = b; bigInt a_mod_n = {0}, b_mod_n = {0};
    if (a->n > mod->n << 1) {
        echeck = __BIGINT_INTERNAL_LINIT__(&a_mod_n, mod->n);
        if (echeck == DNML_ALLOC_OOM) { _free_alloc_list(early_free, early_cnt); *err = DNML_ALLOC_OOM; return; }
        early_free[early_cnt++] = &a_mod_n; alloc_list[alloc_cnt++] = &a_mod_n;
        __BIHEAP_MOD_DISP__(a, mod, &a_mod_n, &tmp, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
        chosen_a = &a_mod_n;
    }
    if (b->n > mod->n << 1) {
        echeck = __BIGINT_INTERNAL_LINIT__(&b_mod_n, mod->n);
        if (echeck == DNML_ALLOC_OOM) { _free_alloc_list(early_free, early_cnt); *err = DNML_ALLOC_OOM; return; }
        early_free[early_cnt++] = &b_mod_n; alloc_list[alloc_cnt++] = &b_mod_n;
        __BIHEAP_MOD_DISP__(b, mod, &b_mod_n, &tmp, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
        chosen_a = &b_mod_n;
    }
    __BIHEAP_MUL_DISP__(chosen_a, chosen_b, &prod, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    __BIHEAP_MOD_DISP__(&prod, mod, res, &tmp, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    _free_alloc_list(alloc_list, alloc_cnt); *err = BIGINT_SUCCESS;
}
void __BIHEAP_MONTMUL__(const bigInt *a, const bigInt *b, mont_ctx ctx, bigInt *res, dnml_status *err) {
    dnml_status echeck; bigInt *early_free[1]; uint8_t early_cnt = 0;
    BIHEAP_RET(t, (2*ctx.k + 1), echeck, err, early_free, early_cnt,);
    __BIHEAP_MUL_DISP__(a, b, &t, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    __BIHEAP_MONT_REDC__(&t, ctx, res); *err = BIGINT_SUCCESS;
}
void __BIHEAP_BIN_MODEXP__(const bigInt *base, const bigInt *exp, const bigInt *mod, bigInt *res, dnml_status *err) {
    int8_t cmp_res = __BIGINT_INTERNAL_COMP__(base, mod);
    dnml_status echeck; bigInt *alloc_list[3], *early_free[4]; uint8_t alloc_cnt = 0, early_cnt = 0;
    BIHEAP_TEMP(tmp, base->n, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    BIHEAP_RET(tmp_res, mod->n, echeck, err, early_free, early_cnt,);
    BIHEAP_TEMP(tmp_exp, exp->n, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    BIHEAP_TEMP(tmp_base, mod->n, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    tmp_res.limbs[0] = 1; memcpy(tmp_exp.limbs, exp->limbs, exp->n * U64_BYTES);
    if (cmp_res < 0) memcpy(tmp_base.limbs, base->limbs, base->n * U64_BYTES);
    else if (cmp_res > 0) { 
        __BIHEAP_MOD_DISP__(base, mod, &tmp_base, &tmp, &echeck); 
        HEAP_OOM(echeck, err, early_free, early_cnt,);
    }
    while (tmp_exp.n > 0) {
        if (tmp_exp.limbs[0] & 1) {
            __BIHEAP_CMODMUL__(&tmp_res, &tmp_base, mod, &tmp_res, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
        } __BIHEAP_CMODMUL__(&tmp_base, &tmp_base, mod, &tmp_base, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
        __BIGINT_INTERNAL_RSHIFT__(&tmp_exp, 1);
    } __BIGINT_INTERNAL_SWAP__(res, &tmp_res); _free_alloc_list(alloc_list, alloc_cnt); *err = BIGINT_SUCCESS;
}
void __BIHEAP_MBIN_MODEXP__(const bigInt *base, const bigInt *exp, const bigInt *mod, bigInt *res, dnml_status *err) {
    //* ----------- 1. SETUP ----------- *//
    dnml_status echeck; bigInt *alloc_list[6], *early_free[6]; uint8_t alloc_cnt = 0, early_cnt = 0;
    mont_ctx modexp_contx = { .n = mod, .nprime = __MODINV_UI64__(mod->limbs[0]), .k = mod->n }; 
    size_t max_tsize = max((mod->n << 1), max(base->n, exp->n));
    BIHEAP_TEMP(r, max_tsize, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,); r.n = mod->n + 1;
    BIHEAP_TEMP(r_mod_n, mod->n, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    BIHEAP_TEMP(tmp, max_tsize, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,); r.limbs[mod->n] = 1; 
    __BIHEAP_MOD_DISP__(&r, mod, &r_mod_n, &tmp, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    __BIHEAP_MUL_DISP__(&r_mod_n, &r_mod_n, &tmp, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    __BIHEAP_MOD_DISP__(&tmp, mod, &tmp, &r, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    modexp_contx.r2 = &tmp;

    //* ----- 2. MAIN LOOP ----- *//
    int8_t cmp_res = __BIGINT_INTERNAL_COMP__(base, mod);
    const bigInt *const r2 = modexp_contx.r2; // We will now re-use r_mod_n in place for tmp_res, in which
    memset(&r_mod_n.limbs[1], 0, r_mod_n.n * U64_BYTES); // both bigints have the same capacity requirement of mod->n
    BIHEAP_TEMP(tmp_exp, exp->n, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    BIHEAP_TEMP(tmp_base, mod->n, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    r_mod_n.limbs[0] = 1; memcpy(tmp_exp.limbs, exp->limbs, exp->n * U64_BYTES);
    if (cmp_res < 0) memcpy(tmp_base.limbs, base->limbs, base->n * U64_BYTES);
    else if (cmp_res > 0) {__BIHEAP_MOD_DISP__(base, mod, &tmp_base, &r, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,); }
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
void __BIHEAP_FIX_MODEXP__(const bigInt *base, const bigInt *exp, const bigInt *mod, bigInt *res, dnml_status *err) {}
void __BIHEAP_SLIDE_MODEXP__(const bigInt *base, const bigInt *exp, const bigInt *mod, bigInt *res, dnml_status *err) {}

/* BigInt Algorithm Dispatchers */
void __BIHEAP_MODMUL_DISP__(const bigInt *a, const bigInt *b, const bigInt *mod, bigInt *res, dnml_status *err) {
    if (mod->n <= BIGINT_CLASSICAL) __BIHEAP_CMODMUL__(a, b, mod, res, err);
    else { dnml_status echeck; bigInt *alloc_list[3], *early_free[3]; uint8_t alloc_cnt = 0, early_cnt = 0;
        mont_ctx modmul_disp_ctx = { .n = mod, .nprime = __MODINV_UI64__(mod->limbs[0]), .k = mod->n };
        BIHEAP_TEMP(r, mod->n + 1, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
        BIHEAP_TEMP(r_mod_n, mod->n, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
        BIHEAP_TEMP(tmp, mod->n << 1, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
        __BIHEAP_MOD_DISP__(&r, mod, &r_mod_n, &tmp, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
        __BIHEAP_MUL_DISP__(&r_mod_n, &r_mod_n, &tmp, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
        __BIHEAP_MOD_DISP__(&tmp, mod, &tmp, &r, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
        modmul_disp_ctx.r2 = &tmp; __BIHEAP_MONTMUL__(a, b, modmul_disp_ctx, res, &echeck);
        _free_alloc_list(alloc_list, alloc_cnt); *err = (echeck == DNML_ALLOC_OOM) ? DNML_ALLOC_OOM : BIGINT_SUCCESS;
    }
}
void __BIHEAP_MODEXP_DISP__(const bigInt *base, const bigInt *exp, const bigInt *mod, bigInt *res, dnml_status *err) {           
    if (mod->n < BIGINT_MOD_BINARY) __BIHEAP_BIN_MODEXP__(base, exp, mod, res, err);
    else if (mod->n < BIGINT_MONT_BINARY) __BIHEAP_MBIN_MODEXP__(base, exp, mod, res, err);
    else if (mod->n < BIGINT_MOD_FIXED) __BIHEAP_FIX_MODEXP__(base, exp, mod, res, err);
    else __BIHEAP_SLIDE_MODEXP__(base, exp, mod, res, err);
}