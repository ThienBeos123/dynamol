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



#include "bigInt_func.h"

/**
 * This variable acts as a switch from arena allocation to heap allocation
 * for bigInt's algorithms. It will always be TRUE when _DNML_ALLOC_STRAT = 2 (Alloc Heap).
 * It will also be TRUE going forward upon an incident of arena allocation overflow
 * from incorrect workspace size estimation when _DNML_ALLOC_STRAT = 1 (Alloc balanced), switching
 * from arena-allocations to the more stable but, perhaps slower, heap-allocations
 */
static const bool heap_switch = false;


//todo ========================================= === INTRODUCTION ============================================= */
/** 
  * Attribute Explanation:
  * +) sign     (uint8_t)       : Stores the sign (negative or positive)
  * +) limbs    (*uint64_t)     : Pointer to each limb that holds part of the bigInt number
  * +) n        (size_t)        : Number of currently used limbs
  * +) cap      (size_t)        : Essentially the bigInt object/number's maximum capacity
  *
  * Terms Explanation:
  * +) Mutative Model: Changes an existing bigInt object/variable value in place (Eg: x += 10; )
  * +) Functional Model:
  *     -) Creates a new variable with the value of the expression (Eg: int x = 5 + 10; )
  *     -) This transfer ownership of the allocated limbs to the caller,
  *        forcing a manual deletion of the object
  */


//* ======================================== CONSTRUCTORS & DESTRUCTOR ======================================= */
void bigInt_free(bigInt *const x) {
    free(x->limbs); x->limbs = NULL;
    x->n = 0; x->cap = 0; x->sign = 0;
}
dnml_status bigInt_new(bigInt *const x) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    if (x->limbs) return BIGINT_SUCCESS; // The bigInt is already initialized
    limb_t *__BUFFER_P = malloc(sizeof(limb_t));
    if (__BUFFER_P == NULL) return DNML_ALLOC_OOM;
    x->limbs = __BUFFER_P; x->cap = 1; x->n = 0; x->sign = 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_snew(bigInt *const x, size_t n) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    if (x->limbs != NULL) return BIGINT_SUCCESS; // already initialized
    limb_t *__BUFFER_P = malloc(n * sizeof(limb_t));
    if (__BUFFER_P == NULL) return DNML_ALLOC_OOM;
    x->limbs = __BUFFER_P; x->cap = n; x->n = 0; x->sign = 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_binew(bigInt *const x, bigInt *const y) {
    test_assert(x != NULL | y != NULL, input_null, clear_arena, BIGINT_NULL);
    if (x->limbs != NULL) return BIGINT_SUCCESS; // Already Initialized
    test_assert(bigInt_pvalidate(y), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (x->limbs == y->limbs) {
        if (x == y) return BIGINT_SUCCESS;
        x->n = y->n; x->cap = y->cap; x->sign = y->sign;
    } size_t alloc_size = (y->n) ? y->n : 1;
    limb_t *__BUFFER_P = malloc(alloc_size * sizeof(limb_t));
    if (__BUFFER_P == NULL) return DNML_ALLOC_OOM;
    x->limbs = __BUFFER_P;
    if (y->n) memcpy(x->limbs, y->limbs, y->n * sizeof(limb_t));
    x->n = y->n; x->cap = alloc_size; x->sign = (y->n) ? y->sign : 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_new_u64(bigInt *const x, const uint64_t in) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    if (x->limbs != NULL) return BIGINT_SUCCESS; // ALREADY INITIALIZED
    limb_t *__BUFFER_P = malloc(sizeof(limb_t));
    if (__BUFFER_P == NULL) return DNML_ALLOC_OOM;
    x->limbs = __BUFFER_P; x->limbs[0] = in;
    x->n = !!(in); x->cap = 1; x->sign = 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_new_i64(bigInt *const x, const int64_t in) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    if (x->limbs != NULL) return BIGINT_SUCCESS; // ALREADY INITIALIZED
    limb_t *__BUFFER_P = malloc(sizeof(limb_t));
    if (__BUFFER_P == NULL) return DNML_ALLOC_OOM;
    x->limbs = __BUFFER_P; x->limbs[0] = __MAG_I64__(in);
    x->n = !!(in); x->cap = 1; x->sign = (in < 0) ? -1 : 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_new_f128(bigInt *const x, long double in) { return FILE_ILLEGAL; /* Placeholder */  }




//* =============================================== ASSIGNMENTS ============================================== */
dnml_status bigInt_set(bigInt x, bigInt *const receiver) {
    test_assert(receiver != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_validate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    test_assert(__BIGINT_INTERNAL_PVALID__(receiver), bi_state_contract, clear_arena, BIGINT_ERR_SINVAL);
    if (receiver->limbs == x.limbs) {
        receiver->n = x.n;
        receiver->cap = x.cap;
        receiver->sign = x.sign;
    }
    size_t set_range = (receiver->cap < x.n) ? receiver->cap : x.n;
    memcpy(receiver->limbs, x.limbs, set_range * U64_BYTES);
    receiver->n     = set_range;
    receiver->sign  = (set_range) ? x.sign : 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_set_safe(bigInt x, bigInt *const receiver) {
    test_assert(receiver != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_validate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    test_assert(__BIGINT_INTERNAL_PVALID__(receiver), bi_state_contract, clear_arena, BIGINT_ERR_SINVAL);
    if (receiver->limbs == x.limbs) {
        receiver->n = x.n;
        receiver->cap = x.cap;
        receiver->sign = x.sign;
    }
    if (receiver->cap < x.n) return BIGINT_ERR_RANGE;
    memcpy(receiver->limbs, x.limbs, x.n * U64_BYTES);
    receiver->n     = x.n;
    receiver->sign  = (x.n) ? x.sign : 1;
    return BIGINT_SUCCESS;
}
/* --------- BigInt --> Primitive Types --------- */
dnml_status bigInt_setu64(bigInt x, uint64_t* receiver) {
    test_assert(receiver != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_validate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    *receiver = (x.n) ? x.limbs[0] : 0; return BIGINT_SUCCESS;
}
dnml_status bigInt_seti64(bigInt x, int64_t* receiver) {
    test_assert(receiver != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_validate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    uint64_t raw_u64 = (x.n) ? x.limbs[0] : 0;
    uint64_t abs_int64_min = (uint64_t)(llabs(INT64_MIN + 1)) + 1;
    if (raw_u64 > abs_int64_min && x.sign == -1) *receiver = (int64_t)(raw_u64 & I64_MIN_BIT_MASK) * x.sign;
    else if (raw_u64 > INT64_MAX && x.sign == 1) *receiver = (int64_t)(raw_u64 & I64_MAX_BIT_MASK) * x.sign;
    else *receiver = ((int64_t)raw_u64) * x.sign;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_setf128(bigInt x, long double* receiver) { return BIGINT_SUCCESS; } 
dnml_status bigInt_setu64_safe(bigInt x, uint64_t* receiver) {
    test_assert(receiver != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_validate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (x.sign == -1 || x.n > 1) return BIGINT_ERR_RANGE;
    *receiver = (x.n) ? x.limbs[0] : 0;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_seti64_safe(bigInt x, int64_t* receiver) {
    test_assert(receiver != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_validate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (x.n > 1) BIGINT_ERR_RANGE;
    uint64_t raw_u64 = (x.n) ? x.limbs[0] : 0;
    uint64_t abs_int64_min = (uint64_t)(llabs(INT64_MIN + 1)) + 1;
    if (raw_u64 > abs_int64_min && x.sign == -1) return BIGINT_ERR_RANGE;
    if (raw_u64 > INT64_MAX && x.sign == 1) return BIGINT_ERR_RANGE;
    *receiver = ((int64_t)raw_u64) * x.sign;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_setf128_safe(bigInt x, long double* receiver) { return BIGINT_SUCCESS; }
/* --------- Primitive Types --> BigInt --------- */
dnml_status bigInt_getu64(const uint64_t val, bigInt *const receiver) {
    test_assert(receiver != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(__BIGINT_INTERNAL_PVALID__(receiver), bi_state_contract, clear_arena, BIGINT_ERR_SINVAL);
    receiver->limbs[0] = val; receiver->n = !!(val); receiver->sign = 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_geti64(const int64_t val, bigInt *const receiver) {
    test_assert(receiver != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(__BIGINT_INTERNAL_PVALID__(receiver), bi_state_contract, clear_arena, BIGINT_ERR_SINVAL);
    uint64_t abs_val = __MAG_I64__(val);
    receiver->limbs[0] = abs_val;
    receiver->n        = (val) ? 1 : 0;
    receiver->sign     = (val < 0) ? -1 : 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_getf128(long double x, bigInt *const receiver) { return BIGINT_SUCCESS; }
dnml_status bigInt_getf128_safe(long double x, bigInt *const receiver) { return BIGINT_SUCCESS; }




//* =============================================== CONVERSIONS ============================================== */
/* --------- BigInt --> Primitive Types --------- */
uint64_t bigInt_tou64(bigInt x, dnml_status *err) {
    if (!bigInt_validate(x)) mut_gret(err, BIGINT_ERR_INVAL, 0);
    uint64_t res = (x.n) ? x.limbs[0] : 0;
    return res;
}
int64_t bigInt_toi64(bigInt x, dnml_status *err) {
    if (!bigInt_validate(x)) mut_gret(err, BIGINT_ERR_INVAL, 0);
    int64_t res; uint64_t raw_u64 = (x.n) ? x.limbs[0] : 0;
    uint64_t abs_int64_min = (uint64_t)(llabs(INT64_MIN + 1)) + 1;
    if (raw_u64 > abs_int64_min && x.sign == -1) res = (int64_t)(raw_u64 & I64_MIN_BIT_MASK) * x.sign;
    else if (raw_u64 > INT64_MAX && x.sign == 1) res = (int64_t)(raw_u64 & I64_MAX_BIT_MASK) * x.sign;
    else res = ((int64_t)raw_u64) * x.sign;
    return res;
}
long double bigInt_tof128(bigInt x, dnml_status *err) { return 0.0f; }
uint64_t bigInt_tou64_safe(bigInt x, dnml_status *err) {
    if (!bigInt_validate(x)) mut_gret(err, BIGINT_ERR_INVAL, 0);
    if (x.sign == -1 || x.n > 1) mut_gret(err, BIGINT_ERR_RANGE, 0);
    uint64_t res = (x.n) ? x.limbs[0] : 0;
    *err = BIGINT_SUCCESS; return res;
}
int64_t bigInt_toi64_safe(bigInt x, dnml_status *err) {
    if (!bigInt_validate(x)) mut_gret(err, BIGINT_ERR_INVAL, 0);
    if (x.n > 1) mut_gret(err, BIGINT_ERR_RANGE, INT64_MIN);
    uint64_t raw_u64 = (x.n) ? x.limbs[0] : 0;
    uint64_t abs_int64_min = (uint64_t)(llabs(INT64_MIN + 1)) + 1;
    if (raw_u64 > abs_int64_min && x.sign == -1) mut_gret(err, BIGINT_ERR_RANGE, INT64_MIN);
    if (raw_u64 > INT64_MAX && x.sign == 1) mut_gret(err, BIGINT_ERR_RANGE, INT64_MIN);
    int64_t res = ((int64_t)raw_u64) * x.sign;
    *err = BIGINT_SUCCESS; return res;
}
long double bigInt_tof128_safe(bigInt x, dnml_status *err) { return 0.0f; }
/* --------- Primitive Types --> BigInt --------- */
bigInt bigInt_fromu64(const uint64_t x, dnml_status *err) {
    bigInt res = {0}; if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err)
    res.limbs[0] = x; res.n = !!(x); *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_fromi64(const int64_t x, dnml_status *err) {
    bigInt res = {0};
    if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err)
    res.limbs[0] = __MAG_I64__(x);
    res.n = !!(x); res.sign = (x < 0) ? -1 : 1;
    *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_fromf128(long double x, dnml_status *err) { return (bigInt){0}; }
bigInt bigInt_fromf128_safe(long double x, dnml_status *err) { return (bigInt){0}; }




//* =========================================== BITWISE OPERATIONS =========================================== */
bigInt bigInt_not(bigInt x, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0}; if (bigInt_snew(&res, x.n) == DNML_ALLOC_OOM) func_ret_oom(err)
    for (size_t i = 0; i < x.n; ++i) {
        res.limbs[i] = ~x.limbs[i];
    } res.n = x.n; res.sign = x.sign;
    bigInt_normalize(&res); return res;
}
bigInt bigInt_rshift(bigInt x, size_t k, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    size_t limb_shift = k / U64_BITS, bshift = k % U64_BITS;
    uint64_t carry_in = 0; bigInt res = {0};
    if (bigInt_snew(&res, x.n) == DNML_ALLOC_OOM) func_ret_oom(err);
    memcpy(res.limbs, x.limbs, x.n * U64_BYTES); res.n = x.n; res.sign = x.sign;
    __BIGINT_INTERNAL_RLSHIFT__(&res, limb_shift); res.n = x.n - limb_shift;
    if (bshift) for (size_t i = res.n; i > 0; --i) {
        uint64_t next_carry = x.limbs[i - 1] & ((UINT64_C(1) << k) - 1);
        res.limbs[i - 1] = (x.limbs[i - 1] >> bshift) | (carry_in << (U64_BITS - bshift));
        carry_in = next_carry;
    } res.sign = x.sign; bigInt_normalize(&res); return res;
}
bigInt bigInt_lshift(bigInt x, size_t k, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    size_t limb_shift = k / U64_BITS, bshift = k % U64_BITS;
    uint64_t discarded_bits = 0; bigInt res = {0};
    if (bigInt_snew(&res, x.n) == DNML_ALLOC_OOM) func_ret_oom(err);
    memcpy(res.limbs, x.limbs, x.n * U64_BYTES); res.n = x.n; res.sign = x.sign;
    __BIGINT_INTERNAL_LLSHIFT__(&res, limb_shift);
    if (bshift) {
        uint64_t iso_mask = (UINT64_C(1) << bshift) - 1;
        for (size_t i = limb_shift; i < x.n; ++i) {
            uint64_t next_discarded = (x.limbs[i] >> (U64_BITS - bshift)) & iso_mask;
            res.limbs[i] = (x.limbs[i] << bshift) | discarded_bits;
            discarded_bits = next_discarded;
        } 
    } bigInt_normalize(&res); *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_lshiftg(bigInt x, size_t k, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    uint64_t discarded_bits = 0; bigInt res = {0};
    size_t limb_shift = k / U64_BITS, bshift = k % U64_BITS;
    size_t alloc_cap = (x.n + limb_shift + !!(bshift));
    if (bigInt_snew(&res, alloc_cap) == DNML_ALLOC_OOM) func_ret_oom(err);
    for (size_t i = 0; i < x.n; ++i) res.limbs[i + limb_shift] = x.limbs[i];
    res.n = alloc_cap; res.sign = x.sign;
    if (bshift) {
        uint64_t iso_mask = (UINT64_C(1) << k) - 1;
        for (size_t i = limb_shift; i < res.n; ++i) {
            uint64_t new_carry = (res.limbs[i] >> (U64_BITS - bshift)) & iso_mask;
            res.limbs[i] = (res.limbs[i] << bshift) | discarded_bits;
            discarded_bits = new_carry;
        }
    } bigInt_normalize(&res); *err = BIGINT_SUCCESS; return res;
}
dnml_status bigInt_mut_not(bigInt *const x) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    for (size_t i = 0; i < x->n; ++i) x->limbs[i] = ~(x->limbs[i]);
    bigInt_normalize(x); return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_rshift(bigInt *const x, size_t k) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    uint64_t carry_in = 0;
    size_t limb_shift = k / U64_BITS, bshift = k % U64_BITS;
    __BIGINT_INTERNAL_RLSHIFT__(x, limb_shift); x->n -= limb_shift;
    if (bshift) for (size_t i = x->n; i > 0; --i) {
        uint64_t next_carry = x->limbs[i - 1] & ((UINT64_C(1) << k) - 1);
        x->limbs[i - 1] = (x->limbs[i - 1] >> bshift) | (carry_in << (U64_BITS - bshift));
        carry_in = next_carry;
    } bigInt_normalize(x); return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_lshift(bigInt *const x, size_t k) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    size_t limb_shift = k / U64_BITS, bshift = k % U64_BITS;
    uint64_t discarded_bits = 0; __BIGINT_INTERNAL_LLSHIFT__(x, limb_shift);
    if (bshift) {
        uint64_t iso_mask = (UINT64_C(1) << bshift) - 1;
        for (size_t i = limb_shift; i < x->n; ++i) {
            uint64_t next_carry = (x->limbs[i] >> (U64_BITS - bshift)) & iso_mask;
            x->limbs[i] = (x->limbs[i] << bshift) | discarded_bits;
            discarded_bits = next_carry;
        } 
    } bigInt_normalize(x); return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_lshiftg(bigInt *const x, size_t k) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    uint64_t discarded_bits = 0;
    size_t limb_shift = k / U64_BITS, bshift = k % U64_BITS;
    size_t alloc_cap = (x->n + limb_shift + !!(bshift));
    if (bigInt_reserve(x, alloc_cap) == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    __BIGINT_INTERNAL_LLSHIFT__(x, limb_shift); x->n = alloc_cap;
    if (bshift) {
        uint64_t iso_mask = (UINT64_C(1) << k) - 1;
        for (size_t i = limb_shift; i < x->n; ++i) {
            uint64_t next_carry = (x->limbs[i] >> (U64_BITS - bshift)) & iso_mask;
            x->limbs[i] = (x->limbs[i] << bshift) | discarded_bits;
            discarded_bits = next_carry;
        }
    } bigInt_normalize(x); return BIGINT_SUCCESS;
}
/* ------------- Mutative, Fixed-width ------------- */
dnml_status bigInt_mut_andu64  (bigInt *const x, const uint64_t val) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (x->n == 0) return BIGINT_SUCCESS;
    x->limbs[0] = x->limbs[0] & val;
    x->n        = (x->limbs[0]) ? 1 : 0;
    x->sign     = (x->limbs[0]) ? x->sign : 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_nandu64 (bigInt *const x, const uint64_t val) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (x->n == 0) { x->limbs[0] = UINT64_MAX; x->n = 1; }
    else {
        x->limbs[0] = ~(x->limbs[0] & val);
        if (x->n > 1) memset(&x->limbs[1], UINT8_MAX, x->n - 1);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_oru64   (bigInt *const x, const uint64_t val) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!x->n) {
        uint64_t res = 0 | val; x->limbs[0] = res;
        x->n = !!(res); x->sign = (res) ? x->sign : 1;
    } else if (val){
        x->limbs[0] |= val; // All the other limbs stay the same due to |= 0
    }
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_noru64  (bigInt *const x, const uint64_t val) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (x->n == 0) {
        uint64_t res = ~(0 | val);
        x->limbs[0] = res;
        x->n        = (res) ? 1 : 0;
        x->sign     = (res) ? x->sign : 1;
    } else {
        for (size_t i = 0; i < x->n; ++i) {
            uint64_t b = (i == 0) ? val : 0;
            x->limbs[i] = ~(x->limbs[i] | b);
        } bigInt_normalize(x);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_xoru64  (bigInt *const x, const uint64_t val) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (x->n == 0) {
        uint64_t res = 0 ^ val;
        x->limbs[0] = res;
        x->n        = (res) ? 1 : 0;
        x->sign     = (res) ? x->sign : 1;
    } else {
        for (size_t i = 0; i < x->n; ++i) {
            uint64_t b = (i == 0) ? val : 0;
            x->limbs[i] = x->limbs[i] ^ b;
        } bigInt_normalize(x);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_xnoru64 (bigInt *const x, const uint64_t val) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (x->n == 0) {
        uint64_t res = ~(0 ^ val);
        x->limbs[0] = res;
        x->n        = (res) ? 1 : 0;
        x->sign     = (res) ? x->sign : 1;
    } else {
        for (size_t i = 0; i < x->n; ++i) {
            uint64_t b = (i == 0) ? val : 0;
            x->limbs[i] = ~(x->limbs[i] ^ b);
        } bigInt_normalize(x);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_and  (bigInt *const x, bigInt y) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    test_assert(x->limbs != y.limbs, bi_aliased_limbs, clear_arena, BIGINT_ERR_ALIASED);
    if (!y.n) bigInt_reset(x);
    else if (x->n) {
        size_t operation_range = max(x->n, y.n);
        dnml_status echeck = bigInt_reserve(x, operation_range);
        heap_alloc_oom(echeck)
        for (size_t i = 0; i < operation_range; ++i) {
            uint64_t a = (i < x->n) ? x->limbs[i] : 0;
            uint64_t b = (i < y.n)  ? y.limbs[i]  : 0;
            x->limbs[i] = a & b;
        }
        x->n = operation_range; bigInt_normalize(x);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_nand (bigInt *const x, bigInt y) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    test_assert(x->limbs != y.limbs, bi_aliased_limbs, clear_arena, BIGINT_ERR_ALIASED);
    if (!x->n) {
        size_t expanded_cap = (y.n) ? y.n : 1;
        dnml_status echeck = bigInt_reserve(x, expanded_cap);
        heap_alloc_oom(echeck); memset(x->limbs, UINT8_MAX, expanded_cap);
        x->n = expanded_cap;
    } else if (!y.n) memset(x->limbs, UINT8_MAX, x->n);
    else {
        size_t operation_range = max(x->n, y.n);
        dnml_status echeck = bigInt_reserve(x, operation_range);
        heap_alloc_oom(echeck);
        for (size_t i = 0; i < operation_range; ++i) {
            uint64_t a = (i < x->n) ? x->limbs[i] : 0;
            uint64_t b = (i < y.n)  ? y.limbs[i]  : 0;
            x->limbs[i] = ~(a & b);
        }
        x->n = operation_range; bigInt_normalize(x);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_or   (bigInt *const x, bigInt y) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    test_assert(x->limbs != y.limbs, bi_aliased_limbs, clear_arena, BIGINT_ERR_ALIASED);
    if (!y.n);
    else if (!x->n) { if (bigInt_mut_copy(x, y) == DNML_ALLOC_OOM) return DNML_ALLOC_OOM; }
    else { size_t operation_range = max(x->n, y.n);
        dnml_status echeck = bigInt_reserve(x, operation_range);
        heap_alloc_oom(echeck);
        for (size_t i = 0; i < operation_range; ++i) {
            uint64_t a = (i < x->n) ? x->limbs[i] : 0;
            uint64_t b = (i < y.n)  ? y.limbs[i]  : 0;
            x->limbs[i] = a | b;
        }
        x->n = operation_range;
        bigInt_normalize(x);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_nor  (bigInt *const x, bigInt y) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    test_assert(x->limbs != y.limbs, bi_aliased_limbs, clear_arena, BIGINT_ERR_ALIASED);
    if (!x->n && !y.n) {
        x->limbs[0] = UINT64_MAX;
        x->n        = 1;
    } else {
        size_t operation_range = max(x->n, y.n);
        dnml_status echeck = bigInt_reserve(x, operation_range);
        heap_alloc_oom(echeck);
        for (size_t i = 0; i < operation_range; ++i) {
            uint64_t a = (i < x->n) ? x->limbs[i] : 0;
            uint64_t b = (i < y.n) ? y.limbs[i] : 0;
            x->limbs[i] = ~(a | b);
        }
        x->n = operation_range;
        bigInt_normalize(x);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_xor  (bigInt *const x, bigInt y) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    test_assert(x->limbs != y.limbs, bi_aliased_limbs, clear_arena, BIGINT_ERR_ALIASED);
    if (!x->n && !y.n);
    else {
        size_t operation_range = max(x->n, y.n);
        dnml_status echeck = bigInt_reserve(x, operation_range);
        heap_alloc_oom(echeck);
        for (size_t i = 0; i < operation_range; ++i) {
            uint64_t a = (i < x->n) ? x->limbs[i] : 0;
            uint64_t b = (i < y.n)  ? y.limbs[i]  : 0;
            x->limbs[i] = a ^ b;
        }
        x->n = operation_range;
        bigInt_normalize(x);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_xnor (bigInt *const x, bigInt y) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    test_assert(x->limbs != y.limbs, bi_aliased_limbs, clear_arena, BIGINT_ERR_ALIASED);
    if (!x->n && !y.n) {
        x->limbs[0] = UINT64_MAX;
        x->n        = 1;
    } else {
        size_t operation_range = max(x->n, y.n);
        dnml_status echeck = bigInt_reserve(x, operation_range);
        heap_alloc_oom(echeck);
        for (size_t i = 0; i < operation_range; ++i) {
            uint64_t a = (i < x->n) ? x->limbs[i] : 0;
            uint64_t b = (i < y.n)  ? y.limbs[i]  : 0;
            x->limbs[i] = ~(a ^ b);
        }
        x->n = operation_range;
        bigInt_normalize(x);
    } return BIGINT_SUCCESS;
}
/* ------------- Mutative, Explicit-width ------------- */
dnml_status bigInt_mutex_andu64  (bigInt *const x, const uint64_t val, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status echeck = bigInt_reserve(x, op_range); heap_alloc_oom(echeck);
    if (!x->n) return BIGINT_SUCCESS;
    x->limbs[0] = x->limbs[0] & val;
    x->n        = (x->limbs[0]) ? 1 : 0;
    x->sign     = (x->limbs[0]) ? x->sign : 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mutex_nandu64 (bigInt *const x, const uint64_t val, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status echeck = bigInt_reserve(x, op_range); heap_alloc_oom(echeck);
    for (size_t i = 0; i < op_range; ++i) {
        uint64_t a = (i < x->n) ? x->limbs[i] : 0;
        uint64_t b = (i == 0)   ? val         : 0;
        x->limbs[i] = ~(a & b);
    } x->n = max(x->n, op_range); bigInt_normalize(x);
    x->sign = (x->n) ? x->sign : 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mutex_oru64   (bigInt *const x, const uint64_t val, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status echeck = bigInt_reserve(x, op_range); heap_alloc_oom(echeck);
    if (!val) return BIGINT_SUCCESS;
    else if (!x->n) {
        uint64_t res = 0 | val;
        x->limbs[0] = res;
        x->n        = (res) ? 1 : 0;
        x->sign     = (res) ? x->sign : 1;
    } else x->limbs[0] |= val; // All the other limbs stay the same due to |= 0
    return BIGINT_SUCCESS;
}
dnml_status bigint_mutex_noru64  (bigInt *const x, const uint64_t val, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status echeck = bigInt_reserve(x, op_range); heap_alloc_oom(echeck);
    for (size_t i = 0; i < op_range; ++i) {
        uint64_t a = (i < x->n) ? x->limbs[i] : 0;
        uint64_t b = (i == 0)   ? val         : 0;
        x->limbs[i] = ~(a | b);
    } x->n = max(x->n, op_range); bigInt_normalize(x);
    x->sign = (x->n ) ? x->sign : 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mutex_xoru64  (bigInt *const x, const uint64_t val, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!op_range) return BIGINT_SUCCESS;
    bigInt_reserve(x, op_range);
    if (x->n == 0) {
        uint64_t res = 0 ^ val;
        x->limbs[0] = res;
        x->n        = (res) ? 1 : 0;
        x->sign     = (res) ? x->sign : 1;
    } else {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x->n) ? x->limbs[i] : 0;
            uint64_t b = (i == 0)   ? val         : 0;
            x->limbs[i] = a ^ b;
        } x->n = max(x->n, op_range); bigInt_normalize(x);
        x->sign = (x->n) ? x->sign : 1;
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mutex_xnoru64 (bigInt *const x, const uint64_t val, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status echeck = bigInt_reserve(x, op_range); heap_alloc_oom(echeck);
    for (size_t i = 0; i < op_range; ++i) {
        uint64_t a = (i < x->n) ? x->limbs[i] : 0;
        uint64_t b = (i == 0)   ? val         : 0;
        x->limbs[i] = ~(a ^ b);
    } x->n = max(x->n, op_range); bigInt_normalize(x);
    x->sign = (x->n) ? x->sign : 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mutex_andi64  (bigInt *const x, const int64_t val, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status echeck = bigInt_reserve(x, op_range); heap_alloc_oom(echeck);
    if (!x->n) return BIGINT_SUCCESS;
    uint64_t extension_bits = (val < 0) ? UINT64_MAX : 0;
    for (size_t i = 0; i < op_range; ++i) {
        uint64_t a = (i < x->n) ? x->limbs[i]       : 0;
        uint64_t b = (i == 0)   ? __MAG_I64__(val)  : extension_bits;
        x->limbs[i] = a & b;
    } x->n = max(x->n, op_range); bigInt_normalize(x);
    x->sign = (x->n) ? x->sign : 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mutex_nandi64 (bigInt *const x, const int64_t val, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status echeck = bigInt_reserve(x, op_range); heap_alloc_oom(echeck);
    uint64_t extension_bits = (val < 0) ? UINT64_MAX : 0;
    for (size_t i = 0; i < op_range; ++i) {
        uint64_t a = (i < x->n) ? x->limbs[i]       : 0;
        uint64_t b = (i == 0)   ? __MAG_I64__(val)  : extension_bits;
        x->limbs[i] = ~(a & b);
    } x->n = max(x->n, op_range); bigInt_normalize(x);
    x->sign = (x->n) ? x->sign : 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mutex_ori64   (bigInt *const x, const int64_t val, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status echeck = bigInt_reserve(x, op_range); heap_alloc_oom(echeck);
    uint64_t extension_bits = (val < 0) ? UINT64_MAX : 0;
    for (size_t i = 0; i < op_range; ++i) {
        uint64_t a = (i < x->n) ? x->limbs[i]       : 0;
        uint64_t b = (i == 0)   ? __MAG_I64__(val)  : extension_bits;
        x->limbs[i] = a | b;
    } x->n = max(x->n, op_range); bigInt_normalize(x);
    x->sign = (x->n) ? x->sign : 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mutex_nori64  (bigInt *const x, const int64_t val, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status echeck = bigInt_reserve(x, op_range); heap_alloc_oom(echeck);
    uint64_t extension_bits = (val < 0) ? UINT64_MAX : 0;
    for (size_t i = 0; i < op_range; ++i) {
        uint64_t a = (i < x->n) ? x->limbs[i]       : 0;
        uint64_t b = (i == 0)   ? __MAG_I64__(val)  : extension_bits;
        x->limbs[i] = ~(a | b);
    } x->n = max(x->n, op_range); bigInt_normalize(x);
    x->sign = (x->n) ? x->sign : 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mutex_xori64  (bigInt *const x, const int64_t val, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status echeck = bigInt_reserve(x, op_range); heap_alloc_oom(echeck);
    uint64_t extension_bits = (val < 0) ? UINT64_MAX : 0;
    for (size_t i = 0; i < op_range; ++i) {
        uint64_t a = (i < x->n) ? x->limbs[i]       : 0;
        uint64_t b = (i == 0)   ? __MAG_I64__(val)  : extension_bits;
        x->limbs[i] = a ^ b;
    } x->n = max(x->n, op_range); bigInt_normalize(x);
    x->sign = (x->n) ? x->sign : 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mutex_xnori64 (bigInt *const x, const int64_t val, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status echeck = bigInt_reserve(x, op_range); heap_alloc_oom(echeck);
    uint64_t extension_bits = (val < 0) ? UINT64_MAX : 0;
    for (size_t i = 0; i < op_range; ++i) {
        uint64_t a = (i < x->n) ? x->limbs[i]       : 0;
        uint64_t b = (i == 0)   ? __MAG_I64__(val)  : extension_bits;
        x->limbs[i] = ~(a ^ b);
    } x->n = max(x->n, op_range); bigInt_normalize(x);
    x->sign = (x->n) ? x->sign : 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mutex_and   (bigInt *const x, bigInt y, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    test_assert(x->limbs != y.limbs, bi_aliased_limbs, clear_arena, BIGINT_ERR_ALIASED);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status echeck = bigInt_reserve(x, op_range); heap_alloc_oom(echeck);
    if (!x->n) return BIGINT_SUCCESS;
    else if (!y.n) bigInt_reset(x);
    else {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x->n) ? x->limbs[i] : 0;
            uint64_t b = (i < y.n) ? y.limbs[i] : 0;
            x->limbs[i] = a & b;
        } x->n = max(x->n, op_range); bigInt_normalize(x);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mutex_nand  (bigInt *const x, bigInt y, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    test_assert(x->limbs != y.limbs, bi_aliased_limbs, clear_arena, BIGINT_ERR_ALIASED);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status echeck = bigInt_reserve(x, op_range); heap_alloc_oom(echeck);
    if (!x->n || !y.n) memset(x->limbs, UINT8_MAX, op_range);
    else {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x->n) ? x->limbs[i] : 0;
            uint64_t b = (i < y.n) ? y.limbs[i] : 0;
            x->limbs[i] = ~(a & b);
        } x->n = max(x->n, op_range); bigInt_normalize(x);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mutex_or    (bigInt *const x, bigInt y, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    test_assert(x->limbs != y.limbs, bi_aliased_limbs, clear_arena, BIGINT_ERR_ALIASED);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status echeck = bigInt_reserve(x, op_range); heap_alloc_oom(echeck);
    if (!x->n) {
        size_t copy_range = (y.n < op_range) ? y.n : op_range;
        memcpy(x->limbs, y.limbs, copy_range * U64_BYTES);
        x->n = copy_range;
        bigInt_normalize(x);
    } else if (y.n) {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x->n) ? x->limbs[i] : 0;
            uint64_t b = (i < y.n) ? y.limbs[i] : 0;
            x->limbs[i] = a | b;
        } x->n = max(x->n, op_range); bigInt_normalize(x);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mutex_nor   (bigInt *const x, bigInt y, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    test_assert(x->limbs != y.limbs, bi_aliased_limbs, clear_arena, BIGINT_ERR_ALIASED);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status echeck = bigInt_reserve(x, op_range); heap_alloc_oom(echeck);
    if (!x->n && !y.n) {
        memset(x->limbs, UINT8_MAX, op_range * U64_BYTES);
        x->n = op_range;
    } else {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x->n) ? x->limbs[i] : 0;
            uint64_t b = (i < y.n) ? y.limbs[i] : 0;
            x->limbs[i] = ~(a | b);
        } x->n = max(x->n, op_range); bigInt_normalize(x);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mutex_xor   (bigInt *const x, bigInt y, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    test_assert(x->limbs != y.limbs, bi_aliased_limbs, clear_arena, BIGINT_ERR_ALIASED);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status echeck = bigInt_reserve(x, op_range); heap_alloc_oom(echeck);
    if (x->n | y.n) {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x->n) ? x->limbs[i] : 0;
            uint64_t b = (i < y.n) ? y.limbs[i] : 0;
            x->limbs[i] = a ^ b;
        } x->n = max(x->n, op_range); bigInt_normalize(x);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mutex_xnor  (bigInt *const x, bigInt y, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    test_assert(x->limbs != y.limbs, bi_aliased_limbs, clear_arena, BIGINT_ERR_ALIASED);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status echeck = bigInt_reserve(x, op_range); heap_alloc_oom(echeck);
    if (!x->n && !y.n) {
        memset(x->limbs, UINT8_MAX, op_range * U64_BYTES);
        x->n = op_range;
    } else {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x->n) ? x->limbs[i] : 0;
            uint64_t b = (i < y.n) ? y.limbs[i] : 0;
            x->limbs[i] = ~(a ^ b);
        } x->n = max(x->n, op_range); bigInt_normalize(x);
    } return BIGINT_SUCCESS;
}
/* ------------- Functional, Fixed-width ------------- */
bigInt bigInt_andu64  (bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0}; if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (x.n) {
        res.limbs[0] = x.limbs[0] & val;
        res.n        = res.limbs[0] ? 1 : 0;
        res.sign     = res.limbs[0] ? x.sign : 1;
    } return res;
}
bigInt bigInt_nandu64 (bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0};
    if (bigInt_snew(&res, (x.n) ? x.n : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (x.n == 0) { res.limbs[0] = UINT64_MAX; res.n = 1; }
    else {
        res.limbs[0] = ~(x.limbs[0] & val);
        if (x.n > 1) memset(&res.limbs[1], UINT8_MAX, x.n - 1);
        res.n = x.n;
    } return res;
}
bigInt bigInt_oru64   (bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0};
    if (!val) { if (bigInt_binew(&res, &x) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else if (x.n == 0) {
        if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err);
        res.limbs[0] = 0 | val;
        res.n        = res.limbs[0] ? 1 : 0;
        res.sign     = res.limbs[0] ? x.sign : 1;
    } else {
        if (bigInt_snew(&res, x.n) == DNML_ALLOC_OOM) func_ret_oom(err);
        for (size_t i = 0; i < x.n; ++i) {
            uint64_t b = (i == 0) ? val : 0;
            res.limbs[i] = x.limbs[i] | b;
        } bigInt_normalize(&res);
    } return res;
}
bigInt bigInt_noru64  (bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0};
    if (x.n == 0) {
        if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err);
        uint64_t op_res = ~(0 | val);
        res.limbs[0] = op_res;
        res.n        = (op_res) ? 1 : 0;
        res.sign     = (op_res) ? x.sign : 1;
    } else {
        if (bigInt_snew(&res, x.n) == DNML_ALLOC_OOM) func_ret_oom(err);
        for (size_t i = 0; i < x.n; ++i) {
            uint64_t b = (i == 0) ? val : 0;
            res.limbs[i] = ~(x.limbs[i] | b);
        } res.n = x.n; bigInt_normalize(&res);
    } return res;
}
bigInt bigInt_xoru64  (bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0};
    if (x.n == 0) {
        if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err);
        uint64_t op_res = 0 ^ val;
        res.limbs[0]    = op_res;
        res.n           = (op_res) ? 1 : 0;
        res.sign        = (op_res) ? x.sign : 1;
    } else {
        if (bigInt_snew(&res, x.n) == DNML_ALLOC_OOM) func_ret_oom(err);
        for (size_t i = 0; i < x.n; ++i) {
            uint64_t b = (i == 0) ? val : 0;
            res.limbs[i] = x.limbs[i] ^ b;
        } res.n = x.n; bigInt_normalize(&res);
    } return res;
}
bigInt bigInt_xnoru64 (bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0};
    if (x.n == 0) {
        if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err);
        uint64_t op_res = ~(0 ^ val);
        res.limbs[0] = op_res;
        res.n        = (op_res) ? 1 : 0;
        res.sign     = (op_res) ? x.sign : 1;
    } else {
        if (bigInt_snew(&res, x.n) == DNML_ALLOC_OOM) func_ret_oom(err);
        for (size_t i = 0; i < x.n; ++i) {
            uint64_t b = (i == 0) ? val : 0;
            res.limbs[i] = ~(x.limbs[i] ^ b);
        } res.n = x.n; bigInt_normalize(&res);
    } return res;
}
bigInt bigInt_and   (bigInt x, bigInt y, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    ); bigInt res = {0}; 
    if (!x.n || !y.n) {
        if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err)
    } else {
        size_t operation_range = max(x.n, y.n);
        if (bigInt_snew(&res, operation_range) == DNML_ALLOC_OOM) func_ret_oom(err);
        for (size_t i = 0; i < operation_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i] : 0;
            uint64_t b = (i < y.n) ? y.limbs[i] : 0;
            res.limbs[i] = a & b;
        } res.n = operation_range; bigInt_normalize(&res);
    } return res;
}
bigInt bigInt_nand  (bigInt x, bigInt y, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0};
    if (!x.n || !y.n) {
        size_t max = max(x.n, y.n);
        size_t expanded_cap = max ? max : 1;
        if (bigInt_snew(&res, expanded_cap) == DNML_ALLOC_OOM) func_ret_oom(err);
        memset(res.limbs, UINT8_MAX, expanded_cap);
        res.n = expanded_cap;
    } else {
        size_t operation_range = max(x.n, y.n);
        if (bigInt_snew(&res, operation_range) == DNML_ALLOC_OOM) func_ret_oom(err);
        for (size_t i = 0; i < operation_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i] : 0;
            uint64_t b = (i < y.n) ? y.limbs[i] : 0;
            res.limbs[i] = ~(a & b);
        } res.n = operation_range; bigInt_normalize(&res);
    } return res;
}
bigInt bigInt_or    (bigInt x, bigInt y, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0}; if (!y.n) { if (bigInt_binew(&res, &x) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else if (!x.n) { if (bigInt_binew(&res, &y) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else {
        size_t operation_range = max(x.n, y.n);
        if (bigInt_snew(&res, operation_range) == DNML_ALLOC_OOM) func_ret_oom(err);
        for (size_t i = 0; i < operation_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i] : 0;
            uint64_t b = (i < y.n) ? y.limbs[i] : 0;
            res.limbs[i] = a | b;
        } res.n = operation_range; bigInt_normalize(&res);
    } return res;
}
bigInt bigInt_nor   (bigInt x, bigInt y, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0};
    if (!x.n && !y.n) {
        if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err);
        res.limbs[0] = UINT64_MAX;
        res.n        = 1;
        res.sign     = x.sign;
    } else {
        size_t operation_range = max(x.n, y.n);
        if (bigInt_snew(&res, operation_range) == DNML_ALLOC_OOM) func_ret_oom(err);
        for (size_t i = 0; i < operation_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i] : 0;
            uint64_t b = (i < y.n) ? y.limbs[i] : 0;
            res.limbs[i] = ~(a | b);
        } res.n = operation_range; bigInt_normalize(&res);
    } return res;
}
bigInt bigInt_xor   (bigInt x, bigInt y, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    ); bigInt res = {0};
    if (!x.n && !y.n) {
        if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err)
    } else {
        size_t operation_range = max(x.n, y.n);
        if (bigInt_snew(&res, operation_range) == DNML_ALLOC_OOM) func_ret_oom(err);
        for (size_t i = 0; i < operation_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i] : 0;
            uint64_t b = (i < y.n) ? y.limbs[i] : 0;
            res.limbs[i] = a ^ b;
        } res.n = operation_range; bigInt_normalize(&res);
    } return res;
}
bigInt bigInt_xnor  (bigInt x, bigInt y, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0};
    if (!x.n && !y.n) {
        if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err)
        res.limbs[0] = UINT64_MAX;
        res.n        = 1;
    } else {
        size_t operation_range = max(x.n, y.n);
        if (bigInt_snew(&res, operation_range) == DNML_ALLOC_OOM) func_ret_oom(err);
        for (size_t i = 0; i < operation_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i] : 0;
            uint64_t b = (i < y.n) ? y.limbs[i] : 0;
            res.limbs[i] = ~(a ^ b);
        } res.n = operation_range;  bigInt_normalize(&res);
    } return res;
}
/* ------------- Functional, Explicit-widht ------------- */
bigInt bigInt_ex_andu64  (bigInt x, const uint64_t val, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0}; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (x.n && op_range) {
        res.limbs[0] = x.limbs[0] & val;
        res.n        = !!(res.limbs[0]);
        res.sign     = res.limbs[0] ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_nandu64 (bigInt x, const uint64_t val, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0};  if (bigInt_snew(&res, op_range) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i == 0) ? val : 0;
            uint64_t b = (i < x.n) ? x.limbs[i] : 0;
            res.limbs[i] = ~(a & b);
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_oru64   (bigInt x, const uint64_t val, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0}; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n)  ? x.limbs[i] : 0;
            uint64_t b = (i == 0)   ? val : 0;
            res.limbs[i] = a | b;
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_noru64  (bigInt x, const uint64_t val, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0}; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n)  ? x.limbs[i] : 0;
            uint64_t b = (i == 0)   ? val : 0;
            res.limbs[i] = ~(a | b);
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_xoru64  (bigInt x, const uint64_t val, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0}; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n)  ? x.limbs[i] : 0;
            uint64_t b = (i == 0)   ? val : 0;
            res.limbs[i] = a ^ b;
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_xnoru64 (bigInt x, const uint64_t val, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0}; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n)  ? x.limbs[i] : 0;
            uint64_t b = (i == 0)   ? val : 0;
            res.limbs[i] = ~(a ^ b);
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_andi64  (bigInt x, const int64_t val, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0}; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range && x.n) {
        uint64_t extension_bits = (val < 0) ? UINT64_MAX : 0;
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i]       : 0;
            uint64_t b = (i == 0)  ? __MAG_I64__(val) : extension_bits;
            res.limbs[i] = a & b;
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_nandi64 (bigInt x, const int64_t val, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0}; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        uint64_t extension_bits = (val < 0) ? UINT64_MAX : 0;
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i]       : 0;
            uint64_t b = (i == 0)  ? __MAG_I64__(val) : extension_bits;
            res.limbs[i] = ~(a & b);
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_ori64   (bigInt x, const int64_t val, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0}; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        uint64_t extension_bits = (val < 0) ? UINT64_MAX : 0;
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i]       : 0;
            uint64_t b = (i == 0)  ? __MAG_I64__(val) : extension_bits;
            res.limbs[i] = a | b;
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_nori64  (bigInt x, const int64_t val, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0}; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        uint64_t extension_bits = (val < 0) ? UINT64_MAX : 0;
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i]       : 0;
            uint64_t b = (i == 0)  ? __MAG_I64__(val) : extension_bits;
            res.limbs[i] = ~(a | b);
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_xori64  (bigInt x, const int64_t val, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0}; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        uint64_t extension_bits = (val < 0) ? UINT64_MAX : 0;
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i]       : 0;
            uint64_t b = (i == 0)  ? __MAG_I64__(val) : extension_bits;
            res.limbs[i] = a ^ b;
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_xnori64 (bigInt x, const int64_t val, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0}; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        uint64_t extension_bits = (val < 0) ? UINT64_MAX : 0;
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i]       : 0;
            uint64_t b = (i == 0)  ? __MAG_I64__(val) : extension_bits;
            res.limbs[i] = ~(a ^ b);
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_and   (bigInt x, bigInt y, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0}; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i] : 0;
            uint64_t b = (i < y.n) ? y.limbs[i] : 0;
            res.limbs[i] = a & b;
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_nand  (bigInt x, bigInt y, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0}; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i] : 0;
            uint64_t b = (i < y.n) ? y.limbs[i] : 0;
            res.limbs[i] = ~(a & b);
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_or    (bigInt x, bigInt y, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0}; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i] : 0;
            uint64_t b = (i < y.n) ? y.limbs[i] : 0;
            res.limbs[i] = a | b;
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_nor   (bigInt x, bigInt y, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0}; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i] : 0;
            uint64_t b = (i < y.n) ? y.limbs[i] : 0;
            res.limbs[i] = ~(a | b);
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_xor   (bigInt x, bigInt y, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0}; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i] : 0;
            uint64_t b = (i < y.n) ? y.limbs[i] : 0;
            res.limbs[i] = a ^ b;
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_xnor  (bigInt x, bigInt y, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0}; if (bigInt_snew(&res, (op_range) ? op_range : 1)  == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i] : 0;
            uint64_t b = (i < y.n) ? y.limbs[i] : 0;
            res.limbs[i] = ~(a ^ b);
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}





//* =============================================== COMPARISONS ============================================== */
static int8_t __BIGINT_MAGCOMP_UI64__(bigInt *const x, const uint64_t val) {
    DNML_TEST_ASSERT(x != NULL, input_null, clear_arena);
    if (x->n > 1) return 1;
    if (x->limbs[0] > val) return 1;
    else if (x->limbs[0] < val) return -1;
    return 0;
}
static int8_t __BIGINT_MAGCOMP__(bigInt *const a, bigInt *const b) {
    test_assert(a != NULL && b != NULL, input_null, clear_arena, BIGINT_NULL);
    if (a->n != b->n) return (a->n > b->n) ? 1 : -1;
    // Loops from most-significant digit down to least-significant digit
    for (size_t i = a->n; i  > 0; --i) {
        if (a->limbs[i - 1] != b->limbs[i - 1]) return (a->limbs[i - 1] > b->limbs[i - 1]) ? 1 : -1;
        // Compare which one current most-significant digit is bigger
    }
    return 0;
}
/* --------------- Integer - I64 --------------- */
bool bigInt_equal_i64(bigInt x, const int64_t val, dnml_status *err) {
    test_assert_mut(bigInt_validate(x), bi_full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (x.n == 0) return (val) ? false : true;
    int8_t val_sign = (val < 0) ? -1 : 1;
    if (val_sign != x.sign) return false;
    if (x.n > 1) return false;
    return x.limbs[0] == __MAG_I64__(val);
}
bool bigInt_less_i64(bigInt x, const int64_t val, dnml_status *err) {
    test_assert_mut(bigInt_validate(x), bi_full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (x.n == 0) return (val > 0) ? true : false;
    int8_t val_sign = (val < 0) ? -1 : 1;
    if (val_sign != x.sign) return (x.sign < val_sign);
    if (x.n > 1) return (x.sign == -1);
    if (x.limbs[0] > __MAG_I64__(val)) return (x.sign == -1);
    return (x.limbs[0] < __MAG_I64__(val)) && (x.sign == 1);
}
bool bigInt_more_i64(bigInt x, const int64_t val, dnml_status *err) {
    test_assert_mut(bigInt_validate(x), bi_full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (x.n == 0) return (val < 0) ? true : false;
    int8_t val_sign = (val < 0) ? -1 : 1;
    if (val_sign != x.sign) return (x.sign > val_sign);
    if (x.n > 1) return (x.sign >= val_sign);
    if (x.limbs[0] < __MAG_I64__(val)) return (x.sign == -1);
    return (x.limbs[0] > __MAG_I64__(val)) && (x.sign == 1);
}
bool bigInt_lequal_i64(bigInt x, const int64_t val, dnml_status *err) {
    test_assert_mut(bigInt_validate(x), bi_full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (x.n == 0) return (val >= 0) ? true : false;
    int8_t val_sign = (val < 0) ? -1 : 1;
    if (x.sign != val_sign) return (x.sign < val_sign);
    if (x.n > 1) return (x.sign == -1);
    // Case eg: 189 > 171  |  -189 < -171
    if (x.limbs[0] > __MAG_I64__(val)) return (x.sign == -1);
    return (x.sign == 1); // Case eg: 178 < 181  |   -178 > -181
}
bool bigInt_mequal_i64(bigInt x, const int64_t val, dnml_status *err) {
    test_assert_mut(bigInt_validate(x), bi_full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (x.n == 0) return (val <= 0) ? true : false;
    int8_t val_sign = (val < 0) ? -1 : 1;
    if (x.sign != val_sign) return (x.sign > val_sign);
    if (x.n > 1) return (x.sign == 1);
    // Case eg: 189 > 171  |  -189 < -171
    if (x.limbs[0] > __MAG_I64__(val)) return (x.sign == 1);
    return (x.sign == -1); // Case eg: 178 < 181  |   -178 > -181
}
/* ---------- Unsigned Integer - UI64 ---------- */
bool bigInt_equal_u64(bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(bigInt_validate(x), bi_full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (x.n == 0) return (val) ? false : true;
    if (x.sign == -1) return false;
    if (x.n > 1) return false;
    return (x.limbs[0] == val);
}
bool bigInt_less_u64(bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(bigInt_validate(x), bi_full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (x.n == 0) return (val) ? true : false;
    if (x.sign == -1) return true;
    if (x.n > 1) return false;
    return (x.limbs[0] < val);
}
bool bigInt_more_u64(bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(bigInt_validate(x), bi_full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (x.n == 0) return (val) ? false : true;
    if (x.sign == -1) return false;
    if (x.n > 1) return true;
    return (x.limbs[0] > val);
}
bool bigInt_lequal_u64(bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(bigInt_validate(x), bi_full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (x.n == 0) return true; // Always true, as every R+ numbers are always >= 0
    if (x.sign == -1) return true;
    if (x.n > 1) return false;
    return (x.limbs[0] <= val);
}
bool bigInt_mequal_u64(bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(bigInt_validate(x), bi_full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (x.n == 0) return (val) ? false : true;
    if (x.sign == -1) return false;
    if (x.n > 1) return true;
    return (x.limbs[0] >= val);
}
/* ------------------- BigInt ------------------ */
bool bigInt_equal(bigInt a, bigInt b, dnml_status *err) {
    test_assert_mut(bigInt_validate(a) && bigInt_validate(b), bi_full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (!a.n) return (!b.n) ? true : false;
    if (a.sign != b.sign) return false;
    if (a.n != b.n) return false;
    if (a.limbs == b.limbs) return true;
    return __BIGINT_MAGCOMP__(&a, &b) == 0;
}
bool bigInt_less(bigInt a, bigInt b, dnml_status *err) {
    test_assert_mut(bigInt_validate(a) && bigInt_validate(b), bi_full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (a.sign != b.sign) return (a.sign < b.sign);
    if (a.n != b.n) return (a.sign == 1) ? (a.n < b.n) : (a.n > b.n);
    if (a.limbs == b.limbs) return false;
    return (a.sign == 1) ? __BIGINT_MAGCOMP__(&a, &b) < 0 : __BIGINT_MAGCOMP__(&a, &b) > 0;
}
bool bigInt_more(bigInt a, bigInt b, dnml_status *err) {
    test_assert_mut(bigInt_validate(a) && bigInt_validate(b), bi_full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (a.sign != b.sign) return (a.sign > b.sign);
    if (a.n    != b.n)    return (a.sign == 1) ? (a.n > b.n) : (a.n < b.n);
    if (a.limbs == b.limbs) return false;
    return (a.sign == 1) ? __BIGINT_MAGCOMP__(&a, &b) > 0 : __BIGINT_MAGCOMP__(&a, &b) < 0;
}
bool bigInt_lequal(bigInt a, bigInt b, dnml_status *err) {
    test_assert_mut(bigInt_validate(a) && bigInt_validate(b), bi_full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (a.sign != b.sign) return (a.sign < b.sign);
    if (a.n != b.n) return (a.sign == 1) ? (a.n < b.n) : (a.n > b.n);
    if (a.limbs == b.limbs) return true;
    return (a.sign == 1) ? __BIGINT_MAGCOMP__(&a, &b) <= 0 : __BIGINT_MAGCOMP__(&a, &b) >= 0;
}
bool bigInt_mequal(bigInt a, bigInt b, dnml_status *err) {
    test_assert_mut(bigInt_validate(a) && bigInt_validate(b), bi_full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (a.sign != b.sign) return (a.sign > b.sign);
    if (a.n != b.n) return (a.sign == 1) ? (a.n > b.n) : (a.n < b.n);
    if (a.limbs == b.limbs) return true;
    return (a.sign == 1) ? __BIGINT_MAGCOMP__(&a, &b) >= 0 : __BIGINT_MAGCOMP__(&a, &b) <= 0;
}





//* ========================================= MAGNITUDE MATHEMATICA ========================================== *//
/* -------------------- MAGNITUDED ARITHMETIC --------------------- */
static void __BIGINT_MAGADD__(bigInt *const res, bigInt *const a, bigInt *const b) {
    size_t max = max(a->n, b->n);
    // Set the minimum capacity of res to be 1 bigger
    // than the largest capacity between a & b ----> res->cap = max + 1
    uint64_t carry = 0;
    for (size_t i = 0; i < max; ++i) {
        uint64_t x = (i < a->n) ? a->limbs[i] : 0; // Assigning limb at position i of a to x
        uint64_t y = (i < b->n) ? b->limbs[i] : 0; // Assigning limb at position i of b to x
        uint8_t u8_carry = (uint8_t)carry;
        res->limbs[i] = __ADD_UI64__(x, y, &u8_carry); // Do single-limb addition with carry (if have) --> Stores the carry
    }
    if (carry) res->limbs[max] = carry; // If carry still needed ---> stores the carry in the (res->cap - 1) limb
    res->n = max + (carry != 0);
}
static void __BIGINT_MAGSUB__(bigInt *const res, bigInt *const a, bigInt *const b) {
    uint64_t borrow = 0;
    for (size_t i = 0; i < a->n; ++i) {
        uint64_t y = (i < b->n) ? b->limbs[i] : 0;
        uint8_t u8_borrow = (uint8_t)borrow;
        res->limbs[i] = __SUB_UI64__(a->limbs[i], y, &u8_borrow);
        // Do single-limb subtraction with borrow ---> Stores the borrow
    } res->n = a->n;
}
static void __BIGINT_MAGMUL__(bigInt *const res, bigInt *const a, bigInt *const b, dnml_status *err) {
    dnml_arena *_DASI_MAGMUL_ARENA = _USE_ARENA();
    test_assert_mut((!(_DASI_MAGMUL_ARENA->poisoined)), alloc_oom, clear_arena, err, DNML_ALLOC_OOM,)
    size_t needed_size = __BIGINT_MUL_WS__(a->n, b->n);
    if (_DASI_MAGMUL_ARENA->cap < needed_size) {
        dnml_status echeck = arena_grow(_DASI_MAGMUL_ARENA, needed_size);
        test_assert_mut((echeck != DNML_ALLOC_OOM), alloc_oom, clear_arena, err, DNML_ALLOC_OOM,);
    }
    calc_ctx magmul_ctx = {
        .alloc = &arena_alloc_adapter, .mark = &arena_mark_adapter,
        .rewind = &arena_rewind_adapter, .clear = &arena_clear_adapter,
        .destruct = &arena_destruct_adapter, .state = _DASI_MAGMUL_ARENA
    }; dnml_status echeck = BIGINT_SUCCESS;
    __BIGINT_MUL_DISPATCH__(a, b, res, magmul_ctx, &echeck); darena_assert(); *err = echeck; *err = echeck;
}
static void __BIGINT_MAGDIV__(bigInt *const quot, bigInt *const tmp_rem, bigInt *const a, bigInt *const b, dnml_status *err) {
    dnml_arena *_DASI_MAGDIV_ARENA = _USE_ARENA();
    test_assert_mut((!(_DASI_MAGDIV_ARENA->poisoined)), alloc_oom, clear_arena, err, DNML_ALLOC_OOM,)
    size_t needed_size = __BIGINT_DIV_WS__(a->n, b->n) + b->n;
    if (_DASI_MAGDIV_ARENA->cap < needed_size) {
        dnml_status echeck = arena_grow(_DASI_MAGDIV_ARENA, needed_size);
        test_assert_mut((echeck != DNML_ALLOC_OOM), alloc_oom, clear_arena, err, DNML_ALLOC_OOM,)
    }
    calc_ctx magdivmod_ctx = {
        .alloc = &arena_alloc_adapter, .mark = &arena_mark_adapter,
        .rewind = &arena_rewind_adapter, .clear = &arena_clear_adapter,
        .destruct = &arena_destruct_adapter, .state = _DASI_MAGDIV_ARENA
    }; dnml_status echeck = BIGINT_SUCCESS;
    __BIGINT_DIV_DISP__(a, b, quot, tmp_rem, magdivmod_ctx, &echeck);
    DNML_TEST_ASSERT(echeck != BIGINT_ERR_RANGE, "128-bit Limb Division Overflowed (-Ediv_overflow)", clear_arena);
    DNML_TEST_ASSERT(echeck == BIGINT_SUCCESS, "Workspace Size Estimation Incorrect (-Earena_overflow)", clear_arena);
    *err = echeck;
}
static void __BIGINT_MAGMOD__(bigInt *const rem, bigInt *const tmp_quot, bigInt *const a, bigInt *const b, dnml_status *err) {
    dnml_arena *_DASI_MAGDIV_ARENA = _USE_ARENA();
    test_assert_mut((!(_DASI_MAGDIV_ARENA->poisoined)), alloc_oom, clear_arena, err, DNML_ALLOC_OOM,)
    size_t needed_size = __BIGINT_MOD_WS__(a->n, b->n) + a->n;
    if (_DASI_MAGDIV_ARENA->cap < needed_size) {
        dnml_status echeck = arena_grow(_DASI_MAGDIV_ARENA, needed_size);
        test_assert_mut((echeck != DNML_ALLOC_OOM), alloc_oom, clear_arena, err, DNML_ALLOC_OOM,)
    }
    calc_ctx magdivmod_ctx = {
        .alloc = &arena_alloc_adapter, .mark = &arena_mark_adapter,
        .rewind = &arena_rewind_adapter, .clear = &arena_clear_adapter,
        .destruct = &arena_destruct_adapter, .state = _DASI_MAGDIV_ARENA
    }; dnml_status echeck = BIGINT_SUCCESS;
    __BIGINT_MOD_DISPATCH__(a, b, rem, tmp_quot, magdivmod_ctx, &echeck);
    DNML_TEST_ASSERT(echeck != BIGINT_ERR_RANGE, "128-bit Limb Division Overflowed (-Ediv_overflow)", clear_arena);
    DNML_TEST_ASSERT(echeck == BIGINT_SUCCESS, "Workspace Size Estimation Incorrect (-Earena_overflow)", clear_arena);
}
static void __BIGINT_MAGMUL_U64__(bigInt *const res, bigInt *const x, const uint64_t val) {
    // Since the divisor size is small (n <= 1), we implement schoolbook multiplication
    uint64_t carry = 0;
    for (size_t i = 0; i < x->n; ++i) {
        uint64_t low, high;
        low = __MUL_UI64__(x->limbs[i], val, &high);
        uint64_t sum = low + carry;
        carry = high + (sum < low) + (sum < carry);
        res->limbs[i] = sum;
    } res->n = x->n;
    if (carry) { res->limbs[res->n++] = carry; }
}
static void __BIGINT_MAGDIVMOD_U64__(bigInt *const quot, uint64_t* rem, bigInt *const x, const uint64_t val) {
    // Since the divisior size is small (n <= 1), we implement normal/long division
    quot->n = x->n; uint64_t remainder = 0; uint8_t ovf_test;
    for (size_t i = x->n; i > 0; --i) {
        quot->limbs[i - 1] = __DIV_HELPER_UI64__(remainder, x->limbs[i - 1], val, &remainder, &ovf_test);
        DNML_TEST_ASSERT(ovf_test, "CRITICIAL DEBUG ERROR: Division quotient's overflowed", clear_arena);
    } *rem = remainder; bigInt_normalize(quot);
}
/* --------------- MAGNITUDED CORE NUMBER-THEORETIC ---------------- */
static inline uint64_t ___GCD_UI64___(const uint64_t a, const uint64_t b) { return __BIGINT_EUCLID__(a, b); }
static void __BIGINT_MAGGCD__(bigInt *const res, bigInt *const a, bigInt *const b, dnml_status *err) {
    dnml_arena *_DASI_MAGGCD_ARENA = _USE_ARENA();
    test_assert_mut((!(_DASI_MAGGCD_ARENA->poisoined)), alloc_oom, clear_arena, err, DNML_ALLOC_OOM,)
    size_t needed = __BIGINT_GCD_WS__(a->n, b->n);
    if (_DASI_MAGGCD_ARENA->cap < needed) {
        dnml_status echeck = arena_grow(_DASI_MAGGCD_ARENA, needed);
        test_assert_mut((echeck != DNML_ALLOC_OOM), alloc_oom, clear_arena, err, DNML_ALLOC_OOM,)
    }
    calc_ctx _maggcd_ctx = {
        .alloc = &arena_alloc_adapter, .mark = &arena_mark_adapter,
        .rewind = &arena_rewind_adapter, .clear = &arena_clear_adapter,
        .destruct = &arena_destruct_adapter, .state = _DASI_MAGGCD_ARENA
    }; dnml_status echeck = BIGINT_SUCCESS; 
    __BIGINT_GCD_DISPATCH__(res, a, b, _maggcd_ctx, &echeck); darena_assert(); *err = echeck;
}
static void __BIGINT_MAGLCM__(bigInt *const res, bigInt *const a, bigInt *const b, dnml_status *err) {
    dnml_arena *_DASI_MAGLCM_ARENA = _USE_ARENA();
    test_assert_mut((!(_DASI_MAGLCM_ARENA->poisoined)), alloc_oom, clear_arena, err, DNML_ALLOC_OOM,)
    size_t low_needed = (
        /* THESE CALCULATIONS ARE MOST CERTAINLY THE UPPERBOUND */
        __BIGINT_GCD_WS__(a->n, b->n) + (min(a->n, b->n) << 1) +
        __BIGINT_DIV_WS__(a->n, min(a->n, b->n)) +
        __BIGINT_MUL_WS__(a->n, b->n)
    );
    if (_DASI_MAGLCM_ARENA->cap < low_needed) {
        dnml_status echeck = arena_grow(_DASI_MAGLCM_ARENA, __BIGINT_GCD_WS__(a->n, b->n));
        test_assert_mut((echeck != DNML_ALLOC_OOM), alloc_oom, clear_arena, err, DNML_ALLOC_OOM,)
    }
    calc_ctx _maglcm_ctx = {
        .alloc = &arena_alloc_adapter, .mark = &arena_mark_adapter,
        .rewind = &arena_rewind_adapter, .clear = &arena_clear_adapter,
        .destruct = &arena_destruct_adapter, .state = _DASI_MAGLCM_ARENA
    }; dnml_status echeck = BIGINT_SUCCESS;
    size_t maglcm_mark = arena_mark(_DASI_MAGLCM_ARENA); dnml_status tmp_check;
    limb_t *gcdres_limbs = arena_alloc(_DASI_MAGLCM_ARENA, min(a->n, b->n), &tmp_check);
    limb_t *tmp_limbs = arena_alloc(_DASI_MAGLCM_ARENA, min(a->n, b->n), &tmp_check);
    limb_t *tmpq_limbs = arena_alloc(_DASI_MAGLCM_ARENA, a->n, &tmp_check);
    bigInt gcd_res = { .limbs = gcdres_limbs, /**/ .n = 0, /**/ .cap = min(a->n, b->n), .sign = 1 };
    bigInt tmp_rem = { .limbs = tmp_limbs, /**/ .n = 0, /**/ .cap = min(a->n, b->n), .sign = 1 };
    bigInt tmp_quot = { .limbs = tmpq_limbs, /**/ .n = 0, /**/ .cap = a->n, .sign = 1 };
    __BIGINT_GCD_DISPATCH__(&gcd_res, a, b, _maglcm_ctx, &echeck); darena_assert(); *err = echeck;
    __BIGINT_DIV_DISP__(a, &gcd_res, &tmp_quot, &tmp_rem, _maglcm_ctx, &echeck); darena_assert(); *err = echeck;
    __BIGINT_MUL_DISPATCH__(&tmp_quot, b, &gcd_res, _maglcm_ctx, &echeck); darena_assert(); *err = echeck;
    tmp_check = bigInt_mut_ocopy(res, gcd_res); ocopy_check(tmp_check, _DASI_MAGLCM_ARENA);
    arena_rewind(_DASI_MAGLCM_ARENA, maglcm_mark);
}
static void __BIGINT_MAGEMOD_U64__(uint64_t* res, bigInt *const a, const uint64_t mod) {
    uint64_t curr_rem = 0; uint8_t ovf_test;
    for (size_t i = a->n; i > 0; --i) {
        uint64_t tmp_quot = __DIV_HELPER_UI64__(a->limbs[i - 1], curr_rem, mod, &curr_rem, &ovf_test);
        DNML_TEST_ASSERT(ovf_test, "CRITICIAL DEBUG ERROR: Division quotient's overflowed", clear_arena);
    } *res = curr_rem;
}
static void __BIGINT_MAGEMOD__(bigInt *const res, bigInt *const a, bigInt *const mod, dnml_status *err) {
    dnml_arena *_DASI_MAGEMOD_ARENA = _USE_ARENA();
    test_assert_mut((!(_DASI_MAGEMOD_ARENA->poisoined)), alloc_oom, clear_arena, err, DNML_ALLOC_OOM,)
    size_t needed = __BIGINT_MOD_WS__(a->n, mod->n) + a->n;
    if (_DASI_MAGEMOD_ARENA->cap < needed) {
        dnml_status echeck = arena_grow(_DASI_MAGEMOD_ARENA, needed);
        test_assert_mut((echeck != DNML_ALLOC_OOM), alloc_oom, clear_arena, err, DNML_ALLOC_OOM,)
    }
    calc_ctx magemod_ctx = {
        .alloc = &arena_alloc_adapter, .mark = &arena_mark_adapter,
        .rewind = &arena_rewind_adapter, .clear = &arena_clear_adapter,
        .destruct = &arena_destruct_adapter, .state = _DASI_MAGEMOD_ARENA
    }; dnml_status echeck;
    size_t tmp_mark = arena_mark(_DASI_MAGEMOD_ARENA);
    limb_t *tmp_limbs = arena_alloc(_DASI_MAGEMOD_ARENA, a->n, &echeck);
    bigInt tmp_quot = { .limbs = tmp_limbs, /**/ .n = 0, /**/ .cap = a->n, /**/ .sign = 1 };
    __BIGINT_MOD_DISPATCH__(a, mod, res, &tmp_quot, magemod_ctx, &echeck); darena_assert(); *err = echeck;
    arena_rewind(_DASI_MAGEMOD_ARENA, tmp_mark); tmp_limbs == NULL;
}
/* ----------------- MAGNITUDED MODULAR-ARITHMETIC ------------------ */
static void __BIGINT_MAGMODADD__(bigInt *const res, bigInt *const a, bigInt *const b, bigInt *const mod) {}
static void __BIGINT_MAGMODSUB__(bigInt *const res, bigInt *const a, bigInt *const b, bigInt *const mod) {}
static void __BIGINT_MAGMODMUL__(bigInt *const res, bigInt *const a, bigInt *const b, bigInt *const mod) {}
static void __BIGINT_MAGMODDIV__(bigInt *const res, bigInt *const a, bigInt *const b, bigInt *const mod) {}
static void __BIGINT_MAGMODEXP__(bigInt *const res, bigInt *const a, bigInt *const b, bigInt *const mod) {}
static void __BIGINT_MAGMODSQR__(bigInt *const res, bigInt *const a, bigInt *const b, bigInt *const mod) {}
static void __BIGINT_MAGMODINV__(bigInt *const res, bigInt *const a, bigInt *const b, bigInt *const mod) {}
/* ----------------- MAGNITUDED ALGEBRAIC OPERATIONS ------------------ */
static void __BIGINT_MAGSQR__(bigInt *const res, bigInt *const base, dnml_status *err) {
    dnml_arena *_DASI_MAGSQR_ARENA = _USE_ARENA();
    test_assert_mut((!(_DASI_MAGSQR_ARENA->poisoined)), alloc_oom, clear_arena, err, DNML_ALLOC_OOM,)
    size_t needed = __BIGINT_MUL_WS__(base->n, base->n);
    if (_DASI_MAGSQR_ARENA->cap < needed) {
        dnml_status echeck = arena_grow(_DASI_MAGSQR_ARENA, needed);
        test_assert_mut((echeck != DNML_ALLOC_OOM), alloc_oom, clear_arena, err, DNML_ALLOC_OOM,)
    }
    calc_ctx magsqr_ctx = {
        .alloc = &arena_alloc_adapter, .mark = &arena_mark_adapter,
        .rewind = &arena_rewind_adapter, .clear = &arena_clear_adapter,
        .destruct = &arena_destruct_adapter, .state  = _DASI_MAGSQR_ARENA
    }; dnml_status echeck = BIGINT_SUCCESS;
    __BIGINT_MUL_DISPATCH__(base, base, res, magsqr_ctx, &echeck); darena_assert(); *err = echeck;
}
static void __BIGINT_MAGPOW__(bigInt *const res, bigInt *const base, const uint64_t pow, dnml_status *err) {
    dnml_arena *_DASI_MAGPOW_ARENA = _USE_ARENA();
    test_assert_mut((!(_DASI_MAGPOW_ARENA->poisoined)), alloc_oom, clear_arena, err, DNML_ALLOC_OOM,)
    size_t needed = __BIGINT_EXP_WS__(base->n, pow);
    if (_DASI_MAGPOW_ARENA->cap < needed) {
        dnml_status echeck = arena_grow(_DASI_MAGPOW_ARENA, needed);
        test_assert_mut((echeck != DNML_ALLOC_OOM), alloc_oom, clear_arena, err, DNML_ALLOC_OOM,)
    }
    calc_ctx magpow_ctx = {
        .alloc = &arena_alloc_adapter, .mark = &arena_mark_adapter,
        .rewind = &arena_rewind_adapter, .clear = &arena_clear_adapter,
        .destruct = &arena_destruct_adapter, .state  = _DASI_MAGPOW_ARENA
    }; dnml_status echeck = BIGINT_SUCCESS;
    __BIGINT_EXP_DISPATCH__(res, base, pow, magpow_ctx, &echeck); darena_assert(); *err = echeck;
}
static void __BIGINT_MAGSQRT__(bigInt *const res, bigInt *const a, dnml_status *err) {
    dnml_arena *_DASI_MAGSQRT_ARENA = _USE_ARENA();
    test_assert_mut((!(_DASI_MAGSQRT_ARENA->poisoined)), alloc_oom, clear_arena, err, DNML_ALLOC_OOM,)
    size_t needed = __BIGINT_SQRT_WS__(a->n);
    if (_DASI_MAGSQRT_ARENA->cap < needed) {
        dnml_status echeck = arena_grow(_DASI_MAGSQRT_ARENA, needed);
        test_assert_mut((echeck != DNML_ALLOC_OOM), alloc_oom, clear_arena, err, DNML_ALLOC_OOM,)
    }
    calc_ctx magsqrt_ctx = {
        .alloc = &arena_alloc_adapter, .mark = &arena_mark_adapter,
        .rewind = &arena_rewind_adapter, .clear = &arena_clear_adapter,
        .destruct = &arena_destruct_adapter, .state  = _DASI_MAGSQRT_ARENA
    }; dnml_status echeck = BIGINT_SUCCESS;
    __BIGINT_SQRT_DISPATCH__(res, a, magsqrt_ctx, &echeck); darena_assert(); *err = echeck;
}
static void __BIGINT_MAGCBRT__(bigInt *const res, bigInt *const a, dnml_status *err) {
    dnml_arena *_DASI_MAGCBRT_ARENA = _USE_ARENA();
    test_assert_mut((!(_DASI_MAGCBRT_ARENA->poisoined)), alloc_oom, clear_arena, err, DNML_ALLOC_OOM,)
    size_t needed = __BIGINT_CBRT_WS__(a->n);
    if (_DASI_MAGCBRT_ARENA->cap < needed) {
        dnml_status echeck = arena_grow(_DASI_MAGCBRT_ARENA, needed);
        test_assert_mut((echeck != DNML_ALLOC_OOM), alloc_oom, clear_arena, err, DNML_ALLOC_OOM,)
    }
    calc_ctx magcbrt_ctx = {
        .alloc = &arena_alloc_adapter, .mark = &arena_mark_adapter,
        .rewind = &arena_rewind_adapter, .clear = &arena_clear_adapter,
        .destruct = &arena_destruct_adapter, .state  = _DASI_MAGCBRT_ARENA
    }; dnml_status echeck = BIGINT_SUCCESS;
    __BIGINT_CBRT_DISPATCH__(res, a, magcbrt_ctx, &echeck); darena_assert(); *err = echeck;
}
static void __BIGINT_MAGNRT__(bigInt *const res, bigInt *const a, const uint64_t root, dnml_status *err) {
    dnml_arena *_DASI_MAG_NROOT_ARENA = _USE_ARENA();
    test_assert_mut((!(_DASI_MAG_NROOT_ARENA->poisoined)), alloc_oom, clear_arena, err, DNML_ALLOC_OOM,)
    size_t needed = __BIGINT_NRT_WS__(a->n, root);
    if (_DASI_MAG_NROOT_ARENA->cap < needed) {
        dnml_status echeck = arena_grow(_DASI_MAG_NROOT_ARENA, needed);
        test_assert_mut((echeck != DNML_ALLOC_OOM), alloc_oom, clear_arena, err, DNML_ALLOC_OOM,)
    }
    calc_ctx mag_nroot_ctx = {
        .alloc = &arena_alloc_adapter, .mark = &arena_mark_adapter,
        .rewind = &arena_rewind_adapter, .clear = &arena_clear_adapter,
        .destruct = &arena_destruct_adapter, .state  = _DASI_MAG_NROOT_ARENA
    }; dnml_status echeck = BIGINT_SUCCESS;
    __BIGINT_NRT_DISPATCH__(res, a, root, mag_nroot_ctx, &echeck); darena_assert(); *err = echeck;
}





//* ============================================ SIGNED ARITHMETIC ========================================== */
/* ------------------- MUTATIVE ARITHMETIC -------------------- */
dnml_status bigInt_mut_mulu64(bigInt *const x, const uint64_t val) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (x->n == 0);
    else if (val == 1);
    else if (!val) bigInt_reset(x);
    else if (x->n == 1 && x->limbs[0] == 1) bigInt_mut_copyu64(x, val);
    else { dnml_status echeck = bigInt_reserve(x, x->n + 1); heap_alloc_oom(echeck); __BIGINT_INTERNAL_MUL_UI64__(x, val); }
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_divu64(bigInt *const x, const uint64_t val) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!val) return BIGINT_ERR_DOMAIN;
    if (x->n == 1 && x->limbs[0] == 1) bigInt_reset(x);
    else if (x->n && val != 1) { __BIGINT_INTERNAL_DIVMOD_UI64__(x, val); bigInt_normalize(x); }
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_modu64(bigInt *const x, const uint64_t val) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!val) return BIGINT_ERR_DOMAIN;
    if (x->n == 0);
    else if (val == 1) bigInt_reset(x);
    else {
        int8_t comp_res = __BIGINT_MAGCOMP_UI64__(x, val);
        if (comp_res < 0);
        else if (!comp_res) bigInt_reset(x);
        else { uint64_t rem = __BIGINT_INTERNAL_DIVMOD_UI64__(x, val); x->limbs[0] = rem; x->n = !!(rem); }
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_muli64(bigInt *const x, const int64_t val) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (x->n == 0) return BIGINT_SUCCESS;
    if (!val) bigInt_reset(x);
    else if (val == 1 || val == -1);
    else if (x->n == 1 && x->limbs[0] == 1) bigInt_mut_copyi64(x, val);
    else { 
        dnml_status echeck = bigInt_reserve(x, x->n + 1); heap_alloc_oom(echeck);
        __BIGINT_INTERNAL_MUL_UI64__(x, __MAG_I64__(val));
    } x->sign *= (val < 0) ? -1 : 1; return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_divi64(bigInt *const x, const int64_t val) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!val) return BIGINT_ERR_DOMAIN;
    if (val == 1 || val == -1) x->sign *= val;
    else if (x->n == 1 && x->limbs[0] == 1) bigInt_reset(x);
    else if (x->n) { __BIGINT_INTERNAL_DIVMOD_UI64__(x, __MAG_I64__(val)); x->sign *= (val < 0) ? -1 : 1; bigInt_normalize(x); }
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_modi64(bigInt *const x, const int64_t val) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!val) return BIGINT_ERR_DOMAIN;
    if (val == 1 || val == -1) bigInt_reset(x);
    else if (x->n) {
        uint64_t mag_val = __MAG_I64__(val);
        int8_t comp_res = __BIGINT_MAGCOMP_UI64__(x, mag_val);
        if (comp_res < 0);
        else if (!comp_res) bigInt_reset(x);
        else {
            uint64_t rem = __BIGINT_INTERNAL_DIVMOD_UI64__(x, __MAG_I64__(val));
            x->limbs[0] = rem; x->n = !!(rem); x->sign = (rem) ? (x->sign * ((val < 0) ? -1 : 1)) : 1;
        }
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_add(bigInt *const x, bigInt y) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!y.n);
    else if (!x->n) { if (bigInt_mut_copy(x, y) == DNML_ALLOC_OOM) return DNML_ALLOC_OOM; }
    else if (x->sign == y.sign) { 
        dnml_status echeck = bigInt_reserve(x, max(x->n, y.n) + 1); 
        heap_alloc_oom(echeck); __BIGINT_MAGADD__(x, x, &y);
    } else {
        int8_t comp_res = __BIGINT_MAGCOMP__(x, &y);
        if (!comp_res) bigInt_reset(x);
        else if (comp_res > 0) __BIGINT_MAGSUB__(x, x, &y);
        else {
            dnml_status echeck = bigInt_reserve(x, y.n); heap_alloc_oom(echeck);
            __BIGINT_MAGSUB__(x, &y, x); x->sign = y.sign;
        }
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_sub(bigInt *const x, bigInt y) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!y.n) return BIGINT_SUCCESS;
    else if (!x->n) { if (bigInt_mut_copy(x, y) == DNML_ALLOC_OOM) return DNML_ALLOC_OOM; x->sign = -y.sign; }
    else if (x->sign == y.sign) {
        int8_t comp_res = __BIGINT_MAGCOMP__(x, &y);
        if (!comp_res) bigInt_reset(x);
        else if (comp_res > 0) __BIGINT_MAGSUB__(x, x, &y);
        else {
            dnml_status echeck = bigInt_reserve(x, y.n); heap_alloc_oom(echeck);
            __BIGINT_MAGSUB__(x, &y, x); x->sign = -x->sign;
        }
    } else {
        dnml_status echeck = bigInt_reserve(x, max(x->n, y.n) + 1);
        heap_alloc_oom(echeck); __BIGINT_MAGADD__(x, x, &y);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_mul(bigInt *const x, bigInt y) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (x->n == 0) return BIGINT_SUCCESS;
    else if (y.n == 1 && y.limbs[0] == 1) return BIGINT_SUCCESS;
    else if (!y.n) bigInt_reset(x);
    else if (x->n == 1 && x->limbs[0] == 1) { if (bigInt_mut_copy(x, y) == DNML_ALLOC_OOM) return DNML_ALLOC_OOM; }
    else {
        // The free_list is only used for debugging and testing, freeing the input bigInt to save the most memory
        bigInt *free_list[2] = { x, &y }; dnml_status echeck = bigInt_reserve(x, x->n + y.n); heap_alloc_oom(echeck);
        if (!_DNML_ALLOC_STRAT) { __BIGINT_MAGMUL__(x, x, &y, &echeck); arena_overflow(echeck, free_list, 2); }
        else if (_DNML_ALLOC_STRAT == 1) { __BIGINT_MAGMUL__(x, x, &y, &echeck);
            if (echeck == DARENA_OVERFLOW) { __BIHEAP_MUL_DISP__(x, x, &y, &echeck); heap_alloc_oom(echeck); }
        } else if (_DNML_ALLOC_STRAT == 2) { __BIHEAP_MUL_DISP__(x, x, &y, &echeck); heap_alloc_oom(echeck); }
    } x->sign *= y.sign; return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_div(bigInt *const x, bigInt y) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!y.n) return BIGINT_ERR_DOMAIN;
    else if (y.n == 1 && y.limbs[0] == 1) x->sign *= y.sign;
    else if (x->n == 1 && x->limbs[0] == 1) bigInt_reset(x);
    else if (x->n) {
        bigInt tmp_rem; bigInt *free_list[3] = { x, &y, &tmp_rem };
        dnml_status echeck = bigInt_snew(&tmp_rem, y.n); heap_alloc_oom(echeck);
        if (!_DNML_ALLOC_STRAT) { __BIGINT_MAGDIV__(x, &tmp_rem, x, &y, &echeck); arena_overflow(echeck, free_list, 3); } 
        else if (_DNML_ALLOC_STRAT == 1) { __BIGINT_MAGDIV__(x, &tmp_rem, x, &y, &echeck);
            if (echeck == DARENA_OVERFLOW) { __BIHEAP_DIV_DISP__(x, &y, x, &tmp_rem, &echeck); heap_alloc_oom(echeck);  }
        } else if (_DNML_ALLOC_STRAT == 2) { __BIHEAP_DIV_DISP__(x, &y, x, &tmp_rem, &echeck); heap_alloc_oom(echeck); }
        x->sign *= y.sign; bigInt_normalize(x);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_mod(bigInt *const x, bigInt y) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!y.n) return BIGINT_ERR_DOMAIN;
    else if (y.n == 1 && y.limbs[0] == 1) bigInt_reset(x);
    else if (x->n) {
        int8_t comp_res = __BIGINT_MAGCOMP__(x, &y);
        if (!comp_res) bigInt_reset(x);
        else if (comp_res > 0) {
            bigInt tmp_quot; bigInt *free_list[3] = { x, &y, &tmp_quot };
            dnml_status echeck = bigInt_snew(&tmp_quot, x->n); heap_alloc_oom(echeck);
            if (!_DNML_ALLOC_STRAT) { 
                __BIGINT_MAGMOD__(x, &tmp_quot, x, &y, &echeck);
                arena_overflow(echeck, free_list, 3);
            } 
            else if (_DNML_ALLOC_STRAT == 1) { 
                __BIGINT_MAGMOD__(x, &tmp_quot, x, &y, &echeck); if (echeck == DARENA_OVERFLOW) { 
                    __BIHEAP_MOD_DISP__(x, &y, x, &tmp_quot, &echeck); heap_alloc_oom(echeck);
                }
            } 
            else if (_DNML_ALLOC_STRAT == 2) { 
                __BIHEAP_MOD_DISP__(x, &y, x, &tmp_quot, &echeck); heap_alloc_oom(echeck); 
            }
        }
    } return BIGINT_SUCCESS;
}
/* ------------------ FUNCTIONAL ARITHMETIC ------------------- */
bigInt bigInt_mulu64(bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0};
    if (!x.n || !val) { if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err) }
    else if (x.n == 1 && x.limbs[0] == 1) { if (bigInt_new_u64(&res, val) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else if (val == 1) { if (bigInt_binew(&res, &x) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else { if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err); __BIGINT_MAGMUL_U64__(&res, &x, val); }
    res.sign = x.sign; return res;
}
bigInt bigInt_divu64(bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    if (!val) mut_err(err, BIGINT_ERR_DOMAIN);
    bigInt quot = {0};
    if (x.n == 0) { if (bigInt_new(&quot) == DNML_ALLOC_OOM) func_ret_oom(err) }
    else if (val == 1) { if (bigInt_binew(&quot, &x) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else if (x.n == 1 && x.limbs[0]) { if (bigInt_new(&quot) == DNML_ALLOC_OOM) func_ret_oom(err) }
    else {
        uint64_t tmp_rem; if (bigInt_new(&quot) == DNML_ALLOC_OOM) func_ret_oom(err)
        __BIGINT_MAGDIVMOD_U64__(&quot, &tmp_rem, &x, val); quot.sign = x.sign; bigInt_normalize(&quot);
    } *err = BIGINT_SUCCESS; return quot;
}
bigInt bigInt_modu64(bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    if (!val) mut_err(err, BIGINT_ERR_DOMAIN); /**/ bigInt rem = {0};
    if (x.n == 0 || val == 1) { if (bigInt_new(&rem) == DNML_ALLOC_OOM) func_ret_oom(err) }
    else {
        int8_t comp_res = __BIGINT_MAGCOMP_UI64__(&x, val);
        if (comp_res < 0) { if (bigInt_binew(&rem, &x) == DNML_ALLOC_OOM) func_ret_oom(err); }
        else if (!comp_res) { if (bigInt_new(&rem) == DNML_ALLOC_OOM) func_ret_oom(err) }
        else {
            if (bigInt_new(&rem) == DNML_ALLOC_OOM) func_ret_oom(err);
            bigInt tmp_quot; dnml_status echeck = bigInt_snew(&tmp_quot, x.n); heap_alloc_oom_mut(echeck, err);
            uint64_t tmp_rem; __BIGINT_MAGDIVMOD_U64__(&tmp_quot, &tmp_rem, &x, val); bigInt_free(&tmp_quot);
            rem.limbs[0] = tmp_rem; rem.n = !!(tmp_rem); rem.sign = (tmp_rem) ? x.sign : 1;
        }
    } *err = BIGINT_SUCCESS;
    return rem;
}
bigInt bigInt_muli64(bigInt x, const int64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0};
    if (!x.n || !val) { if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err) }
    else if (x.n == 1 && x.limbs[0] == 1) { if (bigInt_new_i64(&res, val) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else if (val == 1 || val == -1) {
        if (bigInt_binew(&res, &x) == DNML_ALLOC_OOM) func_ret_oom(err);
        res.sign = x.sign * val;
    }
    else { if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err); __BIGINT_MAGMUL_U64__(&res, &x, __MAG_I64__(val)); }
    res.sign = x.sign * ((val < 0) ? -1 : 1); return res;
}
bigInt bigInt_divi64(bigInt x, const int64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    if (!val) mut_err(err, BIGINT_ERR_DOMAIN); /**/ bigInt quot = {0};
    if (x.n == 0) { if (bigInt_new(&quot) == DNML_ALLOC_OOM) func_ret_oom(err) }
    else if (val == 1 || val == -1) {
        if (bigInt_binew(&quot, &x) == DNML_ALLOC_OOM) func_ret_oom(err);
        quot.sign = x.sign * val;
    }
    else if (x.n == 1 && x.limbs[0]) { if (bigInt_new(&quot) == DNML_ALLOC_OOM) func_ret_oom(err) }
    else { 
        uint64_t tmp_rem = 0; if (bigInt_new(&quot) == DNML_ALLOC_OOM) func_ret_oom(err)
        __BIGINT_MAGDIVMOD_U64__(&quot, &tmp_rem, &x, __MAG_I64__(val));
        quot.sign = x.sign * ((val < 0) ? -1 : 1); bigInt_normalize(&quot);
    } *err = BIGINT_SUCCESS; return quot;
}
bigInt bigInt_modi64(bigInt x, const int64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    if (!val) mut_err(err, BIGINT_ERR_DOMAIN); /**/ bigInt rem = {0};
    if (x.n == 0 || val == 1 || val == -1) { if (bigInt_new(&rem) == DNML_ALLOC_OOM) func_ret_oom(err) }
    else { uint64_t mag_val = __MAG_I64__(val);
        int8_t comp_res = __BIGINT_MAGCOMP_UI64__(&x, mag_val);
        if (comp_res < 0) { if (bigInt_binew(&rem, &x) == DNML_ALLOC_OOM) func_ret_oom(err); }
        else if (!comp_res) { if (bigInt_new(&rem) == DNML_ALLOC_OOM) func_ret_oom(err) }
        else {
            if (bigInt_new(&rem) == DNML_ALLOC_OOM) func_ret_oom(err)
            bigInt tmp_quot; dnml_status echeck = bigInt_snew(&tmp_quot, x.n); heap_alloc_oom_mut(echeck, err)
            uint64_t tmp_rem; __BIGINT_MAGDIVMOD_U64__(&tmp_quot, &tmp_rem, &x, mag_val); bigInt_free(&tmp_quot);
            rem.limbs[0] = tmp_rem; rem.n = !!(tmp_rem); rem.sign = x.sign;
        }
    } *err = BIGINT_SUCCESS; return rem;
}
bigInt bigInt_add(bigInt x, bigInt y, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt sum;
    if (!y.n) { if (bigInt_binew(&sum, &x) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else if (!x.n) { if (bigInt_binew(&sum, &y) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else if (x.sign == y.sign) {
        if (bigInt_new(&sum) == DNML_ALLOC_OOM) func_ret_oom(err);
        dnml_status echeck; __BIGINT_MAGADD__(&sum, &x, &y); sum.sign = x.sign;
    } else {
        dnml_status echeck; int8_t comp_res = __BIGINT_MAGCOMP__(&x, &y);
        if (bigInt_new(&sum) == DNML_ALLOC_OOM) func_ret_oom(err);
        if (comp_res > 0) { __BIGINT_MAGSUB__(&sum, &x, &y); sum.sign = x.sign; }
        else if (comp_res < 0) { __BIGINT_MAGSUB__(&sum, &y, &x); sum.sign = y.sign; }
    } return sum;
}
bigInt bigInt_sub(bigInt x, bigInt y, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt diff;
    if (!y.n) { if (bigInt_binew(&diff, &x) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else if (!x.n) { if (bigInt_binew(&diff, &y) == DNML_ALLOC_OOM) func_ret_oom(err); diff.sign = -y.sign; }
    else if (x.sign == y.sign) {
        dnml_status echeck; int8_t comp_res = __BIGINT_MAGCOMP__(&x, &y);
        if (bigInt_new(&diff) == DNML_ALLOC_OOM) func_ret_oom(err)
        if (comp_res > 0) { __BIGINT_MAGSUB__(&diff, &x, &y); diff.sign =  x.sign; } 
        else if (comp_res < 0) { __BIGINT_MAGSUB__(&diff, &y, &x); diff.sign = -x.sign; }
    } else {
        if (bigInt_new(&diff) == DNML_ALLOC_OOM) func_ret_oom(err)
        dnml_status echeck; __BIGINT_MAGADD__(&diff, &x, &y); diff.sign = x.sign;
    } return diff;
}
bigInt bigInt_mul(bigInt x, bigInt y, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0};
    if (!x.n || !y.n) { if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err) }
    else if (x.n == 1 && x.limbs[0] == 1) { if (bigInt_binew(&res, &y) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else if (y.n == 1 && y.limbs[0] == 1) { if (bigInt_binew(&res, &x) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else {
        if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err)
        bigInt *free_list[3] = { &res, &x, &y }; dnml_status echeck; 
        if (!_DNML_ALLOC_STRAT) { __BIGINT_MAGMUL__(&res, &x, &y, &echeck); arena_ovf_mut(echeck, err, free_list, 3); }
        else if (_DNML_ALLOC_STRAT == 1) { __BIGINT_MAGMUL__(&res, &x, &y, &echeck);
            if (echeck == DARENA_OVERFLOW) { __BIHEAP_MUL_DISP__(&res, &x, &y, &echeck); heap_alloc_oom_mut(echeck, err); }
        } else if (_DNML_ALLOC_STRAT == 2) { __BIHEAP_MUL_DISP__(&res, &x, &y, &echeck); heap_alloc_oom_mut(echeck, err); }

    } res.sign = x.sign * y.sign; return res;
}
bigInt bigInt_div(bigInt x, bigInt y, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    if (!y.n) mut_err(err, BIGINT_ERR_DOMAIN); /**/ bigInt quot = {0};
    if (x.n == 0) { if (bigInt_new(&quot) == DNML_ALLOC_OOM) func_ret_oom(err) }
    else if (y.n == 1 && y.limbs[0] == 1) {
        if (bigInt_binew(&quot, &x) == DNML_ALLOC_OOM) func_ret_oom(err);  quot.sign *= y.sign;
    } else if (x.n == 1 && x.limbs[0] == 1) { if (bigInt_new(&quot) == DNML_ALLOC_OOM) func_ret_oom(err) }
    else {
        bigInt tmp_rem; bigInt *free_list[4] = { &x, &y, &quot, &tmp_rem }; dnml_status echeck;
        if (bigInt_snew(&quot, x.n) == DNML_ALLOC_OOM) func_ret_oom(err)
        if (bigInt_snew(&tmp_rem, y.n) == DNML_ALLOC_OOM) func_ret_oom(err);
        if (!_DNML_ALLOC_STRAT) { 
            __BIGINT_MAGDIV__(&quot, &tmp_rem, &x, &y, &echeck); 
            arena_ovf_mut(echeck, err, free_list, 4); 
        }
        else if (_DNML_ALLOC_STRAT == 1) { 
            __BIGINT_MAGDIV__(&quot, &tmp_rem, &x, &y, &echeck); if (echeck == DARENA_OVERFLOW) {
                __BIHEAP_DIV_DISP__(&x, &y, &quot, &tmp_rem, &echeck); heap_alloc_oom_mut(echeck, err);
            }
        }
        else if (_DNML_ALLOC_STRAT == 2) {
            __BIHEAP_DIV_DISP__(&x, &y, &quot, &tmp_rem, &echeck); heap_alloc_oom_mut(echeck, err);
        } bigInt_free(&tmp_rem);
    } *err = BIGINT_SUCCESS; return quot;
}
bigInt bigInt_mod(bigInt x, bigInt y, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    if (!y.n) mut_err(err, BIGINT_ERR_DOMAIN); /**/ bigInt rem = {0};
    if (x.n == 0) { if (bigInt_new(&rem) == DNML_ALLOC_OOM) func_ret_oom(err) }
    else if (y.n == 1 && y.limbs[0] == 1) { if (bigInt_new(&rem) == DNML_ALLOC_OOM) func_ret_oom(err) }
    else {
        int8_t comp_res = __BIGINT_MAGCOMP__(&x, &y);
        if (comp_res < 0) { if (bigInt_binew(&rem, &x) == DNML_ALLOC_OOM) func_ret_oom(err); }
        else if (!comp_res) { if (bigInt_new(&rem) == DNML_ALLOC_OOM) func_ret_oom(err) }
        else {
            bigInt tmp_quot; bigInt *free_list[4] = { &x, &y, &rem, &tmp_quot }; dnml_status echeck;
            if (bigInt_snew(&rem, y.n) == DNML_ALLOC_OOM) func_ret_oom(err);
            if (bigInt_snew(&tmp_quot, x.n) == DNML_ALLOC_OOM) func_ret_oom(err);
            if (!_DNML_ALLOC_STRAT) {
                __BIGINT_MAGMOD__(&rem, &tmp_quot, &x, &y, &echeck); arena_ovf_mut(echeck, err, free_list, 4);
            }
            else if (_DNML_ALLOC_STRAT == 1) {
                __BIGINT_MAGMOD__(&rem, &tmp_quot, &x, &y, &echeck); if (echeck == DARENA_OVERFLOW) {
                    __BIHEAP_MOD_DISP__(&x, &y, &rem, &tmp_quot, &echeck); heap_alloc_oom_mut(echeck, err);
                }
            }
            else if (_DNML_ALLOC_STRAT == 2) {
                __BIHEAP_MOD_DISP__(&x, &y, &rem, &tmp_quot, &echeck); heap_alloc_oom_mut(echeck, err);
            } bigInt_free(&tmp_quot);
        }
    } *err = BIGINT_SUCCESS; return rem;
}





//* ======================================== SIGNED NUMBER THEORETIC ========================================= */
/* -------------- Pure Number Theoretic -------------- */
bigInt bigInt_gcdu64(bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    if (!val) return x;
    bigInt res = {0};
    if (x.n == 0) { if (bigInt_new_u64(&res, val) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else if (x.n == 1) { if (bigInt_new_u64(&res, ___GCD_UI64___(x.limbs[0], val)) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else {
        if (bigInt_snew(&res, min(x.n, 1)) == DNML_ALLOC_OOM) func_ret_oom(err)
        limb_t tmp_limb[1] = {val}; bigInt y = { .limbs = tmp_limb, .n = 1,  .cap = 1, .sign = 1 };
        dnml_status echeck; __BIGINT_MAGGCD__(&res, &x, &y, &echeck);
    } return res;
}
bigInt bigInt_gcdi64(bigInt x, const int64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    if (!val) return x;
    bigInt res = {0};
    if (x.n == 0) { if (bigInt_new_u64(&res, __MAG_I64__(val)) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else if (x.n == 1) {
        dnml_status echeck = bigInt_new_u64(&res, ___GCD_UI64___(x.limbs[0], __MAG_I64__(val)));
        if (echeck == DNML_ALLOC_OOM) func_ret_oom(err);
    } else {
        if (bigInt_snew(&res, min(x.n, 1)) == DNML_ALLOC_OOM) func_ret_oom(err)
        dnml_arena *_DASI_UI64_ARENA = _USE_ARENA(); arena_poison_mut(_DASI_UI64_ARENA, err);
        size_t tmp_mark = arena_mark(_DASI_UI64_ARENA); dnml_status echeck;
        limb_t *tmp_limbs = arena_galloc(_DASI_UI64_ARENA, 1, &echeck);
        arena_alloc_oom_mut(echeck, _DASI_UI64_ARENA, err);

        bigInt y = { .limbs = tmp_limbs, .n = 1, .cap = 1, .sign = 1 };
        y.limbs[0] = __MAG_I64__(val); __BIGINT_MAGGCD__(&res, &x, &y, &echeck);
        arena_alloc_oom_mut(echeck, _DASI_UI64_ARENA, err); arena_rewind(_DASI_UI64_ARENA, tmp_mark);
        tmp_limbs = NULL; _DASI_UI64_ARENA = NULL;
    } return res;
}
bigInt bigInt_gcd(bigInt x, bigInt y, dnml_status *err) {                   //TODO REFACTOR TO HAVE HEAP VS ARENA
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    if (x.n == 0) return y;
    else if (y.n == 0) return x;
    bigInt res = {0};
    if (x.n == 1 && y.n == 1) {
        dnml_status echeck = bigInt_new_u64(&res, ___GCD_UI64___(x.limbs[0], y.limbs[0]));
        if (echeck == DNML_ALLOC_OOM) func_ret_oom(err);
    } else {
        if (bigInt_snew(&res, min(x.n, y.n)) == DNML_ALLOC_OOM) func_ret_oom(err);
        dnml_status echeck; __BIGINT_MAGGCD__(&res, &x, &y, &echeck);
        arena_alloc_oom_mut(echeck, &___DASI_NUMERIC_ARENA_, err);
    } return res;
}
bigInt bigInt_lcmu64(bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0};
    if (!val || !x.n) { if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err) }
    else if (x.n == 1 && x.limbs[0] == 1) { if (bigInt_new_u64(&res, val) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else if (x.n == 1 && x.limbs[0] == val) { if (bigInt_new_u64(&res, val) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else if (val == 1) { if (bigInt_binew(&res, &x) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else {
        if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err);
        dnml_arena *_DASI_LCM_UI64_ARENA = _USE_ARENA(); arena_poison_mut(_DASI_LCM_UI64_ARENA, err);
        size_t tmp_mark = arena_mark(_DASI_LCM_UI64_ARENA); dnml_status echeck;
        limb_t *tmp_limbs = arena_galloc(_DASI_LCM_UI64_ARENA, 1, &echeck);
        arena_alloc_oom_mut(echeck, _DASI_LCM_UI64_ARENA, err);

        bigInt y = { .limbs = tmp_limbs, .sign = 1, .n = 1, .cap = 1 };
        y.limbs[0] = val; __BIGINT_MAGLCM__(&res, &x, &y, &echeck);
        arena_alloc_oom_mut(echeck, _DASI_LCM_UI64_ARENA, err);
        arena_rewind(_DASI_LCM_UI64_ARENA, tmp_mark); _DASI_LCM_UI64_ARENA = NULL;
    } return res;
}
bigInt bigInt_lcmi64(bigInt x, const int64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0}; uint64_t mag_val = __MAG_I64__(val);
    if (!mag_val || !x.n) { if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err) }
    else if (x.n == 1 && x.limbs[0] == 1) { if (bigInt_new_u64(&res, mag_val) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else if (x.n == 1 && _lib_crt_ispos(val) &&  x.limbs[0] == __MAG_I64__(val)) { 
        if (bigInt_new_u64(&res, mag_val) == DNML_ALLOC_OOM) func_ret_oom(err);
    }
    else if (mag_val == 1) { if (bigInt_binew(&res, &x) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else {
        if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err);
        dnml_arena *_DASI_LCM_UI64_ARENA = _USE_ARENA(); arena_poison_mut(_DASI_LCM_UI64_ARENA, err);
        size_t tmp_mark = arena_mark(_DASI_LCM_UI64_ARENA); dnml_status echeck;
        limb_t *tmp_limbs = arena_galloc(_DASI_LCM_UI64_ARENA, 1, &echeck);
        arena_alloc_oom_mut(echeck, _DASI_LCM_UI64_ARENA, err);

        bigInt y = { .limbs = tmp_limbs, .sign = 1, .n = 1, .cap = 1 };
        y.limbs[0] = mag_val; __BIGINT_MAGLCM__(&res, &x, &y, &echeck);
        arena_alloc_oom_mut(echeck, _DASI_LCM_UI64_ARENA, err);
        arena_rewind(_DASI_LCM_UI64_ARENA, tmp_mark); _DASI_LCM_UI64_ARENA = NULL;
    } return res;
}
bigInt bigInt_lcm(bigInt x, bigInt y, dnml_status *err) {                   //TODO REFACTOR TO HAVE HEAP VS ARENA
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    if (x.n == 1 && x.limbs[0] == 1) return y; // lcm(1, y) = y
    else if (y.n == 1 && y.limbs[0] == 1) return x; // lcm(x, 1) = x
    else if (!__BIGINT_MAGCOMP__(&x, &y)) return x; // lcm(x, y) = x WHEN x = y
    bigInt res = {0}; 
    if (!y.n || !x.n) { if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err) } // lcm(0, x) || lcm(x, 0) = 0
    else {
        if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err);
        dnml_status echeck; __BIGINT_MAGLCM__(&res, &x, &y, &echeck);
        arena_alloc_oom_mut(echeck, &___DASI_NUMERIC_ARENA_, err);
    } return res;
}
bool bigInt_is_prime(bigInt x, dnml_status *err) {                          //TODO REFACTOR TO HAVE HEAP VS ARENA
    test_assert_mut(bigInt_validate(x), bi_full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (x.sign == -1) return false;
    if (x.n == 1) { uint64_t val = x.limbs[0];
        if (val <= 1) return false;
        else if (val == 2 || val == 3 || val == 5) return true;
        else if (!(val & 1) || val % 3 == 0 || val % 5 == 0) return false;
    } else { if (!(x.limbs[0] & 1)) return true;
        else if (x.limbs[0] % 10 == 5 || !(x.limbs[0] % 10)) return true;
    }
    dnml_arena *_DASI_LPRIME_ARENA = _USE_ARENA();
    arena_grow(_DASI_LPRIME_ARENA, __BIGINT_PTEST_WS__(x.n));
    calc_ctx _isprime_ctx = {
        .alloc = &arena_alloc_adapter,
        .mark = &arena_mark_adapter,
        .rewind = &arena_rewind_adapter,
        .clear = &arena_clear_adapter,
        .destruct = &arena_destruct_adapter,
        .state  = _DASI_LPRIME_ARENA
    }; dnml_status echeck = BIGINT_SUCCESS;
    uint8_t ret = __BIGINT_PTEST_DISPATCH__(&x, _isprime_ctx, &echeck); darena_assert();
    *err = echeck; return (bool)(ret);
}
/* ---------------- Modular Reduction ---------------- */
dnml_status bigInt_mut_emodu64(bigInt *const x, const uint64_t mod) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!mod) return BIGINT_ERR_INVAL;
    else if (!x->n);
    else if (mod == 1) bigInt_reset(x);
    else if (x->n == 1 && x->limbs[0] < mod && x->sign == 1);
    else {
        uint64_t res_rem;
        __BIGINT_MAGEMOD_U64__(&res_rem, x, mod);
        if (x->sign == -1 && res_rem) {
            res_rem = (mod - res_rem) % mod;
        } x->limbs[0] = res_rem; x->sign = 1;
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_emodi64(bigInt *const x, const int64_t mod) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!mod) return BIGINT_ERR_INVAL;
    else if (!x->n);
    else if (mod == 1 || mod == -1) bigInt_reset(x);
    else if (x->n == 1 && x->limbs[0] < __MAG_I64__(mod) && x->sign == 1);
    else {
        uint64_t mag_mod = __MAG_I64__(mod), res_rem;
        __BIGINT_MAGEMOD_U64__(&res_rem, x, mag_mod);
        if (x->sign == -1 && res_rem) {
            res_rem = (mag_mod - res_rem) % mag_mod;
        } x->limbs[0] = res_rem; x->sign = 1;
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_emod(bigInt *const x, bigInt mod) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(mod), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!mod.n) return BIGINT_ERR_INVAL;
    else if (!x->n);
    else if (mod.n == 1 && mod.limbs[0] == 1) bigInt_reset(x);
    else if (__BIGINT_MAGCOMP__(x, &mod) == -1 && x->sign == 1);
    else { dnml_arena *_DASI_MUT_MODULO_ARENA = _USE_ARENA(); arena_poisoined(_DASI_MUT_MODULO_ARENA);
        size_t tmp_mark = arena_mark(_DASI_MUT_MODULO_ARENA); dnml_status echeck;
        limb_t *tmp_limbs = arena_galloc(_DASI_MUT_MODULO_ARENA, mod.n, &echeck);
        arena_alloc_oom(echeck, _DASI_MUT_MODULO_ARENA);

        bigInt tmp_res = {  .limbs = tmp_limbs, .n = 0, .cap = mod.n, .sign = 1 };
        __BIGINT_MAGEMOD__(&tmp_res, x, &mod, &echeck);
        arena_alloc_oom(echeck, _DASI_MUT_MODULO_ARENA);
        if (x->sign == -1 && tmp_res.n) { 
            __BIGINT_MAGSUB__(&tmp_res, &mod, &tmp_res);
            __BIGINT_MAGEMOD__(&tmp_res, &tmp_res, &mod, &echeck); arena_alloc_oom(echeck, _DASI_MUT_MODULO_ARENA);
        } __BIGINT_INTERNAL_COPY__(x, &tmp_res);
        arena_rewind(_DASI_MUT_MODULO_ARENA, tmp_mark); _DASI_MUT_MODULO_ARENA = NULL;
    } return BIGINT_SUCCESS;
}
uint64_t bigInt_emodu64(bigInt x, const uint64_t mod, dnml_status *err) {
    test_assert_mut(bigInt_validate(x), bi_full_contract, clear_arena, err, BIGINT_ERR_INVAL, -1);
    if (!mod) mut_gret(err, BIGINT_ERR_INVAL, -1);
    else if (mod == 1 || !x.n) mut_gret(err, BIGINT_SUCCESS, 0);
    else if ( x.n == 1 &&
        x.limbs[0] < mod &&
        x.sign == 1) mut_gret(err, BIGINT_SUCCESS, x.limbs[0]);
    uint64_t res; __BIGINT_MAGEMOD_U64__(&res, &x, mod);
    if (x.sign == -1 && res) res = (mod - res) % mod;
    *err = BIGINT_SUCCESS; return res;
}
uint64_t bigInt_emodi64(bigInt x, const int64_t mod, dnml_status *err) {
    test_assert_mut(bigInt_validate(x), bi_full_contract, clear_arena, err, BIGINT_ERR_INVAL, -1);
    if (!mod) mut_gret(err, BIGINT_ERR_INVAL, -1);
    else if (mod == 1 || mod == -1 || !x.n) mut_gret(err, BIGINT_SUCCESS, 0);
    else if ( x.n == 1 &&
        x.limbs[0] < __MAG_I64__(mod) &&
        x.sign == 1) mut_gret(err, BIGINT_SUCCESS, x.limbs[0]);
    uint64_t res, mag_mod = __MAG_I64__(mod);
    __BIGINT_MAGEMOD_U64__(&res, &x, mag_mod);
    if (x.sign == -1 && res) res = (mag_mod - res) % mag_mod;
    *err = BIGINT_SUCCESS; return res; // Return a uint64_t to holds |INT64_MIN|
}
bigInt bigInt_emod(bigInt x, bigInt mod, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(mod), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    if (!mod.n) mut_err(err, BIGINT_ERR_INVAL);
    bigInt res = {0}; if (!x.n) { if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err) }
    else if (mod.n == 1 && mod.limbs[0] == 1) { if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err) }
    else if (__BIGINT_MAGCOMP__(&x, &mod) == -1  && x.sign == 1) {
        if (bigInt_binew(&res, &x) == DNML_ALLOC_OOM) func_ret_oom(err);
    } else {
        if (bigInt_snew(&res, mod.n) == DNML_ALLOC_OOM) func_ret_oom(err);
        dnml_status echeck; __BIGINT_MAGEMOD__(&res, &x, &mod, &echeck);
        arena_alloc_oom_mut(echeck, &___DASI_NUMERIC_ARENA_, err);
        if (x.sign == -1 && res.n) {
            __BIGINT_MAGSUB__(&res, &mod, &res); 
            __BIGINT_MAGEMOD__(&res, &res, &mod, &echeck); arena_alloc_oom_mut(echeck, &___DASI_NUMERIC_ARENA_, err);
        }
    } *err = BIGINT_SUCCESS; return res;
}
/* ---------------- SMALL Modular Arithmetic --------------- */
dnml_status bigInt_mut_modadd_u64(bigInt *const x, bigInt y, const uint64_t mod) { return BIGINT_SUCCESS; }
dnml_status bigInt_mut_modsub_u64(bigInt *const x, bigInt y, const uint64_t mod) { return BIGINT_SUCCESS; }
dnml_status bigInt_mut_modadd(bigInt *const x, bigInt y, bigInt mod) { return BIGINT_SUCCESS; }
dnml_status bigInt_mut_modsub(bigInt *const x, bigInt y, bigInt mod) { return BIGINT_SUCCESS; }
uint64_t bigInt_modadd_u64(bigInt x, bigInt y, const uint64_t mod) { return 0; }
uint64_t bigInt_modsub_u64(bigInt x, bigInt y, const uint64_t mod) { return 0; }
bigInt bigInt_modadd(bigInt x, bigInt y, bigInt mod) { return (bigInt){0}; }
bigInt bigInt_modsub(bigInt x, bigInt y, bigInt mod) { return (bigInt){0}; }
/* ---------------- LARGE Modular Arithmetic --------------- */
dnml_status bigInt_mut_modmul_u64(bigInt *const x, bigInt y, const uint64_t mod) { return BIGINT_SUCCESS; }
dnml_status bigInt_mut_moddiv_u64(bigInt *const x, bigInt y, const uint64_t mod) { return BIGINT_SUCCESS; }
dnml_status bigInt_mut_modmul(bigInt *const x, bigInt y, bigInt mod) { return BIGINT_SUCCESS; }
dnml_status bigInt_mut_moddiv(bigInt *const x, bigInt y, bigInt mod) { return BIGINT_SUCCESS; }
uint64_t bigInt_modmul_u64(bigInt x, bigInt y, const uint64_t mod) { return 0; }
uint64_t bigInt_moddiv_u64(bigInt x, bigInt y, const uint64_t mod) { return 0; }
bigInt bigInt_modmul(bigInt x, bigInt y, bigInt mod) { return (bigInt){0}; }
bigInt bigInt_moddiv(bigInt x, bigInt y, bigInt mod) { return (bigInt){0}; }
/* ---------------------- Modular Algebraic ------------------ */
dnml_status bigInt_mut_modexp_u64(bigInt *const x, bigInt y, const uint64_t mod) { return BIGINT_SUCCESS; }
dnml_status bigInt_mut_modsqr_u64(bigInt *const x, const uint64_t mod) { return BIGINT_SUCCESS; }
dnml_status bigInt_mut_modinv_u64(bigInt *const x, const uint64_t mod) { return BIGINT_SUCCESS; }
dnml_status bigInt_mut_modexp(bigInt *const x, bigInt y, bigInt mod) { return BIGINT_SUCCESS; }
dnml_status bigInt_mut_modsqr(bigInt *const x, bigInt mod) { return BIGINT_SUCCESS; }
dnml_status bigInt_mut_modinv(bigInt *const x, bigInt mod) { return BIGINT_SUCCESS; }
uint64_t bigInt_modexp_u64(bigInt x, bigInt y, const uint64_t mod) { return 0; }
uint64_t bigInt_modsqr_u64(bigInt x, const uint64_t mod) { return 0; }
uint64_t bigInt_modinv_u64(bigInt x, const uint64_t mod) { return 0; }
bigInt bigInt_modexp(bigInt x, bigInt y, bigInt mod) { return (bigInt){0}; }
bigInt bigInt_modsqr(bigInt x, bigInt mod) { return (bigInt){0}; }
bigInt bigInt_modinv(bigInt x, bigInt mod) { return (bigInt){0}; }




//* ====================================== SIGNED ALGEBRAIC OPERATIONS ======================================= */
/* -------------- MUTATIVE ALGEBRAIC -------------- */
dnml_status bigInt_mut_sqr(bigInt *const x) {                           //TODO REFACTOR TO HAVE HEAP VS ARENA
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (x->n == 1) {
        if (x->limbs[0] < UINT32_MAX) x->limbs[0] *= x->limbs[0];
        else if (x->limbs[0] != 1) {
            bigInt_reserve(x, 2);
            x->limbs[0] = __MUL_UI64__(x->limbs[0], x->limbs[0], &x->limbs[1] );
            x->n = 2;
        }
        x->sign = 1;
    } else if (x->n) {
        dnml_status echeck = bigInt_reserve(x, x->n << 1); heap_alloc_oom(echeck);
        dnml_arena *_DASI_MUTSQR_ARENA = _USE_ARENA(); arena_poisoined(_DASI_MUTSQR_ARENA);
        size_t mutsqr_mark = arena_mark(_DASI_MUTSQR_ARENA);
        limb_t *tmp_limb = arena_galloc(_DASI_MUTSQR_ARENA, x->n * 2, &echeck);
        arena_alloc_oom(echeck, _DASI_MUTSQR_ARENA);

        bigInt tmp_res = {.limbs = tmp_limb, .sign = 1, .n = 0, .cap = x->n * 2};
        __BIGINT_MAGSQR__(&tmp_res, x, &echeck);
        arena_alloc_oom(echeck, _DASI_MUTSQR_ARENA); tmp_res.sign = 1;
        echeck = bigInt_mut_ocopy(x, tmp_res); ocopy_check(echeck, _DASI_MUTSQR_ARENA);
        arena_rewind(_DASI_MUTSQR_ARENA, mutsqr_mark);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_pow(bigInt *const x, const uint64_t exp) {       //TODO REFACTOR TO HAVE HEAP VS ARENA
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!exp) { bigInt_reset(x);
        x->limbs[0] = 1;
        x->n = 1; x->sign = 1;
    } if (!x->n || exp == 1) return BIGINT_SUCCESS;
    if (exp == 2) return bigInt_mut_sqr(x);

    /* More standard cases (computationally) */
    if (x->n == 1 && x->limbs[0] == 1) x->sign = (!(exp & 1)) ? 1 : x->sign;
    else if (x->n == 1 && __SAFE_EXP__(x->limbs[0], exp)) {
        x->limbs[0] = (uint64_t)(pow((double)x->limbs[0], (double)exp));
        x->sign = (!(exp & 1)) ? 1 : x->sign;
    } else {
         dnml_status echeck = bigInt_reserve(x, x->n * exp); heap_alloc_oom(echeck);
        dnml_arena *_DASI_MUTPOW_ARENA = _USE_ARENA(); arena_poisoined(_DASI_MUTPOW_ARENA);
        size_t mutpow_mark = arena_mark(_DASI_MUTPOW_ARENA);
        limb_t *tmp_limbs = arena_galloc(_DASI_MUTPOW_ARENA, x->n * exp, &echeck);
        arena_alloc_oom(echeck, _DASI_MUTPOW_ARENA);

        bigInt tmp_res = {.limbs = tmp_limbs, .sign = 1, .n = 0, .cap = x->n * exp};
        __BIGINT_MAGPOW__(&tmp_res, x, exp, &echeck);
        arena_alloc_oom(echeck, _DASI_MUTPOW_ARENA); tmp_res.sign = (!(exp & 1)) ? 1 : x->sign;
        echeck = bigInt_mut_ocopy(x, tmp_res); ocopy_check(echeck, _DASI_MUTPOW_ARENA);
        arena_rewind(_DASI_MUTPOW_ARENA, mutpow_mark);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_sqrt(bigInt *const x) {                          //TODO REFACTOR TO HAVE HEAP VS ARENA
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (x->sign == -1) return BIGINT_ERR_DOMAIN;
    if (x->n == 1 && x->limbs[0] == 1);
    else if (x->n) {
        /* The square root of any integer x will ALWAYS be <= x */
        dnml_arena *_DASI_MUTSQRT_ARENA = _USE_ARENA(); arena_poisoined(_DASI_MUTSQRT_ARENA);
        size_t mutsqrt_mark = arena_mark(_DASI_MUTSQRT_ARENA); dnml_status echeck;
        limb_t *tmp_limbs = arena_galloc(_DASI_MUTSQRT_ARENA, (x->n >> 1), &echeck);
        arena_alloc_oom(echeck, _DASI_MUTSQRT_ARENA);

        bigInt tmp_res = {.limbs = tmp_limbs, .sign = 1, .n = 0, .cap = (x->n >> 1)};
        __BIGINT_MAGSQRT__(&tmp_res, x, &echeck);
        arena_alloc_oom(echeck, _DASI_MUTSQRT_ARENA); tmp_res.sign = 1;
        echeck = bigInt_mut_ocopy(x, tmp_res); ocopy_check(echeck, _DASI_MUTSQRT_ARENA);
        arena_rewind(_DASI_MUTSQRT_ARENA, mutsqrt_mark);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_cbrt(bigInt *const x) {                          //TODO REFACTOR TO HAVE HEAP VS ARENA
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (x->n == 1 && x->limbs[0] == 1);
    else if (x->n) {
        /* The square root of any integer x will ALWAYS be <= x */
        dnml_arena *_DASI_MUTCBRT_ARENA = _USE_ARENA(); arena_poisoined(_DASI_MUTCBRT_ARENA);
        size_t mutcbrt_mark = arena_mark(_DASI_MUTCBRT_ARENA); dnml_status echeck;
        limb_t *tmp_limbs = arena_galloc(_DASI_MUTCBRT_ARENA, (x->n >> 1), &echeck);
        arena_alloc_oom(echeck, _DASI_MUTCBRT_ARENA);

        bigInt tmp_res = {.limbs = tmp_limbs, .sign = 1, .n = 0, .cap = (x->n / 3)};
        __BIGINT_MAGCBRT__(&tmp_res, x, &echeck);
        arena_alloc_oom(echeck, _DASI_MUTCBRT_ARENA); tmp_res.sign = x->sign;
        echeck = bigInt_mut_ocopy(x, tmp_res); ocopy_check(echeck, _DASI_MUTCBRT_ARENA);
        arena_rewind(_DASI_MUTCBRT_ARENA, mutcbrt_mark);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_nrt(bigInt *const x, const uint64_t root) {      //TODO REFACTOR TO HAVE HEAP VS ARENA
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!root) return BIGINT_ERR_INVAL;
    if (!(root & 1) && x->sign == -1) return BIGINT_ERR_DOMAIN;
    if (x->n == 1 && x->limbs[0] == 1);
    else if (x->n) {
        dnml_arena *_DASI_MUTNRT_ARENA = _USE_ARENA(); arena_poisoined(_DASI_MUTNRT_ARENA);
        size_t mutnrt_mark = arena_mark(_DASI_MUTNRT_ARENA); dnml_status echeck;
        size_t alloc_size = (__IS_2POW__(root)) ? (x->n >> __CTZ_UI64__(root)) : (x->n / root);
        limb_t *tmp_limbs = arena_galloc(_DASI_MUTNRT_ARENA, alloc_size, &echeck);
        arena_alloc_oom(echeck, _DASI_MUTNRT_ARENA);

        bigInt tmp_res = {.limbs = tmp_limbs, .sign = 1, .n = 0, .cap = alloc_size};
        __BIGINT_MAGNRT__(&tmp_res, x, root, &echeck);
        arena_alloc_oom(echeck, _DASI_MUTNRT_ARENA); tmp_res.sign = (!(root & 1)) ? 1 : x->sign;
        echeck = bigInt_mut_ocopy(x, tmp_res); ocopy_check(echeck, _DASI_MUTNRT_ARENA);
        arena_rewind(_DASI_MUTNRT_ARENA, mutnrt_mark);
    } return BIGINT_SUCCESS;
}
/* -------------- FUNCTIONAL ALGEBRAIC -------------- */
bigInt bigInt_sqr(bigInt x, dnml_status *err) {                         //TODO REFACTOR TO HAVE HEAP VS ARENA
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0}; if (x.n == 0) { if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err) }
    else if (x.n == 1) {
        limb_t val = x.limbs[0];
        if (val == 1) { if (bigInt_new_u64(&res, 1) == DNML_ALLOC_OOM) func_ret_oom(err); }
        else if (val < UINT32_MAX) { if (bigInt_new_u64(&res, val * val) == DNML_ALLOC_OOM) func_ret_oom(err); }
        else {
            if (bigInt_snew(&res, 2) == DNML_ALLOC_OOM) func_ret_oom(err);
            res.limbs[0] = __MUL_UI64__(x.limbs[0], x.limbs[0],&res.limbs[1]);
            res.n = 2;
        } res.sign = 1;
    } else {
        if (bigInt_snew(&res, x.n * 2) == DNML_ALLOC_OOM) func_ret_oom(err)
        dnml_status echeck; __BIGINT_MAGSQR__(&res, &x, &echeck);
        arena_alloc_oom_mut(echeck, &___DASI_NUMERIC_ARENA_, err); res.sign = 1;
    } return res;
}
bigInt bigInt_pow(bigInt x, const uint64_t exp, dnml_status *err) {     //TODO REFACTOR TO HAVE HEAP VS ARENA
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    if (exp == 2) return bigInt_sqr(x, err);
    bigInt res = {0}; if (!exp) { if (bigInt_new_u64(&res, 1) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else if (!x.n) { if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err) }
    else if (x.n == 1 && x.limbs[0] == 1) {
        dnml_status echeck = bigInt_new_i64 (&res, 1 * (!(exp & 1) ? 1 : x.sign));
        if (echeck == DNML_ALLOC_OOM) func_ret_oom(err);
    }
    else if (x.n == 1 && __SAFE_EXP__(x.limbs[0], exp)) {
        uint64_t exp_res = (uint64_t)(pow((double)x.limbs[0], (double)exp));
        if (bigInt_new_u64(&res, exp_res) == DNML_ALLOC_OOM) func_ret_oom(err);
        res.sign = (!(exp & 1)) ? 1 : x.sign;
    }
    else if (exp == 1) { if (bigInt_binew(&res, &x) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else {
        if (bigInt_snew(&res, x.n * exp) == DNML_ALLOC_OOM) func_ret_oom(err);
        dnml_status echeck; __BIGINT_MAGPOW__(&res, &x, exp, &echeck);
        arena_alloc_oom_mut(echeck, &___DASI_NUMERIC_ARENA_, err); res.sign = (!(exp & 1)) ? 1 : x.sign;
    } return res;
}
bigInt bigInt_sqrt(bigInt x, dnml_status *err) {                        //TODO REFACTOR TO HAVE HEAP VS ARENA
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    if (x.sign == -1) mut_err(err, BIGINT_ERR_DOMAIN);
    bigInt res = {0}; if (!x.n) { if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err) }
    else if (x.n == 1 && x.limbs[0] == 1) { if (bigInt_new_u64(&res, 1) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else {
        if (bigInt_snew(&res, (x.n >> 1)) == DNML_ALLOC_OOM) func_ret_oom(err);
        dnml_status echeck; __BIGINT_MAGSQR__(&res, &x, &echeck);
        arena_alloc_oom_mut(echeck, &___DASI_NUMERIC_ARENA_, err); res.sign = 1;
    } return res;
}
bigInt bigInt_cbrt(bigInt x, dnml_status *err) {                        //TODO REFACTOR TO HAVE HEAP VS ARENA
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res = {0}; if (!x.n) { if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err) }
    else if (x.n == 1 && x.limbs[0] == 1) { if (bigInt_new_i64(&res, (1 * x.sign)) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else {
        if (bigInt_snew(&res, x.n / 3) == DNML_ALLOC_OOM) func_ret_oom(err);
        dnml_status echeck; __BIGINT_MAGCBRT__(&res, &x, &echeck);
        arena_alloc_oom_mut(echeck, &___DASI_NUMERIC_ARENA_, err); res.sign = x.sign;
    } return res;
}
bigInt bigInt_nrt(bigInt x, const uint64_t root, dnml_status *err) {    //TODO REFACTOR TO HAVE HEAP VS ARENA
    test_assert_mut(
        bigInt_validate(x), bi_full_contract, clear_arena,
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    if (!root) mut_err(err, BIGINT_ERR_INVAL);
    else if (!(root & 1) && x.sign == -1) mut_err(err, BIGINT_ERR_DOMAIN);
    bigInt res = {0}; if (!x.n) { if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err) }
    else if (x.n == 1 && x.limbs[0] == 1) {
        bigInt_new_u64(&res, 1);
        res.sign = (!(root & 1)) ? 1 : x.sign;
    } else {
        size_t alloc_size = (__IS_2POW__(root)) ? (x.n >> __CTZ_UI64__(root)) : (x.n / root);
        if (bigInt_snew(&res, alloc_size) == DNML_ALLOC_OOM) func_ret_oom(err);
        dnml_status echeck; __BIGINT_MAGNRT__(&res, &x, root, &echeck);
        arena_alloc_oom_mut(echeck, &___DASI_NUMERIC_ARENA_, err); res.sign = (!(root & 1)) ? 1 : x.sign;
    } return res;
}




//* ================================================= COPIES ================================================= */
/* -------------  Mutative SMALL Copies ------------- */
dnml_status bigInt_mut_copyu64(bigInt *const dst, const uint64_t src) {
    test_assert(dst != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(__BIGINT_INTERNAL_SVALID__(dst), bi_storage_inval, clear_arena, BIGINT_ERR_STORE_IN);
    bigInt_canonicalize(dst);
    if (dst->n == 0 && !src) return BIGINT_SUCCESS;
    if (dst->n == 1 && dst->limbs[0] == src) return BIGINT_SUCCESS;
    dst->limbs[0] = src; dst->n = src ? 1 : 0; dst->sign = 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_dcopyu64(bigInt *const dst, const uint64_t src) {
    test_assert(dst != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(__BIGINT_INTERNAL_SVALID__(dst), bi_storage_inval, clear_arena, BIGINT_ERR_STORE_IN);
    bigInt_canonicalize(dst);
    // Always reallocate and resize if dst->cap is more than 1
    if (dst->cap > 1) {
        uint64_t* __BUFFER_P = realloc(dst->limbs, sizeof(uint64_t));
        if (__BUFFER_P == NULL) return DNML_ALLOC_OOM;
        dst->limbs = __BUFFER_P;
        dst->cap     = 1;
    }
    if (dst->n == 0 && !src) return BIGINT_SUCCESS;
    if (dst->n == 1 && dst->limbs[0] == src) return BIGINT_SUCCESS;
    dst->limbs[0] = src; dst->n = src ? 1 : 0; dst->sign = 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_copyi64(bigInt *const dst, const int64_t src) {
    test_assert(dst != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(__BIGINT_INTERNAL_SVALID__(dst), bi_storage_inval, clear_arena, BIGINT_ERR_STORE_IN);
    bigInt_canonicalize(dst);
    if (dst->n == 0 && !src) return BIGINT_SUCCESS;
    if (dst->n == 1 && dst->limbs[0] == __MAG_I64__(src)) {
        dst->sign = (src < 0) ? -1 : 1; return BIGINT_SUCCESS;
    }
    dst->limbs[0] = __MAG_I64__(src); dst->n = src ? 1 : 0;
    dst->sign = (src < 0 ? -1 : 1); return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_dcopyi64(bigInt *const dst, const int64_t src) {
    test_assert(dst != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(__BIGINT_INTERNAL_SVALID__(dst), bi_storage_inval, clear_arena, BIGINT_ERR_STORE_IN);
    bigInt_canonicalize(dst);
    // Always reallocate and resize if dst->cap is more than 1
    if (dst->cap > 1) {
        uint64_t* __BUFFER_P = realloc(dst->limbs, sizeof(uint64_t));
        if (__BUFFER_P == NULL) return DNML_ALLOC_OOM;
        dst->limbs = __BUFFER_P; dst->cap = 1;
    }
    if (dst->n == 0 && !src) return BIGINT_SUCCESS;
    if (dst->n == 1 && dst->limbs[0] == __MAG_I64__(src)) {
        dst->sign = (src < 0) ? -1 : 1;
        return BIGINT_SUCCESS;
    }
    dst->limbs[0] = __MAG_I64__(src); dst->n = src ? 1 : 0;
    dst->sign = (src< 0 ? -1 : 1); return BIGINT_SUCCESS;
}
/* -------------  Mutative LARGE Copies ------------- */
dnml_status bigInt_mut_copyf128(bigInt *const dst, long double src) { return BIGINT_SUCCESS; }
dnml_status bigInt_mut_dcopyf128(bigInt *const dst, long double src) { return BIGINT_SUCCESS; }
dnml_status bigInt_mut_ocopyf128(bigInt *const dst, long double src) { return BIGINT_SUCCESS; }
dnml_status bigInt_mut_tover_copyf128(bigInt *const dst, long double src) { return BIGINT_SUCCESS; }
dnml_status bigInt_mut_copy(bigInt *const dst, bigInt src) {
    test_assert(dst != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(__BIGINT_INTERNAL_SVALID__(dst), bi_storage_inval, clear_arena, BIGINT_ERR_STORE_IN);
    test_assert(bigInt_validate(src), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (dst->limbs == src.limbs) {
        dst->n = src.n; dst->cap = src.n;
        dst->sign = src.sign; return BIGINT_SUCCESS;
    } bigInt_canonicalize(dst); // Enforce contracts, ESPECAILLY Contract 3
    /* Fast Paths */
    // Since they're equal, and due to Contract 3
    //  ------> They're not subjected to resizing if these cases are true
    if (dst->n == 0 && src.n == 0) return BIGINT_SUCCESS;
    if (dst->n == src.n && !memcmp(dst->limbs, src.limbs, src.n)) {
        dst->sign = src.sign;
        return BIGINT_SUCCESS;
    }

    /* Standard Route */
    if (dst->cap < src.n) { dnml_status echeck = bigInt_reserve(dst, src.n); heap_alloc_oom(echeck); }
    memcpy(dst->limbs, src.limbs, src.n);
    dst->n = src.n; dst->sign = src.sign;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_dcopy(bigInt *const dst, bigInt src) {
    test_assert(dst != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(__BIGINT_INTERNAL_SVALID__(dst), bi_storage_inval, clear_arena, BIGINT_ERR_STORE_IN);
    test_assert(bigInt_validate(src), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (dst->limbs == src.limbs) {
        dst->n = src.n; dst->cap = src.n;
        dst->sign = src.sign; return BIGINT_SUCCESS;
    } bigInt_canonicalize(dst); // Enforce contracts, ESPECAILLY Contract 3
    if (dst->cap != src.cap) if (bigInt_resize(dst, src.cap) == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    /* Fast Paths */
    // The equal fast path (dst != 0 && src != 0) is not here since
    // Reallocation and Resizing may tamper with the size metadata,
    //  -----> Tampering with the validity of memcmp()
    if (dst->n == 0 && src.n == 0) return BIGINT_SUCCESS;

    /* Standard Path */
    memcpy(dst->limbs, src.limbs, src.n * sizeof(uint64_t));
    dst->n = src.n; dst->sign = src.sign;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_ocopy(bigInt *const dst, bigInt src) {
    test_assert(dst != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(__BIGINT_INTERNAL_SVALID__(dst), bi_storage_inval, clear_arena, BIGINT_ERR_STORE_IN);
    test_assert(bigInt_validate(src), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (dst->limbs == src.limbs) {
        dst->n = src.n; dst->cap = src.n;
        dst->sign = src.sign; return BIGINT_SUCCESS;
    } bigInt_canonicalize(dst); // Enforce contracts, ESPECAILLY Contract 3
    /* Fast Paths */
    // Since they're equal, and due to Contract 3
    //  ------> They're not subjected to errors if these cases are true
    if (dst->n == 0 && src.n == 0) return BIGINT_SUCCESS;
    if (dst->n == src.n && !memcmp(dst->limbs, src.limbs, src.n * sizeof(uint64_t))) {
        dst->sign = src.sign; return BIGINT_SUCCESS;
    }
    /* Standard Route */
    if (dst->cap < src.n) return BIGINT_ERR_RANGE;
    memcpy(dst->limbs, src.limbs, src.n * sizeof(uint64_t));
    dst->n = src.n; dst->sign = src.sign; return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_tover_copy(bigInt *const dst, bigInt src) {
    test_assert(dst != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(__BIGINT_INTERNAL_SVALID__(dst), bi_storage_inval, clear_arena, BIGINT_ERR_STORE_IN);
    test_assert(bigInt_validate(src), bi_full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (dst->limbs == src.limbs) {
        dst->n = src.n; dst->cap = src.n;
        dst->sign = src.sign; return BIGINT_SUCCESS;
    } bigInt_canonicalize(dst); // Enforce contracts, ESPECAILLY Contract 3
    /* Fast Paths */
    // Since they're equal, and due to Contract 3
    //  ------> They're not subjected to truncation if these cases are true
    if (dst->n == 0 && src.n == 0) return BIGINT_SUCCESS;
    if (dst->n == src.n && !memcmp(dst->limbs, src.limbs, src.n * sizeof(uint64_t))) {
        dst->sign = src.sign;
        return BIGINT_SUCCESS;
    }
    /* Standard Route */
    size_t operation_range = (dst->cap < src.n) ? dst->cap : src.n;
    memcpy(dst->limbs, src.limbs, operation_range * sizeof(uint64_t));
    dst->n = operation_range; dst->sign = src.sign; return BIGINT_SUCCESS;
}
/* -------------  Functional SMALL Copies ------------- */
bigInt bigInt_copyu64(const uint64_t src, dnml_status *err) {
    bigInt dst = {0};
    if (bigInt_new(&dst) == DNML_ALLOC_OOM) func_ret_oom(err)
    if (src) { dst.limbs[0] = src; dst.n = 1; }
    return dst;
}
bigInt bigInt_copyi64(const int64_t src, dnml_status *err) {
    bigInt dst = {0};
    if (bigInt_new(&dst) == DNML_ALLOC_OOM) func_ret_oom(err)
    if (src) {
        dst.limbs[0] = __MAG_I64__(src);
        dst.n = 1; dst.sign = (src < 0) ? -1 : 1;
    } return dst;
}
/* -------------  Functional LARGE Copies ------------- */
bigInt bigInt_copyf128(long double src, dnml_status *err) { return (bigInt){0}; }
bigInt bigInt_ocopyf128(long double src, size_t output_cap, dnml_status *err) { return (bigInt){0}; }
bigInt bigInt_tover_copyf128(long double src, size_t output_cap, dnml_status *err) { return (bigInt){0}; }
bigInt bigInt_copy(bigInt src, dnml_status *err) {
    test_assert_mut(bigInt_validate(src), bi_full_contract, clear_arena, err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__());
    bigInt dst = {0};
    if (src.n == 0) {
        if (bigInt_new(&dst) == DNML_ALLOC_OOM) func_ret_oom(err)
        else return dst;
    }
    if (bigInt_snew(&dst, src.n) == DNML_ALLOC_OOM) func_ret_oom(err);
    memcpy(dst.limbs, src.limbs, src.n * sizeof(uint64_t));
    dst.n = src.n; dst.sign  = src.sign; return dst;
}
bigInt bigInt_ocopy(bigInt src, size_t output_cap, dnml_status *err) {
    test_assert_mut(bigInt_validate(src), bi_full_contract, clear_arena, err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__());
    if (output_cap < src.n) {
        if (err != NULL) *err = BIGINT_ERR_RANGE;
        return __BIGINT_ERROR_VALUE__();
    }
    bigInt dst = {0};
    if (bigInt_snew(&dst, output_cap) == DNML_ALLOC_OOM) func_ret_oom(err);
    memcpy(dst.limbs, src.limbs, src.n * sizeof(uint64_t));
    dst.n = src.n; dst.sign = src.sign;
    *err = BIGINT_SUCCESS; return dst;
}
bigInt bigInt_tover_copy(bigInt src, size_t output_cap, dnml_status *err) {
    test_assert_mut(bigInt_validate(src), bi_full_contract, clear_arena, err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__());
    bigInt dst = {0};
    if (output_cap == 0) { if (bigInt_new(&dst) == DNML_ALLOC_OOM) func_ret_oom(err) }
    else {
        if (bigInt_snew(&dst, output_cap) == DNML_ALLOC_OOM) func_ret_oom(err);
        size_t operation_range = (output_cap < src.n) ? output_cap : src.n;
        memcpy(dst.limbs, src.limbs, operation_range * sizeof(uint64_t));
        dst.n     = operation_range;
        dst.sign  = src.sign;
    }
    return dst;
}




//* ========================================== GENERAL UTILITIES ============================================ */
dnml_status bigInt_canonicalize(bigInt *const x) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    if (x->cap < 1) {
        // Just reset to ensure value safety and certainty
        x->n = 0; x->sign = 1; x->cap = 1;
    }
    if (x->n > x->cap) x->n = x->cap;
    if (x->sign != 1 && x->sign != -1) {
        // Just reset to ensure value safety and certainty
        x->n = 0; x->sign = 1;
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_normalize(bigInt *const x) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    while (x->n > 0 && x->limbs[x->n - 1] == 0) --x->n; // Delete trailing/leading zeros
    if (x->n == 0) x->sign = 1; // Guarantees 0, not -0
    return BIGINT_SUCCESS;
}
dnml_status bigInt_transfer(bigInt *dst, bigInt *src) {
    test_assert(dst != NULL & src != NULL, input_null, clear_arena, BIGINT_NULL);
    bigInt_free(dst); *dst = *src; // Assigning the new src struct header into dst
    src->limbs = NULL; src->n = 0; src->cap = 0; src->sign = 0; // Clearing src's own header
    return BIGINT_SUCCESS;
}
dnml_status bigInt_resize(bigInt *const x, size_t k) { //* Exact Capacity resize
    test_assert(__BIGINT_INTERNAL_SVALID__(x), bi_storage_inval, clear_arena, BIGINT_ERR_STORE_IN);
    k = (!k) ? 1 : k;
    limb_t *__BUFFER_P = realloc(x->limbs, k * sizeof(limb_t));
    if (__BUFFER_P == NULL) return DNML_ALLOC_OOM;
    x->limbs = __BUFFER_P; x->cap = k;
    if (x->n > x->cap) x->n = x->cap;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_reserve(bigInt *const x, size_t k) { //* Minimum Capacity
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(__BIGINT_INTERNAL_SVALID__(x), bi_storage_inval, clear_arena, BIGINT_ERR_STORE_IN);
    if (x->cap >= k) return BIGINT_SUCCESS;
    size_t new_cap = x->cap;
    // Capacity doubles instead of incrementation,
    // ---> Ensure less reallocation ---> Enhanced performance
    while (new_cap < k) new_cap += new_cap;
    limb_t *__BUFFER_P = realloc(x->limbs, new_cap * sizeof(limb_t));
    if (__BUFFER_P == NULL) return DNML_ALLOC_OOM;
    x->limbs = __BUFFER_P; x->cap = new_cap; return BIGINT_SUCCESS;
}
dnml_status bigInt_shrink(bigInt *const x, size_t k) { //* Maximum Capacity
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(__BIGINT_INTERNAL_SVALID__(x), bi_storage_inval, clear_arena, BIGINT_ERR_STORE_IN);
    k = (!k) ? 1 : k; if (x->cap <= k) return BIGINT_SUCCESS;
    limb_t *__BUFFER_P = realloc(x->limbs, k * sizeof(limb_t));
    if (__BUFFER_P == NULL) return DNML_ALLOC_OOM;
    x->limbs = __BUFFER_P; x->cap = k;
    if (x->n < x->cap) x->n = x->cap;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_reset(bigInt *const x) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(__BIGINT_INTERNAL_PVALID__(x), bi_state_contract, clear_arena, BIGINT_ERR_SINVAL);
    if (x->n >= 1) x->limbs[0] = 0; /**/ x->n = 0; x->sign = 1;
    return BIGINT_SUCCESS;
}
bool bigInt_validate(bigInt x) {
    /* State Validation */
    if (x.limbs == NULL) return false;
    if (x.cap < 1) return false;
    if (x.n > x.cap) return false;
    if (x.sign != 1 && x.sign != -1) return false;
    /* Arithmetic Validation */
    if (x.limbs[x.n - 1] == 0) return false;
    if (x.n == 0 && x.sign != 1) return false;
    return true;
}
bool bigInt_pvalidate(bigInt *const x) {
    DNML_TEST_ASSERT(x != NULL, input_null, clear_arena);
    /* State Validation */
    if (x->limbs == NULL) return false;
    if (x->cap < 1) return false;
    if (x->n > x->cap) return false;
    if (x->sign != 1 && x->sign != -1) return false;
    /* Arithmetic Validation */
    if (x->limbs[x->n - 1] == 0) return false;
    if (x->n == 0 && x->sign != 1) return false;
    return true;
}
