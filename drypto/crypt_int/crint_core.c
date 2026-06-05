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
#include "dnml_status.h"
#include "include.h"
#include "intrinsics.h"
#include "libdnml_types.h"




//* ========================================= CONSTRUCTORS & DESTRUCTORS ===================================== *//
dnml_status crint_free(crint *x) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {;});
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; return CRINT_NULL; }
    free(x->limbs); // clang-format off
    x->limbs = NULL; x->cap = 0; x->n = 0;
    x->poisoned = 0; x->sign = 0; x = 0; return CRINT_SUCCESS; // clang-format on
}
dnml_status crint_new(crint *x) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {;});
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; return CRINT_NULL; }
    /* Actual operation */
    uint64_t oom_mask = UINT64_MAX;  dnml_status ret_stat = CRINT_SUCCESS; limb_t *__BUFFER_P = calloc(1, U64_BYTES);
    DNML_TEST_ASSERT(_lib_crt_neq((ptr_t)__BUFFER_P, (ptr_t)(NULL)), realloc_null, { crint_free(x); });
    CHOOSE_OPTION((ret_stat), (_lib_crt_eq((ptr_t)__BUFFER_P, (ptr_t)(NULL)) & _lib_crt_eq((ptr_t)x->limbs, (ptr_t)(NULL))), DNML_ALLOC_OOM, ret_stat);
    CHOOSE_OPTION((oom_mask), (_lib_crt_eq((ptr_t)__BUFFER_P, (ptr_t)(NULL)) & _lib_crt_eq((ptr_t)x->limbs, (ptr_t)(NULL))), 0, oom_mask);

    uint8_t uninit = _lib_crt_eq((ptr_t)x->limbs, (ptr_t)(NULL));
    x->limbs = (uninit) ? __BUFFER_P : x->limbs;
    CHOOSE_OPTION((x->cap), (uninit), (1 & oom_mask), (x->cap));
    CHOOSE_OPTION((x->n), (uninit), (0 & oom_mask), (x->n));
    CHOOSE_OPTION((x->sign), (uninit), (1 & oom_mask), (x->sign));
    CHOOSE_OPTION((x->poisoned), (uninit), (0 & oom_mask), (x->poisoned));
    /* Post operation Aggrestive Clearance */ // clang-format off
    oom_mask = 0; __BUFFER_P = 0; uninit = 0; x = 0; return ret_stat; // clang-format on
}
dnml_status crint_snew(crint *x, size_t n) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {;});
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; return CRINT_NULL; }
    /* Actual Operation */
    uint64_t oom_mask = UINT64_MAX; dnml_status ret_stat = CRINT_SUCCESS; size_t salloc = (n) | (!(n));
    limb_t *__BUFFER_P = calloc(salloc, U64_BYTES);
    DNML_TEST_ASSERT(_lib_crt_neq((ptr_t)__BUFFER_P, (ptr_t)(NULL)), realloc_null, { crint_free(x); });
    CHOOSE_OPTION((ret_stat), (_lib_crt_eq((ptr_t)__BUFFER_P, (ptr_t)(NULL)) & _lib_crt_eq((ptr_t)x->limbs, (ptr_t)(NULL))), DNML_ALLOC_OOM, ret_stat);
    CHOOSE_OPTION((oom_mask), (_lib_crt_eq((ptr_t)__BUFFER_P, (ptr_t)(NULL)) & _lib_crt_eq((ptr_t)x->limbs, (ptr_t)(NULL))), 0, oom_mask);

    uint8_t uninit = (_lib_crt_eq((ptr_t)(x->limbs), (ptr_t)(NULL)));
    x->limbs = (uninit) ? __BUFFER_P : x->limbs;
    CHOOSE_OPTION((x->cap), (uninit), (salloc & oom_mask), (x->cap));
    CHOOSE_OPTION((x->n), (uninit), (0 & oom_mask), (x->n));
    CHOOSE_OPTION((x->sign), (uninit), (1 & oom_mask), (x->sign));
    CHOOSE_OPTION((x->poisoned), (uninit), (0 & oom_mask), (x->poisoned));
    /* Post-operation Aggrestive Clearance */ // clang-format off
    oom_mask = 0; salloc = 0; __BUFFER_P = 0; uninit = 0; x = 0; n = 0; return ret_stat; // clang-format on
}
dnml_status crint_cinew(crint *x, crint *y) {
    // Pre-operation Validation & Static Analysis
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL)) & _lib_crt_neq((ptr_t)y, (ptr_t)(NULL))), input_null, {;});
    DNML_TEST_ASSERT((crint_pvalidate(y)), ci_full_contract, { crint_free(y); });
    DNML_TEST_ASSERT((!y->poisoned), crint_poisoned, { crint_free(y); });
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL)) | _lib_crt_eq((ptr_t)y, (ptr_t)(NULL))) { x = 0; y = 0; return CRINT_NULL; }
    if (!crint_pvalidate(y)) { x = 0; y = 0; return CRINT_ERR_INVAL; }
    dnml_status ret_stat = CRINT_SUCCESS; uint64_t mask = UINT64_MAX; uint8_t noop_toggle = false;
    CHOOSE_OPTION((ret_stat), ((y->poisoned) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((noop_toggle), (y->poisoned), (true), (noop_toggle));

    /* Main Operations */
    size_t alloc_size; CHOOSE_OPTION((alloc_size), (y->n), (y->n), (1));
    limb_t *__BUFFER_P = calloc(alloc_size, U64_BYTES);
    DNML_TEST_ASSERT(_lib_crt_neq((ptr_t)__BUFFER_P, (ptr_t)(NULL)), realloc_null, { crint_free(y); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq((ptr_t)__BUFFER_P, (ptr_t)(NULL))) &
        _lib_crt_eq((ptr_t)x->limbs, (ptr_t)(NULL)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        DNML_ALLOC_OOM, ret_stat
    );
    mask = (uint64_t)(-(int64_t)(!(
        (_lib_crt_eq((ptr_t)__BUFFER_P, (ptr_t)(NULL))) & _lib_crt_eq((ptr_t)x->limbs, (ptr_t)(NULL)) &
        (_lib_crt_eq(mask, UINT64_MAX)) & (_lib_crt_neq(ret_stat, CRINT_POISON))
    )));
    uint8_t uninit = _lib_crt_eq((ptr_t)x->limbs, (ptr_t)(NULL)); limb_t* dst_limbs;
    dst_limbs = (uninit) ? __BUFFER_P : y->limbs;
    size_t end; CHOOSE_OPTION((end), (!y->n), (0), (y->n));
    __libdnml_smemcpy_u64(dst_limbs, y->limbs, alloc_size, y->n, 0, end, false);
    x->limbs = (uninit) ? __BUFFER_P : x->limbs;
    CHOOSE_OPTION((x->cap), (uninit), (alloc_size & mask), (x->cap));
    CHOOSE_OPTION((x->sign), (uninit), (y->sign & mask), (x->sign));
    CHOOSE_OPTION((x->poisoned), (uninit), (y->poisoned & mask), (x->poisoned));
    CHOOSE_OPTION((x->n), (uninit), (y->n), (x->n)); x->sign = !(!x->n);
    /* Post-operation Aggrestive Clearance */ // clang-format off
    mask = 0; alloc_size = 0; __BUFFER_P = 0; uninit = 0;
    dst_limbs = 0; x = 0; y = 0; return ret_stat;// clang-format on
}
dnml_status crint_new_u64(crint *x, uint64_t in) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {;});
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; in = 0; return CRINT_NULL; }
    /* Actual Operation */
    uint64_t oom_mask = UINT64_MAX; dnml_status ret_stat = CRINT_SUCCESS; limb_t *__BUFFER_P = calloc(1, U64_BYTES);
    DNML_TEST_ASSERT(_lib_crt_neq((ptr_t)__BUFFER_P, (ptr_t)(NULL)), realloc_null, {;});
    CHOOSE_OPTION((ret_stat), (_lib_crt_eq((ptr_t)__BUFFER_P, (ptr_t)(NULL)) & _lib_crt_eq((ptr_t)x->limbs, (ptr_t)(NULL))), DNML_ALLOC_OOM, ret_stat);
    CHOOSE_OPTION((oom_mask), (_lib_crt_eq((ptr_t)__BUFFER_P, (ptr_t)(NULL)) & _lib_crt_eq((ptr_t)x->limbs, (ptr_t)(NULL))), 0, oom_mask);

    uint8_t uninit = _lib_crt_eq((ptr_t)x->limbs, (ptr_t)(NULL)); uint64_t first_val = x->limbs[0];
    x->limbs = (uninit) ? __BUFFER_P : x->limbs;
    CHOOSE_OPTION((x->limbs[0]), (uninit), (in & oom_mask), (first_val));
    CHOOSE_OPTION((x->cap), (uninit), (1 & oom_mask), (x->cap));
    CHOOSE_OPTION((x->n), (uninit), ((!!(in)) & oom_mask), (x->n));
    CHOOSE_OPTION((x->sign), (uninit), (1 & oom_mask), (x->sign));
    CHOOSE_OPTION((x->poisoned), (uninit), (0 & oom_mask), (x->poisoned));
    /* Post-operation Aggresive Clearance */ // clang-format off
    oom_mask = 0; __BUFFER_P = 0; uninit = 0; first_val = 0; x = 0; in = 0; return ret_stat; // clang-format on
}
dnml_status crint_new_i64(crint *x, int64_t in) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {;});
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; in = 0; return CRINT_NULL; }
    /* Actual Operation */
    uint64_t oom_mask = UINT64_MAX; dnml_status ret_stat = CRINT_SUCCESS; limb_t *__BUFFER_P = calloc(1, U64_BYTES);
    DNML_TEST_ASSERT(_lib_crt_neq((ptr_t)__BUFFER_P, (ptr_t)(NULL)), realloc_null, {;});
    CHOOSE_OPTION((ret_stat), (_lib_crt_eq((ptr_t)__BUFFER_P, (ptr_t)(NULL)) & _lib_crt_eq((ptr_t)x->limbs, (ptr_t)(NULL))), DNML_ALLOC_OOM, ret_stat);
    CHOOSE_OPTION((oom_mask), (_lib_crt_eq((ptr_t)__BUFFER_P, (ptr_t)(NULL)) & _lib_crt_eq((ptr_t)x->limbs, (ptr_t)(NULL))), 0, oom_mask);

    uint8_t uninit = _lib_crt_eq((ptr_t)x->limbs, (ptr_t)(NULL)); uint64_t first_val = x->limbs[0];
    int8_t new_sign; CHOOSE_OPTION((new_sign), (_lib_crt_isneg(in)), (-1), (1));
    x->limbs = (uninit) ? __BUFFER_P : x->limbs;
    CHOOSE_OPTION((x->limbs[0]), (uninit), (__CRT_MAG_I64__(in) & oom_mask), (first_val));
    CHOOSE_OPTION((x->cap), (uninit), (1 & oom_mask), (x->cap));
    CHOOSE_OPTION((x->n), (uninit), ((!!(in)) & oom_mask), (x->n));
    CHOOSE_OPTION((x->sign), (uninit), (new_sign & oom_mask), (x->sign));
    CHOOSE_OPTION((x->poisoned), (uninit), (0 & oom_mask), (x->poisoned));
    /* Post-operation Aggrestive Clearance */ // clang-format off
    oom_mask = 0; __BUFFER_P = 0; uninit = 0; x = 0; in = 0;
    first_val = 0; new_sign = 0; return ret_stat; // clang-format on
}
dnml_status crint_new_f128(crint *x, long double in) { return CRINT_SUCCESS; }




//* =============================================== ASSIGNMENTS ============================================== *//
dnml_status crint_set(crint x, crint *dst) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)dst, (ptr_t)(NULL))), input_null, { crint_free(&x); });
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); crint_free(dst); });
    DNML_TEST_ASSERT((__STORAGE_VAL__(dst)), ci_store_inval, { crint_free(&x); crint_free(dst); });
    if (_lib_crt_eq((ptr_t)dst, (ptr_t)(NULL))) { pbv_crint_clear(x); dst = 0; return CRINT_NULL; }
    if (!__STORAGE_VAL__(dst)) { pbv_crint_clear(x); return CRINT_ERR_SINVAL; }
    if ((!crint_validate(x))) { pbv_crint_clear(x); return CRINT_ERR_SINVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); crint_free(dst); });
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
    set_range = 0; end = 0; rec_sign = 0; pbv_crint_clear(x); dst = 0; return ret_stat; // clang-format on
}
dnml_status crint_set_safe(crint x, crint *dst) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)dst, (ptr_t)(NULL))), input_null, { crint_free(&x); });
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); crint_free(dst); });
    DNML_TEST_ASSERT((__STORAGE_VAL__(dst)), ci_store_inval, { crint_free(&x); crint_free(dst); });
    if (_lib_crt_eq((ptr_t)dst, (ptr_t)(NULL))) { pbv_crint_clear(x); dst = 0; return CRINT_NULL; }
    if (!__STORAGE_VAL__(dst)) { pbv_crint_clear(x); return CRINT_ERR_SINVAL; }
    if ((!crint_validate(x))) { pbv_crint_clear(x); return CRINT_ERR_SINVAL; }
    /* Actual Operations */ dnml_status ret_stat = CRINT_SUCCESS;
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); crint_free(dst); });
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
    end = 0; rec_sign = 0; pbv_crint_clear(x); dst = 0; return ret_stat; // clang-format on
}
/* --------- CryptInt --> Primitive Types --------- */
dnml_status crint_setu64(uint64_t* dst, crint x) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)(dst), (ptr_t)(NULL))), input_null, { crint_free(&x); });
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    if (_lib_crt_eq((ptr_t)dst, (ptr_t)(NULL))) { pbv_crint_clear(x); dst = 0; return CRINT_NULL; }
    if ((!crint_validate(x))) { pbv_crint_clear(x); dst = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned), (CRINT_POISON), (ret_stat));
    limb_t first_limb = x.limbs[0]; CHOOSE_OPTION((first_limb), (x.n), (first_limb), (0));
    CHOOSE_OPTION((*dst), (x.poisoned), (*dst), (first_limb)); // clang-format off
    first_limb = 0; pbv_crint_clear(x); dst = 0; return ret_stat; // clang-format on
}
dnml_status crint_seti64(int64_t* dst, crint x) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)(dst), (ptr_t)(NULL))), input_null, { crint_free(&x); });
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    if (_lib_crt_eq((ptr_t)dst, (ptr_t)(NULL))) { pbv_crint_clear(x); dst = 0; return CRINT_NULL; }
    if ((!crint_validate(x))) { pbv_crint_clear(x); dst = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS;
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
    first_limb = 0; abs_int64_min = 0; pbv_crint_clear(x); dst = 0; return ret_stat; // clang-format on
}
dnml_status crint_setf128(long double* dst, crint x) { return CRINT_SUCCESS; }
dnml_status crint_setu64_safe(uint64_t* dst, crint x) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)(dst), (ptr_t)(NULL))), input_null, { crint_free(&x); });
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    if (_lib_crt_eq((ptr_t)dst, (ptr_t)(NULL))) { pbv_crint_clear(x); dst = 0; return CRINT_NULL; }
    if ((!crint_validate(x))) { pbv_crint_clear(x); dst = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
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
    noop = 0; first_limb = 0; pbv_crint_clear(x); dst = 0; return ret_stat; // clang-format on
}
dnml_status crint_seti64_safe(int64_t* dst, crint x) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)(dst), (ptr_t)(NULL))), input_null, { crint_free(&x); });
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    if (_lib_crt_eq((ptr_t)dst, (ptr_t)(NULL))) { pbv_crint_clear(x); dst = 0; return CRINT_NULL; }
    if ((!crint_validate(x))) { pbv_crint_clear(x); dst = 0; return CRINT_ERR_INVAL; }
    /* Actual Operations */
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
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
    noop = 0; first_limb = 0; abs_int64_min = 0; pbv_crint_clear(x); dst = 0; return ret_stat; // clang-format on
}
dnml_status crint_setf128_safe(long double* dst, crint x) { return CRINT_SUCCESS; }
/* --------- Primitive Types --> CryptInt --------- */
dnml_status crint_getu64(crint *dst, uint64_t val) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)dst, (ptr_t)(NULL))), input_null, {;});
    DNML_TEST_ASSERT((__STORAGE_VAL__(dst)), ci_full_contract, { crint_free(dst); });
    if (_lib_crt_eq((ptr_t)dst, (ptr_t)(NULL))) { dst = 0; val = 0; return CRINT_NULL; }
    if (!__STORAGE_VAL__(dst)) { dst = 0; val = 0; return CRINT_ERR_SINVAL; }
    /* Actual operations */
    DNML_TEST_ASSERT((!dst->poisoned), crint_poisoned, { crint_free(dst); }); dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (dst->poisoned), (CRINT_POISON), (ret_stat));
    limb_t first_limb = val; size_t lcount = !!(val); int8_t new_sign = 1;
    CHOOSE_OPTION((dst->limbs[0]), (dst->poisoned), (dst->limbs[0]), (first_limb));
    CHOOSE_OPTION((dst->n), (dst->poisoned), (dst->n), (lcount));
    CHOOSE_OPTION((dst->sign), (dst->poisoned), (dst->sign), (new_sign));
    __libdnml_smemwipe_u64(dst->limbs, dst->cap, lcount, dst->cap - 1, (dst->poisoned));
    /* Aggressive Post-operation Cleanup */ // clang-format off
    first_limb = 0; lcount = 0; new_sign = 0; val = 0; dst = 0;  return ret_stat; // clang-format on
}
dnml_status crint_geti64(crint *dst, int64_t val) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)dst, (ptr_t)(NULL))), input_null, {;});
    DNML_TEST_ASSERT((__STORAGE_VAL__(dst)), ci_full_contract, { crint_free(dst); });
    if (_lib_crt_eq((ptr_t)dst, (ptr_t)(NULL))) { dst = 0; val = 0; return CRINT_NULL; }
    if (!__STORAGE_VAL__(dst)) { dst = 0; val = 0; return CRINT_ERR_SINVAL; }
    /* Actual operations */
    DNML_TEST_ASSERT((!dst->poisoned), crint_poisoned, { crint_free(dst); }); dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (dst->poisoned), (CRINT_POISON), (ret_stat));
    limb_t first_limb = __CRT_MAG_I64__(val); size_t lcount = !!(val); int8_t new_sign;
    CHOOSE_OPTION((new_sign), (_lib_crt_isneg(val)), (-1), (1));
    CHOOSE_OPTION((dst->limbs[0]), (dst->poisoned), (dst->limbs[0]), (first_limb));
    CHOOSE_OPTION((dst->n), (dst->poisoned), (dst->n), (lcount));
    CHOOSE_OPTION((dst->sign), (dst->poisoned), (dst->sign), (new_sign));
    __libdnml_smemwipe_u64(dst->limbs, dst->cap, lcount, dst->cap - 1, (dst->poisoned));
    /* Aggressive Post-operation Cleanup */ // clang-format off
    first_limb = 0; lcount = 0; new_sign = 0; val = 0; dst = 0; return ret_stat; // clang-format on
}
dnml_status crint_getf128(crint *dst, long double val) { return CRINT_SUCCESS; }
dnml_status crint_getf128_safe(crint *dst, long double val) { return CRINT_SUCCESS; }




//* =============================================== CONVERSIONS ============================================== *//
/* --------- CryptInt --> Primitive Types --------- */
uint64_t crint_tou64(crint x, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); }); dnml_status ret_stat = CRINT_SUCCESS;
    if ((!crint_validate(x))) { pbv_crint_clear(x); err = 0; return CRINT_ERR_INVAL; }
    CHOOSE_OPTION((ret_stat), (x.poisoned), (CRINT_POISON), (ret_stat));
    limb_t first_limb = x.limbs[0]; CHOOSE_OPTION((first_limb), (x.n), (first_limb), (0));
    CHOOSE_OPTION((first_limb), (x.poisoned), (0), (first_limb));
    if (_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))) *err = ret_stat; // clang-format off
    ret_stat = 0; pbv_crint_clear(x); err = 0; return first_limb; // clang-format on
}
int64_t crint_toi64(crint x, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    if ((!crint_validate(x))) { pbv_crint_clear(x); err = 0; return CRINT_ERR_INVAL; }
    dnml_status ret_stat = CRINT_SUCCESS;  CHOOSE_OPTION((ret_stat), (x.poisoned), (CRINT_POISON), (ret_stat));
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
    if (_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))) *err = ret_stat; // clang-format off
    ret_stat = 0; first_limb = 0; abs_int64_min = 0; mask = 0;
    pbv_crint_clear(x); err = 0; return ret; // clang-format on
}
long double crint_tof128(crint x, dnml_status *err) { return 0.0; }
uint64_t crint_tou64_safe(crint x, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    if ((!crint_validate(x))) { pbv_crint_clear(x); err = 0; return CRINT_ERR_INVAL; }
    dnml_status ret_stat = CRINT_SUCCESS; uint8_t noop = false;
    CHOOSE_OPTION((ret_stat), (x.poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((noop), (x.poisoned & !noop), (true), (noop));
    CHOOSE_OPTION((ret_stat), ((_lib_crt_gt(x.n, 1) | _lib_crt_eq(x.sign, -1)) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_ERR_RANGE), (ret_stat));
    CHOOSE_OPTION((noop), ((_lib_crt_gt(x.n, 1) | _lib_crt_eq(x.sign, -1)) & !noop), (true), (noop));

    limb_t first_limb = x.limbs[0]; CHOOSE_OPTION((first_limb), (x.n), (first_limb), (0));
    CHOOSE_OPTION((first_limb), (!(noop)), (first_limb), (0));
    if (_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))) *err = ret_stat; // clang-format off
    noop = 0; ret_stat = 0; pbv_crint_clear(x); err = 0; return first_limb; // clang-format on
}
int64_t crint_toi64_safe(crint x, dnml_status *err) {
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    if ((!crint_validate(x))) { pbv_crint_clear(x); err = 0; return CRINT_ERR_INVAL; }
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
    if (_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))) *err = ret_stat; // clang-format off
    ret_stat = 0; noop = 0; abs_int64_min = 0; pbv_crint_clear(x); err = 0; return first_limb; // clang-format on
}
long double crint_tof128_safe(crint x, dnml_status *err) { return 0.0; }
/* --------- Primitive Types --> CryptInt --------- */
crint crint_fromu64(uint64_t x, dnml_status *err) {
    crint ret; dnml_status new_stat; limb_t tmp[1] = {0}; new_stat = crint_new(&ret);
    ret.limbs = (_lib_crt_eq(new_stat, DNML_ALLOC_OOM)) ? tmp : ret.limbs;
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_neq(new_stat, DNML_ALLOC_OOM)));
    ret.limbs[0] = x; ret.n = !!(x); ret.sign = 1;
    ret.limbs[0] &= mask; ret.n &= mask; ret.sign &= mask;
    ret.limbs = (_lib_crt_eq(new_stat, DNML_ALLOC_OOM)) ? NULL : ret.limbs;
    if (_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))) {
        CHOOSE_OPTION((*err), (_lib_crt_eq(new_stat, DNML_ALLOC_OOM)), (DNML_ALLOC_OOM), (CRINT_SUCCESS));
    }
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    new_stat = 0; tmp[0] = 0; mask = 0; x = 0; err = 0; return ret; // clang-format on
}
crint crint_fromi64(int64_t x, dnml_status *err) {
    crint ret; dnml_status new_stat; limb_t tmp[1] = {0}; new_stat = crint_new(&ret);
    ret.limbs = (_lib_crt_eq(new_stat, DNML_ALLOC_OOM)) ? tmp : ret.limbs;
    uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_neq(new_stat, DNML_ALLOC_OOM)));
    ret.limbs[0] = __CRT_MAG_I64__(x); ret.n = !!(x);
    CHOOSE_OPTION((ret.sign), (_lib_crt_isneg(x)), (-1), (1));
    ret.limbs[0] &= mask; ret.n &= mask; ret.sign &= mask;
    ret.limbs = (_lib_crt_eq(new_stat, DNML_ALLOC_OOM)) ? NULL : ret.limbs;
    if (_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))) {
        CHOOSE_OPTION((*err), (_lib_crt_eq(new_stat, DNML_ALLOC_OOM)), (DNML_ALLOC_OOM), (CRINT_SUCCESS));
    }
    /* Aggressive Post-Operation Cleanup */ // clang-format off
    new_stat = 0; tmp[0] = 0; mask = 0; x = 0; err = 0; return ret; // clang-format on
}
crint crint_fromf128(long double x, dnml_status *err) { return (crint){0}; }
crint crint_fromf128_safe(long double x, dnml_status *err) { return (crint){0}; }




//* =============================================== COMPARISONS ============================================== *//
static int8_t __CRINT_MAGCMP64__(crint *x, uint64_t val) {
    /* Pre-operation Validation & Static Analysis */
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {;});
    DNML_TEST_ASSERT((crint_pvalidate(x)), ci_full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((x->poisoned), crint_poisoned, { crint_free(x); });
    /* Main Operation - Comparison */
    // We set ret to 2 as a safety mask to check if ret is previously set or not
    int8_t ret = 2, curr = 0;
    // Check 1: if (x->n > 1) return 1;
    CHOOSE_OPTION((curr), (_lib_crt_gt(x->n, 1)), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (x->limbs[0] > val) return 1;
    CHOOSE_OPTION((curr), (_lib_crt_gt(x->limbs[0], val)), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 3: if (x->limbs[0] < val) return -1;
    CHOOSE_OPTION((curr), (_lib_crt_lt(x->limbs[0], val)), (-1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // ALL CHECKS WEREN'T SATIFIES --> EQUAL
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (0), (ret)); // clang-format off
    curr = 0; val = 0; x = 0; return ret; // clang-format on
}
static int8_t __CRINT_MAGCMP__(crint *x, crint *y) {
    /* Pre-operation Validation & Static Analysis */
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {;});
    DNML_TEST_ASSERT((crint_pvalidate(x)), ci_full_contract, { crint_free(x); });
    DNML_TEST_ASSERT((x->poisoned), crint_poisoned, { crint_free(x); });
    /* Main Operation - Comparison */
    // We set ret to 2 as a safety mask to check if ret is previously set or not
    int8_t ret = 2, curr = 0;
    // Check 1: if (x->n > y->n) return 1;
    CHOOSE_OPTION((curr), (_lib_crt_gt(x->n, y->n)), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (x->n < y->n) return -1;
    CHOOSE_OPTION((curr), (_lib_crt_lt(x->n, y->n)), (-1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
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
       CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
       CHOOSE_OPTION((curr), (_lib_crt_lt(x_curr, y_curr)), (-1), (0));
       CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    }
    // ALL CHECKS WEREN'T SATIFIES --> EQUAL
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (0), (ret));
    /* Aggresive Memory Sanitization */ // clang-format off
    curr = 0; upperbound = 0; x_curr = 0; y_curr = 0; x = 0; y = 0; return ret; // clang-format on
}
/* ---------------- Integer - I64 ---------------- */
bool crint_equal_i64(crint x, int64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))), null_err, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    if ((!crint_validate(x))) { pbv_crint_clear(x); val = 0; err = 0; return CRINT_ERR_INVAL; }
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr; int8_t vsign;
    CHOOSE_OPTION((vsign), (_lib_crt_isneg(val)), (-1), (1));
    uint64_t mag_val = __CRT_MAG_I64__(val);
    /* if (!x.n) return (val) ? false : true;  */
    // Check 1: if (!x.n && !val) return true
    CHOOSE_OPTION((curr), ((!x.n) & !(val)), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (!x.n && val) return false
    CHOOSE_OPTION((curr), ((!x.n) & (val)), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    // Check 3: if (val_sign != x.sign) return false;
    CHOOSE_OPTION((curr), (_lib_crt_neq(vsign, x.sign)), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 4: if (x.n > 1) return false;
    CHOOSE_OPTION((curr), (_lib_crt_gt(x.n, 1)), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 5: if (x.limbs[0] != __CRT_MAG_I64__(val)) return false;
    CHOOSE_OPTION((curr), (_lib_crt_neq(x.limbs[0], mag_val)), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (1), (ret));
    /* Post-operation Aggrestive Cleanup */
    if (_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))) *err = ret_stat; // clang-format off
    ret_stat = 0; curr = 0; vsign = 0; mag_val = 0;
    pbv_crint_clear(x); val = 0; err = 0; return (bool)(ret); // clang-format on
}
bool crint_less_i64(crint x, int64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))), null_err, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    if ((!crint_validate(x))) { pbv_crint_clear(x); val = 0; err = 0; return CRINT_ERR_INVAL; }
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr; int8_t vsign;
    CHOOSE_OPTION((vsign), (_lib_crt_isneg(val)), (-1), (1));
    uint64_t mag_val = __CRT_MAG_I64__(val);
    /* if (!x.n) return (val > 0) ? 1 : 0 */
    // Check 1: if (!x.n && val > 0) return true
    CHOOSE_OPTION((curr), ((!x.n) & (_lib_crt_gti64(val, 0))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (!x.n && val < 0) return false
    CHOOSE_OPTION((curr), ((!x.n) & (_lib_crt_isneg(val))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (val_sign != x.sign) return (x.sign < val_sign); */
    // Check 3: if (val_sign != x.sign && x.sign > val_sign) return false;
    CHOOSE_OPTION((curr), ((_lib_crt_neq(vsign, x.sign)) & (_lib_crt_gti64(x.sign, vsign))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 4: if (val_sign != x.sign && x.sign < val_sign) return true;
    CHOOSE_OPTION((curr), ((_lib_crt_neq(vsign, x.sign)) & (_lib_crt_lti64(x.sign, vsign))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (x.n > 1) return (x.sign == -1)) */
    // Check 5: if (x.n > 1 && x.sign == 1) return false;
    CHOOSE_OPTION((curr), ((_lib_crt_gt(x.n, 1)) & (_lib_crt_eq(x.sign, 1))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 5: if (x.n > 1 && x.sign == -1) return true;
    CHOOSE_OPTION((curr), ((_lib_crt_gt(x.n, 1)) & (_lib_crt_eq(x.sign, -1))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (x.limbs[0] > __CRT_MAG_I64__(val)) return (x.sign == -1) */
    // Check 6: if (x.limbs[0] > mag_val && x.sign == 1) return false;
    CHOOSE_OPTION((curr), ((_lib_crt_gt(x.limbs[0], mag_val)) & (_lib_crt_eq(x.sign, 1))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 7: if (x.limbs[0] > mag_val && x.sign == -1) return true;
    CHOOSE_OPTION((curr), ((_lib_crt_gt(x.limbs[0], mag_val)) & (_lib_crt_eq(x.sign, -1))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* return (x.limbs[0] < mag_val) && (x.sign == 1) */
    // Check 8: if (x->limbs[0] < mag_val && x.sign == -1) return false;
    CHOOSE_OPTION((curr), ((_lib_crt_lt(x.limbs[0], mag_val)) & (_lib_crt_eq(x.sign, -1))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (1), (ret));
    /* Post-operation Aggrestive Cleanup */
    if (_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))) *err = ret_stat; // clang-format off
    ret_stat = 0; curr = 0; vsign = 0; mag_val = 0;
    pbv_crint_clear(x); val = 0; err = 0; return (bool)(ret); // clang-format on
}
bool crint_more_i64(crint x, int64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))), null_err, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    if ((!crint_validate(x))) { pbv_crint_clear(x); val = 0; err = 0; return CRINT_ERR_INVAL; }
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr; int8_t vsign;
    CHOOSE_OPTION((vsign), (_lib_crt_isneg(val)), (-1), (1));
    uint64_t mag_val = __CRT_MAG_I64__(val);
    /* if (!x.n) return (val) ? 1 : 0 */
    // Check 1: if (!x.n && val < 0) return true
    CHOOSE_OPTION((curr), ((!x.n) & (_lib_crt_isneg(val))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (!x.n && val) return false
    CHOOSE_OPTION((curr), ((!x.n) & (val)), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (val_sign != x.sign) return (x.sign < val_sign); */
    // Check 3: if (val_sign != x.sign && x.sign < val_sign) return false;
    CHOOSE_OPTION((curr), ((_lib_crt_neq(vsign, x.sign)) & (_lib_crt_lti64(x.sign, vsign))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 4: if (val_sign != x.sign && x.sign > val_sign) return true;
    CHOOSE_OPTION((curr), ((_lib_crt_neq(vsign, x.sign)) & (_lib_crt_gti64(x.sign, vsign))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (x.n > 1) return (x.sign == -1) */
    // Check 5: if (x.n > 1 && x.sign == 1) return false;
    CHOOSE_OPTION((curr), ((_lib_crt_gt(x.n, 1)) & (_lib_crt_eq(x.sign, 1))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 5: if (x.n > 1 && x.sign == -1) return true;
    CHOOSE_OPTION((curr), ((_lib_crt_gt(x.n, 1)) & (_lib_crt_eq(x.sign, -1))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (x.limbs[0] < __CRT_MAG_I64__(val)) return (x.sign == -1)) */
    // Check 6: if (x.limbs[0] < mag_val && x.sign == 1) return false;
    CHOOSE_OPTION((curr), ((_lib_crt_lt(x.limbs[0], mag_val)) & (_lib_crt_eq(x.sign, 1))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 7: if (x.limbs[0] < mag_val && x.sign == -1) return true;
    CHOOSE_OPTION((curr), ((_lib_crt_lt(x.limbs[0], mag_val)) & (_lib_crt_eq(x.sign, -1))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* return (x.limbs[0] > __CRT_MAG_I64__(val)) && x.sign == 1) */
    // Check 8: if (x->limbs[0] > mag_val && x.sign == -1) return false;
    CHOOSE_OPTION((curr), ((_lib_crt_gt(x.limbs[0], mag_val)) & (_lib_crt_eq(x.sign, -1))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (1), (ret));
    /* Post-operation Aggrestive Cleanup */
    if (_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))) *err = ret_stat; // clang-format off
    ret_stat = 0; curr = 0; vsign = 0; mag_val = 0;
    pbv_crint_clear(x); val = 0; err = 0; return (bool)(ret); // clang-format on
}
bool crint_lequal_i64(crint x, int64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))), null_err, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    if ((!crint_validate(x))) { pbv_crint_clear(x); val = 0; err = 0; return CRINT_ERR_INVAL; }
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr; int8_t vsign;
    CHOOSE_OPTION((vsign), (_lib_crt_isneg(val)), (-1), (1));
    uint64_t mag_val = __CRT_MAG_I64__(val);
    /* if (!x.n) return (val >= 0) ? 1 : 0 */
    // Check 1: if (!x.n && val >= 0) return true
    CHOOSE_OPTION((curr), ((!x.n) & (_lib_crt_ispos(val))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (!x.n && val < 0) return false
    CHOOSE_OPTION((curr), ((!x.n) & (_lib_crt_isneg(val))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (val_sign != x.sign) return (x.sign < val_sign); */
    // Check 3: if (val_sign != x.sign && x.sign > val_sign) return false;
    CHOOSE_OPTION((curr), ((_lib_crt_neq(vsign, x.sign)) & (_lib_crt_gti64(x.sign, vsign))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 4: if (val_sign != x.sign && x.sign < val_sign) return true;
    CHOOSE_OPTION((curr), ((_lib_crt_neq(vsign, x.sign)) & (_lib_crt_lti64(x.sign, vsign))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (if (x.n > 1) return (x.sign == -1); */
    // Check 5: if (x.n > 1 && x.sign == 1) return false;
    CHOOSE_OPTION((curr), ((_lib_crt_gt(x.n, 1)) & (_lib_crt_eq(x.sign, 1))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 5: if (x.n > 1 && x.sign == -1) return true;
    CHOOSE_OPTION((curr), ((_lib_crt_gt(x.n, 1)) & (_lib_crt_eq(x.sign, -1))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (x.limbs[0] > __CRT_MAG_I64__(val)) return (x.sign == 1); */
    // Check 6: if (x.limbs[0] > mag_val && x.sign == 1) return false;
    CHOOSE_OPTION((curr), ((_lib_crt_gt(x.limbs[0], mag_val)) & (_lib_crt_eq(x.sign, 1))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 7: if (x.limbs[0] > mag_val && x.sign == -1) return true;
    CHOOSE_OPTION((curr), ((_lib_crt_gt(x.limbs[0], mag_val)) & (_lib_crt_eq(x.sign, -1))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* return (x.sign == 1) */
    // Check 8: if (x.sign == -1) return false;
    CHOOSE_OPTION((curr), (_lib_crt_eq(x.sign, -1)), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (1), (ret));
    /* Post-operation Aggrestive Cleanup */
    if (_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))) *err = ret_stat; // clang-format off
    ret_stat = 0; curr = 0; vsign = 0; mag_val = 0;
    pbv_crint_clear(x); val = 0; err = 0; return (bool)(ret); // clang-format on
}
bool crint_mequal_i64(crint x, int64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))), null_err, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    if ((!crint_validate(x))) { pbv_crint_clear(x); val = 0; err = 0; return CRINT_ERR_INVAL; }
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr; int8_t vsign;
    CHOOSE_OPTION((vsign), (_lib_crt_isneg(val)), (-1), (1));
    uint64_t mag_val = __CRT_MAG_I64__(val);
    /* if (!x.n) return (val <= 0) ? 1 : 0 */
    // Check 1: if (!x.n && val <= 0) return true
    CHOOSE_OPTION((curr), ((!x.n) & (_lib_crt_leqi64(val, 0))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (!x.n && val > 0) return false
    CHOOSE_OPTION((curr), ((!x.n) & (_lib_crt_geqi64(val, 0))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (val_sign != x.sign) return (x.sign > val_sign); */
    // Check 3: if (val_sign != x.sign && x.sign < val_sign) return false;
    CHOOSE_OPTION((curr), ((_lib_crt_neq(vsign, x.sign)) & (_lib_crt_lti64(x.sign, vsign))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 4: if (val_sign != x.sign && x.sign > val_sign) return true;
    CHOOSE_OPTION((curr), ((_lib_crt_neq(vsign, x.sign)) & (_lib_crt_gti64(x.sign, vsign))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (x.n > 1) return (x.sign == 1) */
    // Check 5: if (x.n > 1 && x.sign == -1) return false;
    CHOOSE_OPTION((curr), ((_lib_crt_gt(x.n, 1)) & (_lib_crt_eq(x.sign, -1))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 5: if (x.n > 1 && x.sign == 1)) return true;
    CHOOSE_OPTION((curr), ((_lib_crt_gt(x.n, 1)) & (_lib_crt_eq(x.sign, 1))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (x.limbs[0] > __CRT_MAG_I64__(val)) return x.sign == 1) */
    // Check 6: if (x.limbs[0] > mag_val && x.sign == 1) return false;
    CHOOSE_OPTION((curr), ((_lib_crt_gt(x.limbs[0], mag_val)) & (_lib_crt_eq(x.sign, -1))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 7: if (x.limbs[0] > mag_val && x.sign == -1) return true;
    CHOOSE_OPTION((curr), ((_lib_crt_gt(x.limbs[0], mag_val)) & (_lib_crt_eq(x.sign, 1))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* return (x.sign == -1) */
    // Check 8: if (x.sign == 1) return false;
    CHOOSE_OPTION((curr), (_lib_crt_eq(x.sign, 1)), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (1), (ret));
    /* Post-operation Aggrestive Cleanup */
    if (_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))) *err = ret_stat; // clang-format off
    ret_stat = 0; curr = 0; vsign = 0; mag_val = 0;
    pbv_crint_clear(x); val = 0; err = 0; return (bool)(ret); // clang-format on
}
/* ----------- Unsigned Integer - UI64 ----------- */
bool crint_equal_u64(crint x, uint64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))), null_err, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    if ((!crint_validate(x))) { pbv_crint_clear(x); val = 0; err = 0; return CRINT_ERR_INVAL; }
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr;
    /* if (!x.n) return (val) ? false : true;  */
    // Check 1: if (!x.n && !val) return true
    CHOOSE_OPTION((curr), ((!x.n) & !(val)), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (!x.n && val) return false
    CHOOSE_OPTION((curr), ((!x.n) & (val)), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    // Check 3: if (x.sign == -1) return false;
    CHOOSE_OPTION((curr), (_lib_crt_eq(x.sign, -1)), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 4: if (x.n > 1) return false;
    CHOOSE_OPTION((curr), (_lib_crt_gt(x.n, 1)), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 5: if (x.limbs[0] != val) return false;
    CHOOSE_OPTION((curr), (_lib_crt_neq(x.limbs[0], val)), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (1), (ret));
    /* Post-operation Aggresive Cleanup */
    if (_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))) *err = ret_stat; // clang-format off
    ret_stat = 0; curr = 0; pbv_crint_clear(x); val = 0; err = 0; return (bool)(ret); // clang-format on
}
bool crint_less_u64(crint x, uint64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))), null_err, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    if ((!crint_validate(x))) { pbv_crint_clear(x); val = 0; err = 0; return CRINT_ERR_INVAL; }
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr;
    /* if (!x.n) return (val) ? true : false;  */
    // Check 1: if (!x.n && val) return true
    CHOOSE_OPTION((curr), ((!x.n) & (val)), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (!x.n && !val) return false
    CHOOSE_OPTION((curr), ((!x.n) & !(val)), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    // Check 3: if (x.sign == -1) return true;
    CHOOSE_OPTION((curr), (_lib_crt_eq(x.sign, -1)), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 4: if (x.n == 1) return false;
    CHOOSE_OPTION((curr), (_lib_crt_gt(x.n, 1)), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 5: if (x.limbs[0] >= val) return false;
    CHOOSE_OPTION((curr), (_lib_crt_geq(x.limbs[0], val)), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (1), (ret));
    /* Post-operation Aggresive Cleanup */
    if (_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))) *err = ret_stat; // clang-format off
    ret_stat = 0; curr = 0; pbv_crint_clear(x); val = 0; err = 0; return (bool)(ret); // clang-format on
}
bool crint_more_u64(crint x, uint64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))), null_err, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    if ((!crint_validate(x))) { pbv_crint_clear(x); val = 0; err = 0; return CRINT_ERR_INVAL; }
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr;
    /* if (!x.n) return (val) ? false : true;  */
    // Check 1: if (!x.n && !val) return true
    CHOOSE_OPTION((curr), ((!x.n) & !(val)), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (!x.n && val) return false
    CHOOSE_OPTION((curr), ((!x.n) & (val)), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    // Check 3: if (x.sign == -1) return false;
    CHOOSE_OPTION((curr), (_lib_crt_eq(x.sign, -1)), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 4: if (x.n == 1) return true;
    CHOOSE_OPTION((curr), (_lib_crt_gt(x.n, 1)), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 5: if (x.limbs[0] <= val) return false;
    CHOOSE_OPTION((curr), (_lib_crt_leq(x.limbs[0], val)), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (1), (ret));
    /* Post-operation Aggresive Cleanup */
    if (_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))) *err = ret_stat; // clang-format off
    ret_stat = 0; curr = 0; pbv_crint_clear(x); val = 0; err = 0; return (bool)(ret); // clang-format on
}
bool crint_lequal_u64(crint x, uint64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))), null_err, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    if ((!crint_validate(x))) { pbv_crint_clear(x); val = 0; err = 0; return CRINT_ERR_INVAL; }
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr;
    // Check 1: if (!x.n) return true
    CHOOSE_OPTION((curr), ((!x.n)), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (x.sign == -1) return true;
    CHOOSE_OPTION((curr), (_lib_crt_eq(x.sign, -1)), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 4: if (x.n == 1) return false;
    CHOOSE_OPTION((curr), (_lib_crt_gt(x.n, 1)), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 5: if (x.limbs[0] > val) return false;
    CHOOSE_OPTION((curr), (_lib_crt_gt(x.limbs[0], val)), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (1), (ret));
    /* Post-operation Aggresive Cleanup */
    if (_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))) *err = ret_stat; // clang-format off
    ret_stat = 0; curr = 0; pbv_crint_clear(x); val = 0; err = 0; return (bool)(ret); // clang-format on
}
bool crint_mequal_u64(crint x, uint64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_TEST_ASSERT((crint_validate(x)), ci_full_contract, { crint_free(&x); });
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))), null_err, { crint_free(&x); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); });
    if ((!crint_validate(x))) { pbv_crint_clear(x); val = 0; err = 0; return CRINT_ERR_INVAL; }
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr;
    /* if (!x.n) return (val) ? false : true;  */
    // Check 1: if (!x.n && !val) return true
    CHOOSE_OPTION((curr), ((!x.n) & !(val)), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (!x.n && val) return false
    CHOOSE_OPTION((curr), ((!x.n) & (val)), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    // Check 3: if (x.sign == -1) return false;
    CHOOSE_OPTION((curr), (_lib_crt_eq(x.sign, -1)), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 4: if (x.n == 1) return true;
    CHOOSE_OPTION((curr), (_lib_crt_gt(x.n, 1)), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 5: if (x.limbs[0] < val) return false;
    CHOOSE_OPTION((curr), (_lib_crt_lt(x.limbs[0], val)), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (1), (ret));
    /* Post-operation Aggresive Cleanup */
    if (_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))) *err = ret_stat; // clang-format off
    ret_stat = 0; curr = 0; pbv_crint_clear(x); val = 0; err = 0; return (bool)(ret); // clang-format on
}
/* ------------------- Cryptint ------------------ */
bool crint_equal(crint x, crint y, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_TEST_ASSERT(((crint_validate(x)) && (crint_validate(y))), ci_full_contract, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))), null_err, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); crint_free(&y); });
    if ((!crint_validate(x)) | (!crint_validate(y))) { pbv_crint_clear(x); pbv_crint_clear(y); err = 0; return CRINT_ERR_INVAL; }
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr;
    /* if (!x.n) return (val) ? false : true;  */
    // Check 1: if (!x.n && !y.n) return true
    CHOOSE_OPTION((curr), ((!x.n) & !(y.n)), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (!x.n && y.n) return false
    CHOOSE_OPTION((curr), ((!x.n) & (y.n)), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (a.sign != b.sign) return false; */
    CHOOSE_OPTION((curr), (_lib_crt_neq(x.sign, y.sign)), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    /* if (a.n != b.n) return false; */
    CHOOSE_OPTION((curr), (_lib_crt_neq(x.n, y.n)), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    /* if (a.limbs == b.limbs) return true; */
    CHOOSE_OPTION((curr), (_lib_crt_eq((ptr_t)(x.limbs), (ptr_t)(y.limbs))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* Main Validation Loop */
    int8_t mag_ret = __CRINT_MAGCMP__(&x, &y);
    CHOOSE_OPTION((curr), (!(mag_ret)), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (0), (ret));
    /* Post-operation Aggresive Cleanup */
    if (_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))) *err = ret_stat; // clang-format off
    ret_stat = 0; curr = 0; mag_ret = 0; pbv_crint_clear(x);
    pbv_crint_clear(y); err = 0; return (bool)(ret); // clang-format on
}
bool crint_less(crint x, crint y, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_TEST_ASSERT(((crint_validate(x)) && (crint_validate(y))), ci_full_contract, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))), null_err, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); crint_free(&y); });
    if ((!crint_validate(x)) | (!crint_validate(y))) { pbv_crint_clear(x); pbv_crint_clear(y); err = 0; return CRINT_ERR_INVAL; }
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr;
    /* if (_lib_crt_neq(x.sign, y.sign)) return (x.sign < y.sign) */
    // Check 1: if (_lib_crt_neq(x.sign, y.sign) & x.sign > y.sign) return false
    CHOOSE_OPTION((curr), ((_lib_crt_neq(x.sign, y.sign)) & (_lib_crt_gti64(x.sign, y.sign))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (_lib_crt_neq(x.sign, y.sign) & x.sign < y.sign) return true
    CHOOSE_OPTION((curr), ((_lib_crt_neq(x.sign, y.sign)) & (_lib_crt_lti64(x.sign, y.sign))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (_lib_crt_neq(x.n, y.n)) return (x.sign == 1) ? (x.n < y.n) : (x.n > y.n); */
    // Check 3: if (_lib_crt_neq(x.n, y.n) && x.sign == 1 && x.n > y.n) return false
    CHOOSE_OPTION((curr), ((_lib_crt_neq(x.n, y.n)) & (_lib_crt_eq(x.sign, 1)) & (_lib_crt_gt(x.n, y.n))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 4: if (_lib_crt_neq(x.n, y.n) && x.sign == 1 && x.n < y.n) return true
    CHOOSE_OPTION((curr), ((_lib_crt_neq(x.n, y.n)) & (_lib_crt_eq(x.sign, 1)) & (_lib_crt_lt(x.n, y.n))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 5: if (_lib_crt_neq(x.n, y.n) && x.sign == -1 && x.n < y.n) return false
    CHOOSE_OPTION((curr), ((_lib_crt_neq(x.n, y.n)) & (_lib_crt_eq(x.sign, -1)) & (_lib_crt_lt(x.n, y.n))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 6: if (_lib_crt_neq(x.n, y.n) && x.sign == -1 && x.n > y.n) return true
    CHOOSE_OPTION((curr), ((_lib_crt_neq(x.n, y.n)) & (_lib_crt_eq(x.sign, -1)) & (_lib_crt_gt(x.n, y.n))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (a.limbs == b.limbs) return false; */
    CHOOSE_OPTION((curr), (_lib_crt_eq((ptr_t)(x.limbs), (ptr_t)(y.limbs))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* Main Validation Loop */
    /* return (a.sign == 1) ? __CRINT_MAGCMP__(&x, &y) < 0 : __CRINT_MAGCMP__(&x, &y) > 0; */
    int8_t mag_ret = __CRINT_MAGCMP__(&x, &y);
    // Check 7+8: return (a.sign == 1 && mag_ret < 0);
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, 1)) & (_lib_crt_ispos(mag_ret))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, 1)) & _lib_crt_isneg(mag_ret)), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 9+10: return (a.sign == -1 && mag_ret > 0);
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, -1)) & (_lib_crt_neq(mag_ret, 0))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, -1)) & (mag_ret)), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 11: mag_ret == 0
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (0), (ret));
    /* Post-operation Aggresive Cleanup */
    if (_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))) *err = ret_stat; 
    return (bool)(ret); // clang-format off
}
bool crint_more(crint x, crint y, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_TEST_ASSERT(((crint_validate(x)) && (crint_validate(y))), ci_full_contract, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))), null_err, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); crint_free(&y); });
    if ((!crint_validate(x)) | (!crint_validate(y))) { pbv_crint_clear(x); pbv_crint_clear(y); err = 0; return CRINT_ERR_INVAL; }
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr;
    /* if (_lib_crt_neq(x.sign, y.sign)) return (_lib_crt_gti64(x.sign, y.sign)) */
    // Check 1: if (_lib_crt_neq(x.sign, y.sign) & x.sign < y.sign) return false
    CHOOSE_OPTION((curr), ((_lib_crt_neq(x.sign, y.sign)) & (_lib_crt_lti64(x.sign, y.sign))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (_lib_crt_neq(x.sign, y.sign) & x.sign > y.sign) return true
    CHOOSE_OPTION((curr), ((_lib_crt_neq(x.sign, y.sign)) & (_lib_crt_gti64(x.sign, y.sign))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (_lib_crt_neq(x.n, y.n)) return (x.sign == 1) ? (x.n > y.n) : (x.n < y.n); */
    // Check 3: if (_lib_crt_neq(x.n, y.n) && x.sign == 1 && x.n < y.n) return false
    CHOOSE_OPTION((curr), ((_lib_crt_neq(x.n, y.n)) & (_lib_crt_eq(x.sign, 1)) & (_lib_crt_lt(x.n, y.n))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 4: if (_lib_crt_neq(x.n, y.n) && x.sign == 1 && x.n > y.n) return true
    CHOOSE_OPTION((curr), ((_lib_crt_neq(x.n, y.n)) & (_lib_crt_eq(x.sign, 1)) & (_lib_crt_gt(x.n, y.n))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 5: if (_lib_crt_neq(x.n, y.n) && x.sign == -1 && x.n > y.n) return false
    CHOOSE_OPTION((curr), ((_lib_crt_neq(x.n, y.n)) & (_lib_crt_eq(x.sign, -1)) & (_lib_crt_gt(x.n, y.n))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 6: if (_lib_crt_neq(x.n, y.n) && x.sign == -1 && x.n < y.n) return true
    CHOOSE_OPTION((curr), ((_lib_crt_neq(x.n, y.n)) & (_lib_crt_eq(x.sign, -1)) & (_lib_crt_lt(x.n, y.n))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (a.limbs == b.limbs) return false; */
    CHOOSE_OPTION((curr), (_lib_crt_eq((ptr_t)(x.limbs), (ptr_t)(y.limbs))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* Main Validation Loop */
    /* return (a.sign == 1) ? __CRINT_MAGCMP__(&x, &y) > 0 : __CRINT_MAGCMP__(&x, &y) < 0; */
    int8_t mag_ret = __CRINT_MAGCMP__(&x, &y);
    // Check 7+8: return (a.sign == 1 && mag_ret > 0);
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, 1)) & (_lib_crt_neq(mag_ret, 0))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, 1)) & (mag_ret)), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 9+10: return (a.sign == -1 && mag_ret < 0);
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, -1)) & _lib_crt_geqi64(mag_ret, 0)), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, -1)) & _lib_crt_isneg(mag_ret)), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 11: mag_ret == 0
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (0), (ret));
    /* Post-operation Aggresive Cleanup */ // clang-format off
    if (_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))) *err = ret_stat;
    ret_stat = 0; curr = 0; mag_ret = 0; pbv_crint_clear(x); return (bool)(ret); // clang-format on
}
bool crint_lequal(crint x, crint y, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_TEST_ASSERT(((crint_validate(x)) && (crint_validate(y))), ci_full_contract, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))), null_err, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); crint_free(&y); });
    if ((!crint_validate(x)) | (!crint_validate(y))) { pbv_crint_clear(x); pbv_crint_clear(y); err = 0; return CRINT_ERR_INVAL; }
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr;
    /* if (_lib_crt_neq(x.sign, y.sign)) return (x.sign < y.sign) */
    // Check 1: if (_lib_crt_neq(x.sign, y.sign) & x.sign > y.sign) return false
    CHOOSE_OPTION((curr), ((_lib_crt_neq(x.sign, y.sign)) & (_lib_crt_gti64(x.sign, y.sign))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (_lib_crt_neq(x.sign, y.sign) & x.sign < y.sign) return true
    CHOOSE_OPTION((curr), ((_lib_crt_neq(x.sign, y.sign)) & (_lib_crt_lti64(x.sign, y.sign))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (_lib_crt_neq(x.n, y.n)) return (x.sign == 1) ? (x.n < y.n) : (x.n > y.n); */
    // Check 3: if (_lib_crt_neq(x.n, y.n) && x.sign == 1 && x.n > y.n) return false
    CHOOSE_OPTION((curr), ((_lib_crt_neq(x.n, y.n)) & (_lib_crt_eq(x.sign, 1)) & (_lib_crt_gt(x.n, y.n))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 4: if (_lib_crt_neq(x.n, y.n) && x.sign == 1 && x.n < y.n) return true
    CHOOSE_OPTION((curr), ((_lib_crt_neq(x.n, y.n)) & (_lib_crt_eq(x.sign, 1)) & (_lib_crt_lt(x.n, y.n))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 5: if (_lib_crt_neq(x.n, y.n) && x.sign == -1 && x.n < y.n) return false
    CHOOSE_OPTION((curr), ((_lib_crt_neq(x.n, y.n)) & (_lib_crt_eq(x.sign, -1)) & (_lib_crt_lt(x.n, y.n))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 6: if (_lib_crt_neq(x.n, y.n) && x.sign == -1 && x.n > y.n) return true
    CHOOSE_OPTION((curr), ((_lib_crt_neq(x.n, y.n)) & (_lib_crt_eq(x.sign, -1)) & (_lib_crt_gt(x.n, y.n))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (a.limbs == b.limbs) return true; */
    CHOOSE_OPTION((curr), (_lib_crt_eq((ptr_t)(x.limbs), (ptr_t)(y.limbs))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* Main Validation Loop */
    /* return (a.sign == 1) ? __CRINT_MAGCMP__(&x, &y) < 0 : __CRINT_MAGCMP__(&x, &y) > 0; */
    int8_t mag_ret = __CRINT_MAGCMP__(&x, &y);
    // Check 7+8: return (a.sign == 1 && _lib_crt_neq(mag_ret, 0));
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, 1)) & (mag_ret)), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, 1)) & (_lib_crt_neq(mag_ret, 0))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 9+10: return (a.sign == -1 && mag_ret >= 0);
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, -1)) & _lib_crt_isneg(mag_ret)), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, -1)) & (_lib_crt_ispos(mag_ret))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    /* Post-operation Aggresive Cleanup */
    if (_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))) *err = ret_stat;
    pbv_crint_clear(y); err = 0; return (bool)(ret); // clang-format on
}
bool crint_mequal(crint x, crint y, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_TEST_ASSERT(((crint_validate(x)) && (crint_validate(y))), ci_full_contract, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))), null_err, { crint_free(&x); crint_free(&y); });
    DNML_TEST_ASSERT((!x.poisoned), crint_poisoned, { crint_free(&x); crint_free(&y); });
    if ((!crint_validate(x)) | (!crint_validate(y))) { pbv_crint_clear(x); pbv_crint_clear(y); err = 0; return CRINT_ERR_INVAL; }
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr;
    /* if (_lib_crt_neq(x.sign, y.sign)) return (_lib_crt_gti64(x.sign, y.sign)) */
    // Check 1: if (_lib_crt_neq(x.sign, y.sign) & x.sign < y.sign) return false
    CHOOSE_OPTION((curr), ((_lib_crt_neq(x.sign, y.sign)) & (_lib_crt_lti64(x.sign, y.sign))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (_lib_crt_neq(x.sign, y.sign) & x.sign > y.sign) return true
    CHOOSE_OPTION((curr), ((_lib_crt_neq(x.sign, y.sign)) & (_lib_crt_gti64(x.sign, y.sign))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (_lib_crt_neq(x.n, y.n)) return (x.sign == 1) ? (x.n > y.n) : (x.n < y.n); */
    // Check 3: if (_lib_crt_neq(x.n, y.n) && x.sign == 1 && x.n < y.n) return false
    CHOOSE_OPTION((curr), ((_lib_crt_neq(x.n, y.n)) & (_lib_crt_eq(x.sign, 1)) & (_lib_crt_lt(x.n, y.n))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 4: if (_lib_crt_neq(x.n, y.n) && x.sign == 1 && x.n > y.n) return true
    CHOOSE_OPTION((curr), ((_lib_crt_neq(x.n, y.n)) & (_lib_crt_eq(x.sign, 1)) & (_lib_crt_gt(x.n, y.n))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 5: if (_lib_crt_neq(x.n, y.n) && x.sign == -1 && x.n > y.n) return false
    CHOOSE_OPTION((curr), ((_lib_crt_neq(x.n, y.n)) & (_lib_crt_eq(x.sign, -1)) & (_lib_crt_gt(x.n, y.n))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 6: if (_lib_crt_neq(x.n, y.n) && x.sign == -1 && x.n < y.n) return true
    CHOOSE_OPTION((curr), ((_lib_crt_neq(x.n, y.n)) & (_lib_crt_eq(x.sign, -1)) & (_lib_crt_lt(x.n, y.n))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* if (a.limbs == b.limbs) return true; */
    CHOOSE_OPTION((curr), (_lib_crt_eq((ptr_t)(x.limbs), (ptr_t)(y.limbs))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));

    /* Main Validation Loop */
    /* return (a.sign == 1) ? __CRINT_MAGCMP__(&x, &y) >= 0 : __CRINT_MAGCMP__(&x, &y) <= 0; */
    int8_t mag_ret = __CRINT_MAGCMP__(&x, &y);
    // Check 7+8: return (a.sign == 1 && mag_ret >= 0);
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, 1)) & _lib_crt_isneg(mag_ret)), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, 1)) & (_lib_crt_ispos(mag_ret))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 9+10: return (a.sign == -1 && mag_ret <= 0);
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, -1)) & (_lib_crt_gti64(mag_ret, 0))), (0), (1));
    CHOOSE_OPTION((ret), ((!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    CHOOSE_OPTION((curr), ((_lib_crt_eq(x.sign, -1)) & (_lib_crt_leq(mag_ret, 0))), (1), (0));
    CHOOSE_OPTION((ret), (!(!curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    /* Post-operation Aggresive Cleanup */
    if (_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))) *err = ret_stat; // clang-format off
    ret_stat = 0; curr = 0; mag_ret = 0; pbv_crint_clear(x);
    pbv_crint_clear(y); err = 0; return (bool)(ret); // clang-format on
}




//* ================================================= COPIES ================================================= *//
/* -------------  Mutative SMALL Copies ------------- */
dnml_status crint_mut_copyu64(crint *dst, uint64_t src) {
    // Pre-operation Validation & Static Analysis
    DNML_TEST_ASSERT(__STORAGE_VAL__(dst), ci_store_inval, { crint_free(dst); });
    DNML_TEST_ASSERT((!dst->poisoned), crint_poisoned, { crint_free(dst); });
    if (!__STORAGE_VAL__(dst)) { dst = 0; src = 0; return CRINT_ERR_SINVAL; }
    /* Actual Operation */
    dnml_status ret_stat = CRINT_SUCCESS; uint8_t noop_toggle = false;
    CHOOSE_OPTION((ret_stat), (dst->poisoned), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((noop_toggle), (dst->poisoned), (true), (noop_toggle));
    /* Main Operation - Copy */
    crint_canonicalize(dst); uint64_t val = dst->limbs[0];
    CHOOSE_OPTION((dst->limbs[0]), (noop_toggle), (val), (0));
    CHOOSE_OPTION((dst->n), (noop_toggle), (dst->n), (!!(src)));
    CHOOSE_OPTION((dst->sign), (noop_toggle), (dst->sign), (1));
    __libdnml_smemwipe_u64(dst->limbs, dst->cap, 1, dst->cap - 1, noop_toggle); // clang-format off
    val = 0; dst = 0; src = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_dcopyu64(crint *dst, uint64_t src) {
    // Pre-operation Validation & Static Analysis
    DNML_TEST_ASSERT(__STORAGE_VAL__(dst), ci_store_inval, { crint_free(dst); });
    DNML_TEST_ASSERT((!dst->poisoned), crint_poisoned, { crint_free(dst); });
    if (!__STORAGE_VAL__(dst)) { dst = 0; src = 0; return CRINT_ERR_SINVAL; }
    /* Actual Operation */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (dst->poisoned), (CRINT_POISON), (ret_stat));

    /* Main Operation - Copy */
    crint_canonicalize(dst); limb_t TMP_P = 1;
    limb_t *__BUFFER_P = realloc(dst->limbs, U64_BYTES);
    DNML_TEST_ASSERT(_lib_crt_neq((ptr_t)__BUFFER_P, (ptr_t)(NULL)), realloc_null, {;});
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq((ptr_t)__BUFFER_P, (ptr_t)(NULL)) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (CRINT_SUCCESS)
    );
    uint64_t oom; CHOOSE_OPTION((oom), (_lib_crt_eq((ptr_t)__BUFFER_P, (ptr_t)(NULL))), (0), (UINT64_MAX));
    dst->limbs = (_lib_crt_gt((ptr_t)__BUFFER_P, (ptr_t)NULL)) ? &TMP_P : __BUFFER_P;
    dst->limbs[0] = src; dst->n = !!(src); dst->cap = 1; dst->sign = 1;
    /* Invalid Metadata Fill & Aggresive Cleanup */ // clang-format off
    dst->limbs[0] &= oom; dst->n &= oom; dst->cap &= oom; dst->sign &= oom;
    dst->poisoned &= oom; TMP_P = 0; __BUFFER_P = 0; oom = 0; dst = 0; src = 0;
    return ret_stat; // clang-format on
}
dnml_status crint_mut_copyi64(crint *dst, int64_t src) {
    // Pre-operation Validation & Static Analysis
    DNML_TEST_ASSERT(__STORAGE_VAL__(dst), ci_store_inval, { crint_free(dst); });
    DNML_TEST_ASSERT((!dst->poisoned), crint_poisoned, { crint_free(dst); });
    if (!__STORAGE_VAL__(dst)) { dst = 0; src = 0; return CRINT_ERR_SINVAL; }
    /* Actual Operation */
    dnml_status ret_stat = CRINT_SUCCESS; uint8_t noop_toggle = false;
    CHOOSE_OPTION((ret_stat), (dst->poisoned), (CRINT_POISON), (ret_stat))
    CHOOSE_OPTION((noop_toggle), (dst->poisoned), (true), (noop_toggle));
    /* Main Operation - Copy */
    crint_canonicalize(dst); int8_t vsign;
    CHOOSE_OPTION((vsign), (_lib_crt_isneg(src)), (-1), (1));
    uint64_t val = __CRT_MAG_I64__(dst->limbs[0]);
    CHOOSE_OPTION((dst->limbs[0]), (noop_toggle), (val), (0));
    CHOOSE_OPTION((dst->n), (noop_toggle), (dst->n), (!!(src)));
    CHOOSE_OPTION((dst->sign), (noop_toggle), (dst->sign), (vsign));
    __libdnml_smemwipe_u64(dst->limbs, dst->cap, 1, dst->cap - 1, noop_toggle);  // clang-format off
    val = 0; dst = 0; src = 0; return ret_stat; // clang-format on
}
dnml_status crint_mut_dcopyi64(crint *dst, int64_t src) {
    // Pre-operation Validation & Static Analysis
    DNML_TEST_ASSERT(__STORAGE_VAL__(dst), ci_store_inval, { crint_free(dst); });
    DNML_TEST_ASSERT((!dst->poisoned), crint_poisoned, { crint_free(dst); });
    if (!__STORAGE_VAL__(dst)) { dst = 0; src = 0; return CRINT_ERR_SINVAL; }
    /* Actual Operation */ dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (dst->poisoned), (CRINT_POISON), (ret_stat));

    /* Main Operation - Copy */
    crint_canonicalize(dst); limb_t TMP_P = 1;
    limb_t *__BUFFER_P = realloc(dst->limbs, U64_BYTES);
    DNML_TEST_ASSERT(_lib_crt_neq((ptr_t)__BUFFER_P, (ptr_t)(NULL)), realloc_null, {;});
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq((ptr_t)__BUFFER_P, (ptr_t)(NULL)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (DNML_ALLOC_OOM), (CRINT_SUCCESS)
    );
    uint64_t oom; CHOOSE_OPTION((oom), (_lib_crt_eq((ptr_t)__BUFFER_P, (ptr_t)(NULL))), (0), (UINT64_MAX));
    dst->limbs = (_lib_crt_gt((ptr_t)__BUFFER_P, (ptr_t)NULL)) ? &TMP_P : __BUFFER_P;
    dst->limbs[0] = __CRT_MAG_I64__(src); dst->n = !!(src); dst->cap = 1;
    CHOOSE_OPTION((dst->sign), (_lib_crt_isneg(src)), (-1), (1));
    /* Invalid Metadata Fill & Aggresive Cleanup */ // clang-format off
    dst->limbs[0] &= oom; dst->n &= oom; dst->cap &= oom; dst->sign &= oom;
    dst->poisoned &= oom; TMP_P = 0; __BUFFER_P = 0; oom = 0; dst = 0; src = 0;
    return ret_stat; // clang-format on
}
/* -------------  Mutative LARGE Copies ------------- */
dnml_status crint_mut_copyf128(crint *dst, long double src) { return CRINT_SUCCESS; }
dnml_status crint_mut_dcopyf128(crint *dst, long double src) { return CRINT_SUCCESS; }
dnml_status crint_mut_ocopyf128(crint *dst, long double src) { return CRINT_SUCCESS; }
dnml_status crint_mut_tover_copyf128(crint *dst, long double src) { return CRINT_SUCCESS; }
dnml_status crint_mut_copy(crint *dst, crint src) {
    /* Pre-operation Validation & Static Analysis */
    DNML_TEST_ASSERT((crint_validate(src)), ci_full_contract, { crint_free(&src); crint_free(dst); });
    DNML_TEST_ASSERT((__STORAGE_VAL__(dst)), ci_store_inval, { crint_free(&src); crint_free(dst); });
    DNML_TEST_ASSERT(!(dst->poisoned) & !((src.poisoned)), crint_poisoned, { crint_free(&src); crint_free(dst); });
    if (!crint_validate(src)) { pbv_crint_clear(src); dst = 0; return CRINT_ERR_INVAL; }
    if (!__STORAGE_VAL__(dst)) { pbv_crint_clear(src); dst = 0; return CRINT_ERR_INVAL; }
    /* Actual operation */
    dnml_status ret_stat = CRINT_SUCCESS; uint8_t noop_toggle = false;
    CHOOSE_OPTION((ret_stat), (dst->poisoned), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((noop_toggle), (dst->poisoned), (true), (noop_toggle));

    /* Main Operation - Copy */
    crint_canonicalize(dst); dnml_status rcap_stat = CRINT_SUCCESS; uint64_t correctly_set;
    if (_lib_crt_lt(dst->cap, src.n)) {
        rcap_stat = crint_reserve(dst, src.n);
        CHOOSE_OPTION((ret_stat),
            (_lib_crt_eq(rcap_stat, DNML_ALLOC_OOM) &
            (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
            (rcap_stat), (ret_stat)
        );
    }
    CHOOSE_OPTION((correctly_set), (_lib_crt_neq(ret_stat, DNML_ALLOC_OOM)), (UINT64_MAX), (0));
    limb_t dst_buf[FAKE_BUF_CAP] = {0}, src_buf[FAKE_BUF_CAP] = {0};
    size_t end, src_size; limb_t *dst_limbs, *src_limbs;
    dst_limbs = (_lib_crt_neq(ret_stat, DNML_ALLOC_OOM)) ? dst->limbs : dst_buf;
    src_limbs = (_lib_crt_neq(ret_stat, DNML_ALLOC_OOM)) ? src.limbs : src_buf;
    CHOOSE_OPTION((dst->cap), (_lib_crt_neq(ret_stat, DNML_ALLOC_OOM)), (dst->cap), (FAKE_BUF_CAP));
    CHOOSE_OPTION((src_size), (_lib_crt_neq(ret_stat, DNML_ALLOC_OOM)), (src.n), (FAKE_BUF_CAP));
    CHOOSE_OPTION((end), (_lib_crt_neq(ret_stat, DNML_ALLOC_OOM)), (src.n), (FAKE_BUF_CAP));
    CHOOSE_OPTION((end), (!end), (0), (end - 1)); 
    dst->n = src.n; dst->sign = src.sign; __libdnml_smemcpy_u64(
        dst_limbs, src_limbs, dst->cap, src_size, 0, 
        end, (noop_toggle | _lib_crt_neq(rcap_stat, CRINT_SUCCESS))
    );
    size_t clear_start, clear_end;
    CHOOSE_OPTION((clear_start), (_lib_crt_neq(ret_stat, DNML_ALLOC_OOM)), (dst->n), (0));
    CHOOSE_OPTION((clear_end), (_lib_crt_neq(ret_stat, DNML_ALLOC_OOM)), (dst->cap), (FAKE_BUF_CAP));
    __libdnml_smemwipe_u64(
        dst_limbs, dst->cap, clear_start, clear_end - 1, (
        noop_toggle & _lib_crt_neq(rcap_stat, DNML_ALLOC_OOM)
    ));

    /* Invalid Metadata Wiping & Aggressive  */ // clang-format off
    dst->poisoned &= correctly_set; dst->sign &= correctly_set; dst->n &= correctly_set;
    dst->cap &= correctly_set; rcap_stat = 0; correctly_set = 0; end = 0; dst_limbs = 0; src_limbs = 0;
    __libdnml_smemwipe_u64(dst_buf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(src_buf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    clear_start = 0; clear_end = 0; dst = 0; pbv_crint_clear(src); return ret_stat; // clang-format on
}
dnml_status crint_mut_dcopy(crint *dst, crint src) {
    /* Pre-operation Validation & Static Analysis */
    DNML_TEST_ASSERT((crint_validate(src)), ci_full_contract, { crint_free(&src); crint_free(dst); });
    DNML_TEST_ASSERT((__STORAGE_VAL__(dst)), ci_store_inval, { crint_free(&src); crint_free(dst); });
    DNML_TEST_ASSERT(!(dst->poisoned) & !((src.poisoned)), crint_poisoned, { crint_free(&src); crint_free(dst); });
    if (!crint_validate(src)) { pbv_crint_clear(src); dst = 0; return CRINT_ERR_INVAL; }
    if (!__STORAGE_VAL__(dst)) { pbv_crint_clear(src); dst = 0; return CRINT_ERR_INVAL; }
    /* Actual operation */
    dnml_status ret_stat = CRINT_SUCCESS; uint8_t noop_toggle = false;
    CHOOSE_OPTION((ret_stat), (dst->poisoned | src.poisoned), (CRINT_POISON), (ret_stat));
    CHOOSE_OPTION((noop_toggle), (dst->poisoned | src.poisoned), (true), (noop_toggle));

    /* Main Operation - Copy */
    crint_canonicalize(dst); dnml_status resize_stat;
    resize_stat = crint_resize(dst, src.n);
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_eq(resize_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (resize_stat), (ret_stat)
    );
    uint64_t oom_filter = (uint64_t)(-(int64_t)(_lib_crt_neq(ret_stat, DNML_ALLOC_OOM)));
    limb_t dst_buf[FAKE_BUF_CAP] = {0}, src_buf[FAKE_BUF_CAP] = {0}; size_t end, src_size; limb_t *dst_limbs, *src_limbs;
    dst_limbs = (_lib_crt_neq(ret_stat, DNML_ALLOC_OOM)) ? dst->limbs : dst_buf;
    src_limbs = (_lib_crt_neq(ret_stat, DNML_ALLOC_OOM)) ? src.limbs : src_buf;
    CHOOSE_OPTION((dst->cap), (_lib_crt_neq(ret_stat, DNML_ALLOC_OOM)), (dst->cap), (FAKE_BUF_CAP));
    CHOOSE_OPTION((src_size), (_lib_crt_neq(ret_stat, DNML_ALLOC_OOM)), (src.n), (FAKE_BUF_CAP));
    CHOOSE_OPTION((end), (_lib_crt_neq(ret_stat, DNML_ALLOC_OOM)), (src.n), (FAKE_BUF_CAP));
    CHOOSE_OPTION((end), (!end), (0), (end - 1)); dst->n = src.n; dst->sign = src.sign;
    __libdnml_smemcpy_u64(
        dst_limbs, src_limbs, dst->cap, src_size, 0, end,
        (noop_toggle & _lib_crt_neq(resize_stat, DNML_ALLOC_OOM))
    );
    /* Clearing Out Values to Invalidity & Aggresive Cleanup */ // clang-format off
    dst->n &= oom_filter; dst->cap &= oom_filter; dst->poisoned &= oom_filter;
    dst->sign &= oom_filter; resize_stat = 0; oom_filter = 0; end = 0;
    __libdnml_smemwipe_u64(dst_buf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(src_buf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    dst_limbs = 0; src_limbs = 0; dst = 0; pbv_crint_clear(src); return ret_stat; // clang-format on
}
dnml_status crint_mut_ocopy(crint *dst, crint src) {
    /* Pre-operation Validation & Static Analysis */
    DNML_TEST_ASSERT((crint_validate(src)), ci_full_contract, { crint_free(&src); crint_free(dst); });
    DNML_TEST_ASSERT((__STORAGE_VAL__(dst)), ci_store_inval, { crint_free(&src); crint_free(dst); });
    DNML_TEST_ASSERT(!(dst->poisoned) & !((src.poisoned)), crint_poisoned, { crint_free(&src); crint_free(dst); });
    if (!crint_validate(src)) { pbv_crint_clear(src); dst = 0; return CRINT_ERR_INVAL; }
    if (!__STORAGE_VAL__(dst)) { pbv_crint_clear(src); dst = 0; return CRINT_ERR_INVAL; }
    /* Actual operation */
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
    /* Aggresive Post-Operation Clean-up */ // clang-format off
    noop_toggle = 0; noop_toggle = 0; dst = 0; pbv_crint_clear(src); return ret_stat; // clang-format on
}
dnml_status crint_mut_tover_copy(crint *dst, crint src) {
    /* Pre-operation Validation & Static Analysis */
    DNML_TEST_ASSERT((crint_validate(src)), ci_full_contract, { crint_free(&src); crint_free(dst); });
    DNML_TEST_ASSERT((__STORAGE_VAL__(dst)), ci_store_inval, { crint_free(&src); crint_free(dst); });
    DNML_TEST_ASSERT(!(dst->poisoned) & !((src.poisoned)), crint_poisoned, { crint_free(&src); crint_free(dst); });
    if (!crint_validate(src)) { pbv_crint_clear(src); dst = 0; return CRINT_ERR_INVAL; }
    if (!__STORAGE_VAL__(dst)) { pbv_crint_clear(src); dst = 0; return CRINT_ERR_INVAL; }
    /* Actual operation */
    dnml_status ret_status = CRINT_SUCCESS; uint8_t noop_toggle = false;
    CHOOSE_OPTION((ret_status), (dst->poisoned | src.poisoned), (CRINT_POISON), (ret_status));
    CHOOSE_OPTION((noop_toggle), (dst->poisoned | src.poisoned), (true), (noop_toggle));

    /* Main Operation - Copy */
    size_t op_range; CHOOSE_OPTION((op_range), (_lib_crt_lt(dst->cap, src.n)), (dst->cap), (src.n));
    size_t end; CHOOSE_OPTION((end), (!op_range), (0), (op_range - 1));
    __libdnml_smemcpy_u64(dst->limbs, src.limbs, dst->cap, src.n, 0, op_range, noop_toggle);
    dst->n = op_range; dst->sign = src.sign;
    __libdnml_smemwipe_u64(dst->limbs, dst->cap, dst->n, dst->cap - 1, noop_toggle);
    /* Aggrestive Post-Operation Clean-up */ // clang-format off
    op_range = 0; noop_toggle = 0; dst = 0; pbv_crint_clear(src); return ret_status; // clang-format on
}
/* -------------  Functional SMALL Copies ------------- */
crint crint_copyu64(uint64_t src, dnml_status *err) {
    crint dst; dnml_status new_stat; uint64_t correctly_set;
    limb_t dst_tmp_p[1] = {0}; new_stat = crint_new(&dst);
    dst.limbs = (_lib_crt_neq(new_stat, DNML_ALLOC_OOM)) ? dst.limbs : dst_tmp_p;
    CHOOSE_OPTION((correctly_set), (_lib_crt_neq(new_stat, DNML_ALLOC_OOM)), (UINT64_MAX), (0));
    /* Setting Up correctly - Standard Case */
    dst.limbs[0] = src; dst.n = !!(src);
    dst.cap = 1; dst.sign = 1;
    /* Setting Up invalid metadata - DNML_ALLOC_OOM */ // clang-format off
    dst.limbs[0] &= correctly_set; dst.n &= correctly_set;
    dst.cap &= correctly_set; dst.sign &= correctly_set;
    if (_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))) {
        CHOOSE_OPTION((*err), (_lib_crt_neq(new_stat, DNML_ALLOC_OOM)), (CRINT_SUCCESS), (DNML_ALLOC_OOM));
    } dst_tmp_p[0] = 0; new_stat = 0; correctly_set = 0; src = 0; err = 0; return dst; // clang-format on
}
crint crint_copyi64(int64_t src, dnml_status *err) {
    crint dst; dnml_status new_stat; uint64_t correctly_set;
    limb_t dst_tmp_p[1] = {0}; new_stat = crint_new(&dst);
    dst.limbs = (_lib_crt_neq(new_stat, DNML_ALLOC_OOM)) ? dst.limbs : dst_tmp_p;
    CHOOSE_OPTION((correctly_set), (_lib_crt_neq(new_stat, DNML_ALLOC_OOM)), (UINT64_MAX), (0));
    /* Setting Up correctly - Standard Case */
    dst.limbs[0] = __CRT_MAG_I64__(src); dst.n = !!(src); dst.cap = 1;
    CHOOSE_OPTION((dst.sign), (_lib_crt_isneg(src)), (-1), (1));
    /* Setting Up invalid metadata - DNML_ALLOC_OOM */ // clang-format off
    dst.limbs[0] &= correctly_set; dst.n &= correctly_set;
    dst.cap &= correctly_set; dst.sign &= correctly_set;
    if (_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))) {
        CHOOSE_OPTION((*err), (_lib_crt_neq(new_stat, DNML_ALLOC_OOM)), (CRINT_SUCCESS), (DNML_ALLOC_OOM));
    } dst_tmp_p[0] = 0; new_stat = 0; correctly_set = 0; src = 0; err = 0; return dst; // clang-format on
}
/* -------------  Functional LARGE Copies ------------- */
crint crint_copyf128(long double src, dnml_status *err) { return (crint){0}; }
crint crint_ocopyf128(long double src, size_t output_cap, dnml_status *err) { return (crint){0}; }
crint crint_tover_copyf128(long double src, size_t output_cap, dnml_status *err) { return (crint){0}; }
crint crint_copy(crint src, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_TEST_ASSERT((crint_validate(src)), ci_full_contract, { crint_free(&src); });
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))), null_err, { crint_free(&src); });
    DNML_TEST_ASSERT(!(src.poisoned), crint_poisoned, { crint_free(&src); });
    if (!crint_validate(src)) {
        if (_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))) *err = CRINT_ERR_INVAL;
        pbv_crint_clear(src); return __CRINT_ERRVAL__();
    }
    /* Actual Operation */
    dnml_status ret_stat = CRINT_SUCCESS; uint8_t noop_toggle = false;
    CHOOSE_OPTION((ret_stat), (src.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));

    /* Main Operation - Copy */
    crint dst; dnml_status new_stat; uint64_t correctly_set;
    limb_t dst_tmp_p[FAKE_BUF_CAP] = {0}, src_tmp_p[FAKE_BUF_CAP] = {0}; limb_t* src_limbs;
    new_stat = crint_snew(&dst, src.n); // Guaraneed dst->cap >= 1
    CHOOSE_OPTION((ret_stat),
        (_lib_crt_neq(new_stat, DNML_ALLOC_OOM) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (new_stat), (ret_stat)
    );
    dst.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? dst.limbs : dst_tmp_p;
    CHOOSE_OPTION((noop_toggle), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), (noop_toggle), (true));
    correctly_set = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS)));
    src_limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? src.limbs : src_tmp_p;

    /* Setting Up correctly - Standard Case */
    size_t iter_cnt = (size_t)(src.n / FAKE_BUF_CAP + 1);
    size_t end; CHOOSE_OPTION( (end), (!(src.n)), 0, (src.n - 1));
    CHOOSE_OPTION((end), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), (end), (FAKE_BUF_CAP - 1));
    CHOOSE_OPTION((dst.cap), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), (dst.cap), (FAKE_BUF_CAP));
    while (iter_cnt--) __libdnml_smemcpy_u64( dst.limbs, src.limbs, dst.cap, src.n,  0, end, noop_toggle);
    dst.n = src.n; dst.sign = src.sign; *err = CRINT_SUCCESS;

    /* Setting Up invalid metadata */ // clang-format off
    limb_t* chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : dst.limbs;
    free(chosen_freed); // Mandated to be safe nop since ANSI-C
    dst.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? dst.limbs : 0;
    dst.n &= correctly_set; dst.cap &= correctly_set;
    dst.sign &= correctly_set; dst.poisoned = src.poisoned;
    /* Aggressive Post-operation Cleanup */
    if (_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))) *err = ret_stat;
    new_stat = 0; correctly_set = 0; noop_toggle = 0; src_limbs = 0; iter_cnt = 0;
    __libdnml_smemwipe_u64(dst_tmp_p, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(src_tmp_p, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    end = 0; ret_stat = 0; pbv_crint_clear(src); err = 0; chosen_freed = 0; return dst; // clang-format on
}
crint crint_ocopy(crint src, size_t output_cap, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_TEST_ASSERT((crint_validate(src)), ci_full_contract, { crint_free(&src); });
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))), null_err, { crint_free(&src); });
    DNML_TEST_ASSERT(!(src.poisoned), crint_poisoned, { crint_free(&src); });
    if (!crint_validate(src)) {
        if (_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))) *err = CRINT_ERR_INVAL;
        pbv_crint_clear(src); return __CRINT_ERRVAL__();
    }
    /* Actual Operation */
    dnml_status ret_stat = CRINT_SUCCESS; uint8_t noop_toggle = false;
    CHOOSE_OPTION((ret_stat), (src.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));

    /* Main Operation - Copy */
    crint dst; uint64_t correctly_set;
    CHOOSE_OPTION((ret_stat), (_lib_crt_lt(output_cap, src.n)), (CRINT_ERR_RANGE), (ret_stat));
    CHOOSE_OPTION((noop_toggle), ((_lib_crt_lt(output_cap, src.n)) & (!(noop_toggle))), (true), (noop_toggle));
    limb_t dst_tmp_p[FAKE_BUF_CAP] = {0}, src_tmp_p[FAKE_BUF_CAP] = {0};
    dnml_status new_stat = crint_snew(&dst, src.n);
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_neq(new_stat, CRINT_SUCCESS)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        (new_stat), (ret_stat)
    );
    CHOOSE_OPTION((noop_toggle), (_lib_crt_neq(ret_stat, CRINT_SUCCESS)), (true), (noop_toggle));
    correctly_set = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS)));
    limb_t* dst_limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? dst.limbs : dst_tmp_p;
    limb_t* src_limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? src.limbs : src_tmp_p;

    /* Setting Up correctly - Standard Case */
    size_t iter_cnt = (size_t)(src.n / FAKE_BUF_CAP + 1);
    size_t end; CHOOSE_OPTION( (end), (!(src.n)), 0, (src.n - 1));
    CHOOSE_OPTION((end), (_lib_crt_eq(ret_stat, DNML_ALLOC_OOM)), (end), (FAKE_BUF_CAP - 1));
    CHOOSE_OPTION((dst.cap), (_lib_crt_eq(ret_stat, DNML_ALLOC_OOM)), (dst.cap), (FAKE_BUF_CAP));
    while (iter_cnt--) __libdnml_smemcpy_u64(dst_limbs, src_limbs, dst.cap, src.n, 0, end, noop_toggle);
    dst.n = src.n; dst.sign = src.sign;

    /* Setting Up invalid metadata + Agressive Stack Cleanup */ // clang-format off
    limb_t* chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : dst.limbs;
    free(chosen_freed); // Mandated to be safe nop since ANSI-C
    dst.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? dst.limbs : 0;
    dst.n &= correctly_set; dst.cap &= correctly_set;
    dst.sign &= correctly_set; dst.poisoned = src.poisoned;
    /* Aggressive Post-operation Cleanup */
    if (_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))) *err = ret_stat;
    ret_stat = 0; correctly_set = 0; new_stat = 0; noop_toggle = 0; dst_limbs = 0; output_cap = 0;
    __libdnml_smemwipe_u64(dst_tmp_p, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    __libdnml_smemwipe_u64(src_tmp_p, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
    src_limbs = 0; iter_cnt = 0; end = 0; pbv_crint_clear(src); err = 0; chosen_freed = 0; return dst; // clang-format on
}
crint crint_tover_copy(crint src, size_t output_cap, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_TEST_ASSERT((crint_validate(src)), ci_full_contract, { crint_free(&src); });
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))), null_err, { crint_free(&src); });
    DNML_TEST_ASSERT(!(src.poisoned), crint_poisoned, { crint_free(&src); });
    if (!crint_validate(src)) {
        if (_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))) *err = CRINT_ERR_INVAL;
        pbv_crint_clear(src); return __CRINT_ERRVAL__();
    }
    /* Actual Operation */
    dnml_status ret_stat = CRINT_SUCCESS; uint8_t noop_toggle = false;
    CHOOSE_OPTION((ret_stat), (src.poisoned & _lib_crt_eq(ret_stat, CRINT_SUCCESS)), (CRINT_POISON), (ret_stat));

    /* Main Operation - Copy */ crint dst; uint64_t correctly_set;
    size_t op_range; CHOOSE_OPTION((op_range), (_lib_crt_lt(output_cap, src.n)), (output_cap), (src.n));
    limb_t dst_tmp_p[FAKE_BUF_CAP] = {0}, src_tmp_p[FAKE_BUF_CAP] = {0};
    dnml_status new_stat = crint_snew(&dst, src.n); // Guaraneed dst->cap >= 1
    CHOOSE_OPTION((ret_stat), (
        _lib_crt_neq(new_stat, CRINT_SUCCESS) &
        _lib_crt_eq(ret_stat, CRINT_SUCCESS)),
        (new_stat), (ret_stat)
    );
    CHOOSE_OPTION((noop_toggle), (_lib_crt_neq(ret_stat, CRINT_SUCCESS)), (true), (noop_toggle));
    correctly_set = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS)));
    limb_t* dst_limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? dst.limbs : dst_tmp_p;
    limb_t* src_limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? src.limbs : src_tmp_p;

    /* Setting Up correctly - Standard Case */
    size_t iter_cnt = (size_t)(src.n / FAKE_BUF_CAP + 1); size_t end;
    CHOOSE_OPTION((end), (!(op_range)), 0, (op_range - 1));
    CHOOSE_OPTION((end), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), (end), (FAKE_BUF_CAP - 1));
    CHOOSE_OPTION((dst.cap), (_lib_crt_eq(ret_stat, CRINT_SUCCESS)), (dst.cap), (FAKE_BUF_CAP));
    while (iter_cnt--) __libdnml_smemcpy_u64(dst.limbs, src.limbs, dst.cap, src.n, 0, end, noop_toggle);
    dst.n = src.n; dst.sign = src.sign;

    /* Setting Up invalid metadata - DNML_ALLOC_OOM */ // clang-format off
    limb_t* chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : dst.limbs;
    free(chosen_freed); // Mandated to be safe nop since ANSI-C
    dst.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? dst.limbs : 0;
    dst.n &= correctly_set; dst.cap &= correctly_set;
    dst.sign &= correctly_set; dst.poisoned = src.poisoned;
    /* Aggressive Post-operation Cleanuo */
    if (_lib_crt_neq((ptr_t)(err), (ptr_t)(NULL))) *err = ret_stat; // Conditional Branching here is acceptable
    ret_stat = 0; new_stat = 0; correctly_set = 0; op_range = 0; iter_cnt = 0; output_cap = 0;
    __libdnml_smemwipe_u64(dst_tmp_p, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false); dst_limbs = 0;
    __libdnml_smemwipe_u64(src_tmp_p, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false); src_limbs = 0;
    end = 0; noop_toggle = 0; pbv_crint_clear(src); err = 0; chosen_freed = 0; return dst; // clang-format on
}




//* ===================================== STATE ALTERATION FUNCTIONS ===================================== *//
dnml_status crint_canonicalize(crint *x) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {;});
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; return CRINT_NULL; }
    // Fix invalid capacity
    uint8_t cap_invalid = (_lib_crt_lt(x->cap, 1));
    CHOOSE_OPTION(x->cap, cap_invalid, x->cap, 1);
    CHOOSE_OPTION(x->n, cap_invalid, x->n, 0);
    CHOOSE_OPTION(x->sign, cap_invalid, x->sign, 1);

    // Clamp n to capacity
    uint8_t n_overflow = (_lib_crt_gt(x->n, x->cap));
    CHOOSE_OPTION(x->n, n_overflow, x->n, x->cap);

    // Fix invalid sign
    uint8_t sign_invalid = (_lib_crt_neq(x->sign, -1)) & (_lib_crt_neq(x->sign, 1));
    CHOOSE_OPTION(x->n, sign_invalid, x->n, 0);
    CHOOSE_OPTION(x->sign, sign_invalid, x->sign, 1); // clang-format off
    cap_invalid = 0; n_overflow = 0; sign_invalid = 0; x = 0; // clang-format on
    return CRINT_SUCCESS;
}
dnml_status crint_normalize(crint *x) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {;});
    if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; return CRINT_NULL; }
    __CRINT_TRIM_LZ__(x); CHOOSE_OPTION((x->sign), (!x->n), (1), (-1));
    return CRINT_SUCCESS;
}
dnml_status crint_transfer(crint *dst, crint *src) {
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)dst, (ptr_t)(NULL))) & (_lib_crt_neq((ptr_t)(src), (ptr_t)(NULL))), input_null, {;});
    DNML_TEST_ASSERT((__STORAGE_VAL__(dst) & __STORAGE_VAL__(src)), ci_store_inval, { crint_free(dst); crint_free(src); });
    if (_lib_crt_eq((ptr_t)dst, (ptr_t)(NULL)) | _lib_crt_eq((ptr_t)(src), (ptr_t)(NULL))) { dst = 0; src = 0; return CRINT_NULL;}
    if (!__STORAGE_VAL__(dst) | !__STORAGE_VAL__(src)) { dst = 0; src = 0; return CRINT_ERR_SINVAL; } // clang-format off
    crint_free(dst);
    dst->limbs = src->limbs; dst->n = src->n; dst->cap = src->cap;
    dst->sign = src->sign; dst->poisoned = src->poisoned;
    /* Invalidate src */
    src->limbs = NULL; src->n = 0; src->cap = 1; src->sign = 1;
    src->poisoned = false; dst = 0; src = 0; return CRINT_SUCCESS; // clang-format on
}
dnml_status crint_resize(crint *x, size_t k) {
    // Pre-operation Validation & Static Analysis
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {;});
    DNML_TEST_ASSERT(__STORAGE_VAL__(x), ci_store_inval, { crint_free(x); });
    if (!__STORAGE_VAL__(x)) { x = 0; k = 0; return CRINT_ERR_SINVAL; }
    DNML_TEST_ASSERT((x->poisoned), crint_poisoned, {;});
    DNML_TEST_ASSERT((k), "Invalid Capacity Request (-Einval_cap_request)", {;});
    dnml_status ret_stat = CRINT_SUCCESS; uint64_t oom_mask = UINT64_MAX;
    CHOOSE_OPTION((ret_stat), (x->poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));

    /* Memory Clearance on Resizing to a smaller size */
    size_t start, end, op_cap, dist;
    CHOOSE_OPTION((dist), (_lib_crt_lt(k, x->cap)), (x->cap - k), (k - x->cap))
    CHOOSE_OPTION((start), (_lib_crt_lt(k, x->cap)), (k - 1), (0));
    CHOOSE_OPTION((end), (_lib_crt_lt(k, x->cap)), (x->cap - 1), (__clamp_size(x->cap, dist - 1)));
    __libdnml_smemwipe_u64(x->limbs, x->cap, start, end, (x->poisoned | (!(_lib_crt_lt(k, x->cap)))));

    /* Main Resizing */
    limb_t* operated = (_lib_crt_eq(ret_stat, CRINT_POISON)) ? malloc(1) : x->limbs;
    size_t normalized_size = (k) | (!(k)); size_t op_size;
    CHOOSE_OPTION((op_size), (_lib_crt_eq(ret_stat, CRINT_POISON)), (1), (normalized_size));
    limb_t *__BUFFER_P = realloc(operated, op_size * U64_BYTES);
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)__BUFFER_P, (ptr_t)(NULL))), realloc_null, { if (_lib_crt_neq((ptr_t)(operated), (ptr_t)(NULL))) free(operated); });
    CHOOSE_OPTION((ret_stat), (_lib_crt_eq((ptr_t)__BUFFER_P, (ptr_t)(NULL)) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), DNML_ALLOC_OOM, ret_stat);
    CHOOSE_OPTION((oom_mask), (_lib_crt_eq((ptr_t)__BUFFER_P, (ptr_t)(NULL)) & (_lib_crt_neq(ret_stat, CRINT_POISON))), 0, oom_mask);

    x->limbs = (_lib_crt_neq(ret_stat, CRINT_POISON)) ? __BUFFER_P : x->limbs;
    CHOOSE_OPTION((x->cap), (_lib_crt_neq(ret_stat, CRINT_POISON)), (normalized_size & oom_mask), (x->cap));
    CHOOSE_OPTION((x->n), (_lib_crt_neq(ret_stat, CRINT_POISON)), (crtmin(normalized_size, x->cap) & oom_mask), (x->n));
    /* Post-operation Aggrestive Clearance */
    if (_lib_crt_eq(ret_stat, CRINT_POISON) & _lib_crt_neq((ptr_t)__BUFFER_P, (ptr_t)(NULL))) free(__BUFFER_P); // clang-format off
    start = 0; end = 0; op_cap = 0; operated = 0; oom_mask = 0; __BUFFER_P = 0;
    normalized_size = 0; op_size = 0; x = 0; k = 0; return ret_stat; // clang-format on
}
dnml_status crint_reserve(crint *x, size_t k) {
    // Pre-operation Validation & Static Analysis
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {;});
    DNML_TEST_ASSERT(__STORAGE_VAL__(x), ci_store_inval, { crint_free(x); });
    if (!__STORAGE_VAL__(x)) { x = 0; k = 0; return CRINT_ERR_SINVAL; }
    DNML_TEST_ASSERT((x->poisoned), crint_poisoned, {;});
    dnml_status ret_stat = CRINT_SUCCESS; uint64_t oom_mask = UINT64_MAX;
    CHOOSE_OPTION((ret_stat), (x->poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));

    /* Main Resizing */
    size_t new_cap = !!(x->cap); while (_lib_crt_lt(new_cap, k)) new_cap *= 2;
    limb_t *operated = (_lib_crt_eq(ret_stat, CRINT_POISON) | _lib_crt_eq(new_cap, x->cap)) ? malloc(U64_BYTES) : x->limbs;
    limb_t* __BUFFER_P = realloc(operated, new_cap * U64_BYTES);
    DNML_TEST_ASSERT(_lib_crt_neq((ptr_t)__BUFFER_P, (ptr_t)(NULL)), realloc_null, {;});
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq((ptr_t)__BUFFER_P, (ptr_t)(NULL))) & (_lib_crt_neq(new_cap, x->cap)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), DNML_ALLOC_OOM, ret_stat
    );
    oom_mask = (uint64_t)(-(int64_t)(!(
        (_lib_crt_eq((ptr_t)__BUFFER_P, (ptr_t)(NULL))) &
        (_lib_crt_neq(new_cap, x->cap)) &
        (_lib_crt_neq(ret_stat, CRINT_POISON))
    )));
    // DNML_ALLOC_OOM --> Always assign new
    // CRINT_SUCCESS ---> ASSIGN NEW IF NEEDED (ACTUALLY NEW CAPACITY)
    // CRINT_POISON ----> NEVER
    uint8_t change_cap = (
        (_lib_crt_eq(ret_stat, DNML_ALLOC_OOM) |
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS) &
         _lib_crt_eq(new_cap, x->cap)))
    );
    x->limbs = (change_cap) ? __BUFFER_P : x->limbs;
    CHOOSE_OPTION((x->cap), (change_cap), (new_cap & oom_mask), (x->cap));
    /* Post-operation Aggrestive Clearance */ // clang-format off
    if ((_lib_crt_eq(ret_stat, CRINT_POISON) |
        _lib_crt_eq(new_cap, x->cap)) &
        _lib_crt_neq((ptr_t)__BUFFER_P, (ptr_t)(NULL))
    ) free(__BUFFER_P); // clang-format off
    oom_mask = 0; operated = 0;  new_cap = 0; __BUFFER_P = 0;
    x = 0; k = 0; return ret_stat; // clang-format on

}
dnml_status crint_shrink(crint *x, size_t k) { /* Maximum capacity */
    // Pre-operation Validation & Static Analysis
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {;});
    DNML_TEST_ASSERT(__STORAGE_VAL__(x), ci_store_inval, { crint_free(x); });
    if (!__STORAGE_VAL__(x)) { x = 0; k = 0; return CRINT_ERR_SINVAL; }
    DNML_TEST_ASSERT((x->poisoned), crint_poisoned, {;});
    DNML_TEST_ASSERT((k), "Invalid Capacity Request (-Einval_cap_request)", {;});
    dnml_status ret_stat = CRINT_SUCCESS; uint64_t oom_mask = UINT64_MAX;
    CHOOSE_OPTION((ret_stat), (x->poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));

    /* Memory Clearance on Resizing to a smaller size */
    size_t start, end, op_cap, new_cap = x->cap; while (_lib_crt_gt(new_cap, k)) --new_cap;
    CHOOSE_OPTION((start), (_lib_crt_lt(new_cap, x->cap)), (new_cap - 1), (0));
    CHOOSE_OPTION((end), (_lib_crt_lt(new_cap, x->cap)), (x->cap - 1), (0));
    __libdnml_smemwipe_u64(x->limbs, x->cap, start, end, (x->poisoned));

    /* Main Resizing */
    limb_t* operated = (_lib_crt_eq(ret_stat, CRINT_POISON)) ? malloc(U64_BYTES) : x->limbs;
    size_t normalized_size = (k) | (!(k)); size_t opsize;
    CHOOSE_OPTION((opsize), (_lib_crt_eq(ret_stat, CRINT_POISON)), (1), (normalized_size));
    limb_t* __BUFFER_P = realloc(operated, opsize * U64_BYTES);
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)__BUFFER_P, (ptr_t)(NULL))), realloc_null, { if (_lib_crt_neq((ptr_t)(operated), (ptr_t)(NULL))) free(operated); });
    CHOOSE_OPTION((ret_stat), (
        (_lib_crt_eq((ptr_t)__BUFFER_P, (ptr_t)(NULL))) &
        (_lib_crt_neq(new_cap, x->cap)) &
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
        DNML_ALLOC_OOM, ret_stat
    );
    CHOOSE_OPTION((oom_mask), (
        (_lib_crt_eq((ptr_t)__BUFFER_P, (ptr_t)(NULL))) & (_lib_crt_neq(new_cap, x->cap)) &
        (_lib_crt_neq(ret_stat, CRINT_POISON))), 0, oom_mask
    );
    // DNLM_ALLOC_OOM --> Always assign new
    // CRINT_SUCCESS ---> ASSIGN NEW IF NEEDED (ACTUALLY NEW CAPACITY)
    // CRINT_POISON ----> NEVER
    uint8_t change_cap = (
        (_lib_crt_eq(ret_stat, DNML_ALLOC_OOM) |
        (_lib_crt_eq(ret_stat, CRINT_SUCCESS) &
         _lib_crt_eq(new_cap, x->cap)))
    );
    x->limbs = (change_cap) ? __BUFFER_P : x->limbs;
    CHOOSE_OPTION((x->cap), (change_cap), (normalized_size & oom_mask), (x->cap));
    CHOOSE_OPTION((x->n), (change_cap), (crtmin(normalized_size, x->cap) & oom_mask), (x->n));
    /* Post-operation Aggrestive Clearance */
    if (
        (_lib_crt_eq(ret_stat, CRINT_POISON) |
        _lib_crt_eq(new_cap, x->cap)) &
        _lib_crt_neq((ptr_t)__BUFFER_P, (ptr_t)(NULL))
    ) free(__BUFFER_P); // clang-format off
    start = 0; end = 0; op_cap = 0; operated = 0; oom_mask = 0; __BUFFER_P = 0;
    normalized_size = 0; opsize = 0; x = 0; k = 0; return ret_stat; // clang-format on
}
dnml_status crint_reset(crint *x) {
    // Pre-operation Validation & Static Analysis
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)x, (ptr_t)(NULL))), input_null, {;});
    DNML_TEST_ASSERT(__STORAGE_VAL__(x), ci_store_inval, { crint_free(x); });
    if (!__STORAGE_VAL__(x)) { x = 0; return CRINT_ERR_SINVAL; }
    DNML_TEST_ASSERT((x->poisoned), crint_poisoned, {;});
    dnml_status ret_stat = CRINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned), (CRINT_POISON), (ret_stat));
    __libdnml_smemwipe_u64(x->limbs, x->cap, 0, x->cap - 1, (x->poisoned));
    x->n = 0; x->sign = 1; // clang-format off
    x = 0; return ret_stat; // clang-format on
}
uint8_t __STORAGE_VAL__(crint *x) {
    uint8_t ret = 0;
    ret |= _lib_crt_eq((ptr_t)x->limbs, (ptr_t)(NULL));
    ret |= _lib_crt_lt(x->cap, 1); // clang-format off
    x = 0; return (!(ret)); // clang-format on
}
bool crint_validate(crint x) {
    /* State Validation */
    uint8_t ret = 0;
    ret |= (_lib_crt_eq((ptr_t)(x.limbs), (ptr_t)(NULL)));
    ret |= (_lib_crt_lt(x.cap, 1));
    ret |= (_lib_crt_gt(x.n, x.cap));
    limb_t fake_dst = 0; limb_t *dst;
    dst = (!ret) ? &fake_dst : &x.limbs[x.n - 1];
    ret |= (_lib_crt_neq(x.sign, 1) & _lib_crt_neq(x.sign, -1));
    /* Arithmetic Validation */
    ret |= (_lib_crt_eq(*dst, 0));
    ret |= ((!x.n) & (_lib_crt_neq(x.sign, -1)));
    fake_dst = 0; dst = 0; // clang-format off
    pbv_crint_clear(x); return (bool)(!(ret)); // clang-format on
}
bool crint_pvalidate(crint *x) {
    /* State Validation */
    uint8_t ret = 0;
    ret |= _lib_crt_eq((ptr_t)x->limbs, (ptr_t)(NULL));
    ret |= (_lib_crt_lt(x->cap, 1));
    ret |= (_lib_crt_gt(x->n, x->cap));
    limb_t fake_dst = 0; limb_t *dst;
    dst = (!ret) ? &fake_dst : &x->limbs[x->n - 1];
    ret |= (_lib_crt_neq(x->sign, 1) & _lib_crt_neq(x->sign, -1));
    /* Arithmetic Validation */
    ret |= (_lib_crt_eq(*dst, 0));
    ret |= ((!x->n) & (_lib_crt_neq(x->sign, -1))); // clang-format off
    fake_dst = 0; dst = 0; x = 0; return (bool)(!(ret)); // clang-format on
}
