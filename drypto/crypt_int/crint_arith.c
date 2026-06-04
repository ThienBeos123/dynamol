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



#include "cryptInt_func.h"
#include "crint_algo_core.h"


//* ========================================= MAGNITUDE MATHEMATICA ========================================== *//
/*
* IMPORTANT NOTE FOR LIB-DNML DEVELOPERS - REPETITION OF API-DESING.md:
*   1. Any usage of MAGNITUDED MATHEMATICAL FUNCTIONS MUST ensure that the result 
*      destination CryptInt has satisafctory capacity, non-poisoned, and has a valid 
*      pointer address to allocated buffers to reduce error handling flow from upstream
*
*   2. Any usage of MAGNITUDED MATHEMATICAL FUNCTIONS MUST ensure that the input
*      CryptInt be filtered, handled, and saturated multiplexed to prevent any unwanted errors
*      regarding segmentation faults, mathematically impossible outputs, test-assert triggering,
*      or allocation failures. This is also mainly to reduce error handling work from upstream
*
*   3. You MUST use heap-allocation for the temporary destination of operation results
*      for easier memory management and opaque, up-front OOM handling. This is for 
*      the same reason as detailed in (1), in which the public-facing functions
*      is responsible for up-front error signals passing and falsehood buffers multiplexing, 
*      while the magnituded engine are only responsible for the brute, algorithmic work
*      and setup, with minimal DNML_TEST_ASSERT for testing. At most, there will be
*      error returns for function that utilizes arena allocation for its algorithmic core
*/
/* ------------------- MAGNITUDED ARITHMETIC ------------------- */
void __CRINT_MAGADD__(crint *res, crint *a, crint *b) {
    _pre_assert(res, { crint_free(res); crint_free(a); crint_free(b); });
    _pre_assert(a, { crint_free(res); crint_free(a); crint_free(b); });
    _pre_assert(b, { crint_free(res); crint_free(a); crint_free(b); });
    DNML_TEST_ASSERT((_lib_crt_geq(res->cap, crtmax(a->n, b->n) + 1)),
        "Insufficient Sum Buffer: Capacity Unsatisfactory for a + b (-Eadd_insufficient_cap)", 
        { crint_free(res); crint_free(a); crint_free(b); }
    ); __CRINT_ADD_WC__(res, a, b); // clang-format off
    res = 0; a = 0; b = 0; // clang-format on
}
void __CRINT_MAGSUB__(crint *res, crint *a, crint *b) {
    _pre_assert(res, { crint_free(res); crint_free(a); crint_free(b); });
    _pre_assert(a, { crint_free(res); crint_free(a); crint_free(b); });
    _pre_assert(b, { crint_free(res); crint_free(a); crint_free(b); });
    DNML_TEST_ASSERT((__CRINT_INTERNAL_CMP__(a, b) != -1),
        "Subtraction Underflow: Subtrahend's magnitude is too large for Minuend"
        " (-Esub_underflow)", { crint_free(res); crint_free(a); crint_free(b); }
    ); __CRINT_SUB_WC__(res, a, b); // clang-format off
    res = 0; a = 0; b = 0; // clang-format on
}
void __CRINT_MAGMUL___(crint *res, crint *a, crint *b) {
    _pre_assert(res, { crint_free(res); crint_free(a); crint_free(b); });
    _pre_assert(a, { crint_free(res); crint_free(a); crint_free(b); });
    _pre_assert(b, { crint_free(res); crint_free(a); crint_free(b); });
    DNML_TEST_ASSERT(
        (_lib_crt_geq(res->cap, a->n + b->n)), // Result Capacity >= Sum of multiplicand's length
        "Insufficient Product Capacity: Capacity insatisfactory for a * b (-Emul_insufficient_cap)",
        { crint_free(res); crint_free(a); crint_free(b); }
    ); __CRINT_MUL_DISP__(a, b, res); // clang-format off
    res = 0; a = 0; b = 0; // clang-format on
}
void __CRINT_MAGDIV__(crint *quot, crint *tmp_rem, crint *a, crint *b) {
    _pre_assert(quot, { crint_free(quot); crint_free(tmp_rem); crint_free(a); crint_free(b); })
    _pre_assert(tmp_rem, { crint_free(quot); crint_free(tmp_rem); crint_free(a); crint_free(b); })
    _pre_assert(a, { crint_free(quot); crint_free(tmp_rem); crint_free(a); crint_free(b); })
    _pre_assert(b, { crint_free(quot); crint_free(tmp_rem); crint_free(a); crint_free(b); })
    DNML_TEST_ASSERT((b->n),
        "Mathematical Undefinindness: Division by 0 (-Ediv_by_zero)", 
        { crint_free(quot); crint_free(tmp_rem); crint_free(a); crint_free(b); }
    );
    DNML_TEST_ASSERT((quot->cap >= a->n),
        "Insufficient Quotient Capacity: Capacity unsatisfactory for a / b (-Ediv_insufficient_qcap)",
        { crint_free(quot); crint_free(tmp_rem); crint_free(a); crint_free(b); }
    );
    DNML_TEST_ASSERT((tmp_rem->cap >= b->n),
        "Insufficient Remainder Capacity: Capacity unsatisfactory for a mod(b) (-Ediv_insufficient_rcap)",
        { crint_free(quot); crint_free(tmp_rem); crint_free(a); crint_free(b); }
    ); __CRINT_DIV_DISP__(a, b, quot, tmp_rem); // clang-format off
    quot = 0; tmp_rem = 0; a = 0; b = 0; // clang-format on
}
void __CRINT_MAGMOD__(crint *temp_quot, crint *rem, crint *a, crint *b) {
    _pre_assert(temp_quot, { crint_free(temp_quot); crint_free(rem); crint_free(a); crint_free(b); })
    _pre_assert(rem, { crint_free(temp_quot); crint_free(rem); crint_free(a); crint_free(b); })
    _pre_assert(a, { crint_free(temp_quot); crint_free(rem); crint_free(a); crint_free(b); })
    _pre_assert(b, { crint_free(temp_quot); crint_free(rem); crint_free(a); crint_free(b); })
    DNML_TEST_ASSERT((b->n),
        "Mathematical Undefinindness: Division by 0 (-Ediv_by_zero)", 
        { crint_free(temp_quot); crint_free(rem); crint_free(a); crint_free(b); }
    );
    DNML_TEST_ASSERT((temp_quot->cap >= a->n),
        "Insufficient Quotient Capacity: Capacity unsatisfactory for a / b (-Ediv_insufficient_qcap)",
        { crint_free(temp_quot); crint_free(rem); crint_free(a); crint_free(b); }
    );
    DNML_TEST_ASSERT((rem->cap >= b->n),
        "Insufficient Remainder Capacity: Capacity unsatisfactory for a mod(b) (-Ediv_insufficient_rcap)",
        { crint_free(temp_quot); crint_free(rem); crint_free(a); crint_free(b); }
    ); __CRINT_MOD_DISP__(a, b, rem, temp_quot); // clang-format off
    temp_quot = 0; rem = 0; a = 0; b = 0; // clang-format on
}
void __CRINT_MAGMUL_U64__(crint *res, crint *x, uint64_t val) {
    DNML_TEST_ASSERT(
        (_lib_crt_geq(res->cap, x->n + 1)), // Result Capacity >= Sum of multiplicand's length
        "Insufficient Product Capacity: Capacity insatisfactory for a * b (-Emul_insufficient_cap)",
        { crint_free(res); crint_free(x); val = 0; }
    ); uint64_t carry = 0, lo, hi, sum;
    for (size_t i = 0; _lib_crt_lt(i, x->n); ++i) {
        lo = __CRT_MUL_U64__(x->limbs[i], val, &hi); sum = lo + carry;
        carry = hi + (_lib_crt_lt(sum, lo)) + (_lib_crt_lt(sum, carry));
        res->limbs[i] = sum;
    }
    res->n = x->n + (!!(carry)); uint64_t curr = res->limbs[res->n - !(carry)];
    CHOOSE_OPTION((res->limbs[res->n - !(carry)]), (carry), (carry), (curr)); // clang-format off
    carry = 0; lo = 0; hi = 0; sum = 0; curr = 0; val = 0; res = 0; x = 0; val = 0; // clang-format on
}
void __CRINT_MAGDIVMOD_U64__(crint *quot, uint64_t *rem, crint *a, uint64_t val) {
    DNML_TEST_ASSERT( /* Check for a zero divisor */
        val, "Mathematical Undefinindness: Division by 0 (-Ediv_by_zero)",
        { crint_free(quot); crint(a); val = 0; }
    );
    DNML_TEST_ASSERT((quot->cap >= a->n),
        "Insufficient Quotient Capacity: Capacity unsatisfactory for a / b (-Ediv_insufficient_qcap)",
        { crint_free(quot); crint_free(a); val = 0; }
    );
    limb_t trem_limbs[1] = {0};
    crint tmp_rem = { .limbs = trem_limbs, .cap = 1, .n = 0, .sign = 1, .poisoned = false };
    __CRINT_SHORT_DIVISION__(a, val, quot, &tmp_rem); // clang-format off
    *rem = tmp_rem.limbs[0]; pbv_crint_clear((tmp_rem));
    trem_limbs[0] = 0; val = 0; quot = 0; a = 0; rem = 0; // clang-format on
}
/* --------------- MAGNITUDED MODULAR ARITHMETIC --------------- */
void __CRINT_MAGEMOD_U64__(uint64_t *rem, crint *tmp_quot, crint *dend, uint64_t mod) {}
void __CRINT_MAGEMOD__(crint *rem, crint *tmp_quot, crint *dend, crint *mod) {}
void __CRINT_MAGMADD__(crint *res, crint *a, crint *b, crint *mod) {}
void __CRINT_MAGMSUB__(crint *res, crint *a, crint *b, crint *mod) {}
void __CRINT_MAGMMUL__(crint *res, crint *a, crint *b, crint *mod) {}
void __CRINT_MAGMDIV__(crint *res, crint *a, crint *b, crint *mod) {}
void __CRINT_MAGMEXP__(crint *res, crint *a, crint *b, crint *mod) {}
void __CRINT_MAGMSQR__(crint *res, crint *a, crint *b, crint *mod) {}
void __CRINT_MAGMINV__(crint *res, crint *a, crint *b, crint *mod) {}




//* ============================================ SIGNED ARITHMETIC ========================================== */
/* ------------------- MUTATIVE ARITHMETIC -------------------- */
dnml_status crint_mut_mulu64(crint *x, uint64_t val) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), pointer_null, {});
    DNML_TEST_ASSERT((crint_pvalidate(x)), full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((!x->poisoned), crint_poisoned, { crint_free(x); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; val = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x)) { x = 0; val = 0; return CRINT_ERR_INVAL; }
    /* Actual Operation */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Main Operation */
    crint tmp_prod; dnml_status snew_stat = crint_snew(&tmp_prod, x->n + 1);
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(snew_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    ); limb_t *prev_src_buf; limb_t fake_mul = UINT64_C(0x123456789ABCDEF0);
    limb_t fsbuf[FAKE_BUF_CAP] = {0}, fpbuf[FAKE_BUF_CAP + 1] = {0};
    for (size_t i = 0; _lib_crt_lt(i, FAKE_BUF_CAP); ++i) fsbuf[i] = UINT64_MAX;
    crint fake_src = { .limbs = fsbuf, .cap = FAKE_BUF_CAP, .n = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    crint fake_prod = { .limbs = fpbuf, .cap = FAKE_BUF_CAP + 1, .n = 0, .sign = 1, .poisoned = false };
    crint *src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_src;
    crint *prod = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &tmp_prod : &fake_prod;
    __CRINT_MAGMUL_U64__(prod, src, _lib_crt_select(_lib_crt_eq(ret_stat, CRINT_SUCCESS), val, fake_mul));
    crint_normalize(prod); // Ensure the conservative size guessing of x->n + 1 is reduced correctly

    /* Finishing Touches */ limb_t *chosen_freed;
    tmp_cleanup(tmp_prod, ret_stat, chosen_freed); // Only cleanup on NON-SUCCESS CASES (ensures no memory leak)
    prev_src_buf = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x->limbs : NULL;
    crint_transfer(src, prod); free(prev_src_buf);
    /* Post-operation Aggressive Cleanup */ // clang-format off
    __libdnml_smemwipe_u64(fsbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fpbuf, FAKE_BUF_CAP + 1, 0, FAKE_BUF_CAP, false);
    pbv_crint_clear(tmp_prod); snew_stat = 0; src = 0;  prod = 0; prev_src_buf = 0; 
    fake_mul = 0; pbv_crint_clear(fake_src);  pbv_crint_clear(fake_prod); x = 0; 
    val = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_divu64(crint *x, uint64_t val) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), pointer_null, {});
    DNML_TEST_ASSERT((val), "Mathematical Undefinindness: Division by 0 (-Ediv_by_zero)", { crint_free(x); });
    DNML_TEST_ASSERT((crint_pvalidate(x)), full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((!x->poisoned), crint_poisoned, { crint_free(x); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; val = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x)) { x = 0; val = 0; return CRINT_ERR_INVAL; }
    /* Actual Operation */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (!val & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_ERR_DOMAIN), (ret_stat));
    CHOOSE_OPTION((x->poisoned),  (!val | x->poisoned), (true), (x->poisoned));
    /* Main Operation */
    crint tmp_quot; dnml_status snew_stat = crint_snew(&tmp_quot, x->n);
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(snew_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    ); limb_t *prev_src_buf; limb_t tmp_rem, fake_div = UINT64_C(0x182945687ABC16D);
    limb_t fsbuf[FAKE_BUF_CAP] = {0}, fqbuf[FAKE_BUF_CAP] = {0};
    for (size_t i = 0; _lib_crt_lt(i, FAKE_BUF_CAP); ++i) fsbuf[i] = UINT64_C(0x104562ABC89DEF18);
    crint fake_src = { .limbs = fsbuf, .cap = FAKE_BUF_CAP, .n = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    crint fake_quot = { .limbs = fqbuf, .cap = FAKE_BUF_CAP, .n = 0, .sign = 1, .poisoned = false };
    crint *src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_src;
    crint *quot = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &tmp_quot : &fake_quot;
    __CRINT_MAGDIVMOD_U64__(quot, &tmp_rem, src, _lib_crt_select(_lib_crt_eq(ret_stat, CRINT_SUCCESS), val, fake_div));
    crint_normalize(quot); // Normalize on both case to always ensure mathematical correctness

    /* Finishing Touches */ limb_t *chosen_freed;
    tmp_cleanup(tmp_quot, ret_stat, chosen_freed); // Only cleanup on NON-SUCCESS CASES (ensures no memory leak)
    prev_src_buf = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x->limbs : NULL;
    crint_transfer(src, quot); free(prev_src_buf);
    /* Post-operation Aggressive Cleanup */ // clang-format off
    __libdnml_smemwipe_u64(fsbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fqbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    pbv_crint_clear(tmp_quot); snew_stat = 0; quot = 0; src = 0;
    prev_src_buf = 0; tmp_rem = 0; fake_div = 0; pbv_crint_clear(fake_src);
    pbv_crint_clear(fake_quot); x = 0; val = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_modu64(crint *x, uint64_t val) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), pointer_null, {});
    DNML_TEST_ASSERT((val), "Mathematical Undefinindness: Division by 0 (-Ediv_by_zero)", { crint_free(x); });
    DNML_TEST_ASSERT((crint_pvalidate(x)), full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((!x->poisoned), crint_poisoned, { crint_free(x); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; val = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x)) { x = 0; val = 0; return CRINT_ERR_INVAL; }
    /* Actual Operation */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (!val & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_ERR_DOMAIN), (ret_stat));
    CHOOSE_OPTION((x->poisoned),  (!val | x->poisoned), (true), (x->poisoned));
    /* Main Operation */
    crint tmp_quot; dnml_status snew_stat = crint_snew(&tmp_quot, x->n);
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(snew_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    ); limb_t *prev_src_buf; limb_t tmp_rem, first_limb = x->limbs[0];
    limb_t fsbuf[FAKE_BUF_CAP] = {0}, fqbuf[FAKE_BUF_CAP] = {0}, fake_div = UINT64_C(0x10BCDE56271CCDEF);;
    for (size_t i = 0; _lib_crt_lt(i, FAKE_BUF_CAP); ++i) fsbuf[i] = UINT64_C(0xFF812BBEDF12345C);
    crint fake_src = { .limbs = fsbuf, .cap = FAKE_BUF_CAP, .n = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    crint fake_quot = { .limbs = fqbuf, .cap = FAKE_BUF_CAP, .n = 0, .sign = 1, .poisoned = false };
    crint *src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_src;
    crint *quot = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &tmp_quot : &fake_quot;
    __CRINT_MAGDIVMOD_U64__(quot, &tmp_rem, src, _lib_crt_select(_lib_crt_eq(ret_stat, CRINT_SUCCESS), val, fake_div));

    /* Finishing Touches */ 
    CHOOSE_OPTION((x->limbs[0]), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), (tmp_rem), (first_limb));
    CHOOSE_OPTION((x->n), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), (!!(tmp_rem)), (x->n));
    CHOOSE_OPTION((x->sign), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), (_lib_crt_select(tmp_rem, x->sign, 1)), (x->sign));
    crint_free(&tmp_quot); // Free the tmp_quot buffer, won't cause trigger even on OOM since free(NULL) is a no-op
    /* Post-operation Aggressive Cleanup */ // clang-format off
    __libdnml_smemwipe_u64(fsbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fqbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    pbv_crint_clear(tmp_quot); snew_stat = 0; quot = 0; src = 0; tmp_rem = 0; fake_div = 0; 
    pbv_crint_clear(fake_src); pbv_crint_clear(fake_quot); x = 0; val = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_muli64(crint *x, int64_t val) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), pointer_null, {});
    DNML_TEST_ASSERT((crint_pvalidate(x)), full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((!x->poisoned), crint_poisoned, { crint_free(x); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; val = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x)) { x = 0; val = 0; return CRINT_ERR_INVAL; }
    /* Actual Operation */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Main Operation */
    crint tmp_prod; dnml_status snew_stat = crint_snew(&tmp_prod, x->n + 1);
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(snew_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    ); uint64_t mag_val = __CRT_MAG_I64__(val);
    crint *src, *prod; limb_t *prev_src_buf; limb_t fake_mul = UINT64_C(0xA287323836D42B7B);
    limb_t fsbuf[FAKE_BUF_CAP] = {0}, fpbuf[FAKE_BUF_CAP + 1] = {0};
    for (size_t i = 0; _lib_crt_lt(i, FAKE_BUF_CAP); ++i) fsbuf[i] = UINT64_C(0x93426FA7CA049FB);
    crint fake_src = { .limbs = fsbuf, .cap = FAKE_BUF_CAP, .n = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    crint fake_prod = { .limbs = fpbuf, .cap = FAKE_BUF_CAP + 1, .n = 0, .sign = 1, .poisoned = false };
    src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_src;
    prod = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &tmp_prod : &fake_prod;
    __CRINT_MAGMUL_U64__(prod, src, _lib_crt_select(_lib_crt_eq(ret_stat, CRINT_SUCCESS), mag_val, fake_mul));
    prod->sign = x->sign * (_lib_crt_select(_lib_crt_isneg(val), -1, 1));
    crint_normalize(prod); // Ensure the conservative size guessing of x->n + 1 is reduced correctly

    /* Finishing Touches */ limb_t *chosen_freed;
    tmp_cleanup(tmp_prod, ret_stat, chosen_freed); // Only cleanup on NON-SUCCESS CASES (ensures no memory leak)
    prev_src_buf = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x->limbs : NULL;
    crint_transfer(src, prod); free(prev_src_buf);
    /* Post-operation Aggressive Cleanup */ // clang-format off
    __libdnml_smemwipe_u64(fsbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fpbuf, FAKE_BUF_CAP + 1, 0, FAKE_BUF_CAP, false);
    pbv_crint_clear(tmp_prod); snew_stat = 0; src = 0; prod = 0; prev_src_buf = 0;
    fake_mul = 0; pbv_crint_clear(fake_src); pbv_crint_clear(fake_prod); mag_val = 0; 
    x = 0; val = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_divi64(crint *x, int64_t val) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), pointer_null, {});
    DNML_TEST_ASSERT((val), "Mathematical Undefinindness: Division by 0 (-Ediv_by_zero)", { crint_free(x); });
    DNML_TEST_ASSERT((crint_pvalidate(x)), full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((!x->poisoned), crint_poisoned, { crint_free(x); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; val = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x)) { x = 0; val = 0; return CRINT_ERR_INVAL; }
    /* Actual Operation */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (!val & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_ERR_DOMAIN), (ret_stat));
    CHOOSE_OPTION((x->poisoned),  (!val | x->poisoned), (true), (x->poisoned));
    /* Main Operation */
    crint tmp_quot; dnml_status snew_stat = crint_snew(&tmp_quot, x->n);
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(snew_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    ); uint64_t mag_val = __CRT_MAG_I64__(val);
    limb_t *prev_src_buf; limb_t tmp_rem, fake_div = UINT64_C(0x182945687ABC16D);
    limb_t fsbuf[FAKE_BUF_CAP] = {0}, fqbuf[FAKE_BUF_CAP] = {0};
    for (size_t i = 0; _lib_crt_lt(i, FAKE_BUF_CAP); ++i) fsbuf[i] = UINT64_C(0x104562ABC89DEF18);
    crint fake_src = { .limbs = fsbuf, .cap = FAKE_BUF_CAP, .n = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    crint fake_quot = { .limbs = fqbuf, .cap = FAKE_BUF_CAP, .n = 0, .sign = 1, .poisoned = false };
    crint *src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_src;
    crint *quot = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &tmp_quot : &fake_quot;
    __CRINT_MAGDIVMOD_U64__(quot, &tmp_rem, src, _lib_crt_select(_lib_crt_eq(ret_stat, CRINT_SUCCESS), mag_val, fake_div));
    quot->sign = x->sign * (_lib_crt_select(_lib_crt_isneg(val), -1, 1));
    crint_normalize(quot); // Normalize on both case to always ensure mathematical correctness

    /* Finishing Touches */ limb_t *chosen_freed;
    tmp_cleanup(tmp_quot, ret_stat, chosen_freed); // Only cleanup on NON-SUCCESS CASES (ensures no memory leak)
    prev_src_buf = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x->limbs : NULL;
    crint_transfer(src, quot); free(prev_src_buf);
    /* Post-operation Aggressive Cleanup */ // clang-format off
    __libdnml_smemwipe_u64(fsbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fqbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    pbv_crint_clear(tmp_quot); snew_stat = 0; quot = 0; src = 0;
    prev_src_buf = 0; tmp_rem = 0; fake_div = 0; pbv_crint_clear(fake_src);
    pbv_crint_clear(fake_quot); mag_val = 0; x = 0; val = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_modi64(crint *x, int64_t val) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), pointer_null, {});
    DNML_TEST_ASSERT((val), "Mathematical Undefinindness: Division by 0 (-Ediv_by_zero)", { crint_free(x); });
    DNML_TEST_ASSERT((crint_pvalidate(x)), full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((!x->poisoned), crint_poisoned, { crint_free(x); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; val = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x)) { x = 0; val = 0; return CRINT_ERR_INVAL; }
    /* Actual Operation */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (!val & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_ERR_DOMAIN), (ret_stat));
    CHOOSE_OPTION((x->poisoned),  (!val | x->poisoned), (true), (x->poisoned));
    /* Main Operation */
    crint tmp_quot; dnml_status snew_stat = crint_snew(&tmp_quot, x->n);
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(snew_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    ); uint64_t mag_val = __CRT_MAG_I64__(val);
    limb_t *prev_src_buf; limb_t tmp_rem, first_limb = x->limbs[0];
    limb_t fsbuf[FAKE_BUF_CAP] = {0}, fqbuf[FAKE_BUF_CAP] = {0}, fake_div = UINT64_C(0x10BCDE56271CCDEF);;
    for (size_t i = 0; _lib_crt_lt(i, FAKE_BUF_CAP); ++i) fsbuf[i] = UINT64_C(0xFF812BBEDF12345C);
    crint fake_src = { .limbs = fsbuf, .cap = FAKE_BUF_CAP, .n = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    crint fake_quot = { .limbs = fqbuf, .cap = FAKE_BUF_CAP, .n = 0, .sign = 1, .poisoned = false };
    crint *src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_src;
    crint *quot = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &tmp_quot : &fake_quot;
    __CRINT_MAGDIVMOD_U64__(quot, &tmp_rem, src, _lib_crt_select(_lib_crt_eq(ret_stat, CRINT_SUCCESS), mag_val, fake_div));

    /* Finishing Touches */
    CHOOSE_OPTION((x->limbs[0]), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), (tmp_rem), (first_limb));
    CHOOSE_OPTION((x->n), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), (!!(tmp_rem)), (x->n));
    CHOOSE_OPTION((x->sign), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), (_lib_crt_select(tmp_rem, x->sign, 1)), (x->sign));
    crint_free(&tmp_quot); // Free the tmp_quot buffer, won't cause trigger even on OOM since free(NULL) is a no-op
    /* Post-operation Aggressive Cleanup */ // clang-format off
    __libdnml_smemwipe_u64(fsbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fqbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    pbv_crint_clear(tmp_quot); snew_stat = 0; quot = 0; src = 0; tmp_rem = 0; fake_div = 0; mag_val = 0;
    pbv_crint_clear(fake_src); pbv_crint_clear(fake_quot); x = 0; val = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_add(crint *x, crint y) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), pointer_null, { crint_free(&y); });
    DNML_TEST_ASSERT(crint_pvalidate(x) & crint_validate(y), full_contract, { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((!x->poisoned & !y.poisoned), crint_poisoned, { crint_free(x); crint_free(&y); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)NULL)) { pbv_crint_clear(y); x = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x) | !crint_validate(y)) { pbv_crint_clear(y); x = 0; return CRINT_ERR_INVAL; }
    /* Actual Operation */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), ((x->poisoned | y.poisoned) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Temporary Allocation and Multiplexing */
    crint tmp_sum; dnml_status snew_stat = crint_snew(&tmp_sum, crtmax(x->n, y.n) + 1);
    crint tmp_diff; dnml_status dnew_stat = crint_snew(&tmp_diff, crtmax(x->n, y.n));
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(snew_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(dnew_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    limb_t fxbuf[FAKE_BUF_CAP] = {0}, fybuf[FAKE_BUF_CAP] = {0};
    limb_t fsbuf[FAKE_BUF_CAP + 1] = {0}, fdbuf[FAKE_BUF_CAP] = {0};
    for (size_t i = 0; _lib_crt_lt(i, FAKE_BUF_CAP); ++i) {
        fxbuf[i] = UINT64_C(0xDA94C5273B9B9407); // Fill in as a fake for x
        fybuf[i] = UINT64_C(0x169984F8175ED977); // Fill in as a fake for y
    }
    crint fake_x = { .limbs = fxbuf, .cap = FAKE_BUF_CAP, .n = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    crint fake_y = { .limbs = fybuf, .cap = FAKE_BUF_CAP, .n = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    crint fake_sum = { .limbs = fsbuf, .cap = FAKE_BUF_CAP + 1, .n = 0, .sign = 1, .poisoned = false };
    crint fake_diff = { .limbs = fdbuf, .cap = FAKE_BUF_CAP, .n = 0, .sign = 1, .poisoned = false };

    /* Addition & Subtraction Execution with Dispatching */
    int8_t cmp_res = __CRINT_INTERNAL_CMP__(x, &y);
    crint *add_srcx = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_x;
    crint *add_srcy = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &y : &fake_y;
    crint *msub_minued = (_lib_crt_isneg(cmp_res)) ? &y : x;
    crint *msub_subtrahend = (_lib_crt_isneg(cmp_res)) ? x : &y;
    crint *sub_sminuend = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? msub_minued : &fake_x;
    crint *sub_ssubtrahend = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? msub_subtrahend : &fake_y;
    crint *sum = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &tmp_sum : &fake_sum;
    crint *diff = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &tmp_diff : &fake_diff;
    __CRINT_MAGADD__(sum, add_srcx, add_srcy); __CRINT_MAGSUB__(diff, sub_sminuend, sub_ssubtrahend);

    /* Re-assignment / Transferring to x */ limb_t *chosen_freed, *prev_src_buf;
    crint *trans_tres = (_lib_crt_eq(x->sign, y.sign)) ? sum : diff;
    crint *rem_tres = (_lib_crt_eq(x->sign, y.sign)) ? diff : sum;
    tmp_cleanup(tmp_sum, ret_stat, chosen_freed); // Only cleanup fully before crint_transfer on ret_stat != CRINT_SUCCESS
    tmp_cleanup(tmp_diff, ret_stat, chosen_freed); // Only cleanup fully before crint_transfer on ret_stat != CRINT_SUCCESS
    prev_src_buf = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x->limbs : NULL;
    crint_transfer(add_srcx, trans_tres); free(prev_src_buf); // ret_stat != SUCCESSS -> fake transfers + x unchanged
    add_srcx->sign = _lib_crt_select((_lib_crt_neq(x->sign, y.sign) & _lib_crt_isneg(cmp_res)), y.sign, x->sign);
    crint_normalize(add_srcx); // Normalization to ensure mathematical correctness
    // Actually free on SUCCESS, change fake buffer's limb to NULL to prevent segfaults
    rem_tres->limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? rem_tres->limbs : NULL; crint_free(rem_tres);

    /* Aggressive Post-operation Cleanup */ // clang-format off
    __libdnml_smemwipe_u64(fxbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fybuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fsbuf, FAKE_BUF_CAP + 1, 0, FAKE_BUF_CAP, false);
    __libdnml_smemwipe_u64(fdbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    pbv_crint_clear(tmp_sum); pbv_crint_clear(tmp_diff); snew_stat = 0; dnew_stat = 0;
    pbv_crint_clear(fake_x); pbv_crint_clear(fake_y); pbv_crint_clear(fake_sum); pbv_crint_clear(fake_diff);
    cmp_res = 0; add_srcx = 0; add_srcy = 0; msub_minued = 0; msub_subtrahend = 0; sub_sminuend = 0;
    sub_ssubtrahend = 0; sum = 0; diff = 0; chosen_freed = 0; prev_src_buf = 0; trans_tres = 0; rem_tres = 0;
    pbv_crint_clear(y); x = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_sub(crint *x, crint y) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), pointer_null, { crint_free(&y); });
    DNML_TEST_ASSERT(crint_pvalidate(x) & crint_validate(y), full_contract, { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((!x->poisoned & !y.poisoned), crint_poisoned, { crint_free(x); crint_free(&y); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)NULL)) { pbv_crint_clear(y); x = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x) | !crint_validate(y)) { pbv_crint_clear(y); x = 0; return CRINT_ERR_INVAL; }
    /* Actual Operation */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), ((x->poisoned | y.poisoned) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Temporary Allocation and Multiplexing */
    crint tmp_sum; dnml_status snew_stat = crint_snew(&tmp_sum, crtmax(x->n, y.n) + 1);
    crint tmp_diff; dnml_status dnew_stat = crint_snew(&tmp_diff, crtmax(x->n, y.n));
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(snew_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(dnew_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    limb_t fxbuf[FAKE_BUF_CAP] = {0}, fybuf[FAKE_BUF_CAP] = {0};
    limb_t fsbuf[FAKE_BUF_CAP + 1] = {0}, fdbuf[FAKE_BUF_CAP] = {0};
    for (size_t i = 0; _lib_crt_lt(i, FAKE_BUF_CAP); ++i) {
        fxbuf[i] = UINT64_C(0x970CBC8D57B853E3); // Fill in as a fake for x
        fybuf[i] = UINT64_C(0x820CDA0C419D9F52); // Fill in as a fake for y
    }
    crint fake_x = { .limbs = fxbuf, .cap = FAKE_BUF_CAP, .n = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    crint fake_y = { .limbs = fybuf, .cap = FAKE_BUF_CAP, .n = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    crint fake_sum = { .limbs = fsbuf, .cap = FAKE_BUF_CAP + 1, .n = 0, .sign = 1, .poisoned = false };
    crint fake_diff = { .limbs = fdbuf, .cap = FAKE_BUF_CAP, .n = 0, .sign = 1, .poisoned = false };

    /* Addition & Subtraction Execution with Dispatching */
    int8_t cmp_res = __CRINT_INTERNAL_CMP__(x, &y);
    crint *add_srcx = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_x;
    crint *add_srcy = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &y : &fake_y;
    crint *msub_minued = (_lib_crt_isneg(cmp_res)) ? &y : x;
    crint *msub_subtrahend = (_lib_crt_isneg(cmp_res)) ? x : &y;
    crint *sub_sminuend = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? msub_minued : &fake_x;
    crint *sub_ssubtrahend = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? msub_subtrahend : &fake_y;
    crint *sum = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &tmp_sum : &fake_sum;
    crint *diff = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &tmp_diff : &fake_diff;
    __CRINT_MAGADD__(sum, add_srcx, add_srcy); __CRINT_MAGSUB__(diff, sub_sminuend, sub_ssubtrahend);

    /* Re-assignment / Transferring to x */ limb_t *chosen_freed, *prev_src_buf;
    crint *trans_tres = (_lib_crt_eq(x->sign, y.sign)) ? diff : sum;
    crint *rem_tres = (_lib_crt_eq(x->sign, y.sign)) ? sum : diff;
    tmp_cleanup(tmp_sum, ret_stat, chosen_freed); // Only cleanup fully before crint_transfer on ret_stat != CRINT_SUCCESS
    tmp_cleanup(tmp_diff, ret_stat, chosen_freed); // Only cleanup fully before crint_transfer on ret_stat != CRINT_SUCCESS
    prev_src_buf = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x->limbs : NULL;
    crint_transfer(add_srcx, trans_tres); free(prev_src_buf); // ret_stat != SUCCESSS -> fake transfers + x unchanged
    add_srcx->sign = _lib_crt_select((_lib_crt_eq(x->sign, y.sign) & _lib_crt_isneg(cmp_res)), -(x->sign), x->sign);
    crint_normalize(add_srcx); // Normalization to ensure mathematical correctness
    // Actually free on SUCCESS, change fake buffer's limb to NULL to prevent segfaults
    rem_tres->limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? rem_tres->limbs : NULL; crint_free(rem_tres);

    /* Aggressive Post-operation Cleanup */ // clang-format off
    __libdnml_smemwipe_u64(fxbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fybuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fsbuf, FAKE_BUF_CAP + 1, 0, FAKE_BUF_CAP, false);
    __libdnml_smemwipe_u64(fdbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    pbv_crint_clear(tmp_sum); pbv_crint_clear(tmp_diff); snew_stat = 0; dnew_stat = 0;
    pbv_crint_clear(fake_x); pbv_crint_clear(fake_y); pbv_crint_clear(fake_sum); pbv_crint_clear(fake_diff);
    cmp_res = 0; add_srcx = 0; add_srcy = 0; msub_minued = 0; msub_subtrahend = 0; sub_sminuend = 0;
    sub_ssubtrahend = 0; sum = 0; diff = 0; chosen_freed = 0; prev_src_buf = 0; trans_tres = 0; rem_tres = 0;
    pbv_crint_clear(y); x = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_mul(crint *x, crint y) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), pointer_null, { crint_free(&y); });
    DNML_TEST_ASSERT(crint_pvalidate(x) & crint_validate(y), full_contract, { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((!x->poisoned & !y.poisoned), crint_poisoned, { crint_free(x); crint_free(&y); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)NULL)) { pbv_crint_clear(y); x = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x) | !crint_validate(y)) { pbv_crint_clear(y); x = 0; return CRINT_ERR_INVAL; }
    /* Actual Operation */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), ((x->poisoned | y.poisoned) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Main Operation */
    crint tmp_prod; dnml_status snew_stat = crint_snew(&tmp_prod, x->n + y.n);
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(snew_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    ); limb_t *prev_src_buf;
    limb_t fxbuf[FAKE_BUF_CAP] = {0}, fybuf[FAKE_BUF_CAP] = {0}, fpbuf[FAKE_BUF_CAP << 1] = {0};
    for (size_t i = 0; _lib_crt_lt(i, FAKE_BUF_CAP); ++i) {
        fxbuf[i] = UINT64_C(0x3D1C876BFA002DE4);
        fybuf[i] = UINT64_C(0x9E24480AEE54E148);
    }
    crint fake_x = { .limbs = fxbuf, .cap = FAKE_BUF_CAP, .n = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    crint fake_y = { .limbs = fybuf, .cap = FAKE_BUF_CAP, .n = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    crint fake_prod = { .limbs = fpbuf, .cap = FAKE_BUF_CAP << 1, .n = 0, .sign = 1, .poisoned = false };
    crint *mulx = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_x;
    crint *muly = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &y : &fake_y;
    crint *prod = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &tmp_prod : &fake_prod;
    __CRINT_MAGMUL___(prod, mulx, muly); prod->sign = mulx->sign * muly->sign;
    prod->sign = _lib_crt_select(prod->n, prod->sign, 1);
    crint_normalize(prod); // Ensure the conservative size guessing of x->n + 1 is reduced correctly

    /* Finishing Touches */ limb_t *chosen_freed;
    tmp_cleanup(tmp_prod, ret_stat, chosen_freed); // Only cleanup on NON-SUCCESS CASES (ensures no memory leak)
    prev_src_buf = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x->limbs : NULL;
    crint_transfer(mulx, prod); free(prev_src_buf);
    /* Post-operation Aggressive Cleanup */ // clang-format off
    __libdnml_smemwipe_u64(fxbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fybuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fpbuf, FAKE_BUF_CAP << 1, 0, (FAKE_BUF_CAP << 1) - 1, false);
    pbv_crint_clear(tmp_prod); snew_stat = 0; mulx = 0; muly = 0; prod = 0;
    prev_src_buf = 0; pbv_crint_clear(fake_x); pbv_crint_clear(fake_y); x = 0;
    pbv_crint_clear(tmp_prod); pbv_crint_clear(y); return ret_stat; // clang-format on
}
dnml_status crint_mut_div(crint *x, crint y) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), pointer_null, { crint_free(&y); });
    DNML_TEST_ASSERT(crint_pvalidate(x) & crint_validate(y), full_contract, { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((!y.n), "Mathematical Undefinindness: Division by 0 (-Ediv_by_zero)", { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((!x->poisoned & !y.poisoned), crint_poisoned, { crint_free(x); crint_free(&y); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)NULL)) { pbv_crint_clear(y); x = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x) | !crint_validate(y)) { pbv_crint_clear(y); x = 0; return CRINT_ERR_INVAL; }
    /* Actual Operation */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), ((x->poisoned | y.poisoned) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (!y.n & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_ERR_DOMAIN), (ret_stat));
    CHOOSE_OPTION((x->poisoned),  (!y.n | x->poisoned), (true), (x->poisoned));
    /* Main Operation */ limb_t *prev_src_buf;
    crint tmp_quot; dnml_status qnew_stat = crint_snew(&tmp_quot, x->n);
    crint tmp_rem; dnml_status rnew_stat = crint_snew(&tmp_rem, y.n);
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(qnew_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(rnew_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    limb_t fxbuf[FAKE_BUF_CAP] = {0}, fybuf[FAKE_BUF_CAP >> 2] = {0};
    limb_t fqbuf[FAKE_BUF_CAP] = {0}, frbuf[FAKE_BUF_CAP >> 2] = {0};
    for (size_t i = 0; _lib_crt_lt(i, FAKE_BUF_CAP); ++i) {
        size_t yindex = _lib_crt_select((i < FAKE_BUF_CAP >> 2), i, 0);
        fxbuf[i] = UINT64_C(0xC44DE1D9DB484A26);
        fybuf[yindex] = UINT64_C(0x6754B31793E979B6); yindex = 0;
    }
    crint fake_x = { .limbs = fxbuf, .cap = FAKE_BUF_CAP, .n = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    crint fake_y = { .limbs = fybuf, .cap = FAKE_BUF_CAP >> 2, .n = FAKE_BUF_CAP >> 2, .sign = 1, .poisoned = false };
    crint fake_quot = { .limbs = fqbuf, .cap = FAKE_BUF_CAP, .n = 0, .sign = 1, .poisoned = false };
    crint fake_rem = { .limbs = frbuf, .cap = FAKE_BUF_CAP >> 2, .n = 0, .sign = 1, .poisoned = false };
    crint *dend_x = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_x;
    crint *div_y = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &y : &fake_y;
    crint *quot = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &tmp_quot : &fake_quot;
    crint *rem = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &tmp_rem : &fake_rem;
    __CRINT_MAGDIV__(quot, rem, dend_x, div_y); quot->sign = dend_x->sign * div_y->sign;
    crint_normalize(quot); // Normalize on both case to always ensure mathematical correctness

    /* Finishing Touches */ limb_t *chosen_freed;
    tmp_cleanup(tmp_quot, ret_stat, chosen_freed); // Only cleanup on NON-SUCCESS CASES (ensures no memory leak)
    prev_src_buf = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x->limbs : NULL;
    crint_transfer(dend_x, quot); free(prev_src_buf); crint_free(&tmp_rem);
    /* Post-operation Aggressive Cleanup */ // clang-format off
    __libdnml_smemwipe_u64(fxbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fqbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fybuf, FAKE_BUF_CAP >> 2, 0, (FAKE_BUF_CAP >> 2) - 1, false);
    __libdnml_smemwipe_u64(frbuf, FAKE_BUF_CAP >> 2, 0, (FAKE_BUF_CAP >> 2) - 1, false);
    pbv_crint_clear(tmp_quot); pbv_crint_clear(tmp_rem); qnew_stat = 0; rnew_stat = 0;
    pbv_crint_clear(fake_x); pbv_crint_clear(fake_y); dend_x = 0 ; div_y = 0;
    pbv_crint_clear(fake_quot); pbv_crint_clear(fake_rem); quot = 0; rem = 0;
    chosen_freed = 0; x = 0; pbv_crint_clear(y); return ret_stat; // clang-format on
}
dnml_status crint_mut_mod(crint *x, crint y) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), pointer_null, { crint_free(&y); });
    DNML_TEST_ASSERT(crint_pvalidate(x) & crint_validate(y), full_contract, { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((!y.n), "Mathematical Undefinindness: Division by 0 (-Ediv_by_zero)", { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((!x->poisoned & !y.poisoned), crint_poisoned, { crint_free(x); crint_free(&y); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)NULL)) { pbv_crint_clear(y); x = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x) | !crint_validate(y)) { pbv_crint_clear(y); x = 0; return CRINT_ERR_INVAL; }
    /* Actual Operation */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), ((x->poisoned | y.poisoned) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (!y.n & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_ERR_DOMAIN), (ret_stat));
    CHOOSE_OPTION((x->poisoned),  (!y.n | x->poisoned), (true), (x->poisoned));
    /* Main Operation */ limb_t *prev_src_buf;
    crint tmp_quot; dnml_status qnew_stat = crint_snew(&tmp_quot, x->n);
    crint tmp_rem; dnml_status rnew_stat = crint_snew(&tmp_rem, y.n);
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(qnew_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(rnew_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    limb_t fxbuf[FAKE_BUF_CAP] = {0}, fybuf[FAKE_BUF_CAP >> 2] = {0};
    limb_t fqbuf[FAKE_BUF_CAP] = {0}, frbuf[FAKE_BUF_CAP >> 2] = {0};
    for (size_t i = 0; _lib_crt_lt(i, FAKE_BUF_CAP); ++i) {
        size_t yindex = _lib_crt_select((i < FAKE_BUF_CAP >> 2), i, 0);
        fxbuf[i] = UINT64_C(0x70543FDA99E95442);
        fybuf[yindex] = UINT64_C(0x1416AA6C2BD92484); yindex = 0;
    }
    crint fake_x = { .limbs = fxbuf, .cap = FAKE_BUF_CAP, .n = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    crint fake_y = { .limbs = fybuf, .cap = FAKE_BUF_CAP >> 2, .n = FAKE_BUF_CAP >> 2, .sign = 1, .poisoned = false };
    crint fake_quot = { .limbs = fqbuf, .cap = FAKE_BUF_CAP, .n = 0, .sign = 1, .poisoned = false };
    crint fake_rem = { .limbs = frbuf, .cap = FAKE_BUF_CAP >> 2, .n = 0, .sign = 1, .poisoned = false };
    crint *dend_x = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_x;
    crint *div_y = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &y : &fake_y;
    crint *quot = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &tmp_quot : &fake_quot;
    crint *rem = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &tmp_rem : &fake_rem;
    __CRINT_MAGMOD__(quot, rem, dend_x, div_y); rem->sign = dend_x->sign;
    crint_normalize(rem); // Normalize on both case to always ensure mathematical correctness

    /* Finishing Touches */ limb_t *chosen_freed;
    tmp_cleanup(tmp_rem, ret_stat, chosen_freed); // Only cleanup on NON-SUCCESS CASES (ensures no memory leak)
    prev_src_buf = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x->limbs : NULL;
    crint_transfer(dend_x, rem); free(prev_src_buf); crint_free(&tmp_quot);
    /* Post-operation Aggressive Cleanup */ // clang-format off
    __libdnml_smemwipe_u64(fxbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fqbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fybuf, FAKE_BUF_CAP >> 2, 0, (FAKE_BUF_CAP >> 2) - 1, false);
    __libdnml_smemwipe_u64(frbuf, FAKE_BUF_CAP >> 2, 0, (FAKE_BUF_CAP >> 2) - 1, false);
    pbv_crint_clear(tmp_quot); pbv_crint_clear(tmp_rem); qnew_stat = 0; rnew_stat = 0;
    pbv_crint_clear(fake_x); pbv_crint_clear(fake_y); dend_x = 0 ; div_y = 0;
    pbv_crint_clear(fake_quot); pbv_crint_clear(fake_rem); quot = 0; rem = 0;
    chosen_freed = 0; x = 0; pbv_crint_clear(y); return ret_stat; // clang-format on
}
/* ------------------ FUNCTIONAL ARITHMETIC ------------------- */
crint crint_mulu64(crint x, uint64_t val, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    preop_err((!crint_validate(x)), err, CRINT_ERR_INVAL, { pbv_crint_clear(x); val = 0; err = 0; });
    /* Actual Operation */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    crint ret_prod; dnml_status snew_stat = crint_snew(&ret_prod, x.n + 1);
    CHOOSE_OPTION((ret_stat), (
        _lib_crt_eq(snew_stat, DNML_ALLOC_OOM) & 
        _lib_crt_eq(ret_stat, CRINT_SUCCESS)), 
        (DNML_ALLOC_OOM), (ret_stat)
    ); uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS)));
    limb_t fsbuf[FAKE_BUF_CAP] = {0}, fpbuf[FAKE_BUF_CAP + 1] = {0}, fake_mul = UINT64_C(0x123456789ABCDEF0);
    for (size_t i = 0; _lib_crt_lt(i, FAKE_BUF_CAP); ++i) fsbuf[i] = UINT64_MAX;
    crint fake_src = { .limbs = fsbuf, .cap = FAKE_BUF_CAP, .n = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    crint fake_prod = { .limbs =fpbuf, .cap = FAKE_BUF_CAP + 1, .n = 0, .sign = 1, .poisoned = false };
    crint *src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x : &fake_src;
    crint *prod = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &ret_prod : &fake_prod;
    __CRINT_MAGMUL_U64__(prod, src, _lib_crt_select(_lib_crt_eq(ret_stat, CRINT_SUCCESS), val, fake_mul));
    crint_normalize(prod); // Ensure the conservative size guessing of x->n + 1 is reduced correctly
    /* Setting the Return Product metadata */
    limb_t *chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : ret_prod.limbs;
    free(chosen_freed); // Mandated as safe nop when chosen_freed == NULL since ANSI-C (C89)
    ret_prod.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? ret_prod.limbs : 0;
    ret_prod.n &= mask; ret_prod.cap &= mask; ret_prod.sign = x.sign & mask;
    ret_prod.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));

    /* Post-operation Aggressive Cleanup */ // clang-format off
    if (_lib_crt_neq((ptr_t)err, (ptr_t)NULL)) *err = ret_stat;
    __libdnml_smemwipe_u64(fsbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fpbuf, FAKE_BUF_CAP + 1, 0, FAKE_BUF_CAP, false);
    ret_stat = 0; pbv_crint_clear(fake_src); pbv_crint_clear(fake_prod); snew_stat = 0; fake_mul = 0; 
    src = 0; prod = 0; chosen_freed = 0; pbv_crint_clear(x); val = 0; err = 0; return ret_prod; // clang-format on
}
crint crint_divu64(crint x, uint64_t val, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((val), "Mathematical Undefinindness: Division by 0 (-Ediv_by_zero)", { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    preop_err((!crint_validate(x)), err, CRINT_ERR_INVAL, { pbv_crint_clear(x); val = 0; err = 0; });
    /* Actual Operation */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (!val & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Main Operation */
    crint ret_quot; dnml_status snew_stat = crint_snew(&ret_quot, x.n);
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(snew_stat, DNML_ALLOC_OOM) & 
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    ); uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS)));
    limb_t tmp_rem, fake_div = UINT64_C(0x3f1e891a29e2a5f7), fsbuf[FAKE_BUF_CAP] = {0}, fqbuf[FAKE_BUF_CAP] = {0};
    for (size_t i = 0; _lib_crt_lt(i, FAKE_BUF_CAP); ++i) fsbuf[i] = UINT64_C(0x95148E9ED7DB5BDB);
    crint fake_src = { .limbs = fsbuf, .cap = FAKE_BUF_CAP, .n = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    crint fake_quot = { .limbs = fqbuf, .cap = FAKE_BUF_CAP, .n = 0, .sign = 1, .poisoned = false };
    crint *src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x : &fake_src;
    crint *quot = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &ret_quot : &fake_quot;
    __CRINT_MAGDIVMOD_U64__(quot, &tmp_rem, src, _lib_crt_select(_lib_crt_eq(ret_stat, CRINT_SUCCESS), val, fake_div));
    crint_normalize(quot); // Normalize on both case to always ensure mathematical correctness
    /* Setting the Return Product metadata */
    limb_t *chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : ret_quot.limbs;
    free(chosen_freed); // Mandated as safe nop when chosen_freed == NULL since ANSI-C (C89)
    ret_quot.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? ret_quot.limbs : 0;
    ret_quot.n &= mask; ret_quot.cap &= mask; ret_quot.sign = x.sign & mask;
    ret_quot.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));

    /* Post-operation Aggressive Cleanup */ // clang-format off
    if (_lib_crt_neq((ptr_t)err, (ptr_t)NULL)) *err = ret_stat;
    __libdnml_smemwipe_u64(fsbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fqbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    ret_stat = 0; pbv_crint_clear(fake_src); pbv_crint_clear(fake_quot); 
    snew_stat = 0; mask = 0; tmp_rem = 0; fake_div = 0; src = 0; quot = 0; 
    chosen_freed = 0; pbv_crint_clear(x); val = 0; err = 0; return ret_quot; // clang-format on
}
crint crint_modu64(crint x, uint64_t val, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((val), "Mathematical Undefinindness: Division by 0 (-Ediv_by_zero)", { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    preop_err((!crint_validate(x)), err, CRINT_ERR_INVAL, { pbv_crint_clear(x); val = 0; err = 0; });
    /* Actual Operation */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (!val & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Main Operation */
    crint ret_rem; dnml_status rnew_stat = crint_snew(&ret_rem, 1);
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(rnew_stat, DNML_ALLOC_OOM) & 
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    crint tmp_quot; dnml_status qnew_stat = crint_snew(&tmp_quot, x.n);
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(qnew_stat, DNML_ALLOC_OOM) & 
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    ); uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS)));
    limb_t tmp_rem, fake_div = UINT64_C(0x3F1E891A29E2A5F7);
    limb_t fsbuf[FAKE_BUF_CAP] = {UINT64_C(0xC3BF40C73592C58A)}, fqbuf[FAKE_BUF_CAP] = {0};
    for (size_t i = 0; _lib_crt_lt(i, FAKE_BUF_CAP); ++i) fsbuf[i] = UINT64_C(0xC3BF40C73592C58A);
    crint fake_src = { .limbs = fsbuf, .cap = FAKE_BUF_CAP, .n = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    crint fake_quot = { .limbs = fqbuf, .cap = FAKE_BUF_CAP, .n = 0, .sign = 1, .poisoned = false };
    crint *src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x : &fake_src;
    crint *quot = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &tmp_quot : &fake_quot;
    __CRINT_MAGDIVMOD_U64__(quot, &tmp_rem, src, _lib_crt_select(_lib_crt_eq(ret_stat, CRINT_SUCCESS), val, fake_div));
    crint_free(&tmp_quot); // Free the tmp_quot buffer, won't cause trigger even on OOM since free(NULL) is a no-op
    /* Setting the Return Product metadata */ limb_t tmp_p = 0;
    ret_rem.limbs = (_lib_crt_eq(rnew_stat, DNML_ALLOC_OOM)) ? &tmp_p : ret_rem.limbs; 
    ret_rem.limbs[0] = tmp_rem & mask;
    limb_t *chosen_freed = (_lib_crt_eq(ret_stat, CRINT_POISON)) ? ret_rem.limbs : NULL;
    free(chosen_freed); // Mandated as safe nop when chosen_freed == NULL since ANSI-C (C89)
    ret_rem.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? ret_rem.limbs : 0;
    ret_rem.n = 1 & mask; ret_rem.cap &= mask; ret_rem.sign = x.sign & mask;
    ret_rem.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));

    /* Post-operation Aggressive Cleanup */ // clang-format off
    if (_lib_crt_neq((ptr_t)err, (ptr_t)NULL)) *err = ret_stat;
    __libdnml_smemwipe_u64(fsbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fqbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    ret_stat = 0; pbv_crint_clear(fake_src); pbv_crint_clear(fake_quot); qnew_stat = 0; 
    rnew_stat = 0; tmp_p = 0; mask = 0; tmp_rem = 0; fake_div = 0; src = 0; quot = 0;
    chosen_freed = 0; pbv_crint_clear(x); val = 0; err = 0; return ret_rem; // clang-format on
}
crint crint_muli64(crint x, int64_t val, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    preop_err((!crint_validate(x)), err, CRINT_ERR_INVAL, { pbv_crint_clear(x); val = 0; err = 0; });
    /* Actual Operation */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    crint ret_prod; dnml_status snew_stat = crint_snew(&ret_prod, x.n + 1);
    CHOOSE_OPTION((ret_stat), (
        _lib_crt_eq(snew_stat, DNML_ALLOC_OOM) & 
        _lib_crt_eq(ret_stat, CRINT_SUCCESS)), 
        (DNML_ALLOC_OOM), (ret_stat)
    ); 
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS))), mag_val = __CRT_MAG_I64__(val);
    limb_t fsbuf[FAKE_BUF_CAP] = {0}, fpbuf[FAKE_BUF_CAP + 1] = {0}, fake_mul = UINT64_C(0x123456789ABCDEF0);
    for (size_t i = 0; _lib_crt_lt(i, FAKE_BUF_CAP); ++i) fsbuf[i] = UINT64_MAX;
    crint fake_src = { .limbs = fsbuf, .cap = FAKE_BUF_CAP, .n = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    crint fake_prod = { .limbs =fpbuf, .cap = FAKE_BUF_CAP + 1, .n = 0, .sign = 1, .poisoned = false };
    crint *src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x : &fake_src;
    crint *prod = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &ret_prod : &fake_prod;
    __CRINT_MAGMUL_U64__(prod, src, _lib_crt_select(_lib_crt_eq(ret_stat, CRINT_SUCCESS), mag_val, fake_mul));
    crint_normalize(prod); // Ensure the conservative size guessing of x->n + 1 is reduced correctly
    /* Setting the Return Product metadata */
    limb_t *chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : ret_prod.limbs;
    free(chosen_freed); // Mandated as safe nop when chosen_freed == NULL since ANSI-C (C89)
    ret_prod.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? ret_prod.limbs : 0;
    ret_prod.n &= mask; ret_prod.cap &= mask; ret_prod.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));
    ret_prod.sign = x.sign * (_lib_crt_select(_lib_crt_isneg(val), -1, 1)) & mask;

    /* Post-operation Aggressive Cleanup */ // clang-format off
    if (_lib_crt_neq((ptr_t)err, (ptr_t)NULL)) *err = ret_stat;
    __libdnml_smemwipe_u64(fsbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fpbuf, FAKE_BUF_CAP + 1, 0, FAKE_BUF_CAP, false);
    ret_stat = 0; pbv_crint_clear(fake_src); pbv_crint_clear(fake_prod); 
    mag_val = 0; snew_stat = 0; fake_mul = 0; src = 0; prod = 0; chosen_freed = 0;
    pbv_crint_clear(x); val = 0; err = 0; return ret_prod; // clang-format on
}
crint crint_divi64(crint x, int64_t val, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((val), "Mathematical Undefinindness: Division by 0 (-Ediv_by_zero)", { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    preop_err((!crint_validate(x)), err, CRINT_ERR_INVAL, { pbv_crint_clear(x); val = 0; err = 0; });
    /* Actual Operation */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (!val & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Main Operation */
    crint ret_quot; dnml_status snew_stat = crint_snew(&ret_quot, x.n);
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(snew_stat, DNML_ALLOC_OOM) & 
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );  uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS)));
    limb_t tmp_rem, fake_div = UINT64_C(0x3f1e891a29e2a5f7); uint64_t mag_val = __CRT_MAG_I64__(val);
    limb_t fsbuf[FAKE_BUF_CAP] = {UINT64_C(0xE4B7EBA0ED88624)}, fqbuf[FAKE_BUF_CAP] = {0};
    for (size_t i = 0; _lib_crt_lt(i, FAKE_BUF_CAP); ++i) fsbuf[i] = UINT64_C(0xE4B7EBA0ED88624);
    crint fake_src = { .limbs = fsbuf, .cap = FAKE_BUF_CAP, .n = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    crint fake_quot = { .limbs = fqbuf, .cap = FAKE_BUF_CAP, .n = 0, .sign = 1, .poisoned = false };
    crint *src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x : &fake_src;
    crint *quot = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &ret_quot : &fake_quot;
    __CRINT_MAGDIVMOD_U64__(quot, &tmp_rem, src, _lib_crt_select(_lib_crt_eq(ret_stat, CRINT_SUCCESS), mag_val, fake_div));
    crint_normalize(quot); // Normalize on both case to always ensure mathematical correctness
    /* Setting the Return Product metadata */
    limb_t *chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : ret_quot.limbs;
    free(chosen_freed); // Mandated as safe nop when chosen_freed == NULL since ANSI-C (C89)
    ret_quot.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? ret_quot.limbs : 0;
    ret_quot.n &= mask; ret_quot.cap &= mask; ret_quot.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));
    ret_quot.sign = x.sign * (_lib_crt_select(_lib_crt_isneg(val), -1, 1)) & mask;

    /* Post-operation Aggressive Cleanup */ // clang-format off
    if (_lib_crt_neq((ptr_t)err, (ptr_t)NULL)) *err = ret_stat;
    __libdnml_smemwipe_u64(fsbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fqbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    ret_stat = 0; pbv_crint_clear(fake_src); pbv_crint_clear(fake_quot); 
    snew_stat = 0; mag_val = 0; mask = 0; tmp_rem = 0; fake_div = 0; src = 0; 
    quot = 0; chosen_freed = 0; pbv_crint_clear(x); val = 0; err = 0; return ret_quot; // clang-format on
}
crint crint_modi64(crint x, int64_t val, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((val), "Mathematical Undefinindness: Division by 0 (-Ediv_by_zero)", { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    preop_err((!crint_validate(x)), err, CRINT_ERR_INVAL, { pbv_crint_clear(x); val = 0; err = 0; });
    /* Actual Operation */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (!val & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Main Operation */
    crint ret_rem; dnml_status rnew_stat = crint_snew(&ret_rem, 1);
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(rnew_stat, DNML_ALLOC_OOM) & 
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    crint tmp_quot; dnml_status qnew_stat = crint_snew(&tmp_quot, x.n);
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(qnew_stat, DNML_ALLOC_OOM) & 
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    ); uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS)));
    limb_t tmp_rem, fake_div = UINT64_C(0x3f1e891a29e2a5f7); uint64_t mag_val = __CRT_MAG_I64__(val);
    limb_t fsbuf[FAKE_BUF_CAP] = {UINT64_C(0x210BB8251DA28EB9)}, fqbuf[FAKE_BUF_CAP] = {0};
    for (size_t i = 0; _lib_crt_lt(i, FAKE_BUF_CAP); ++i) fsbuf[i] = UINT64_C(0x210BB8251DA28EB9);
    crint fake_src = { .limbs = fsbuf, .cap = FAKE_BUF_CAP, .n = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    crint fake_quot = { .limbs = fqbuf, .cap = FAKE_BUF_CAP, .n = 0, .sign = 1, .poisoned = false };
    crint *src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x : &fake_src;
    crint *quot = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &tmp_quot : &fake_quot;
    __CRINT_MAGDIVMOD_U64__(quot, &tmp_rem, src, _lib_crt_select(_lib_crt_eq(ret_stat, CRINT_SUCCESS), mag_val, fake_div));
    crint_free(&tmp_quot); // Free the tmp_quot buffer, won't cause trigger even on OOM since free(NULL) is a no-op
    /* Setting the Return Product metadata */ limb_t tmp_p = 0;
    ret_rem.limbs = (_lib_crt_eq(rnew_stat, DNML_ALLOC_OOM)) ? &tmp_p : ret_rem.limbs; 
    ret_rem.limbs[0] = tmp_rem & mask;
    limb_t *chosen_freed = (_lib_crt_eq(ret_stat, CRINT_POISON)) ? ret_rem.limbs : NULL;
    free(chosen_freed); // Mandated as safe nop when chosen_freed == NULL since ANSI-C (C89)
    ret_rem.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? ret_rem.limbs : 0;
    ret_rem.n = 1 & mask; ret_rem.cap &= mask; ret_rem.sign = x.sign & mask;
    ret_rem.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));

    /* Post-operation Aggressive Cleanup */ // clang-format off
    if (_lib_crt_neq((ptr_t)err, (ptr_t)NULL)) *err = ret_stat;
    __libdnml_smemwipe_u64(fsbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fqbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    ret_stat = 0; pbv_crint_clear(fake_src); pbv_crint_clear(fake_quot); qnew_stat = 0;
    rnew_stat = 0; tmp_p = 0; mask = 0; tmp_rem = 0; fake_div = 0; src = 0; quot = 0;
    chosen_freed = 0; mag_val = 0; pbv_crint_clear(x); val = 0; err = 0; return ret_rem; // clang-format on
}
crint crint_add(crint x, crint y, dnml_status *err) {
    DNML_TEST_ASSERT(crint_validate(x) & crint_validate(y), full_contract, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((!x.poisoned & !y.poisoned), crint_poisoned, { crint_free(&x); crint_free(&y); });
    preop_err((!crint_validate(x) | !crint_validate(y)), err, CRINT_ERR_INVAL, { pbv_crint_clear(x); pbv_crint_clear(y); err = 0; });
    /* Actual Operation */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), ((x.poisoned | y.poisoned) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Result Buffer Allocation & Multiplexing */
    crint ret_sum; dnml_status snew_stat = crint_snew(&ret_sum, crtmax(x.n, y.n) + 1);
    crint ret_diff; dnml_status dnew_stat = crint_snew(&ret_diff, crtmax(x.n, y.n));
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(snew_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(dnew_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    limb_t fxbuf[FAKE_BUF_CAP] = {0}, fybuf[FAKE_BUF_CAP] = {0};
    limb_t fsbuf[FAKE_BUF_CAP + 1] = {0}, fdbuf[FAKE_BUF_CAP] = {0};
    for (size_t i = 0; _lib_crt_lt(i, FAKE_BUF_CAP); ++i) {
        fxbuf[i] = UINT64_C(0xB7C2637A58007883); // Fill in as a fake for x
        fybuf[i] = UINT64_C(0xAEC4741F660B331B); // Fill in as a fake for y
    }
    crint fake_x = { .limbs = fxbuf, .cap = FAKE_BUF_CAP, .n = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    crint fake_y = { .limbs = fybuf, .cap = FAKE_BUF_CAP, .n = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    crint fake_sum = { .limbs = fsbuf, .cap = FAKE_BUF_CAP + 1, .n = 0, .sign = 1, .poisoned = false };
    crint fake_diff = { .limbs = fdbuf, .cap = FAKE_BUF_CAP, .n = 0, .sign = 1, .poisoned = false };

    /* Addition & Subtraction Execution with Dispatching */
    // ANY FAKE BUFFERS HERE ARE PURELY FOR TIMING NORMALIZATION FOR BRANCHLESS GUARANTEES,
    // AND AREN'T USED AGAINST STATE ALTERATION FUNCTIONS LIKE crint_transfer() LIKE IN MUTATIVE VARIANTS
    int8_t cmp_res = __CRINT_INTERNAL_CMP__(&x, &y);
    crint *add_srcx = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x : &fake_x;
    crint *add_srcy = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &y : &fake_y;
    crint *msub_minued = (_lib_crt_isneg(cmp_res)) ? &y : &x;
    crint *msub_subtrahend = (_lib_crt_isneg(cmp_res)) ? &x : &y;
    crint *sub_sminuend = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? msub_minued : &fake_x;
    crint *sub_ssubtrahend = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? msub_subtrahend : &fake_y;
    crint *sum = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &ret_sum : &fake_sum;
    crint *diff = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &ret_diff : &fake_diff;
    __CRINT_MAGADD__(sum, add_srcx, add_srcy); __CRINT_MAGSUB__(diff, sub_sminuend, sub_ssubtrahend);
    crint_normalize(diff); crint_normalize(sum); // Always normalize both the sum and difference

    /* Masking to Invalidity and Preparing for returns */ limb_t *chosen_freed;
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS)));
    crint *true_ret = (_lib_crt_eq(x.sign, y.sign)) ? &ret_sum : &ret_diff;
    crint *free_ret = (_lib_crt_eq(x.sign, y.sign)) ? &ret_diff : &ret_sum;
    true_ret->sign = _lib_crt_select((_lib_crt_neq(x.sign, y.sign) & _lib_crt_isneg(cmp_res)), y.sign, x.sign);
    mask_ret(true_ret, mask, ret_stat, chosen_freed); crint_free(free_ret);

    /* Aggressive Post-operation Cleanup */ // clang-format off
    if (_lib_crt_neq((ptr_t)err, (ptr_t)NULL)) *err = ret_stat;
    __libdnml_smemwipe_u64(fxbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fybuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fsbuf, FAKE_BUF_CAP + 1, 0, FAKE_BUF_CAP, false);
    __libdnml_smemwipe_u64(fdbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    ret_stat = 0; snew_stat = 0; dnew_stat = 0; pbv_crint_clear(fake_x); pbv_crint_clear(fake_y);
    pbv_crint_clear(fake_sum); pbv_crint_clear(fake_diff); cmp_res = 0; add_srcx = 0; add_srcy = 0;
    msub_minued = 0; msub_subtrahend = 0; sub_sminuend = 0; sub_ssubtrahend = 0; sum = 0; diff = 0;
    mask = 0; free_ret = 0; pbv_crint_clear(x); pbv_crint_clear(y); err = 0; return *true_ret; // clang-format on
}
crint crint_sub(crint x, crint y, dnml_status *err) {
    DNML_TEST_ASSERT(crint_validate(x) & crint_validate(y), full_contract, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((!x.poisoned & !y.poisoned), crint_poisoned, { crint_free(&x); crint_free(&y); });
    preop_err((!crint_validate(x) | !crint_validate(y)), err, CRINT_ERR_INVAL, { pbv_crint_clear(x); pbv_crint_clear(y); err = 0; });
    /* Actual Operation */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), ((x.poisoned | y.poisoned) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Result Buffer Allocation & Multiplexing */
    crint ret_sum; dnml_status snew_stat = crint_snew(&ret_sum, crtmax(x.n, y.n) + 1);
    crint ret_diff; dnml_status dnew_stat = crint_snew(&ret_diff, crtmax(x.n, y.n));
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(snew_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(dnew_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    limb_t fxbuf[FAKE_BUF_CAP] = {0}, fybuf[FAKE_BUF_CAP] = {0};
    limb_t fsbuf[FAKE_BUF_CAP + 1] = {0}, fdbuf[FAKE_BUF_CAP] = {0};
    for (size_t i = 0; _lib_crt_lt(i, FAKE_BUF_CAP); ++i) {
        fxbuf[i] = UINT64_C(0x71FC5FEAA2A80467); // Fill in as a fake for x
        fybuf[i] = UINT64_C(0x6CF3AD2A25C35643); // Fill in as a fake for y
    }
    crint fake_x = { .limbs = fxbuf, .cap = FAKE_BUF_CAP, .n = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    crint fake_y = { .limbs = fybuf, .cap = FAKE_BUF_CAP, .n = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    crint fake_sum = { .limbs = fsbuf, .cap = FAKE_BUF_CAP + 1, .n = 0, .sign = 1, .poisoned = false };
    crint fake_diff = { .limbs = fdbuf, .cap = FAKE_BUF_CAP, .n = 0, .sign = 1, .poisoned = false };

    /* Addition & Subtraction Execution with Dispatching */
    // ANY FAKE BUFFERS HERE ARE PURELY FOR TIMING NORMALIZATION FOR BRANCHLESS GUARANTEES,
    // AND AREN'T USED AGAINST STATE ALTERATION FUNCTIONS LIKE crint_transfer() LIKE IN MUTATIVE VARIANTS
    int8_t cmp_res = __CRINT_INTERNAL_CMP__(&x, &y);
    crint *add_srcx = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x : &fake_x;
    crint *add_srcy = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &y : &fake_y;
    crint *msub_minued = (_lib_crt_isneg(cmp_res)) ? &y : &x;
    crint *msub_subtrahend = (_lib_crt_isneg(cmp_res)) ? &x : &y;
    crint *sub_sminuend = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? msub_minued : &fake_x;
    crint *sub_ssubtrahend = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? msub_subtrahend : &fake_y;
    crint *sum = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &ret_sum : &fake_sum;
    crint *diff = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &ret_diff : &fake_diff;
    __CRINT_MAGADD__(sum, add_srcx, add_srcy); __CRINT_MAGSUB__(diff, sub_sminuend, sub_ssubtrahend);
    crint_normalize(diff); crint_normalize(sum); // Always normalize both the sum and difference

    /* Masking to Invalidity and Preparing for returns */ limb_t *chosen_freed;
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS)));
    crint *true_ret = (_lib_crt_eq(x.sign, y.sign)) ? &ret_diff : &ret_sum;
    crint *free_ret = (_lib_crt_eq(x.sign, y.sign)) ? &ret_sum : &ret_diff;
    true_ret->sign = _lib_crt_select((_lib_crt_neq(x.sign, y.sign) & _lib_crt_isneg(cmp_res)), -(x.sign), x.sign);
    mask_ret(true_ret, mask, ret_stat, chosen_freed); crint_free(free_ret);

    /* Aggressive Post-operation Cleanup */ // clang-format off
    if (_lib_crt_neq((ptr_t)err, (ptr_t)NULL)) *err = ret_stat;
    __libdnml_smemwipe_u64(fxbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fybuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fsbuf, FAKE_BUF_CAP + 1, 0, FAKE_BUF_CAP, false);
    __libdnml_smemwipe_u64(fdbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    ret_stat = 0; snew_stat = 0; dnew_stat = 0; pbv_crint_clear(fake_x); pbv_crint_clear(fake_y);
    pbv_crint_clear(fake_sum); pbv_crint_clear(fake_diff); cmp_res = 0; add_srcx = 0; add_srcy = 0;
    msub_minued = 0; msub_subtrahend = 0; sub_sminuend = 0; sub_ssubtrahend = 0; sum = 0; diff = 0;
    mask = 0; free_ret = 0; pbv_crint_clear(x); pbv_crint_clear(y); err = 0; return *true_ret; // clang-format on
}
crint crint_mul(crint x, crint y, dnml_status *err) {
    DNML_TEST_ASSERT(crint_validate(x) & crint_validate(y), full_contract, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((!x.poisoned & !y.poisoned), crint_poisoned, { crint_free(&x); crint_free(&y); });
    preop_err((!crint_validate(x) | !crint_validate(y)), err, CRINT_ERR_INVAL, { pbv_crint_clear(x); pbv_crint_clear(y); err = 0; });
    /* Actual Operation */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), ((x.poisoned | y.poisoned) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Main Operation */
    crint ret_prod; dnml_status snew_stat = crint_snew(&ret_prod, x.n + y.n);
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(snew_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    limb_t fxbuf[FAKE_BUF_CAP] = {0}, fybuf[FAKE_BUF_CAP] = {0}, fpbuf[FAKE_BUF_CAP << 1] = {0};
    for (size_t i = 0; _lib_crt_lt(i, FAKE_BUF_CAP); ++i) {
        fxbuf[i] = UINT64_C(0xDAB10405F5092B14);
        fybuf[i] = UINT64_C(0xBA6A405F1ABF3A59);
    }
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS)));
    crint fake_x = { .limbs = fxbuf, .cap = FAKE_BUF_CAP, .n = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    crint fake_y = { .limbs = fybuf, .cap = FAKE_BUF_CAP, .n = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    crint fake_prod = { .limbs = fpbuf, .cap = FAKE_BUF_CAP << 1, .n = 0, .sign = 1, .poisoned = false };
    crint *mulx = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x : &fake_x;
    crint *muly = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &y : &fake_y;
    crint *prod = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &ret_prod : &fake_prod;
    __CRINT_MAGMUL___(prod, mulx, muly); prod->sign = mulx->sign * muly->sign;
    prod->sign = _lib_crt_select(prod->n, prod->sign, 1); crint_normalize(prod);
    limb_t *chosen_freed; mask_ret(&ret_prod, mask, ret_stat, chosen_freed);

    /* Aggressive Post-operation Cleanup */ // clang-format off
    if (_lib_crt_neq((ptr_t)err, (ptr_t)NULL)) *err = ret_stat;
    __libdnml_smemwipe_u64(fxbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fybuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fpbuf, FAKE_BUF_CAP << 1, 0, (FAKE_BUF_CAP << 1) - 1, false);
    ret_stat = 0; snew_stat = 0; mask = 0; pbv_crint_clear(fake_x); pbv_crint_clear(fake_y);
    pbv_crint_clear(fake_prod); mulx = 0; muly = 0; prod = 0; chosen_freed = 0; 
    pbv_crint_clear(x); pbv_crint_clear(y); err = 0; return ret_prod; // clang-format on
}
crint crint_div(crint x, crint y, dnml_status *err) {
    DNML_TEST_ASSERT(crint_validate(x) & crint_validate(y), full_contract, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((!x.poisoned & !y.poisoned), crint_poisoned, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((!y.n), "Mathematical Undefinindness: Division by 0 (-Ediv_by_zero)", { crint_free(&x); crint_free(&y); });
    preop_err((!crint_validate(x) | !crint_validate(y)), err, CRINT_ERR_INVAL, { pbv_crint_clear(x); pbv_crint_clear(y); err = 0; });
    /* Actual Operation */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), ((x.poisoned | y.poisoned) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (!y.n & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_ERR_DOMAIN), (ret_stat));
    /* Main Operation */
    crint ret_quot; dnml_status qnew_stat = crint_snew(&ret_quot, x.n);
    crint tmp_rem; dnml_status rnew_stat = crint_snew(&tmp_rem, y.n);
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(qnew_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(rnew_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    limb_t fxbuf[FAKE_BUF_CAP] = {0}, fybuf[FAKE_BUF_CAP >> 2] = {0};
    limb_t fqbuf[FAKE_BUF_CAP] = {0}, frbuf[FAKE_BUF_CAP >> 2] = {0};
    for (size_t i = 0; _lib_crt_lt(i, FAKE_BUF_CAP); ++i) {
        size_t yindex = _lib_crt_select((i < FAKE_BUF_CAP >> 2), i, 0);
        fxbuf[i] = UINT64_C(0xFB7F8C2B813BFF3C);
        fybuf[yindex] = UINT64_C(0x1BCC3D315E08D4A0); yindex = 0;
    }
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS)));
    crint fake_x = { .limbs = fxbuf, .cap = FAKE_BUF_CAP, .n = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    crint fake_y = { .limbs = fybuf, .cap = FAKE_BUF_CAP >> 2, .n = FAKE_BUF_CAP >> 2, .sign = 1, .poisoned = false };
    crint fake_quot = { .limbs = fqbuf, .cap = FAKE_BUF_CAP, .n = 0, .sign = 1, .poisoned = false };
    crint fake_rem = { .limbs = frbuf, .cap = FAKE_BUF_CAP >> 2, .n = 0, .sign = 1, .poisoned = false };
    crint *dend_x = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x : &fake_x;
    crint *div_y = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &y : &fake_y;
    crint *quot = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &ret_quot : &fake_quot;
    crint *rem = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &tmp_rem : &fake_rem;
    __CRINT_MAGDIV__(quot, rem, dend_x, div_y); quot->sign = dend_x->sign * div_y->sign;
    crint_normalize(quot); // Normalize on both case to always ensure mathematical correctness
    limb_t *chosen_freed; mask_ret(&ret_quot, mask, ret_stat, chosen_freed); crint_free(&tmp_rem);

    /* Post-operation Aggressive Cleanup */ // clang-format off
    if (_lib_crt_neq((ptr_t)err, (ptr_t)NULL)) *err = ret_stat;
    __libdnml_smemwipe_u64(fxbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fqbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fybuf, FAKE_BUF_CAP >> 2, 0, (FAKE_BUF_CAP >> 2) - 1, false);
    __libdnml_smemwipe_u64(frbuf, FAKE_BUF_CAP >> 2, 0, (FAKE_BUF_CAP >> 2) - 1, false);
    pbv_crint_clear(tmp_rem); qnew_stat = 0; rnew_stat = 0; err = 0;
    pbv_crint_clear(fake_x); pbv_crint_clear(fake_y); dend_x = 0 ; div_y = 0;
    pbv_crint_clear(fake_quot); pbv_crint_clear(fake_rem); quot = 0; rem = 0;
    chosen_freed = 0; pbv_crint_clear(x); pbv_crint_clear(y); return ret_quot; // clang-format on
}
crint crint_mod(crint x, crint y, dnml_status *err) {
    DNML_TEST_ASSERT(crint_validate(x) & crint_validate(y), full_contract, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((!x.poisoned & !y.poisoned), crint_poisoned, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((!y.n), "Mathematical Undefinindness: Division by 0 (-Ediv_by_zero)", { crint_free(&x); crint_free(&y); });
    preop_err((!crint_validate(x) | !crint_validate(y)), err, CRINT_ERR_INVAL, { pbv_crint_clear(x); pbv_crint_clear(y); err = 0; });
    /* Actual Operation */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), ((x.poisoned | y.poisoned) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (!y.n & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_ERR_DOMAIN), (ret_stat));
    /* Main Operation */
    crint tmp_quot; dnml_status qnew_stat = crint_snew(&tmp_quot, x.n);
    crint ret_rem; dnml_status rnew_stat = crint_snew(&ret_rem, y.n);
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(qnew_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(rnew_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    limb_t fxbuf[FAKE_BUF_CAP] = {0}, fybuf[FAKE_BUF_CAP >> 2] = {0};
    limb_t fqbuf[FAKE_BUF_CAP] = {0}, frbuf[FAKE_BUF_CAP >> 2] = {0};
    for (size_t i = 0; _lib_crt_lt(i, FAKE_BUF_CAP); ++i) {
        size_t yindex = _lib_crt_select((i < FAKE_BUF_CAP >> 2), i, 0);
        fxbuf[i] = UINT64_C(0xD2F5881C20D0E671);
        fybuf[yindex] = UINT64_C(0xBC6F1AE0427F65EE); yindex = 0;
    }
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS)));
    crint fake_x = { .limbs = fxbuf, .cap = FAKE_BUF_CAP, .n = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    crint fake_y = { .limbs = fybuf, .cap = FAKE_BUF_CAP >> 2, .n = FAKE_BUF_CAP >> 2, .sign = 1, .poisoned = false };
    crint fake_quot = { .limbs = fqbuf, .cap = FAKE_BUF_CAP, .n = 0, .sign = 1, .poisoned = false };
    crint fake_rem = { .limbs = frbuf, .cap = FAKE_BUF_CAP >> 2, .n = 0, .sign = 1, .poisoned = false };
    crint *dend_x = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x : &fake_x;
    crint *div_y = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &y : &fake_y;
    crint *quot = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &tmp_quot : &fake_quot;
    crint *rem = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &ret_rem : &fake_rem;
    __CRINT_MAGMOD__(quot, rem, dend_x, div_y); rem->sign = dend_x->sign;
    crint_normalize(rem); // Normalize on both case to always ensure mathematical correctness
    limb_t *chosen_freed; mask_ret(&ret_rem, mask, ret_stat, chosen_freed); crint_free(&tmp_quot);

    /* Post-operation Aggressive Cleanup */ // clang-format off
    if (_lib_crt_neq((ptr_t)err, (ptr_t)NULL)) *err = ret_stat;
    __libdnml_smemwipe_u64(fxbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fqbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fybuf, FAKE_BUF_CAP >> 2, 0, (FAKE_BUF_CAP >> 2) - 1, false);
    __libdnml_smemwipe_u64(frbuf, FAKE_BUF_CAP >> 2, 0, (FAKE_BUF_CAP >> 2) - 1, false);
    pbv_crint_clear(tmp_quot); qnew_stat = 0; rnew_stat = 0; err = 0;
    pbv_crint_clear(fake_x); pbv_crint_clear(fake_y); dend_x = 0 ; div_y = 0;
    pbv_crint_clear(fake_quot); pbv_crint_clear(fake_rem); quot = 0; rem = 0;
    chosen_freed = 0; pbv_crint_clear(x); pbv_crint_clear(y); return ret_rem; // clang-format on
}






