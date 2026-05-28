#include "cryptInt_func.h"
#include <stdbool.h>




//* ========================================= CONSTRUCTORS & DESTRUCTORS ===================================== *//
void crint_free(crint *x) {
    if (x->limbs != NULL) free(x->limbs);
    x->limbs = 0; x->cap = 0; x->n = 0;
    x->poisoned = 0; x->sign = 0;
}
dnml_status crint_new(crint *x) {
    uint64_t oom_mask = UINT64_MAX; 
    dnml_status ret_stat = CRINT_SUCCESS;
    limb_t *__BUFFER_P = calloc(1, U64_BYTES);
    DNML_TEST_ASSERT( __BUFFER_P != NULL, realloc_null, {});
    CHOOSE_OPTION((ret_stat), ((_lib_crt_eq(__BUFFER_P, NULL)) & (_lib_crt_eq(x->limbs, NULL))), (DNML_ALLOC_OOM), (ret_stat));
    CHOOSE_OPTION((oom_mask), ((_lib_crt_eq(__BUFFER_P, NULL)) & (_lib_crt_eq(x->limbs, NULL))), (0), (oom_mask));

    uint8_t uninit = (_lib_crt_eq(x->limbs, NULL));
    CHOOSE_OPTION((x->limbs), (uninit), ((uintptr_t)(__BUFFER_P) & oom_mask), ((uintptr_t)x->limbs));
    CHOOSE_OPTION((x->cap), (uninit), (1 & oom_mask), (x->cap));
    CHOOSE_OPTION((x->n), (uninit), (0 & oom_mask), (x->n));
    CHOOSE_OPTION((x->sign), (uninit), (1 & oom_mask), (x->sign));
    CHOOSE_OPTION((x->poisoned), (uninit), (0 & oom_mask), (x->poisoned));
    /* Post operation Aggrestive Clearance */ // clang-format off
    oom_mask = 0; __BUFFER_P = 0; uninit = 0; return ret_stat; 
    // clang-format on
}
dnml_status crint_snew(crint *x, const size_t n) {
    uint64_t oom_mask = UINT64_MAX;
    dnml_status ret_stat = CRINT_SUCCESS;
    size_t salloc; NORMALIZE_0_TO_1(salloc, n);
    limb_t *__BUFFER_P = calloc(salloc, U64_BYTES);
    DNML_TEST_ASSERT( __BUFFER_P != NULL, realloc_null, {});
    CHOOSE_OPTION((ret_stat), ((_lib_crt_eq(__BUFFER_P, NULL)) & (_lib_crt_eq(x->limbs, NULL))), (DNML_ALLOC_OOM), (ret_stat));
    CHOOSE_OPTION((oom_mask), ((_lib_crt_eq(__BUFFER_P, NULL)) & (_lib_crt_eq(x->limbs, NULL))), (0), (oom_mask));

    uint8_t uninit = (x->limbs = NULL);
    CHOOSE_OPTION((x->limbs), (uninit), ((uintptr_t)(__BUFFER_P) & oom_mask), ((uintptr_t)x->limbs));
    CHOOSE_OPTION((x->cap), (uninit), (salloc & oom_mask), (x->cap));
    CHOOSE_OPTION((x->n), (uninit), (0 & oom_mask), (x->n));
    CHOOSE_OPTION((x->sign), (uninit), (1 & oom_mask), (x->sign));
    CHOOSE_OPTION((x->poisoned), (uninit), (0 & oom_mask), (x->poisoned));
    /* Post-operation Aggrestive Clearance */ // clang-format off
    oom_mask = 0; salloc = 0; __BUFFER_P = 0; uninit = 0; return ret_stat;
    // clang-format on
}
dnml_status crint_cinew(crint *x, crint *y) {
    // Pre-operation Validation & Static Analysis
    DNML_ASSERT((crint_pvalidate(y)), full_contract,
        { /* Set everything to 0 to completely wipe out memory */
          /* We can only guarantee wiping out the first limb safely */
            if (y->limbs != NULL) *(y->limbs) = 0;
            crint_free(y); x->limbs = 0; x->n = 0;
            x->cap = 0; x->sign = 0; x->poisoned = 0;
        }
    ); DNML_TEST_ASSERT((y->poisoned), poisoined, { crint_free(y); });
    dnml_status ret_stat = CRINT_SUCCESS; uint64_t mask = UINT64_MAX; uint8_t noop_toggle = false;
    CHOOSE_OPTION((ret_stat), ((y->poisoned) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((noop_toggle), (y->poisoned), (true), (noop_toggle));

    /* Main Operations */
    size_t alloc_size; CHOOSE_OPTION((alloc_size), (y->n), (y->n), (1));
    limb_t *__BUFFER_P = calloc(alloc_size, U64_BYTES);
    DNML_TEST_ASSERT( __BUFFER_P != NULL, realloc_null, {});
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(__BUFFER_P, NULL)) & (_lib_crt_eq(x->limbs, NULL)) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), 
        (DNML_ALLOC_OOM), (ret_stat)
    );
    mask = (uint64_t)(-(int64_t)(!(
        (_lib_crt_eq(__BUFFER_P, NULL)) & (_lib_crt_eq(x->limbs, NULL)) &
        (_lib_crt_eq(mask, UINT64_MAX)) & (ret_stat != CRINT_POISON)
    )));
    uint8_t uninit = (_lib_crt_eq(x->limbs, NULL)); limb_t* dst_limbs;
    CHOOSE_OPTION((dst_limbs), (uninit), ((uintptr_t)__BUFFER_P), ((uintptr_t)y->limbs));
    size_t end; CHOOSE_OPTION((end), (!y->n), (0), (y->n));
    __libdnml_smemcpy_u64(dst_limbs, y->limbs, alloc_size, y->n, 0, end, false);
    CHOOSE_OPTION((x->limbs), (uninit), ((uintptr_t)(__BUFFER_P) & mask), ((uintptr_t)x->limbs));
    CHOOSE_OPTION((x->cap), (uninit), (alloc_size & mask), (x->cap));
    CHOOSE_OPTION((x->sign), (uninit), (y->sign & mask), (x->sign));
    CHOOSE_OPTION((x->poisoned), (uninit), (y->poisoned & mask), (x->poisoned));
    CHOOSE_OPTION((x->n), (uninit), (y->n), (x->n)); x->sign = !!(x->n); 
    /* Post-operation Aggrestive Clearance */ // clang-format off
    mask = 0; alloc_size = 0; __BUFFER_P = 0; uninit = 0; 
    dst_limbs = 0; return ret_stat;// clang-format on
}
dnml_status crint_new_u64(crint *x, const uint64_t in) {
    uint64_t oom_mask = UINT64_MAX;
    dnml_status ret_stat = CRINT_SUCCESS;
    limb_t *__BUFFER_P = calloc(1, U64_BYTES);
    DNML_TEST_ASSERT( __BUFFER_P != NULL, realloc_null, {});
    CHOOSE_OPTION((ret_stat), ((_lib_crt_eq(__BUFFER_P, NULL)) & (_lib_crt_eq(x->limbs, NULL))), (DNML_ALLOC_OOM), (ret_stat));
    CHOOSE_OPTION((oom_mask), ((_lib_crt_eq(__BUFFER_P, NULL)) & (_lib_crt_eq(x->limbs, NULL))), (0), (oom_mask));

    uint8_t uninit = (x->limbs = NULL); uint64_t first_val = x->limbs[0];
    CHOOSE_OPTION((x->limbs), (uninit), ((uintptr_t)(__BUFFER_P) & oom_mask), ((uintptr_t)x->limbs));
    CHOOSE_OPTION((x->limbs[0]), (uninit), (in & oom_mask), (first_val));
    CHOOSE_OPTION((x->cap), (uninit), (1 & oom_mask), (x->cap));
    CHOOSE_OPTION((x->n), (uninit), ((!!(in)) & oom_mask), (x->n));
    CHOOSE_OPTION((x->sign), (uninit), (1 & oom_mask), (x->sign));
    CHOOSE_OPTION((x->poisoned), (uninit), (0 & oom_mask), (x->poisoned));
    /* Post-operation Aggresive Clearance */ // clang-format off
    oom_mask = 0; __BUFFER_P = 0; uninit = 0; first_val = 0;
    return ret_stat; // clang-format on
}
dnml_status crint_new_i64(crint *x, const int64_t in) {
    uint64_t oom_mask = UINT64_MAX;
    dnml_status ret_stat = CRINT_SUCCESS;
    limb_t *__BUFFER_P = calloc(1, U64_BYTES);
    DNML_TEST_ASSERT( __BUFFER_P != NULL, realloc_null, {});
    CHOOSE_OPTION((ret_stat), ((_lib_crt_eq(__BUFFER_P, NULL)) & (_lib_crt_eq(x->limbs, NULL))), (DNML_ALLOC_OOM), (ret_stat));
    CHOOSE_OPTION((oom_mask), ((_lib_crt_eq(__BUFFER_P, NULL)) & (_lib_crt_eq(x->limbs, NULL))), (0), (oom_mask));

    uint8_t uninit = (x->limbs = NULL); uint64_t first_val = x->limbs[0];
    int8_t new_sign; CHOOSE_OPTION((new_sign), (_lib_crt_isneg(in)), (-1), (1));
    CHOOSE_OPTION((x->limbs), (uninit), ((uintptr_t)(__BUFFER_P) & oom_mask), ((uintptr_t)x->limbs));
    CHOOSE_OPTION((x->limbs[0]), (uninit), (__MAG_I64__(in) & oom_mask), (first_val));
    CHOOSE_OPTION((x->cap), (uninit), (1 & oom_mask), (x->cap));
    CHOOSE_OPTION((x->n), (uninit), ((!!(in)) & oom_mask), (x->n));
    CHOOSE_OPTION((x->sign), (uninit), (new_sign & oom_mask), (x->sign));
    CHOOSE_OPTION((x->poisoned), (uninit), (0 & oom_mask), (x->poisoned));
    /* Post-operation Aggrestive Clearance */ // clang-format off
    oom_mask = 0; __BUFFER_P = 0; uninit = 0;
    first_val = 0; new_sign = 0; return ret_stat; // clang-format on
}
dnml_status crint_new_f128(crint *x, long double in) {}




//* =============================================== ASSIGNMENTS ============================================== */
dnml_status crint_set(crint x, crint *dst) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_ASSERT((__STORAGE_VAL__(dst)), store_inval, {
        if (dst->limbs != NULL) *(dst->limbs) = 0; crint_free(&x);
    }); 
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), ((x.poisoned) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    size_t set_range; size_t end; 
    CHOOSE_OPTION((set_range), (_lib_crt_lt(dst->cap, x.n)), dst->cap, x.n)
    CHOOSE_OPTION((end), (set_range), (set_range), (0));
    __libdnml_smemcpy_u64(dst->limbs, x.limbs, dst->cap, x.n, 0, end, (x.poisoned));
    int8_t rec_sign; CHOOSE_OPTION((rec_sign), (set_range), (x.sign), (1));
    CHOOSE_OPTION((dst->sign), (x.poisoned), (dst->sign), (rec_sign));
    CHOOSE_OPTION((dst->n), (x.poisoned), (dst->n), (set_range));
    __libdnml_smemwipe_u64(dst->limbs, dst->cap, set_range, dst->cap - 1, (x.poisoned));
    /* Aggressive Post-operation Cleanup */ // clang-format off
    set_range = 0; end = 0; rec_sign = 0; return ret_stat; // clang-format on
}
dnml_status crint_set_safe(crint x, crint *dst) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_ASSERT((__STORAGE_VAL__(dst)), store_inval, {
        if (dst->limbs != NULL) *(dst->limbs) = 0; crint_free(&x);
    });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), ((x.poisoned) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_lt(dst->cap, x.n)) & 
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), 
        (CRINT_ERR_RANGE), (ret_stat)
    );
    size_t end; CHOOSE_OPTION((end), (x.n), (x.n), (0));
    __libdnml_smemcpy_u64(dst->limbs, x.limbs, dst->cap, x.n, 0, end, (x.poisoned | _lib_crt_lt(dst->cap, x.n)));
    int8_t rec_sign; CHOOSE_OPTION((rec_sign), (x.n), (x.sign), (1));
    CHOOSE_OPTION((dst->sign), (x.poisoned), (dst->sign), (rec_sign));
    CHOOSE_OPTION((dst->n), (x.poisoned), (dst->n), (x.n));
    __libdnml_smemwipe_u64(dst->limbs, dst->cap, x.n, dst->cap - 1, (x.poisoned | _lib_crt_lt(dst->cap, x.n)));
    /* Aggressive Post-operation Cleanup */ // clang-format off
    end = 0; rec_sign = 0; return ret_stat; // clang-format on
}
/* --------- CryptInt --> Primitive Types --------- */
dnml_status crint_setu64(uint64_t* dst, crint x) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS; 
    CHOOSE_OPTION((ret_stat), (x.poisoned), (CRINT_POISON), (ret_stat));
    limb_t first_limb = x.limbs[0]; CHOOSE_OPTION((first_limb), (x.n), (first_limb), (0));
    CHOOSE_OPTION((*dst), (x.poisoned), (*dst), (first_limb)); // clang-format off 
    first_limb = 0; return ret_stat; // clang-format on
}
dnml_status crint_seti64(int64_t* dst, crint x) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS; 
    CHOOSE_OPTION((ret_stat), (x.poisoned), (CRINT_POISON), (ret_stat));
    limb_t first_limb = x.limbs[0]; CHOOSE_OPTION((first_limb), (x.n), (first_limb), (0));
    uint64_t abs_int64_min = (uint64_t)(llabs(INT64_MIN + 1)) + 1; uint64_t mask = UINT64_MAX;
    CHOOSE_OPTION((mask), (
        (_lib_crt_gt(first_limb, UINT64_MAX)) & 
        (_lib_crt_eq(x.sign, 1)) & (_lib_crt_eq(mask, UINT64_MAX))), 
        (I64_MIN_BIT_MASK), (mask)
    );
    CHOOSE_OPTION((mask), (
        (_lib_crt_gt(first_limb, abs_int64_min)) & 
        (_lib_crt_eq(x.sign, -1)) & (_lib_crt_eq(mask, UINT64_MAX))), 
        (I64_MIN_BIT_MASK), (mask)
    );
    CHOOSE_OPTION((*dst), (x.poisoned), (*dst), ((int64_t)(first_limb) * x.sign)); // clang-format off
    first_limb = 0; abs_int64_min = 0; return ret_stat; // clang-format on
}
dnml_status crint_setf128(long double* dst, crint x) {}
dnml_status crint_setu64_safe(uint64_t* dst, crint x) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); });
    dnml_status ret_stat = CRINT_SUCCESS; uint8_t noop = false;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((noop), (x.poisoned & !noop), (true), (noop));
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_gt(x.n, 1) | _lib_crt_eq(x.sign, -1)) 
        & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), 
        (CRINT_ERR_RANGE), (ret_stat)
    );
    CHOOSE_OPTION((noop), ((_lib_crt_gt(x.n, 1) | _lib_crt_eq(x.sign, -1)) & !noop), (true), (noop));
    limb_t first_limb = x.limbs[0]; CHOOSE_OPTION((first_limb), (x.n), (first_limb), (0));
    CHOOSE_OPTION((*dst), (!(noop)), (first_limb), (*dst)); // clang-format off
    noop = 0; first_limb = 0; return ret_stat; // clang-format on
}
dnml_status crint_seti64_safe(int64_t* dst, crint x) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); }); 
    dnml_status ret_stat = CRINT_SUCCESS; uint8_t noop = false;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((noop), (x.poisoned & !noop), (true), (noop));
    CHOOSE_OPTION((ret_stat), ((_lib_crt_gt(x.n, 1)) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_ERR_RANGE), (ret_stat));
    CHOOSE_OPTION((noop), ((_lib_crt_gt(x.n, 1)) & !noop), (true), (noop));

    limb_t first_limb = x.limbs[0]; CHOOSE_OPTION((first_limb), (x.n), (first_limb), (0));
    uint64_t abs_int64_min = (uint64_t)(llabs(INT64_MIN + 1)) + 1;
    CHOOSE_OPTION( /* if (first_limb > abs_int64_min) && x.sign == -1) */
        (ret_stat), ((_lib_crt_gt(first_limb, abs_int64_min)) & (_lib_crt_eq(x.sign, -1)) & 
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_ERR_RANGE), (ret_stat)
    ); 
    CHOOSE_OPTION((noop), ((_lib_crt_gt(first_limb, abs_int64_min)) & (_lib_crt_eq(x.sign, -1)) & !noop), (true), (noop));
    CHOOSE_OPTION( /* if (first_limb > UINT64_MAX && x.sign == 1) */
        (ret_stat), ((_lib_crt_gt(first_limb, UINT64_MAX)) & (_lib_crt_eq(x.sign, 1)) & 
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_ERR_RANGE), (ret_stat)
    ); 
    CHOOSE_OPTION((noop), ((_lib_crt_gt(first_limb, UINT64_MAX)) & (_lib_crt_eq(x.sign, 1)) & !noop), (true), (noop));
    CHOOSE_OPTION((*dst), (!(noop)), ((int64_t)(first_limb) * x.sign), (*dst)); // clang-format off
    noop = 0; first_limb = 0; abs_int64_min = 0; return ret_stat; // clang-format on
}
dnml_status crint_setf128_safe(long double* dst, crint x) {}
/* --------- Primitive Types --> CryptInt --------- */
dnml_status crint_getu64(crint *dst, const uint64_t val) {
    DNML_ASSERT((__STORAGE_VAL__(dst)), full_contract, { crint_free(dst); });
    DNML_TEST_ASSERT((dst->poisoned), poisoined, { crint_free(dst); }); dnml_status ret_stat = CRINT_SUCCESS; 
    CHOOSE_OPTION((ret_stat), (dst->poisoned), (CRINT_POISON), (ret_stat));
    limb_t first_limb = val; size_t lcount = !!(val); int8_t new_sign = 1;
    CHOOSE_OPTION((dst->limbs[0]), (dst->poisoned), (dst->limbs[0]), (first_limb));
    CHOOSE_OPTION((dst->n), (dst->poisoned), (dst->n), (lcount));
    CHOOSE_OPTION((dst->sign), (dst->poisoned), (dst->sign), (new_sign));
    __libdnml_smemwipe_u64(dst->limbs, dst->cap, lcount, dst->cap - 1, (dst->poisoned));
    /* Aggressive Post-operation Cleanup */ // clang-format off
    first_limb = 0; lcount = 0; new_sign = 0; return ret_stat; // clang-format on
}
dnml_status crint_geti64(crint *dst, const int64_t val) {
    DNML_ASSERT((__STORAGE_VAL__(dst)), full_contract, { crint_free(dst); });
    DNML_TEST_ASSERT((dst->poisoned), poisoined, { crint_free(dst); }); dnml_status ret_stat = CRINT_SUCCESS; 
    CHOOSE_OPTION((ret_stat), (dst->poisoned), (CRINT_POISON), (ret_stat));
    limb_t first_limb = __MAG_I64__(val); size_t lcount = !!(val); int8_t new_sign;
    CHOOSE_OPTION((new_sign), (_lib_crt_isneg(val)), (-1), (1));
    CHOOSE_OPTION((dst->limbs[0]), (dst->poisoned), (dst->limbs[0]), (first_limb));
    CHOOSE_OPTION((dst->n), (dst->poisoned), (dst->n), (lcount));
    CHOOSE_OPTION((dst->sign), (dst->poisoned), (dst->sign), (new_sign));
    __libdnml_smemwipe_u64(dst->limbs, dst->cap, lcount, dst->cap - 1, (dst->poisoned));
    /* Aggressive Post-operation Cleanup */ // clang-format off
    first_limb = 0; lcount = 0; new_sign = 0; return ret_stat; // clang-format on
}
dnml_status crint_getf128(crint *dst, const long double val) {}
dnml_status crint_getf128_safe(crint *dst, const long double val) {}




//* =============================================== CONVERSIONS ============================================== */
/* --------- CryptInt --> Primitive Types --------- */
uint64_t crint_tou64(crint x, dnml_status *err) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS; 
    CHOOSE_OPTION((ret_stat), (x.poisoned), (CRINT_POISON), (ret_stat));
    limb_t first_limb = x.limbs[0]; CHOOSE_OPTION((first_limb), (x.n), (first_limb), (0));
    CHOOSE_OPTION((first_limb), (x.poisoned), (0), (first_limb)); // clang-format off
    if (err != NULL) *err = ret_stat; ret_stat = 0; return first_limb; // clang-format on
}
int64_t crint_toi64(crint x, dnml_status *err) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS; 
    CHOOSE_OPTION((ret_stat), (x.poisoned), (CRINT_POISON), (ret_stat));

    limb_t first_limb = x.limbs[0]; CHOOSE_OPTION((first_limb), (x.n), (first_limb), (0));
    uint64_t abs_int64_min = (uint64_t)(llabs(INT64_MIN + 1)) + 1; uint64_t mask = UINT64_MAX;
    CHOOSE_OPTION((mask), (
        (_lib_crt_gt(first_limb, UINT64_MAX)) & 
        (_lib_crt_eq(x.sign, 1)) & (_lib_crt_eq(mask, UINT64_MAX))), 
        (I64_MIN_BIT_MASK), (mask)
    );
    CHOOSE_OPTION((mask), (
        (_lib_crt_gt(first_limb, abs_int64_min)) & 
        (_lib_crt_eq(x.sign, -1)) & (_lib_crt_eq(mask, UINT64_MAX))), 
        (I64_MIN_BIT_MASK), (mask)
    );
    int64_t ret; CHOOSE_OPTION((ret), (x.poisoned), (0), ((int64_t)(first_limb) * x.sign));
    if (err != NULL) *err = ret_stat; // clang-format off
    ret_stat = 0; first_limb = 0;  abs_int64_min = 0; mask = 0; return ret; // clang-format on
} 
long double crint_tof128(crint x, dnml_status *err) {}
uint64_t crint_tou64_safe(crint x, dnml_status *err) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); });
    dnml_status ret_stat = CRINT_SUCCESS; uint8_t noop = false;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((noop), (x.poisoned & !noop), (true), (noop));
    CHOOSE_OPTION((ret_stat), ((_lib_crt_gt(x.n, 1) | _lib_crt_eq(x.sign, -1)) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_ERR_RANGE), (ret_stat));
    CHOOSE_OPTION((noop), ((_lib_crt_gt(x.n, 1) | _lib_crt_eq(x.sign, -1)) & !noop), (true), (noop));

    limb_t first_limb = x.limbs[0]; CHOOSE_OPTION((first_limb), (x.n), (first_limb), (0));
    CHOOSE_OPTION((first_limb), (!(noop)), (first_limb), (0));
    if (err != NULL) *err = ret_stat; // clang-format off
    noop = 0; ret_stat = 0; return first_limb; // clang-format on
}
int64_t crint_toi64_safe(crint x, dnml_status *err) {
    DNML_ASSERT((crint_validate(x)), full_contract, { if (x.limbs != NULL) *(x.limbs) = 0; crint_free(&x); });
    DNML_TEST_ASSERT((x.poisoned), poisoined, { crint_free(&x); }); 
    dnml_status ret_stat = CRINT_SUCCESS; uint8_t noop = false;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((noop), (x.poisoned & !noop), (true), (noop));
    CHOOSE_OPTION((ret_stat), ((_lib_crt_gt(x.n, 1)) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_ERR_RANGE), (ret_stat));
    CHOOSE_OPTION((noop), ((_lib_crt_gt(x.n, 1)) & !noop), (true), (noop));

    limb_t first_limb = x.limbs[0]; CHOOSE_OPTION((first_limb), (x.n), (first_limb), (0));
    uint64_t abs_int64_min = (uint64_t)(llabs(INT64_MIN + 1)) + 1;
    CHOOSE_OPTION( /* if (first_limb > abs_int64_min) && x.sign == -1) */
        (ret_stat), ((_lib_crt_gt(first_limb, abs_int64_min)) & (_lib_crt_eq(x.sign, -1)) & 
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_ERR_RANGE), (ret_stat)
    ); 
    CHOOSE_OPTION((noop), ((_lib_crt_gt(first_limb, abs_int64_min)) & (_lib_crt_eq(x.sign, -1)) & !noop), (true), (noop));
    CHOOSE_OPTION( /* if (first_limb > UINT64_MAX && x.sign == 1) */
        (ret_stat), ((_lib_crt_gt(first_limb, UINT64_MAX)) & (_lib_crt_eq(x.sign, 1)) & 
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_ERR_RANGE), (ret_stat)
    ); 
    CHOOSE_OPTION((noop), ((_lib_crt_gt(first_limb, UINT64_MAX)) & (_lib_crt_eq(x.sign, 1)) & !noop), (true), (noop));
    CHOOSE_OPTION((first_limb), (!(noop)), ((int64_t)(first_limb) * x.sign), (0));
    if (err != NULL) *err = ret_stat; // clang-format off
    ret_stat = 0; noop = 0; abs_int64_min = 0; return first_limb; // clang-format on
}
long double crint_tof128_safe(crint x, dnml_status *err) {}
/* --------- Primitive Types --> CryptInt --------- */
crint crint_fromu64(const uint64_t x, dnml_status *err) {
    crint ret; dnml_status new_stat; limb_t tmp[1] = {0}; new_stat = crint_new(&ret);
    CHOOSE_OPTION((ret.limbs), (_lib_crt_eq(new_stat, DNML_ALLOC_OOM)), ((uintptr_t)tmp), ((uintptr_t)ret.limbs));
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_neq(new_stat, DNML_ALLOC_OOM)));
    ret.limbs[0] = x; ret.n = !!(x); ret.sign = 1; /* Correctly Setting Up - Standard Case */
    ret.limbs[0] &= mask; ret.n &= mask; ret.sign &= mask; /* Selectively masking into invalidity */
    CHOOSE_OPTION((ret.limbs), (_lib_crt_eq(new_stat, DNML_ALLOC_OOM)), ((uintptr_t)0), ((uintptr_t)ret.limbs));
    if (err != NULL) CHOOSE_OPTION((*err), (_lib_crt_eq(new_stat, DNML_ALLOC_OOM)), (DNML_ALLOC_OOM), (CRINT_SUCCESS));
    /* Aggressive POst-Operation Cleanup */ // clang-format off
    new_stat = 0; tmp[0] = 0; mask = 0; return ret; // clang-format on
}
crint crint_fromi64(const int64_t x, dnml_status *err) {
    crint ret; dnml_status new_stat; limb_t tmp[1] = {0}; new_stat = crint_new(&ret);
    CHOOSE_OPTION((ret.limbs), (_lib_crt_eq(new_stat, DNML_ALLOC_OOM)), ((uintptr_t)tmp), ((uintptr_t)ret.limbs));
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_neq(new_stat, DNML_ALLOC_OOM)));
    ret.limbs[0] = __MAG_I64__(x); ret.n = !!(x); /* Correctly Setting Up - Standard Case */
    CHOOSE_OPTION((ret.sign), (_lib_crt_isneg(x)), (-1), (1)); /* Correctly Setting Up - Standard Case */
    ret.limbs[0] &= mask; ret.n &= mask; ret.sign &= mask; /* Selectively masking into invalidity */
    CHOOSE_OPTION((ret.limbs), (_lib_crt_eq(new_stat, DNML_ALLOC_OOM)), ((uintptr_t)0), ((uintptr_t)ret.limbs));
    if (err != NULL) CHOOSE_OPTION((*err), (_lib_crt_eq(new_stat, DNML_ALLOC_OOM)), (DNML_ALLOC_OOM), (CRINT_SUCCESS));
    /* Aggressive POst-Operation Cleanup */ // clang-format off
    new_stat = 0; tmp[0] = 0; mask = 0; return ret; // clang-format on
}
crint crint_fromf128(const long double x, dnml_status *err) {}
crint crint_fromf128_safe(const long double x, dnml_status *err) {}




//* =========================================== BITWISE OPERATIONS =========================================== */
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
/* ----------------------- Mutative, Fixed Width ----------------------- */
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
/* ----------------------- Mutative, Explicit Width ----------------------- */
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
/* ----------------------- Functional, Fixed Width ----------------------- */
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
/* ----------------------- Functional, Explicit Width ----------------------- */
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




//* =============================================== COMPARISONS ============================================== */
static int8_t __CRINT_MAGCMP64__(crint *x, const uint64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_TEST_ASSERT((crint_pvalidate(x)), full_contract, {});
    DNML_TEST_ASSERT((x->poisoned), poisoined, {});
    /* Main Operation - Comparison */
    // We set ret to 2 as a safety mask to check if ret is previously set or not
    int8_t ret = 2, curr = 0;
    // Check 1: if (x->n > 1) return 1;
    CHOOSE_OPTION((curr), (_lib_crt_gt(x->n, 1)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (x->limbs[0] > val) return 1;
    CHOOSE_OPTION((curr), (_lib_crt_gt(x->limbs[0], val)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 3: if (x->limbs[0] < val) return -1;
    CHOOSE_OPTION((curr), (_lib_crt_lt(x->limbs[0], val)), (-1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // ALL CHECKS WEREN'T SATIFIES --> EQUAL
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (0), (ret)); 
    curr = 0; return ret; // set curr to sanitize memory
}
static int8_t __CRINT_MAGCMP__(crint *x, crint *y) {
    /* Pre-operation Validation & Static Analysis */
    DNML_TEST_ASSERT((crint_pvalidate(x)), full_contract, {});
    DNML_TEST_ASSERT((x->poisoned), poisoined, {});
    /* Main Operation - Comparison */
    // We set ret to 2 as a safety mask to check if ret is previously set or not
    int8_t ret = 2, curr = 0;
    // Check 1: if (x->n > y->n) return 1;
    CHOOSE_OPTION((curr), (_lib_crt_gt(x->n, y->n)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (x->n < y->n) return -1;
    CHOOSE_OPTION((curr), (_lib_crt_lt(x->n, y->n)), (-1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 3: Sequential checks
    size_t upperbound = crtmax(x->cap, y->cap);
    uint64_t x_curr, y_curr;
    for (size_t i = upperbound - 1; _lib_crt_neq(i, -1); --i) {
        /* 
        * Guarantees that each iteration always access an index to normalize timing
        * We universally chosen the first limb of each crint as a placeholder
        * with not semantic meaning 
        */
        CHOOSE_OPTION((x_curr), (_lib_crt_lt(i, x->cap)), (x->limbs[i]), (x->limbs[0]));
        CHOOSE_OPTION((x_curr), (_lib_crt_lt(i, x->cap)), (y->limbs[i]), (y->limbs[0]));
        // Actually Getting the right value
        CHOOSE_OPTION((x_curr), (_lib_crt_lt(i, x->n)), (x_curr), (0));
        CHOOSE_OPTION((y_curr), (_lib_crt_lt(i, y->n)), (y_curr), (0));
        // Comparing values
        /* 
        * If our x_curr == x->limbs[i], in which i > y->n,
        * then such case was already covered above in our pre-checks
        * due to the fact that _lib_crt_gt(x->n, y->n). This also works inversely for y
        */
       CHOOSE_OPTION((curr), (_lib_crt_gt(x_curr, y_curr)), (1), (0));
       CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
       CHOOSE_OPTION((curr), (_lib_crt_lt(x_curr, y_curr)), (-1), (0));
       CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    } 
    // ALL CHECKS WEREN'T SATIFIES --> EQUAL
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (0), (ret));
    /* Aggresive Memory Sanitization */
    curr = 0; upperbound = 0; x_curr = 0; y_curr = 0;
    return ret;
}
/* ---------------- Integer - I64 ---------------- */
bool crint_equal_i64(crint x, const int64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); x.limbs = 0; x.cap = 0; 
        x.n = 0; x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((x.poisoned), poisoined, {});
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr; int8_t vsign;
    CHOOSE_OPTION((vsign), (_lib_crt_isneg(val)), (-1), (1));
    uint64_t mag_val = __MAG_I64__(val);
    /* if (!x.n) return (val) ? false : true;  */
    // Check 1: if (!x.n && !val) return true
    CHOOSE_OPTION((curr), (!(x.n) & !(val)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (!x.n && val) return false
    CHOOSE_OPTION((curr), (!(x.n) & (val)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    // Check 3: if (val_sign != x.sign) return false;
    CHOOSE_OPTION((curr), (vsign != x.sign), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 4: if (x.n > 1) return false;
    CHOOSE_OPTION((curr), (_lib_crt_gt(x.n, 1)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 5: if (x.limbs[0] != __MAG_I64__(val)) return false;
    CHOOSE_OPTION((curr), (x.limbs[0] != mag_val), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (1), (ret)); 
    /* Post-operation Aggrestive Cleanup */
    if (err != NULL) *err = ret_stat; // Conditional Branching here is aceptable
    ret_stat = 0; curr = 0; vsign = 0; mag_val = 0; return (bool)(ret);
}
bool crint_less_i64(crint x, const int64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); x.limbs = 0; x.cap = 0; 
        x.n = 0; x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((x.poisoned), poisoined, {});
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr; int8_t vsign;
    CHOOSE_OPTION((vsign), (_lib_crt_isneg(val)), (-1), (1));
    uint64_t mag_val = __MAG_I64__(val);
    /* if (!x.n) return (val > 0) ? 1 : 0 */
    // Check 1: if (!x.n && val > 0) return true
    CHOOSE_OPTION((curr), (!(x.n) & (_lib_crt_gti64(val, 0))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (!x.n && val < 0) return false
    CHOOSE_OPTION((curr), (!(x.n) & (_lib_crt_isneg(val))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (val_sign != x.sign) return (x.sign < val_sign); */
    // Check 3: if (val_sign != x.sign && x.sign > val_sign) return false;
    CHOOSE_OPTION((curr), ((vsign != x.sign) & (_lib_crt_gti64(x.sign, vsign))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 4: if (val_sign != x.sign && x.sign < val_sign) return true;
    CHOOSE_OPTION((curr), ((vsign != x.sign) & (_lib_crt_lti64(x.sign, vsign))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (x.n > 1) return (x.sign == -1)) */
    // Check 5: if (x.n > 1 && x.sign == 1) return false;
    CHOOSE_OPTION((curr), ((_lib_crt_gt(x.n, 1)) & (_lib_crt_eq(x.sign, 1))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 5: if (x.n > 1 && x.sign == -1) return true;
    CHOOSE_OPTION((curr), ((_lib_crt_gt(x.n, 1)) & (_lib_crt_eq(x.sign, -1))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (x.limbs[0] > __MAG_I64__(val)) return (x.sign == -1) */
    // Check 6: if (x.limbs[0] > mag_val && x.sign == 1) return false;
    CHOOSE_OPTION((curr), ((_lib_crt_gt(x.limbs[0], mag_val)) & (_lib_crt_eq(x.sign, 1))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 7: if (x.limbs[0] > mag_val && x.sign == -1) return true;
    CHOOSE_OPTION((curr), ((_lib_crt_gt(x.limbs[0], mag_val)) & (_lib_crt_eq(x.sign, -1))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* return (x.limbs[0] < mag_val) && (x.sign == 1) */
    // Check 8: if (x->limbs[0] < mag_val && x.sign == -1) return false;
    CHOOSE_OPTION((curr), ((_lib_crt_lt(x.limbs[0], mag_val)) & (_lib_crt_eq(x.sign, -1))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (1), (ret)); 
    /* Post-operation Aggrestive Cleanup */
    if (err != NULL) *err = ret_stat; // Conditional Branching here is aceptable
    ret_stat = 0; curr = 0; vsign = 0; mag_val = 0; return (bool)(ret);
}
bool crint_more_i64(crint x, const int64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); x.limbs = 0; x.cap = 0; 
        x.n = 0; x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((x.poisoned), poisoined, {});
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr; int8_t vsign;
    CHOOSE_OPTION((vsign), (_lib_crt_isneg(val)), (-1), (1));
    uint64_t mag_val = __MAG_I64__(val);
    /* if (!x.n) return (val) ? 1 : 0 */
    // Check 1: if (!x.n && val < 0) return true
    CHOOSE_OPTION((curr), (!(x.n) & (_lib_crt_isneg(val))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (!x.n && val) return false
    CHOOSE_OPTION((curr), (!(x.n) & (val)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (val_sign != x.sign) return (x.sign < val_sign); */
    // Check 3: if (val_sign != x.sign && x.sign < val_sign) return false;
    CHOOSE_OPTION((curr), ((vsign != x.sign) & (_lib_crt_lti64(x.sign, vsign))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 4: if (val_sign != x.sign && x.sign > val_sign) return true;
    CHOOSE_OPTION((curr), ((vsign != x.sign) & (_lib_crt_gti64(x.sign, vsign))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (x.n > 1) return (x.sign == -1) */
    // Check 5: if (x.n > 1 && x.sign == 1) return false;
    CHOOSE_OPTION((curr), ((_lib_crt_gt(x.n, 1)) & (_lib_crt_eq(x.sign, 1))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 5: if (x.n > 1 && x.sign == -1) return true;
    CHOOSE_OPTION((curr), ((_lib_crt_gt(x.n, 1)) & (_lib_crt_eq(x.sign, -1))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (x.limbs[0] < __MAG_I64__(val)) return (x.sign == -1)) */
    // Check 6: if (x.limbs[0] < mag_val && x.sign == 1) return false;
    CHOOSE_OPTION((curr), ((_lib_crt_lt(x.limbs[0], mag_val)) & (_lib_crt_eq(x.sign, 1))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 7: if (x.limbs[0] < mag_val && x.sign == -1) return true;
    CHOOSE_OPTION((curr), ((_lib_crt_lt(x.limbs[0], mag_val)) & (_lib_crt_eq(x.sign, -1))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* return (x.limbs[0] > __MAG_I64__(val)) && x.sign == 1) */
    // Check 8: if (x->limbs[0] > mag_val && x.sign == -1) return false;
    CHOOSE_OPTION((curr), ((_lib_crt_gt(x.limbs[0], mag_val)) & (_lib_crt_eq(x.sign, -1))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (1), (ret)); 
    /* Post-operation Aggrestive Cleanup */
    if (err != NULL) *err = ret_stat; // Conditional Branching here is aceptable
    ret_stat = 0; curr = 0; vsign = 0; mag_val = 0; return (bool)(ret);
}
bool crint_lequal_i64(crint x, const int64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); x.limbs = 0; x.cap = 0; 
        x.n = 0; x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((x.poisoned), poisoined, {});
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr; int8_t vsign;
    CHOOSE_OPTION((vsign), (_lib_crt_isneg(val)), (-1), (1));
    uint64_t mag_val = __MAG_I64__(val);
    /* if (!x.n) return (val >= 0) ? 1 : 0 */
    // Check 1: if (!x.n && val >= 0) return true
    CHOOSE_OPTION((curr), (!(x.n) & (_lib_crt_ispos(val))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (!x.n && val < 0) return false
    CHOOSE_OPTION((curr), (!(x.n) & (_lib_crt_isneg(val))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (val_sign != x.sign) return (x.sign < val_sign); */
    // Check 3: if (val_sign != x.sign && x.sign > val_sign) return false;
    CHOOSE_OPTION((curr), ((vsign != x.sign) & (_lib_crt_gti64(x.sign, vsign))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 4: if (val_sign != x.sign && x.sign < val_sign) return true;
    CHOOSE_OPTION((curr), ((vsign != x.sign) & (_lib_crt_lti64(x.sign, vsign))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (if (x.n > 1) return (x.sign == -1); */
    // Check 5: if (x.n > 1 && x.sign == 1) return false;
    CHOOSE_OPTION((curr), ((_lib_crt_gt(x.n, 1)) & (_lib_crt_eq(x.sign, 1))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 5: if (x.n > 1 && x.sign == -1) return true;
    CHOOSE_OPTION((curr), ((_lib_crt_gt(x.n, 1)) & (_lib_crt_eq(x.sign, -1))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (x.limbs[0] > __MAG_I64__(val)) return (x.sign == 1); */
    // Check 6: if (x.limbs[0] > mag_val && x.sign == 1) return false;
    CHOOSE_OPTION((curr), ((_lib_crt_gt(x.limbs[0], mag_val)) & (_lib_crt_eq(x.sign, 1))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 7: if (x.limbs[0] > mag_val && x.sign == -1) return true;
    CHOOSE_OPTION((curr), ((_lib_crt_gt(x.limbs[0], mag_val)) & (_lib_crt_eq(x.sign, -1))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* return (x.sign == 1) */
    // Check 8: if (x.sign == -1) return false;
    CHOOSE_OPTION((curr), (_lib_crt_eq(x.sign, -1)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (1), (ret)); 
    /* Post-operation Aggrestive Cleanup */
    if (err != NULL) *err = ret_stat; // Conditional Branching here is aceptable
    ret_stat = 0; curr = 0; vsign = 0; mag_val = 0; return (bool)(ret);
}
bool crint_mequal_i64(crint x, const int64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); x.limbs = 0; x.cap = 0; 
        x.n = 0; x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((x.poisoned), poisoined, {});
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr; int8_t vsign;
    CHOOSE_OPTION((vsign), (_lib_crt_isneg(val)), (-1), (1));
    uint64_t mag_val = __MAG_I64__(val);
    /* if (!x.n) return (val <= 0) ? 1 : 0 */
    // Check 1: if (!x.n && val <= 0) return true
    CHOOSE_OPTION((curr), (!(x.n) & (_lib_crt_leqi64(val, 0))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (!x.n && val > 0) return false
    CHOOSE_OPTION((curr), (!(x.n) & (_lib_crt_geqi64(val, 0))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (val_sign != x.sign) return (x.sign > val_sign); */
    // Check 3: if (val_sign != x.sign && x.sign < val_sign) return false;
    CHOOSE_OPTION((curr), ((vsign != x.sign) & (_lib_crt_lti64(x.sign, vsign))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 4: if (val_sign != x.sign && x.sign > val_sign) return true;
    CHOOSE_OPTION((curr), ((vsign != x.sign) & (_lib_crt_gti64(x.sign, vsign))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (x.n > 1) return (x.sign == 1) */
    // Check 5: if (x.n > 1 && x.sign == -1) return false;
    CHOOSE_OPTION((curr), ((_lib_crt_gt(x.n, 1)) & (_lib_crt_eq(x.sign, -1))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 5: if (x.n > 1 && x.sign == 1)) return true;
    CHOOSE_OPTION((curr), ((_lib_crt_gt(x.n, 1)) & (_lib_crt_eq(x.sign, 1))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (x.limbs[0] > __MAG_I64__(val)) return x.sign == 1) */
    // Check 6: if (x.limbs[0] > mag_val && x.sign == 1) return false;
    CHOOSE_OPTION((curr), ((_lib_crt_gt(x.limbs[0], mag_val)) & (_lib_crt_eq(x.sign, -1))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 7: if (x.limbs[0] > mag_val && x.sign == -1) return true;
    CHOOSE_OPTION((curr), ((_lib_crt_gt(x.limbs[0], mag_val)) & (_lib_crt_eq(x.sign, 1))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* return (x.sign == -1) */
    // Check 8: if (x.sign == 1) return false;
    CHOOSE_OPTION((curr), (_lib_crt_eq(x.sign, 1)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (1), (ret)); 
    /* Post-operation Aggrestive Cleanup */
    if (err != NULL) *err = ret_stat; // Conditional Branching here is aceptable
    ret_stat = 0; curr = 0; vsign = 0; mag_val = 0; return (bool)(ret);
}
/* ----------- Unsigned Integer - UI64 ----------- */
bool crint_equal_u64(crint x, const uint64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); x.limbs = 0; x.cap = 0; 
        x.n = 0; x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((x.poisoned), poisoined, {});
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr;
    /* if (!x.n) return (val) ? false : true;  */
    // Check 1: if (!x.n && !val) return true
    CHOOSE_OPTION((curr), (!(x.n) & !(val)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (!x.n && val) return false
    CHOOSE_OPTION((curr), (!(x.n) & (val)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    // Check 3: if (x.sign == -1) return false;
    CHOOSE_OPTION((curr), (_lib_crt_eq(x.sign, -1)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 4: if (x.n > 1) return false;
    CHOOSE_OPTION((curr), (_lib_crt_gt(x.n, 1)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 5: if (x.limbs[0] != val) return false;
    CHOOSE_OPTION((curr), (x.limbs[0] != val), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (1), (ret)); 
    /* Post-operation Aggresive Cleanup */
    if (err != NULL) *err = ret_stat; // Conditional Branching here is aceptable
    ret_stat = 0; curr = 0; return (bool)(ret);
}
bool crint_less_u64(crint x, const uint64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); x.limbs = 0; x.cap = 0; 
        x.n = 0; x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((x.poisoned), poisoined, {});
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr;
    /* if (!x.n) return (val) ? true : false;  */
    // Check 1: if (!x.n && val) return true
    CHOOSE_OPTION((curr), (!(x.n) & (val)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (!x.n && !val) return false
    CHOOSE_OPTION((curr), (!(x.n) & !(val)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    // Check 3: if (x.sign == -1) return true;
    CHOOSE_OPTION((curr), (_lib_crt_eq(x.sign, -1)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 4: if (x.n == 1) return false;
    CHOOSE_OPTION((curr), (_lib_crt_gt(x.n, 1)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 5: if (x.limbs[0] >= val) return false;
    CHOOSE_OPTION((curr), (_lib_crt_geq(x.limbs[0], val)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (1), (ret)); 
    /* Post-operation Aggresive Cleanup */
    if (err != NULL) *err = ret_stat; // Conditional Branching here is aceptable
    ret_stat = 0; curr = 0; return (bool)(ret);
}
bool crint_more_u64(crint x, const uint64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); x.limbs = 0; x.cap = 0; 
        x.n = 0; x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((x.poisoned), poisoined, {});
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr;
    /* if (!x.n) return (val) ? false : true;  */
    // Check 1: if (!x.n && !val) return true
    CHOOSE_OPTION((curr), (!(x.n) & !(val)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (!x.n && val) return false
    CHOOSE_OPTION((curr), (!(x.n) & (val)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    // Check 3: if (x.sign == -1) return false;
    CHOOSE_OPTION((curr), (_lib_crt_eq(x.sign, -1)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 4: if (x.n == 1) return true;
    CHOOSE_OPTION((curr), (_lib_crt_gt(x.n, 1)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 5: if (x.limbs[0] <= val) return false;
    CHOOSE_OPTION((curr), (_lib_crt_leq(x.limbs[0], val)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (1), (ret)); 
    /* Post-operation Aggresive Cleanup */
    if (err != NULL) *err = ret_stat; // Conditional Branching here is aceptable
    ret_stat = 0; curr = 0; return (bool)(ret);
}
bool crint_lequal_u64(crint x, const uint64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); x.limbs = 0; x.cap = 0; 
        x.n = 0; x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((x.poisoned), poisoined, {});
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr;
    // Check 1: if (!x.n) return true
    CHOOSE_OPTION((curr), (!(x.n)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (x.sign == -1) return true;
    CHOOSE_OPTION((curr), (_lib_crt_eq(x.sign, -1)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 4: if (x.n == 1) return false;
    CHOOSE_OPTION((curr), (_lib_crt_gt(x.n, 1)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 5: if (x.limbs[0] > val) return false;
    CHOOSE_OPTION((curr), (_lib_crt_gt(x.limbs[0], val)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (1), (ret)); 
    /* Post-operation Aggresive Cleanup */
    if (err != NULL) *err = ret_stat; // Conditional Branching here is aceptable
    ret_stat = 0; curr = 0; return (bool)(ret);
}
bool crint_mequal_u64(crint x, const uint64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); x.limbs = 0; x.cap = 0; 
        x.n = 0; x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((x.poisoned), poisoined, {});
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr;
    /* if (!x.n) return (val) ? false : true;  */
    // Check 1: if (!x.n && !val) return true
    CHOOSE_OPTION((curr), (!(x.n) & !(val)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (!x.n && val) return false
    CHOOSE_OPTION((curr), (!(x.n) & (val)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    // Check 3: if (x.sign == -1) return false;
    CHOOSE_OPTION((curr), (_lib_crt_eq(x.sign, -1)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 4: if (x.n == 1) return true;
    CHOOSE_OPTION((curr), (_lib_crt_gt(x.n, 1)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 5: if (x.limbs[0] < val) return false;
    CHOOSE_OPTION((curr), (_lib_crt_lt(x.limbs[0], val)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (1), (ret)); 
    /* Post-operation Aggresive Cleanup */
    if (err != NULL) *err = ret_stat; // Conditional Branching here is aceptable
    ret_stat = 0; curr = 0; return (bool)(ret);
}
/* ------------------- Cryptint ------------------ */
bool crint_equal(crint x, crint y, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT(((crint_validate(x)) && (crint_validate(y))), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (y.limbs != NULL) *(y.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); crint_free(&y);
        x.limbs = 0; x.cap = 0; x.n = 0; x.poisoned = 0; x.sign = 0;
        y.limbs = 0; y.cap = 0; y.n = 0; y.poisoned = 0; y.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((x.poisoned), poisoined, {});
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr;
    /* if (!x.n) return (val) ? false : true;  */
    // Check 1: if (!x.n && !y.n) return true
    CHOOSE_OPTION((curr), (!(x.n) & !(y.n)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (!x.n && y.n) return false
    CHOOSE_OPTION((curr), (!(x.n) & (y.n)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (a.sign != b.sign) return false; */
    CHOOSE_OPTION((curr), (x.sign != y.sign), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    /* if (a.n != b.n) return false; */
    CHOOSE_OPTION((curr), (x.n != y.n), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    /* if (a.limbs == b.limbs) return true; */
    CHOOSE_OPTION((curr), (_lib_crt_eq(x.limbs, y.limbs)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* Main Validation Loop */
    int8_t mag_ret = __CRINT_MAGCMP__(&x, &y);
    CHOOSE_OPTION((curr), (!(mag_ret)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (0), (ret));
    /* Post-operation Aggresive Cleanup */
    if (err != NULL) *err = ret_stat; // Conditional Branching here is aceptableerr = ret_stat; 
    ret_stat = 0; curr = 0; mag_ret = 0; return (bool)(ret);
}
bool crint_less(crint x, crint y, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT(((crint_validate(x)) && (crint_validate(y))), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (y.limbs != NULL) *(y.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); crint_free(&y);
        x.limbs = 0; x.cap = 0; x.n = 0; 
        x.poisoned = 0; x.sign = 0;
        y.limbs = 0; y.cap = 0; y.n = 0;
        y.poisoned = 0; y.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((x.poisoned), poisoined, {});
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr;
    /* if (x.sign != y.sign) return (x.sign < y.sign) */
    // Check 1: if (x.sign != y.sign & x.sign > y.sign) return false
    CHOOSE_OPTION((curr), ((x.sign != y.sign) & (_lib_crt_gti64(x.sign, y.sign))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (x.sign != y.sign & x.sign < y.sign) return true
    CHOOSE_OPTION((curr), ((x.sign != y.sign) & (_lib_crt_lti64(x.sign, y.sign))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (x.n != y.n) return (x.sign == 1) ? (x.n < y.n) : (x.n > y.n); */
    // Check 3: if (x.n != y.n && x.sign == 1 && x.n > y.n) return false
    CHOOSE_OPTION((curr), ((x.n != y.n) & (_lib_crt_eq(x.sign, 1)) & (_lib_crt_gt(x.n, y.n))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 4: if (x.n != y.n && x.sign == 1 && x.n < y.n) return true
    CHOOSE_OPTION((curr), ((x.n != y.n) & (_lib_crt_eq(x.sign, 1)) & (_lib_crt_lt(x.n, y.n))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 5: if (x.n != y.n && x.sign == -1 && x.n < y.n) return false
    CHOOSE_OPTION((curr), ((x.n != y.n) & (_lib_crt_eq(x.sign, -1)) & (_lib_crt_lt(x.n, y.n))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 6: if (x.n != y.n && x.sign == -1 && x.n > y.n) return true
    CHOOSE_OPTION((curr), ((x.n != y.n) & (_lib_crt_eq(x.sign, -1)) & (_lib_crt_gt(x.n, y.n))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (a.limbs == b.limbs) return false; */
    CHOOSE_OPTION((curr), (_lib_crt_eq(x.limbs, y.limbs)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* Main Validation Loop */
    /* return (a.sign == 1) ? __CRINT_MAGCMP__(&x, &y) < 0 : __CRINT_MAGCMP__(&x, &y) > 0; */
    int8_t mag_ret = __CRINT_MAGCMP__(&x, &y);
    // Check 7+8: return (a.sign == 1 && mag_ret < 0);
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, 1)) & (_lib_crt_ispos(mag_ret))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, 1)) & _lib_crt_isneg(mag_ret)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 9+10: return (a.sign == -1 && mag_ret > 0);
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, -1)) & (_lib_crt_neq(mag_ret, 0))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, -1)) & (mag_ret)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 11: mag_ret == 0
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (0), (ret));
    /* Post-operation Aggresive Cleanup */
    if (err != NULL) *err = ret_stat; // Conditional Branching here is aceptable
    ret_stat = 0; curr = 0; mag_ret = 0; return (bool)(ret);
}
bool crint_more(crint x, crint y, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT(((crint_validate(x)) && (crint_validate(y))), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (y.limbs != NULL) *(y.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); crint_free(&y);
        x.limbs = 0; x.cap = 0; x.n = 0; 
        x.poisoned = 0; x.sign = 0;
        y.limbs = 0; y.cap = 0; y.n = 0;
        y.poisoned = 0; y.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((x.poisoned), poisoined, {});
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr;
    /* if (x.sign != y.sign) return (_lib_crt_gti64(x.sign, y.sign)) */
    // Check 1: if (x.sign != y.sign & x.sign < y.sign) return false
    CHOOSE_OPTION((curr), ((x.sign != y.sign) & (_lib_crt_lti64(x.sign, y.sign))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (x.sign != y.sign & x.sign > y.sign) return true
    CHOOSE_OPTION((curr), ((x.sign != y.sign) & (_lib_crt_gti64(x.sign, y.sign))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (x.n != y.n) return (x.sign == 1) ? (x.n > y.n) : (x.n < y.n); */
    // Check 3: if (x.n != y.n && x.sign == 1 && x.n < y.n) return false
    CHOOSE_OPTION((curr), ((x.n != y.n) & (_lib_crt_eq(x.sign, 1)) & (_lib_crt_lt(x.n, y.n))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 4: if (x.n != y.n && x.sign == 1 && x.n > y.n) return true
    CHOOSE_OPTION((curr), ((x.n != y.n) & (_lib_crt_eq(x.sign, 1)) & (_lib_crt_gt(x.n, y.n))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 5: if (x.n != y.n && x.sign == -1 && x.n > y.n) return false
    CHOOSE_OPTION((curr), ((x.n != y.n) & (_lib_crt_eq(x.sign, -1)) & (_lib_crt_gt(x.n, y.n))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 6: if (x.n != y.n && x.sign == -1 && x.n < y.n) return true
    CHOOSE_OPTION((curr), ((x.n != y.n) & (_lib_crt_eq(x.sign, -1)) & (_lib_crt_lt(x.n, y.n))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (a.limbs == b.limbs) return false; */
    CHOOSE_OPTION((curr), (_lib_crt_eq(x.limbs, y.limbs)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* Main Validation Loop */
    /* return (a.sign == 1) ? __CRINT_MAGCMP__(&x, &y) > 0 : __CRINT_MAGCMP__(&x, &y) < 0; */
    int8_t mag_ret = __CRINT_MAGCMP__(&x, &y);
    // Check 7+8: return (a.sign == 1 && mag_ret > 0);
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, 1)) & (_lib_crt_neq(mag_ret, 0))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, 1)) & (mag_ret)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 9+10: return (a.sign == -1 && mag_ret < 0);
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, -1)) & _lib_crt_geqi64(mag_ret, 0)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, -1)) & _lib_crt_isneg(mag_ret)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 11: mag_ret == 0
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (0), (ret));
    /* Post-operation Aggresive Cleanup */
    if (err != NULL) *err = ret_stat; // Conditional Branching here is aceptable
    ret_stat = 0; curr = 0; mag_ret = 0; return (bool)(ret);
}
bool crint_lequal(crint x, crint y, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT(((crint_validate(x)) && (crint_validate(y))), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (y.limbs != NULL) *(y.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); crint_free(&y);
        x.limbs = 0; x.cap = 0; x.n = 0; 
        x.poisoned = 0; x.sign = 0;
        y.limbs = 0; y.cap = 0; y.n = 0;
        y.poisoned = 0; y.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((x.poisoned), poisoined, {});
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr;
    /* if (x.sign != y.sign) return (x.sign < y.sign) */
    // Check 1: if (x.sign != y.sign & x.sign > y.sign) return false
    CHOOSE_OPTION((curr), ((x.sign != y.sign) & (_lib_crt_gti64(x.sign, y.sign))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (x.sign != y.sign & x.sign < y.sign) return true
    CHOOSE_OPTION((curr), ((x.sign != y.sign) & (_lib_crt_lti64(x.sign, y.sign))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (x.n != y.n) return (x.sign == 1) ? (x.n < y.n) : (x.n > y.n); */
    // Check 3: if (x.n != y.n && x.sign == 1 && x.n > y.n) return false
    CHOOSE_OPTION((curr), ((x.n != y.n) & (_lib_crt_eq(x.sign, 1)) & (_lib_crt_gt(x.n, y.n))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 4: if (x.n != y.n && x.sign == 1 && x.n < y.n) return true
    CHOOSE_OPTION((curr), ((x.n != y.n) & (_lib_crt_eq(x.sign, 1)) & (_lib_crt_lt(x.n, y.n))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 5: if (x.n != y.n && x.sign == -1 && x.n < y.n) return false
    CHOOSE_OPTION((curr), ((x.n != y.n) & (_lib_crt_eq(x.sign, -1)) & (_lib_crt_lt(x.n, y.n))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 6: if (x.n != y.n && x.sign == -1 && x.n > y.n) return true
    CHOOSE_OPTION((curr), ((x.n != y.n) & (_lib_crt_eq(x.sign, -1)) & (_lib_crt_gt(x.n, y.n))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (a.limbs == b.limbs) return true; */
    CHOOSE_OPTION((curr), (_lib_crt_eq(x.limbs, y.limbs)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* Main Validation Loop */
    /* return (a.sign == 1) ? __CRINT_MAGCMP__(&x, &y) < 0 : __CRINT_MAGCMP__(&x, &y) > 0; */
    int8_t mag_ret = __CRINT_MAGCMP__(&x, &y);
    // Check 7+8: return (a.sign == 1 && _lib_crt_neq(mag_ret, 0));
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, 1)) & (mag_ret)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, 1)) & (_lib_crt_neq(mag_ret, 0))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 9+10: return (a.sign == -1 && mag_ret >= 0);
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, -1)) & _lib_crt_isneg(mag_ret)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, -1)) & (_lib_crt_ispos(mag_ret))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    /* Post-operation Aggresive Cleanup */
    if (err != NULL) *err = ret_stat; // Conditional Branching here is aceptable
    ret_stat = 0; curr = 0; mag_ret = 0; return (bool)(ret);
}
bool crint_mequal(crint x, crint y, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT(((crint_validate(x)) && (crint_validate(y))), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (y.limbs != NULL) *(y.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); crint_free(&y);
        x.limbs = 0; x.cap = 0; x.n = 0; 
        x.poisoned = 0; x.sign = 0;
        y.limbs = 0; y.cap = 0; y.n = 0;
        y.poisoned = 0; y.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((x.poisoned), poisoined, {});
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr;
    /* if (x.sign != y.sign) return (_lib_crt_gti64(x.sign, y.sign)) */
    // Check 1: if (x.sign != y.sign & x.sign < y.sign) return false
    CHOOSE_OPTION((curr), ((x.sign != y.sign) & (_lib_crt_lti64(x.sign, y.sign))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (x.sign != y.sign & x.sign > y.sign) return true
    CHOOSE_OPTION((curr), ((x.sign != y.sign) & (_lib_crt_gti64(x.sign, y.sign))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (x.n != y.n) return (x.sign == 1) ? (x.n > y.n) : (x.n < y.n); */
    // Check 3: if (x.n != y.n && x.sign == 1 && x.n < y.n) return false
    CHOOSE_OPTION((curr), ((x.n != y.n) & (_lib_crt_eq(x.sign, 1)) & (_lib_crt_lt(x.n, y.n))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 4: if (x.n != y.n && x.sign == 1 && x.n > y.n) return true
    CHOOSE_OPTION((curr), ((x.n != y.n) & (_lib_crt_eq(x.sign, 1)) & (_lib_crt_gt(x.n, y.n))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 5: if (x.n != y.n && x.sign == -1 && x.n > y.n) return false
    CHOOSE_OPTION((curr), ((x.n != y.n) & (_lib_crt_eq(x.sign, -1)) & (_lib_crt_gt(x.n, y.n))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 6: if (x.n != y.n && x.sign == -1 && x.n < y.n) return true
    CHOOSE_OPTION((curr), ((x.n != y.n) & (_lib_crt_eq(x.sign, -1)) & (_lib_crt_lt(x.n, y.n))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (a.limbs == b.limbs) return true; */
    CHOOSE_OPTION((curr), (_lib_crt_eq(x.limbs, y.limbs)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* Main Validation Loop */
    /* return (a.sign == 1) ? __CRINT_MAGCMP__(&x, &y) >= 0 : __CRINT_MAGCMP__(&x, &y) <= 0; */
    int8_t mag_ret = __CRINT_MAGCMP__(&x, &y);
    // Check 7+8: return (a.sign == 1 && mag_ret >= 0);
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, 1)) & _lib_crt_isneg(mag_ret)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, 1)) & (_lib_crt_ispos(mag_ret))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 9+10: return (a.sign == -1 && mag_ret <= 0);
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, -1)) & (_lib_crt_gti64(mag_ret, 0))), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, -1)) & (_lib_crt_leq(mag_ret, 0))), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    /* Post-operation Aggresive Cleanup */
    if (err != NULL) *err = ret_stat; // Conditional Branching here is aceptable
    ret_stat = 0; curr = 0;  mag_ret = 0; return (bool)(ret);
}




//* ================================================= COPIES ================================================= */
/* -------------  Mutative SMALL Copies ------------- */
dnml_status crint_mut_copyu64(crint *dst, const uint64_t src) {
    // Pre-operation Validation & Static Analysis
    DNML_ASSERT(__STORAGE_VAL__(dst), store_inval, {
        if (dst->limbs != NULL) *(dst->limbs) = 0;
        dst->limbs = 0; dst->cap = 0; dst->n = 0; 
        dst->poisoned = 0; dst->sign = 0;
    });
    DNML_TEST_ASSERT((dst->poisoned), poisoined, {});
    dnml_status ret_stat = CRINT_SUCCESS; uint8_t noop_toggle = false;
    CHOOSE_OPTION((ret_stat), (dst->poisoned), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((noop_toggle), (dst->poisoned), (true), (noop_toggle));
    /* Main Operation - Copy */
    crint_canonicalize(dst); uint64_t val = dst->limbs[0];
    CHOOSE_OPTION((dst->limbs[0]), (noop_toggle), (val), (0));
    CHOOSE_OPTION((dst->n), (noop_toggle), (dst->n), (!!(src)));
    CHOOSE_OPTION((dst->sign), (noop_toggle), (dst->sign), (1));
    __libdnml_smemwipe_u64(dst->limbs, dst->cap, 1, dst->cap - 1, noop_toggle);
    val = 0; return ret_stat;
}
dnml_status crint_mut_dcopyu64(crint *dst, const uint64_t src) {
    // Pre-operation Validation & Static Analysis
    DNML_ASSERT(__STORAGE_VAL__(dst), store_inval, {
        if (dst->limbs != NULL) *(dst->limbs) = 0;
        dst->limbs = 0; dst->cap = 0; dst->n = 0; 
        dst->poisoned = 0; dst->sign = 0;
    });
    DNML_TEST_ASSERT((dst->poisoned), poisoined, {});
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (dst->poisoned), (CRINT_POISON), (ret_stat));

    /* Main Operation - Copy */
    crint_canonicalize(dst); limb_t TMP_P = 1;
    limb_t *__BUFFER_P = realloc(dst->limbs, U64_BYTES);
    DNML_TEST_ASSERT(__BUFFER_P != NULL, realloc_null, {});
    CHOOSE_OPTION((ret_stat), 
        (_lib_crt_eq(__BUFFER_P, NULL) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), 
        (DNML_ALLOC_OOM), (CRINT_SUCCESS)
    );
    uint64_t oom; CHOOSE_OPTION((oom), (_lib_crt_eq(__BUFFER_P, NULL)), (0), (UINT64_MAX));
    CHOOSE_OPTION((dst->limbs), (_lib_crt_eq(__BUFFER_P, NULL)), ((uintptr_t)(&TMP_P)), ((uintptr_t)__BUFFER_P));
    dst->limbs[0] = src; dst->n = !!(src); dst->cap = 1; dst->sign = 1;
    /* Invalid Metadata Fill & Aggresive Cleanup */ 
    dst->limbs[0] &= oom; dst->n &= oom; dst->cap &= oom;
    dst->sign &= oom; dst->poisoned &= oom;
    TMP_P = 0; __BUFFER_P = 0; oom = 0; return ret_stat;
}
dnml_status crint_mut_copyi64(crint *dst, const int64_t src) {
    // Pre-operation Validation & Static Analysis
    DNML_ASSERT(__STORAGE_VAL__(dst),
    "Partial Contract Violation: CryptInt invalid for storage "
    "(-Ecrypt_int_sinvalid)", {
        if (dst->limbs != NULL) *(dst->limbs) = 0;
        dst->limbs = 0; dst->cap = 0; dst->n = 0; 
        dst->poisoned = 0; dst->sign = 0;
    });
    DNML_TEST_ASSERT((dst->poisoned), poisoined, {});
    dnml_status ret_stat = CRINT_SUCCESS; uint8_t noop_toggle = false;
    CHOOSE_OPTION((ret_stat), (dst->poisoned), (CRINT_POISON), (ret_stat))
    CHOOSE_OPTION((noop_toggle), (dst->poisoned), (true), (noop_toggle));
    /* Main Operation - Copy */
    crint_canonicalize(dst); int8_t vsign;
    CHOOSE_OPTION((vsign), (_lib_crt_isneg(src)), (-1), (1));
    uint64_t val = __MAG_I64__(dst->limbs[0]);
    CHOOSE_OPTION((dst->limbs[0]), (noop_toggle), (val), (0));
    CHOOSE_OPTION((dst->n), (noop_toggle), (dst->n), (!!(src)));
    CHOOSE_OPTION((dst->sign), (noop_toggle), (dst->sign), (vsign));
    __libdnml_smemwipe_u64(dst->limbs, dst->cap, 1, dst->cap - 1, noop_toggle);
    val = 0; return ret_stat;
}
dnml_status crint_mut_dcopyi64(crint *dst, const int64_t src) {
    // Pre-operation Validation & Static Analysis
    DNML_ASSERT(__STORAGE_VAL__(dst), store_inval, {
        if (dst->limbs != NULL) *(dst->limbs) = 0;
        dst->limbs = 0; dst->cap = 0; dst->n = 0; 
        dst->poisoned = 0; dst->sign = 0;
    });
    DNML_TEST_ASSERT((dst->poisoned), poisoined, {});
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (dst->poisoned), (CRINT_POISON), (ret_stat));

    /* Main Operation - Copy */
    crint_canonicalize(dst); limb_t TMP_P = 1;
    limb_t *__BUFFER_P = realloc(dst->limbs, U64_BYTES);
    DNML_TEST_ASSERT(__BUFFER_P != NULL, realloc_null, {});
    CHOOSE_OPTION((ret_stat), 
        (_lib_crt_eq(__BUFFER_P, NULL) & 
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), 
        (DNML_ALLOC_OOM), (CRINT_SUCCESS)
    );
    uint64_t oom; CHOOSE_OPTION((oom), (_lib_crt_eq(__BUFFER_P, NULL)), (0), (UINT64_MAX));
    CHOOSE_OPTION((dst->limbs), (_lib_crt_eq(__BUFFER_P, NULL)), ((uintptr_t)(&TMP_P)), ((uintptr_t)__BUFFER_P));
    dst->limbs[0] = __MAG_I64__(src); dst->n = !!(src); dst->cap = 1;
    CHOOSE_OPTION((dst->sign), (_lib_crt_isneg(src)), (-1), (1));
    /* Invalid Metadata Fill */ 
    dst->limbs[0] &= oom; dst->n &= oom; dst->cap &= oom;
    dst->sign &= oom; dst->poisoned &= oom;
    TMP_P = 0; __BUFFER_P = 0; oom = 0; return ret_stat;
}
/* -------------  Mutative LARGE Copies ------------- */
dnml_status crint_mut_copyf128(crint *dst, long double src) {}
dnml_status crint_mut_dcopyf128(crint *dst, long double src) {}
dnml_status crint_mut_ocopyf128(crint *dst, long double src) {}
dnml_status crint_mut_tover_copyf128(crint *dst, long double src) {}
dnml_status crint_mut_copy(crint *dst, crint src) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(src)), full_contract,
    { /* Set everything to 0 to completely wipe out memory */
        if (src.limbs != NULL) *(src.limbs) = 0;
        crint_free(&src); dst->limbs = 0; dst->n = 0;
        dst->cap = 0; dst->sign = 0; dst->poisoned = 0;
    });
    DNML_ASSERT((__STORAGE_VAL__(dst)), store_inval,
    { /* Set everything to 0 to completely wipe out memory */
        __libdnml_smemwipe_u64(src.limbs, src.cap, 0, src.cap - 1, false);
        crint_free(&src); dst->limbs = 0; dst->n = 0;
        dst->cap = 0; dst->sign = 0; dst->poisoned = 0;
    }); DNML_TEST_ASSERT((dst->poisoned) && ((src.poisoned)), poisoined, {});
    dnml_status ret_stat = CRINT_SUCCESS; uint8_t noop_toggle = false;
    CHOOSE_OPTION((ret_stat), (dst->poisoned), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((noop_toggle), (dst->poisoned), (true), (noop_toggle));

    /* Main Operation - Copy */
    crint_canonicalize(dst); dnml_status rcap_stat; uint64_t correctly_set;
    if (_lib_crt_lt(dst->cap, src.n)) {
        rcap_stat = crint_reserve(dst, src.n);
        CHOOSE_OPTION((ret_stat), 
            (_lib_crt_eq(rcap_stat, DNML_ALLOC_OOM) &
            (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
            (rcap_stat), (ret_stat)
        );
    }
    CHOOSE_OPTION((correctly_set), (ret_stat != DNML_ALLOC_OOM), (UINT64_MAX), (0));
    limb_t dst_buf[FAKE_BUF_CAP] = {0}, src_buf[FAKE_BUF_CAP] = {0};
    size_t end, src_size; limb_t *dst_limbs, src_limbs;
    CHOOSE_OPTION((dst_limbs), (ret_stat != DNML_ALLOC_OOM), ((uintptr_t)(dst->limbs)), ((uintptr_t)dst_buf));
    CHOOSE_OPTION((src_limbs), (ret_stat != DNML_ALLOC_OOM), ((uintptr_t)(src.limbs)), ((uintptr_t)src_buf));
    CHOOSE_OPTION((dst->cap), (ret_stat != DNML_ALLOC_OOM), (dst->cap), (FAKE_BUF_CAP));
    CHOOSE_OPTION((src_size), (ret_stat != DNML_ALLOC_OOM), (src.n), (FAKE_BUF_CAP));
    CHOOSE_OPTION((end), (ret_stat != DNML_ALLOC_OOM), (src.n), (FAKE_BUF_CAP));
    CHOOSE_OPTION((end), (!end), (0), (end - 1));
    dst->n = src.n; dst->sign = src.sign;
    __libdnml_smemcpy_u64(dst_limbs, src_limbs, dst->cap, src_size, 0, end, (noop_toggle & rcap_stat));
    size_t clear_start, clear_end;
    CHOOSE_OPTION((clear_start), (ret_stat != DNML_ALLOC_OOM), (dst->n), (0));
    CHOOSE_OPTION((clear_end), (ret_stat != DNML_ALLOC_OOM), (dst->cap), (FAKE_BUF_CAP));
    __libdnml_smemwipe_u64(dst_limbs, dst->cap, clear_start, clear_end - 1, (noop_toggle & rcap_stat != DNML_ALLOC_OOM));
    
    /* Invalid Metadata Wiping & Aggressive  */
    dst->poisoned &= correctly_set; dst->sign &= correctly_set;
    dst->n &= correctly_set; dst->cap &= correctly_set;
    rcap_stat = 0; correctly_set = 0;
    __libdnml_smemwipe_u64(dst_buf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(src_buf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    end = 0; dst_limbs = 0; src_limbs = 0;
    clear_start = 0; clear_end = 0; return ret_stat;
}
dnml_status crint_mut_dcopy(crint *dst, crint src) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(src)), full_contract,
    { /* Set everything to 0 to completely wipe out memory */
        if (src.limbs != NULL) *(src.limbs) = 0;
        crint_free(&src); dst->limbs = 0; dst->n = 0;
        dst->cap = 0; dst->sign = 0; dst->poisoned = 0;
    });
    DNML_ASSERT((__STORAGE_VAL__(dst)), store_inval,
    { /* Set everything to 0 to completely wipe out memory */
        __libdnml_smemwipe_u64(src.limbs, src.cap, 0, src.cap - 1, false);
        crint_free(&src); dst->limbs = 0; dst->n = 0;
        dst->cap = 0; dst->sign = 0; dst->poisoned = 0;
    }); DNML_TEST_ASSERT(((dst->poisoned) && (src.poisoned)), poisoined, {});
    dnml_status ret_stat = CRINT_SUCCESS; uint8_t noop_toggle = false;
    CHOOSE_OPTION((ret_stat), (dst->poisoned | src.poisoned), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((noop_toggle), (dst->poisoned | src.poisoned), (true), (noop_toggle));

    /* Main Operation - Copy */
    crint_canonicalize(dst); dnml_status resize_stat;
    resize_stat = crint_dnml_resize(dst, src.n);
    CHOOSE_OPTION((ret_stat), 
        (_lib_crt_eq(resize_stat, DNML_ALLOC_OOM) & 
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), 
        (resize_stat), (ret_stat)
    );
    uint64_t oom_filter = (uint64_t)(-(int64_t)(ret_stat != DNML_ALLOC_OOM));
    limb_t dst_buf[FAKE_BUF_CAP] = {0}, src_buf[FAKE_BUF_CAP] = {0}; size_t end, src_size; limb_t *dst_limbs, src_limbs;
    CHOOSE_OPTION((dst_limbs), (ret_stat != DNML_ALLOC_OOM), ((uintptr_t)(dst->limbs)), ((uintptr_t)dst_buf));
    CHOOSE_OPTION((src_limbs), (ret_stat != DNML_ALLOC_OOM), ((uintptr_t)(src.limbs)), ((uintptr_t)src_buf));
    CHOOSE_OPTION((dst->cap), (ret_stat != DNML_ALLOC_OOM), (dst->cap), (FAKE_BUF_CAP));
    CHOOSE_OPTION((src_size), (ret_stat != DNML_ALLOC_OOM), (src.n), (FAKE_BUF_CAP));
    CHOOSE_OPTION((end), (ret_stat != DNML_ALLOC_OOM), (src.n), (FAKE_BUF_CAP));
    CHOOSE_OPTION((end), (!end), (0), (end - 1)); dst->n = src.n; dst->sign = src.sign;
    __libdnml_smemcpy_u64(dst_limbs, src_limbs, dst->cap, src_size, 0, end, (noop_toggle & resize_stat != DNML_ALLOC_OOM));
    /* Clearing Out Values to Invalidity & Aggresive Cleanup */
    dst->n &= oom_filter; dst->cap &= oom_filter;
    dst->poisoned &= oom_filter; dst->sign &= oom_filter;
    resize_stat = 0; oom_filter = 0; end = 0;
    __libdnml_smemwipe_u64(dst_buf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(src_buf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    dst_limbs = 0; src_limbs = 0; return ret_stat;
}
dnml_status crint_mut_ocopy(crint *dst, crint src) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(src)), full_contract,
    { /* Set everything to 0 to completely wipe out memory */
        if (src.limbs != NULL) *(src.limbs) = 0;
        crint_free(&src); dst->limbs = 0; dst->n = 0;
        dst->cap = 0; dst->sign = 0; dst->poisoned = 0;
    });
    DNML_ASSERT((__STORAGE_VAL__(dst)), store_inval,
    { /* Set everything to 0 to completely wipe out memory */
        __libdnml_smemwipe_u64(src.limbs, src.cap, 0, src.cap - 1, false);
        crint_free(&src); dst->limbs = 0; dst->n = 0;
        dst->cap = 0; dst->sign = 0; dst->poisoned = 0;
    }); DNML_TEST_ASSERT(((dst->poisoned) && ((src.poisoned))), poisoined, {});
    dnml_status ret_stat = CRINT_SUCCESS; uint8_t noop_toggle = false;
    CHOOSE_OPTION((ret_stat), (dst->poisoned | src.poisoned), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((noop_toggle), (dst->poisoned | src.poisoned), (true), (noop_toggle));

    /* Main Operation - Copy */
    CHOOSE_OPTION((ret_stat), ((_lib_crt_lt(dst->cap, src.n)) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_ERR_RANGE), (ret_stat));
    CHOOSE_OPTION((noop_toggle), ((_lib_crt_lt(dst->cap, src.n)) & (!noop_toggle)), (true), (noop_toggle));
    size_t end; CHOOSE_OPTION((end), (!src.n), (0), (src.n - 1));
    __libdnml_smemcpy_u64(dst->limbs, src.limbs, dst->cap, src.n, 0, end, noop_toggle);
    dst->n = src.n; dst->sign = src.sign;
    __libdnml_smemwipe_u64(dst->limbs, dst->cap, dst->n, dst->cap - 1, noop_toggle);
    /* Aggresive Post-Operation Clean-up */ noop_toggle = 0; noop_toggle = 0; return ret_stat;
}
dnml_status crint_mut_tover_copy(crint *dst, crint src) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(src)), full_contract,
    { /* Set everything to 0 to completely wipe out memory */
        if (src.limbs != NULL) *(src.limbs) = 0;
        crint_free(&src); dst->limbs = 0; dst->n = 0;
        dst->cap = 0; dst->sign = 0; dst->poisoned = 0;
    });
    DNML_ASSERT((__STORAGE_VAL__(dst)), store_inval,
    { /* Set everything to 0 to completely wipe out memory */
        __libdnml_smemwipe_u64(src.limbs, src.cap, 0, src.cap - 1, false);
        crint_free(&src); dst->limbs = 0; dst->n = 0;
        dst->cap = 0; dst->sign = 0; dst->poisoned = 0;
    }); DNML_TEST_ASSERT(((dst->poisoned) && (!(src.poisoned))), poisoined, {});
    dnml_status ret_status = CRINT_SUCCESS; uint8_t noop_toggle = false;
    CHOOSE_OPTION((ret_status), (dst->poisoned | src.poisoned), (CRINT_POISON), (ret_status));
    CHOOSE_OPTION((noop_toggle), (dst->poisoned | src.poisoned), (true), (noop_toggle));

    /* Main Operation - Copy */
    size_t op_range; CHOOSE_OPTION((op_range), (_lib_crt_lt(dst->cap, src.n)), (dst->cap), (src.n));
    size_t end; CHOOSE_OPTION((end), (!op_range), (0), (op_range - 1));
    __libdnml_smemcpy_u64(dst->limbs, src.limbs, dst->cap, src.n, 0, op_range, noop_toggle);
    dst->n = op_range; dst->sign = src.sign;
    __libdnml_smemwipe_u64(dst->limbs, dst->cap, dst->n, dst->cap - 1, noop_toggle);
    /* Aggrestive Post-Operation Clean-up */ op_range = 0; noop_toggle = 0; return ret_status;
}
/* -------------  Functional SMALL Copies ------------- */
crint crint_copyu64(const uint64_t src, dnml_status *err) {
    crint dst; dnml_status new_stat; uint64_t correctly_set;
    limb_t dst_tmp_p[1] = {0}; new_stat = crint_new(&dst);
    CHOOSE_OPTION((dst.limbs), (_lib_crt_neq(new_stat, DNML_ALLOC_OOM)), ((uintptr_t)dst.limbs), ((uintptr_t)dst_tmp_p));
    CHOOSE_OPTION((correctly_set), (_lib_crt_neq(new_stat, DNML_ALLOC_OOM)), (UINT64_MAX), (0));
    /* Setting Up correctly - Standard Case */
    dst.limbs[0] = src; dst.n = !!(src);
    dst.cap = 1; dst.sign = 1;
    /* Setting Up invalid metadata - DNML_ALLOC_OOM */
    dst.limbs[0] &= correctly_set; dst.n &= correctly_set;
    dst.cap &= correctly_set; dst.sign &= correctly_set;
    if (err != NULL) {
        CHOOSE_OPTION((*err), (_lib_crt_neq(new_stat, DNML_ALLOC_OOM)), (CRINT_SUCCESS), (DNML_ALLOC_OOM));
    } dst_tmp_p[0] = 0; new_stat = 0; correctly_set = 0; return dst;
}
crint crint_copyi64(const int64_t src, dnml_status *err) {
    crint dst; dnml_status new_stat; uint64_t correctly_set;
    limb_t dst_tmp_p[1] = {0}; new_stat = crint_new(&dst);
    CHOOSE_OPTION((dst.limbs), (_lib_crt_neq(new_stat, DNML_ALLOC_OOM)), ((uintptr_t)dst.limbs), ((uintptr_t)dst_tmp_p));
    CHOOSE_OPTION((correctly_set), (_lib_crt_neq(new_stat, DNML_ALLOC_OOM)), (UINT64_MAX), (0));
    /* Setting Up correctly - Standard Case */
    dst.limbs[0] = __MAG_I64__(src); dst.n = !!(src); dst.cap = 1;
    CHOOSE_OPTION((dst.sign), (_lib_crt_isneg(src)), (-1), (1));
    /* Setting Up invalid metadata - DNML_ALLOC_OOM */
    dst.limbs[0] &= correctly_set; dst.n &= correctly_set;
    dst.cap &= correctly_set; dst.sign &= correctly_set;
    if (err != NULL) {
        CHOOSE_OPTION((*err), (_lib_crt_neq(new_stat, DNML_ALLOC_OOM)), (CRINT_SUCCESS), (DNML_ALLOC_OOM));
    } dst_tmp_p[0] = 0; new_stat = 0; correctly_set = 0; return dst;
}
/* -------------  Functional LARGE Copies ------------- */
crint crint_copyf128(long double src, dnml_status *err) {}
crint crint_ocopyf128(long double src, size_t output_cap, dnml_status *err) {}
crint crint_tover_copyf128(long double src, size_t output_cap, dnml_status *err) {}
crint crint_copy(crint src, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(src)), full_contract,
    { /* Set everything to 0 to completely wipe out memory */
        if (src.limbs != NULL) *(src.limbs) = 0;
        crint_free(&src);
    });
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((src.poisoned), poisoined, {});
    dnml_status ret_stat = CRINT_SUCCESS; uint8_t noop_toggle = false;
    CHOOSE_OPTION((ret_stat), (src.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((noop_toggle), (src.poisoned), (true), (noop_toggle));

    /* Main Operation - Copy */
    crint dst; dnml_status new_stat; uint64_t correctly_set;
    limb_t dst_tmp_p[FAKE_BUF_CAP] = {0}, src_tmp_p[FAKE_BUF_CAP] = {0}; limb_t* src_limbs;
    new_stat = crint_snew(&dst, src.n); // Guaraneed dst->cap >= 1
    CHOOSE_OPTION((dst.limbs), (_lib_crt_neq(new_stat, DNML_ALLOC_OOM)), ((uintptr_t)dst.limbs), ((uintptr_t)dst_tmp_p));
    CHOOSE_OPTION((correctly_set), (_lib_crt_neq(new_stat, DNML_ALLOC_OOM)), (UINT64_MAX), (0));
    CHOOSE_OPTION((src_limbs), (_lib_crt_neq(new_stat, DNML_ALLOC_OOM)), ((uintptr_t)src.limbs), ((uintptr_t)src_tmp_p));

    /* Setting Up correctly - Standard Case */
    size_t iter_cnt = (size_t)(src.n / FAKE_BUF_CAP + 1);
    size_t end; CHOOSE_OPTION( (end), (!(src.n)), 0, (src.n - 1));
    CHOOSE_OPTION((end), (_lib_crt_neq(new_stat, DNML_ALLOC_OOM)), (end), (511));
    CHOOSE_OPTION((dst.cap), (_lib_crt_neq(new_stat, DNML_ALLOC_OOM)), (dst.cap), (FAKE_BUF_CAP));
    while (iter_cnt--) __libdnml_smemcpy_u64(
        dst.limbs, src.limbs, dst.cap, src.n, 
        0, end, (noop_toggle & _lib_crt_neq(new_stat, DNML_ALLOC_OOM))
    ); dst.n = src.n; dst.sign = src.sign; *err = CRINT_SUCCESS;

    /* Setting Up invalid metadata + Agressive Stack Cleanup */
    dst.limbs[0] &= correctly_set; dst.n &= correctly_set;
    dst.cap &= correctly_set; dst.sign &= correctly_set;
    CHOOSE_OPTION((ret_stat), (_lib_crt_neq(new_stat, DNML_ALLOC_OOM) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (new_stat), (ret_stat));
    if (err != NULL) *err = ret_stat; // Conditional Branching here is acceptable
    new_stat = 0; correctly_set = 0; noop_toggle = 0;
    __libdnml_smemwipe_u64(dst_tmp_p, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(src_tmp_p, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    src_limbs = 0; iter_cnt = 0; end = 0; ret_stat = 0; return dst;
}
crint crint_ocopy(crint src, size_t output_cap, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(src)), full_contract,
    { /* Set everything to 0 to completely wipe out memory */
        if (src.limbs != NULL) *(src.limbs) = 0;
        crint_free(&src);
    });
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((src.poisoned), poisoined, {});
    dnml_status ret_stat = CRINT_SUCCESS; uint8_t noop_toggle = false;
    CHOOSE_OPTION((ret_stat), (src.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((noop_toggle), (src.poisoned & (!(noop_toggle))), (true), (noop_toggle));

    /* Main Operation - Copy */
    crint dst; uint64_t correctly_set; dnml_status new_stat;
    CHOOSE_OPTION((ret_stat), (_lib_crt_lt(output_cap, src.n)), (CRINT_ERR_RANGE), (ret_stat));
    CHOOSE_OPTION((noop_toggle), ((_lib_crt_lt(output_cap, src.n)) & (!(noop_toggle))), (true), (noop_toggle));
    limb_t dst_tmp_p[FAKE_BUF_CAP] = {0}, src_tmp_p[FAKE_BUF_CAP] = {0}; new_stat = crint_snew(&dst, src.n); // Guaraneed dst->cap >= 1
    CHOOSE_OPTION((ret_stat), ((new_stat != CRINT_SUCCESS) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (new_stat), (ret_stat));
    CHOOSE_OPTION((correctly_set), (ret_stat != DNML_ALLOC_OOM), (UINT64_MAX), (0)); limb_t* dst_limbs, src_limbs;
    CHOOSE_OPTION((dst_limbs), (_lib_crt_neq(new_stat, DNML_ALLOC_OOM)), ((uintptr_t)dst.limbs), ((uintptr_t)dst_tmp_p));
    CHOOSE_OPTION((src_limbs), (_lib_crt_neq(new_stat, DNML_ALLOC_OOM)), ((uintptr_t)src.limbs), ((uintptr_t)src_tmp_p));

    /* Setting Up correctly - Standard Case */
    size_t iter_cnt = (size_t)(src.n / FAKE_BUF_CAP + 1);
    size_t end; CHOOSE_OPTION( (end), (!(src.n)), 0, (src.n - 1));
    CHOOSE_OPTION((end), (ret_stat != DNML_ALLOC_OOM), (end), (511));
    CHOOSE_OPTION((dst.cap), (ret_stat != DNML_ALLOC_OOM), (dst.cap), (FAKE_BUF_CAP));
    while (iter_cnt--) __libdnml_smemcpy_u64(
        dst_limbs, src_limbs, dst.cap, src.n, 
        0, end, (noop_toggle & (_lib_crt_neq(new_stat, DNML_ALLOC_OOM)))
    ); dst.n = src.n; dst.sign = src.sign;

    /* Setting Up invalid metadata + Agressive Stack Cleanup */
    dst.limbs[0] &= correctly_set; dst.n &= correctly_set;
    dst.cap &= correctly_set; dst.sign &= correctly_set;
    if (err != NULL) *err = ret_stat; // Conditional Branching here is acceptable
    ret_stat = 0; correctly_set = 0; new_stat = 0; noop_toggle = 0;
    __libdnml_smemwipe_u64(dst_tmp_p, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(src_tmp_p, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    dst_limbs = 0; src_limbs = 0; iter_cnt = 0; end = 0;
    return dst;
}
crint crint_tover_copy(crint src, size_t output_cap, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(src)), full_contract,
    { /* Set everything to 0 to completely wipe out memory */
        if (src.limbs != NULL) *(src.limbs) = 0;
        crint_free(&src);
    });
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((src.poisoned), poisoined, {});
    dnml_status ret_stat = CRINT_SUCCESS; uint8_t noop_toggle = false;
    CHOOSE_OPTION((ret_stat), (src.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((noop_toggle), (src.poisoned), (true), (noop_toggle));

    /* Main Operation - Copy */
    crint dst; dnml_status new_stat; uint64_t correctly_set;
    size_t op_range; CHOOSE_OPTION((op_range), (_lib_crt_lt(output_cap, src.n)), (output_cap), (src.n));
    limb_t dst_tmp_p[FAKE_BUF_CAP] = {0}, src_tmp_p[FAKE_BUF_CAP] = {0}; new_stat = crint_snew(&dst, src.n); // Guaraneed dst->cap >= 1
    CHOOSE_OPTION((correctly_set), (_lib_crt_neq(new_stat, DNML_ALLOC_OOM)), (UINT64_MAX), (0)); limb_t* dst_limbs, src_limbs;
    CHOOSE_OPTION((dst_limbs), (_lib_crt_neq(new_stat, DNML_ALLOC_OOM)), ((uintptr_t)dst.limbs), ((uintptr_t)dst_tmp_p));
    CHOOSE_OPTION((src_limbs), (_lib_crt_neq(new_stat, DNML_ALLOC_OOM)), ((uintptr_t)src.limbs), ((uintptr_t)src_tmp_p));

    /* Setting Up correctly - Standard Case */
    size_t iter_cnt = (size_t)(src.n / FAKE_BUF_CAP + 1); size_t end;
    CHOOSE_OPTION((end), (!(op_range)), 0, (op_range - 1));
    CHOOSE_OPTION((end), (_lib_crt_neq(new_stat, DNML_ALLOC_OOM)), (end), (511));
    CHOOSE_OPTION((dst.cap), (_lib_crt_neq(new_stat, DNML_ALLOC_OOM)), (dst.cap), (FAKE_BUF_CAP));
    while (iter_cnt--) __libdnml_smemcpy_u64(
        dst.limbs, src.limbs, dst.cap, src.n,
        0, end, (noop_toggle & (_lib_crt_neq(new_stat, DNML_ALLOC_OOM)))
    ); 
    dst.n = src.n; dst.sign = src.sign;

    /* Setting Up invalid metadata - DNML_ALLOC_OOM */
    dst.limbs[0] &= correctly_set; dst.n &= correctly_set;
    dst.cap &= correctly_set; dst.sign &= correctly_set;
    CHOOSE_OPTION((ret_stat), (new_stat != CRINT_SUCCESS & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (new_stat), (ret_stat));
    if (err != NULL) *err = ret_stat; // Conditional Branching here is acceptable
    ret_stat = 0; new_stat = 0; correctly_set = 0; op_range = 0;
    __libdnml_smemwipe_u64(dst_tmp_p, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false); dst_limbs = 0;
    __libdnml_smemwipe_u64(src_tmp_p, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false); src_limbs = 0;
    iter_cnt = 0; end = 0; noop_toggle = 0; return dst;
}




//* ===================================== STATE ALTERATION FUNCTIONS ===================================== *//
void crint_canonicalize(crint *x) {
    // Fix invalid capacity
    uint8_t cap_invalid = (_lib_crt_lt(x->cap, 1));
    CT_COND_ASSIGN(x->cap, cap_invalid, 1);
    CT_COND_ASSIGN(x->n, cap_invalid, 0);
    CT_COND_ASSIGN(x->sign, cap_invalid, 1);
    
    // Clamp n to capacity
    uint8_t n_overflow = (_lib_crt_gt(x->n, x->cap));
    CT_COND_ASSIGN(x->n, n_overflow, x->cap);
    
    // Fix invalid sign
    uint8_t sign_invalid = (x->sign != 1) & (x->sign != -1);
    CT_COND_ASSIGN(x->n, sign_invalid, 0);
    CT_COND_ASSIGN(x->sign, sign_invalid, 1);
    cap_invalid = 0; n_overflow = 0; sign_invalid = 0;
}
void crint_normalize(crint *x) { __CRINT_TRIM_LZ__(x); CHOOSE_OPTION((x->sign), (!x->n), (1), (-1)); }
dnml_status crint_resize(crint *x, size_t k) {
    // Pre-operation Validation & Static Analysis
    DNML_ASSERT(__STORAGE_VAL__(x), store_inval, {
        x->limbs = 0; x->cap = 0; x->n = 0; 
        x->poisoned = 0; x->sign = 0;
    }); 
    DNML_TEST_ASSERT((x->poisoned), poisoined, {});
    DNML_TEST_ASSERT((k), "Invalid Capacity Request (-Einval_cap_request)", {});
    dnml_status ret_stat = CRINT_SUCCESS; uint64_t oom_mask = UINT64_MAX;
    CHOOSE_OPTION((ret_stat), (x->poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));


    /* Memory Clearance on Resizing to a smaller size */
    size_t start, end, op_cap, dist;
    CHOOSE_OPTION((dist), (_lib_crt_lt(k, x->cap)), (x->cap - k), (k - x->cap))
    CHOOSE_OPTION((start), (_lib_crt_lt(k, x->cap)), (k - 1), (0));
    CHOOSE_OPTION((end), (_lib_crt_lt(k, x->cap)), (x->cap - 1), (__clamp_size(x->cap, dist - 1)));
    __libdnml_smemwipe_u64(x->limbs, x->cap, start, end, (x->poisoned | (!(_lib_crt_lt(k, x->cap)))));

    /* Main Resizing */ limb_t* operated;
    CHOOSE_OPTION((operated), (_lib_crt_eq(ret_stat, CRINT_POISON)), ((uintptr_t)(malloc(1))), ((uintptr_t)x->limbs));
    size_t normalized_size; NORMALIZE_0_TO_1(normalized_size, k); size_t op_size; 
    CHOOSE_OPTION((op_size), (_lib_crt_eq(ret_stat, CRINT_POISON)), (1), (normalized_size));
    limb_t *__BUFFER_P = realloc(operated, op_size * U64_BYTES);
    DNML_TEST_ASSERT((__BUFFER_P != NULL), realloc_null, { if (operated != x->limbs) free(operated); });
    CHOOSE_OPTION((ret_stat), ((_lib_crt_eq(__BUFFER_P, NULL)) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (DNML_ALLOC_OOM), (ret_stat));
    CHOOSE_OPTION((oom_mask), ((_lib_crt_eq(__BUFFER_P, NULL)) & (ret_stat != CRINT_POISON)), (0), (oom_mask));

    CHOOSE_OPTION((x->limbs), (ret_stat != CRINT_POISON), ((uintptr_t)(__BUFFER_P) & oom_mask), ((uintptr_t)x->limbs));
    CHOOSE_OPTION((x->cap), (ret_stat != CRINT_POISON), (normalized_size & oom_mask), (x->cap));
    CHOOSE_OPTION((x->n), (ret_stat != CRINT_POISON), (crtmin(normalized_size, x->cap) & oom_mask), (x->n));
    /* Post-operation Aggrestive Clearance */
    if (_lib_crt_eq(ret_stat, CRINT_POISON) & __BUFFER_P != NULL) free(__BUFFER_P);
    start = 0; end = 0; op_cap = 0; operated = 0; oom_mask = 0;
    __BUFFER_P = 0; normalized_size = 0; op_size = 0; return ret_stat;
}
dnml_status crint_reserve(crint *x, size_t k) {
    // Pre-operation Validation & Static Analysis
    DNML_ASSERT(__STORAGE_VAL__(x), store_inval, {
        x->limbs = 0; x->cap = 0; x->n = 0;
        x->poisoned = 0; x->sign = 0;
    });
    DNML_TEST_ASSERT((x->poisoned), poisoined, {});
    dnml_status ret_stat = CRINT_SUCCESS; uint64_t oom_mask = UINT64_MAX;
    CHOOSE_OPTION((ret_stat), (x->poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));

    /* Main Resizing */ limb_t *operated;
    size_t new_cap = x->cap; while (_lib_crt_lt(new_cap, k)) new_cap *= 2;
    CHOOSE_OPTION((operated), (
        _lib_crt_eq(ret_stat, CRINT_POISON) | _lib_crt_eq(new_cap, x->cap)),
        ((uintptr_t)(malloc(U64_BYTES))), ((uintptr_t)x->limbs)
    );
    limb_t* __BUFFER_P = realloc(operated, new_cap * U64_BYTES);
    DNML_TEST_ASSERT(__BUFFER_P != NULL, realloc_null, {});
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(__BUFFER_P, NULL)) & (_lib_crt_neq(new_cap, x->cap)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (DNML_ALLOC_OOM), (ret_stat)
    );
    CHOOSE_OPTION((oom_mask), (
        (_lib_crt_eq(__BUFFER_P, NULL)) & (_lib_crt_neq(new_cap, x->cap)) & 
        (ret_stat != CRINT_POISON)), (0), (oom_mask)
    );
    // DNLM_ALLOC_OOM --> Always assign new
    // CRINT_SUCCESS ---> ASSIGN NEW IF NEEDED (ACTUALLY NEW CAPACITY)
    // CRINT_POISON ----> NEVER
    uint8_t change_cap = (
        (_lib_crt_eq(ret_stat, DNML_ALLOC_OOM) | 
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS) &
         _lib_crt_eq(new_cap, x->cap)))
    );
    CHOOSE_OPTION((x->limbs), (change_cap), ((uintptr_t)(__BUFFER_P) & oom_mask), ((uintptr_t)x->limbs));
    CHOOSE_OPTION((x->cap), (change_cap), (new_cap & oom_mask), (x->cap));
    /* Post-operation Aggrestive Clearance */
    if ((_lib_crt_eq(ret_stat, CRINT_POISON) | _lib_crt_eq(new_cap, x->cap)) & __BUFFER_P != NULL) free(__BUFFER_P);
    oom_mask = 0; operated = 0; new_cap = 0; __BUFFER_P = 0; return ret_stat;
    
}
dnml_status crint_shrink(crint *x, size_t k) { /* Maximum capacity */
    // Pre-operation Validation & Static Analysis
    DNML_ASSERT(__STORAGE_VAL__(x), store_inval, {
        x->limbs = 0; x->cap = 0; x->n = 0; 
        x->poisoned = 0; x->sign = 0;
    });
    DNML_TEST_ASSERT((x->poisoned), poisoined, {});
    DNML_TEST_ASSERT((k), "Invalid Capacity Request (-Einval_cap_request)", {});
    dnml_status ret_stat = CRINT_SUCCESS; uint64_t oom_mask = UINT64_MAX;
    CHOOSE_OPTION((ret_stat), (x->poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));

    /* Memory Clearance on Resizing to a smaller size */
    size_t start, end, op_cap, new_cap = x->cap; while (_lib_crt_gt(new_cap, k)) --new_cap;
    CHOOSE_OPTION((start), (_lib_crt_lt(new_cap, x->cap)), (new_cap - 1), (0));
    CHOOSE_OPTION((end), (_lib_crt_lt(new_cap, x->cap)), (x->cap - 1), (0));
    __libdnml_smemwipe_u64(x->limbs, x->cap, start, end, (x->poisoned));

    /* Main Resizing */ limb_t* operated;
    CHOOSE_OPTION((operated), (_lib_crt_eq(ret_stat, CRINT_POISON)), ((uintptr_t)(malloc(U64_BYTES))), ((uintptr_t)x->limbs));
    size_t normalized_size; NORMALIZE_0_TO_1(normalized_size, new_cap); size_t opsize;
    CHOOSE_OPTION((opsize), (_lib_crt_eq(ret_stat, CRINT_POISON)), (1), (normalized_size));
    limb_t* __BUFFER_P = realloc(operated, opsize * U64_BYTES);
    DNML_TEST_ASSERT((__BUFFER_P != NULL), realloc_null, { if (operated != x->limbs) free(operated); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq(__BUFFER_P, NULL)) & 
        (_lib_crt_neq(new_cap, x->cap)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), 
        (DNML_ALLOC_OOM), (ret_stat)
    );
    CHOOSE_OPTION((oom_mask), (
        (_lib_crt_eq(__BUFFER_P, NULL)) & (_lib_crt_neq(new_cap, x->cap)) & 
        (ret_stat != CRINT_POISON)), (0), (oom_mask)
    );
    // DNLM_ALLOC_OOM --> Always assign new
    // CRINT_SUCCESS ---> ASSIGN NEW IF NEEDED (ACTUALLY NEW CAPACITY)
    // CRINT_POISON ----> NEVER
    uint8_t change_cap = (
        (_lib_crt_eq(ret_stat, DNML_ALLOC_OOM) | 
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS) &
         _lib_crt_eq(new_cap, x->cap)))
    );
    CHOOSE_OPTION((x->limbs), (change_cap), ((uintptr_t)(__BUFFER_P) & oom_mask), ((uintptr_t)x->limbs));
    CHOOSE_OPTION((x->cap), (change_cap), (normalized_size & oom_mask), (x->cap));
    CHOOSE_OPTION((x->n), (change_cap), (crtmin(normalized_size, x->cap) & oom_mask), (x->n));
    /* Post-operation Aggrestive Clearance */
    if ((_lib_crt_eq(ret_stat, CRINT_POISON) | _lib_crt_eq(new_cap, x->cap)) & __BUFFER_P != NULL) free(__BUFFER_P);
    start = 0; end = 0; op_cap = 0; operated = 0; oom_mask = 0;
    __BUFFER_P = 0; normalized_size = 0; opsize = 0; return ret_stat;
}
dnml_status crint_reset(crint *x) {
    DNML_ASSERT(__STORAGE_VAL__(x),
    "Partial Contract Violation: CryptInt State Invariant is violated "
    "(-Ecrypt_int_state_invalid)", {
        x->limbs = 0; x->cap = 0; x->n = 0;
        x->poisoned = 0; x->sign = 0;
    });
    DNML_TEST_ASSERT((x->poisoned), poisoined, {});
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned), (CRINT_POISON), (ret_stat));
    __libdnml_smemwipe_u64(x->limbs, x->cap, 0, x->cap - 1, (x->poisoned));
    x->n = 0; x->sign = 1;
}
static inline uint8_t __STORAGE_VAL__(crint *x) {
    if (_lib_crt_eq(x->limbs, NULL)) return 0;
    if (_lib_crt_lt(x->cap, 1)) return 0;
    return 1;
}
bool crint_validate(crint x) {
    /* State Validation */
    uint8_t ret = 0;
    ret |= (_lib_crt_eq(x.limbs, NULL)); 
    ret |= (_lib_crt_lt(x.cap, 1));
    ret |= (_lib_crt_gt(x.n, x.cap));
    limb_t fake_dst = 0; limb_t *dst;
    CHOOSE_OPTION((dst), (!(ret)), ((uintptr_t)(&fake_dst)), ((uintptr_t)(&x.limbs[x.n - 1])));
    ret |= (_lib_crt_neq(x.sign, 1) & _lib_crt_neq(x.sign, -1));
    /* Arithmetic Validation */
    ret |= (_lib_crt_eq(*dst, 0));
    ret |= (!(x.n) & (_lib_crt_neq(x.sign, -1)));
    fake_dst = 0; dst = 0; return (bool)(ret);
}
bool crint_pvalidate(crint *x) {
    /* State Validation */
    uint8_t ret = 0;
    ret |= (_lib_crt_eq(x->limbs, NULL)); 
    ret |= (_lib_crt_lt(x->cap, 1));
    ret |= (_lib_crt_gt(x->n, x->cap));
    limb_t fake_dst = 0; limb_t *dst;
    CHOOSE_OPTION((dst), (!(ret)), ((uintptr_t)(&fake_dst)), ((uintptr_t)(&x->limbs[x->n - 1])));
    ret |= (_lib_crt_neq(x->sign, 1) & _lib_crt_neq(x->sign, -1));
    /* Arithmetic Validation */
    ret |= (_lib_crt_eq(*dst, 0));
    ret |= (!(x->n) & (_lib_crt_neq(x->sign, -1)));
    fake_dst = 0; dst = 0; return (bool)(ret);
}


