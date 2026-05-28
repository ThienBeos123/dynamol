#include "cryptInt_func.h"


//* ================================= MISCALLENOUS BITWISE OPERATION ================================== */
crint crint_not(crint x, dnml_status *err) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS; 
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    crint res; dnml_status new_stat = crint_snew(&res, x.n); limb_t fake_dst; limb_t *dst;
    CHOOSE_OPTION((ret_stat), 
        (_lib_crt_eq(new_stat, DNML_ALLOC_OOM) & 
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), 
        (DNML_ALLOC_OOM), (ret_stat)
    );
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS)));
    for (size_t i = 0; _lib_crt_lt(i, x.n); ++i) {
        CHOOSE_OPTION(
            (dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), 
            ((uintptr_t)(&res.limbs[i])), 
            ((uintptr_t)(&fake_dst))
        ); *dst = ~(x.limbs[i]);
    } 
    res.n = x.n & mask; res.sign = x.sign & mask; res.poisoned = (ret_stat != CRINT_SUCCESS);
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; 
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); /**/ if (err != NULL) *err = ret_stat;
    /* Aggresive, Post-oepration Cleanup */ // clang-format off
    dst = 0; mask = 0; ret_stat = 0;  new_stat = 0; fake_dst = 0; norm_crint = 0; 
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return res; // clang-format on
}
crint crint_rshift(crint x, size_t k, dnml_status *err) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS; 
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    size_t limb_shift = k / U64_BITS, bshift = k % U64_BITS; crint res; dnml_status new_stat = crint_snew(&res, x.n);
    CHOOSE_OPTION((ret_stat), 
        (_lib_crt_eq(new_stat, DNML_ALLOC_OOM) & 
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), 
        (DNML_ALLOC_OOM), (ret_stat)
    );

    limb_t fake_dst; limb_t *dst; uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS))), discarded_bits = 0;
    CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(res.limbs)), ((uintptr_t)(x.limbs)));
    size_t end; CHOOSE_OPTION((end), (x.n), (x.n - 1), (0));
    __libdnml_smemcpy_u64(dst, x.limbs, x.n, x.n, 0, end, (!(_lib_crt_eq(ret_stat, CRINT_SUCCESS))));
    __CRINT_INTERNAL_RLSHIFT__(&res, x.n, limb_shift & mask); res.n = x.n - limb_shift;
    for (size_t i = res.n - 1; _lib_crt_neq(i, -1); --i) { /* Individual Bit Shift Loops */
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res.limbs[i])), ((uintptr_t)(&fake_dst)));
        uint64_t positioned_bits = discarded_bits << (U64_BITS - bshift); 
        *dst = (x.limbs[i] >> bshift) | positioned_bits; positioned_bits = 0; // Clearance
        discarded_bits = x.limbs[i] & ((UINT64_C(1) << bshift) - 1);
    }
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(res.limbs)), (0));
    res.n &= mask; res.sign = x.sign & mask; res.poisoned = (ret_stat != CRINT_SUCCESS);
    /* Normalization (with faking tricks) */
    res.n = x.n & mask; res.sign = x.sign & mask; res.poisoned = (ret_stat != CRINT_SUCCESS);
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; 
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); /**/ if (err != NULL) *err = ret_stat;
    /* Aggresive, Post-oepration Cleanup */ // clang-format off
    ret_stat = 0; new_stat = 0; limb_shift = 0;  bshift = 0; end = 0; fake_dst = 0; dst = 0; 
    mask = 0; discarded_bits = 0; norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.n = 0; 
    fake_normalized.cap = 0; fake_normalized.sign = 0; fake_normalized.poisoned = 0; return res; // clang-format on
}
crint crint_lshift(crint x, size_t k, dnml_status *err) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS; 
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    size_t limb_shift = k / U64_BITS, bshift = k % U64_BITS; crint res; dnml_status new_stat = crint_snew(&res, x.n);
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(new_stat, DNML_ALLOC_OOM) & 
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), 
        (DNML_ALLOC_OOM), (ret_stat)
    );

    limb_t fake_dst; limb_t *dst; uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS))), discarded_bits = 0;
    CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(res.limbs)), ((uintptr_t)(x.limbs)));
    size_t end; CHOOSE_OPTION((end), (x.n), (x.n - 1), (0));
    __libdnml_smemcpy_u64(dst, x.limbs, x.n, x.n, 0, end, (!(_lib_crt_eq(ret_stat, CRINT_SUCCESS))));
    __CRINT_INTERNAL_LLSHIFT__(&res, x.n, limb_shift & mask); res.n = x.n;
    for (size_t i = 0; _lib_crt_lt(i, x.n); ++i) { /* Individual Bit Shift Loops */
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res.limbs[i])), ((uintptr_t)(&fake_dst)));
        *dst = (x.limbs[i] << k) | discarded_bits; uint64_t iso_mask = (UINT64_C(1) << bshift) - 1;
        discarded_bits = x.limbs[i] & (iso_mask << U64_BITS - bshift); iso_mask = 0;
    }
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(res.limbs)), (0));
    res.n = x.n & mask; res.sign = x.sign & mask; res.poisoned = (ret_stat != CRINT_SUCCESS);
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; 
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); /**/ if (err != NULL) *err = ret_stat;
    /* Aggresive, Post-oepration Cleanup */ // clang-format off
    ret_stat = 0; new_stat = 0; limb_shift = 0;  bshift = 0; end = 0; fake_dst = 0; dst = 0; 
    mask = 0; discarded_bits = 0; norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.n = 0; 
    fake_normalized.cap = 0; fake_normalized.sign = 0; fake_normalized.poisoned = 0; return res; // clang-format on
}
crint crint_lshiftg(crint x, size_t k, dnml_status *err) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS; 
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), 
            ((uintptr_t)(&res.limbs[i + limb_shift])), 
            ((uintptr_t)(&fake_dst))
        ); *dst = x.limbs[i];
    } res.n = alloc_size;
    for (size_t i = limb_shift; _lib_crt_lt(i, res.n); ++i) { /* Individual Bit Shift Loops */
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res.limbs[i])), ((uintptr_t)(&fake_dst)));
        uint64_t new_carry = res.limbs[i] >> (U64_BITS - bshift);
        *dst = (res.limbs[i] << bshift) | discarded_bits;
        discarded_bits = new_carry; new_carry = 0;
    }
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(res.limbs)), (0));
    res.n &= mask; res.sign = x.sign & mask; res.poisoned = (ret_stat != CRINT_SUCCESS);
    /* Normalization (with faking tricks) */
    res.n = x.n & mask; res.sign = x.sign & mask; res.poisoned = (ret_stat != CRINT_SUCCESS);
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; 
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); /**/ if (err != NULL) *err = ret_stat;
    /* Aggresive, Post-oepration Cleanup */ // clang-format off
    ret_stat = 0; new_stat = 0; limb_shift = 0; bshift = 0; alloc_size = 0; fake_dst = 0; dst = 0; 
    mask = 0; discarded_bits = 0; norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.n = 0; 
    fake_normalized.cap = 0; fake_normalized.sign = 0; fake_normalized.poisoned = 0; return res; // clang-format on
}
dnml_status crint_mut_not(crint *x) {
    DNML_ASSERT((crint_pvalidate(x)), full_contract, { if (x->limbs != NULL) *(x->limbs) = 0; crint_free(x); });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); }); dnml_status ret_stat = CRINT_SUCCESS; 
    CHOOSE_OPTION((ret_stat), (x->poisoned), (CRINT_POISON), (ret_stat));
    for (size_t i = 0; _lib_crt_lt(i, x->n); ++i) {
        CHOOSE_OPTION((x->limbs[i]), (x->poisoned), (x->limbs[i]), (~(x->limbs[i])));
    }
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; 
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(x)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); 
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_rshift(crint *x, size_t k) {
    DNML_ASSERT((crint_pvalidate(x)), full_contract, { if (x->limbs != NULL) *(x->limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); }); dnml_status ret_stat = CRINT_SUCCESS; 
    CHOOSE_OPTION((ret_stat), (x->poisoned), (CRINT_POISON), (ret_stat));
    size_t limb_shift = k / U64_BITS, bshift = k % U64_BITS;
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS)));

    __CRINT_INTERNAL_RLSHIFT__(x, x->n, limb_shift & mask); x->n -= limb_shift & mask; 
    limb_t fake_dst; limb_t* dst; uint64_t discarded_bits = 0;
    for (size_t i = x->cap - 1; _lib_crt_neq(i, -1); --i) { /* Individual Bits Loop */
        size_t index; CHOOSE_OPTION((index), (_lib_crt_lt(i, x->n)), (i), (0));
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x->limbs[i])), ((uintptr_t)(&fake_dst)));
        /* Pre-calculations - Ensure Constant Time */
        uint64_t positioned_bits = discarded_bits << (U64_BITS - bshift);
        uint64_t dbit_calc = x->limbs[index] & ((UINT64_C(1) << bshift) - 1);
        uint64_t dst_val = (x->limbs[index] >> bshift) | positioned_bits;
        /* Actual Assignment + Per Iteration Cleanup */
        CHOOSE_OPTION((discarded_bits), (_lib_crt_lt(i, x->n)), (dbit_calc), (discarded_bits));
        CHOOSE_OPTION((*dst), (_lib_crt_lt(i, x->n)), (dst_val), (*dst));
        index = 0; positioned_bits = 0; dbit_calc = 0; dst_val = 0;
    } 
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; 
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(x)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); 
    /* Aggrestive, Post-operation Cleanup */ // clang-format off
    limb_shift = 0; bshift = 0; mask = 0; fake_dst = 0; dst = 0; discarded_bits = 0; norm_crint = 0; 
    fake_normalized.limbs = 0; fake_normalized. n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0; 
    fake_normalized.poisoned = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_lshift(crint *x, size_t k) {
    DNML_ASSERT((crint_pvalidate(x)), full_contract, { if (x->limbs != NULL) *(x->limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); }); dnml_status ret_stat = CRINT_SUCCESS; 
    CHOOSE_OPTION((ret_stat), (x->poisoned), (CRINT_POISON), (ret_stat));
    size_t limb_shift = k / U64_BITS, bshift = k % U64_BITS;
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS)));

    __CRINT_INTERNAL_LLSHIFT__(x, x->n, limb_shift & mask);
    limb_t fake_dst; limb_t* dst; uint64_t discarded_bits = 0;
    for (size_t i = 0; _lib_crt_lt(i, x->cap); ++i) { /* Individual Bits Loop */
        size_t index; CHOOSE_OPTION((index), (_lib_crt_lt(i, x->n)), (i), (0));
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x->limbs[i])), ((uintptr_t)(&fake_dst)));
        /* Pre-calculations - Ensure Constant Time */
        uint64_t previous_dbits = discarded_bits, iso_mask = (UINT64_C(1) << bshift) - 1;
        uint64_t dbit_calc = x->limbs[index] & (iso_mask << U64_BITS - bshift);
        uint64_t dst_val = (x->limbs[index] << bshift) | previous_dbits;
        /* Actual Assignment + Per Iteration Cleanup */
        CHOOSE_OPTION((discarded_bits), (_lib_crt_lt(i, x->n)), (dbit_calc), (discarded_bits));
        CHOOSE_OPTION((*dst), (_lib_crt_lt(i, x->n)), (dst_val), (*dst));
        index = 0; previous_dbits = 0; dbit_calc = 0; dst_val = 0;
    } 
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; 
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(x)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); 
    /* Aggrestive, Post-operation Cleanup */ // clang-format off
    limb_shift = 0; bshift = 0; mask = 0; fake_dst = 0; dst = 0; discarded_bits = 0; norm_crint = 0; 
    fake_normalized.limbs = 0; fake_normalized. n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0; 
    fake_normalized.poisoned = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_lshiftg(crint *x, size_t k) {
    DNML_ASSERT((crint_pvalidate(x)), full_contract, { if (x->limbs != NULL) *(x->limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); }); dnml_status ret_stat = CRINT_SUCCESS; 
    CHOOSE_OPTION((ret_stat), (x->poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    size_t limb_shift = k / U64_BITS, bshift = k % U64_BITS; size_t alloc_cap = (x->n + limb_shift + !!(bshift));
    dnml_status reserve_stat = crint_reserve(x, alloc_cap); CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)) & 
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), 
        (reserve_stat), (ret_stat)
    ); uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS)));

    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint *llshift_operated; size_t llshift_size; // Used in LLSHIFT
    crint fake_operated = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((llshift_operated), (_lib_crt_eq(ret_stat, DNML_ALLOC_OOM)), ((uintptr_t)(&fake_operated)), ((uintptr_t)x));
    CHOOSE_OPTION((llshift_size), (_lib_crt_eq(ret_stat, DNML_ALLOC_OOM)), (FAKE_BUF_CAP), (x->cap));
    __CRINT_INTERNAL_LLSHIFT__(llshift_operated, llshift_size, limb_shift & mask); 
    x->n += (limb_shift + !!(bshift)) & mask;
    limb_t fake_dst; limb_t* dst; uint64_t discarded_bits = 0; // Used in individual Bits Loop
    for (size_t i = limb_shift & mask; _lib_crt_lt(i, x->n); ++i) { /* Individual Bits Loop */ 
       size_t index; CHOOSE_OPTION((index), (_lib_crt_lt(i, x->n)), (i), (0));
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x->limbs[i])), ((uintptr_t)(&fake_dst)));
        /* Pre-calculations - Ensure Constant Time */
        uint64_t new_carry = x->limbs[index] >> (U64_BITS - bshift);
        uint64_t dst_val = (x->limbs[index] << bshift) | discarded_bits;
        /* Actual Assignment + Per Iteration Cleanup */
        CHOOSE_OPTION((discarded_bits), (_lib_crt_lt(i, x->n)), (new_carry), (discarded_bits));
        CHOOSE_OPTION((*dst), (_lib_crt_lt(i, x->n)), (dst_val), (*dst));
        index = 0; new_carry = 0; dst_val = 0;
    }
    crint* norm_crint; CHOOSE_OPTION(
        (norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), 
        ((uintptr_t)(x)), ((uintptr_t)(&fake_operated))
    ); crint_normalize(norm_crint); 
    /* Aggrestive, Post-operation Cleanup */ // clang-format off
    limb_shift = 0; bshift = 0; alloc_cap = 0; reserve_stat = 0; mask = 0;
    __libdnml_smemwipe_u64(fake_buf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    llshift_operated = 0; llshift_size = 0; norm_crint = 0; fake_operated.poisoned = 0;
    fake_operated.limbs = 0; fake_operated.n = 0; fake_operated.cap = 0;
    fake_operated.sign = 0; fake_dst = 0; dst = 0; discarded_bits = 0; return ret_stat; // clang-format on
}




//* ================================= MUTATIVE, FIXED WIDTH BITWISE OPERATION ================================== */
dnml_status crint_mut_andu64(crint *x, const uint64_t val) {
    DNML_ASSERT((crint_pvalidate(x)), full_contract, { if (x->limbs != NULL) *(x->limbs) = 0; crint_free(x); });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); }); dnml_status ret_stat = CRINT_SUCCESS; 
    CHOOSE_OPTION((ret_stat), (x->poisoned), (CRINT_POISON), (ret_stat));
    uint64_t first_limb = x->limbs[0]; CHOOSE_OPTION((first_limb), (x->n), (first_limb), (0));
    first_limb = first_limb & val; int8_t ret_sign; uint64_t origin_val = x->limbs[0];
    /* Re-assignment */
    CHOOSE_OPTION((x->limbs[0]), (x->poisoned), (origin_val), (first_limb));
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (!!(first_limb)));
    CHOOSE_OPTION((ret_sign), (first_limb), (x->sign), (1));
    CHOOSE_OPTION((x->sign), (x->poisoned), (x->sign), (ret_sign));
    __libdnml_smemwipe_u64(x->limbs, x->cap, 1, x->cap - 1, (x->poisoned)); // clang-format off
    first_limb = 0; ret_sign = 0; origin_val = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_nandu64(crint *x, const uint64_t val) {
    DNML_ASSERT((crint_pvalidate(x)), full_contract, { if (x->limbs != NULL) *(x->limbs) = 0; crint_free(x); });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); }); dnml_status ret_stat = CRINT_SUCCESS; 
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
    first_limb = 0; origin_val = 0; ret_size = 0; end = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_oru64(crint *x, const uint64_t val) {
    DNML_ASSERT((crint_pvalidate(x)), full_contract, { if (x->limbs != NULL) *(x->limbs) = 0; crint_free(x); });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); }); dnml_status ret_stat = CRINT_SUCCESS; 
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
    first_limb = 0; origin_val = 0; ret_size = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_noru64(crint *x, const uint64_t val) {
    DNML_ASSERT((crint_pvalidate(x)), full_contract, { if (x->limbs != NULL) *(x->limbs) = 0; crint_free(x); });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); }); dnml_status ret_stat = CRINT_SUCCESS; 
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
        CHOOSE_OPTION((dst), (_lib_crt_lt(i, x->n) & !(x->poisoned)), ((uintptr_t)(&x->limbs[i])), ((uintptr_t)(&fake_dst)));
        *dst = ~(curr_limb | 0); curr_limb = 0; // Current Iteration Clearance
    }
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(x)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); __libdnml_smemwipe_u64(x->limbs, x->cap, x->n, x->cap - 1, (x->poisoned));
    /* Aggressive Post-operation CLeanup */ // clang-format off
    first_limb = 0; origin_val = 0; ret_size = 0; fake_dst = 0; dst = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_xoru64(crint *x, const uint64_t val) {
    DNML_ASSERT((crint_pvalidate(x)), full_contract, { if (x->limbs != NULL) *(x->limbs) = 0; crint_free(x); });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); }); dnml_status ret_stat = CRINT_SUCCESS; 
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
        CHOOSE_OPTION((dst), (_lib_crt_lt(i, x->n) & !(x->poisoned)), ((uintptr_t)(&x->limbs[i])), ((uintptr_t)(&fake_dst)));
        *dst = curr_limb ^ 0; curr_limb = 0; // Current Iteration Clearance
    } 
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(x)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); __libdnml_smemwipe_u64(x->limbs, x->cap, x->n, x->cap - 1, (x->poisoned));
    /* Aggressive Post-operation CLeanup */ // clang-format off
    first_limb = 0; origin_val = 0; ret_size = 0; fake_dst = 0; dst = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_xnoru64(crint *x, const uint64_t val) {
    DNML_ASSERT((crint_pvalidate(x)), full_contract, { if (x->limbs != NULL) *(x->limbs) = 0; crint_free(x); });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); }); dnml_status ret_stat = CRINT_SUCCESS; 
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
        CHOOSE_OPTION((dst), (_lib_crt_lt(i, x->n) & !(x->poisoned)), ((uintptr_t)(&x->limbs[i])), ((uintptr_t)(&fake_dst)));
        *dst = ~(curr_limb ^ 0); curr_limb = 0; // Current Iteration Clearance
    } 
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(x)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); __libdnml_smemwipe_u64(x->limbs, x->cap, x->n, x->cap - 1, (x->poisoned));
    /* Aggressive Post-operation CLeanup */ // clang-format off
    first_limb = 0; origin_val = 0; ret_size = 0; fake_dst = 0; dst = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_and(crint *x, crint y) {
    DNML_ASSERT((crint_pvalidate(x) | crint_validate(y)), full_contract, {
        if (x->limbs != NULL) *x->limbs = 0; crint_free(x); 
        if (y.limbs != NULL) *y.limbs = 0; crint_free(&y);
    });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((y.poisoned), poisoined, { crint_free(x); crint_free(&y); }); dnml_status ret_stat = CRINT_SUCCESS; 
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x->limbs[i])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((src), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&y.limbs[i])), ((uintptr_t)&fake_src))
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src), (0));
        *dst = a & b; a = 0; b = 0;
    } 
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (op_range));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; 
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(x)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint);
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    op_range = 0; reserve_stat = 0;; fake_dst = 0; dst = 0; fake_src = 0; src = 0; 
    norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_nand(crint *x, crint y) {
    DNML_ASSERT((crint_pvalidate(x) | crint_validate(y)), full_contract, {
        if (x->limbs != NULL) *x->limbs = 0; crint_free(x); 
        if (y.limbs != NULL) *y.limbs = 0; crint_free(&y);
    });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((y.poisoned), poisoined, { crint_free(x); crint_free(&y); }); dnml_status ret_stat = CRINT_SUCCESS; 
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x->limbs[i])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((src), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&y.limbs[i])), ((uintptr_t)&fake_src))
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src), (0));
        *dst = ~(a & b); a = 0; b = 0;
    } 
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (op_range));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; 
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(x)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); 
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    op_range = 0; reserve_stat = 0;; fake_dst = 0; dst = 0; fake_src = 0; src = 0; 
    norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_or(crint *x, crint y) {
    DNML_ASSERT((crint_pvalidate(x) | crint_validate(y)), full_contract, {
        if (x->limbs != NULL) *x->limbs = 0; crint_free(x); 
        if (y.limbs != NULL) *y.limbs = 0; crint_free(&y);
    });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((y.poisoned), poisoined, { crint_free(x); crint_free(&y); }); dnml_status ret_stat = CRINT_SUCCESS; 
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x->limbs[i])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((src), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&y.limbs[i])), ((uintptr_t)&fake_src))
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src), (0));
        *dst = (a | b); a = 0; b = 0;
    } 
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (op_range));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; 
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(x)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); 
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    op_range = 0; reserve_stat = 0;; fake_dst = 0; dst = 0; fake_src = 0; src = 0; 
    norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_nor(crint *x, crint y) {
    DNML_ASSERT((crint_pvalidate(x) | crint_validate(y)), full_contract, {
        if (x->limbs != NULL) *x->limbs = 0; crint_free(x); 
        if (y.limbs != NULL) *y.limbs = 0; crint_free(&y);
    });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((y.poisoned), poisoined, { crint_free(x); crint_free(&y); }); dnml_status ret_stat = CRINT_SUCCESS; 
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x->limbs[i])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((src), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&y.limbs[i])), ((uintptr_t)&fake_src))
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src), (0));
        *dst = ~(a | b); a = 0; b = 0;
    } 
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (op_range));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; 
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(x)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); 
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    op_range = 0; reserve_stat = 0;; fake_dst = 0; dst = 0; fake_src = 0; src = 0; 
    norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_xor(crint *x, crint y) {
    DNML_ASSERT((crint_pvalidate(x) | crint_validate(y)), full_contract, {
        if (x->limbs != NULL) *x->limbs = 0; crint_free(x); 
        if (y.limbs != NULL) *y.limbs = 0; crint_free(&y);
    });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((y.poisoned), poisoined, { crint_free(x); crint_free(&y); }); dnml_status ret_stat = CRINT_SUCCESS; 
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x->limbs[i])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((src), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&y.limbs[i])), ((uintptr_t)&fake_src))
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src), (0));
        *dst = (a ^ b); a = 0; b = 0;
    } 
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (op_range));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; 
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(x)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); 
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    op_range = 0; reserve_stat = 0;; fake_dst = 0; dst = 0; fake_src = 0; src = 0; 
    norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_xnor(crint *x, crint y) {
    DNML_ASSERT((crint_pvalidate(x) | crint_validate(y)), full_contract, {
        if (x->limbs != NULL) *x->limbs = 0; crint_free(x); 
        if (y.limbs != NULL) *y.limbs = 0; crint_free(&y);
    });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((y.poisoned), poisoined, { crint_free(x); crint_free(&y); }); dnml_status ret_stat = CRINT_SUCCESS; 
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x->limbs[i])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((src), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&y.limbs[i])), ((uintptr_t)&fake_src))
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src), (0));
        *dst = ~(a ^ b); a = 0; b = 0;
    } 
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (op_range));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; 
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(x)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); 
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    op_range = 0; reserve_stat = 0; fake_dst = 0; dst = 0; fake_src = 0; src = 0; 
    norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return ret_stat; // clang-format on
}




//* ================================= MUTATIVE, EXPLICIT WIDTH BITWISE OPERATION ================================== */
dnml_status crint_mutex_andu64(crint *x, const uint64_t val, size_t op_range) {
    DNML_ASSERT((crint_pvalidate(x)), full_contract, { if (x->limbs != NULL) *(x->limbs) = 0; crint_free(x); });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); }); dnml_status ret_stat = CRINT_SUCCESS; uint8_t noop = false;
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
    ); uint64_t oom_mask = (uint64_t)(-(int64_t)(ret_stat != DNML_ALLOC_OOM));
    /* Clearing out the other limbs */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; limb_t *dst; size_t buf_cap;
    CHOOSE_OPTION((dst), (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)), ((uintptr_t)(fake_buf)), ((uintptr_t)(x->limbs)))
    CHOOSE_OPTION((buf_cap), (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)), (FAKE_BUF_CAP), (x->cap));
    __libdnml_smemwipe_u64(dst, buf_cap, 1, buf_cap - 1, (noop));
    /* Aggressive Memory Clearance */ // clang-format off 
    x->cap &= oom_mask; x->n &= oom_mask; x->sign &= oom_mask; x->poisoned &= oom_mask;
    noop = 0; first_limb = 0; ret_sign = 0; origin_val = 0; alloc_range = 0;
    reserve_stat = 0; dst = 0; buf_cap = 0; oom_mask = 0; return ret_stat;
}
dnml_status crint_mutex_nandu64(crint *x, const uint64_t val, size_t op_range) {
    DNML_ASSERT((crint_pvalidate(x)), full_contract, { if (x->limbs != NULL) *(x->limbs) = 0; crint_free(x); });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); }); dnml_status ret_stat = CRINT_SUCCESS;
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x->limbs[i])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((a), (i < x->n), (*dst), (0)); CHOOSE_OPTION((b), (!i), (val), (0));
        *dst = ~(a & b); a = 0; b = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(x)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; fake_dst = 0; dst = 0; norm_crint = 0; fake_normalized.limbs = 0; 
    fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0; 
    fake_normalized.poisoned = 0; return ret_stat; // clang-format on
}
dnml_status crint_mutex_oru64(crint *x, const uint64_t val, size_t op_range) {
    DNML_ASSERT((crint_pvalidate(x)), full_contract, { if (x->limbs != NULL) *(x->limbs) = 0; crint_free(x); });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); }); 
    dnml_status ret_stat = CRINT_SUCCESS; uint8_t noop = false;
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
    ); uint64_t oom_mask = (uint64_t)(-(int64_t)(ret_stat != DNML_ALLOC_OOM));
    /* Clearing out the other limbs */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; limb_t *dst; size_t end, start, buf_cap;
    CHOOSE_OPTION((dst), (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)), ((uintptr_t)(fake_buf)), ((uintptr_t)(x->limbs)))
    CHOOSE_OPTION((start), (_lib_crt_lt(op_range, x->n)), (x->n), (0));
    CHOOSE_OPTION((start), (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)), (0), (start));
    CHOOSE_OPTION((end), (_lib_crt_lt(op_range, x->n)), (op_range - 1), (x->n - op_range));
    CHOOSE_OPTION((end), (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)), (FAKE_BUF_CAP), (end));
    CHOOSE_OPTION((buf_cap), (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM)), (FAKE_BUF_CAP), (x->cap));
    __libdnml_smemwipe_u64(dst, buf_cap, start, end, (noop));
    /* Aggressive Memory Clearance */ // clang-format off 
    x->cap &= oom_mask; x->n &= oom_mask; x->sign &= oom_mask; x->poisoned &= oom_mask;
    noop = 0; first_limb = 0; ret_sign = 0; origin_val = 0; alloc_range = 0; reserve_stat = 0; 
    dst = 0; start = 0; end = 0; buf_cap = 0; oom_mask = 0; return ret_stat;
}
dnml_status crint_mutex_noru64(crint *x, const uint64_t val, size_t op_range) {
    DNML_ASSERT((crint_pvalidate(x)), full_contract, { if (x->limbs != NULL) *(x->limbs) = 0; crint_free(x); });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); }); dnml_status ret_stat = CRINT_SUCCESS;
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x->limbs[i])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((a), (i < x->n), (*dst), (0)); CHOOSE_OPTION((b), (!i), (val), (0));
        *dst = ~(a | b); a = 0; b = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(x)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; fake_dst = 0; dst = 0; norm_crint = 0; fake_normalized.limbs = 0; 
    fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0; 
    fake_normalized.poisoned = 0; return ret_stat; // clang-format on
}
dnml_status crint_mutex_xoru64(crint *x, const uint64_t val, size_t op_range) {
    DNML_ASSERT((crint_pvalidate(x)), full_contract, { if (x->limbs != NULL) *(x->limbs) = 0; crint_free(x); });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); }); dnml_status ret_stat = CRINT_SUCCESS;
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x->limbs[i])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((a), (i < x->n), (*dst), (0)); CHOOSE_OPTION((b), (!i), (val), (0));
        *dst = (a ^ b); a = 0; b = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(x)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; fake_dst = 0; dst = 0; norm_crint = 0; fake_normalized.limbs = 0; 
    fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0; 
    fake_normalized.poisoned = 0; return ret_stat; // clang-format on
}
dnml_status crint_mutex_xnoru64(crint *x, const uint64_t val, size_t op_range) {
    DNML_ASSERT((crint_pvalidate(x)), full_contract, { if (x->limbs != NULL) *(x->limbs) = 0; crint_free(x); });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); }); dnml_status ret_stat = CRINT_SUCCESS;
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x->limbs[i])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((a), (i < x->n), (*dst), (0)); CHOOSE_OPTION((b), (!i), (val), (0));
        *dst = ~(a ^ b); a = 0; b = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(x)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; fake_dst = 0; dst = 0; norm_crint = 0; fake_normalized.limbs = 0; 
    fake_normalized.n = 0; fake_normalized.cap = 0; fake_normalized.sign = 0; 
    fake_normalized.poisoned = 0; return ret_stat; // clang-format on
}
dnml_status crint_mutex_andi64(crint *x, const int64_t val, size_t op_range) {
    DNML_ASSERT((crint_pvalidate(x)), full_contract, { if (x->limbs != NULL) *(x->limbs) = 0; crint_free(x); });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); }); dnml_status ret_stat = CRINT_SUCCESS;
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
        uint64_t a, b, mag_val = __MAG_I64__(val); 
        CHOOSE_OPTION((index), (_lib_crt_lt(i, x->n)), (i), (0));
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x->limbs[index])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0));
        CHOOSE_OPTION((b), (!i), (mag_val), (extension_bits)); 
        *dst = (a & b); a = 0; b = 0; mag_val = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(x)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; extension_bits = 0; fake_dst = 0; dst = 0; index = 0;
    norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return ret_stat; // clang-format-on
}
dnml_status crint_mutex_nandi64(crint *x, const int64_t val, size_t op_range) {
    DNML_ASSERT((crint_pvalidate(x)), full_contract, { if (x->limbs != NULL) *(x->limbs) = 0; crint_free(x); });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); }); dnml_status ret_stat = CRINT_SUCCESS;
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
        uint64_t a, b, mag_val = __MAG_I64__(val); 
        CHOOSE_OPTION((index), (_lib_crt_lt(i, x->n)), (i), (0));
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x->limbs[index])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0));
        CHOOSE_OPTION((b), (!i), (mag_val), (extension_bits)); 
        *dst = ~(a & b); a = 0; b = 0; mag_val = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(x)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; extension_bits = 0; fake_dst = 0; dst = 0; index = 0;
    norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return ret_stat; // clang-format-on
}
dnml_status crint_mutex_ori64(crint *x, const int64_t val, size_t op_range) {
    DNML_ASSERT((crint_pvalidate(x)), full_contract, { if (x->limbs != NULL) *(x->limbs) = 0; crint_free(x); });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); }); dnml_status ret_stat = CRINT_SUCCESS;
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
        uint64_t a, b, mag_val = __MAG_I64__(val); 
        CHOOSE_OPTION((index), (_lib_crt_lt(i, x->n)), (i), (0));
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x->limbs[index])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0));
        CHOOSE_OPTION((b), (!i), (mag_val), (extension_bits)); 
        *dst = (a | b); a = 0; b = 0; mag_val = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(x)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; extension_bits = 0; fake_dst = 0; dst = 0; index = 0;
    norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return ret_stat; // clang-format-on
}
dnml_status crint_mutex_nori64(crint *x, const int64_t val, size_t op_range) {
    DNML_ASSERT((crint_pvalidate(x)), full_contract, { if (x->limbs != NULL) *(x->limbs) = 0; crint_free(x); });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); }); dnml_status ret_stat = CRINT_SUCCESS;
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
        uint64_t a, b, mag_val = __MAG_I64__(val); 
        CHOOSE_OPTION((index), (_lib_crt_lt(i, x->n)), (i), (0));
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x->limbs[index])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0));
        CHOOSE_OPTION((b), (!i), (mag_val), (extension_bits)); 
        *dst = ~(a | b); a = 0; b = 0; mag_val = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(x)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; extension_bits = 0; fake_dst = 0; dst = 0; index = 0;
    norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return ret_stat; // clang-format-on
}
dnml_status crint_mutex_xori64(crint *x, const int64_t val, size_t op_range) {
    DNML_ASSERT((crint_pvalidate(x)), full_contract, { if (x->limbs != NULL) *(x->limbs) = 0; crint_free(x); });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); }); dnml_status ret_stat = CRINT_SUCCESS;
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
        uint64_t a, b, mag_val = __MAG_I64__(val); 
        CHOOSE_OPTION((index), (_lib_crt_lt(i, x->n)), (i), (0));
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x->limbs[index])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0));
        CHOOSE_OPTION((b), (!i), (mag_val), (extension_bits)); 
        *dst = (a ^ b); a = 0; b = 0; mag_val = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(x)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; extension_bits = 0; fake_dst = 0; dst = 0; index = 0;
    norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return ret_stat; // clang-format-on
}
dnml_status crint_mutex_xnori64(crint *x, const int64_t val, size_t op_range) {
    DNML_ASSERT((crint_pvalidate(x)), full_contract, { if (x->limbs != NULL) *(x->limbs) = 0; crint_free(x); });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); }); dnml_status ret_stat = CRINT_SUCCESS;
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
        uint64_t a, b, mag_val = __MAG_I64__(val); 
        CHOOSE_OPTION((index), (_lib_crt_lt(i, x->n)), (i), (0));
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x->limbs[index])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0));
        CHOOSE_OPTION((b), (!i), (mag_val), (extension_bits));
        *dst = ~(a ^ b); a = 0; b = 0; mag_val = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(x)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; extension_bits = 0; fake_dst = 0; dst = 0; index = 0;
    norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return ret_stat; // clang-format-on
}
dnml_status crint_mutex_and(crint *x, crint y, size_t op_range) {
    DNML_ASSERT((crint_pvalidate(x) | crint_validate(y)), full_contract, {
        if (x->limbs != NULL) *x->limbs = 0; crint_free(x); 
        if (y.limbs != NULL) *y.limbs = 0; crint_free(&y);
    });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((y.poisoned), poisoined, { crint_free(x); crint_free(&y); }); dnml_status ret_stat = CRINT_SUCCESS; 
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x->limbs[index])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((src), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&y.limbs[index])), ((uintptr_t)&fake_src));
        CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0)); CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src), (0));
        *dst = (a & b); a = 0; b = 0; index = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(x)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.sign = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return ret_stat; // clang-format on
}
dnml_status crint_mutex_nand(crint *x, crint y, size_t op_range) {
    DNML_ASSERT((crint_pvalidate(x) | crint_validate(y)), full_contract, {
        if (x->limbs != NULL) *x->limbs = 0; crint_free(x); 
        if (y.limbs != NULL) *y.limbs = 0; crint_free(&y);
    });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((y.poisoned), poisoined, { crint_free(x); crint_free(&y); }); dnml_status ret_stat = CRINT_SUCCESS; 
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x->limbs[index])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((src), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&y.limbs[index])), ((uintptr_t)&fake_src));
        CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0)); CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src), (0));
        *dst = ~(a & b); a = 0; b = 0; index = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(x)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.sign = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return ret_stat; // clang-format on
}
dnml_status crint_mutex_or(crint *x, crint y, size_t op_range) {
    DNML_ASSERT((crint_pvalidate(x) | crint_validate(y)), full_contract, {
        if (x->limbs != NULL) *x->limbs = 0; crint_free(x); 
        if (y.limbs != NULL) *y.limbs = 0; crint_free(&y);
    });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((y.poisoned), poisoined, { crint_free(x); crint_free(&y); }); dnml_status ret_stat = CRINT_SUCCESS; 
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x->limbs[index])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((src), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&y.limbs[index])), ((uintptr_t)&fake_src));
        CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0)); CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src), (0));
        *dst = (a | b); a = 0; b = 0; index = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(x)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.sign = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return ret_stat; // clang-format on
}
dnml_status crint_mutex_nor(crint *x, crint y, size_t op_range) {
    DNML_ASSERT((crint_pvalidate(x) | crint_validate(y)), full_contract, {
        if (x->limbs != NULL) *x->limbs = 0; crint_free(x); 
        if (y.limbs != NULL) *y.limbs = 0; crint_free(&y);
    });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((y.poisoned), poisoined, { crint_free(x); crint_free(&y); }); dnml_status ret_stat = CRINT_SUCCESS; 
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x->limbs[index])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((src), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&y.limbs[index])), ((uintptr_t)&fake_src));
        CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0)); CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src), (0));
        *dst = ~(a | b); a = 0; b = 0; index = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(x)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.sign = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return ret_stat; // clang-format on
}
dnml_status crint_mutex_xor(crint *x, crint y, size_t op_range) {
    DNML_ASSERT((crint_pvalidate(x) | crint_validate(y)), full_contract, {
        if (x->limbs != NULL) *x->limbs = 0; crint_free(x); 
        if (y.limbs != NULL) *y.limbs = 0; crint_free(&y);
    });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((y.poisoned), poisoined, { crint_free(x); crint_free(&y); }); dnml_status ret_stat = CRINT_SUCCESS; 
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x->limbs[index])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((src), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&y.limbs[index])), ((uintptr_t)&fake_src));
        CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0)); CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src), (0));
        *dst = (a ^ b); a = 0; b = 0; index = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(x)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.sign = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return ret_stat; // clang-format on
}
dnml_status crint_mutex_xnor(crint *x, crint y, size_t op_range) {
    DNML_ASSERT((crint_pvalidate(x) | crint_validate(y)), full_contract, {
        if (x->limbs != NULL) *x->limbs = 0; crint_free(x); 
        if (y.limbs != NULL) *y.limbs = 0; crint_free(&y);
    });
    DNML_TEST_ASSERT((x->poisoned), poisoined, { crint_free(x); crint_free(&y); });
    DNML_TEST_ASSERT((y.poisoned), poisoined, { crint_free(x); crint_free(&y); }); dnml_status ret_stat = CRINT_SUCCESS; 
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x->limbs[index])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((src), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&y.limbs[index])), ((uintptr_t)&fake_src));
        CHOOSE_OPTION((a), (_lib_crt_lt(i, x->n)), (*dst), (0)); CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src), (0));
        *dst = ~(a ^ b); a = 0; b = 0; index = 0;
    }
    CHOOSE_OPTION((x->n), (x->poisoned), (x->n), (crtmax(x->n, op_range)));
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(x)), ((uintptr_t)(&fake_normalized)));
    crint_normalize(norm_crint); CHOOSE_OPTION((x->sign), (x->n | x->poisoned), (x->sign), (1));
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    reserve_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; norm_crint = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.sign = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return ret_stat; // clang-format on
}




//* ================================= FUNCTION, FIXED WIDTH BITWISE OPERATION ================================== */
crint crint_andu64(crint x, const uint64_t val, dnml_status *err) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS; 
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
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, DNML_ALLOC_OOM)), ((uintptr_t)(fake_buf)), (uintptr_t)(res.limbs));
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    res.limbs[0] = first_limb & val; res.n = !!(first_limb); CHOOSE_OPTION((res.sign), (first_limb), (x.sign), (1));
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)res.limbs), ((uintptr_t)0));

    /* Masking to Invalidity + Aggressive Post-operation Cleanup */
    res.limbs[0] &= mask; res.n &= mask; res.sign &= mask; res.poisoned = (ret_stat == CRINT_POISON);
    if (err != NULL) *err = ret_stat; // clang-format off
    ret_stat = 0; new_stat = 0;  fake_buf[0] = 0; 
    first_limb = 0; mask = 0; return res; // clang-format on
}
crint crint_nandu64(crint x, const uint64_t val, dnml_status *err) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS; 
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
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, DNML_ALLOC_OOM)), ((uintptr_t)(fake_buf)), (uintptr_t)(res.limbs));
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    res.limbs[0] = ~(first_limb & val) & mask; size_t cap;
    CHOOSE_OPTION((cap), (_lib_crt_eq(ret_stat, DNML_ALLOC_OOM)), (FAKE_BUF_CAP), (x.n));
    __libdnml_smemset_u64(res.limbs, UINT8_MAX, cap, 0, cap - 1, (x.poisoned)); res.n = x.n;

    /* Masking to Invalidity + Aggressive Post-operation Cleanup */
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)res.limbs), ((uintptr_t)0));
    res.limbs[0] &= mask; res.n &= mask; res.sign &= mask; res.poisoned = (ret_stat == CRINT_POISON);
    if (err != NULL) *err = ret_stat;
    __libdnml_memwipe_strict(fake_buf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false); // clang-format off
    ret_stat = 0; new_stat = 0; fake_buf[0] = 0; first_limb = 0; mask = 0; return res; // clang-format on
}
crint crint_oru64(crint x, const uint64_t val, dnml_status *err) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS; 
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res.limbs[i])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((src), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x.limbs[i])), ((uintptr_t)(&fake_src)));
        uint64_t b; CHOOSE_OPTION((b), (!i), (val), (0)); *dst = (*src | b); b = 0;
    }
    /* Masking to Invalidity + Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = x.n;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)&res), ((uintptr_t)&fake_normalized));
    crint_normalize(norm_crint);
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)res.limbs), ((uintptr_t)0));
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask)); res.n &= mask; res.cap = x.n & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON)); /**/ if (err != NULL) *err = ret_stat;
    /* Aggressive Post-operation Cleanup */ // clang-format off
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; mask = 0;
    norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return res; // clang-format on
}
crint crint_noru64(crint x, const uint64_t val, dnml_status *err) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS; 
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res.limbs[i])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((src), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x.limbs[i])), ((uintptr_t)(&fake_src)));
        uint64_t b; CHOOSE_OPTION((b), (!i), (val), (0)); *dst = ~(*src | b); b = 0;
    }
    /* Masking to Invalidity + Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = x.n;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)&res), ((uintptr_t)&fake_normalized));
    crint_normalize(norm_crint);
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)res.limbs), ((uintptr_t)0));
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask)); res.n &= mask; res.cap = x.n & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON)); /**/ if (err != NULL) *err = ret_stat;
    /* Aggressive Post-operation Cleanup */ // clang-format off
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; mask = 0;
    norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return res; // clang-format on
}
crint crint_xoru64(crint x, const uint64_t val, dnml_status *err) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS; 
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res.limbs[i])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((src), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x.limbs[i])), ((uintptr_t)(&fake_src)));
        uint64_t b; CHOOSE_OPTION((b), (!i), (val), (0)); *dst = (*src ^ b); b = 0;
    }
    /* Masking to Invalidity + Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = x.n;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)&res), ((uintptr_t)&fake_normalized));
    crint_normalize(norm_crint);
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)res.limbs), ((uintptr_t)0));
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask)); res.n &= mask; res.cap = x.n & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON)); /**/ if (err != NULL) *err = ret_stat;
    /* Aggressive Post-operation Cleanup */ // clang-format off
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; mask = 0;
    norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return res; // clang-format on
}
crint crint_xnoru64(crint x, const uint64_t val, dnml_status *err) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS; 
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res.limbs[i])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((src), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x.limbs[i])), ((uintptr_t)(&fake_src)));
        uint64_t b; CHOOSE_OPTION((b), (!i), (val), (0)); *dst = ~(*src ^ b); b = 0;
    }
    /* Masking to Invalidity + Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = x.n;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)&res), ((uintptr_t)&fake_normalized));
    crint_normalize(norm_crint);
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)res.limbs), ((uintptr_t)0));
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask)); res.n &= mask; res.cap = x.n & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON)); /**/ if (err != NULL) *err = ret_stat;
    /* Aggressive Post-operation Cleanup */ // clang-format off
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; mask = 0;
    norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return res; // clang-format on
}
crint crint_and(crint x, crint y, dnml_status *err) {
    DNML_ASSERT((crint_validate(x) | crint_validate(y)), full_contract, {
        if (x.limbs != NULL) *x.limbs = 0; crint_free(&x); 
        if (y.limbs != NULL) *y.limbs = 0; crint_free(&y);
    });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((y.poisoned), poisoined, { crint_free(&x); crint_free(&y); }); 
    dnml_status ret_stat = CRINT_SUCCESS;
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res.limbs[i])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((src1), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x.limbs[i])), ((uintptr_t)&fake_src1));
        CHOOSE_OPTION((src2), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&y.limbs[i])), ((uintptr_t)&fake_src2));
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src1), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src2), (0));
        *dst = ((*src1) & (*src2)); a = 0; b = 0;
    }
    /* Normalization + Masking to Invalidity */
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)res.limbs), ((uintptr_t)0));
    res.n = op_range & mask; res.cap = op_range & mask; res.sign = (int64_t)((x.sign | y.sign) & mask);
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON)); // Masked to invalidity on non-SUCCESS cases
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; // Setting up fake, pseudo-nops buffer for normalization
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)&res), ((uintptr_t)&fake_normalized))
    crint_normalize(norm_crint); /**/ if (err != NULL) *err= ret_stat;
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    ret_stat = 0; op_range = 0; new_stat = 0; mask = 0; norm_crint = 0;
    fake_dst = 0; fake_src1 = 0; fake_src2 = 0; dst = 0; src1 = 0; src2 = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return res;
}
crint crint_nand(crint x, crint y, dnml_status *err) {
    DNML_ASSERT((crint_validate(x) | crint_validate(y)), full_contract, {
        if (x.limbs != NULL) *x.limbs = 0; crint_free(&x); 
        if (y.limbs != NULL) *y.limbs = 0; crint_free(&y);
    });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((y.poisoned), poisoined, { crint_free(&x); crint_free(&y); }); 
    dnml_status ret_stat = CRINT_SUCCESS;
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res.limbs[i])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((src1), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x.limbs[i])), ((uintptr_t)&fake_src1));
        CHOOSE_OPTION((src2), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&y.limbs[i])), ((uintptr_t)&fake_src2));
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src1), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src2), (0));
        *dst = ~((*src1) & (*src2)); a = 0; b = 0;
    }
    /* Normalization + Masking to Invalidity */
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)res.limbs), ((uintptr_t)0));
    res.n = op_range & mask; res.cap = op_range & mask; res.sign = (int64_t)((x.sign | y.sign) & mask);
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON)); // Masked to invalidity on non-SUCCESS cases
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; // Setting up fake, pseudo-nops buffer for normalization
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)&res), ((uintptr_t)&fake_normalized))
    crint_normalize(norm_crint); /**/ if (err != NULL) *err= ret_stat;
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    ret_stat = 0; op_range = 0; new_stat = 0; mask = 0; norm_crint = 0;
    fake_dst = 0; fake_src1 = 0; fake_src2 = 0; dst = 0; src1 = 0; src2 = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return res;
}
crint crint_or(crint x, crint y, dnml_status *err) {
    DNML_ASSERT((crint_validate(x) | crint_validate(y)), full_contract, {
        if (x.limbs != NULL) *x.limbs = 0; crint_free(&x); 
        if (y.limbs != NULL) *y.limbs = 0; crint_free(&y);
    });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((y.poisoned), poisoined, { crint_free(&x); crint_free(&y); }); 
    dnml_status ret_stat = CRINT_SUCCESS;
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res.limbs[i])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((src1), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x.limbs[i])), ((uintptr_t)&fake_src1));
        CHOOSE_OPTION((src2), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&y.limbs[i])), ((uintptr_t)&fake_src2));
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src1), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src2), (0));
        *dst = ((*src1) | (*src2)); a = 0; b = 0;
    }
    /* Normalization + Masking to Invalidity */
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)res.limbs), ((uintptr_t)0));
    res.n = op_range & mask; res.cap = op_range & mask; res.sign = (int64_t)((x.sign | y.sign) & mask);
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON)); // Masked to invalidity on non-SUCCESS cases
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; // Setting up fake, pseudo-nops buffer for normalization
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)&res), ((uintptr_t)&fake_normalized))
    crint_normalize(norm_crint); /**/ if (err != NULL) *err= ret_stat;
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    ret_stat = 0; op_range = 0; new_stat = 0; mask = 0; norm_crint = 0;
    fake_dst = 0; fake_src1 = 0; fake_src2 = 0; dst = 0; src1 = 0; src2 = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return res;
}
crint crint_nor(crint x, crint y, dnml_status *err) {
    DNML_ASSERT((crint_validate(x) | crint_validate(y)), full_contract, {
        if (x.limbs != NULL) *x.limbs = 0; crint_free(&x); 
        if (y.limbs != NULL) *y.limbs = 0; crint_free(&y);
    });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((y.poisoned), poisoined, { crint_free(&x); crint_free(&y); }); 
    dnml_status ret_stat = CRINT_SUCCESS;
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res.limbs[i])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((src1), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x.limbs[i])), ((uintptr_t)&fake_src1));
        CHOOSE_OPTION((src2), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&y.limbs[i])), ((uintptr_t)&fake_src2));
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src1), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src2), (0));
        *dst = ~((*src1) | (*src2)); a = 0; b = 0;
    }
    /* Normalization + Masking to Invalidity */
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)res.limbs), ((uintptr_t)0));
    res.n = op_range & mask; res.cap = op_range & mask; res.sign = (int64_t)((x.sign | y.sign) & mask);
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON)); // Masked to invalidity on non-SUCCESS cases
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; // Setting up fake, pseudo-nops buffer for normalization
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)&res), ((uintptr_t)&fake_normalized))
    crint_normalize(norm_crint); /**/ if (err != NULL) *err= ret_stat;
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    ret_stat = 0; op_range = 0; new_stat = 0; mask = 0; norm_crint = 0;
    fake_dst = 0; fake_src1 = 0; fake_src2 = 0; dst = 0; src1 = 0; src2 = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return res;
}
crint crint_xor(crint x, crint y, dnml_status *err) {
    DNML_ASSERT((crint_validate(x) | crint_validate(y)), full_contract, {
        if (x.limbs != NULL) *x.limbs = 0; crint_free(&x); 
        if (y.limbs != NULL) *y.limbs = 0; crint_free(&y);
    });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((y.poisoned), poisoined, { crint_free(&x); crint_free(&y); }); 
    dnml_status ret_stat = CRINT_SUCCESS;
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res.limbs[i])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((src1), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x.limbs[i])), ((uintptr_t)&fake_src1));
        CHOOSE_OPTION((src2), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&y.limbs[i])), ((uintptr_t)&fake_src2));
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src1), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src2), (0));
        *dst = ((*src1) ^ (*src2)); a = 0; b = 0;
    }
    /* Normalization + Masking to Invalidity */
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)res.limbs), ((uintptr_t)0));
    res.n = op_range & mask; res.cap = op_range & mask; res.sign = (int64_t)((x.sign | y.sign) & mask);
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON)); // Masked to invalidity on non-SUCCESS cases
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; // Setting up fake, pseudo-nops buffer for normalization
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)&res), ((uintptr_t)&fake_normalized))
    crint_normalize(norm_crint); /**/ if (err != NULL) *err= ret_stat;
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    ret_stat = 0; op_range = 0; new_stat = 0; mask = 0; norm_crint = 0;
    fake_dst = 0; fake_src1 = 0; fake_src2 = 0; dst = 0; src1 = 0; src2 = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return res;
}
crint crint_xnor(crint x, crint y, dnml_status *err) {
    DNML_ASSERT((crint_validate(x) | crint_validate(y)), full_contract, {
        if (x.limbs != NULL) *x.limbs = 0; crint_free(&x); 
        if (y.limbs != NULL) *y.limbs = 0; crint_free(&y);
    });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((y.poisoned), poisoined, { crint_free(&x); crint_free(&y); }); 
    dnml_status ret_stat = CRINT_SUCCESS;
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res.limbs[i])), ((uintptr_t)&fake_dst));
        CHOOSE_OPTION((src1), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x.limbs[i])), ((uintptr_t)&fake_src1));
        CHOOSE_OPTION((src2), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&y.limbs[i])), ((uintptr_t)&fake_src2));
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src1), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src2), (0));
        *dst = ~((*src1) ^ (*src2)); a = 0; b = 0;
    }
    /* Normalization + Masking to Invalidity */
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)res.limbs), ((uintptr_t)0));
    res.n = op_range & mask; res.cap = op_range & mask; res.sign = (int64_t)((x.sign | y.sign) & mask);
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON)); // Masked to invalidity on non-SUCCESS cases
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; // Setting up fake, pseudo-nops buffer for normalization
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)&res), ((uintptr_t)&fake_normalized))
    crint_normalize(norm_crint); /**/ if (err != NULL) *err= ret_stat;
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    ret_stat = 0; op_range = 0; new_stat = 0; mask = 0; norm_crint = 0;
    fake_dst = 0; fake_src1 = 0; fake_src2 = 0; dst = 0; src1 = 0; src2 = 0;
    fake_normalized.limbs = 0; fake_normalized.n = 0; fake_normalized.cap = 0;
    fake_normalized.sign = 0; fake_normalized.poisoned = 0; return res;
}




//* ================================= FUNCTION, EXPLICIT WIDTH BITWISE OPERATION ================================== */
crint crint_ex_andu64(crint x, const uint64_t val, size_t op_range, dnml_status *err) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    uint64_t first_limb = x.limbs[0]; CHOOSE_OPTION((first_limb), (x.n), (first_limb), (0)); first_limb = first_limb & val;
    /* Allocation of Result */
    crint res; dnml_status new_stat = crint_snew(&res, op_range);
    CHOOSE_OPTION((ret_stat), (
        _lib_crt_eq(new_stat, DNML_ALLOC_OOM) & 
        _lib_crt_eq(ret_stat, CRINT_SUCCESS)), 
        (DNML_ALLOC_OOM), (ret_stat)
    );
    uint64_t mask = (uint64_t)(-(int64_t)(ret_stat == CRINT_SUCCESS)); limb_t tmp_limb[1] = {0};
    /* Actual Bitwise Operation */ int8_t ret_sign;
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(new_stat, DNML_ALLOC_OOM)), ((uintptr_t)tmp_limb), ((uintptr_t)res.limbs));
    CHOOSE_OPTION((res.limbs[0]), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), (first_limb), (0));
    CHOOSE_OPTION((res.n), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), (!!(first_limb)), (0));
    CHOOSE_OPTION((ret_sign), (first_limb), (x.sign), (1));
    CHOOSE_OPTION((res.sign), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), (ret_sign), (0));
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)res.limbs), ((uintptr_t)0));
    res.cap = op_range & mask; res.poisoned = (x.poisoned); /**/ if (err != NULL) *err = ret_stat; // clang-format off
    ret_stat = 0; first_limb = 0; new_stat = 0; mask = 0; tmp_limb[0] = 0; return res; // clang-format on
}
crint crint_ex_nandu64(crint x, const uint64_t val, size_t op_range, dnml_status *err) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS;
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res.limbs[i])), ((uintptr_t)(&fake_dst)));
        CHOOSE_OPTION((src), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x.limbs[sindex])), ((uintptr_t)(&fake_src)));
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src), (0));
        uint64_t b; CHOOSE_OPTION((b), (!i), (val), (0));
        *dst = ~(a & b); sindex = 0; a = 0; b = 0;
    }
    /* Masking to Invalidity + Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)&res), ((uintptr_t)&fake_normalized));
    crint_normalize(norm_crint);
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)res.limbs), ((uintptr_t)0));
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask)); res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON)); /**/ if (err != NULL) *err = ret_stat;
    /* Aggeressive Post-operation Cleanup */ // clang-format off
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0;
    norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.cap = 0; fake_normalized.n = 0;
    fake_normalized.poisoned = 0; fake_normalized.sign = 0; return res; // clang-format on
}
crint crint_ex_oru64(crint x, const uint64_t val, size_t op_range, dnml_status *err) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS;
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res.limbs[i])), ((uintptr_t)(&fake_dst)));
        CHOOSE_OPTION((src), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x.limbs[sindex])), ((uintptr_t)(&fake_src)));
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src), (0));
        uint64_t b; CHOOSE_OPTION((b), (!i), (val), (0));
        *dst = (a | b); sindex = 0; a = 0; b = 0;
    }
    /* Masking to Invalidity + Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)&res), ((uintptr_t)&fake_normalized));
    crint_normalize(norm_crint);
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)res.limbs), ((uintptr_t)0));
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask)); res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON)); /**/ if (err != NULL) *err = ret_stat;
    /* Aggeressive Post-operation Cleanup */ // clang-format off
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0;
    norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.cap = 0; fake_normalized.n = 0;
    fake_normalized.poisoned = 0; fake_normalized.sign = 0; return res; // clang-format on
}
crint crint_ex_noru64(crint x, const uint64_t val, size_t op_range, dnml_status *err) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS;
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res.limbs[i])), ((uintptr_t)(&fake_dst)));
        CHOOSE_OPTION((src), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x.limbs[sindex])), ((uintptr_t)(&fake_src)));
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src), (0));
        uint64_t b; CHOOSE_OPTION((b), (!i), (val), (0));
        *dst = ~(a | b); sindex = 0; a = 0; b = 0;
    }
    /* Masking to Invalidity + Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)&res), ((uintptr_t)&fake_normalized));
    crint_normalize(norm_crint);
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)res.limbs), ((uintptr_t)0));
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask)); res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON)); /**/ if (err != NULL) *err = ret_stat;
    /* Aggeressive Post-operation Cleanup */ // clang-format off
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0;
    norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.cap = 0; fake_normalized.n = 0;
    fake_normalized.poisoned = 0; fake_normalized.sign = 0; return res; // clang-format on
}
crint crint_ex_xoru64(crint x, const uint64_t val, size_t op_range, dnml_status *err) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS;
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res.limbs[i])), ((uintptr_t)(&fake_dst)));
        CHOOSE_OPTION((src), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x.limbs[sindex])), ((uintptr_t)(&fake_src)));
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src), (0));
        uint64_t b; CHOOSE_OPTION((b), (!i), (val), (0));
        *dst = (a ^ b); sindex = 0; a = 0; b = 0;
    }
    /* Masking to Invalidity + Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)&res), ((uintptr_t)&fake_normalized));
    crint_normalize(norm_crint);
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)res.limbs), ((uintptr_t)0));
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask)); res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON)); /**/ if (err != NULL) *err = ret_stat;
    /* Aggeressive Post-operation Cleanup */ // clang-format off
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0;
    norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.cap = 0; fake_normalized.n = 0;
    fake_normalized.poisoned = 0; fake_normalized.sign = 0; return res; // clang-format on
}
crint crint_ex_xnoru64(crint x, const uint64_t val, size_t op_range, dnml_status *err) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS;
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res.limbs[i])), ((uintptr_t)(&fake_dst)));
        CHOOSE_OPTION((src), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x.limbs[sindex])), ((uintptr_t)(&fake_src)));
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src), (0));
        uint64_t b; CHOOSE_OPTION((b), (!i), (val), (0));
        *dst = ~(a ^ b); sindex = 0; a = 0; b = 0;
    }
    /* Masking to Invalidity + Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)&res), ((uintptr_t)&fake_normalized));
    crint_normalize(norm_crint);
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)res.limbs), ((uintptr_t)0));
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask)); res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON)); /**/ if (err != NULL) *err = ret_stat;
    /* Aggeressive Post-operation Cleanup */ // clang-format off
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0;
    norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.cap = 0; fake_normalized.n = 0;
    fake_normalized.poisoned = 0; fake_normalized.sign = 0; return res; // clang-format on
}
crint crint_ex_andi64(crint x, const int64_t val, size_t op_range, dnml_status *err) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS;
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
    uint64_t extension_bits, mag_val = __MAG_I64__(val);
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    CHOOSE_OPTION((extension_bits), (_lib_crt_isneg(val)), (UINT64_MAX), (0));
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        size_t sindex; CHOOSE_OPTION((sindex), (_lib_crt_lt(i, x.n)), (i), (0));
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res.limbs[i])), ((uintptr_t)(&fake_dst)));
        CHOOSE_OPTION((src), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x.limbs[sindex])), ((uintptr_t)(&fake_src)));
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src), (0));
        uint64_t b; CHOOSE_OPTION((b), (!i), (mag_val), (extension_bits));
        *dst = (a & b); sindex = 0; a = 0; b = 0;
    }
    /* Masking to Invalidity + Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)&res), ((uintptr_t)&fake_normalized));
    crint_normalize(norm_crint);
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)res.limbs), ((uintptr_t)0));
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask)); res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON)); /**/ if (err != NULL) *err = ret_stat;
    /* Aggeressive Post-operation Cleanup */ // clang-format off
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; mag_val = 0;
    extension_bits = 0; norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.cap = 0; 
    fake_normalized.n = 0; fake_normalized.poisoned = 0; fake_normalized.sign = 0; return res; // clang-format on
}
crint crint_ex_nandi64(crint x, const int64_t val, size_t op_range, dnml_status *err) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS;
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
    uint64_t extension_bits, mag_val = __MAG_I64__(val);
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    CHOOSE_OPTION((extension_bits), (_lib_crt_isneg(val)), (UINT64_MAX), (0));
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        size_t sindex; CHOOSE_OPTION((sindex), (_lib_crt_lt(i, x.n)), (i), (0));
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res.limbs[i])), ((uintptr_t)(&fake_dst)));
        CHOOSE_OPTION((src), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x.limbs[sindex])), ((uintptr_t)(&fake_src)));
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src), (0));
        uint64_t b; CHOOSE_OPTION((b), (!i), (mag_val), (extension_bits));
        *dst = ~(a & b); sindex = 0; a = 0; b = 0;
    }
    /* Masking to Invalidity + Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)&res), ((uintptr_t)&fake_normalized));
    crint_normalize(norm_crint);
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)res.limbs), ((uintptr_t)0));
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask)); res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON)); /**/ if (err != NULL) *err = ret_stat;
    /* Aggeressive Post-operation Cleanup */ // clang-format off
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; mag_val = 0;
    extension_bits = 0; norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.cap = 0; 
    fake_normalized.n = 0; fake_normalized.poisoned = 0; fake_normalized.sign = 0; return res; // clang-format on
}
crint crint_ex_ori64(crint x, const int64_t val, size_t op_range, dnml_status *err) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS;
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
    uint64_t extension_bits, mag_val = __MAG_I64__(val);
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    CHOOSE_OPTION((extension_bits), (_lib_crt_isneg(val)), (UINT64_MAX), (0));
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        size_t sindex; CHOOSE_OPTION((sindex), (_lib_crt_lt(i, x.n)), (i), (0));
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res.limbs[i])), ((uintptr_t)(&fake_dst)));
        CHOOSE_OPTION((src), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x.limbs[sindex])), ((uintptr_t)(&fake_src)));
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src), (0));
        uint64_t b; CHOOSE_OPTION((b), (!i), (mag_val), (extension_bits));
        *dst = (a | b); sindex = 0; a = 0; b = 0;
    }
    /* Masking to Invalidity + Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)&res), ((uintptr_t)&fake_normalized));
    crint_normalize(norm_crint);
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)res.limbs), ((uintptr_t)0));
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask)); res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON)); /**/ if (err != NULL) *err = ret_stat;
    /* Aggeressive Post-operation Cleanup */ // clang-format off
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; mag_val = 0;
    extension_bits = 0; norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.cap = 0; 
    fake_normalized.n = 0; fake_normalized.poisoned = 0; fake_normalized.sign = 0; return res; // clang-format on
}
crint crint_ex_nori64(crint x, const int64_t val, size_t op_range, dnml_status *err) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS;
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
    uint64_t extension_bits, mag_val = __MAG_I64__(val);
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    CHOOSE_OPTION((extension_bits), (_lib_crt_isneg(val)), (UINT64_MAX), (0));
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        size_t sindex; CHOOSE_OPTION((sindex), (_lib_crt_lt(i, x.n)), (i), (0));
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res.limbs[i])), ((uintptr_t)(&fake_dst)));
        CHOOSE_OPTION((src), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x.limbs[sindex])), ((uintptr_t)(&fake_src)));
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src), (0));
        uint64_t b; CHOOSE_OPTION((b), (!i), (mag_val), (extension_bits));
        *dst = ~(a | b); sindex = 0; a = 0; b = 0;
    }
    /* Masking to Invalidity + Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)&res), ((uintptr_t)&fake_normalized));
    crint_normalize(norm_crint);
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)res.limbs), ((uintptr_t)0));
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask)); res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON)); /**/ if (err != NULL) *err = ret_stat;
    /* Aggeressive Post-operation Cleanup */ // clang-format off
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; mag_val = 0;
    extension_bits = 0; norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.cap = 0; 
    fake_normalized.n = 0; fake_normalized.poisoned = 0; fake_normalized.sign = 0; return res; // clang-format on
}
crint crint_ex_xori64(crint x, const int64_t val, size_t op_range, dnml_status *err) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS;
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
    uint64_t extension_bits, mag_val = __MAG_I64__(val);
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    CHOOSE_OPTION((extension_bits), (_lib_crt_isneg(val)), (UINT64_MAX), (0));
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        size_t sindex; CHOOSE_OPTION((sindex), (_lib_crt_lt(i, x.n)), (i), (0));
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res.limbs[i])), ((uintptr_t)(&fake_dst)));
        CHOOSE_OPTION((src), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x.limbs[sindex])), ((uintptr_t)(&fake_src)));
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src), (0));
        uint64_t b; CHOOSE_OPTION((b), (!i), (mag_val), (extension_bits));
        *dst = (a ^ b); sindex = 0; a = 0; b = 0;
    }
    /* Masking to Invalidity + Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)&res), ((uintptr_t)&fake_normalized));
    crint_normalize(norm_crint);
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)res.limbs), ((uintptr_t)0));
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask)); res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON)); /**/ if (err != NULL) *err = ret_stat;
    /* Aggeressive Post-operation Cleanup */ // clang-format off
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; mag_val = 0;
    extension_bits = 0; norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.cap = 0; 
    fake_normalized.n = 0; fake_normalized.poisoned = 0; fake_normalized.sign = 0; return res; // clang-format on
}
crint crint_ex_xnori64(crint x, const int64_t val, size_t op_range, dnml_status *err) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS;
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
    uint64_t extension_bits, mag_val = __MAG_I64__(val);
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(new_stat, CRINT_SUCCESS)));
    CHOOSE_OPTION((extension_bits), (_lib_crt_isneg(val)), (UINT64_MAX), (0));
    for (size_t i = 0; _lib_crt_lt(i, op_range); ++i) {
        size_t sindex; CHOOSE_OPTION((sindex), (_lib_crt_lt(i, x.n)), (i), (0));
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res.limbs[i])), ((uintptr_t)(&fake_dst)));
        CHOOSE_OPTION((src), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x.limbs[sindex])), ((uintptr_t)(&fake_src)));
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src), (0));
        uint64_t b; CHOOSE_OPTION((b), (!i), (mag_val), (extension_bits));
        *dst = ~(a ^ b); sindex = 0; a = 0; b = 0;
    }
    /* Masking to Invalidity + Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)&res), ((uintptr_t)&fake_normalized));
    crint_normalize(norm_crint);
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)res.limbs), ((uintptr_t)0));
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask)); res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON)); /**/ if (err != NULL) *err = ret_stat;
    /* Aggeressive Post-operation Cleanup */ // clang-format off
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src = 0; dst = 0; src = 0; mag_val = 0;
    extension_bits = 0; norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.cap = 0; 
    fake_normalized.n = 0; fake_normalized.poisoned = 0; fake_normalized.sign = 0; return res; // clang-format on
}
crint crint_ex_and(crint x, crint y, size_t op_range, dnml_status *err) {
    DNML_ASSERT((crint_validate(x) & crint_validate(y)), full_contract, { 
        if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); 
        if (y.limbs != NULL) *(y.limbs) = 0; crint_free(&y);
    });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((y.poisoned), poisoined, { crint_free(&x); crint_free(&y); }); dnml_status ret_stat = CRINT_SUCCESS;
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res.limbs[i])), ((uintptr_t)(&fake_dst)));
        CHOOSE_OPTION((src1), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x.limbs[sindex1])), ((uintptr_t)(&fake_src1)));
        CHOOSE_OPTION((src2), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&y.limbs[sindex2])), ((uintptr_t)(&fake_src1)));
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src1), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src2), (0));
        *dst = (a & b); sindex1 = 0; sindex2 = 0; a = 0; b = 0;
    }
    /* Masking to Invalidity + Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)&res), ((uintptr_t)&fake_normalized));
    crint_normalize(norm_crint);
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)res.limbs), ((uintptr_t)0));
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask)); res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON)); /**/ if (err != NULL) *err = ret_stat;
    /* Aggeressive Post-operation Cleanup */ // clang-format off
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src1 = 0; fake_src2 = 0; dst = 0; 
    src1 = 0; src2 = 0; norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.cap = 0; 
    fake_normalized.n = 0; fake_normalized.poisoned = 0; fake_normalized.sign = 0; return res; // clang-format on
}
crint crint_ex_nand(crint x, crint y, size_t op_range, dnml_status *err) {
    DNML_ASSERT((crint_validate(x) & crint_validate(y)), full_contract, { 
        if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); 
        if (y.limbs != NULL) *(y.limbs) = 0; crint_free(&y);
    });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((y.poisoned), poisoined, { crint_free(&x); crint_free(&y); }); dnml_status ret_stat = CRINT_SUCCESS;
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res.limbs[i])), ((uintptr_t)(&fake_dst)));
        CHOOSE_OPTION((src1), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x.limbs[sindex1])), ((uintptr_t)(&fake_src1)));
        CHOOSE_OPTION((src2), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&y.limbs[sindex2])), ((uintptr_t)(&fake_src1)));
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src1), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src2), (0));
        *dst = ~(a & b); sindex1 = 0; sindex2 = 0; a = 0; b = 0;
    }
    /* Masking to Invalidity + Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)&res), ((uintptr_t)&fake_normalized));
    crint_normalize(norm_crint);
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)res.limbs), ((uintptr_t)0));
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask)); res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON)); /**/ if (err != NULL) *err = ret_stat;
    /* Aggeressive Post-operation Cleanup */ // clang-format off
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src1 = 0; fake_src2 = 0; dst = 0; 
    src1 = 0; src2 = 0; norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.cap = 0; 
    fake_normalized.n = 0; fake_normalized.poisoned = 0; fake_normalized.sign = 0; return res; // clang-format on
}
crint crint_ex_or(crint x, crint y, size_t op_range, dnml_status *err) {
    DNML_ASSERT((crint_validate(x) & crint_validate(y)), full_contract, { 
        if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); 
        if (y.limbs != NULL) *(y.limbs) = 0; crint_free(&y);
    });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((y.poisoned), poisoined, { crint_free(&x); crint_free(&y); }); dnml_status ret_stat = CRINT_SUCCESS;
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res.limbs[i])), ((uintptr_t)(&fake_dst)));
        CHOOSE_OPTION((src1), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x.limbs[sindex1])), ((uintptr_t)(&fake_src1)));
        CHOOSE_OPTION((src2), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&y.limbs[sindex2])), ((uintptr_t)(&fake_src1)));
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src1), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src2), (0));
        *dst = (a | b); sindex1 = 0; sindex2 = 0; a = 0; b = 0;
    }
    /* Masking to Invalidity + Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)&res), ((uintptr_t)&fake_normalized));
    crint_normalize(norm_crint);
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)res.limbs), ((uintptr_t)0));
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask)); res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON)); /**/ if (err != NULL) *err = ret_stat;
    /* Aggeressive Post-operation Cleanup */ // clang-format off
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src1 = 0; fake_src2 = 0; dst = 0; 
    src1 = 0; src2 = 0; norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.cap = 0; 
    fake_normalized.n = 0; fake_normalized.poisoned = 0; fake_normalized.sign = 0; return res; // clang-format on
}
crint crint_ex_nor(crint x, crint y, size_t op_range, dnml_status *err) {
    DNML_ASSERT((crint_validate(x) & crint_validate(y)), full_contract, { 
        if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); 
        if (y.limbs != NULL) *(y.limbs) = 0; crint_free(&y);
    });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((y.poisoned), poisoined, { crint_free(&x); crint_free(&y); }); dnml_status ret_stat = CRINT_SUCCESS;
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res.limbs[i])), ((uintptr_t)(&fake_dst)));
        CHOOSE_OPTION((src1), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x.limbs[sindex1])), ((uintptr_t)(&fake_src1)));
        CHOOSE_OPTION((src2), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&y.limbs[sindex2])), ((uintptr_t)(&fake_src1)));
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src1), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src2), (0));
        *dst = ~(a | b); sindex1 = 0; sindex2 = 0; a = 0; b = 0;
    }
    /* Masking to Invalidity + Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)&res), ((uintptr_t)&fake_normalized));
    crint_normalize(norm_crint);
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)res.limbs), ((uintptr_t)0));
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask)); res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON)); /**/ if (err != NULL) *err = ret_stat;
    /* Aggeressive Post-operation Cleanup */ // clang-format off
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src1 = 0; fake_src2 = 0; dst = 0; 
    src1 = 0; src2 = 0; norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.cap = 0; 
    fake_normalized.n = 0; fake_normalized.poisoned = 0; fake_normalized.sign = 0; return res; // clang-format on
}
crint crint_ex_xor(crint x, crint y, size_t op_range, dnml_status *err) {
    DNML_ASSERT((crint_validate(x) & crint_validate(y)), full_contract, { 
        if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); 
        if (y.limbs != NULL) *(y.limbs) = 0; crint_free(&y);
    });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((y.poisoned), poisoined, { crint_free(&x); crint_free(&y); }); dnml_status ret_stat = CRINT_SUCCESS;
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res.limbs[i])), ((uintptr_t)(&fake_dst)));
        CHOOSE_OPTION((src1), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x.limbs[sindex1])), ((uintptr_t)(&fake_src1)));
        CHOOSE_OPTION((src2), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&y.limbs[sindex2])), ((uintptr_t)(&fake_src1)));
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src1), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src2), (0));
        *dst = (a ^ b); sindex1 = 0; sindex2 = 0; a = 0; b = 0;
    }
    /* Masking to Invalidity + Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)&res), ((uintptr_t)&fake_normalized));
    crint_normalize(norm_crint);
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)res.limbs), ((uintptr_t)0));
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask)); res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON)); /**/ if (err != NULL) *err = ret_stat;
    /* Aggeressive Post-operation Cleanup */ // clang-format off
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src1 = 0; fake_src2 = 0; dst = 0; 
    src1 = 0; src2 = 0; norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.cap = 0; 
    fake_normalized.n = 0; fake_normalized.poisoned = 0; fake_normalized.sign = 0; return res; // clang-format on
}
crint crint_ex_xnor(crint x, crint y, size_t op_range, dnml_status *err) {
    DNML_ASSERT((crint_validate(x) & crint_validate(y)), full_contract, { 
        if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); 
        if (y.limbs != NULL) *(y.limbs) = 0; crint_free(&y);
    });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((y.poisoned), poisoined, { crint_free(&x); crint_free(&y); }); dnml_status ret_stat = CRINT_SUCCESS;
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
        CHOOSE_OPTION((dst), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&res.limbs[i])), ((uintptr_t)(&fake_dst)));
        CHOOSE_OPTION((src1), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&x.limbs[sindex1])), ((uintptr_t)(&fake_src1)));
        CHOOSE_OPTION((src2), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)(&y.limbs[sindex2])), ((uintptr_t)(&fake_src1)));
        uint64_t a; CHOOSE_OPTION((a), (_lib_crt_lt(i, x.n)), (*src1), (0));
        uint64_t b; CHOOSE_OPTION((b), (_lib_crt_lt(i, y.n)), (*src2), (0));
        *dst = ~(a ^ b); sindex1 = 0; sindex2 = 0; a = 0; b = 0;
    }
    /* Masking to Invalidity + Normalization */
    limb_t fake_buf[FAKE_BUF_CAP] = {0}; crint* norm_crint; res.n = op_range;
    crint fake_normalized = { .limbs = fake_buf, .n = FAKE_BUF_CAP, .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((norm_crint), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)&res), ((uintptr_t)&fake_normalized));
    crint_normalize(norm_crint);
    CHOOSE_OPTION((res.limbs), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((uintptr_t)res.limbs), ((uintptr_t)0));
    CHOOSE_OPTION((res.sign), (res.n), (x.sign & mask), (1 & mask)); res.n &= mask; res.cap = op_range & mask;
    res.poisoned = (_lib_crt_eq(ret_stat, CRINT_POISON)); /**/ if (err != NULL) *err = ret_stat;
    /* Aggeressive Post-operation Cleanup */ // clang-format off
    ret_stat = 0; new_stat = 0; fake_dst = 0; fake_src1 = 0; fake_src2 = 0; dst = 0; 
    src1 = 0; src2 = 0; norm_crint = 0; fake_normalized.limbs = 0; fake_normalized.cap = 0; 
    fake_normalized.n = 0; fake_normalized.poisoned = 0; fake_normalized.sign = 0; return res; // clang-format on
}



