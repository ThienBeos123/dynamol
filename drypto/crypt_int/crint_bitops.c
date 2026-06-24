/*
Copyright (C) 2026 @ThienBeos123/@Poly-glon

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

  http://apache.org

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific  language governing permissions and
limitations under the License.
*/



#include "cryptInt_func.h"
#include "_crint_macros.h"


//* ================================= MISCALLENOUS BITWISE OPERATION ================================== */
crint crint_not(crint x, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    preop_err((!crint_validate((x))), err, CRINT_ERR_INVAL, { pbv_crint_clear(x); err = 0; })
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    crint res; dnml_status new_stat = crint_snew(&res, x.n); limb_t fake_dst; limb_t *dst;
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(new_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS)));
    for (size_t i = 0; _lib_crt_lt(i, x.n); ++i) {
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i] : &fake_dst;
        *dst = ~(x.limbs[i]);
    }
    res.n = x.n & mask; res.sign = x.sign & mask;
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res : &fake_normalized;
    crint_normalize(norm_crint);

    /* Masking Metadata to Invalidity */
    limb_t* chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Mandated to always be safe even on NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    res.poisoned = (_lib_crt_neq(ret_stat, CRINT_SUCCESS));
    /* Aggresive, Post-oepration Cleanup */ // clang-format off
    if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = ret_stat;
    dst = 0; mask = 0; ret_stat = 0;  new_stat = 0; fake_dst = 0; norm_crint = 0; fake_normalized.limbs = 0;
    fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0; fake_normalized.poisoned = 0;
    pbv_crint_clear(x); err = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_rshift(crint x, size_t k, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    preop_err((!crint_validate((x))), err, CRINT_ERR_INVAL, { pbv_crint_clear(x); err = 0; })
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    size_t limb_shift = k / U64_BITS, bshift = k % U64_BITS; crint res; dnml_status new_stat = crint_snew(&res, x.n);
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(new_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );

    limb_t fake_dst; limb_t *dst;
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS))), carry_in = 0;
    dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : x.limbs;
    size_t end; CHOOSE_OPTION((end), (x.n), (x.n - 1), (0));
    __libdnml_smemcpy_u64(dst, x.limbs, x.n, x.n, 0, end, (!(_lib_crt_eq(ret_stat, CRINT_SUCCESS))));
    __CRINT_INTERNAL_RLSHIFT__(&res, x.n, limb_shift & mask); res.n = x.n - limb_shift;
    for (size_t i = res.n; i > 0; --i) {
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i - 1] : &fake_dst;
        uint64_t next_carry = x.limbs[i - 1] & ((UINT64_C(1) << k) - 1);
        *dst = (x.limbs[i - 1] >> k) | (carry_in << (U64_BITS - k)); 
        carry_in = next_carry;
    }
    /* Normalization (with faking tricks) */
    res.n &= mask; res.sign = x.sign & mask;
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res : &fake_normalized;
    crint_normalize(norm_crint);

    /* Masking Metadata to Invalidity */
    limb_t* chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Mandated to always be safe even on NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    res.poisoned = (_lib_crt_neq(ret_stat, CRINT_SUCCESS));
    /* Aggresive, Post-oepration Cleanup */ // clang-format off
    if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = ret_stat; ret_stat = 0; new_stat = 0; limb_shift = 0;
    bshift = 0; end = 0; fake_dst = 0; dst = 0; mask = 0; carry_in = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0;
    fake_normalized.poisoned = 0; pbv_crint_clear(x); k = 0; err = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_lshift(crint x, size_t k, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    preop_err((!crint_validate((x))), err, CRINT_ERR_INVAL, { pbv_crint_clear(x); err = 0; })
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    size_t limb_shift = k / U64_BITS, bshift = k % U64_BITS; crint res; dnml_status new_stat = crint_snew(&res, x.n);
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(new_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );

    limb_t fake_dst; limb_t *dst;
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS))), discarded_bits = 0;
    dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : x.limbs;
    size_t end; CHOOSE_OPTION((end), (x.n), (x.n - 1), (0));
    __libdnml_smemcpy_u64(dst, x.limbs, x.n, x.n, 0, end, (!(_lib_crt_eq(ret_stat, CRINT_SUCCESS))));
    __CRINT_INTERNAL_LLSHIFT__(&res, x.n, limb_shift & mask); res.n = x.n;
    uint64_t iso_mask = (UINT64_C(1) << bshift) - 1;
    for (size_t i = 0; _lib_crt_lt(i, x.n); ++i) { /* Individual Bit Shift Loops */
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i] : &fake_dst;
        uint64_t next_carry = (x.limbs[i] >> (U64_BITS - bshift)) & iso_mask;
        *dst = (x.limbs[i] << k) | discarded_bits; 
        discarded_bits = next_carry; next_carry = 0;
    }
    /* Normalization (with faking tricks) */
    res.n = x.n & mask; res.sign = x.sign & mask;
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res : &fake_normalized;
    crint_normalize(norm_crint);

    /* Masking Metadata to Invalidity */
    limb_t* chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Mandated to always be safe even on NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    res.poisoned = (_lib_crt_neq(ret_stat, CRINT_SUCCESS));
    /* Aggresive, Post-oepration Cleanup */ // clang-format off
    if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = ret_stat; ret_stat = 0; new_stat = 0; limb_shift = 0;
    bshift = 0; end = 0; fake_dst = 0; dst = 0;  mask = 0; discarded_bits = 0; norm_crint = 0; iso_mask = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0;
    fake_normalized.poisoned = 0; pbv_crint_clear(x); k = 0; err = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_lshiftg(crint x, size_t k, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    preop_err((!crint_validate((x))), err, CRINT_ERR_INVAL, { pbv_crint_clear(x); err = 0; })
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    size_t limb_shift = k / U64_BITS, bshift = k % U64_BITS; crint res;
    size_t alloc_size = (x.n + limb_shift + !!(bshift)); dnml_status new_stat = crint_snew(&res, alloc_size);
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(new_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );

    limb_t fake_dst; limb_t *dst;
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS))), discarded_bits = 0;
    for (size_t i = 0; _lib_crt_lt(i, x.n); ++i) { /* Limb Shifting Loop */
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i + limb_shift] : &fake_dst;
        *dst = x.limbs[i];
    } 
    res.n = alloc_size; 
    uint64_t iso_mask = (UINT64_C(1) << k) - 1;
    for (size_t i = limb_shift; _lib_crt_lt(i, res.n); ++i) {
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i] : &fake_dst;
        uint64_t new_carry = (res.limbs[i] >> (U64_BITS - bshift)) & iso_mask;
        *dst = (res.limbs[i] << bshift) | discarded_bits;
        discarded_bits = new_carry; new_carry = 0;
    }
    /* Normalization (with faking tricks) */
    res.n &= mask; res.sign = x.sign & mask;
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res : &fake_normalized;
    crint_normalize(norm_crint);

    /* Masking Metadata to Invaldity */
    limb_t* chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Mandated to always be safe even on NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    res.poisoned = (_lib_crt_neq(ret_stat, CRINT_SUCCESS));
    /* Aggresive, Post-oepration Cleanup */ // clang-format off
    if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = ret_stat; ret_stat = 0; new_stat = 0; limb_shift = 0;
    bshift = 0; alloc_size = 0; fake_dst = 0; dst = 0; mask = 0; discarded_bits = 0; norm_crint = 0; iso_mask = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0;
    fake_normalized.poisoned = 0; pbv_crint_clear(x); k = 0; err = 0; chosen_freed = 0; return res; // clang-format on
}
dnml_status crint_mut_not(crint *x) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {});
    DNML_TEST_ASSERT((crint_pvalidate(x)), ci_full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((!x->poisoned), crint_poisoned, { crint_free(x); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x)) { x = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned), (CRINT_POISON), (ret_stat));
    for (size_t i = 0; _lib_crt_lt(i, x->n); ++i) {
        CHOOSE_OPTION((x->limbs[i]), (x->poisoned), (x->limbs[i]), (~(x->limbs[i])));
    }
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_normalized;
    crint_normalize(norm_crint);
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; x = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_rshift(crint *x, size_t k) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {});
    DNML_TEST_ASSERT((crint_pvalidate(x)), ci_full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((!x->poisoned), crint_poisoned, { crint_free(x); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x)) { x = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned), (CRINT_POISON), (ret_stat));
    size_t limb_shift = k / U64_BITS, bshift = k % U64_BITS;
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS)));

    __CRINT_INTERNAL_RLSHIFT__(x, x->n, limb_shift & mask); x->n -= limb_shift & mask;
    limb_t fake_dst; limb_t* dst; uint64_t carry_in = 0;
    for (size_t i = x->cap; i > 0; --i) {
        size_t index; CHOOSE_OPTION((index), (_lib_crt_lt(i - 1, x->n)), (i - 1), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x->limbs[i - 1] : &fake_dst;
        /* Pre-calculations - Ensure Constant Time */
        uint64_t positioned_bits = (carry_in << (U64_BITS - bshift));
        uint64_t dbit_calc = x->limbs[index] & ((UINT64_C(1) << bshift) - 1);
        uint64_t dst_val = (x->limbs[index] >> bshift) | positioned_bits;
        uint64_t curr_dval = *dst;
        /* Actual Assignment + Per Iteration Cleanup */
        CHOOSE_OPTION((carry_in), (_lib_crt_lt(i - 1, x->n)), (dbit_calc), (carry_in));
        CHOOSE_OPTION((*dst), (_lib_crt_lt(i - 1, x->n)), (dst_val), (curr_dval));
        index = 0; positioned_bits = 0; dbit_calc = 0; dst_val = 0; curr_dval = 0;
    } crint_normalize(x);
    /* Aggrestive, Post-operation Cleanup */ // clang-format off
    limb_shift = 0; bshift = 0; mask = 0; fake_dst = 0; dst = 0; carry_in = 0; 
    x = 0; k = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_lshift(crint *x, size_t k) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {});
    DNML_TEST_ASSERT((crint_pvalidate(x)), ci_full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((!x->poisoned), crint_poisoned, { crint_free(x); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x)) { x = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned), (CRINT_POISON), (ret_stat));
    size_t limb_shift = k / U64_BITS, bshift = k % U64_BITS;
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS)));

    __CRINT_INTERNAL_LLSHIFT__(x, x->n, limb_shift & mask); limb_t fake_dst; limb_t* dst; 
    uint64_t discarded_bits = 0, iso_mask = (UINT64_C(1) << bshift) - 1;
    for (size_t i = 0; _lib_crt_lt(i, x->cap); ++i) { /* Individual Bits Loop */
        size_t index; CHOOSE_OPTION((index), (_lib_crt_lt(i, x->n)), (i), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x->limbs[i] : &fake_dst;
        /* Pre-calculations - Ensure Constant Time */
        uint64_t previous_dbits = discarded_bits;
        uint64_t dbit_calc = (x->limbs[index] >> (U64_BITS - bshift)) & iso_mask;
        uint64_t dst_val = (x->limbs[index] << bshift) | previous_dbits;
        /* Actual Assignment + Per Iteration Cleanup */
        CHOOSE_OPTION((discarded_bits), (_lib_crt_lt(i, x->n)), (dbit_calc), (discarded_bits));
        CHOOSE_OPTION((*dst), (_lib_crt_lt(i, x->n)), (dst_val), (*dst));
        index = 0; previous_dbits = 0; dbit_calc = 0; dst_val = 0;
    }
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_normalized;
    crint_normalize(norm_crint);
    /* Aggrestive, Post-operation Cleanup */ // clang-format off
    limb_shift = 0; bshift = 0; mask = 0; fake_dst = 0; dst = 0; discarded_bits = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized. n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0;
    fake_normalized.poisoned = 0; x = 0; k = 0; iso_mask = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_lshiftg(crint *x, size_t k) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {});
    DNML_TEST_ASSERT((crint_pvalidate(x)), ci_full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((!x->poisoned), crint_poisoned, { crint_free(x); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x)) { x = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    size_t limb_shift = k / U64_BITS, bshift = k % U64_BITS; size_t alloc_cap = (x->n + limb_shift + !!(bshift));
    dnml_status reserve_stat = crint_reserve(x, alloc_cap); CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (reserve_stat), (ret_stat)
    ); uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS)));

    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint *llshift_operated; size_t llshift_size; // Used in LLSHIFT
    crint fake_operated = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    llshift_operated = (_lib_crt_eq(ret_stat, DNML_ALLOC_OOM)) ? &fake_operated : x;
    CHOOSE_OPTION((llshift_size), (_lib_crt_eq(ret_stat, DNML_ALLOC_OOM)), (FAKE_BUF_CAP), (x->cap));
    __CRINT_INTERNAL_LLSHIFT__(llshift_operated, llshift_size, limb_shift & mask);
    x->n += (limb_shift + !!(bshift)) & mask; limb_t fake_dst; limb_t* dst; 
    uint64_t discarded_bits = 0, iso_mask = (UINT64_C(1) << bshift) - 1;
    for (size_t i = limb_shift & mask; _lib_crt_lt(i, x->n); ++i) {
       size_t index; CHOOSE_OPTION((index), (_lib_crt_lt(i, x->n)), (i), (0));
       dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x->limbs[i] : &fake_dst;
        /* Pre-calculations - Ensure Constant Time */
        uint64_t new_carry = (x->limbs[index] >> (U64_BITS - bshift)) & iso_mask;
        uint64_t dst_val = (x->limbs[index] << bshift) | discarded_bits;
        /* Actual Assignment + Per Iteration Cleanup */
        CHOOSE_OPTION((discarded_bits), (_lib_crt_lt(i, x->n)), (new_carry), (discarded_bits));
        CHOOSE_OPTION((*dst), (_lib_crt_lt(i, x->n)), (dst_val), (*dst));
        index = 0; new_carry = 0; dst_val = 0;
    }
    crint* norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_operated;
    crint_normalize(norm_crint);
    /* Aggrestive, Post-operation Cleanup */ // clang-format off
    limb_shift = 0; bshift = 0; alloc_cap = 0; reserve_stat = 0; mask = 0; iso_mask = 0;
    __libdnml_smemwipe_u64(fake_buf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    llshift_operated = 0; llshift_size = 0; norm_crint = 0; fake_operated.poisoned = 0;
    fake_operated.limbs = 0; fake_operated.n = 0; fake_operated.cap = 0; fake_operated.sign = 0;
    fake_dst = 0; dst = 0; discarded_bits = 0; x = 0; k = 0; return ret_stat; // clang-format on
}




//* ================================= MUTATIVE, FIXED WIDTH BITWISE OPERATION ================================== */
dnml_status crint_mut_andu64(crint *x, uint64_t val) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {});
    DNML_TEST_ASSERT((crint_pvalidate(x)), ci_full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((!x->poisoned), crint_poisoned, { crint_free(x); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; val = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x)) { x = 0; val = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned), (CRINT_POISON), (ret_stat));
    uint64_t first_limb = x->limbs[0]; CHOOSE_OPTION((first_limb), (x->n), (first_limb), (0));
    first_limb = first_limb & val; int8_t ret_sign; uint64_t origin_val = x->limbs[0];
    /* Re-assignment */
    CHOOSE_OPTION((x->limbs[0]), (x->poisoned), (origin_val), (first_limb));
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (!!(first_limb)));
    CHOOSE_OPTION((ret_sign), (first_limb), (x->sign), (1));
    CHOOSE_OPTION((x->sign), (x->poisoned), (x->sign), (ret_sign));
    __libdnml_smemwipe_u64(x->limbs, x->cap, 1, x->cap - 1, (x->poisoned)); // clang-format off
    first_limb = 0; ret_sign = 0; origin_val = 0; x = 0; val = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_nandu64(crint *x, uint64_t val) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {});
    DNML_TEST_ASSERT((crint_pvalidate(x)), ci_full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((!x->poisoned), crint_poisoned, { crint_free(x); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; val = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x)) { x = 0; val = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned), (CRINT_POISON), (ret_stat));
    uint64_t first_limb = x->limbs[0]; CHOOSE_OPTION((first_limb), (x->n), (first_limb), (0));
    first_limb = ~(first_limb & val); uint64_t origin_val = x->limbs[0]; size_t ret_size;
    /* Reassignment */
    CHOOSE_OPTION((x->limbs[0]), (x->poisoned), (origin_val), (first_limb));
    CHOOSE_OPTION((ret_size), (_lib_crt_gt(x->n, 1)), (x->n), (!!(first_limb)));
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (ret_size));
    CHOOSE_OPTION((x->sign), (x->poisoned | x->n), (x->sign), (1));
    size_t end; CHOOSE_OPTION((end), (!x->n), (0), (x->n - 1));
    __libdnml_smemset_u64(x->limbs, UINT8_MAX, x->cap, 1, end, (x->poisoned));
    __libdnml_smemwipe_u64(x->limbs, x->cap, end + 1, x->cap - 1, (x->poisoned)); // clang-format off
    first_limb = 0; origin_val = 0; ret_size = 0; end = 0; x = 0; val = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_oru64(crint *x, uint64_t val) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {});
    DNML_TEST_ASSERT((crint_pvalidate(x)), ci_full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((!x->poisoned), crint_poisoned, { crint_free(x); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; val = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x)) { x = 0; val = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned), (CRINT_POISON), (ret_stat));
    uint64_t first_limb = x->limbs[0]; CHOOSE_OPTION((first_limb), (x->n), (first_limb), (0));
    first_limb = first_limb | val; uint64_t origin_val = x->limbs[0]; size_t ret_size;
    /* Reassignment */
    CHOOSE_OPTION((x->limbs[0]), (x->poisoned), (origin_val), (first_limb));
    CHOOSE_OPTION((ret_size), (_lib_crt_gt(x->n, 1)), (x->n), (!!(first_limb)));
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (ret_size));
    CHOOSE_OPTION((x->sign), (x->poisoned | x->n), (x->sign), (1));
    /* We do NOT modify the later limbs, in which BITWISE ORing by 0 makes them the same */
    __libdnml_smemwipe_u64(x->limbs, x->cap, x->n, x->cap - 1, (x->poisoned)); // clang-format off
    first_limb = 0; origin_val = 0; ret_size = 0; x = 0; val = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_noru64(crint *x, uint64_t val) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {});
    DNML_TEST_ASSERT((crint_pvalidate(x)), ci_full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((!x->poisoned), crint_poisoned, { crint_free(x); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; val = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x)) { x = 0; val = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned), (CRINT_POISON), (ret_stat));
    uint64_t first_limb = x->limbs[0]; CHOOSE_OPTION((first_limb), (x->n), (first_limb), (0));
    first_limb = ~(first_limb | val); uint64_t origin_val = x->limbs[0]; size_t ret_size;
    /* Re-assignment - Pre-Loop to handle the potentially case of size of 1 limb */
    CHOOSE_OPTION((x->limbs[0]), (x->poisoned), (origin_val), (first_limb));
    CHOOSE_OPTION((ret_size), (_lib_crt_gt(x->n, 1)), (x->n), (!!(first_limb)));
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (ret_size));
    CHOOSE_OPTION((x->sign), (x->poisoned | x->n), (x->sign), (1));
    /* Main Bitwise Operation For Loop */
    limb_t fake_dst; limb_t *dst;
    for (size_t i = 1; _lib_crt_lt(i, x->cap); ++i) {
        limb_t curr_limb = x->limbs[i]; // Always valid to access, but not to be used
        dst = (_lib_crt_lt(i, x->n) & !(x->poisoned)) ? &x->limbs[i] : &fake_dst;
        *dst = ~(curr_limb | 0); curr_limb = 0; // Current Iteration Clearance
    }
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_normalized;
    crint_normalize(norm_crint); __libdnml_smemwipe_u64(x->limbs, x->cap, x->n, x->cap - 1, (x->poisoned));
    /* Aggressive Post-operation CLeanup */ // clang-format off
    first_limb = 0; origin_val = 0; ret_size = 0; fake_dst = 0; dst = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; x = 0; val = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_xoru64(crint *x, uint64_t val) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {});
    DNML_TEST_ASSERT((crint_pvalidate(x)), ci_full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((!x->poisoned), crint_poisoned, { crint_free(x); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; val = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x)) { x = 0; val = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned), (CRINT_POISON), (ret_stat));
    uint64_t first_limb = x->limbs[0]; CHOOSE_OPTION((first_limb), (x->n), (first_limb), (0));
    first_limb = first_limb ^ val; uint64_t origin_val = x->limbs[0]; size_t ret_size;
    /* Re-assignment - Pre-Loop to handle the potentially case of size of 1 limb */
    CHOOSE_OPTION((x->limbs[0]), (x->poisoned), (origin_val), (first_limb));
    CHOOSE_OPTION((ret_size), (_lib_crt_gt(x->n, 1)), (x->n), (!!(first_limb)));
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (ret_size));
    CHOOSE_OPTION((x->sign), (x->poisoned | x->n), (x->sign), (1));
    /* Main Bitwise Operation For Loop */
    limb_t fake_dst; limb_t *dst;
    for (size_t i = 1; _lib_crt_lt(i, x->cap); ++i) {
        limb_t curr_limb = x->limbs[i]; // Always valid to access, but not to be used
        dst = (_lib_crt_lt(i, x->n) & !(x->poisoned)) ? &x->limbs[i] : &fake_dst;
        *dst = curr_limb ^ 0; curr_limb = 0; // Current Iteration Clearance
    }
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_normalized;
    crint_normalize(norm_crint); __libdnml_smemwipe_u64(x->limbs, x->cap, x->n, x->cap - 1, (x->poisoned));
    /* Aggressive Post-operation CLeanup */ // clang-format off
    first_limb = 0; origin_val = 0; ret_size = 0; fake_dst = 0; dst = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; x = 0; val = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_xnoru64(crint *x, uint64_t val) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {});
    DNML_TEST_ASSERT((crint_pvalidate(x)), ci_full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((!x->poisoned), crint_poisoned, { crint_free(x); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; val = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x)) { x = 0; val = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned), (CRINT_POISON), (ret_stat));
    uint64_t first_limb = x->limbs[0]; CHOOSE_OPTION((first_limb), (x->n), (first_limb), (0));
    first_limb = ~(first_limb ^ val); uint64_t origin_val = x->limbs[0]; size_t ret_size;
    /* Re-assignment - Pre-Loop to handle the potentially case of size of 1 limb */
    CHOOSE_OPTION((x->limbs[0]), (x->poisoned), (origin_val), (first_limb));
    CHOOSE_OPTION((ret_size), (_lib_crt_gt(x->n, 1)), (x->n), (!!(first_limb)));
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (ret_size));
    CHOOSE_OPTION((x->sign), (x->poisoned | x->n), (x->sign), (1));
    /* Main Bitwise Operation For Loop */
    limb_t fake_dst; limb_t *dst;
    for (size_t i = 1; _lib_crt_lt(i, x->cap); ++i) {
        limb_t curr_limb = x->limbs[i]; // Always valid to access, but not to be used
        dst = (_lib_crt_lt(i, x->n) & !(x->poisoned)) ? &x->limbs[i] : &fake_dst;
        *dst = ~(curr_limb ^ 0); curr_limb = 0; // Current Iteration Clearance
    }
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_normalized;
    crint_normalize(norm_crint); __libdnml_smemwipe_u64(x->limbs, x->cap, x->n, x->cap - 1, (x->poisoned));
    /* Aggressive Post-operation CLeanup */ // clang-format off
    first_limb = 0; origin_val = 0; ret_size = 0; fake_dst = 0; dst = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; x = 0; val = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_and(crint *x, crint y) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, { crint_free(&y); });
    DNML_TEST_ASSERT((crint_pvalidate(x) & crint_validate(y)), ci_full_contract, { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((!x->poisoned & !y.poisoned), crint_poisoned, { crint_free(x); crint_free(&y); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { pbv_crint_clear(y); x = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x) | !crint_validate(y)) { pbv_crint_clear(y); x = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (y.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    size_t op_range = crtmax(x->n, y.n); dnml_status reserve_stat = crint_reserve(x, op_range);
    DNML_TEST_ASSERT((_lib_crt_neq(reserve_stat, DNML_ALLOC_OOM)), realloc_null, { crint_free(x); crint_free(&y); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Main Loop and Fake-handling for OOM */
    limb_t fake_dst = 123, fake_src = 456; limb_t *dst, *src;
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x->limbs[i] : &fake_dst;
        src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &y.limbs[i] : &fake_src;
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src), (0));
        *dst = a & b; a = 0; b = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (op_range));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_normalized;
    crint_normalize(norm_crint);
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    op_range = 0; reserve_stat = 0;; fake_dst = 0; dst = 0; fake_src = 0; src = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0;
    fake_normalized.poisoned = 0; x = 0; pbv_crint_clear(y); return ret_stat; // clang-format on
}
dnml_status crint_mut_nand(crint *x, crint y) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, { crint_free(&y); });
    DNML_TEST_ASSERT((crint_pvalidate(x) & crint_validate(y)), ci_full_contract, { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((!x->poisoned & !y.poisoned), crint_poisoned, { crint_free(x); crint_free(&y); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { pbv_crint_clear(y); x = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x) | !crint_validate(y)) { pbv_crint_clear(y); x = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (y.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    size_t op_range = crtmax(x->n, y.n); dnml_status reserve_stat = crint_reserve(x, op_range);
    DNML_TEST_ASSERT((_lib_crt_neq(reserve_stat, DNML_ALLOC_OOM)), realloc_null, { crint_free(x); crint_free(&y); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Main Loop and Fake-handling for OOM */
    limb_t fake_dst = 123, fake_src = 456; limb_t *dst, *src;
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x->limbs[i] : &fake_dst;
        src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &y.limbs[i] : &fake_src;
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src), (0));
        *dst = ~(a & b); a = 0; b = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (op_range));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_normalized;
    crint_normalize(norm_crint);
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    op_range = 0; reserve_stat = 0;; fake_dst = 0; dst = 0; fake_src = 0; src = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0;
    fake_normalized.poisoned = 0; x = 0; pbv_crint_clear(y); return ret_stat; // clang-format on
}
dnml_status crint_mut_or(crint *x, crint y) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, { crint_free(&y); });
    DNML_TEST_ASSERT((crint_pvalidate(x) & crint_validate(y)), ci_full_contract, { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((!x->poisoned & !y.poisoned), crint_poisoned, { crint_free(x); crint_free(&y); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { pbv_crint_clear(y); x = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x) | !crint_validate(y)) { pbv_crint_clear(y); x = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (y.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    size_t op_range = crtmax(x->n, y.n); dnml_status reserve_stat = crint_reserve(x, op_range);
    DNML_TEST_ASSERT((_lib_crt_neq(reserve_stat, DNML_ALLOC_OOM)), realloc_null, { crint_free(x); crint_free(&y); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Main Loop and Fake-handling for OOM */
    limb_t fake_dst = 123, fake_src = 456; limb_t *dst, *src;
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x->limbs[i] : &fake_dst;
        src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &y.limbs[i] : &fake_src;
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src), (0));
        *dst = (a | b); a = 0; b = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (op_range));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_normalized;
    crint_normalize(norm_crint);
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    op_range = 0; reserve_stat = 0;; fake_dst = 0; dst = 0; fake_src = 0; src = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0;
    fake_normalized.poisoned = 0; x = 0; pbv_crint_clear(y); return ret_stat; // clang-format on
}
dnml_status crint_mut_nor(crint *x, crint y) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, { crint_free(&y); });
    DNML_TEST_ASSERT((crint_pvalidate(x) & crint_validate(y)), ci_full_contract, { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((!x->poisoned & !y.poisoned), crint_poisoned, { crint_free(x); crint_free(&y); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { pbv_crint_clear(y); x = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x) | !crint_validate(y)) { pbv_crint_clear(y); x = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (y.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    size_t op_range = crtmax(x->n, y.n); dnml_status reserve_stat = crint_reserve(x, op_range);
    DNML_TEST_ASSERT((_lib_crt_neq(reserve_stat, DNML_ALLOC_OOM)), realloc_null, { crint_free(x); crint_free(&y); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Main Loop and Fake-handling for OOM */
    limb_t fake_dst = 123, fake_src = 456; limb_t *dst, *src;
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x->limbs[i] : &fake_dst;
        src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &y.limbs[i] : &fake_src;
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src), (0));
        *dst = ~(a | b); a = 0; b = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (op_range));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_normalized;
    crint_normalize(norm_crint);
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    op_range = 0; reserve_stat = 0;; fake_dst = 0; dst = 0; fake_src = 0; src = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0;
    fake_normalized.poisoned = 0; x = 0; pbv_crint_clear(y); return ret_stat; // clang-format on
}
dnml_status crint_mut_xor(crint *x, crint y) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, { crint_free(&y); });
    DNML_TEST_ASSERT((crint_pvalidate(x) & crint_validate(y)), ci_full_contract, { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((!x->poisoned & !y.poisoned), crint_poisoned, { crint_free(x); crint_free(&y); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { pbv_crint_clear(y); x = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x) | !crint_validate(y)) { pbv_crint_clear(y); x = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (y.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    size_t op_range = crtmax(x->n, y.n); dnml_status reserve_stat = crint_reserve(x, op_range);
    DNML_TEST_ASSERT((_lib_crt_neq(reserve_stat, DNML_ALLOC_OOM)), realloc_null, { crint_free(x); crint_free(&y); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Main Loop and Fake-handling for OOM */
    limb_t fake_dst = 123, fake_src = 456; limb_t *dst, *src;
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x->limbs[i] : &fake_dst;
        src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &y.limbs[i] : &fake_src;
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src), (0));
        *dst = (a ^ b); a = 0; b = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (op_range));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_normalized;
    crint_normalize(norm_crint);
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    op_range = 0; reserve_stat = 0;; fake_dst = 0; dst = 0; fake_src = 0; src = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0;
    fake_normalized.poisoned = 0; x = 0; pbv_crint_clear(y); return ret_stat; // clang-format on
}
dnml_status crint_mut_xnor(crint *x, crint y) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, { crint_free(&y); });
    DNML_TEST_ASSERT((crint_pvalidate(x) & crint_validate(y)), ci_full_contract, { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((!x->poisoned & !y.poisoned), crint_poisoned, { crint_free(x); crint_free(&y); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { pbv_crint_clear(y); x = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x) | !crint_validate(y)) { pbv_crint_clear(y); x = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (y.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    size_t op_range = crtmax(x->n, y.n); dnml_status reserve_stat = crint_reserve(x, op_range);
    DNML_TEST_ASSERT((_lib_crt_neq(reserve_stat, DNML_ALLOC_OOM)), realloc_null, { crint_free(x); crint_free(&y); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Main Loop and Fake-handling for OOM */
    limb_t fake_dst = 123, fake_src = 456; limb_t *dst, *src;
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x->limbs[i] : &fake_dst;
        src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &y.limbs[i] : &fake_src;
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src), (0));
        *dst = ~(a ^ b); a = 0; b = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (op_range));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_normalized;
    crint_normalize(norm_crint);
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    op_range = 0; reserve_stat = 0; fake_dst = 0; dst = 0; fake_src = 0; src = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0;
    fake_normalized.poisoned = 0; x = 0; pbv_crint_clear(y); return ret_stat; // clang-format on
}




//* ================================= MUTATIVE, EXPLICIT WIDTH BITWISE OPERATION ================================== */
dnml_status crint_mutex_andu64(crint *x, uint64_t val, size_t op_range) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {});
    DNML_TEST_ASSERT((crint_pvalidate(x)), ci_full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((!x->poisoned), crint_poisoned, { crint_free(x); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; val = 0; op_range = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x)) { x = 0; val = 0; op_range = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS; uint8_t noop = false;
    CHOOSE_OPTION((ret_stat), (x->poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    noop |= (x->poisoned); noop |= (!(op_range)); uint64_t first_limb = x->limbs[0];
    CHOOSE_OPTION((first_limb), (x->n), (first_limb), (0)); first_limb = first_limb & val;
    int8_t ret_sign; uint64_t origin_val = x->limbs[0];
    /* Re-assignment */
    CHOOSE_OPTION((x->limbs[0]), (x->poisoned | !op_range), (origin_val), (first_limb));
    CHOOSE_OPTION((x->n), (x->poisoned | !op_range), (x->n), (!!(first_limb)));
    CHOOSE_OPTION((ret_sign), (first_limb), (x->sign), (1));
    CHOOSE_OPTION((x->sign), (x->poisoned | !op_range), (x->sign), (ret_sign));
    /* Reservation */
    size_t alloc_range; CHOOSE_OPTION((alloc_range), (noop), (0), (op_range));
    dnml_status reserve_stat = crint_reserve(x, alloc_range);
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    ); uint64_t oom_mask = (uint64_t)(-(int64_t)(_lib_crt_neq(ret_stat, DNML_ALLOC_OOM)));
    /* Clearing out the other limbs */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; limb_t *dst; size_t buf_cap;
    dst = (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)) ? fake_buf : x->limbs;
    CHOOSE_OPTION((buf_cap), (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)), (FAKE_BUF_CAP), (x->cap));
    __libdnml_smemwipe_u64(dst, buf_cap, 1, buf_cap - 1, (noop));
    /* Aggressive Memory Clearance */ // clang-format off
    x->cap &= oom_mask; x->n &= oom_mask; x->sign &= oom_mask; x->poisoned &= oom_mask;
    noop = 0; first_limb = 0; ret_sign = 0; origin_val = 0; alloc_range = 0; reserve_stat = 0;
    dst = 0; buf_cap = 0; oom_mask = 0; x = 0; val = 0; op_range = 0; return ret_stat; // clang-format on
}
dnml_status crint_mutex_nandu64(crint *x, uint64_t val, size_t op_range) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {});
    DNML_TEST_ASSERT((crint_pvalidate(x)), ci_full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((!x->poisoned), crint_poisoned, { crint_free(x); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; val = 0; op_range = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x)) { x = 0; val = 0; op_range = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    dnml_status reserve_stat = crint_reserve(x, op_range);
    DNML_TEST_ASSERT((_lib_crt_neq(reserve_stat, DNML_ALLOC_OOM)), realloc_null, { crint_free(x); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Main Loop and Fake-handling for OOM */ limb_t fake_dst = 123; limb_t *dst;
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) { uint64_t a, b;
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x->limbs[i] : &fake_dst;
        CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0)); CHOOSE_OPTION((b), (!i), (val), (0));
        *dst = ~(a & b); a = 0; b = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_normalized;
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; fake_dst = 0; dst = 0; norm_crint = 0; fake_normalized.limbs = 0;
    fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0;
    fake_normalized.poisoned = 0; x = 0; val = 0; op_range = 0; return ret_stat; // clang-format on
}
dnml_status crint_mutex_oru64(crint *x, uint64_t val, size_t op_range) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {});
    DNML_TEST_ASSERT((crint_pvalidate(x)), ci_full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((!x->poisoned), crint_poisoned, { crint_free(x); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; val = 0; op_range = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x)) { x = 0; val = 0; op_range = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS; uint8_t noop = false;
    CHOOSE_OPTION((ret_stat), (x->poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    noop |= (x->poisoned); noop |= (!(op_range)); uint64_t first_limb = x->limbs[0];
    CHOOSE_OPTION((first_limb), (x->n), (first_limb), (0)); first_limb = first_limb | val;
    int8_t ret_sign; uint64_t origin_val = x->limbs[0];
    /* Re-assignment */
    CHOOSE_OPTION((x->limbs[0]), (x->poisoned | !op_range), (origin_val), (first_limb));
    CHOOSE_OPTION((x->n), (x->poisoned | !op_range), (x->n), (!!(first_limb)));
    CHOOSE_OPTION((ret_sign), (first_limb), (x->sign), (1));
    CHOOSE_OPTION((x->sign), (x->poisoned | !op_range), (x->sign), (ret_sign));
    /* Reservation */
    size_t alloc_range; CHOOSE_OPTION((alloc_range), (noop), (0), (op_range));
    dnml_status reserve_stat = crint_reserve(x, alloc_range);
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    ); uint64_t oom_mask = (uint64_t)(-(int64_t)(_lib_crt_neq(ret_stat, DNML_ALLOC_OOM)));
    /* Clearing out the other limbs */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; limb_t *dst; size_t end, start, buf_cap;
    dst = (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)) ? fake_buf : x->limbs;
    CHOOSE_OPTION((start), (_lib_crt_lt(op_range, x->n)), (x->n), (0));
    CHOOSE_OPTION((start), (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)), (0), (start));
    CHOOSE_OPTION((end), (_lib_crt_lt(op_range, x->n)), (op_range - 1), (x->n - op_range));
    CHOOSE_OPTION((end), (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)), (FAKE_BUF_CAP), (end));
    CHOOSE_OPTION((buf_cap), (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)), (FAKE_BUF_CAP), (x->cap));
    __libdnml_smemwipe_u64(dst, buf_cap, start, end, (noop));
    /* Aggressive Memory Clearance */ // clang-format off
    x->cap &= oom_mask; x->n &= oom_mask; x->sign &= oom_mask; x->poisoned &= oom_mask;
    noop = 0; first_limb = 0; ret_sign = 0; origin_val = 0; alloc_range = 0; reserve_stat = 0;
    dst = 0; start = 0; end = 0; buf_cap = 0; oom_mask = 0; x = 0; val = 0; op_range = 0; return ret_stat; // clang-format on
}
dnml_status crint_mutex_noru64(crint *x, uint64_t val, size_t op_range) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {});
    DNML_TEST_ASSERT((crint_pvalidate(x)), ci_full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((!x->poisoned), crint_poisoned, { crint_free(x); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; val = 0; op_range = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x)) { x = 0; val = 0; op_range = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    dnml_status reserve_stat = crint_reserve(x, op_range);
    DNML_TEST_ASSERT((_lib_crt_neq(reserve_stat, DNML_ALLOC_OOM)), realloc_null, { crint_free(x); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Main Loop and Fake-handling for OOM */ limb_t fake_dst = 123; limb_t *dst;
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) { uint64_t a, b;
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x->limbs[i] : &fake_dst;
        CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0)); CHOOSE_OPTION((b), (!i), (val), (0));
        *dst = ~(a | b); a = 0; b = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_normalized;
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; fake_dst = 0; dst = 0; norm_crint = 0; fake_normalized.limbs = 0;
    fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0;
    fake_normalized.poisoned = 0; x = 0; val = 0; op_range = 0; return ret_stat; // clang-format on
}
dnml_status crint_mutex_xoru64(crint *x, uint64_t val, size_t op_range) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {});
    DNML_TEST_ASSERT((crint_pvalidate(x)), ci_full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((!x->poisoned), crint_poisoned, { crint_free(x); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; val = 0; op_range = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x)) { x = 0; val = 0; op_range = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    dnml_status reserve_stat = crint_reserve(x, op_range);
    DNML_TEST_ASSERT((_lib_crt_neq(reserve_stat, DNML_ALLOC_OOM)), realloc_null, { crint_free(x); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Main Loop and Fake-handling for OOM */ limb_t fake_dst = 123; limb_t *dst;
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) { uint64_t a, b;
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x->limbs[i] : &fake_dst;
        CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0)); CHOOSE_OPTION((b), (!i), (val), (0));
        *dst = (a ^ b); a = 0; b = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_normalized;
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; fake_dst = 0; dst = 0; norm_crint = 0; fake_normalized.limbs = 0;
    fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0;
    fake_normalized.poisoned = 0; x = 0; val = 0; op_range = 0; return ret_stat; // clang-format on
}
dnml_status crint_mutex_xnoru64(crint *x, uint64_t val, size_t op_range) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {});
    DNML_TEST_ASSERT((crint_pvalidate(x)), ci_full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((!x->poisoned), crint_poisoned, { crint_free(x); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; val = 0; op_range = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x)) { x = 0; val = 0; op_range = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    dnml_status reserve_stat = crint_reserve(x, op_range);
    DNML_TEST_ASSERT((_lib_crt_neq(reserve_stat, DNML_ALLOC_OOM)), realloc_null, { crint_free(x); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Main Loop and Fake-handling for OOM */
    limb_t fake_dst = 123; limb_t *dst;
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        uint64_t a, b;
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x->limbs[i] : &fake_dst;
        CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0)); CHOOSE_OPTION((b), (!i), (val), (0));
        *dst = ~(a ^ b); a = 0; b = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_normalized;
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; fake_dst = 0; dst = 0; norm_crint = 0; fake_normalized.limbs = 0;
    fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0;
    fake_normalized.poisoned = 0; x = 0; val = 0; op_range = 0; return ret_stat; // clang-format on
}
dnml_status crint_mutex_andi64(crint *x, int64_t val, size_t op_range) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {});
    DNML_TEST_ASSERT((crint_pvalidate(x)), ci_full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((!x->poisoned), crint_poisoned, { crint_free(x); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; val = 0; op_range = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x)) { x = 0; val = 0; op_range = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    dnml_status reserve_stat = crint_reserve(x, op_range);
    DNML_TEST_ASSERT((_lib_crt_neq(reserve_stat, DNML_ALLOC_OOM)), realloc_null, { crint_free(x); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Main Loop and Fake-handling for OOM */
    uint64_t extension_bits; CHOOSE_OPTION((extension_bits), (_lib_crt_isneg(val)), (UINT64_MAX), (0));
    limb_t fake_dst = 123; limb_t* dst; size_t index;
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        uint64_t a, b, mag_val = __CRT_MAG_I64__(val);
        CHOOSE_OPTION((index), (_lib_crt_lt(i, x->n)), (i), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x->limbs[index] : &fake_dst;
        CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0));
        CHOOSE_OPTION((b), (!i), (mag_val), (extension_bits));
        *dst = (a & b); a = 0; b = 0; mag_val = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_normalized;
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; extension_bits = 0; fake_dst = 0; dst = 0; index = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0;
    fake_normalized.poisoned = 0; x = 0; val = 0; op_range = 0; return ret_stat; // clang-format-on
}
dnml_status crint_mutex_nandi64(crint *x, int64_t val, size_t op_range) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {});
    DNML_TEST_ASSERT((crint_pvalidate(x)), ci_full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((!x->poisoned), crint_poisoned, { crint_free(x); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; val = 0; op_range = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x)) { x = 0; val = 0; op_range = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    dnml_status reserve_stat = crint_reserve(x, op_range);
    DNML_TEST_ASSERT((_lib_crt_neq(reserve_stat, DNML_ALLOC_OOM)), realloc_null, { crint_free(x); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Main Loop and Fake-handling for OOM */
    uint64_t extension_bits; CHOOSE_OPTION((extension_bits), (_lib_crt_isneg(val)), (UINT64_MAX), (0));
    limb_t fake_dst = 123; limb_t* dst; size_t index;
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        uint64_t a, b, mag_val = __CRT_MAG_I64__(val);
        CHOOSE_OPTION((index), (_lib_crt_lt(i, x->n)), (i), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x->limbs[index] : &fake_dst;
        CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0));
        CHOOSE_OPTION((b), (!i), (mag_val), (extension_bits));
        *dst = ~(a & b); a = 0; b = 0; mag_val = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_normalized;
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; extension_bits = 0; fake_dst = 0; dst = 0; index = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0;
    fake_normalized.poisoned = 0; x = 0; val = 0; op_range = 0; return ret_stat; // clang-format-on
}
dnml_status crint_mutex_ori64(crint *x, int64_t val, size_t op_range) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {});
    DNML_TEST_ASSERT((crint_pvalidate(x)), ci_full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((!x->poisoned), crint_poisoned, { crint_free(x); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; val = 0; op_range = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x)) { x = 0; val = 0; op_range = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    dnml_status reserve_stat = crint_reserve(x, op_range);
    DNML_TEST_ASSERT((_lib_crt_neq(reserve_stat, DNML_ALLOC_OOM)), realloc_null, { crint_free(x); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Main Loop and Fake-handling for OOM */
    uint64_t extension_bits; CHOOSE_OPTION((extension_bits), (_lib_crt_isneg(val)), (UINT64_MAX), (0));
    limb_t fake_dst = 123; limb_t* dst; size_t index;
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        uint64_t a, b, mag_val = __CRT_MAG_I64__(val);
        CHOOSE_OPTION((index), (_lib_crt_lt(i, x->n)), (i), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x->limbs[index] : &fake_dst;
        CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0));
        CHOOSE_OPTION((b), (!i), (mag_val), (extension_bits));
        *dst = (a | b); a = 0; b = 0; mag_val = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_normalized;
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; extension_bits = 0; fake_dst = 0; dst = 0; index = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0;
    fake_normalized.poisoned = 0; x = 0; val = 0; op_range = 0; return ret_stat; // clang-format-on
}
dnml_status crint_mutex_nori64(crint *x, int64_t val, size_t op_range) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {});
    DNML_TEST_ASSERT((crint_pvalidate(x)), ci_full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((!x->poisoned), crint_poisoned, { crint_free(x); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; val = 0; op_range = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x)) { x = 0; val = 0; op_range = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    dnml_status reserve_stat = crint_reserve(x, op_range);
    DNML_TEST_ASSERT((_lib_crt_neq(reserve_stat, DNML_ALLOC_OOM)), realloc_null, { crint_free(x); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Main Loop and Fake-handling for OOM */
    uint64_t extension_bits; CHOOSE_OPTION((extension_bits), (_lib_crt_isneg(val)), (UINT64_MAX), (0));
    limb_t fake_dst = 123; limb_t* dst; size_t index;
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        uint64_t a, b, mag_val = __CRT_MAG_I64__(val);
        CHOOSE_OPTION((index), (_lib_crt_lt(i, x->n)), (i), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x->limbs[index] : &fake_dst;
        CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0));
        CHOOSE_OPTION((b), (!i), (mag_val), (extension_bits));
        *dst = ~(a | b); a = 0; b = 0; mag_val = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_normalized;
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; extension_bits = 0; fake_dst = 0; dst = 0; index = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0;
    fake_normalized.poisoned = 0; x = 0; val = 0; op_range = 0; return ret_stat; // clang-format-on
}
dnml_status crint_mutex_xori64(crint *x, int64_t val, size_t op_range) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {});
    DNML_TEST_ASSERT((crint_pvalidate(x)), ci_full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((!x->poisoned), crint_poisoned, { crint_free(x); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; val = 0; op_range = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x)) { x = 0; val = 0; op_range = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    dnml_status reserve_stat = crint_reserve(x, op_range);
    DNML_TEST_ASSERT((_lib_crt_neq(reserve_stat, DNML_ALLOC_OOM)), realloc_null, { crint_free(x); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Main Loop and Fake-handling for OOM */
    uint64_t extension_bits; CHOOSE_OPTION((extension_bits), (_lib_crt_isneg(val)), (UINT64_MAX), (0));
    limb_t fake_dst = 123; limb_t* dst; size_t index;
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        uint64_t a, b, mag_val = __CRT_MAG_I64__(val);
        CHOOSE_OPTION((index), (_lib_crt_lt(i, x->n)), (i), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x->limbs[index] : &fake_dst;
        CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0));
        CHOOSE_OPTION((b), (!i), (mag_val), (extension_bits));
        *dst = (a ^ b); a = 0; b = 0; mag_val = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_normalized;
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; extension_bits = 0; fake_dst = 0; dst = 0; index = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0;
    fake_normalized.poisoned = 0; x = 0; val = 0; op_range = 0; return ret_stat; // clang-format-on
}
dnml_status crint_mutex_xnori64(crint *x, int64_t val, size_t op_range) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {});
    DNML_TEST_ASSERT((crint_pvalidate(x)), ci_full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((!x->poisoned), crint_poisoned, { crint_free(x); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; val = 0; op_range = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x)) { x = 0; val = 0; op_range = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    dnml_status reserve_stat = crint_reserve(x, op_range);
    DNML_TEST_ASSERT((_lib_crt_neq(reserve_stat, DNML_ALLOC_OOM)), realloc_null, { crint_free(x); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Main Loop and Fake-handling for OOM */
    uint64_t extension_bits; CHOOSE_OPTION((extension_bits), (_lib_crt_isneg(val)), (UINT64_MAX), (0));
    limb_t fake_dst = 123; limb_t* dst; size_t index;
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        uint64_t a, b, mag_val = __CRT_MAG_I64__(val);
        CHOOSE_OPTION((index), (_lib_crt_lt(i, x->n)), (i), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x->limbs[index] : &fake_dst;
        CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0));
        CHOOSE_OPTION((b), (!i), (mag_val), (extension_bits));
        *dst = ~(a ^ b); a = 0; b = 0; mag_val = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_normalized;
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; extension_bits = 0; fake_dst = 0; dst = 0; index = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0;
    fake_normalized.poisoned = 0; x = 0; val = 0; op_range = 0; return ret_stat; // clang-format-on
}
dnml_status crint_mutex_and(crint *x, crint y, size_t op_range) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, { crint_free(&y); });
    DNML_TEST_ASSERT((crint_pvalidate(x) & crint_validate(y)), ci_full_contract, { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((!x->poisoned & !y.poisoned), crint_poisoned, { crint_free(x); crint_free(&y); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { pbv_crint_clear(y); x = 0; op_range = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x) | !crint_validate(y)) { pbv_crint_clear(y); x = 0; op_range = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (y.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    dnml_status reserve_stat = crint_reserve(x, op_range);
    DNML_TEST_ASSERT((_lib_crt_neq(reserve_stat, DNML_ALLOC_OOM)), realloc_null, { crint_free(x); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Main Loop and Fake-handling for OOM */
    limb_t fake_dst = 123, fake_src = 456; limb_t *dst, *src;
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        uint64_t a, b; size_t index;
        CHOOSE_OPTION((index), (_lib_crt_lt(i, x->n)), (i), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x->limbs[index] : &fake_dst;
        src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &y.limbs[index] : &fake_src;
        CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0)); CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src), (0));
        *dst = (a & b); a = 0; b = 0; index = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_normalized;
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.sign = 0; fake_normalized.sign = 0;
    fake_normalized.poisoned = 0; x = 0; pbv_crint_clear(y); op_range = 0; return ret_stat; // clang-format on
}
dnml_status crint_mutex_nand(crint *x, crint y, size_t op_range) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, { crint_free(&y); });
    DNML_TEST_ASSERT((crint_pvalidate(x) & crint_validate(y)), ci_full_contract, { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((!x->poisoned & !y.poisoned), crint_poisoned, { crint_free(x); crint_free(&y); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { pbv_crint_clear(y); x = 0; op_range = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x) | !crint_validate(y)) { pbv_crint_clear(y); x = 0;  op_range = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (y.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    dnml_status reserve_stat = crint_reserve(x, op_range);
    DNML_TEST_ASSERT((_lib_crt_neq(reserve_stat, DNML_ALLOC_OOM)), realloc_null, { crint_free(x); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Main Loop and Fake-handling for OOM */
    limb_t fake_dst = 123, fake_src = 456; limb_t *dst, *src;
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        uint64_t a, b; size_t index;
        CHOOSE_OPTION((index), (_lib_crt_lt(i, x->n)), (i), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x->limbs[index] : &fake_dst;
        src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &y.limbs[index] : &fake_src;
        CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0));
        CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src), (0));
        *dst = ~(a & b); a = 0; b = 0; index = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_normalized;
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.sign = 0; fake_normalized.sign = 0;
    fake_normalized.poisoned = 0; x = 0; pbv_crint_clear(y); op_range = 0; return ret_stat; // clang-format on
}
dnml_status crint_mutex_or(crint *x, crint y, size_t op_range) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, { crint_free(&y); });
    DNML_TEST_ASSERT((crint_pvalidate(x) & crint_validate(y)), ci_full_contract, { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((!x->poisoned & !y.poisoned), crint_poisoned, { crint_free(x); crint_free(&y); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { pbv_crint_clear(y); x = 0; op_range = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x) | !crint_validate(y)) { pbv_crint_clear(y); x = 0; op_range = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (y.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    dnml_status reserve_stat = crint_reserve(x, op_range);
    DNML_TEST_ASSERT((_lib_crt_neq(reserve_stat, DNML_ALLOC_OOM)), realloc_null, { crint_free(x); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Main Loop and Fake-handling for OOM */
    limb_t fake_dst = 123, fake_src = 456; limb_t *dst, *src;
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        uint64_t a, b; size_t index;
        CHOOSE_OPTION((index), (_lib_crt_lt(i, x->n)), (i), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x->limbs[index] : &fake_dst;
        src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &y.limbs[index] : &fake_src;
        CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0)); CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src), (0));
        *dst = (a | b); a = 0; b = 0; index = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_normalized;
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.sign = 0; fake_normalized.sign = 0;
    fake_normalized.poisoned = 0; x = 0; pbv_crint_clear(y); op_range = 0; return ret_stat; // clang-format on
}
dnml_status crint_mutex_nor(crint *x, crint y, size_t op_range) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, { crint_free(&y); });
    DNML_TEST_ASSERT((crint_pvalidate(x) & crint_validate(y)), ci_full_contract, { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((!x->poisoned & !y.poisoned), crint_poisoned, { crint_free(x); crint_free(&y); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { pbv_crint_clear(y); x = 0; op_range = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x) | !crint_validate(y)) { pbv_crint_clear(y); x = 0; op_range = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (y.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    dnml_status reserve_stat = crint_reserve(x, op_range);
    DNML_TEST_ASSERT((_lib_crt_neq(reserve_stat, DNML_ALLOC_OOM)), realloc_null, { crint_free(x); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Main Loop and Fake-handling for OOM */
    limb_t fake_dst = 123, fake_src = 456; limb_t *dst, *src;
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        uint64_t a, b; size_t index;
        CHOOSE_OPTION((index), (_lib_crt_lt(i, x->n)), (i), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x->limbs[index] : &fake_dst;
        src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &y.limbs[index] : &fake_src;
        CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0)); CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src), (0));
        *dst = ~(a | b); a = 0; b = 0; index = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_normalized;
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.sign = 0; fake_normalized.sign = 0;
    fake_normalized.poisoned = 0; x = 0; pbv_crint_clear(y); op_range = 0; return ret_stat; // clang-format on
}
dnml_status crint_mutex_xor(crint *x, crint y, size_t op_range) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, { crint_free(&y); });
    DNML_TEST_ASSERT((crint_pvalidate(x) & crint_validate(y)), ci_full_contract, { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((!x->poisoned & !y.poisoned), crint_poisoned, { crint_free(x); crint_free(&y); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { pbv_crint_clear(y); x = 0; op_range = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x) | !crint_validate(y)) { pbv_crint_clear(y); x = 0; op_range = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (y.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    dnml_status reserve_stat = crint_reserve(x, op_range);
    DNML_TEST_ASSERT((_lib_crt_neq(reserve_stat, DNML_ALLOC_OOM)), realloc_null, { crint_free(x); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Main Loop and Fake-handling for OOM */
    limb_t fake_dst = 123, fake_src = 456; limb_t *dst, *src;
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        uint64_t a, b; size_t index;
        CHOOSE_OPTION((index), (_lib_crt_lt(i, x->n)), (i), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x->limbs[index] : &fake_dst;
        src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &y.limbs[index] : &fake_src;
        CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0)); CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src), (0));
        *dst = (a ^ b); a = 0; b = 0; index = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_normalized;
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.sign = 0; fake_normalized.sign = 0;
    fake_normalized.poisoned = 0; x = 0; pbv_crint_clear(y); op_range = 0; return ret_stat; // clang-format on
}
dnml_status crint_mutex_xnor(crint *x, crint y, size_t op_range) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, { crint_free(&y); });
    DNML_TEST_ASSERT((crint_pvalidate(x) & crint_validate(y)), ci_full_contract, { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((!x->poisoned & !y.poisoned), crint_poisoned, { crint_free(x); crint_free(&y); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { pbv_crint_clear(y); x = 0; op_range = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x) | !crint_validate(y)) { pbv_crint_clear(y); x = 0; op_range = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (y.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    dnml_status reserve_stat = crint_reserve(x, op_range);
    DNML_TEST_ASSERT((_lib_crt_neq(reserve_stat, DNML_ALLOC_OOM)), realloc_null, { crint_free(x); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Main Loop and Fake-handling for OOM */
    limb_t fake_dst = 123, fake_src = 456; limb_t *dst, *src;
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        uint64_t a, b; size_t index;
        CHOOSE_OPTION((index), (_lib_crt_lt(i, x->n)), (i), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x->limbs[index] : &fake_dst;
        src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &y.limbs[index] : &fake_src;
        CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0)); CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src), (0));
        *dst = ~(a ^ b); a = 0; b = 0; index = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x : &fake_normalized;
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.sign = 0; fake_normalized.sign = 0;
    fake_normalized.poisoned = 0; x = 0; pbv_crint_clear(y); op_range = 0; return ret_stat; // clang-format on
}




//* ================================= FUNCTION, FIXED WIDTH BITWISE OPERATION ================================== */
crint crint_andu64(crint x, uint64_t val, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    preop_err((!crint_validate((x))), err, CRINT_ERR_INVAL, { pbv_crint_clear(x); val = 0; err = 0; })
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Allocating a New Crint */
    crint res; dnml_status new_stat = crint_new(&res);
    CHOOSE_OPTION((ret_stat), (
        _lib_crt_eq(new_stat, DNML_ALLOC_OOM) &
        _lib_crt_eq(ret_stat, CRINT_ERR_RANGE)),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Actual Operation */ limb_t fake_buf[1] = {0};
    uint64_t first_limb = x.limbs[0]; CHOOSE_OPTION((first_limb), (x.n), (first_limb), (0));
    res.limbs = (_lib_crt_eq(ret_stat, DNML_ALLOC_OOM)) ? fake_buf : res.limbs;
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    res.limbs[0] = first_limb & val; res.n = !!(first_limb); CHOOSE_OPTION((res.sign), (first_limb), (x.sign), (1));

    /* Masking to Invalidity + Aggressive Post-operation Cleanup */ // clang-format off
    res.limbs[0] &= mask;
    limb_t *chosen_freed = (_lib_crt_eq(ret_stat, CRINT_POISON)) ? res.limbs : NULL;
    free(chosen_freed); // Safe nop when chosen_freed = NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    res.n &= mask; res.sign &= mask; res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));
    if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = ret_stat;
    ret_stat = 0; new_stat = 0; fake_buf[0] = 0; pbv_crint_clear(x);
    val = 0; err = 0; first_limb = 0; mask = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_nandu64(crint x, uint64_t val, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    preop_err((!crint_validate((x))), err, CRINT_ERR_INVAL, { pbv_crint_clear(x); val = 0; err = 0; })
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Allocating a New Crint */
    crint res; dnml_status new_stat = crint_snew(&res, x.n);
    CHOOSE_OPTION((ret_stat), (
        _lib_crt_eq(new_stat, DNML_ALLOC_OOM) &
        _lib_crt_eq(ret_stat, CRINT_ERR_RANGE)),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Actual Operation */ limb_t fake_buf[FAKE_BUF_CAP] = {0};
    uint64_t first_limb = x.limbs[0]; CHOOSE_OPTION((first_limb), (x.n), (first_limb), (0));
    res.limbs = (_lib_crt_eq(ret_stat, DNML_ALLOC_OOM)) ? fake_buf : res.limbs;
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    res.limbs[0] = ~(first_limb & val) & mask; size_t cap;
    CHOOSE_OPTION((cap), (_lib_crt_eq(ret_stat, DNML_ALLOC_OOM)), (FAKE_BUF_CAP), (x.n));
    __libdnml_smemset_u64(res.limbs, UINT8_MAX, cap, 0, cap - 1, (x.poisoned)); res.n = x.n;

    /* Masking to Invalidity + Aggressive Post-operation Cleanup */ // clang-format off
    res.limbs[0] &= mask;
    limb_t *chosen_freed = (_lib_crt_eq(ret_stat, CRINT_POISON)) ? res.limbs : NULL;
    free(chosen_freed); // Safe nop when chosen_freed = NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    res.n &= mask; res.sign &= mask; res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));
    if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = ret_stat; ret_stat = 0;
    __libdnml_smemwipe_u64(fake_buf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    new_stat = 0; fake_buf[0] = 0; first_limb = 0; mask = 0;
    pbv_crint_clear(x); val = 0; err = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_oru64(crint x, uint64_t val, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    preop_err((!crint_validate((x))), err, CRINT_ERR_INVAL, { pbv_crint_clear(x); val = 0; err = 0; })
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Allocating a New Crint */
    crint res; dnml_status new_stat = crint_snew(&res, x.n);
    CHOOSE_OPTION((ret_stat), (
        _lib_crt_eq(new_stat, DNML_ALLOC_OOM) &
        _lib_crt_eq(ret_stat, CRINT_ERR_RANGE)),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Bitwise Loop Operation */
    limb_t fake_dst = 123, fake_src = 456; limb_t *dst, *src;
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    for (size_t i = 0; _lib_crt_lt(i, x.n); ++i) {
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i] : &fake_dst;
        src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x.limbs[i] : &fake_src;
        uint64_t b; CHOOSE_OPTION((b), (!i), (val), (0)); *dst = (*src | b); b = 0;
    }
    /* Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = x.n;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res : &fake_normalized; crint_normalize(norm_crint);

    /* Masking metadata to Invalidity based on status */
    limb_t *chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Safe nop when chosen_freed = NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask)); res.n &= mask; res.cap = x.n & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));
    /* Aggressive Post-operation Cleanup */ // clang-format off
    if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = ret_stat;
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; mask = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0;
    fake_normalized.poisoned = 0; pbv_crint_clear(x); val = 0; err = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_noru64(crint x, uint64_t val, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    preop_err((!crint_validate((x))), err, CRINT_ERR_INVAL, { pbv_crint_clear(x); val = 0; err = 0; })
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Allocating a New Crint */
    crint res; dnml_status new_stat = crint_snew(&res, x.n);
    CHOOSE_OPTION((ret_stat), (
        _lib_crt_eq(new_stat, DNML_ALLOC_OOM) &
        _lib_crt_eq(ret_stat, CRINT_ERR_RANGE)),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Bitwise Loop Operation */
    limb_t fake_dst = 123, fake_src = 456; limb_t *dst, *src;
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    for (size_t i = 0; _lib_crt_lt(i, x.n); ++i) {
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i] : &fake_dst;
        src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x.limbs[i] : &fake_src;
        uint64_t b; CHOOSE_OPTION((b), (!i), (val), (0)); *dst = ~(*src | b); b = 0;
    }
    /* Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = x.n;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res : &fake_normalized; crint_normalize(norm_crint);

    /* Masking metadata to Invalidity based on status */
    limb_t *chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Safe nop when chosen_freed = NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask)); res.n &= mask; res.cap = x.n & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));
    /* Aggressive Post-operation Cleanup */ // clang-format off
    if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = ret_stat;
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; mask = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0;
    fake_normalized.poisoned = 0; pbv_crint_clear(x); val = 0; err = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_xoru64(crint x, uint64_t val, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    preop_err((!crint_validate((x))), err, CRINT_ERR_INVAL, { pbv_crint_clear(x); val = 0; err = 0; })
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Allocating a New Crint */
    crint res; dnml_status new_stat = crint_snew(&res, x.n);
    CHOOSE_OPTION((ret_stat), (
        _lib_crt_eq(new_stat, DNML_ALLOC_OOM) &
        _lib_crt_eq(ret_stat, CRINT_ERR_RANGE)),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Bitwise Loop Operation */
    limb_t fake_dst = 123, fake_src = 456; limb_t *dst, *src;
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    for (size_t i = 0; _lib_crt_lt(i, x.n); ++i) {
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i] : &fake_dst;
        src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x.limbs[i] : &fake_src;
        uint64_t b; CHOOSE_OPTION((b), (!i), (val), (0)); *dst = (*src ^ b); b = 0;
    }
    /* Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = x.n;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res : &fake_normalized; crint_normalize(norm_crint);

    /* Masking metadata to Invalidity based on status */
    limb_t *chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Safe nop when chosen_freed = NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask)); res.n &= mask; res.cap = x.n & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));
    /* Aggressive Post-operation Cleanup */ // clang-format off
    if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = ret_stat;
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; mask = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0;
    fake_normalized.poisoned = 0; pbv_crint_clear(x); val = 0; err = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_xnoru64(crint x, uint64_t val, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    preop_err((!crint_validate((x))), err, CRINT_ERR_INVAL, { pbv_crint_clear(x); val = 0; err = 0; })
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Allocating a New Crint */
    crint res; dnml_status new_stat = crint_snew(&res, x.n);
    CHOOSE_OPTION((ret_stat), (
        _lib_crt_eq(new_stat, DNML_ALLOC_OOM) &
        _lib_crt_eq(ret_stat, CRINT_ERR_RANGE)),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Bitwise Loop Operation */
    limb_t fake_dst = 123, fake_src = 456; limb_t *dst, *src;
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    for (size_t i = 0; _lib_crt_lt(i, x.n); ++i) {
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i] : &fake_dst;
        src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x.limbs[i] : &fake_src;
        uint64_t b; CHOOSE_OPTION((b), (!i), (val), (0)); *dst = ~(*src ^ b); b = 0;
    }
    /* Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = x.n;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res : &fake_normalized; crint_normalize(norm_crint);

    /* Masking metadata to Invalidity based on status */
    limb_t *chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Safe nop when chosen_freed = NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask)); res.n &= mask; res.cap = x.n & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));
    /* Aggressive Post-operation Cleanup */ // clang-format off
    if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = ret_stat;
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; mask = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0;
    fake_normalized.poisoned = 0; pbv_crint_clear(x); val = 0; err = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_and(crint x, crint y, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x) & crint_validate(y)), ci_full_contract, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((!x.poisoned & !y.poisoned), crint_poisoned, { crint_free(&x); crint_free(&y); });
    if (!crint_validate(x) | !crint_validate(y)) {
        if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = CRINT_ERR_INVAL;
        pbv_crint_clear(x); pbv_crint_clear(y); return __CRINT_ERRVAL__();
    } /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (y.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    size_t op_range = crtmax(x.n, y.n); crint res; dnml_status new_stat = crint_snew(&res, op_range);
    DNML_TEST_ASSERT((_lib_crt_neq(new_stat, DNML_ALLOC_OOM)), realloc_null, { crint_free(&x); crint_free(&y); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(new_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Main Loop & Fake-handling for OOM */
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS)));
    limb_t fake_dst = 123, fake_src1 = 456, fake_src2 = 789; limb_t *dst, *src1, *src2;
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i] : &fake_dst;
        src1 = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x.limbs[i] : &fake_src1;
        src2 = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &y.limbs[i] : &fake_src2;
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src1), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src2), (0));
        *dst = ((*src1) & (*src2)); a = 0; b = 0;
    }
    /* Masking to Invalidity */
    limb_t *chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Safe nop when chosen_freed = NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    res.n = op_range & mask; res.cap = op_range & mask; res.sign = (int64_t)((x.sign | y.sign) & mask);
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));

    /* Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_gt(ret_stat, CRINT_SUCCESS)) ? &res : & fake_normalized;
    crint_normalize(norm_crint); /**/ if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err= ret_stat;
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    ret_stat = 0; op_range = 0; new_stat = 0; mask = 0; norm_crint = 0; fake_dst = 0;
    fake_src1 = 0; fake_src2 = 0; dst = 0; src1 = 0; src2 = 0; fake_normalized.limbs = 0;
    fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0; fake_normalized.poisoned = 0;
    pbv_crint_clear(x); pbv_crint_clear(y); err = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_nand(crint x, crint y, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x) & crint_validate(y)), ci_full_contract, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((!x.poisoned & !y.poisoned), crint_poisoned, { crint_free(&x); crint_free(&y); });
    if (!crint_validate(x) | !crint_validate(y)) {
        if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = CRINT_ERR_INVAL;
        pbv_crint_clear(x); pbv_crint_clear(y); return __CRINT_ERRVAL__();
    } /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (y.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    size_t op_range = crtmax(x.n, y.n); crint res; dnml_status new_stat = crint_snew(&res, op_range);
    DNML_TEST_ASSERT((_lib_crt_neq(new_stat, DNML_ALLOC_OOM)), realloc_null, { crint_free(&x); crint_free(&y); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(new_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Main Loop & Fake-handling for OOM */
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS)));
    limb_t fake_dst = 123, fake_src1 = 456, fake_src2 = 789; limb_t *dst, *src1, *src2;
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i] : &fake_dst;
        src1 = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x.limbs[i] : &fake_src1;
        src2 = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &y.limbs[i] : &fake_src2;
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src1), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src2), (0));
        *dst = ~((*src1) & (*src2)); a = 0; b = 0;
    }
    /* Masking to Invalidity */
    limb_t *chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Safe nop when chosen_freed = NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    res.n = op_range & mask; res.cap = op_range & mask; res.sign = (int64_t)((x.sign | y.sign) & mask);
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));

    /* Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_gt(ret_stat, CRINT_SUCCESS)) ? &res : & fake_normalized;
    crint_normalize(norm_crint); /**/ if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err= ret_stat;
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    ret_stat = 0; op_range = 0; new_stat = 0; mask = 0; norm_crint = 0; fake_dst = 0;
    fake_src1 = 0; fake_src2 = 0; dst = 0; src1 = 0; src2 = 0; fake_normalized.limbs = 0;
    fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0; fake_normalized.poisoned = 0;
    pbv_crint_clear(x); pbv_crint_clear(y); err = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_or(crint x, crint y, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x) & crint_validate(y)), ci_full_contract, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((!x.poisoned & !y.poisoned), crint_poisoned, { crint_free(&x); crint_free(&y); });
    if (!crint_validate(x) | !crint_validate(y)) {
        if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = CRINT_ERR_INVAL;
        pbv_crint_clear(x); pbv_crint_clear(y); return __CRINT_ERRVAL__();
    } /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (y.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    size_t op_range = crtmax(x.n, y.n); crint res; dnml_status new_stat = crint_snew(&res, op_range);
    DNML_TEST_ASSERT((_lib_crt_neq(new_stat, DNML_ALLOC_OOM)), realloc_null, { crint_free(&x); crint_free(&y); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(new_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Main Loop & Fake-handling for OOM */
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS)));
    limb_t fake_dst = 123, fake_src1 = 456, fake_src2 = 789; limb_t *dst, *src1, *src2;
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i] : &fake_dst;
        src1 = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x.limbs[i] : &fake_src1;
        src2 = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &y.limbs[i] : &fake_src2;
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src1), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src2), (0));
        *dst = ((*src1) | (*src2)); a = 0; b = 0;
    }
    /* Masking to Invalidity */
    limb_t *chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Safe nop when chosen_freed = NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    res.n = op_range & mask; res.cap = op_range & mask; res.sign = (int64_t)((x.sign | y.sign) & mask);
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));

    /* Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_gt(ret_stat, CRINT_SUCCESS)) ? &res : & fake_normalized;
    crint_normalize(norm_crint); /**/ if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err= ret_stat;
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    ret_stat = 0; op_range = 0; new_stat = 0; mask = 0; norm_crint = 0; fake_dst = 0;
    fake_src1 = 0; fake_src2 = 0; dst = 0; src1 = 0; src2 = 0; fake_normalized.limbs = 0;
    fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0; fake_normalized.poisoned = 0;
    pbv_crint_clear(x); pbv_crint_clear(y); err = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_nor(crint x, crint y, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x) & crint_validate(y)), ci_full_contract, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((!x.poisoned & !y.poisoned), crint_poisoned, { crint_free(&x); crint_free(&y); });
    if (!crint_validate(x) | !crint_validate(y)) {
        if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = CRINT_ERR_INVAL;
        pbv_crint_clear(x); pbv_crint_clear(y); return __CRINT_ERRVAL__();
    } /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (y.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    size_t op_range = crtmax(x.n, y.n); crint res; dnml_status new_stat = crint_snew(&res, op_range);
    DNML_TEST_ASSERT((_lib_crt_neq(new_stat, DNML_ALLOC_OOM)), realloc_null, { crint_free(&x); crint_free(&y); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(new_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Main Loop & Fake-handling for OOM */
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS)));
    limb_t fake_dst = 123, fake_src1 = 456, fake_src2 = 789; limb_t *dst, *src1, *src2;
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i] : &fake_dst;
        src1 = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x.limbs[i] : &fake_src1;
        src2 = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &y.limbs[i] : &fake_src2;
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src1), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src2), (0));
        *dst = ~((*src1) | (*src2)); a = 0; b = 0;
    }
    /* Masking to Invalidity */
    limb_t *chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Safe nop when chosen_freed = NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    res.n = op_range & mask; res.cap = op_range & mask; res.sign = (int64_t)((x.sign | y.sign) & mask);
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));

    /* Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_gt(ret_stat, CRINT_SUCCESS)) ? &res : & fake_normalized;
    crint_normalize(norm_crint); /**/ if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err= ret_stat;
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    ret_stat = 0; op_range = 0; new_stat = 0; mask = 0; norm_crint = 0; fake_dst = 0;
    fake_src1 = 0; fake_src2 = 0; dst = 0; src1 = 0; src2 = 0; fake_normalized.limbs = 0;
    fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0; fake_normalized.poisoned = 0;
    pbv_crint_clear(x); pbv_crint_clear(y); err = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_xor(crint x, crint y, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x) & crint_validate(y)), ci_full_contract, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((!x.poisoned & !y.poisoned), crint_poisoned, { crint_free(&x); crint_free(&y); });
    if (!crint_validate(x) | !crint_validate(y)) {
        if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = CRINT_ERR_INVAL;
        pbv_crint_clear(x); pbv_crint_clear(y); return __CRINT_ERRVAL__();
    } /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (y.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    size_t op_range = crtmax(x.n, y.n); crint res; dnml_status new_stat = crint_snew(&res, op_range);
    DNML_TEST_ASSERT((_lib_crt_neq(new_stat, DNML_ALLOC_OOM)), realloc_null, { crint_free(&x); crint_free(&y); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(new_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Main Loop & Fake-handling for OOM */
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS)));
    limb_t fake_dst = 123, fake_src1 = 456, fake_src2 = 789; limb_t *dst, *src1, *src2;
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i] : &fake_dst;
        src1 = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x.limbs[i] : &fake_src1;
        src2 = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &y.limbs[i] : &fake_src2;
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src1), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src2), (0));
        *dst = ((*src1) ^ (*src2)); a = 0; b = 0;
    }
    /* Masking to Invalidity */
    limb_t *chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Safe nop when chosen_freed = NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    res.n = op_range & mask; res.cap = op_range & mask; res.sign = (int64_t)((x.sign | y.sign) & mask);
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));

    /* Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_gt(ret_stat, CRINT_SUCCESS)) ? &res : & fake_normalized;
    crint_normalize(norm_crint); /**/ if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err= ret_stat;
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    ret_stat = 0; op_range = 0; new_stat = 0; mask = 0; norm_crint = 0; fake_dst = 0;
    fake_src1 = 0; fake_src2 = 0; dst = 0; src1 = 0; src2 = 0; fake_normalized.limbs = 0;
    fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0; fake_normalized.poisoned = 0;
    pbv_crint_clear(x); pbv_crint_clear(y); err = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_xnor(crint x, crint y, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x) & crint_validate(y)), ci_full_contract, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((!x.poisoned & !y.poisoned), crint_poisoned, { crint_free(&x); crint_free(&y); });
    if (!crint_validate(x) | !crint_validate(y)) {
        if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = CRINT_ERR_INVAL;
        pbv_crint_clear(x); pbv_crint_clear(y); return __CRINT_ERRVAL__();
    } /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (y.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    size_t op_range = crtmax(x.n, y.n); crint res; dnml_status new_stat = crint_snew(&res, op_range);
    DNML_TEST_ASSERT((_lib_crt_neq(new_stat, DNML_ALLOC_OOM)), realloc_null, { crint_free(&x); crint_free(&y); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(new_stat, DNML_ALLOC_OOM)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Main Loop & Fake-handling for OOM */
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS)));
    limb_t fake_dst = 123, fake_src1 = 456, fake_src2 = 789; limb_t *dst, *src1, *src2;
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i] : &fake_dst;
        src1 = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x.limbs[i] : &fake_src1;
        src2 = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &y.limbs[i] : &fake_src2;
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src1), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src2), (0));
        *dst = ~((*src1) ^ (*src2)); a = 0; b = 0;
    }
    /* Masking to Invalidity */
    limb_t *chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Safe nop when chosen_freed = NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    res.n = op_range & mask; res.cap = op_range & mask; res.sign = (int64_t)((x.sign | y.sign) & mask);
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));

    /* Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_gt(ret_stat, CRINT_SUCCESS)) ? &res : & fake_normalized;
    crint_normalize(norm_crint); /**/ if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err= ret_stat;
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    ret_stat = 0; op_range = 0; new_stat = 0; mask = 0; norm_crint = 0; fake_dst = 0;
    fake_src1 = 0; fake_src2 = 0; dst = 0; src1 = 0; src2 = 0; fake_normalized.limbs = 0;
    fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0; fake_normalized.poisoned = 0;
    pbv_crint_clear(x); pbv_crint_clear(y); err = 0; chosen_freed = 0; return res; // clang-format on
}




//* ================================= FUNCTION, EXPLICIT WIDTH BITWISE OPERATION ================================== */
crint crint_ex_andu64(crint x, uint64_t val, size_t op_range, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    if (!crint_validate(x)) {
        if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = CRINT_ERR_INVAL;
        pbv_crint_clear(x); val = 0; op_range = 0; return __CRINT_ERRVAL__();
    } /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    uint64_t first_limb = x.limbs[0]; CHOOSE_OPTION((first_limb), (x.n), (first_limb), (0)); first_limb = first_limb & val;
    /* Allocation of Result */
    crint res; dnml_status new_stat = crint_snew(&res, op_range);
    CHOOSE_OPTION((ret_stat), (
        _lib_crt_eq(new_stat, DNML_ALLOC_OOM) &
        _lib_crt_eq(ret_stat, CRINT_SUCCESS)),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS))); limb_t tmp_limb[1] = {0};
    /* Actual Bitwise Operation */ int8_t ret_sign;
    res.limbs = (_lib_crt_eq(new_stat, DNML_ALLOC_OOM)) ? tmp_limb : res.limbs;
    CHOOSE_OPTION((res.limbs[0]), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), (first_limb), (0));
    CHOOSE_OPTION((res.n), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), (!!(first_limb)), (0));
    CHOOSE_OPTION((ret_sign), (first_limb), (x.sign), (1));
    CHOOSE_OPTION((res.sign), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), (ret_sign), (0));
    /* Post-operation Cleanup */ // clang-format off
    limb_t *chosen_freed = (_lib_crt_eq(ret_stat, CRINT_POISON)) ? res.limbs : NULL;
    free(chosen_freed); // Safe nop when chosen_freed = NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = ret_stat;
    res.cap = op_range & mask; res.poisoned = (x.poisoned); ret_stat = 0;
    first_limb = 0; new_stat = 0; mask = 0; tmp_limb[0] = 0; pbv_crint_clear(x);
    val = 0; op_range = 0; err = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_ex_nandu64(crint x, uint64_t val, size_t op_range, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    if (!crint_validate(x)) {
        if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = CRINT_ERR_INVAL;
        pbv_crint_clear(x); val = 0; op_range = 0; return __CRINT_ERRVAL__();
    } /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Allocation of Result crint */
    crint res; dnml_status new_stat = crint_snew(&res, op_range);
    CHOOSE_OPTION((ret_stat), (
        _lib_crt_eq(new_stat, DNML_ALLOC_OOM) &
        _lib_crt_eq(ret_stat, CRINT_SUCCESS)),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Bitwise Loop Operation */
    limb_t fake_dst = 123, fake_src = 456; limb_t *dst, *src;
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        size_t sindex; CHOOSE_OPTION((sindex), (_lib_crt_lt(i, x.n)), (i), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i] : &fake_dst;
        src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x.limbs[sindex] : &fake_src;
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src), (0));
        uint64_t b; CHOOSE_OPTION((b), (!i), (val), (0));
        *dst = ~(a & b); sindex = 0; a = 0; b = 0;
    }
    /* Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res : &fake_normalized;
    crint_normalize(norm_crint);

    /* Post-operation Cleanup */ // clang-format off
    limb_t* chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Safe nop when chosen_freed = NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask));
    res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));
    /* Aggeressive Post-operation Cleanup */
    if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = ret_stat;
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; norm_crint = 0; fake_normalized.limbs = 0;
    fake_normalized.cap = 0; fake_normalized.n = 0; fake_normalized.poisoned = 0; fake_normalized.sign = 0;
    pbv_crint_clear(x); val = 0; op_range = 0; err = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_ex_oru64(crint x, uint64_t val, size_t op_range, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    if (!crint_validate(x)) {
        if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = CRINT_ERR_INVAL;
        pbv_crint_clear(x); val = 0; op_range = 0; return __CRINT_ERRVAL__();
    } /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Allocation of Result crint */
    crint res; dnml_status new_stat = crint_snew(&res, op_range);
    CHOOSE_OPTION((ret_stat), (
        _lib_crt_eq(new_stat, DNML_ALLOC_OOM) &
        _lib_crt_eq(ret_stat, CRINT_SUCCESS)),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Bitwise Loop Operation */
    limb_t fake_dst = 123, fake_src = 456; limb_t *dst, *src;
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        size_t sindex; CHOOSE_OPTION((sindex), (_lib_crt_lt(i, x.n)), (i), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i] : &fake_dst;
        src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x.limbs[sindex] : &fake_src;
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src), (0));
        uint64_t b; CHOOSE_OPTION((b), (!i), (val), (0));
        *dst = (a | b); sindex = 0; a = 0; b = 0;
    }
    /* Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res : &fake_normalized;
    crint_normalize(norm_crint);

    /* Post-operation Cleanup */ // clang-format off
    limb_t* chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Safe nop when chosen_freed = NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask));
    res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));
    /* Aggeressive Post-operation Cleanup */
    if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = ret_stat;
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; norm_crint = 0; fake_normalized.limbs = 0;
    fake_normalized.cap = 0; fake_normalized.n = 0; fake_normalized.poisoned = 0; fake_normalized.sign = 0;
    pbv_crint_clear(x); val = 0; op_range = 0; err = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_ex_noru64(crint x, uint64_t val, size_t op_range, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    if (!crint_validate(x)) {
        if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = CRINT_ERR_INVAL;
        pbv_crint_clear(x); val = 0; op_range = 0; return __CRINT_ERRVAL__();
    } /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Allocation of Result crint */
    crint res; dnml_status new_stat = crint_snew(&res, op_range);
    CHOOSE_OPTION((ret_stat), (
        _lib_crt_eq(new_stat, DNML_ALLOC_OOM) &
        _lib_crt_eq(ret_stat, CRINT_SUCCESS)),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Bitwise Loop Operation */
    limb_t fake_dst = 123, fake_src = 456; limb_t *dst, *src;
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        size_t sindex; CHOOSE_OPTION((sindex), (_lib_crt_lt(i, x.n)), (i), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i] : &fake_dst;
        src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x.limbs[sindex] : &fake_src;
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src), (0));
        uint64_t b; CHOOSE_OPTION((b), (!i), (val), (0));
        *dst = ~(a | b); sindex = 0; a = 0; b = 0;
    }
    /* Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res : &fake_normalized;
    crint_normalize(norm_crint);

    /* Post-operation Cleanup */ // clang-format off
    limb_t* chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Safe nop when chosen_freed = NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask));
    res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));
    /* Aggeressive Post-operation Cleanup */
    if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = ret_stat;
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; norm_crint = 0; fake_normalized.limbs = 0;
    fake_normalized.cap = 0; fake_normalized.n = 0; fake_normalized.poisoned = 0; fake_normalized.sign = 0;
    pbv_crint_clear(x); val = 0; op_range = 0; err = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_ex_xoru64(crint x, uint64_t val, size_t op_range, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    if (!crint_validate(x)) {
        if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = CRINT_ERR_INVAL;
        pbv_crint_clear(x); val = 0; op_range = 0; return __CRINT_ERRVAL__();
    } /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Allocation of Result crint */
    crint res; dnml_status new_stat = crint_snew(&res, op_range);
    CHOOSE_OPTION((ret_stat), (
        _lib_crt_eq(new_stat, DNML_ALLOC_OOM) &
        _lib_crt_eq(ret_stat, CRINT_SUCCESS)),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Bitwise Loop Operation */
    limb_t fake_dst = 123, fake_src = 456; limb_t *dst, *src;
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        size_t sindex; CHOOSE_OPTION((sindex), (_lib_crt_lt(i, x.n)), (i), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i] : &fake_dst;
        src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x.limbs[sindex] : &fake_src;
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src), (0));
        uint64_t b; CHOOSE_OPTION((b), (!i), (val), (0));
        *dst = (a ^ b); sindex = 0; a = 0; b = 0;
    }
    /* Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res : &fake_normalized;
    crint_normalize(norm_crint);

    /* Post-operation Cleanup */ // clang-format off
    limb_t* chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Safe nop when chosen_freed = NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask));
    res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));
    /* Aggeressive Post-operation Cleanup */
    if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = ret_stat;
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; norm_crint = 0; fake_normalized.limbs = 0;
    fake_normalized.cap = 0; fake_normalized.n = 0; fake_normalized.poisoned = 0; fake_normalized.sign = 0;
    pbv_crint_clear(x); val = 0; op_range = 0; err = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_ex_xnoru64(crint x, uint64_t val, size_t op_range, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    if (!crint_validate(x)) {
        if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = CRINT_ERR_INVAL;
        pbv_crint_clear(x); val = 0; op_range = 0; return __CRINT_ERRVAL__();
    } /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Allocation of Result crint */
    crint res; dnml_status new_stat = crint_snew(&res, op_range);
    CHOOSE_OPTION((ret_stat), (
        _lib_crt_eq(new_stat, DNML_ALLOC_OOM) &
        _lib_crt_eq(ret_stat, CRINT_SUCCESS)),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Bitwise Loop Operation */
    limb_t fake_dst = 123, fake_src = 456; limb_t *dst, *src;
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        size_t sindex; CHOOSE_OPTION((sindex), (_lib_crt_lt(i, x.n)), (i), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i] : &fake_dst;
        src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x.limbs[sindex] : &fake_src;
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src), (0));
        uint64_t b; CHOOSE_OPTION((b), (!i), (val), (0));
        *dst = ~(a ^ b); sindex = 0; a = 0; b = 0;
    }
    /* Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res : &fake_normalized;
    crint_normalize(norm_crint);

    /* Post-operation Cleanup */ // clang-format off
    limb_t* chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Safe nop when chosen_freed = NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask));
    res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));
    /* Aggeressive Post-operation Cleanup */
    if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = ret_stat;
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; norm_crint = 0; fake_normalized.limbs = 0;
    fake_normalized.cap = 0; fake_normalized.n = 0; fake_normalized.poisoned = 0; fake_normalized.sign = 0;
    pbv_crint_clear(x); val = 0; op_range = 0; err = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_ex_andi64(crint x, int64_t val, size_t op_range, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    if (!crint_validate(x)) {
        if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = CRINT_ERR_INVAL;
        pbv_crint_clear(x); val = 0; op_range = 0; return __CRINT_ERRVAL__();
    } /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Allocation of Result crint */
    crint res; dnml_status new_stat = crint_snew(&res, op_range);
    CHOOSE_OPTION((ret_stat), (
        _lib_crt_eq(new_stat, DNML_ALLOC_OOM) &
        _lib_crt_eq(ret_stat, CRINT_SUCCESS)),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Bitwise Loop Operation */
    limb_t fake_dst = 123, fake_src = 456; limb_t *dst, *src;
    uint64_t extension_bits, mag_val = __CRT_MAG_I64__(val);
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    CHOOSE_OPTION((extension_bits), (_lib_crt_isneg(val)), (UINT64_MAX), (0));
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        size_t sindex; CHOOSE_OPTION((sindex), (_lib_crt_lt(i, x.n)), (i), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i] : &fake_dst;
        src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x.limbs[sindex] : &fake_src;
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src), (0));
        uint64_t b; CHOOSE_OPTION((b), (!i), (mag_val), (extension_bits));
        *dst = (a & b); sindex = 0; a = 0; b = 0;
    }
    /* Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res : &fake_normalized;
    crint_normalize(norm_crint);

    /* Post-operation Cleanup */ // clang-format off
    limb_t* chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Safe nop when chosen_freed = NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask));
    res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));
    /* Aggeressive Post-operation Cleanup */
    if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = ret_stat;
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; mag_val = 0;
    extension_bits = 0; norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.cap = 0;
    fake_normalized.n = 0; fake_normalized.poisoned = 0; fake_normalized.sign = 0;
    pbv_crint_clear(x); val = 0; op_range = 0; err = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_ex_nandi64(crint x, int64_t val, size_t op_range, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    if (!crint_validate(x)) {
        if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = CRINT_ERR_INVAL;
        pbv_crint_clear(x); val = 0; op_range = 0; return __CRINT_ERRVAL__();
    } /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Allocation of Result crint */
    crint res; dnml_status new_stat = crint_snew(&res, op_range);
    CHOOSE_OPTION((ret_stat), (
        _lib_crt_eq(new_stat, DNML_ALLOC_OOM) &
        _lib_crt_eq(ret_stat, CRINT_SUCCESS)),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Bitwise Loop Operation */
    limb_t fake_dst = 123, fake_src = 456; limb_t *dst, *src;
    uint64_t extension_bits, mag_val = __CRT_MAG_I64__(val);
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    CHOOSE_OPTION((extension_bits), (_lib_crt_isneg(val)), (UINT64_MAX), (0));
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        size_t sindex; CHOOSE_OPTION((sindex), (_lib_crt_lt(i, x.n)), (i), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i] : &fake_dst;
        src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x.limbs[sindex] : &fake_src;
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src), (0));
        uint64_t b; CHOOSE_OPTION((b), (!i), (mag_val), (extension_bits));
        *dst = ~(a & b); sindex = 0; a = 0; b = 0;
    }
    /* Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res : &fake_normalized;
    crint_normalize(norm_crint);

    /* Post-operation Cleanup */ // clang-format off
    limb_t* chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Safe nop when chosen_freed = NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask));
    res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));
    /* Aggeressive Post-operation Cleanup */
    if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = ret_stat;
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; mag_val = 0;
    extension_bits = 0; norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.cap = 0;
    fake_normalized.n = 0; fake_normalized.poisoned = 0; fake_normalized.sign = 0;
    pbv_crint_clear(x); val = 0; op_range = 0; err = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_ex_ori64(crint x, int64_t val, size_t op_range, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    if (!crint_validate(x)) {
        if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = CRINT_ERR_INVAL;
        pbv_crint_clear(x); val = 0; op_range = 0; return __CRINT_ERRVAL__();
    } /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Allocation of Result crint */
    crint res; dnml_status new_stat = crint_snew(&res, op_range);
    CHOOSE_OPTION((ret_stat), (
        _lib_crt_eq(new_stat, DNML_ALLOC_OOM) &
        _lib_crt_eq(ret_stat, CRINT_SUCCESS)),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Bitwise Loop Operation */
    limb_t fake_dst = 123, fake_src = 456; limb_t *dst, *src;
    uint64_t extension_bits, mag_val = __CRT_MAG_I64__(val);
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    CHOOSE_OPTION((extension_bits), (_lib_crt_isneg(val)), (UINT64_MAX), (0));
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        size_t sindex; CHOOSE_OPTION((sindex), (_lib_crt_lt(i, x.n)), (i), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i] : &fake_dst;
        src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x.limbs[sindex] : &fake_src;
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src), (0));
        uint64_t b; CHOOSE_OPTION((b), (!i), (mag_val), (extension_bits));
        *dst = (a | b); sindex = 0; a = 0; b = 0;
    }
    /* Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res : &fake_normalized;
    crint_normalize(norm_crint);

    /* Post-operation Cleanup */ // clang-format off
    limb_t* chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Safe nop when chosen_freed = NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask));
    res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));
    /* Aggeressive Post-operation Cleanup */
    if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = ret_stat;
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; mag_val = 0;
    extension_bits = 0; norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.cap = 0;
    fake_normalized.n = 0; fake_normalized.poisoned = 0; fake_normalized.sign = 0;
    pbv_crint_clear(x); val = 0; op_range = 0; err = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_ex_nori64(crint x, int64_t val, size_t op_range, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    if (!crint_validate(x)) {
        if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = CRINT_ERR_INVAL;
        pbv_crint_clear(x); val = 0; op_range = 0; return __CRINT_ERRVAL__();
    } /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Allocation of Result crint */
    crint res; dnml_status new_stat = crint_snew(&res, op_range);
    CHOOSE_OPTION((ret_stat), (
        _lib_crt_eq(new_stat, DNML_ALLOC_OOM) &
        _lib_crt_eq(ret_stat, CRINT_SUCCESS)),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Bitwise Loop Operation */
    limb_t fake_dst = 123, fake_src = 456; limb_t *dst, *src;
    uint64_t extension_bits, mag_val = __CRT_MAG_I64__(val);
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    CHOOSE_OPTION((extension_bits), (_lib_crt_isneg(val)), (UINT64_MAX), (0));
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        size_t sindex; CHOOSE_OPTION((sindex), (_lib_crt_lt(i, x.n)), (i), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i] : &fake_dst;
        src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x.limbs[sindex] : &fake_src;
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src), (0));
        uint64_t b; CHOOSE_OPTION((b), (!i), (mag_val), (extension_bits));
        *dst = ~(a | b); sindex = 0; a = 0; b = 0;
    }
    /* Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res : &fake_normalized;
    crint_normalize(norm_crint);

    /* Post-operation Cleanup */ // clang-format off
    limb_t* chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Safe nop when chosen_freed = NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask));
    res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));
    /* Aggeressive Post-operation Cleanup */
    if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = ret_stat;
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; mag_val = 0;
    extension_bits = 0; norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.cap = 0;
    fake_normalized.n = 0; fake_normalized.poisoned = 0; fake_normalized.sign = 0;
    pbv_crint_clear(x); val = 0; op_range = 0; err = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_ex_xori64(crint x, int64_t val, size_t op_range, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    if (!crint_validate(x)) {
        if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = CRINT_ERR_INVAL;
        pbv_crint_clear(x); val = 0; op_range = 0; return __CRINT_ERRVAL__();
    } /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Allocation of Result crint */
    crint res; dnml_status new_stat = crint_snew(&res, op_range);
    CHOOSE_OPTION((ret_stat), (
        _lib_crt_eq(new_stat, DNML_ALLOC_OOM) &
        _lib_crt_eq(ret_stat, CRINT_SUCCESS)),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Bitwise Loop Operation */
    limb_t fake_dst = 123, fake_src = 456; limb_t *dst, *src;
    uint64_t extension_bits, mag_val = __CRT_MAG_I64__(val);
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    CHOOSE_OPTION((extension_bits), (_lib_crt_isneg(val)), (UINT64_MAX), (0));
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        size_t sindex; CHOOSE_OPTION((sindex), (_lib_crt_lt(i, x.n)), (i), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i] : &fake_dst;
        src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x.limbs[sindex] : &fake_src;
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src), (0));
        uint64_t b; CHOOSE_OPTION((b), (!i), (mag_val), (extension_bits));
        *dst = (a ^ b); sindex = 0; a = 0; b = 0;
    }
    /* Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res : &fake_normalized;
    crint_normalize(norm_crint);

    /* Post-operation Cleanup */ // clang-format off
    limb_t* chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Safe nop when chosen_freed = NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask));
    res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));
    /* Aggeressive Post-operation Cleanup */
    if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = ret_stat;
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; mag_val = 0;
    extension_bits = 0; norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.cap = 0;
    fake_normalized.n = 0; fake_normalized.poisoned = 0; fake_normalized.sign = 0;
    pbv_crint_clear(x); val = 0; op_range = 0; err = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_ex_xnori64(crint x, int64_t val, size_t op_range, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    if (!crint_validate(x)) {
        if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = CRINT_ERR_INVAL;
        pbv_crint_clear(x); val = 0; op_range = 0; return __CRINT_ERRVAL__();
    } /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Allocation of Result crint */
    crint res; dnml_status new_stat = crint_snew(&res, op_range);
    CHOOSE_OPTION((ret_stat), (
        _lib_crt_eq(new_stat, DNML_ALLOC_OOM) &
        _lib_crt_eq(ret_stat, CRINT_SUCCESS)),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Bitwise Loop Operation */
    limb_t fake_dst = 123, fake_src = 456; limb_t *dst, *src;
    uint64_t extension_bits, mag_val = __CRT_MAG_I64__(val);
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    CHOOSE_OPTION((extension_bits), (_lib_crt_isneg(val)), (UINT64_MAX), (0));
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        size_t sindex; CHOOSE_OPTION((sindex), (_lib_crt_lt(i, x.n)), (i), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i] : &fake_dst;
        src = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x.limbs[sindex] : &fake_src;
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src), (0));
        uint64_t b; CHOOSE_OPTION((b), (!i), (mag_val), (extension_bits));
        *dst = ~(a ^ b); sindex = 0; a = 0; b = 0;
    }
    /* Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res : &fake_normalized;
    crint_normalize(norm_crint);

    /* Post-operation Cleanup */ // clang-format off
    limb_t* chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Safe nop when chosen_freed = NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask));
    res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));
    /* Aggeressive Post-operation Cleanup */
    if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = ret_stat;
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; mag_val = 0;
    extension_bits = 0; norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.cap = 0;
    fake_normalized.n = 0; fake_normalized.poisoned = 0; fake_normalized.sign = 0;
    pbv_crint_clear(x); val = 0; op_range = 0; err = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_ex_and(crint x, crint y, size_t op_range, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x) & crint_validate(y)), ci_full_contract, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((!x.poisoned & !y.poisoned), crint_poisoned, { crint_free(&x); crint_free(&y); });
    if (!crint_validate(x) | !crint_validate(y)) {
        if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = CRINT_ERR_INVAL;
        pbv_crint_clear(x); pbv_crint_clear(y); op_range = 0; return __CRINT_ERRVAL__();
    } /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), ((x.poisoned | y.poisoned) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Allocation of Result crint */
    crint res; dnml_status new_stat = crint_snew(&res, op_range);
    CHOOSE_OPTION((ret_stat), (
        _lib_crt_eq(new_stat, DNML_ALLOC_OOM) &
        _lib_crt_eq(ret_stat, CRINT_SUCCESS)),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Bitwise Loop Operation */
    limb_t fake_dst = 123, fake_src1 = 456, fake_src2 = 789; limb_t *dst, *src1, *src2;
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        size_t sindex1; CHOOSE_OPTION((sindex1), (_lib_crt_lt(i, x.n)), (i), (0));
        size_t sindex2; CHOOSE_OPTION((sindex2), (_lib_crt_lt(i, y.n)), (i), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i] : &fake_dst;
        src1 = _lib_crt_eq(ret_stat, CRINT_SUCCESS) ? &x.limbs[sindex1] : &fake_src1;
        src2 = _lib_crt_eq(ret_stat, CRINT_SUCCESS) ? &y.limbs[sindex2] : &fake_src2;
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src1), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src2), (0));
        *dst = (a & b); sindex1 = 0; sindex2 = 0; a = 0; b = 0;
    }
    /* Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res : &fake_normalized;
    crint_normalize(norm_crint);

    /* Masking Metadata to Invalidity on FAILURE */ // clang-format off
    limb_t* chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Safe No-op on chosen_freed = NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask));
    res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));
    /* Aggeressive Post-operation Cleanup */
    if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = ret_stat;
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src1 = 0; fake_src2 = 0; dst = 0;
    src1 = 0; src2 = 0; norm_crint = 0;fake_normalized.limbs = 0; fake_normalized.cap = 0;
    fake_normalized.n = 0; fake_normalized.poisoned = 0; fake_normalized.sign = 0;
    pbv_crint_clear(x); pbv_crint_clear(y); op_range = 0; err = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_ex_nand(crint x, crint y, size_t op_range, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x) & crint_validate(y)), ci_full_contract, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((!x.poisoned & !y.poisoned), crint_poisoned, { crint_free(&x); crint_free(&y); });
    if (!crint_validate(x) | !crint_validate(y)) {
        if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = CRINT_ERR_INVAL;
        pbv_crint_clear(x); pbv_crint_clear(y); op_range = 0; return __CRINT_ERRVAL__();
    } /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), ((x.poisoned | y.poisoned) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Allocation of Result crint */
    crint res; dnml_status new_stat = crint_snew(&res, op_range);
    CHOOSE_OPTION((ret_stat), (
        _lib_crt_eq(new_stat, DNML_ALLOC_OOM) &
        _lib_crt_eq(ret_stat, CRINT_SUCCESS)),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Bitwise Loop Operation */
    limb_t fake_dst = 123, fake_src1 = 456, fake_src2 = 789; limb_t *dst, *src1, *src2;
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        size_t sindex1; CHOOSE_OPTION((sindex1), (_lib_crt_lt(i, x.n)), (i), (0));
        size_t sindex2; CHOOSE_OPTION((sindex2), (_lib_crt_lt(i, y.n)), (i), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i] : &fake_dst;
        src1 = _lib_crt_eq(ret_stat, CRINT_SUCCESS) ? &x.limbs[sindex1] : &fake_src1;
        src2 = _lib_crt_eq(ret_stat, CRINT_SUCCESS) ? &y.limbs[sindex2] : &fake_src2;
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src1), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src2), (0));
        *dst = ~(a & b); sindex1 = 0; sindex2 = 0; a = 0; b = 0;
    }
    /* Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res : &fake_normalized;
    crint_normalize(norm_crint);

    /* Masking Metadata to Invalidity on FAILURE */ // clang-format off
    limb_t* chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Safe No-op on chosen_freed = NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask));
    res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));
    /* Aggeressive Post-operation Cleanup */
    if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = ret_stat;
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src1 = 0; fake_src2 = 0; dst = 0;
    src1 = 0; src2 = 0; norm_crint = 0;fake_normalized.limbs = 0; fake_normalized.cap = 0;
    fake_normalized.n = 0; fake_normalized.poisoned = 0; fake_normalized.sign = 0;
    pbv_crint_clear(x); pbv_crint_clear(y); op_range = 0; err = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_ex_or(crint x, crint y, size_t op_range, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x) & crint_validate(y)), ci_full_contract, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((!x.poisoned & !y.poisoned), crint_poisoned, { crint_free(&x); crint_free(&y); });
    if (!crint_validate(x) | !crint_validate(y)) {
        if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = CRINT_ERR_INVAL;
        pbv_crint_clear(x); pbv_crint_clear(y); op_range = 0; return __CRINT_ERRVAL__();
    } /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), ((x.poisoned | y.poisoned) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Allocation of Result crint */
    crint res; dnml_status new_stat = crint_snew(&res, op_range);
    CHOOSE_OPTION((ret_stat), (
        _lib_crt_eq(new_stat, DNML_ALLOC_OOM) &
        _lib_crt_eq(ret_stat, CRINT_SUCCESS)),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Bitwise Loop Operation */
    limb_t fake_dst = 123, fake_src1 = 456, fake_src2 = 789; limb_t *dst, *src1, *src2;
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        size_t sindex1; CHOOSE_OPTION((sindex1), (_lib_crt_lt(i, x.n)), (i), (0));
        size_t sindex2; CHOOSE_OPTION((sindex2), (_lib_crt_lt(i, y.n)), (i), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i] : &fake_dst;
        src1 = _lib_crt_eq(ret_stat, CRINT_SUCCESS) ? &x.limbs[sindex1] : &fake_src1;
        src2 = _lib_crt_eq(ret_stat, CRINT_SUCCESS) ? &y.limbs[sindex2] : &fake_src2;
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src1), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src2), (0));
        *dst = (a | b); sindex1 = 0; sindex2 = 0; a = 0; b = 0;
    }
    /* Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res : &fake_normalized;
    crint_normalize(norm_crint);

    /* Masking Metadata to Invalidity on FAILURE */ // clang-format off
    limb_t* chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Safe No-op on chosen_freed = NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask));
    res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));
    /* Aggeressive Post-operation Cleanup */
    if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = ret_stat;
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src1 = 0; fake_src2 = 0; dst = 0;
    src1 = 0; src2 = 0; norm_crint = 0;fake_normalized.limbs = 0; fake_normalized.cap = 0;
    fake_normalized.n = 0; fake_normalized.poisoned = 0; fake_normalized.sign = 0;
    pbv_crint_clear(x); pbv_crint_clear(y); op_range = 0; err = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_ex_nor(crint x, crint y, size_t op_range, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x) & crint_validate(y)), ci_full_contract, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((!x.poisoned & !y.poisoned), crint_poisoned, { crint_free(&x); crint_free(&y); });
    if (!crint_validate(x) | !crint_validate(y)) {
        if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = CRINT_ERR_INVAL;
        pbv_crint_clear(x); pbv_crint_clear(y); op_range = 0; return __CRINT_ERRVAL__();
    } /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), ((x.poisoned | y.poisoned) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Allocation of Result crint */
    crint res; dnml_status new_stat = crint_snew(&res, op_range);
    CHOOSE_OPTION((ret_stat), (
        _lib_crt_eq(new_stat, DNML_ALLOC_OOM) &
        _lib_crt_eq(ret_stat, CRINT_SUCCESS)),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Bitwise Loop Operation */
    limb_t fake_dst = 123, fake_src1 = 456, fake_src2 = 789; limb_t *dst, *src1, *src2;
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        size_t sindex1; CHOOSE_OPTION((sindex1), (_lib_crt_lt(i, x.n)), (i), (0));
        size_t sindex2; CHOOSE_OPTION((sindex2), (_lib_crt_lt(i, y.n)), (i), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i] : &fake_dst;
        src1 = _lib_crt_eq(ret_stat, CRINT_SUCCESS) ? &x.limbs[sindex1] : &fake_src1;
        src2 = _lib_crt_eq(ret_stat, CRINT_SUCCESS) ? &y.limbs[sindex2] : &fake_src2;
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src1), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src2), (0));
        *dst = ~(a | b); sindex1 = 0; sindex2 = 0; a = 0; b = 0;
    }
    /* Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res : &fake_normalized;
    crint_normalize(norm_crint);

    /* Masking Metadata to Invalidity on FAILURE */ // clang-format off
    limb_t* chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Safe No-op on chosen_freed = NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask));
    res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));
    /* Aggeressive Post-operation Cleanup */
    if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = ret_stat;
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src1 = 0; fake_src2 = 0; dst = 0;
    src1 = 0; src2 = 0; norm_crint = 0;fake_normalized.limbs = 0; fake_normalized.cap = 0;
    fake_normalized.n = 0; fake_normalized.poisoned = 0; fake_normalized.sign = 0;
    pbv_crint_clear(x); pbv_crint_clear(y); op_range = 0; err = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_ex_xor(crint x, crint y, size_t op_range, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x) & crint_validate(y)), ci_full_contract, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((!x.poisoned & !y.poisoned), crint_poisoned, { crint_free(&x); crint_free(&y); });
    if (!crint_validate(x) | !crint_validate(y)) {
        if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = CRINT_ERR_INVAL;
        pbv_crint_clear(x); pbv_crint_clear(y); op_range = 0; return __CRINT_ERRVAL__();
    } /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), ((x.poisoned | y.poisoned) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Allocation of Result crint */
    crint res; dnml_status new_stat = crint_snew(&res, op_range);
    CHOOSE_OPTION((ret_stat), (
        _lib_crt_eq(new_stat, DNML_ALLOC_OOM) &
        _lib_crt_eq(ret_stat, CRINT_SUCCESS)),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Bitwise Loop Operation */
    limb_t fake_dst = 123, fake_src1 = 456, fake_src2 = 789; limb_t *dst, *src1, *src2;
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        size_t sindex1; CHOOSE_OPTION((sindex1), (_lib_crt_lt(i, x.n)), (i), (0));
        size_t sindex2; CHOOSE_OPTION((sindex2), (_lib_crt_lt(i, y.n)), (i), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i] : &fake_dst;
        src1 = _lib_crt_eq(ret_stat, CRINT_SUCCESS) ? &x.limbs[sindex1] : &fake_src1;
        src2 = _lib_crt_eq(ret_stat, CRINT_SUCCESS) ? &y.limbs[sindex2] : &fake_src2;
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src1), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src2), (0));
        *dst = (a ^ b); sindex1 = 0; sindex2 = 0; a = 0; b = 0;
    }
    /* Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res : &fake_normalized;
    crint_normalize(norm_crint);

    /* Masking Metadata to Invalidity on FAILURE */ // clang-format off
    limb_t* chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Safe No-op on chosen_freed = NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask));
    res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));
    /* Aggeressive Post-operation Cleanup */
    if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = ret_stat;
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src1 = 0; fake_src2 = 0; dst = 0;
    src1 = 0; src2 = 0; norm_crint = 0;fake_normalized.limbs = 0; fake_normalized.cap = 0;
    fake_normalized.n = 0; fake_normalized.poisoned = 0; fake_normalized.sign = 0;
    pbv_crint_clear(x); pbv_crint_clear(y); op_range = 0; err = 0; chosen_freed = 0; return res; // clang-format on
}
crint crint_ex_xnor(crint x, crint y, size_t op_range, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x) & crint_validate(y)), ci_full_contract, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((!x.poisoned & !y.poisoned), crint_poisoned, { crint_free(&x); crint_free(&y); });
    if (!crint_validate(x) | !crint_validate(y)) {
        if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = CRINT_ERR_INVAL;
        pbv_crint_clear(x); pbv_crint_clear(y); op_range = 0; return __CRINT_ERRVAL__();
    } /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), ((x.poisoned | y.poisoned) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    /* Allocation of Result crint */
    crint res; dnml_status new_stat = crint_snew(&res, op_range);
    CHOOSE_OPTION((ret_stat), (
        _lib_crt_eq(new_stat, DNML_ALLOC_OOM) &
        _lib_crt_eq(ret_stat, CRINT_SUCCESS)),
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Bitwise Loop Operation */
    limb_t fake_dst = 123, fake_src1 = 456, fake_src2 = 789; limb_t *dst, *src1, *src2;
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        size_t sindex1; CHOOSE_OPTION((sindex1), (_lib_crt_lt(i, x.n)), (i), (0));
        size_t sindex2; CHOOSE_OPTION((sindex2), (_lib_crt_lt(i, y.n)), (i), (0));
        dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res.limbs[i] : &fake_dst;
        src1 = _lib_crt_eq(ret_stat, CRINT_SUCCESS) ? &x.limbs[sindex1] : &fake_src1;
        src2 = _lib_crt_eq(ret_stat, CRINT_SUCCESS) ? &y.limbs[sindex2] : &fake_src2;
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src1), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src2), (0));
        *dst = ~(a ^ b); sindex1 = 0; sindex2 = 0; a = 0; b = 0;
    }
    /* Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res : &fake_normalized;
    crint_normalize(norm_crint);

    /* Masking Metadata to Invalidity on FAILURE */ // clang-format off
    limb_t* chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
    free(chosen_freed); // Safe No-op on chosen_freed = NULL since ANSI-C
    res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask));
    res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON));
    /* Aggeressive Post-operation Cleanup */
    if (_lib_crt_neq((ptr_t)err, (ptr_t)(NULL))) *err = ret_stat;
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src1 = 0; fake_src2 = 0; dst = 0;
    src1 = 0; src2 = 0; norm_crint = 0;fake_normalized.limbs = 0; fake_normalized.cap = 0;
    fake_normalized.n = 0; fake_normalized.poisoned = 0; fake_normalized.sign = 0;
    pbv_crint_clear(x); pbv_crint_clear(y); op_range = 0; err = 0; chosen_freed = 0; return res; // clang-format on
}
