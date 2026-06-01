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
    ); __CRINT_ADD_WC__(res, a, b);
}
void __CRINT_MAGSUB__(crint *res, crint *a, crint *b) {
    _pre_assert(res, { crint_free(res); crint_free(a); crint_free(b); });
    _pre_assert(a, { crint_free(res); crint_free(a); crint_free(b); });
    _pre_assert(b, { crint_free(res); crint_free(a); crint_free(b); });
    DNML_TEST_ASSERT((__CRINT_INTERNAL_CMP__(a, b) != -1),
        "Subtraction Underflow: Subtrahend's magnitude is too large for Minuend"
        " (-Esub_underflow)", { crint_free(res); crint_free(a); crint_free(b); }
    ); __CRINT_SUB_WC__(res, a, b);
}
void __CRINT_MAGMUL___(crint *res, crint *a, crint *b) {
    _pre_assert(res, { crint_free(res); crint_free(a); crint_free(b); });
    _pre_assert(a, { crint_free(res); crint_free(a); crint_free(b); });
    _pre_assert(b, { crint_free(res); crint_free(a); crint_free(b); });
    DNML_TEST_ASSERT(
        (_lib_crt_geq(res->cap, a->n + b->n)), // Result Capacity >= Sum of multiplicand's length
        "Insufficient Product Capacity: Capacity insatisfactory for a * b (-Emul_insufficient_cap)",
        { crint_free(res); crint_free(a); crint_free(b); }
    ); __CRINT_MUL_DISP__(a, b, res);
}
void __CRINT_MAGDIV__(crint *quot, crint *temp_rem, crint *a, crint *b) {
    _pre_assert(quot, { crint_free(quot); crint_free(temp_rem); crint_free(a); crint_free(b); })
    _pre_assert(temp_rem, { crint_free(quot); crint_free(temp_rem); crint_free(a); crint_free(b); })
    _pre_assert(a, { crint_free(quot); crint_free(temp_rem); crint_free(a); crint_free(b); })
    _pre_assert(b, { crint_free(quot); crint_free(temp_rem); crint_free(a); crint_free(b); })
    DNML_TEST_ASSERT((b->n),
        "Mathematical Undefinindness: Division by 0 (-Ediv_by_zero)", 
        { crint_free(quot); crint_free(temp_rem); crint_free(a); crint_free(b); }
    );
    DNML_TEST_ASSERT((quot->cap >= a->n),
        "Insufficient Quotient Capacity: Capacity unsatisfactory for a / b (-Ediv_insufficient_qcap)",
        { crint_free(quot); crint_free(temp_rem); crint_free(a); crint_free(b); }
    );
    DNML_TEST_ASSERT((temp_rem->cap >= b->n),
        "Insufficient Remainder Capacity: Capacity unsatisfactory for a mod(b) (-Ediv_insufficient_rcap)",
        { crint_free(quot); crint_free(temp_rem); crint_free(a); crint_free(b); }
    ); __CRINT_DIV_DISP__(a, b, quot, temp_rem);
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
    ); __CRINT_MOD_DISP__(a, b, rem, temp_quot);
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
    carry = 0; lo = 0; hi = 0; sum = 0; curr = 0; val = 0; // clang-format on
}
void __CRINT_MAGDIVMOD_U64__(crint *quot, crint *rem, crint *a, uint64_t val) {
    DNML_TEST_ASSERT( /* Check for a zero divisor */
        val, "Mathematical Undefinindness: Division by 0 (-Ediv_by_zero)",
        { crint_free(quot); crint_free(rem); crint(a); val = 0; }
    );
    DNML_TEST_ASSERT((quot->cap >= a->n),
        "Insufficient Quotient Capacity: Capacity unsatisfactory for a / b (-Ediv_insufficient_qcap)",
        { crint_free(quot); crint_free(rem); crint_free(a); val = 0; }
    );
    DNML_TEST_ASSERT((rem->cap >= 1),
        "Insufficient Remainder Capacity: Capacity unsatisfactory for a mod(b) (-Ediv_insufficient_rcap)",
        { crint_free(quot); crint_free(rem); crint_free(a); val = 0; }
    ); __CRINT_SHORT_DIVISION__(a, val, quot, rem);
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
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, NULL)), pointer_null, {});
    DNML_TEST_ASSERT((crint_pvalidate(x)), full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((x->poisoned), crint_poisoned, { crint_free(x); });
    if (_lib_crt_eq((ptr_t)x, NULL)) { x = 0; val = 0; return CRINT_NULL; }
    if (!crint_pvalidate(x)) { x = 0; val = 0; return CRINT_ERR_INVAL; }
    /* Actual Operation */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    dnml_status reserve_stat = crint_reserve(x, x->n + 1);
    CHOOSE_OPTION((ret_stat), 
        (_lib_crt_eq(reserve_stat, DNML_ALLOC_OOM) & 
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), 
        (DNML_ALLOC_OOM), (ret_stat)
    );
    /* Main Operation */
    crint tmp_prod; dnml_status snew_stat = crint_snew(&tmp_prod, x->n + 1);
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(snew_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (ret_stat)
    ); crint *src, *prod; limb_t *prev_src_buf;
    limb_t fsbuf[FAKE_BUF_CAP] = {UINT64_MAX}, fpbuf[FAKE_BUF_CAP + 1] = {0}, fake_mul = UINT64_C(0x123456789ABCDEF0);
    crint fake_source = { .limbs = fsbuf, .cap = FAKE_BUF_CAP, .n = FAKE_BUF_CAP, .sign = 1, .poisoned = false };
    crint fake_prod = { .limbs = fpbuf, .cap = FAKE_BUF_CAP + 1, .n = 0, .sign = 1, .poisoned = false };
    CHOOSE_OPTION((src), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((ptr_t)x), ((ptr_t)&fake_source));
    CHOOSE_OPTION((prod), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((ptr_t)&tmp_prod), ((ptr_t)&fake_prod));
    __CRINT_MAGMUL_U64__(prod, src, _lib_crt_select(_lib_crt_eq(ret_stat, CRINT_SUCCESS), val, fake_mul));
    CHOOSE_OPTION((prev_src_buf), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), ((ptr_t)x->limbs), ((ptr_t)NULL));
    crint_transfer(src, prod); free(prev_src_buf); // Would lead to a standard no-op
    /* Post-operation Aggressive Cleanup */ // clang-format off
    __libdnml_smemwipe_u64(fsbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(fpbuf, FAKE_BUF_CAP + 1, 0, FAKE_BUF_CAP, false);
    reserve_stat = 0; pbv_crint_clear(tmp_prod); snew_stat = 0; src = 0; 
    prod = 0; prev_src_buf = 0; fake_mul = 0; pbv_crint_clear(fake_source); 
    pbv_crint_clear(fake_prod); x = 0; val = 0; return ret_stat;
}
dnml_status crint_mut_divu64(crint *x, uint64_t val) {}
dnml_status crint_mut_modu64(crint *x, uint64_t val) {}
dnml_status crint_mut_muli64(crint *x, int64_t val) {}
dnml_status crint_mut_divi64(crint *x, int64_t val) {}
dnml_status crint_mut_modi64(crint *x, int64_t val) {}
dnml_status crint_mut_add(crint *x, crint y) {}
dnml_status crint_mut_sub(crint *x, crint y) {}
dnml_status crint_mut_mul(crint *x, crint y) {}
dnml_status crint_mut_div(crint *x, crint y) {}
dnml_status crint_mut_mod(crint *x, crint y) {}
/* ------------------ FUNCTIONAL ARITHMETIC ------------------- */
crint crint_mulu64(crint x, uint64_t val, dnml_status *err) {}
crint crint_divu64(crint x, uint64_t val, dnml_status *err) {}
crint crint_modu64(crint x, uint64_t val, dnml_status *err) {}
crint crint_muli64(crint x, int64_t val, dnml_status *err) {}
crint crint_divi64(crint x, int64_t val, dnml_status *err) {}
crint crint_modi64(crint x, int64_t val, dnml_status *err) {}
crint crint_add(crint x, crint y, dnml_status *err) {}
crint crint_sub(crint x, crint y, dnml_status *err) {}
crint crint_mul(crint x, crint y, dnml_status *err) {}
crint crint_div(crint x, crint y, dnml_status *err) {}
crint crint_mod(crint x, crint y, dnml_status *err) {}






