#include "heap_mod.h"

/* BigInt Barett Modular Reduction */
void __BIHEAP_BARETT__(const bigInt *a, const bigInt *n, bigInt *rem, dnml_status *err) {
    // ONE NOTE: <<< is denoted as limb shift in base 2^64
    /* ---- 1. PRECOMPUTATION - μ ---- */ dnml_status echeck = BIGINT_SUCCESS;
    size_t precomp_size = (n->n << 1) + 1, remlimbs = a->n - (n->n - 1);
    bigInt *alloc_list[3], *early_free[4]; uint8_t alloc_cnt = 0, early_cnt = 0;
    BIHEAP_TEMP(numer, precomp_size + remlimbs, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,); numer.n = precomp_size;
    BIHEAP_TEMP(precomp, precomp_size, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    BIHEAP_TEMP(tmp, max(n->n, precomp_size + remlimbs + 1), echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    numer.limbs[(n->n << 1)] = 1; __BIHEAP_DIV_DISP__(&numer, n, &precomp, &tmp, &echeck);


    //* ---- 2. NUMERATOR CALCULATION ---- *// 
    memset(numer.limbs, 0, precomp_size * U64_BYTES); /**/ tmp.limbs[(n->n << 1)] = 0; tmp.n = remlimbs;
    memcpy(tmp.limbs, &a->limbs[n->n - 1], remlimbs * U64_BYTES); // Now, tmp = a_after_shift (a >>> n->n - 1)
    // We copy starting from the n limbs, because:
    // - For instance: we want to limb right shift by 3 limbs:
    //  -----> Limbs [0] [1] [2] is lost
    //  -----> The remaining limbs start from 3;
    __BIHEAP_MUL_DISP__(&tmp, &precomp, &numer, &echeck);
    HEAP_OOM(echeck, err, early_free, early_cnt,);


    //* ---- 3. FINAL CALCULATION ---- *//
    remlimbs = numer.n - (n->n + 1); // Represents the number of blocks of shifts
    memcpy(numer.limbs, &numer.limbs[n->n - 1], remlimbs * U64_BYTES); __BIGINT_INTERNAL_TRIM_LZ__(&numer);
    // Now the predicted size of numer is:
    //
    //      ((precomp_size + remlimbs) [from a_after_shift * precomp]) - ((n->n - 1) [subtracted from the right limb shift])
    //
    // Therefre, the size requirements of __BIGINT_MUL_DISPATCH__(numer, n) OR (numer * n) should be:
    //
    //      (precomp_size + remlimbs - n->n + 1) + n->n -------> (precomp_size + remlimbs + 1) ---> REUSE tmp
    //
    // DO NOTE FOR ANY DEVELOEPRS AND MAINTAINERS, THIS SIZE PREDICTION IS PURELY A MATHEMATICAL
    // UPPERBOUND FROM SIZE-CALCULATION PRINCIPLES, AND MIGHT BE FRAGILE AND INCORRECT
    

    BIHEAP_RET(a_copy, a->n, echeck, err, early_free, early_cnt,); 
    a_copy.n = a->n; /**/ memcpy(a_copy.limbs, a->limbs, a->n * U64_BYTES);
    __BIHEAP_MUL_DISP__(&numer, n, &tmp, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    __BIGINT_SUB_WB__(&a_copy, &a_copy, &tmp);
    while (__BIGINT_INTERNAL_COMP__(&a_copy, n) >= 0) __BIGINT_SUB_WB__(&a_copy, &a_copy, n);
    __BIGINT_INTERNAL_SWAP__(rem, &a_copy); _free_alloc_list(alloc_list, alloc_cnt); *err = BIGINT_SUCCESS;
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