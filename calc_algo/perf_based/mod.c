#include "mod.h"

/* ----------------- WORKSPACE FUNCTIONS ----------------- */
static size_t __BIGINT_BARETT_WS__(size_t a_size, size_t n_size) {
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
            __BIGINT_DIVMOD_WS__(numlimbs_size, n_size)
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
static void __BIGINT_BARETT__(const bigInt *a, const bigInt *n, bigInt *rem, calc_ctx barett_ctx) {
    //* ---- 1. PRECOMPUTATION - μ ---- *//
    size_t barett_mark = scratch_mark(&barett_ctx), precomp_size = 2 * n->n + 1;
    limb_t *numerator_limbs = scratch_alloc(&barett_ctx, precomp_size * BYTES_IN_UINT64_T);
    limb_t *precomp_limbs = scratch_alloc(&barett_ctx, precomp_size * BYTES_IN_UINT64_T);
    limb_t *tmp_limbs = scratch_alloc(&barett_ctx, n->n * BYTES_IN_UINT64_T);
    bigInt numerator = {.limbs = numerator_limbs, .n = precomp_size, .cap = precomp_size, .sign = 1};
    bigInt precomputation = {.limbs = precomp_limbs, .n = 0, .cap = precomp_size, .sign = 1};
    bigInt tmp = {.limbs = tmp_limbs, .n = 0, .cap = n->n, .sign = 1};
    numerator.limbs[2 * n->n] = 1;
    __BIGINT_DIVMOD_DISPATCH__(&numerator, n, &precomputation, &tmp, barett_ctx);


    //* ---- 2. NUMERATOR CALCULATION ---- *//
    size_t remaining_limbs = a->n - (n->n - 1);
    limb_t *aas_limbs = scratch_alloc(&barett_ctx, remaining_limbs * BYTES_IN_UINT64_T);
    limb_t *anumerator_limbs = scratch_alloc(&barett_ctx, (remaining_limbs + precomputation.n) * BYTES_IN_UINT64_T);
    bigInt a_after_shift = {.limbs = aas_limbs, .n = remaining_limbs, .cap = remaining_limbs, .sign = 1};
    bigInt anumerator = {
        .limbs = anumerator_limbs, .sign = 1,
        .n = 0, .cap = (remaining_limbs + precomputation.n)
    }; memcpy(a_after_shift.limbs, &a->limbs[n->n - 1], remaining_limbs * BYTES_IN_UINT64_T);
    // We copy starting from the n limbs, because:
    // - For instance: we want to limb right shift by 3 limbs:
    //  -----> Limbs [0] [1] [2] is lost
    //  -----> The remaining limbs start from 3;
    __BIGINT_MUL_DISPATCH__(&a_after_shift, &precomputation, &anumerator, barett_ctx);

    //* ---- 3. FINAL CALCULATION ---- *//
    remaining_limbs = anumerator.n - (n->n + 1); // This value can be shortened to a->n + 1
    memcpy(anumerator.limbs, &anumerator.limbs[n->n - 1], remaining_limbs * BYTES_IN_UINT64_T);
    limb_t *acopy_limbs = scratch_alloc(&barett_ctx, a->n * BYTES_IN_UINT64_T);
    memcpy(acopy_limbs, a->limbs, a->n * BYTES_IN_UINT64_T);
    bigInt a_copy = {.limbs = acopy_limbs, .n = a->n, .cap = a->n, .sign = a->sign};
    if (unlikely(precomputation.cap >= remaining_limbs)) {
        precomputation.n = 0; precomputation.sign = 1;
        __BIGINT_MUL_DISPATCH__(&anumerator, n, &precomputation, barett_ctx);
        __BIGINT_SUB_WB__(&a_copy, &a_copy, &precomputation);
    } else {
        limb_t *final_limb = scratch_alloc(&barett_ctx, remaining_limbs);
        bigInt final_res = {.limbs = final_limb, .n = 0, .cap = remaining_limbs, .sign = 1};
        __BIGINT_MUL_DISPATCH__(&anumerator, n, &final_res, barett_ctx);
        __BIGINT_SUB_WB__(&a_copy, &a_copy, &final_res); 
    } 
    while (__BIGINT_INTERNAL_COMP__(&a_copy, n) >= 0) __BIGINT_SUB_WB__(&a_copy, &a_copy, n);
    __BIGINT_INTERNAL_COPY__(rem, &a_copy); scratch_reset(&barett_ctx, barett_mark);
}
void __BIGINT_MONT_REDC__(bigInt *t, mont_ctx mredc_ctx, bigInt *rem, calc_ctx redc_ctx) {
    uint64_t m, carry = 0;
    // Loop basically cancels k lowest limbs
    for (size_t i = 0; i < mredc_ctx.k; ++i) {
        uint64_t lo, hi; m = t->limbs[i] * mredc_ctx.nprime;
        lo = __MUL_UI64__(mredc_ctx.n->limbs[i], m, &hi);
        t->limbs[i] = __ADD_UI64__(t->limbs[i], lo + carry, &carry);
        carry += hi;
    } t->limbs[mredc_ctx.k] += carry;
    // Right Limb Shift by k  --  t +>> k (+>> or +<< means LIMB SHIFT)
    memcpy(&t->limbs[0], &t->limbs[mredc_ctx.k], (mredc_ctx.k) * BYTES_IN_UINT64_T);
    t->n = mredc_ctx.k + 1; // From 2k + 1 (upperbound) ---> k + 1 (from the limb shift)
    if (__BIGINT_INTERNAL_COMP__(t, mredc_ctx.n) > 0) __BIGINT_SUB_WB__(t, t, mredc_ctx.n);
    __BIGINT_INTERNAL_COPY__(rem, t);
}
void __BIGINT_MOD_DISPATCH__(const bigInt *a, const bigInt *n, bigInt *rem, bigInt *tmp_quot, calc_ctx mod_ctx) {
    if (n->n < BIGINT_SHORT) __BIGINT_SHORT_DIVISION__(a, n->limbs[0], &tmp_quot, rem);
    else if (n->n < BIGINT_KNUTH) __BIGINT_KNUTH_D__(a, n, &tmp_quot, rem, mod_ctx);
    else if (n->n < BIGINT_BARETT) __BIGINT_BARETT__(a, n, rem, mod_ctx);
    else __BIGINT_NEWTON__(a, n, &tmp_quot, rem, mod_ctx);
}