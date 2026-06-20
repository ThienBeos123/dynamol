#include "heap_mod.h"

/* BigInt Barett Modular Reduction */
void __BIHEAP_BARETT__(const bigInt *a, const bigInt *n, bigInt *rem, dnml_status *err) {
    //* ---- 1. PRECOMPUTATION - μ ---- *//
    dnml_status echeck; size_t precomp_size = (n->n << 1) + 1;
    bigInt *alloc_list[7]; uint8_t alloc_cnt = 0;
    BIHEAP_TEMP(numer, precomp_size, echeck, err, alloc_list, alloc_cnt,); numer.n = precomp_size;
    BIHEAP_TEMP(precomp, precomp_size, echeck, err, alloc_list, alloc_cnt,);
    BIHEAP_TEMP(tmp, n->n, echeck, err, alloc_list, alloc_cnt,);
    numer.limbs[(n->n << 1)] = 1; __BIHEAP_DIV_DISP__(&numer, n, &precomp, &tmp, &echeck);


    //* ---- 2. NUMERATOR CALCULATION ---- *//
    size_t remlimbs = a->n - (n->n - 1);;
    BIHEAP_TEMP(a_after_shift, remlimbs, echeck, err, alloc_list, alloc_cnt,); a_after_shift.n = remlimbs;
    BIHEAP_TEMP(anumer, (remlimbs + precomp.n), echeck, err, alloc_list, alloc_cnt,);
    memcpy(a_after_shift.limbs, &a->limbs[n->n - 1], remlimbs * U64_BYTES);
    // We copy starting from the n limbs, because:
    // - For instance: we want to limb right shift by 3 limbs:
    //  -----> Limbs [0] [1] [2] is lost
    //  -----> The remaining limbs start from 3;
    __BIHEAP_MUL_DISP__(&a_after_shift, &precomp, &anumer, &echeck);
    HEAP_OOM(echeck, err, alloc_list, alloc_cnt,);


    //* ---- 3. FINAL CALCULATION ---- *//
    remlimbs = anumer.n - (n->n + 1); // This value can be shortened to a->n + 1
    memcpy(anumer.limbs, &anumer.limbs[n->n - 1], remlimbs * U64_BYTES);
    BIHEAP_TEMP(a_copy, a->n, echeck, err, alloc_list, alloc_cnt,);
    memcpy(a_copy.limbs, a->limbs, a->n * U64_BYTES);
    if (unlikely(precomp.cap >= remlimbs)) {
        precomp.n = 0; precomp.sign = 1;
        __BIHEAP_MUL_DISP__(&anumer, n, &precomp, &echeck); HEAP_OOM(echeck, err, alloc_list, alloc_cnt,);
        __BIGINT_SUB_WB__(&a_copy, &a_copy, &precomp);
    } else {
        BIHEAP_TEMP(final_res, remlimbs, echeck, err, alloc_list, alloc_cnt,);
        __BIHEAP_MUL_DISP__(&anumer, n, &final_res, &echeck); HEAP_OOM(echeck, err, alloc_list, alloc_cnt,);
        __BIGINT_SUB_WB__(&a_copy, &a_copy, &final_res);
    }
    while (__BIGINT_INTERNAL_COMP__(&a_copy, n) >= 0) __BIGINT_SUB_WB__(&a_copy, &a_copy, n);
    __BIGINT_INTERNAL_COPY__(rem, &a_copy); _free_alloc_list(alloc_list, alloc_cnt); *err = BIGINT_SUCCESS;
}


/* BigInt "Helper" Montgomery REDC */
void __BIHEAP_MONT_REDC__(bigInt *t, mont_ctx mredc_ctx, bigInt *rem) {
    uint64_t m, carry = 0;
    // Loop basically cancels k lowest limbs
    for (size_t i = 0; i < mredc_ctx.k; ++i) {
        uint64_t lo, hi; m = t->limbs[i] * mredc_ctx.nprime;
        lo = __MUL_UI64__(mredc_ctx.n->limbs[i], m, &hi);
        uint8_t u8_carry = (uint8_t)carry;
        t->limbs[i] = __ADD_UI64__(t->limbs[i], lo + carry, &u8_carry);
        carry += hi;
    } t->limbs[mredc_ctx.k] += carry;
    // Right Limb Shift by k  --  t +>> k (+>> or +<< means LIMB SHIFT)
    memcpy(&t->limbs[0], &t->limbs[mredc_ctx.k], (mredc_ctx.k) * U64_BYTES);
    t->n = mredc_ctx.k + 1; // From 2k + 1 (upperbound) ---> k + 1 (from the limb shift)
    if (__BIGINT_INTERNAL_COMP__(t, mredc_ctx.n) > 0) __BIGINT_SUB_WB__(t, t, mredc_ctx.n);
    __BIGINT_INTERNAL_COPY__(rem, t);
}


/* Modular Reduction Algorithm Dispatcher */
void __BIHEAP_MOD_DISP__(const bigInt *a, const bigInt *n, bigInt *rem, bigInt *tmp_quot, dnml_status *err) {
    if (n->n < BIGINT_SHORT) { __BIHEAP_SHORT_DIVISION__(a, n->limbs[0], tmp_quot, rem); *err = BIGINT_SUCCESS; }
    else if (n->n < BIGINT_KNUTH) __BIHEAP_KNUTH_D__(a, n, tmp_quot, rem, err);
    else if (n->n < BIGINT_BARETT) __BIHEAP_BARETT__(a, n, rem, err);
    else __BIHEAP_NEWTON__(a, n, tmp_quot, rem, err);
}