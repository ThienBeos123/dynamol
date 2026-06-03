#include "mod.h"

/* ----------------- WORKSPACE FUNCTIONS ----------------- */
size_t __BIGINT_BARETT_WS__(size_t a_size, size_t n_size) {
    // Precomputation Temporaries
    size_t numlimbs_size = (2 * n_size + 1);
    size_t prelimbs_size = (2 * n_size + 1);
    size_t tmp_size = n_size;
    // Numerator Calculation Temporaries
    size_t aaslimbs_size = (a_size - n_size + 1);
    size_t anumerator_size = (a_size + n_size + 2);
    // Final Calculation Temporaries
    size_t acopy_size = a_size;
    size_t additional_size = 0;
    if (likely((2 * n_size + 1) < (a_size + 1))) additional_size = (a_size + 1);
    // Paddings & Low-level Arenas
    size_t mul_divmod_size = max(
        __BIGINT_MUL_WS__(anumerator_size, n_size), max(
            __BIGINT_MUL_WS__(aaslimbs_size, prelimbs_size), 
            __BIGINT_DIV_WS__(numlimbs_size, n_size)
        )
    ); return numlimbs_size + prelimbs_size + tmp_size 
              + aaslimbs_size + anumerator_size 
              + acopy_size + additional_size + mul_divmod_size;
}
size_t __BIGINT_MOD_WS__(size_t a_size, size_t n_size) {
    if (n_size < BIGINT_SHORT) return __BIGINT_SHORTDIV_WS__(a_size, n_size);
    else if (n_size < BIGINT_KNUTH) return __BIGINT_KNUTH_WS__(a_size, n_size);
    else if (n_size < BIGINT_BARETT) return __BIGINT_BARETT_WS__(a_size, n_size);
    else return __BIGINT_NEWTON_WS__(a_size, n_size);
}




/* ----------------- ALGORITHMS FUNCTIONS ----------------- */
void __BIGINT_BARETT__(const bigInt *a, const bigInt *n, bigInt *rem, calc_ctx barett_ctx) {
    //* ---- 1. PRECOMPUTATION - μ ---- *//
    dnml_status err_check, end_stat = 0;
    size_t barett_mark = scratch_mark(&barett_ctx), precomp_size = (n->n << 1) + 1;
    BIGINT_TEMP(numer, precomp_size, barett_ctx, err_check, end_stat); numer.n = precomp_size;
    BIGINT_TEMP(precomp, precomp_size, barett_ctx, err_check, end_stat);
    BIGINT_TEMP(tmp, n->n, barett_ctx, err_check, end_stat);
    numer.limbs[(n->n << 1)] = 1;
    __BIGINT_DIV_DISPATCH__(&numer, n, &precomp, &tmp, barett_ctx);


    //* ---- 2. NUMERATOR CALCULATION ---- *//
    size_t remlimbs = a->n - (n->n - 1);;
    BIGINT_TEMP(a_after_shift, remlimbs, barett_ctx, err_check, end_stat); a_after_shift.n = remlimbs;
    BIGINT_TEMP(anumer, (remlimbs + precomp.n), barett_ctx, err_check, end_stat);
    memcpy(a_after_shift.limbs, &a->limbs[n->n - 1], remlimbs * U64_BYTES);
    // We copy starting from the n limbs, because:
    // - For instance: we want to limb right shift by 3 limbs:
    //  -----> Limbs [0] [1] [2] is lost
    //  -----> The remaining limbs start from 3;
    __BIGINT_MUL_DISPATCH__(&a_after_shift, &precomp, &anumer, barett_ctx);

    //* ---- 3. FINAL CALCULATION ---- *//
    remlimbs = anumer.n - (n->n + 1); // This value can be shortened to a->n + 1
    memcpy(anumer.limbs, &anumer.limbs[n->n - 1], remlimbs * U64_BYTES);
    limb_t *acopy_limbs = scratch_alloc(&barett_ctx, a->n, &end_stat); mod_endstat(end_stat, err_check);
    DNML_TEST_ASSERT(
        !(end_stat == DARENA_OVERFLOW), 
        "Insufficient Scratch Allocation Capaicty (-Earena_cap_overflow)",
        { scratch_clear(&barett_ctx); scratch_destruct(&barett_ctx); }
    );
    bigInt a_copy = {.limbs = acopy_limbs, .n = a->n, .cap = a->n, .sign = a->sign};
    memcpy(a_copy.limbs, a->limbs, a->n * U64_BYTES);
    if (unlikely(precomp.cap >= remlimbs)) {
        precomp.n = 0; precomp.sign = 1;
        __BIGINT_MUL_DISPATCH__(&anumer, n, &precomp, barett_ctx);
        __BIGINT_SUB_WB__(&a_copy, &a_copy, &precomp);
    } else {
        limb_t *final_limb = scratch_alloc(&barett_ctx, remlimbs, &err_check); mod_endstat(end_stat, err_check);
        DNML_TEST_ASSERT(
            !(end_stat == DARENA_OVERFLOW), 
            "Insufficient Scratch Allocation Capaicty (-Earena_cap_overflow)",
            { scratch_clear(&barett_ctx); scratch_destruct(&barett_ctx); }
        );
        bigInt final_res = {.limbs = final_limb, .n = 0, .cap = remlimbs, .sign = 1};
        __BIGINT_MUL_DISPATCH__(&anumer, n, &final_res, barett_ctx);
        __BIGINT_SUB_WB__(&a_copy, &a_copy, &final_res); 
    } 
    while (__BIGINT_INTERNAL_COMP__(&a_copy, n) >= 0) __BIGINT_SUB_WB__(&a_copy, &a_copy, n);
    __BIGINT_INTERNAL_COPY__(rem, &a_copy); scratch_reset(&barett_ctx, barett_mark);
}
void __BIGINT_MONT_REDC__(bigInt *t, mont_ctx mredc_ctx, bigInt *rem) {
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
void __BIGINT_MOD_DISPATCH__(const bigInt *a, const bigInt *n, bigInt *rem, bigInt *tmp_quot, calc_ctx mod_ctx) {
    if (n->n < BIGINT_SHORT) __BIGINT_SHORT_DIVISION__(a, n->limbs[0], tmp_quot, rem);
    else if (n->n < BIGINT_KNUTH) __BIGINT_KNUTH_D__(a, n, tmp_quot, rem, mod_ctx);
    else if (n->n < BIGINT_BARETT) __BIGINT_BARETT__(a, n, rem, mod_ctx);
    else __BIGINT_NEWTON__(a, n, tmp_quot, rem, mod_ctx);
}