#include "heap_modop.h"

/* BigInt Modular Multiplication Algorithms */
void __BIHEAP_CMODMUL__(const bigInt *a, const bigInt *b, const bigInt *mod, bigInt *res, dnml_status *err) {
    dnml_status echeck; bigInt *alloc_list[2]; uint8_t alloc_cnt = 0;
    BIHEAP_TEMP(prod, (a->n + b->n), echeck, err, alloc_list, alloc_cnt,);
    BIHEAP_TEMP(tmp, (a->n + b->n), echeck, err, alloc_list, alloc_cnt,);
    __BIHEAP_MUL_DISP__(a, b, &prod, &echeck); HEAP_OOM(echeck, err, alloc_list, alloc_cnt,)
    __BIHEAP_MOD_DISP__(&prod, mod, res, &tmp, &echeck); HEAP_OOM(echeck, err, alloc_list, alloc_cnt,)
    *err = BIGINT_SUCCESS;
}
void __BIHEAP_MONTMUL__(const bigInt *a, const bigInt *b, mont_ctx ctx, bigInt *res, dnml_status *err) {
    dnml_status echeck; bigInt *alloc_list[1]; uint8_t alloc_cnt = 0;
    BIHEAP_TEMP(t, (2*ctx.k + 1), echeck, err, alloc_list, alloc_cnt,);
    __BIHEAP_MUL_DISP__(a, b, &t, &echeck); HEAP_OOM(echeck, err, alloc_list, alloc_cnt,)
    __BIHEAP_MONT_REDC__(&t, ctx, res); *err = BIGINT_SUCCESS;
}

/* BigInt Modular Exponentiation - Binary Exponentiation */
void __BIHEAP_BIN_MODEXP__(const bigInt *base, const bigInt *exp, const bigInt *mod, bigInt *res, dnml_status *err) {
    dnml_status echeck; bigInt *alloc_list[4]; uint8_t alloc_cnt = 0;
    BIHEAP_TEMP(buf, base->n, echeck, err, alloc_list, alloc_cnt,);
    BIHEAP_TEMP(tmp_res, mod->n, echeck, err, alloc_list, alloc_cnt,);
    BIHEAP_TEMP(tmp_exp, exp->n, echeck, err, alloc_list, alloc_cnt,);
    BIHEAP_TEMP(tmp_base, mod->n, echeck, err, alloc_list, alloc_cnt,);
    tmp_res.limbs[0] = 1; memcpy(tmp_exp.limbs, exp->limbs, exp->n * U64_BYTES);
    __BIHEAP_MOD_DISP__(base, mod, &tmp_base, &buf, &echeck); 
    while (tmp_exp.n > 0) {
        if (tmp_exp.limbs[0] & 1) {
            __BIHEAP_CMODMUL__(&tmp_res, &tmp_base, mod, &tmp_res, &echeck); HEAP_OOM(echeck, err, alloc_list, alloc_cnt,);
        } __BIHEAP_CMODMUL__(&tmp_base, &tmp_base, mod, &tmp_base, &echeck); HEAP_OOM(echeck, err, alloc_list, alloc_cnt,);
        __BIGINT_INTERNAL_RSHIFT__(&tmp_exp, 1);
    } __BIGINT_INTERNAL_COPY__(res, &tmp_res); _free_alloc_list(alloc_list, alloc_cnt); *err = BIGINT_SUCCESS;
}
void __BIHEAP_MBIN_MODEXP__(const bigInt *base, const bigInt *exp, const bigInt *mod, bigInt *res, dnml_status *err) {
    /* --- 1. SETUP ---- */ dnml_status echeck; bigInt *alloc_list[6]; uint8_t alloc_cnt = 0;
    mont_ctx modexp_contx = { .n = mod, .nprime = __MODINV_UI64__(mod->limbs[0]), .k = mod->n }; 
    size_t max_tsize = max((mod->n << 1), max(base->n, exp->n));
    BIHEAP_TEMP(r, max_tsize, echeck, err, alloc_list, alloc_cnt,); r.n = mod->n + 1;
    BIHEAP_TEMP(r_mod_n, mod->n, echeck, err, alloc_list, alloc_cnt,);
    BIHEAP_TEMP(tmp, max_tsize, echeck, err, alloc_list, alloc_cnt,); r.limbs[mod->n] = 1; 
    __BIHEAP_MOD_DISP__(&r, mod, &r_mod_n, &tmp, &echeck); HEAP_OOM(echeck, err, alloc_list, alloc_cnt,);
    __BIHEAP_MUL_DISP__(&r_mod_n, &r_mod_n, &tmp, &echeck); HEAP_OOM(echeck, err, alloc_list, alloc_cnt,);
    __BIHEAP_MOD_DISP__(&tmp, mod, &tmp, &r, &echeck); HEAP_OOM(echeck, err, alloc_list, alloc_cnt,);
    modexp_contx.r2 = &tmp;

    //* ----- 2. MAIN LOOP ----- *//
    PCONST_BIGINT r2 = modexp_contx.r2;
    BIHEAP_TEMP(tmp_res, mod->n, echeck, err, alloc_list, alloc_cnt,);
    BIHEAP_TEMP(tmp_exp, exp->n, echeck, err, alloc_list, alloc_cnt,);
    BIHEAP_TEMP(tmp_base, mod->n, echeck, err, alloc_list, alloc_cnt,);
    tmp_res.limbs[0] = 1; memcpy(tmp_exp.limbs, exp->limbs, exp->n * U64_BYTES);
    __BIHEAP_MOD_DISP__(base, mod, &tmp_base, &r, &echeck); HEAP_OOM(echeck, err, alloc_list, alloc_cnt,);
    __BIHEAP_MONTMUL__(&tmp_res, r2, modexp_contx, &tmp_res, &echeck); HEAP_OOM(echeck, err, alloc_list, alloc_cnt,);
    __BIHEAP_MONTMUL__(&tmp_base, r2, modexp_contx, &tmp_base, &echeck); HEAP_OOM(echeck, err, alloc_list, alloc_cnt,);
    while (tmp_exp.n > 0) {
        if (tmp_exp.limbs[0] & 1) {
            __BIHEAP_MONTMUL__(&tmp_res, &tmp_base, modexp_contx, &tmp_res, &echeck); HEAP_OOM(echeck, err, alloc_list, alloc_cnt,);
        } __BIHEAP_MONTMUL__(&tmp_base, &tmp_base, modexp_contx, &tmp_base, &echeck); HEAP_OOM(echeck, err, alloc_list, alloc_cnt,); 
        __BIGINT_INTERNAL_RSHIFT__(&tmp_exp, 1);
    } uint64_t a[1] = {1};
    __BIHEAP_MONTMUL__(&tmp_res, &(bigInt){.limbs = a, .n = 1, .cap = 1, .sign = 1}, modexp_contx, res, &echeck); 
    HEAP_OOM(echeck, err, alloc_list, alloc_cnt,); _free_alloc_list(alloc_list, alloc_cnt) ;*err = BIGINT_SUCCESS;
}
/* BigInt Modular Exponentiation - Windows Algorithms */
void __BIHEAP_FIX_MODEXP__(const bigInt *base, const bigInt *exp, const bigInt *mod, bigInt *res, dnml_status *err) {}
void __BIHEAP_SLIDE_MODEXP__(const bigInt *base, const bigInt *exp, const bigInt *mod, bigInt *res, dnml_status *err) {}

/* BigInt Algorithm Dispatchers */
void __BIHEAP_MODMUL_DISP__(const bigInt *a, const bigInt *b, const bigInt *mod, bigInt *res, dnml_status *err) {
    if (mod->n <= BIGINT_CLASSICAL) __BIHEAP_CMODMUL__(a, b, mod, res, err);
    else { dnml_status echeck; bigInt* alloc_list[3]; uint8_t alloc_cnt = 0;
        mont_ctx modmul_disp_ctx = { .n = mod, .nprime = __MODINV_UI64__(mod->limbs[0]), .k = mod->n };
        BIHEAP_TEMP(r, mod->n + 1, echeck, err, alloc_list, alloc_cnt,);
        BIHEAP_TEMP(r_mod_n, mod->n, echeck, err, alloc_list, alloc_cnt,);
        BIHEAP_TEMP(tmp, mod->n << 1, echeck, err, alloc_list, alloc_cnt,);
        __BIHEAP_MOD_DISP__(&r, mod, &r_mod_n, &tmp, &echeck); HEAP_OOM(echeck, err, alloc_list, alloc_cnt,);
        __BIHEAP_MUL_DISP__(&r_mod_n, &r_mod_n, &tmp, &echeck); HEAP_OOM(echeck, err, alloc_list, alloc_cnt,);
        __BIHEAP_MOD_DISP__(&tmp, mod, &tmp, &r, &echeck); HEAP_OOM(echeck, err, alloc_list, alloc_cnt,);
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