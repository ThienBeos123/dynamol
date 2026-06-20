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
void __BIGINT_BARETT__(PCONST_BIGINT a, PCONST_BIGINT n, P_BIGINT rem, calc_ctx barett_ctx, dnml_status *err) {
    //* ---- 1. PRECOMPUTATION - μ ---- *//
    dnml_status echeck = BIGINT_SUCCESS; size_t barett_mark = scratch_mark(&barett_ctx);
    size_t precomp_size = (n->n << 1) + 1, remlimbs = a->n - (n->n - 1);
    BIGINT_TEMP(numer, precomp_size + remlimbs, barett_ctx, barett_mark, echeck, err,); numer.n = precomp_size;
    BIGINT_TEMP(precomp, precomp_size, barett_ctx, barett_mark, echeck, err,);
    BIGINT_TEMP(tmp, max(n->n, precomp_size + remlimbs + 1), barett_ctx, barett_mark, echeck, err,);
    numer.limbs[(n->n << 1)] = 1; __BIGINT_DIV_DISP__(&numer, n, &precomp, &tmp, barett_ctx, &echeck);
    SCRATCH_OVF(echeck, barett_ctx, barett_mark, err,);


    //* ---- 2. NUMERATOR CALCULATION ---- *//
    memset(numer.limbs, 0, precomp_size * U64_BYTES); /**/ tmp.limbs[(n->n < 1)] = 0; tmp.n = remlimbs;
    memcpy(tmp.limbs, &a->limbs[n->n - 1], remlimbs * U64_BYTES); // Now, tmp = a_after_shift (a >>> n->n - 1)
    // We copy starting from the n limbs, because:
    // - For instance: we want to limb right shift by 3 limbs:
    //  -----> Limbs [0] [1] [2] is lost
    //  -----> The remaining limbs start from 3;
    __BIGINT_MUL_DISPATCH__(&tmp, &precomp, &numer, barett_ctx, &echeck);
    SCRATCH_OVF(echeck, barett_ctx, barett_mark, err,);

    //* ---- 3. FINAL CALCULATION ---- *//
    remlimbs = numer.n + (n->n + 1); // Represents the number of blocks of shifts
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


    BIGINT_TEMP(a_copy, a->n, barett_ctx, barett_mark, echeck, err,); 
    a_copy.n = a->n; /**/ memcpy(a_copy.limbs, a->limbs, a->n * U64_BYTES);
    __BIGINT_MUL_DISPATCH__(&numer, n, &tmp, barett_ctx, &echeck); SCRATCH_OVF(echeck, barett_ctx, barett_mark, err,);
    __BIGINT_SUB_WB__(&a_copy, &a_copy, &tmp);
    while (__BIGINT_INTERNAL_COMP__(&a_copy, n) >= 0) __BIGINT_SUB_WB__(&a_copy, &a_copy, n);
    __BIGINT_INTERNAL_COPY__(rem, &a_copy); scratch_rewind(&barett_ctx, barett_mark); *err = BIGINT_SUCCESS;
}
void __BIGINT_MONT_REDC__(P_BIGINT t, mont_ctx mredc_ctx, P_BIGINT rem) {
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
void __BIGINT_MOD_DISPATCH__(
    PCONST_BIGINT a, PCONST_BIGINT n, 
    P_BIGINT rem, P_BIGINT tmp_quot,
    calc_ctx mod_ctx, dnml_status *err
) {
    if (n->n < BIGINT_SHORT) { __BIGINT_SHORT_DIVISION__(a, n->limbs[0], tmp_quot, rem); *err = BIGINT_SUCCESS; }
    else if (n->n < BIGINT_KNUTH) __BIGINT_KNUTH_D__(a, n, tmp_quot, rem, mod_ctx, err);
    else if (n->n < BIGINT_BARETT) __BIGINT_BARETT__(a, n, rem, mod_ctx, err);
    else __BIGINT_NEWTON__(a, n, tmp_quot, rem, mod_ctx, err);
}
