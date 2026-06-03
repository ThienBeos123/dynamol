#include "bigInt_func.h"


//todo ============================================ INTRODUCTION ============================================= */
/* Attribute Explanation:
*   +) sign     (uint8_t)       : Stores the sign (negative or positive)
*   +) limbs    (*uint64_t)     : Pointer to each limb that holds part of the bigInt number
*   +) n        (size_t)        : Number of currently used limbs
*   +) cap      (size_t)        : Essentially the bigInt object/number's maximum capacity 
* /

/* Terms Explanation:
*   +) Mutative Model: Changes an existing bigInt object/variable value in place (Eg: x += 10; )
*   +) Functional Model: 
*       -) Creates a new variable with the value of the expression (Eg: int x = 5 + 10; ) 
*       -) This transfer ownership of the allocated limbs to the caller, 
*          forcing a manual deletion of the object
*/

/* Global, Thread-local Arena */
static local_thread dnml_arena ___DASI_NUMERIC_ARENA_;
static local_thread dnml_arena ___DASI_LOWLVL_ARENA_;
static inline dnml_arena* _USE_ARENA(void) {
    /* Support 64 limbs (the gold standard) */
    if (___DASI_NUMERIC_ARENA_.base == NULL) init_arena(&___DASI_NUMERIC_ARENA_, 64);
    return &___DASI_NUMERIC_ARENA_;
} 
static inline dnml_arena* _USE_LOW_ARENA(void) {
    /* Support 64 limbs (the gold standard) */
    if (___DASI_LOWLVL_ARENA_.base == NULL) init_arena(&___DASI_NUMERIC_ARENA_, 64);
    return &___DASI_LOWLVL_ARENA_;
}
dnml_status _init_dynamol_bigint(void) {
    test_assert(
        init_arena(&___DASI_NUMERIC_ARENA_, 64) != DNML_ALLOC_OOM, arena_oom, { 
        arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_);
    }, DNML_ALLOC_OOM);
    test_assert(
        init_arena(&___DASI_LOWLVL_ARENA_, 64) != DNML_ALLOC_OOM, arena_oom, { 
        arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_);
        arena_clear(&___DASI_LOWLVL_ARENA_); arena_destruct(&___DASI_LOWLVL_ARENA_);
    }, DNML_ALLOC_OOM);
}


//* ======================================== CONSTRUCTORS & DESTRUCTOR ======================================= */
void bigInt_free(bigInt *x) {
    if (x->limbs == NULL) return;
    free(x->limbs); x->limbs = NULL;
    x->n = 0; x->cap = 0; x->sign = 0;
}
dnml_status bigInt_new(bigInt *x) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    if (x->limbs) return BIGINT_SUCCESS; // The bigInt is already initialized
    limb_t *P_BUFFER__ = malloc(sizeof(limb_t));
    if (P_BUFFER__) return DNML_ALLOC_OOM;
    x->limbs = P_BUFFER__;
    x->cap = 1; x->n = 0; x->sign = 1;
}
dnml_status bigInt_snew(bigInt *x, size_t n) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    if (x->limbs != NULL) return BIGINT_SUCCESS; // already initialized
    limb_t *__BUFFER_P = malloc(n * sizeof(limb_t));
    if (__BUFFER_P) return DNML_ALLOC_OOM;
    x->limbs = __BUFFER_P;
    x->cap = n; x->n = 0; x->sign = 1;
}
dnml_status bigInt_binew(bigInt *x, const bigInt *y) {
    test_assert(x != NULL | y != NULL, input_null, {}, BIGINT_NULL);
    if (x->limbs != NULL) return BIGINT_SUCCESS; // Already Initialized
    test_assert(bigInt_pvalidate(y), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (x->limbs == y->limbs) {
        if (x == y) return BIGINT_SUCCESS; 
        x->n = y->n; x->cap = y->cap; x->sign = y->sign;
    } size_t alloc_size = (y->n) ? y->n : 1;
    limb_t *__BUFFER_P = malloc(alloc_size * sizeof(limb_t));
    if (__BUFFER_P == NULL) return DNML_ALLOC_OOM;
    x->limbs = __BUFFER_P;
    if (y->n) memcpy(x->limbs, y->limbs, y->n * sizeof(limb_t));
    x->n = y->n; x->cap = alloc_size; x->sign = (y->n) ? y->sign : 1;
}
dnml_status bigInt_new_u64(bigInt *x, const uint64_t in) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    if (x->limbs != NULL) return BIGINT_SUCCESS; // ALREADY INITIALIZED
    limb_t *__BUFFER_P = malloc(sizeof(limb_t));
    if (__BUFFER_P == NULL) return DNML_ALLOC_OOM;
    x->limbs = __BUFFER_P; x->limbs[0] = in;
    x->n = !!(in); x->cap = 1; x->sign = 1;
}
dnml_status bigInt_new_i64(bigInt *x, const int64_t in) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    if (x->limbs != NULL) return BIGINT_SUCCESS; // ALREADY INITIALIZED
    limb_t *__BUFFER_P = malloc(sizeof(limb_t));
    if (__BUFFER_P) return DNML_ALLOC_OOM;
    x->limbs = __BUFFER_P; x->limbs[0] = __MAG_I64__(in);
    x->n = !!(in); x->cap = 1; x->sign = (in < 0) ? -1 : 1;
}
dnml_status bigInt_new_f128(bigInt *x, long double in) { return FILE_ILLEGAL; /* Placeholder */  }




//* =============================================== ASSIGNMENTS ============================================== */
dnml_status bigInt_set(const bigInt x, bigInt *receiver) {
    test_assert(receiver != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_validate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    test_assert(__BIGINT_INTERNAL_PVALID__(receiver), state_contract, clear_arena, BIGINT_ERR_SINVAL);
    if (receiver->limbs = x.limbs) {
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
dnml_status bigInt_set_safe(const bigInt x, bigInt *receiver) {
    test_assert(receiver != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_validate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    test_assert(__BIGINT_INTERNAL_PVALID__(receiver), state_contract, clear_arena, BIGINT_ERR_SINVAL);
    if (receiver->limbs = x.limbs) {
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
dnml_status bigInt_setu64(const bigInt x, uint64_t* receiver) {
    test_assert(receiver != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_validate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    *receiver = (x.n) ? x.limbs[0] : 0; return BIGINT_SUCCESS;
}
dnml_status bigInt_seti64(const bigInt x, int64_t* receiver) {
    test_assert(receiver != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_validate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    uint64_t raw_u64 = (x.n) ? x.limbs[0] : 0;
    uint64_t abs_int64_min = (uint64_t)(llabs(INT64_MIN + 1)) + 1;
    if (raw_u64 > abs_int64_min && x.sign == -1) *receiver = (int64_t)(raw_u64 & I64_MIN_BIT_MASK) * x.sign;
    else if (raw_u64 > INT64_MAX && x.sign == 1) *receiver = (int64_t)(raw_u64 & I64_MAX_BIT_MASK) * x.sign;
    else *receiver = ((int64_t)raw_u64) * x.sign;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_setf128(const bigInt x, long double* receiver) {}
dnml_status bigInt_setu64_safe(const bigInt x, uint64_t* receiver) {
    test_assert(receiver != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_validate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (x.sign == -1 || x.n > 1) return BIGINT_ERR_RANGE;
    *receiver = (x.n) ? x.limbs[0] : 0;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_seti64_safe(const bigInt x, int64_t* receiver) {
    test_assert(receiver != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_validate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (x.n > 1) BIGINT_ERR_RANGE;
    uint64_t raw_u64 = (x.n) ? x.limbs[0] : 0;
    uint64_t abs_int64_min = (uint64_t)(llabs(INT64_MIN + 1)) + 1;
    if (raw_u64 > abs_int64_min && x.sign == -1) return BIGINT_ERR_RANGE;
    if (raw_u64 > INT64_MAX && x.sign == 1) return BIGINT_ERR_RANGE;
    *receiver = ((int64_t)raw_u64) * x.sign;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_setf128_safe(const bigInt x, long double* receiver) {}
/* --------- Primitive Types --> BigInt --------- */
dnml_status bigInt_getu64(const uint64_t val, bigInt *receiver) {
    test_assert(receiver != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(__BIGINT_INTERNAL_PVALID__(receiver), state_contract, clear_arena, BIGINT_ERR_SINVAL);
    receiver->limbs[0] = val; receiver->n = !!(val); receiver->sign = 1;
}
dnml_status bigInt_geti64(const int64_t val, bigInt *receiver) {
    test_assert(receiver != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(__BIGINT_INTERNAL_PVALID__(receiver), state_contract, clear_arena, BIGINT_ERR_SINVAL);
    uint64_t abs_val = __MAG_I64__(val);
    receiver->limbs[0] = abs_val;
    receiver->n        = (val) ? 1 : 0;
    receiver->sign     = (val < 0) ? -1 : 1;
}
dnml_status bigInt_getf128(long double x, bigInt *receiver) {}
dnml_status bigInt_getf128_safe(long double x, bigInt *receiver) {}




//* =============================================== CONVERSIONS ============================================== */
/* --------- BigInt --> Primitive Types --------- */
uint64_t bigInt_tou64(const bigInt x, dnml_status *err) {
    if (!bigInt_validate(x)) mut_gret(err, BIGINT_ERR_INVAL, 0);
    uint64_t res = (x.n) ? x.limbs[0] : 0;
    return res;
}
int64_t bigInt_toi64(const bigInt x, dnml_status *err) {
    if (!bigInt_validate(x)) mut_gret(err, BIGINT_ERR_INVAL, 0);
    int64_t res; uint64_t raw_u64 = (x.n) ? x.limbs[0] : 0;
    uint64_t abs_int64_min = (uint64_t)(llabs(INT64_MIN + 1)) + 1;
    if (raw_u64 > abs_int64_min && x.sign == -1) res = (int64_t)(raw_u64 & I64_MIN_BIT_MASK) * x.sign;
    else if (raw_u64 > INT64_MAX && x.sign == 1) res = (int64_t)(raw_u64 & I64_MAX_BIT_MASK) * x.sign;
    else res = ((int64_t)raw_u64) * x.sign;
    return res;
}
long double bigInt_tof128(const bigInt x, dnml_status *err) {}
uint64_t bigInt_tou64_safe(const bigInt x, dnml_status *err) {
    if (!bigInt_validate(x)) mut_gret(err, BIGINT_ERR_INVAL, 0);
    if (x.sign == -1 || x.n > 1) mut_gret(err, BIGINT_ERR_RANGE, 0);
    uint64_t res = (x.n) ? x.limbs[0] : 0;
    *err = BIGINT_SUCCESS; return res;
}
int64_t bigInt_toi64_safe(const bigInt x, dnml_status *err) {
    if (!bigInt_validate(x)) mut_gret(err, BIGINT_ERR_INVAL, 0);
    if (x.n > 1) mut_gret(err, BIGINT_ERR_RANGE, INT64_MIN);
    uint64_t raw_u64 = (x.n) ? x.limbs[0] : 0;
    uint64_t abs_int64_min = (uint64_t)(llabs(INT64_MIN + 1)) + 1;
    if (raw_u64 > abs_int64_min && x.sign == -1) mut_gret(err, BIGINT_ERR_RANGE, INT64_MIN);
    if (raw_u64 > INT64_MAX && x.sign == 1) mut_gret(err, BIGINT_ERR_RANGE, INT64_MIN);
    int64_t res = ((int64_t)raw_u64) * x.sign;
    *err = BIGINT_SUCCESS; return res;
}
long double bigInt_tof128_safe(const bigInt x, dnml_status *err) {}
/* --------- Primitive Types --> BigInt --------- */
bigInt bigInt_fromu64(const uint64_t x, dnml_status *err) {
    bigInt res; if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err)
    res.limbs[0] = x; res.n = !!(x); *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_fromi64(const int64_t x, dnml_status *err) {
    bigInt res; 
    if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err)
    res.limbs[0] = __MAG_I64__(x);
    res.n = !!(x); res.sign = (x < 0) ? -1 : 1;
    *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_fromf128(long double x, dnml_status *err) { return (bigInt){0}; }
bigInt bigInt_fromf128_safe(long double x, dnml_status *err) { return (bigInt){0}; }




//* =========================================== BITWISE OPERATIONS =========================================== */
bigInt bigInt_not(const bigInt x, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res; if (bigInt_snew(&res, x.n) == DNML_ALLOC_OOM) func_ret_oom(err)
    for (size_t i = 0; i < x.n; ++i) {
        res.limbs[i] = ~x.limbs[i];
    } res.n = x.n; res.sign = x.sign;
    bigInt_normalize(&res); return res;
}
bigInt bigInt_rshift(const bigInt x, size_t k, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    size_t limb_shift = k / U64_BITS, bshift = k % U64_BITS;
    uint64_t discarded_bits = 0; bigInt res;
    if (bigInt_snew(&res, x.n) == DNML_ALLOC_OOM) func_ret_oom(err);
    memcpy(res.limbs, x.limbs, x.n * U64_BYTES); res.n = x.n; res.sign = x.sign;
    __BIGINT_INTERNAL_RLSHIFT__(&res, limb_shift); res.n = x.n - limb_shift;
    if (bshift) for (size_t i = res.n - 1; i != -1; --i) {
        uint64_t positioned_bits = discarded_bits << (U64_BITS - bshift);
        res.limbs[i] = (x.limbs[i] >> bshift) | positioned_bits;
        discarded_bits = x.limbs[i] & ((UINT64_C(1) << bshift) - 1);
    } res.sign = x.sign; bigInt_normalize(&res); return res;
}
bigInt bigInt_lshift(const bigInt x, size_t k, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    size_t limb_shift = k / U64_BITS, bshift = k % U64_BITS;
    uint64_t discarded_bits = 0; bigInt res;
    if (bigInt_snew(&res, x.n) == DNML_ALLOC_OOM) func_ret_oom(err);
    memcpy(res.limbs, x.limbs, x.n * U64_BYTES); res.n = x.n; res.sign = x.sign; 
    __BIGINT_INTERNAL_LLSHIFT__(&res, limb_shift);
    if (bshift) for (size_t i = limb_shift; i < x.n; ++i) {
        res.limbs[i] = (x.limbs[i] << bshift) | discarded_bits;
        uint64_t iso_mask = (UINT64_C(1) << bshift) - 1;
        discarded_bits = x.limbs[i] & (iso_mask << U64_BITS - bshift);
    } bigInt_normalize(&res); *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_lshiftg(const bigInt x, size_t k, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    uint64_t discarded_bits = 0; bigInt res;
    size_t limb_shift = k / U64_BITS, bshift = k % U64_BITS;
    size_t alloc_cap = (x.n + limb_shift + !!(bshift));
    if (bigInt_snew(&res, alloc_cap) == DNML_ALLOC_OOM) func_ret_oom(err);
    for (size_t i = 0; i < x.n; ++i) res.limbs[i + limb_shift] = x.limbs[i];
    res.n = alloc_cap; res.sign = x.sign;
    if (bshift) for (size_t i = limb_shift; i < res.n; ++i) {
        uint64_t new_carry = res.limbs[i] >> (U64_BITS - bshift);
        res.limbs[i] = (res.limbs[i] << bshift) | discarded_bits;
        discarded_bits = new_carry;
    } bigInt_normalize(&res); *err = BIGINT_SUCCESS; return res;
}
dnml_status bigInt_mut_not(bigInt *x) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    for (size_t i = 0; i < x->n; ++i) x->limbs[i] = ~(x->limbs[i]);
    bigInt_normalize(x);
}
dnml_status bigInt_mut_rshift(bigInt *x, size_t k) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    uint64_t discarded_bits = 0;
    size_t limb_shift = k / U64_BITS, bshift = k % U64_BITS;
    __BIGINT_INTERNAL_RLSHIFT__(x, limb_shift); x->n -= limb_shift;
    if (bshift) for (size_t i = x->n - 1; i != -1; --i) {
        uint64_t positioned_bits = discarded_bits << (U64_BITS - bshift);
        discarded_bits = x->limbs[i] & ((UINT64_C(1) << bshift) - 1);
        x->limbs[i] = (x->limbs[i] >> bshift) | positioned_bits;
    } bigInt_normalize(x);
}
dnml_status bigInt_mut_lshift(bigInt *x, size_t k) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    size_t limb_shift = k / U64_BITS, bshift = k % U64_BITS;
    uint64_t discarded_bits = 0; __BIGINT_INTERNAL_LLSHIFT__(x, limb_shift);
    if (bshift) for (size_t i = limb_shift; i < x->n; ++i) {
        uint64_t previous_dbits = discarded_bits;
        uint64_t iso_mask = (UINT64_C(1) << bshift) - 1;
        discarded_bits = x->limbs[i] & (iso_mask << U64_BITS - bshift);
        x->limbs[i] = (x->limbs[i] << bshift) | previous_dbits;
    } bigInt_normalize(x);
}
dnml_status bigInt_mut_lshiftg(bigInt *x, size_t k) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    uint64_t discarded_bits = 0;
    size_t limb_shift = k / U64_BITS, bshift = k % U64_BITS;
    size_t alloc_cap = (x->n + limb_shift + !!(bshift));
    if (bigInt_reserve(x, alloc_cap) == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    __BIGINT_INTERNAL_LLSHIFT__(x, limb_shift); x->n = alloc_cap;
    if (bshift) for (size_t i = limb_shift; i < x->n; ++i) {
        uint64_t new_carry = x->limbs[i] >> (U64_BITS - bshift);
        x->limbs[i] = (x->limbs[i] << bshift) | discarded_bits;
        discarded_bits = new_carry;
    } bigInt_normalize(x); return BIGINT_SUCCESS;
}
/* ------------- Mutative, Fixed-width ------------- */
dnml_status bigInt_mut_andu64  (bigInt *x, const uint64_t val) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (x->n == 0) return BIGINT_SUCCESS;
    x->limbs[0] = x->limbs[0] & val;
    x->n        = (x->limbs[0]) ? 1 : 0;
    x->sign     = (x->limbs[0]) ? x->sign : 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_nandu64 (bigInt *x, const uint64_t val) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (x->n == 0) { x->limbs[0] = UINT64_MAX; x->n = 1; } 
    else {
        x->limbs[0] = ~(x->limbs[0] & val);
        if (x->n > 1) memset(&x->limbs[1], UINT64_MAX, x->n - 1);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_oru64   (bigInt *x, const uint64_t val) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!x->n) {
        uint64_t res = 0 | val; x->limbs[0] = res;
        x->n = !!(res); x->sign = (res) ? x->sign : 1;
    } else if (val){ 
        x->limbs[0] |= val; // All the other limbs stay the same due to |= 0
    }
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_noru64  (bigInt *x, const uint64_t val) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
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
dnml_status bigInt_mut_xoru64  (bigInt *x, const uint64_t val) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
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
dnml_status bigInt_mut_xnoru64 (bigInt *x, const uint64_t val) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
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
dnml_status bigInt_mut_and  (bigInt *x, const bigInt y) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), full_contract, clear_arena, BIGINT_ERR_INVAL);
    test_assert(x->limbs != y.limbs, aliased_limbs, clear_arena, BIGINT_ERR_ALIASED);
    if (!y.n) bigInt_reset(x);
    else if (x->n) {
        size_t operation_range = max(x->n, y.n);
        dnml_status err_check = bigInt_reserve(x, operation_range);
        heap_alloc_oom(err_check)
        for (size_t i = 0; i < operation_range; ++i) {
            uint64_t a = (i < x->n) ? x->limbs[i] : 0;
            uint64_t b = (i < y.n)  ? y.limbs[i]  : 0;
            x->limbs[i] = a & b;
        }
        x->n = operation_range; bigInt_normalize(x);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_nand (bigInt *x, const bigInt y) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), full_contract, clear_arena, BIGINT_ERR_INVAL);
    test_assert(x->limbs != y.limbs, aliased_limbs, clear_arena, BIGINT_ERR_ALIASED);
    if (!x->n) {
        size_t expanded_cap = (y.n) ? y.n : 1;
        dnml_status err_check = bigInt_reserve(x, expanded_cap);
        heap_alloc_oom(err_check); memset(x->limbs, UINT64_MAX, expanded_cap);
        x->n = expanded_cap;
    } else if (!y.n) memset(x->limbs, UINT64_MAX, x->n);
    else {
        size_t operation_range = max(x->n, y.n);
        dnml_status err_check = bigInt_reserve(x, operation_range);
        heap_alloc_oom(err_check);
        for (size_t i = 0; i < operation_range; ++i) {
            uint64_t a = (i < x->n) ? x->limbs[i] : 0;
            uint64_t b = (i < y.n)  ? y.limbs[i]  : 0;
            x->limbs[i] = ~(a & b);
        }
        x->n = operation_range; bigInt_normalize(x);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_or   (bigInt *x, const bigInt y) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), full_contract, clear_arena, BIGINT_ERR_INVAL);
    test_assert(x->limbs != y.limbs, aliased_limbs, clear_arena, BIGINT_ERR_ALIASED);
    if (!y.n);
    else if (!x->n) { if (bigInt_mut_copy(x, y) == DNML_ALLOC_OOM) return DNML_ALLOC_OOM; }
    else { size_t operation_range = max(x->n, y.n);
        dnml_status err_check = bigInt_reserve(x, operation_range);
        heap_alloc_oom(err_check);
        for (size_t i = 0; i < operation_range; ++i) {
            uint64_t a = (i < x->n) ? x->limbs[i] : 0;
            uint64_t b = (i < y.n)  ? y.limbs[i]  : 0;
            x->limbs[i] = a | b;
        }
        x->n = operation_range; 
        bigInt_normalize(x);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_nor  (bigInt *x, const bigInt y) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), full_contract, clear_arena, BIGINT_ERR_INVAL);
    test_assert(x->limbs != y.limbs, aliased_limbs, clear_arena, BIGINT_ERR_ALIASED);
    if (!x->n && !y.n) {
        x->limbs[0] = UINT64_MAX;
        x->n        = 1;
    } else {
        size_t operation_range = max(x->n, y.n);
        dnml_status err_check = bigInt_reserve(x, operation_range);
        heap_alloc_oom(err_check);
        for (size_t i = 0; i < operation_range; ++i) {
            uint64_t a = (i < x->n) ? x->limbs[i] : 0;
            uint64_t b = (i < y.n) ? y.limbs[i] : 0;
            x->limbs[i] = ~(a | b);
        } 
        x->n = operation_range;
        bigInt_normalize(x);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_xor  (bigInt *x, const bigInt y) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), full_contract, clear_arena, BIGINT_ERR_INVAL);
    test_assert(x->limbs != y.limbs, aliased_limbs, clear_arena, BIGINT_ERR_ALIASED);
    if (!x->n && !y.n);
    else {
        size_t operation_range = max(x->n, y.n);
        dnml_status err_check = bigInt_reserve(x, operation_range);
        heap_alloc_oom(err_check);
        for (size_t i = 0; i < operation_range; ++i) {
            uint64_t a = (i < x->n) ? x->limbs[i] : 0;
            uint64_t b = (i < y.n)  ? y.limbs[i]  : 0;
            x->limbs[i] = a ^ b;
        }
        x->n = operation_range; 
        bigInt_normalize(x);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_xnor (bigInt *x, const bigInt y) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), full_contract, clear_arena, BIGINT_ERR_INVAL);
    test_assert(x->limbs != y.limbs, aliased_limbs, clear_arena, BIGINT_ERR_ALIASED);
    if (!x->n && !y.n) {
        x->limbs[0] = UINT64_MAX;
        x->n        = 1;
    } else {
        size_t operation_range = max(x->n, y.n);
        dnml_status err_check = bigInt_reserve(x, operation_range);
        heap_alloc_oom(err_check);
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
dnml_status bigInt_mutex_andu64  (bigInt *x, const uint64_t val, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status err_check = bigInt_reserve(x, op_range); heap_alloc_oom(err_check);
    if (!x->n) return BIGINT_SUCCESS;
    x->limbs[0] = x->limbs[0] & val;
    x->n        = (x->limbs[0]) ? 1 : 0;
    x->sign     = (x->limbs[0]) ? x->sign : 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mutex_nandu64 (bigInt *x, const uint64_t val, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status err_check = bigInt_reserve(x, op_range); heap_alloc_oom(err_check);
    for (size_t i = 0; i < op_range; ++i) {
        uint64_t a = (i < x->n) ? x->limbs[i] : 0;
        uint64_t b = (i == 0)   ? val         : 0;
        x->limbs[i] = ~(a & b);
    } x->n = max(x->n, op_range); bigInt_normalize(x);
    x->sign = (x->n) ? x->sign : 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mutex_oru64   (bigInt *x, const uint64_t val, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status err_check = bigInt_reserve(x, op_range); heap_alloc_oom(err_check);
    if (!val) return BIGINT_SUCCESS;
    else if (!x->n) {
        uint64_t res = 0 | val;
        x->limbs[0] = res;
        x->n        = (res) ? 1 : 0;
        x->sign     = (res) ? x->sign : 1;
    } else x->limbs[0] |= val; // All the other limbs stay the same due to |= 0
    return BIGINT_SUCCESS;
}
dnml_status bigint_mutex_noru64  (bigInt *x, const uint64_t val, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status err_check = bigInt_reserve(x, op_range); heap_alloc_oom(err_check);
    for (size_t i = 0; i < op_range; ++i) {
        uint64_t a = (i < x->n) ? x->limbs[i] : 0;
        uint64_t b = (i == 0)   ? val         : 0;
        x->limbs[i] = ~(a | b);
    } x->n = max(x->n, op_range); bigInt_normalize(x);
    x->sign = (x->n ) ? x->sign : 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mutex_xoru64  (bigInt *x, const uint64_t val, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
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
dnml_status bigInt_mutex_xnoru64 (bigInt *x, const uint64_t val, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status err_check = bigInt_reserve(x, op_range); heap_alloc_oom(err_check);
    for (size_t i = 0; i < op_range; ++i) {
        uint64_t a = (i < x->n) ? x->limbs[i] : 0;
        uint64_t b = (i == 0)   ? val         : 0;
        x->limbs[i] = ~(a ^ b);
    } x->n = max(x->n, op_range); bigInt_normalize(x);
    x->sign = (x->n) ? x->sign : 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mutex_andi64  (bigInt *x, const int64_t val, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status err_check = bigInt_reserve(x, op_range); heap_alloc_oom(err_check);
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
dnml_status bigInt_mutex_nandi64 (bigInt *x, const int64_t val, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status err_check = bigInt_reserve(x, op_range); heap_alloc_oom(err_check);
    uint64_t extension_bits = (val < 0) ? UINT64_MAX : 0;
    for (size_t i = 0; i < op_range; ++i) {
        uint64_t a = (i < x->n) ? x->limbs[i]       : 0;
        uint64_t b = (i == 0)   ? __MAG_I64__(val)  : extension_bits;
        x->limbs[i] = ~(a & b);
    } x->n = max(x->n, op_range); bigInt_normalize(x);
    x->sign = (x->n) ? x->sign : 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mutex_ori64   (bigInt *x, const int64_t val, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status err_check = bigInt_reserve(x, op_range); heap_alloc_oom(err_check);
    uint64_t extension_bits = (val < 0) ? UINT64_MAX : 0;
    for (size_t i = 0; i < op_range; ++i) {
        uint64_t a = (i < x->n) ? x->limbs[i]       : 0;
        uint64_t b = (i == 0)   ? __MAG_I64__(val)  : extension_bits;
        x->limbs[i] = a | b;
    } x->n = max(x->n, op_range); bigInt_normalize(x);
    x->sign = (x->n) ? x->sign : 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mutex_nori64  (bigInt *x, const int64_t val, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status err_check = bigInt_reserve(x, op_range); heap_alloc_oom(err_check);
    uint64_t extension_bits = (val < 0) ? UINT64_MAX : 0;
    for (size_t i = 0; i < op_range; ++i) {
        uint64_t a = (i < x->n) ? x->limbs[i]       : 0;
        uint64_t b = (i == 0)   ? __MAG_I64__(val)  : extension_bits;
        x->limbs[i] = ~(a | b);
    } x->n = max(x->n, op_range); bigInt_normalize(x);
    x->sign = (x->n) ? x->sign : 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mutex_xori64  (bigInt *x, const int64_t val, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status err_check = bigInt_reserve(x, op_range); heap_alloc_oom(err_check);
    uint64_t extension_bits = (val < 0) ? UINT64_MAX : 0;
    for (size_t i = 0; i < op_range; ++i) {
        uint64_t a = (i < x->n) ? x->limbs[i]       : 0;
        uint64_t b = (i == 0)   ? __MAG_I64__(val)  : extension_bits;
        x->limbs[i] = a ^ b;
    } x->n = max(x->n, op_range); bigInt_normalize(x);
    x->sign = (x->n) ? x->sign : 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mutex_xnori64 (bigInt *x, const int64_t val, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status err_check = bigInt_reserve(x, op_range); heap_alloc_oom(err_check);
    uint64_t extension_bits = (val < 0) ? UINT64_MAX : 0;
    for (size_t i = 0; i < op_range; ++i) {
        uint64_t a = (i < x->n) ? x->limbs[i]       : 0;
        uint64_t b = (i == 0)   ? __MAG_I64__(val)  : extension_bits;
        x->limbs[i] = ~(a ^ b);
    } x->n = max(x->n, op_range); bigInt_normalize(x);
    x->sign = (x->n) ? x->sign : 1;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mutex_and   (bigInt *x, const bigInt y, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), full_contract, clear_arena, BIGINT_ERR_INVAL);
    test_assert(x->limbs != y.limbs, aliased_limbs, clear_arena, BIGINT_ERR_ALIASED);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status err_check = bigInt_reserve(x, op_range); heap_alloc_oom(err_check);
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
dnml_status bigInt_mutex_nand  (bigInt *x, const bigInt y, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), full_contract, clear_arena, BIGINT_ERR_INVAL);
    test_assert(x->limbs != y.limbs, aliased_limbs, clear_arena, BIGINT_ERR_ALIASED);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status err_check = bigInt_reserve(x, op_range); heap_alloc_oom(err_check);
    if (!x->n || !y.n) memset(x->limbs, UINT64_MAX, op_range);
    else {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x->n) ? x->limbs[i] : 0;
            uint64_t b = (i < y.n) ? y.limbs[i] : 0;
            x->limbs[i] = ~(a & b);
        } x->n = max(x->n, op_range); bigInt_normalize(x);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mutex_or    (bigInt *x, const bigInt y, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), full_contract, clear_arena, BIGINT_ERR_INVAL);
    test_assert(x->limbs != y.limbs, aliased_limbs, clear_arena, BIGINT_ERR_ALIASED);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status err_check = bigInt_reserve(x, op_range); heap_alloc_oom(err_check);
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
dnml_status bigInt_mutex_nor   (bigInt *x, const bigInt y, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), full_contract, clear_arena, BIGINT_ERR_INVAL);
    test_assert(x->limbs != y.limbs, aliased_limbs, clear_arena, BIGINT_ERR_ALIASED);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status err_check = bigInt_reserve(x, op_range); heap_alloc_oom(err_check);
    if (!x->n && !y.n) {
        memset(x->limbs, UINT64_MAX, op_range * U64_BYTES);
        x->n = op_range;
    } else {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x->n) ? x->limbs[i] : 0;
            uint64_t b = (i < y.n) ? y.limbs[i] : 0;
            x->limbs[i] = ~(a | b);
        } x->n = max(x->n, op_range); bigInt_normalize(x);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mutex_xor   (bigInt *x, const bigInt y, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), full_contract, clear_arena, BIGINT_ERR_INVAL);
    test_assert(x->limbs != y.limbs, aliased_limbs, clear_arena, BIGINT_ERR_ALIASED);
    if (!op_range) return BIGINT_SUCCESS;
    dnml_status err_check = bigInt_reserve(x, op_range); heap_alloc_oom(err_check);
    if (x->n | y.n) {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x->n) ? x->limbs[i] : 0;
            uint64_t b = (i < y.n) ? y.limbs[i] : 0;
            x->limbs[i] = a ^ b;
        } x->n = max(x->n, op_range); bigInt_normalize(x);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mutex_xnor  (bigInt *x, const bigInt y, size_t op_range) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), full_contract, clear_arena, BIGINT_ERR_INVAL);
    test_assert(x->limbs != y.limbs, aliased_limbs, clear_arena, BIGINT_ERR_ALIASED);
    if (!op_range);
    dnml_status err_check = bigInt_reserve(x, op_range); heap_alloc_oom(err_check);
    if (!x->n && !y.n) {
        memset(x->limbs, UINT64_MAX, op_range * U64_BYTES);
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
bigInt bigInt_andu64  (const bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res; if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (x.n) {
        res.limbs[0] = x.limbs[0] & val;
        res.n        = res.limbs[0] ? 1 : 0;
        res.sign     = res.limbs[0] ? x.sign : 1;
    } return res;
}
bigInt bigInt_nandu64 (const bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res;
    if (bigInt_snew(&res, (x.n) ? x.n : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (x.n == 0) { res.limbs[0] = UINT64_MAX; res.n = 1; } 
    else {
        res.limbs[0] = ~(x.limbs[0] & val);
        if (x.n > 1) memset(&res.limbs[1], UINT64_MAX, x.n - 1);
        res.n = x.n;
    } return res;
}
bigInt bigInt_oru64   (const bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res;
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
bigInt bigInt_noru64  (const bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res; 
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
bigInt bigInt_xoru64  (const bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res;
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
bigInt bigInt_xnoru64 (const bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res;
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
bigInt bigInt_and   (const bigInt x, const bigInt y, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res; if (!x.n || !y.n) if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err)
    else {
        size_t operation_range = max(x.n, y.n);
        if (bigInt_snew(&res, operation_range) == DNML_ALLOC_OOM) func_ret_oom(err);
        for (size_t i = 0; i < operation_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i] : 0;
            uint64_t b = (i < y.n) ? y.limbs[i] : 0;
            res.limbs[i] = a & b;
        } res.n = operation_range; bigInt_normalize(&res);
    } return res;
}
bigInt bigInt_nand  (const bigInt x, const bigInt y, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res;
    if (!x.n || !y.n) {
        size_t max = max(x.n, y.n);
        size_t expanded_cap = max ? max : 1;
        if (bigInt_snew(&res, expanded_cap) == DNML_ALLOC_OOM) func_ret_oom(err);
        memset(res.limbs, UINT64_MAX, expanded_cap);
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
bigInt bigInt_or    (const bigInt x, const bigInt y, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res; if (!y.n) { if (bigInt_binew(&res, &x) == DNML_ALLOC_OOM) func_ret_oom(err); }
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
bigInt bigInt_nor   (const bigInt x, const bigInt y, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res;
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
bigInt bigInt_xor   (const bigInt x, const bigInt y, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res; if (!x.n && !y.n) if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err)
    else {
        size_t operation_range = max(x.n, y.n);
        if (bigInt_snew(&res, operation_range) == DNML_ALLOC_OOM) func_ret_oom(err);
        for (size_t i = 0; i < operation_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i] : 0;
            uint64_t b = (i < y.n) ? y.limbs[i] : 0;
            res.limbs[i] = a ^ b;
        } res.n = operation_range; bigInt_normalize(&res);
    } return res;
}
bigInt bigInt_xnor  (const bigInt x, const bigInt y, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res;
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
bigInt bigInt_ex_andu64  (const bigInt x, const uint64_t val, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (x.n && op_range) {
        res.limbs[0] = x.limbs[0] & val;
        res.n        = !!(res.limbs[0]);
        res.sign     = res.limbs[0] ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_nandu64 (const bigInt x, const uint64_t val, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res;  if (bigInt_snew(&res, op_range) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i == 0) ? val : 0;
            uint64_t b = (i < x.n) ? x.limbs[i] : 0;
            res.limbs[i] = ~(a & b);
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_oru64   (const bigInt x, const uint64_t val, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n)  ? x.limbs[i] : 0;
            uint64_t b = (i == 0)   ? val : 0;
            res.limbs[i] = a | b;
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_noru64  (const bigInt x, const uint64_t val, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n)  ? x.limbs[i] : 0;
            uint64_t b = (i == 0)   ? val : 0;
            res.limbs[i] = ~(a | b);
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_xoru64  (const bigInt x, const uint64_t val, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n)  ? x.limbs[i] : 0;
            uint64_t b = (i == 0)   ? val : 0;
            res.limbs[i] = a ^ b;
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_xnoru64 (const bigInt x, const uint64_t val, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n)  ? x.limbs[i] : 0;
            uint64_t b = (i == 0)   ? val : 0;
            res.limbs[i] = ~(a ^ b);
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_andi64  (const bigInt x, const int64_t val, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range && x.n) {
        uint8_t extension_bits = (val < 0) ? UINT64_MAX : 0;
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i]       : 0;
            uint64_t b = (i == 0)  ? __MAG_I64__(val) : extension_bits;
            res.limbs[i] = a & b;
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_nandi64 (const bigInt x, const int64_t val, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        uint8_t extension_bits = (val < 0) ? UINT64_MAX : 0;
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i]       : 0;
            uint64_t b = (i == 0)  ? __MAG_I64__(val) : extension_bits;
            res.limbs[i] = ~(a & b);
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_ori64   (const bigInt x, const int64_t val, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        uint8_t extension_bits = (val < 0) ? UINT64_MAX : 0;
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i]       : 0;
            uint64_t b = (i == 0)  ? __MAG_I64__(val) : extension_bits;
            res.limbs[i] = a | b;
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_nori64  (const bigInt x, const int64_t val, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        uint8_t extension_bits = (val < 0) ? UINT64_MAX : 0;
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i]       : 0;
            uint64_t b = (i == 0)  ? __MAG_I64__(val) : extension_bits;
            res.limbs[i] = ~(a | b);
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_xori64  (const bigInt x, const int64_t val, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        uint8_t extension_bits = (val < 0) ? UINT64_MAX : 0;
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i]       : 0;
            uint64_t b = (i == 0)  ? __MAG_I64__(val) : extension_bits;
            res.limbs[i] = a ^ b;
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_xnori64 (const bigInt x, const int64_t val, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        uint8_t extension_bits = (val < 0) ? UINT64_MAX : 0;
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i]       : 0;
            uint64_t b = (i == 0)  ? __MAG_I64__(val) : extension_bits;
            res.limbs[i] = ~(a ^ b);
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_and   (const bigInt x, const bigInt y, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i] : 0;
            uint64_t b = (i < y.n) ? y.limbs[i] : 0;
            res.limbs[i] = a & b;
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_nand  (const bigInt x, const bigInt y, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i] : 0;
            uint64_t b = (i < y.n) ? y.limbs[i] : 0;
            res.limbs[i] = ~(a & b);
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_or    (const bigInt x, const bigInt y, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i] : 0;
            uint64_t b = (i < y.n) ? y.limbs[i] : 0;
            res.limbs[i] = a | b;
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_nor   (const bigInt x, const bigInt y, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i] : 0;
            uint64_t b = (i < y.n) ? y.limbs[i] : 0;
            res.limbs[i] = ~(a | b);
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_xor   (const bigInt x, const bigInt y, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res; if (bigInt_snew(&res, (op_range) ? op_range : 1) == DNML_ALLOC_OOM) func_ret_oom(err);
    if (op_range) {
        for (size_t i = 0; i < op_range; ++i) {
            uint64_t a = (i < x.n) ? x.limbs[i] : 0;
            uint64_t b = (i < y.n) ? y.limbs[i] : 0;
            res.limbs[i] = a ^ b;
        } res.n = op_range; bigInt_normalize(&res);
        res.sign = (res.n) ? x.sign : 1;
    } *err = BIGINT_SUCCESS; return res;
}
bigInt bigInt_ex_xnor  (const bigInt x, const bigInt y, size_t op_range, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res; if (bigInt_snew(&res, (op_range) ? op_range : 1)  == DNML_ALLOC_OOM) func_ret_oom(err);
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
static int8_t __BIGINT_MAGCOMP_UI64__(const bigInt *x, const uint64_t val) {
    DNML_TEST_ASSERT(x != NULL, input_null, {});
    if (x->n > 1) return 1;
    if (x->limbs[0] > val) return 1;
    else if (x->limbs[0] < val) return -1;
    return 0;
}
static int8_t __BIGINT_MAGCOMP__(const bigInt *a, const bigInt *b) {
    test_assert(a != NULL && b != NULL, input_null, {}, BIGINT_NULL);
    if (a->n != b->n) return (a->n > b->n) ? 1 : -1;
    // Loops from most-significant digit down to least-significant digit
    for (size_t i = a->n - 1; i != -1; --i) {
        if (a->limbs[i] != b->limbs[i]) return (a->limbs[i] > b->limbs[i]) ? 1 : -1; 
        // Compare which one current most-significant digit is bigger
    }
    return 0;
}
/* --------------- Integer - I64 --------------- */
bool bigInt_equal_i64(const bigInt x, const int64_t val, dnml_status *err) {
    test_assert_mut(bigInt_validate(x), full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (x.n == 0) return (val) ? false : true;
    int8_t val_sign = (val < 0) ? -1 : 1;
    if (val_sign != x.sign) return false;
    if (x.n > 1) return false;
    return x.limbs[0] == __MAG_I64__(val);
}
bool bigInt_less_i64(const bigInt x, const int64_t val, dnml_status *err) { 
    test_assert_mut(bigInt_validate(x), full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (x.n == 0) return (val > 0) ? true : false;
    int8_t val_sign = (val < 0) ? -1 : 1;
    if (val_sign != x.sign) return (x.sign < val_sign);
    if (x.n > 1) return (x.sign == -1);
    if (x.limbs[0] > __MAG_I64__(val)) return (x.sign == -1);
    return (x.limbs[0] < __MAG_I64__(val)) && (x.sign == 1);
}
bool bigInt_more_i64(const bigInt x, const int64_t val, dnml_status *err) {
    test_assert_mut(bigInt_validate(x), full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (x.n == 0) return (val < 0) ? true : false;
    int8_t val_sign = (val < 0) ? -1 : 1;
    if (val_sign != x.sign) return (x.sign > val_sign);
    if (x.n > 1) return (x.sign >= val_sign);
    if (x.limbs[0] < __MAG_I64__(val)) return (x.sign == -1);
    return (x.limbs[0] > __MAG_I64__(val)) && (x.sign == 1);
}
bool bigInt_lequal_i64(const bigInt x, const int64_t val, dnml_status *err) {
    test_assert_mut(bigInt_validate(x), full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (x.n == 0) return (val >= 0) ? true : false;
    int8_t val_sign = (val < 0) ? -1 : 1;
    if (x.sign != val_sign) return (x.sign < val_sign);
    if (x.n > 1) return (x.sign == -1);
    // Case eg: 189 > 171  |  -189 < -171
    if (x.limbs[0] > __MAG_I64__(val)) return (x.sign == -1);
    return (x.sign == 1); // Case eg: 178 < 181  |   -178 > -181
}
bool bigInt_mequal_i64(const bigInt x, const int64_t val, dnml_status *err) {
    test_assert_mut(bigInt_validate(x), full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (x.n == 0) return (val <= 0) ? true : false;
    int8_t val_sign = (val < 0) ? -1 : 1;
    if (x.sign != val_sign) return (x.sign > val_sign);
    if (x.n > 1) return (x.sign == 1);
    // Case eg: 189 > 171  |  -189 < -171
    if (x.limbs[0] > __MAG_I64__(val)) return (x.sign == 1);
    return (x.sign == -1); // Case eg: 178 < 181  |   -178 > -181
}
/* ---------- Unsigned Integer - UI64 ---------- */
bool bigInt_equal_u64(const bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(bigInt_validate(x), full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (x.n == 0) return (val) ? false : true;
    if (x.sign == -1) return false;
    if (x.n > 1) return false;
    return (x.limbs[0] == val);
}
bool bigInt_less_u64(const bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(bigInt_validate(x), full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (x.n == 0) return (val) ? true : false;
    if (x.sign == -1) return true;
    if (x.n > 1) return false;
    return (x.limbs[0] < val);
}
bool bigInt_more_u64(const bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(bigInt_validate(x), full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (x.n == 0) return (val) ? false : true;
    if (x.sign == -1) return false;
    if (x.n > 1) return true;
    return (x.limbs[0] > val);
}
bool bigInt_lequal_u64(const bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(bigInt_validate(x), full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (x.n == 0) return true; // Always true, as every R+ numbers are always >= 0
    if (x.sign == -1) return true;
    if (x.n > 1) return false;
    return (x.limbs[0] <= val);
}
bool bigInt_mequal_u64(const bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(bigInt_validate(x), full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (x.n == 0) return (val) ? false : true;
    if (x.sign == -1) return false;
    if (x.n > 1) return true;
    return (x.limbs[0] >= val);
}
/* ------------------- BigInt ------------------ */
bool bigInt_equal(const bigInt a, const bigInt b, dnml_status *err) {
    test_assert_mut(bigInt_validate(a) && bigInt_validate(b), full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (!a.n) return (!b.n) ? true : false;
    if (a.sign != b.sign) return false;
    if (a.n != b.n) return false;
    if (a.limbs == b.limbs) return true;
    return __BIGINT_MAGCOMP__(&a, &b) == 0;
}
bool bigInt_less(const bigInt a, const bigInt b, dnml_status *err) {
    test_assert_mut(bigInt_validate(a) && bigInt_validate(b), full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (a.sign != b.sign) return (a.sign < b.sign);
    if (a.n != b.n) return (a.sign == 1) ? (a.n < b.n) : (a.n > b.n);
    if (a.limbs == b.limbs) return false;
    return (a.sign == 1) ? __BIGINT_MAGCOMP__(&a, &b) < 0 : __BIGINT_MAGCOMP__(&a, &b) > 0;
}
bool bigInt_more(const bigInt a, const bigInt b, dnml_status *err) {
    test_assert_mut(bigInt_validate(a) && bigInt_validate(b), full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (a.sign != b.sign) return (a.sign > b.sign);
    if (a.n    != b.n)    return (a.sign == 1) ? (a.n > b.n) : (a.n < b.n);
    if (a.limbs == b.limbs) return false;
    return (a.sign == 1) ? __BIGINT_MAGCOMP__(&a, &b) > 0 : __BIGINT_MAGCOMP__(&a, &b) < 0;
}
bool bigInt_lequal(const bigInt a, const bigInt b, dnml_status *err) {
    test_assert_mut(bigInt_validate(a) && bigInt_validate(b), full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (a.sign != b.sign) return (a.sign < b.sign);
    if (a.n != b.n) return (a.sign == 1) ? (a.n < b.n) : (a.n > b.n);
    if (a.limbs == b.limbs) return true;
    return (a.sign == 1) ? __BIGINT_MAGCOMP__(&a, &b) <= 0 : __BIGINT_MAGCOMP__(&a, &b) >= 0;
}
bool bigInt_mequal(const bigInt a, const bigInt b, dnml_status *err) {
    test_assert_mut(bigInt_validate(a) && bigInt_validate(b), full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (a.sign != b.sign) return (a.sign > b.sign);
    if (a.n != b.n) return (a.sign == 1) ? (a.n > b.n) : (a.n < b.n);
    if (a.limbs == b.limbs) return true;
    return (a.sign == 1) ? __BIGINT_MAGCOMP__(&a, &b) >= 0 : __BIGINT_MAGCOMP__(&a, &b) <= 0;
}





//* ========================================= MAGNITUDE MATHEMATICA ========================================== *//
/* -------------------- MAGNITUDED ARITHMETIC --------------------- */
static void __BIGINT_MAGADD__(bigInt *res, const bigInt *a, const bigInt *b, dnml_status *err) {
    size_t max = max(a->n, b->n);
    // Set the minimum capacity of res to be 1 bigger
    // than the largest capacity between a & b ----> res->cap = max + 1
    dnml_status err_check = bigInt_reserve(res, max + 1); heap_alloc_oom_void(err_check, err);
    uint64_t carry = 0;
    for (size_t i = 0; i < max; ++i) {
        uint64_t x = (i < a->n) ? a->limbs[i] : 0; // Assigning limb at position i of a to x
        uint64_t y = (i < b->n) ? b->limbs[i] : 0; // Assigning limb at position i of b to x
        uint8_t u8_carry = (uint8_t)carry;
        res->limbs[i] = __ADD_UI64__(x, y, &u8_carry); // Do single-limb addition with carry (if have) --> Stores the carry
    }
    if (carry) res->limbs[max] = carry; // If carry still needed ---> stores the carry in the (res->cap - 1) limb
    res->n = max + (carry != 0); *err = BIGINT_SUCCESS;
}
static void __BIGINT_MAGSUB__(bigInt *res, const bigInt *a, const bigInt *b, dnml_status *err) {
    dnml_status err_check = bigInt_reserve(res, a->n); heap_alloc_oom_void(err_check, err);
    uint64_t borrow = 0;
    for (size_t i = 0; i < a->n; ++i) {
        uint64_t y = (i < b->n) ? b->limbs[i] : 0;
        uint8_t u8_borrow = (uint8_t)borrow;
        res->limbs[i] = __SUB_UI64__(a->limbs[i], y, &u8_borrow);
        // Do single-limb subtraction with borrow ---> Stores the borrow
    } res->n = a->n;
}
static void __BIGINT_MAGMUL__(bigInt *res, const bigInt *a, const bigInt *b, dnml_status *err) {
    dnml_arena *_DASI_MAGMUL_ARENA = _USE_LOW_ARENA();
    test_assert_mut(
        /* Static Analysis - Assert Parameters */
        (!(_DASI_MAGMUL_ARENA->poisoined)), alloc_oom, {
            arena_clear(_DASI_MAGMUL_ARENA); arena_destruct(_DASI_MAGMUL_ARENA);
            arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_);
        }, err, DNML_ALLOC_OOM, ; /* Error Returns Parameters */
    )
    size_t needed_size = __BIGINT_MUL_WS__(a->n, b->n);
    if (_DASI_MAGMUL_ARENA->cap < needed_size) {
        dnml_status err_check = arena_grow(_DASI_MAGMUL_ARENA, needed_size);
        test_assert_mut(
            /* Static Analysis - Assert Parameters */
            (err_check != DNML_ALLOC_OOM), alloc_oom, {
                arena_clear(_DASI_MAGMUL_ARENA); arena_destruct(_DASI_MAGMUL_ARENA);
                arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_);
            }, err, DNML_ALLOC_OOM, ; /* Error Returns Parameters */
        );
    }
    calc_ctx magmul_ctx = {
        .alloc = &arena_alloc_adapter,
        .mark = &arena_mark_adapter,
        .reset = &arena_reset_adapter,
        .clear = &arena_clear_adapter,
        .destruct = &arena_destruct_adapter,
        .state = _DASI_MAGMUL_ARENA
    }; __BIGINT_MUL_DISPATCH__(a, b, res, magmul_ctx);
}
static void __BIGINT_MAGDIV__(bigInt *quot, bigInt *tmp_rem, const bigInt *a, const bigInt *b, dnml_status *err) {
    dnml_arena *_DASI_MAGDIV_ARENA = _USE_LOW_ARENA();
    test_assert_mut(
        /* Static Analysis - Assert Parameters */
        (!(_DASI_MAGDIV_ARENA->poisoined)), alloc_oom, {
            arena_clear(_DASI_MAGDIV_ARENA); arena_destruct(_DASI_MAGDIV_ARENA);
            arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_);
        }, err, DNML_ALLOC_OOM, ; /* Error Returns Parameters */
    )
    size_t needed_size = __BIGINT_DIV_WS__(a->n, b->n) + b->n;
    if (_DASI_MAGDIV_ARENA->cap < needed_size) {
        dnml_status err_check = arena_grow(_DASI_MAGDIV_ARENA, needed_size);
        test_assert_mut(
            /* Static Analysis - Assert Parameters */
            (err_check != DNML_ALLOC_OOM), alloc_oom, {
                arena_clear(_DASI_MAGDIV_ARENA); arena_destruct(_DASI_MAGDIV_ARENA);
                arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_);
            }, err, DNML_ALLOC_OOM, ; /* Error Returns Parameters */
        )
    }
    calc_ctx magdivmod_ctx = {
        .alloc = &arena_alloc_adapter, .mark = &arena_mark_adapter,
        .reset = &arena_reset_adapter, .clear = &arena_clear_adapter,
        .destruct = &arena_destruct_adapter, .state = _DASI_MAGDIV_ARENA
    };
    __BIGINT_DIV_DISPATCH__(a, b, quot, tmp_rem, magdivmod_ctx);
}
static void __BIGINT_MAGMOD__(bigInt *rem, bigInt *tmp_quot, const bigInt *a, const bigInt *b, dnml_status *err) {
    dnml_arena *_DASI_MAGDIV_ARENA = _USE_LOW_ARENA();
    test_assert_mut(
        /* Static Analysis - Assert Parameters */
        (!(_DASI_MAGDIV_ARENA->poisoined)), alloc_oom, {
            arena_clear(_DASI_MAGDIV_ARENA); arena_destruct(_DASI_MAGDIV_ARENA);
            arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_);
        }, err, DNML_ALLOC_OOM, ; /* Error Returns Parameters */
    )
    size_t needed_size = __BIGINT_MOD_WS__(a->n, b->n) + a->n;
    if (_DASI_MAGDIV_ARENA->cap < needed_size) {
        dnml_status err_check = arena_grow(_DASI_MAGDIV_ARENA, needed_size);
        test_assert_mut(
            /* Static Analysis - Assert Parameters */
            (err_check != DNML_ALLOC_OOM), alloc_oom, {
                arena_clear(_DASI_MAGDIV_ARENA); arena_destruct(_DASI_MAGDIV_ARENA);
                arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_);
            }, err, DNML_ALLOC_OOM, ; /* Error Returns Parameters */
        )
    }
    calc_ctx magdivmod_ctx = {
        .alloc = &arena_alloc_adapter,
        .mark = &arena_mark_adapter,
        .reset = &arena_reset_adapter,
        .clear = &arena_clear_adapter,
        .destruct = &arena_destruct_adapter,
        .state = _DASI_MAGDIV_ARENA
    };
    __BIGINT_MOD_DISPATCH__(a, b, rem, tmp_quot, magdivmod_ctx);
}
static void __BIGINT_MAGMUL_U64__(bigInt *res, const bigInt *x, const uint64_t val, dnml_status *err) {
    // Since the divisor size is small (n <= 1), we implement schoolbook multiplication
    dnml_status err_check = bigInt_reserve(res, x->n + 1); 
    heap_alloc_oom_void(err_check, err); uint64_t carry = 0;
    for (size_t i = 0; i < x->n; ++i) {
        uint64_t low, high;
        low = __MUL_UI64__(x->limbs[i], val, &high);
        uint64_t sum = low + carry;
        carry = high + (sum < low) + (sum < carry);
        res->limbs[i] = sum;
    } res->n = x->n;
    if (carry) { res->limbs[res->n++] = carry; }
}
static void __BIGINT_MAGDIVMOD_U64__(
    bigInt *quot, uint64_t* rem, 
    const bigInt *x, const uint64_t val, dnml_status *err
) {
    // Since the divisior size is small (n <= 1), we implement normal/long division
    DNML_TEST_ASSERT(val, "Mathematical Undefinindness: Division by 0 (-Ediv_by_zero)", clear_arena);
    dnml_status err_check = bigInt_reserve(quot, x->n+1); heap_alloc_oom_void(err_check, err);
    quot->n = x->n; uint64_t remainder = 0; uint8_t ovf_test;
    for (size_t i = x->n - 1; i != -1; --i) {
        quot->limbs[i] = __DIV_HELPER_UI64__(remainder, x->limbs[i], val, &remainder, &ovf_test);
        DNML_TEST_ASSERT(ovf_test, "CRITICIAL DEBUG ERROR: Division quotient's overflowed", clear_arena);
    }
    *rem = remainder;
    bigInt_normalize(quot);
}
/* --------------- MAGNITUDED CORE NUMBER-THEORETIC ---------------- */
static inline uint64_t ___GCD_UI64___(const uint64_t a, const uint64_t b) { return __BIGINT_EUCLID__(a, b); }
static void __BIGINT_MAGGCD__(bigInt *res, const bigInt *a, const bigInt *b, dnml_status *err) {
    dnml_arena *_DASI_MAGGCD_ARENA = _USE_LOW_ARENA();
    test_assert_mut(
        /* Static Analysis - Assert Parameters */
        (!(_DASI_MAGGCD_ARENA->poisoined)), alloc_oom, {
            arena_clear(_DASI_MAGGCD_ARENA); arena_destruct(_DASI_MAGGCD_ARENA);
            arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_);
        }, err, DNML_ALLOC_OOM, ; /* Error Returns Parameters */
    )
    size_t needed = __BIGINT_GCD_WS__(a->n, b->n);
    if (_DASI_MAGGCD_ARENA->cap < needed) {
        dnml_status err_check = arena_grow(_DASI_MAGGCD_ARENA, needed);
        test_assert_mut(
            /* Static Analysis - Assert Parameters */
            (err_check != DNML_ALLOC_OOM), alloc_oom, {
                arena_clear(_DASI_MAGGCD_ARENA); arena_destruct(_DASI_MAGGCD_ARENA);
                arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_);
            }, err, DNML_ALLOC_OOM, ; /* Error Returns Parameters */
        )
    }
    calc_ctx _maggcd_ctx = {
        .alloc = &arena_alloc_adapter,
        .mark = &arena_mark_adapter,
        .reset = &arena_reset_adapter,
        .clear = &arena_clear_adapter,
        .destruct = &arena_destruct_adapter,
        .state = _DASI_MAGGCD_ARENA
    }; __BIGINT_GCD_DISPATCH__(res, a, b, _maggcd_ctx);
}
static void __BIGINT_MAGLCM__(bigInt *res, const bigInt *a, const bigInt *b, dnml_status *err) {
    dnml_arena *_DASI_MAGLCM_ARENA = _USE_LOW_ARENA();
    test_assert_mut(
        /* Static Analysis - Assert Parameters */
        (!(_DASI_MAGLCM_ARENA->poisoined)), alloc_oom, {
            arena_clear(_DASI_MAGLCM_ARENA); arena_destruct(_DASI_MAGLCM_ARENA);
            arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_);
        }, err, DNML_ALLOC_OOM, ; /* Error Returns Parameters */
    )
    size_t low_needed = (
        /* THESE CALCULATIONS ARE MOST CERTAINLY THE UPPERBOUND */
        __BIGINT_GCD_WS__(a->n, b->n) + min(a->n, b->n) << 1 + 
        __BIGINT_DIV_WS__(a->n, min(a->n, b->n)) +
        __BIGINT_MUL_WS__(a->n, b->n)
    );
    if (_DASI_MAGLCM_ARENA->cap < low_needed) {
        dnml_status err_check = arena_grow(_DASI_MAGLCM_ARENA, __BIGINT_GCD_WS__(a->n, b->n));
        test_assert_mut(
            /* Static Analysis - Assert Parameters */
            (err_check != DNML_ALLOC_OOM), alloc_oom, {
                arena_clear(_DASI_MAGLCM_ARENA); arena_destruct(_DASI_MAGLCM_ARENA);
                arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_);
            }, err, DNML_ALLOC_OOM, ; /* Error Returns Parameters */
        )
    }
    calc_ctx _maglcm_ctx = {
        .alloc = &arena_alloc_adapter,
        .mark = &arena_mark_adapter,
        .reset = &arena_reset_adapter,
        .clear = &arena_clear_adapter,
        .destruct = &arena_destruct_adapter,
        .state = _DASI_MAGLCM_ARENA
    }; 
    size_t maglcm_mark = arena_mark(_DASI_MAGLCM_ARENA); dnml_status tmp_check;
    limb_t *gcdres_limbs = arena_alloc(_DASI_MAGLCM_ARENA, min(a->n, b->n), &tmp_check);
    limb_t *tmp_limbs = arena_alloc(_DASI_MAGLCM_ARENA, min(a->n, b->n), &tmp_check);
    limb_t *tmpq_limbs = arena_alloc(_DASI_MAGLCM_ARENA, a->n, &tmp_check);
    bigInt gcd_res = { .limbs = gcdres_limbs, /**/ .n = 0, /**/ .cap = min(a->n, b->n), .sign = 1 }; 
    bigInt temp_rem = { .limbs = tmp_limbs, /**/ .n = 0, /**/ .cap = min(a->n, b->n), .sign = 1 };
    bigInt temp_quot = { .limbs = tmpq_limbs, /**/ .n = 0, /**/ .cap = a->n, .sign = 1 };
    __BIGINT_GCD_DISPATCH__(&gcd_res, a, b, _maglcm_ctx);
    __BIGINT_DIV_DISPATCH__(a, &gcd_res, &temp_quot, &temp_rem, _maglcm_ctx);
    __BIGINT_MUL_DISPATCH__(&temp_quot, b, &gcd_res, _maglcm_ctx);
    tmp_check = bigInt_mut_ocopy(res, gcd_res); ocopy_check(tmp_check, _DASI_MAGLCM_ARENA);
    arena_reset(_DASI_MAGLCM_ARENA, maglcm_mark);
}
static void __BIGINT_MAGEMOD_U64__(uint64_t* res, const bigInt *a, const uint64_t mod) {
    uint64_t curr_rem = 0; uint8_t ovf_test;
    for (size_t i = a->n - 1; i != -1; --i) {
        uint64_t tmp_quot = __DIV_HELPER_UI64__(a->limbs[i], curr_rem, mod, &curr_rem, &ovf_test);
        DNML_TEST_ASSERT(ovf_test, "CRITICIAL DEBUG ERROR: Division quotient's overflowed", clear_arena);
    } *res = curr_rem;
}
static void __BIGINT_MAGEMOD__(bigInt *res, const bigInt *a, const bigInt *mod, dnml_status *err) {
    dnml_arena *_DASI_MAGEMOD_ARENA = _USE_LOW_ARENA();
    test_assert_mut(
        /* Static Analysis - Assert Parameters */
        (!(_DASI_MAGEMOD_ARENA->poisoined)), alloc_oom, {
            arena_clear(_DASI_MAGEMOD_ARENA); arena_destruct(_DASI_MAGEMOD_ARENA);
            arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_);
        }, err, DNML_ALLOC_OOM, ; /* Error Returns Parameters */
    )
    size_t needed = __BIGINT_MOD_WS__(a->n, mod->n) + a->n;
    if (_DASI_MAGEMOD_ARENA->cap < needed) {
        dnml_status err_check = arena_grow(_DASI_MAGEMOD_ARENA, needed);
        test_assert_mut(
            /* Static Analysis - Assert Parameters */
            (err_check != DNML_ALLOC_OOM), alloc_oom, {
                arena_clear(_DASI_MAGEMOD_ARENA); arena_destruct(_DASI_MAGEMOD_ARENA);
                arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_);
            }, err, DNML_ALLOC_OOM, ; /* Error Returns Parameters */
        )
    }
    calc_ctx magemod_ctx = {
        .alloc = &arena_alloc_adapter,
        .mark = &arena_mark_adapter,
        .reset = &arena_reset_adapter,
        .clear = &arena_clear_adapter,
        .destruct = &arena_destruct_adapter,
        .state = _DASI_MAGEMOD_ARENA
    }; dnml_status tmp_check;
    size_t tmp_mark = arena_mark(_DASI_MAGEMOD_ARENA);
    limb_t *tmp_limbs = arena_alloc(_DASI_MAGEMOD_ARENA, a->n, &tmp_check);
    bigInt tmp_quot = { .limbs = tmp_limbs, /**/ .n = 0, /**/ .cap = a->n, /**/ .sign = 1 };
    __BIGINT_MOD_DISPATCH__(a, mod, res, &tmp_quot, magemod_ctx);
    arena_reset(_DASI_MAGEMOD_ARENA, tmp_mark); tmp_limbs == NULL;
}
/* ----------------- MAGNITUDED MODULAR-ARITHMETIC ------------------ */
static void __BIGINT_MAGMODADD__(bigInt *res, const bigInt *a, const bigInt *b, const bigInt *mod) {}
static void __BIGINT_MAGMODSUB__(bigInt *res, const bigInt *a, const bigInt *b, const bigInt *mod) {}
static void __BIGINT_MAGMODMUL__(bigInt *res, const bigInt *a, const bigInt *b, const bigInt *mod) {}
static void __BIGINT_MAGMODDIV__(bigInt *res, const bigInt *a, const bigInt *b, const bigInt *mod) {}
static void __BIGINT_MAGMODEXP__(bigInt *res, const bigInt *a, const bigInt *b, const bigInt *mod) {}
static void __BIGINT_MAGMODSQR__(bigInt *res, const bigInt *a, const bigInt *b, const bigInt *mod) {}
static void __BIGINT_MAGMODINV__(bigInt *res, const bigInt *a, const bigInt *b, const bigInt *mod) {}
/* ----------------- MAGNITUDED ALGEBRAIC OPERATIONS ------------------ */
static void __BIGINT_MAGSQR__(bigInt *res, const bigInt *base, dnml_status *err) {
    dnml_arena *_DASI_MAGSQR_ARENA = _USE_LOW_ARENA();
    test_assert_mut(
        /* Static Analysis - Assert Parameters */
        (!(_DASI_MAGSQR_ARENA->poisoined)), alloc_oom, {
            arena_clear(_DASI_MAGSQR_ARENA); arena_destruct(_DASI_MAGSQR_ARENA);
            arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_);
        }, err, DNML_ALLOC_OOM, ; /* Error Returns Parameters */
    )
    size_t needed = __BIGINT_MUL_WS__(base->n, base->n);
    if (_DASI_MAGSQR_ARENA->cap < needed) {
        dnml_status err_check = arena_grow(_DASI_MAGSQR_ARENA, needed);
        test_assert_mut(
            /* Static Analysis - Assert Parameters */
            (err_check != DNML_ALLOC_OOM), alloc_oom, {
                arena_clear(_DASI_MAGSQR_ARENA); arena_destruct(_DASI_MAGSQR_ARENA);
                arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_);
            }, err, DNML_ALLOC_OOM, ; /* Error Returns Parameters */
        )
    }
    calc_ctx magsqr_ctx = {
        .alloc = &arena_alloc_adapter,
        .mark = &arena_mark_adapter,
        .reset = &arena_reset_adapter,
        .clear = &arena_clear_adapter,
        .destruct = &arena_destruct_adapter,
        .state  = _DASI_MAGSQR_ARENA
    }; __BIGINT_MUL_DISPATCH__(base, base, res, magsqr_ctx);
}
static void __BIGINT_MAGPOW__(bigInt *res, const bigInt *base, const uint64_t pow, dnml_status *err) {
    dnml_arena *_DASI_MAGPOW_ARENA = _USE_LOW_ARENA();
    test_assert_mut(
        /* Static Analysis - Assert Parameters */
        (!(_DASI_MAGPOW_ARENA->poisoined)), alloc_oom, {
            arena_clear(_DASI_MAGPOW_ARENA); arena_destruct(_DASI_MAGPOW_ARENA);
            arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_);
        }, err, DNML_ALLOC_OOM, ; /* Error Returns Parameters */
    )
    size_t needed = __BIGINT_EXP_WS__(base->n, pow);
    if (_DASI_MAGPOW_ARENA->cap < needed) {
        dnml_status err_check = arena_grow(_DASI_MAGPOW_ARENA, needed);
        test_assert_mut(
            /* Static Analysis - Assert Parameters */
            (err_check != DNML_ALLOC_OOM), alloc_oom, {
                arena_clear(_DASI_MAGPOW_ARENA); arena_destruct(_DASI_MAGPOW_ARENA);
                arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_);
            }, err, DNML_ALLOC_OOM, ; /* Error Returns Parameters */
        )
    }
    calc_ctx magpow_ctx = {
        .alloc = &arena_alloc_adapter,
        .mark = &arena_mark_adapter,
        .reset = &arena_reset_adapter,
        .clear = &arena_clear_adapter,
        .destruct = &arena_destruct_adapter,
        .state  = _DASI_MAGPOW_ARENA
    }; __BIGINT_EXP_DISPATCH__(res, base, pow, magpow_ctx);
}
static void __BIGINT_MAGSQRT__(bigInt *res, const bigInt *a, dnml_status *err) {
    dnml_arena *_DASI_MAGSQRT_ARENA = _USE_LOW_ARENA();
    test_assert_mut(
        /* Static Analysis - Assert Parameters */
        (!(_DASI_MAGSQRT_ARENA->poisoined)), alloc_oom, {
            arena_clear(_DASI_MAGSQRT_ARENA); arena_destruct(_DASI_MAGSQRT_ARENA);
            arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_);
        }, err, DNML_ALLOC_OOM, ; /* Error Returns Parameters */
    )
    size_t needed = __BIGINT_SQRT_WS__(a->n);
    if (_DASI_MAGSQRT_ARENA->cap < needed) {
        dnml_status err_check = arena_grow(_DASI_MAGSQRT_ARENA, needed);
        test_assert_mut(
            /* Static Analysis - Assert Parameters */
            (err_check != DNML_ALLOC_OOM), alloc_oom, {
                arena_clear(_DASI_MAGSQRT_ARENA); arena_destruct(_DASI_MAGSQRT_ARENA);
                arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_);
            }, err, DNML_ALLOC_OOM, ; /* Error Returns Parameters */
        )
    }
    calc_ctx magsqrt_ctx = {
        .alloc = &arena_alloc_adapter,
        .mark = &arena_mark_adapter,
        .reset = &arena_reset_adapter,
        .clear = &arena_clear_adapter,
        .destruct = &arena_destruct_adapter,
        .state  = _DASI_MAGSQRT_ARENA
    }; __BIGINT_SQRT_DISPATCH__(res, a, magsqrt_ctx);
}
static void __BIGINT_MAGCBRT__(bigInt *res, const bigInt *a, dnml_status *err) {
    dnml_arena *_DASI_MAGCBRT_ARENA = _USE_LOW_ARENA();
    test_assert_mut(
        /* Static Analysis - Assert Parameters */
        (!(_DASI_MAGCBRT_ARENA->poisoined)), alloc_oom, {
            arena_clear(_DASI_MAGCBRT_ARENA); arena_destruct(_DASI_MAGCBRT_ARENA);
            arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_);
        }, err, DNML_ALLOC_OOM, ; /* Error Returns Parameters */
    )
    size_t needed = __BIGINT_CBRT_WS__(a->n);
    if (_DASI_MAGCBRT_ARENA->cap < needed) {
        dnml_status err_check = arena_grow(_DASI_MAGCBRT_ARENA, needed);
        test_assert_mut(
            /* Static Analysis - Assert Parameters */
            (err_check != DNML_ALLOC_OOM), alloc_oom, {
                arena_clear(_DASI_MAGCBRT_ARENA); arena_destruct(_DASI_MAGCBRT_ARENA);
                arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_);
            }, err, DNML_ALLOC_OOM, ; /* Error Returns Parameters */
        )
    }
    calc_ctx magcbrt_ctx = {
        .alloc = &arena_alloc_adapter,
        .mark = &arena_mark_adapter,
        .reset = &arena_reset_adapter,
        .clear = &arena_clear_adapter,
        .destruct = &arena_destruct_adapter,
        .state  = _DASI_MAGCBRT_ARENA
    }; __BIGINT_CBRT_DISPATCH__(res, a, magcbrt_ctx);
}
static void __BIGINT_MAGNRT__(bigInt *res, const bigInt *a, const uint64_t root, dnml_status *err) {
    dnml_arena *_DASI_MAG_NROOT_ARENA = _USE_LOW_ARENA();
    test_assert_mut(
        /* Static Analysis - Assert Parameters */
        (!(_DASI_MAG_NROOT_ARENA->poisoined)), alloc_oom, {
            arena_clear(_DASI_MAG_NROOT_ARENA); arena_destruct(_DASI_MAG_NROOT_ARENA);
            arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_);
        }, err, DNML_ALLOC_OOM, ; /* Error Returns Parameters */
    )
    size_t needed = __BIGINT_NROOT_WS__(a->n, root);
    if (_DASI_MAG_NROOT_ARENA->cap < needed) {
        dnml_status err_check = arena_grow(_DASI_MAG_NROOT_ARENA, needed);
        test_assert_mut(
            /* Static Analysis - Assert Parameters */
            (err_check != DNML_ALLOC_OOM), alloc_oom, {
                arena_clear(_DASI_MAG_NROOT_ARENA); arena_destruct(_DASI_MAG_NROOT_ARENA);
                arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_);
            }, err, DNML_ALLOC_OOM, ; /* Error Returns Parameters */
        )
    }
    calc_ctx mag_nroot_ctx = {
        .alloc = &arena_alloc_adapter,
        .mark = &arena_mark_adapter,
        .reset = &arena_reset_adapter,
        .clear = &arena_clear_adapter,
        .destruct = &arena_destruct_adapter,
        .state  = _DASI_MAG_NROOT_ARENA
    }; __BIGINT_NROOT_DISPATCH__(res, a, root, mag_nroot_ctx);
}





//* ============================================ SIGNED ARITHMETIC ========================================== */
/* ------------------- MUTATIVE ARITHMETIC -------------------- */
dnml_status bigInt_mut_mulu64(bigInt *x, const uint64_t val) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (x->n == 0);
    else if (val == 1);
    else if (!val) bigInt_reset(x);
    else if (x->n == 1 && x->limbs[0] == 1) bigInt_mut_copyu64(x, val);
    else { dnml_status err_check = bigInt_reserve(x, x->n + 1); heap_alloc_oom(err_check);
        dnml_arena *_DASI_MUL_UI64_ARENA = _USE_ARENA(); arena_poisoined(_DASI_MUL_UI64_ARENA);
        size_t tmp_mark = arena_mark(_DASI_MUL_UI64_ARENA);
        limb_t *tmp_limbs = arena_galloc(_DASI_MUL_UI64_ARENA, x->n + 1, &err_check); 
        arena_alloc_oom(err_check, _DASI_MUL_UI64_ARENA);
        
        bigInt tmp_prod = { .limbs = tmp_limbs, .cap = x->n + 1, .n = 0, .sign = 1 }; 
        __BIGINT_MAGMUL_U64__(&tmp_prod, x, val, &err_check); arena_alloc_oom(err_check, _DASI_MUL_UI64_ARENA);
        err_check = bigInt_mut_ocopy(x, tmp_prod); ocopy_check(err_check, _DASI_MUL_UI64_ARENA);
        arena_reset(_DASI_MUL_UI64_ARENA, tmp_mark); _DASI_MUL_UI64_ARENA = NULL;
    }
    return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_divu64(bigInt *x, const uint64_t val) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!val) return BIGINT_ERR_DOMAIN;
    if (x->n == 1 && x->limbs[0] == 1) bigInt_reset(x);
    else if (x->n && val != 1) { dnml_arena *_DASI_DIV_UI64_ARENA = _USE_ARENA();
        arena_poisoined(_DASI_DIV_UI64_ARENA); dnml_status err_check;
        size_t tmp_mark = arena_mark(_DASI_DIV_UI64_ARENA);
        limb_t *tmp_limbs = arena_galloc(_DASI_DIV_UI64_ARENA, x->n, &err_check);
        arena_alloc_oom(err_check, _DASI_DIV_UI64_ARENA);

        bigInt temp_quot = { .limbs = tmp_limbs, .cap = x->n,  .n = 0, .sign = 1 }; 
        uint64_t temp_rem; __BIGINT_MAGDIVMOD_U64__(&temp_quot, &temp_rem, x, val, &err_check);
        arena_alloc_oom(err_check, _DASI_DIV_UI64_ARENA); temp_quot.sign = x->sign; bigInt_normalize(&temp_quot);
        err_check = bigInt_mut_ocopy(x, temp_quot); ocopy_check(err_check, _DASI_DIV_UI64_ARENA);
        arena_reset(_DASI_DIV_UI64_ARENA, tmp_mark); _DASI_DIV_UI64_ARENA = NULL;
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_modu64(bigInt *x, const uint64_t val) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!val) return BIGINT_ERR_DOMAIN;
    if (x->n == 0);
    else if (val == 1) bigInt_reset(x);
    else {
        int8_t comp_res = __BIGINT_MAGCOMP_UI64__(x, val);
        if (comp_res < 0);
        else if (!comp_res) bigInt_reset(x);
        else { dnml_arena *_DASI_MOD_UI64_ARENA = _USE_ARENA();
            arena_poisoined(_DASI_MOD_UI64_ARENA); dnml_status err_check;
            size_t tmp_mark = arena_mark(_DASI_MOD_UI64_ARENA);
            limb_t *tmp_limbs = arena_galloc(_DASI_MOD_UI64_ARENA, x->n, &err_check);
            arena_alloc_oom(err_check, _DASI_MOD_UI64_ARENA);

            bigInt temp_quot = { .limbs = tmp_limbs, .cap = x->n, .n = 0, .sign = 1 };
            uint64_t temp_rem; __BIGINT_MAGDIVMOD_U64__(&temp_quot, &temp_rem, x, val, &err_check);
            arena_alloc_oom(err_check, _DASI_MOD_UI64_ARENA); x->limbs[0] = temp_rem;
            x->n = !!(temp_rem); x->sign = (temp_rem) ? x->sign : 1;
            arena_reset(_DASI_MOD_UI64_ARENA, tmp_mark); _DASI_MOD_UI64_ARENA = NULL;
        }
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_muli64(bigInt *x, const int64_t val) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (x->n == 0) return BIGINT_SUCCESS;
    if (!val) bigInt_reset(x);
    else if (val == 1 || val == -1);
    else if (x->n == 1 && x->limbs[0] == 1) bigInt_mut_copyi64(x, val);
    else { dnml_status err_check = bigInt_reserve(x, x->n + 1); heap_alloc_oom(err_check);
        dnml_arena *_DASI_MUL_I64_ARENA = _USE_ARENA(); arena_poisoined(_DASI_MUL_I64_ARENA);

        size_t tmp_mark = arena_mark(_DASI_MUL_I64_ARENA);
        limb_t *tmp_limbs = arena_galloc(_DASI_MUL_I64_ARENA,  x->n + 1, &err_check); arena_alloc_oom(err_check, _DASI_MUL_I64_ARENA);
        bigInt tmp_prod = { .limbs = tmp_limbs, .cap = x->n + 1, .n = 0, .sign = 1 }; uint64_t mag_val = __MAG_I64__(val);
        __BIGINT_MAGMUL_U64__(&tmp_prod, x, mag_val, &err_check); arena_alloc_oom(err_check, _DASI_MUL_I64_ARENA);
        err_check = bigInt_mut_ocopy(x, tmp_prod); ocopy_check(err_check, _DASI_MUL_I64_ARENA);
        arena_reset(_DASI_MUL_I64_ARENA, tmp_mark); _DASI_MUL_I64_ARENA = NULL;
    } x->sign *= (val < 0) ? -1 : 1;
}
dnml_status bigInt_mut_divi64(bigInt *x, const int64_t val) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!val) return BIGINT_ERR_DOMAIN;
    if (val == 1 || val == -1) x->sign *= val;
    else if (x->n == 1 && x->limbs[0] == 1) bigInt_reset(x);
    else if (x->n) { 
        dnml_arena *_DASI_DIV_I64_ARENA = _USE_ARENA(); arena_poisoined(_DASI_DIV_I64_ARENA);
        size_t tmp_mark = arena_mark(_DASI_DIV_I64_ARENA); dnml_status err_check;
        limb_t *tmp_limbs = arena_galloc(_DASI_DIV_I64_ARENA, x->n, &err_check); 
        arena_alloc_oom(err_check, _DASI_DIV_I64_ARENA);

        uint64_t temp_rem, mag_val = __MAG_I64__(val); 
        bigInt temp_quot = { .limbs = tmp_limbs, .cap = x->n, .n = 0, .sign = 1 };
        __BIGINT_MAGDIVMOD_U64__(&temp_quot, &temp_rem, x, mag_val, &err_check); 
        arena_alloc_oom(err_check, _DASI_DIV_I64_ARENA);
        temp_quot.sign = x->sign * ((val < 0) ? -1 : 1); bigInt_normalize(&temp_quot); 
        err_check = bigInt_mut_ocopy(x, temp_quot); ocopy_check(err_check, _DASI_DIV_I64_ARENA);
        arena_reset(_DASI_DIV_I64_ARENA, tmp_mark); _DASI_DIV_I64_ARENA = NULL;
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_modi64(bigInt *x, const int64_t val) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!val) return BIGINT_ERR_DOMAIN;
    if (val == 1 || val == -1) bigInt_reset(x);
    else if (x->n) {
        uint64_t mag_val = __MAG_I64__(val);
        int8_t comp_res = __BIGINT_MAGCOMP_UI64__(x, mag_val);
        if (comp_res < 0);
        else if (!comp_res) bigInt_reset(x);
        else { 
            dnml_arena *_DASI_MOD_I64_ARENA = _USE_ARENA(); arena_poisoined(_DASI_MOD_I64_ARENA);
            size_t tmp_mark = arena_mark(_DASI_MOD_I64_ARENA); dnml_status err_check;
            limb_t *tmp_limbs = arena_galloc(_DASI_MOD_I64_ARENA, x->n, &err_check); arena_alloc_oom(err_check, _DASI_MOD_I64_ARENA);
            bigInt temp_quot = { .limbs = tmp_limbs, .cap = x->n, .n = 0, .sign = 1 }; uint64_t temp_rem; 
            __BIGINT_MAGDIVMOD_U64__(&temp_quot, &temp_rem, x, mag_val, &err_check);
            arena_alloc_oom(err_check, _DASI_MOD_I64_ARENA); x->limbs[0] = temp_rem;
            x->n = !!(temp_rem); x->sign = (temp_rem) ? x->sign : 1;
            arena_reset(_DASI_MOD_I64_ARENA, tmp_mark); _DASI_MOD_I64_ARENA = NULL;
        }
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_add(bigInt *x, const bigInt y) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!y.n);
    else if (!x->n) { if (bigInt_mut_copy(x, y) == DNML_ALLOC_OOM) return DNML_ALLOC_OOM; }
    else if (x->sign == y.sign) {
        dnml_status err_check = bigInt_reserve(x, max(x->n, y.n) + 1); heap_alloc_oom(err_check);
        dnml_arena *_DASI_ADD_ARENA = _USE_ARENA(); arena_poisoined(_DASI_ADD_ARENA);
        size_t tmp_mark = arena_mark(_DASI_ADD_ARENA);
        limb_t *tmp_limbs = arena_galloc(_DASI_ADD_ARENA, max(x->n, y.n) + 1, &err_check);
        arena_alloc_oom(err_check, _DASI_ADD_ARENA);

        bigInt temp_sum = { .limbs = tmp_limbs, .cap = max(x->n, y.n) + 1, .n = 0, .sign = 1 };
        __BIGINT_MAGADD__(&temp_sum, x, &y, &err_check); arena_alloc_oom(err_check, _DASI_ADD_ARENA); 
        temp_sum.sign = x->sign; err_check = bigInt_mut_ocopy(x, temp_sum); ocopy_check(err_check, _DASI_ADD_ARENA);
        arena_reset(_DASI_ADD_ARENA, tmp_mark); _DASI_ADD_ARENA = NULL;
    } else {
        int8_t comp_res = __BIGINT_MAGCOMP__(x, &y);
        if (!comp_res) bigInt_reset(x);
        else { dnml_arena *_DASI_ADD_ARENA = _USE_ARENA(); arena_poisoined(_DASI_ADD_ARENA);
            size_t tmp_mark = arena_mark(_DASI_ADD_ARENA); dnml_status err_check;
            limb_t *tmp_limbs = arena_galloc(_DASI_ADD_ARENA,  x->n, &err_check);
            arena_alloc_oom(err_check, _DASI_ADD_ARENA);

            bigInt temp_sum = { .limbs = tmp_limbs, .cap = x->n, .n = 0, .sign = 1 };
            if (comp_res > 0) { 
                __BIGINT_MAGSUB__(&temp_sum, x, &y, &err_check);
                arena_alloc_oom(err_check, _DASI_ADD_ARENA); temp_sum.sign = x->sign; 
            } else {
                __BIGINT_MAGSUB__(&temp_sum, &y, x, &err_check); 
                arena_alloc_oom(err_check, _DASI_ADD_ARENA); temp_sum.sign = y.sign;
            } err_check = bigInt_mut_ocopy(x, temp_sum); ocopy_check(err_check, _DASI_ADD_ARENA);
            arena_reset(_DASI_ADD_ARENA, tmp_mark); _DASI_ADD_ARENA = NULL;
        }
    }
}
dnml_status bigInt_mut_sub(bigInt *x, const bigInt y) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!y.n) return BIGINT_SUCCESS;
    else if (!x->n) { if (bigInt_mut_copy(x, y) == DNML_ALLOC_OOM) return DNML_ALLOC_OOM; x->sign = -y.sign; }
    else if (x->sign == y.sign) {
        int8_t comp_res = __BIGINT_MAGCOMP__(x, &y);
        if (!comp_res) bigInt_reset(x);
        else {
            dnml_status err_check = bigInt_reserve(x, max(x->n, y.n) + 1); heap_alloc_oom(err_check);
            dnml_arena *_DASI_SUB_ARENA = _USE_ARENA(); arena_poisoined(_DASI_SUB_ARENA);
            size_t tmp_mark = arena_mark(_DASI_SUB_ARENA);
            limb_t *tmp_limbs = arena_galloc(_DASI_SUB_ARENA,  x->n, &err_check);
            arena_alloc_oom(err_check, _DASI_SUB_ARENA);

            bigInt temp_diff = { .limbs = tmp_limbs, .cap = x->n, .n = 0, .sign = 1 };
            if (comp_res > 0) { 
                __BIGINT_MAGSUB__(&temp_diff, x, &y, &err_check);
                arena_alloc_oom(err_check, _DASI_SUB_ARENA); temp_diff.sign = x->sign;
            } else {
                __BIGINT_MAGSUB__(&temp_diff, &y, x, &err_check);
                arena_alloc_oom(err_check, _DASI_SUB_ARENA); temp_diff.sign = -x->sign; 
            } err_check = bigInt_mut_ocopy(x, temp_diff); ocopy_check(err_check, _DASI_SUB_ARENA);
            arena_reset(_DASI_SUB_ARENA, tmp_mark); _DASI_SUB_ARENA = NULL;
        }
    } else { 
        dnml_arena *_DASI_SUB_ARENA = _USE_ARENA(); arena_poisoined(_DASI_SUB_ARENA);
        size_t tmp_mark = arena_mark(_DASI_SUB_ARENA); dnml_status err_check;
        limb_t *tmp_limbs = arena_galloc(_DASI_SUB_ARENA, max(x->n, y.n) + 1, &err_check);
        arena_alloc_oom(err_check, _DASI_SUB_ARENA);

        bigInt temp_diff = { .limbs = tmp_limbs, .cap = max(x->n, y.n) + 1, .n = 0, .sign = 1 };
        __BIGINT_MAGADD__(&temp_diff, x, &y, &err_check); arena_alloc_oom(err_check, _DASI_SUB_ARENA);
        temp_diff.sign = x->sign; err_check = bigInt_mut_ocopy(x, temp_diff); ocopy_check(err_check, _DASI_SUB_ARENA);
        arena_reset(_DASI_SUB_ARENA, tmp_mark); _DASI_SUB_ARENA = NULL;
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_mul(bigInt *x, const bigInt y) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (x->n == 0) return BIGINT_SUCCESS;
    else if (y.n == 1 && y.limbs[0] == 1) return BIGINT_SUCCESS;
    else if (!y.n) bigInt_reset(x);
    else if (x->n == 1 && x->limbs[0] == 1) {
        if (bigInt_mut_copy(x, y) == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    }
    else { dnml_status err_check = bigInt_reserve(x, x->n + y.n); heap_alloc_oom(err_check);
        dnml_arena *_DASI_MUL_ARENA = _USE_ARENA(); arena_poisoined(_DASI_MUL_ARENA);

        size_t tmp_mark = arena_mark(_DASI_MUL_ARENA);
        limb_t *tmp_limbs = arena_galloc(_DASI_MUL_ARENA, x->n + y.n, &err_check); arena_alloc_oom(err_check, _DASI_MUL_ARENA);
        bigInt tmp_prod = { .limbs = tmp_limbs, .cap = x->n + y.n, .n = 0, .sign = 1 };
        __BIGINT_MAGMUL__(&tmp_prod, x, &y, &err_check); arena_alloc_oom(err_check, _DASI_MUL_ARENA);
        err_check = bigInt_mut_ocopy(x, tmp_prod); ocopy_check(err_check, _DASI_MUL_ARENA);
        arena_reset(_DASI_MUL_ARENA, tmp_mark); _DASI_MUL_ARENA = NULL;
    } x->sign *= y.sign;
}
dnml_status bigInt_mut_div(bigInt *x, const bigInt y) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!y.n) return BIGINT_ERR_DOMAIN;
    if (x->n == 0);
    else if (y.n == 1 && y.limbs[0] == 1) x->sign *= y.sign;
    else if (x->n == 1 && x->limbs[0] == 1) bigInt_reset(x);
    else { dnml_arena *_DASI_DIV_ARENA = _USE_ARENA(); arena_poisoined(_DASI_DIV_ARENA);
        dnml_status err_check = arena_grow(_DASI_DIV_ARENA, x->n + y.n); arena_alloc_oom(err_check, _DASI_DIV_ARENA);

        size_t mutdiv_mark = arena_mark(_DASI_DIV_ARENA);
        limb_t *quot_limbs = arena_alloc(_DASI_DIV_ARENA, x->n, &err_check);
        limb_t *rem_limbs = arena_alloc(_DASI_DIV_ARENA, y.n, &err_check);
        bigInt temp_quot = {.limbs = quot_limbs, .sign = 1,     /**/    .cap = x->n, .n = 0}; 
        bigInt temp_rem = {.limbs = rem_limbs, .sign = 1,       /**/    .cap = y.n,  .n = 0};
        __BIGINT_MAGDIV__(&temp_quot, &temp_rem, x, &y, &err_check); arena_alloc_oom(err_check, _DASI_DIV_ARENA);
        temp_quot.sign = x->sign * y.sign; bigInt_normalize(&temp_quot);
        err_check = bigInt_mut_ocopy(x, temp_quot); ocopy_check(err_check, _DASI_DIV_ARENA);
        arena_reset(_DASI_DIV_ARENA, mutdiv_mark); _DASI_DIV_ARENA = NULL;
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_mod(bigInt *x, const bigInt y) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(y), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!y.n) return BIGINT_ERR_DOMAIN;
    if (x->n == 0);
    else if (y.n == 1 && y.limbs[0] == 1) bigInt_reset(x);
    else {
        int8_t comp_res = __BIGINT_MAGCOMP__(x, &y);
        if (comp_res < 0);
        else if (!comp_res) bigInt_reset(x);
        else { 
            /* It can be proven that x's capacity can sufficiently store temp_rem due to x->n > y->n */
            dnml_arena *_DASI_MOD_ARENA = _USE_ARENA(); arena_poisoined(_DASI_MOD_ARENA);
            dnml_status err_check = arena_grow(_DASI_MOD_ARENA, x->n + y.n); arena_alloc_oom(err_check, _DASI_MOD_ARENA);

            size_t mutmod_mark = arena_mark(_DASI_MOD_ARENA);
            limb_t *quot_limbs = arena_alloc(_DASI_MOD_ARENA, x->n, &err_check);
            limb_t *rem_limbs = arena_alloc(_DASI_MOD_ARENA, y.n, &err_check);
            bigInt temp_quot = {.limbs = quot_limbs, .sign = 1,     /**/    .cap = x->n, .n = 0}; 
            bigInt temp_rem = {.limbs = rem_limbs, .sign = 1,       /**/    .cap = y.n,  .n = 0};
            __BIGINT_MAGMOD__(&temp_rem, &temp_quot, x, &y, &err_check);
            arena_alloc_oom(err_check, _DASI_MOD_ARENA); temp_rem.sign = x->sign; 
            err_check = bigInt_mut_ocopy(x, temp_rem); ocopy_check(err_check, _DASI_MOD_ARENA);
            arena_reset(_DASI_MOD_ARENA, mutmod_mark); _DASI_MOD_ARENA = NULL;
        }
    } return BIGINT_SUCCESS;
}
/* ------------------ FUNCTIONAL ARITHMETIC ------------------- */
bigInt bigInt_mulu64(const bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res; 
    if (!x.n || !val) if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err)
    else if (x.n == 1 && x.limbs[0] == 1) { if (bigInt_new_u64(&res, val) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else if (val == 1) { if (bigInt_binew(&res, &x) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else { 
        /* Standard Case */
        if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err);
        dnml_status err_check; __BIGINT_MAGMUL_U64__(&res, &x, val, &err_check);
        arena_alloc_oom_mut(err_check, &___DASI_NUMERIC_ARENA_, err);
    } res.sign = x.sign; return res;
}
bigInt bigInt_divu64(const bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    if (!val) mut_err(err, BIGINT_ERR_DOMAIN);
    bigInt quot; 
    if (x.n == 0) if (bigInt_new(&quot) == DNML_ALLOC_OOM) func_ret_oom(err)
    else if (val == 1) { if (bigInt_binew(&quot, &x) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else if (x.n == 1 && x.limbs[0]) if (bigInt_new(&quot) == DNML_ALLOC_OOM) func_ret_oom(err)
    else {
        /* Main Case */
        uint64_t temp_rem; if (bigInt_new(&quot) == DNML_ALLOC_OOM) func_ret_oom(err)
        dnml_status err_check; __BIGINT_MAGDIVMOD_U64__(&quot, &temp_rem, &x, val, &err_check);
        arena_alloc_oom_mut(err_check, &___DASI_NUMERIC_ARENA_, err); quot.sign = x.sign; bigInt_normalize(&quot);
    }  *err = BIGINT_SUCCESS; return quot;
}
bigInt bigInt_modu64(const bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    if (!val) mut_err(err, BIGINT_ERR_DOMAIN);
    bigInt rem; 
    if (x.n == 0 || val == 1) if (bigInt_new(&rem) == DNML_ALLOC_OOM) func_ret_oom(err)
    else {
        int8_t comp_res = __BIGINT_MAGCOMP_UI64__(&x, val);
        if (comp_res < 0) { if (bigInt_binew(&rem, &x) == DNML_ALLOC_OOM) func_ret_oom(err); }
        else if (!comp_res) if (bigInt_new(&rem) == DNML_ALLOC_OOM) func_ret_oom(err)
        else {
            if (bigInt_new(&rem) == DNML_ALLOC_OOM) func_ret_oom(err);
            dnml_arena *_DASI_FMOD_UI64_ARENA = _USE_ARENA(); arena_poison_mut(_DASI_FMOD_UI64_ARENA, err);
            size_t tmp_mark = arena_mark(_DASI_FMOD_UI64_ARENA); dnml_status err_check;
            limb_t *tmp_limbs = arena_galloc(_DASI_FMOD_UI64_ARENA, x.n, &err_check);
            arena_alloc_oom_mut(err_check, _DASI_FMOD_UI64_ARENA, err);

            bigInt temp_quot = { .limbs = tmp_limbs, .cap = x.n, .n = 0, .sign = 1 };
            uint64_t temp_rem; __BIGINT_MAGDIVMOD_U64__(&temp_quot, &temp_rem, &x, val, &err_check);
            arena_alloc_oom_mut(err_check, _DASI_FMOD_UI64_ARENA, err); rem.limbs[0] = temp_rem;
            rem.n = !!(temp_rem); rem.sign = (temp_rem) ? x.sign : 1;
            arena_reset(_DASI_FMOD_UI64_ARENA, tmp_mark); _DASI_FMOD_UI64_ARENA = NULL;
        }
    } *err = BIGINT_SUCCESS;
    return rem;
}
bigInt bigInt_muli64(const bigInt x, const int64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res; 
    if (!x.n || !val) if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err)
    else if (x.n == 1 && x.limbs[0] == 1) { if (bigInt_new_i64(&res, val) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else if (val == 1 || val == -1) { 
        if (bigInt_binew(&res, &x) == DNML_ALLOC_OOM) func_ret_oom(err); 
        res.sign = x.sign * val;
    }
    else {
        uint64_t mag_val = __MAG_I64__(val);
        if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err);
        dnml_status err_check; __BIGINT_MAGMUL_U64__(&res, &x, mag_val, &err_check);
        arena_alloc_oom_mut(err_check, &___DASI_NUMERIC_ARENA_, err);
    } res.sign = x.sign * ((val < 0) ? -1 : 1); return res;
}
bigInt bigInt_divi64(const bigInt x, const int64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    if (!val) mut_err(err, BIGINT_ERR_DOMAIN);

    bigInt quot; 
    if (x.n == 0) if (bigInt_new(&quot) == DNML_ALLOC_OOM) func_ret_oom(err)
    else if (val == 1 || val == -1) { 
        if (bigInt_binew(&quot, &x) == DNML_ALLOC_OOM) func_ret_oom(err);
        quot.sign = x.sign * val;
    }
    else if (x.n == 1 && x.limbs[0]) if (bigInt_new(&quot) == DNML_ALLOC_OOM) func_ret_oom(err)
    else {
        /* Main Case */
        dnml_status err_check; uint64_t temp_rem, mag_val = __MAG_I64__(val);
        if (bigInt_new(&quot) == DNML_ALLOC_OOM) func_ret_oom(err)
        __BIGINT_MAGDIVMOD_U64__(&quot, &temp_rem, &x, mag_val, &err_check);
        arena_alloc_oom_mut(err_check, &___DASI_NUMERIC_ARENA_, err);
        quot.sign = x.sign * ((val < 0) ? -1 : 1); bigInt_normalize(&quot);
    } *err = BIGINT_SUCCESS; return quot;
}
bigInt bigInt_modi64(const bigInt x, const int64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    if (!val) mut_err(err, BIGINT_ERR_DOMAIN);

    bigInt rem;
    if (x.n == 0 || val == 1 || val == -1) if (bigInt_new(&rem) == DNML_ALLOC_OOM) func_ret_oom(err)
    else { uint64_t mag_val = __MAG_I64__(val);
        int8_t comp_res = __BIGINT_MAGCOMP_UI64__(&x, mag_val);
        if (comp_res < 0) { if (bigInt_binew(&rem, &x) == DNML_ALLOC_OOM) func_ret_oom(err); }
        else if (!comp_res) if (bigInt_new(&rem) == DNML_ALLOC_OOM) func_ret_oom(err)
        else {
            if (bigInt_new(&rem) == DNML_ALLOC_OOM) func_ret_oom(err)
            dnml_arena *_DASI_FMOD_I64_ARENA = _USE_ARENA(); arena_poison_mut(_DASI_FMOD_I64_ARENA, err);
            size_t tmp_mark = arena_mark(_DASI_FMOD_I64_ARENA); dnml_status err_check;
            limb_t *tmp_limbs = arena_galloc(_DASI_FMOD_I64_ARENA, x.n, &err_check);
            arena_alloc_oom_mut(err_check, _DASI_FMOD_I64_ARENA, err);

            bigInt temp_quot = { .limbs = tmp_limbs, .cap = x.n, .n = 0, .sign = 1 };
            uint64_t temp_rem; __BIGINT_MAGDIVMOD_U64__(&temp_quot, &temp_rem, &x, mag_val, &err_check);
            arena_alloc_oom_mut(err_check, _DASI_FMOD_I64_ARENA, err); rem.limbs[0] = temp_rem;
            rem.n = !!(temp_rem); rem.sign = x.sign; arena_reset(_DASI_FMOD_I64_ARENA, tmp_mark);
            _DASI_FMOD_I64_ARENA = NULL;
        }
    } *err = BIGINT_SUCCESS; return rem;
}
bigInt bigInt_add(const bigInt x, const bigInt y, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt sum;
    if (!y.n) { if (bigInt_binew(&sum, &x) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else if (!x.n) { if (bigInt_binew(&sum, &y) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else if (x.sign == y.sign) {
        if (bigInt_new(&sum) == DNML_ALLOC_OOM) func_ret_oom(err);
        dnml_status err_check; __BIGINT_MAGADD__(&sum, &x, &y, &err_check);
        arena_alloc_oom_mut(err_check, &___DASI_NUMERIC_ARENA_, err); sum.sign = x.sign;
    } else {
        dnml_status err_check;
        int8_t comp_res = __BIGINT_MAGCOMP__(&x, &y);
        if (bigInt_new(&sum) == DNML_ALLOC_OOM) func_ret_oom(err);
        if (comp_res > 0) { 
            __BIGINT_MAGSUB__(&sum, &x, &y, &err_check); 
            arena_alloc_oom_mut(err_check, &___DASI_NUMERIC_ARENA_, err); sum.sign = x.sign; 
        } else if (comp_res < 0) { 
            __BIGINT_MAGSUB__(&sum, &y, &x, &err_check);
            arena_alloc_oom_mut(err_check, &___DASI_NUMERIC_ARENA_, err); sum.sign = y.sign;
        }
    } return sum;
}
bigInt bigInt_sub(const bigInt x, const bigInt y, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt diff;
    if (!y.n) { if (bigInt_binew(&diff, &x) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else if (!x.n) { if (bigInt_binew(&diff, &y) == DNML_ALLOC_OOM) func_ret_oom(err); diff.sign = -y.sign; }
    else if (x.sign == y.sign) {
        dnml_status err_check;
        int8_t comp_res = __BIGINT_MAGCOMP__(&x, &y);
        if (bigInt_new(&diff) == DNML_ALLOC_OOM) func_ret_oom(err)
        if (comp_res > 0) { 
            __BIGINT_MAGSUB__(&diff, &x, &y, &err_check);
            arena_alloc_oom_mut(err_check, &___DASI_NUMERIC_ARENA_, err); diff.sign =  x.sign; 
        } else if (comp_res < 0) {
            __BIGINT_MAGSUB__(&diff, &y, &x, &err_check);
            arena_alloc_oom_mut(err_check, &___DASI_NUMERIC_ARENA_, err); diff.sign = -x.sign;
        }
    } else {
        if (bigInt_new(&diff) == DNML_ALLOC_OOM) func_ret_oom(err)
        dnml_status err_check; __BIGINT_MAGADD__(&diff, &x, &y, &err_check);
        arena_alloc_oom_mut(err_check, &___DASI_NUMERIC_ARENA_, err); diff.sign = x.sign;
    } return diff;
}
bigInt bigInt_mul(const bigInt x, const bigInt y, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res;
    if (!x.n || !y.n) if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err)
    else if (x.n == 1 && x.limbs[0] == 1) { if (bigInt_binew(&res, &y) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else if (y.n == 1 && y.limbs[0] == 1) { if (bigInt_binew(&res, &x) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else {
        if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err)
        dnml_status err_check; __BIGINT_MAGMUL__(&res, &x, &y, &err_check);
        arena_alloc_oom_mut(err_check, &___DASI_NUMERIC_ARENA_, err);
    }
    res.sign = x.sign * y.sign; return res;
}
bigInt bigInt_div(const bigInt x, const bigInt y, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    if (!y.n) mut_err(err, BIGINT_ERR_DOMAIN);

    bigInt quot;
    if (x.n == 0) if (bigInt_new(&quot) == DNML_ALLOC_OOM) func_ret_oom(err)
    else if (y.n == 1 && y.limbs[0] == 1) { 
        if (bigInt_binew(&quot, &x) == DNML_ALLOC_OOM) func_ret_oom(err);  quot.sign *= y.sign; 
    } else if (x.n == 1 && x.limbs[0] == 1) if (bigInt_new(&quot) == DNML_ALLOC_OOM) func_ret_oom(err)
    else {
        if (bigInt_snew(&quot, x.n) == DNML_ALLOC_OOM) func_ret_oom(err)
        dnml_arena *_DASI_FDIV_ARENA = _USE_ARENA(); arena_poison_mut(_DASI_FDIV_ARENA, err);
        size_t tmp_mark = arena_mark(_DASI_FDIV_ARENA); dnml_status err_check;
        limb_t *tmp_limbs = arena_galloc(_DASI_FDIV_ARENA, y.n, &err_check); 
        arena_alloc_oom_mut(err_check, _DASI_FDIV_ARENA, err);

        bigInt temp_rem = { .limbs = tmp_limbs, .cap = y.n, .n = 0, .sign = 1 };
        __BIGINT_MAGDIV__(&quot, &temp_rem, &x, &y, &err_check);
        arena_alloc_oom_mut(err_check, _DASI_FDIV_ARENA, err);
        quot.sign = x.sign * y.sign; bigInt_normalize(&quot);
        arena_reset(_DASI_FDIV_ARENA, tmp_mark); _DASI_FDIV_ARENA = NULL;
    } *err = BIGINT_SUCCESS; 
    return quot;
}
bigInt bigInt_mod(const bigInt x, const bigInt y, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    if (!y.n) mut_err(err, BIGINT_ERR_DOMAIN);
    bigInt rem;
    if (x.n == 0) if (bigInt_new(&rem) == DNML_ALLOC_OOM) func_ret_oom(err)
    else if (y.n == 1 && y.limbs[0] == 1) if (bigInt_new(&rem) == DNML_ALLOC_OOM) func_ret_oom(err)
    else {
        int8_t comp_res = __BIGINT_MAGCOMP__(&x, &y);
        if (comp_res < 0) { if (bigInt_binew(&rem, &x) == DNML_ALLOC_OOM) func_ret_oom(err); }
        else if (!comp_res) if (bigInt_new(&rem) == DNML_ALLOC_OOM) func_ret_oom(err)
        else {
            if (bigInt_snew(&rem, y.n) == DNML_ALLOC_OOM) func_ret_oom(err);
            dnml_arena *_DASI_FMOD_ARENA = _USE_ARENA(); arena_poison_mut(_DASI_FMOD_ARENA, err);
            size_t tmp_mark = arena_mark(_DASI_FMOD_ARENA); dnml_status err_check;
            limb_t *tmp_limbs = arena_galloc(_DASI_FMOD_ARENA, x.n, &err_check);
            arena_alloc_oom_mut(err_check, _DASI_FMOD_ARENA, err);
            bigInt temp_quot = { .limbs = tmp_limbs, .cap = x.n, .n = 0, .sign  = 1 }; 
            __BIGINT_MAGMOD__(&rem, &temp_quot, &x, &y, &err_check);
            arena_alloc_oom_mut(err_check, _DASI_FMOD_ARENA, err);
            rem.sign = x.sign; bigInt_normalize(&rem);
            arena_reset(_DASI_FMOD_ARENA, tmp_mark); _DASI_FMOD_ARENA = NULL;
        }
    } *err = BIGINT_SUCCESS;
    return rem;
}





//* ======================================== SIGNED NUMBER THEORETIC ========================================= */
/* -------------- Pure Number Theoretic -------------- */
bigInt bigInt_gcdu64(const bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    if (!val) return x;
    bigInt res;
    if (x.n == 0) { if (bigInt_new_u64(&res, val) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else if (x.n == 1) { if (bigInt_new_u64(&res, ___GCD_UI64___(x.limbs[0], val)) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else {
        if (bigInt_snew(&res, min(x.n, 1)) == DNML_ALLOC_OOM) func_ret_oom(err)
        dnml_arena *_DASI_UI64_ARENA = _USE_ARENA(); arena_poison_mut(_DASI_UI64_ARENA, err);
        size_t tmp_mark = arena_mark(_DASI_UI64_ARENA); dnml_status err_check;
        limb_t *tmp_limbs = arena_galloc(_DASI_UI64_ARENA, 1, &err_check);
        arena_alloc_oom_mut(err_check, _DASI_UI64_ARENA, err);

        bigInt y = { .limbs = tmp_limbs, .n = 1,  .cap = 1, .sign = 1 };
        y.limbs[0] = val; __BIGINT_MAGGCD__(&res, &x, &y, &err_check); 
        arena_alloc_oom_mut(err_check, _DASI_UI64_ARENA, err); arena_reset(_DASI_UI64_ARENA, tmp_mark);
        tmp_limbs = NULL; _DASI_UI64_ARENA = NULL;
    } return res;
}
bigInt bigInt_gcdi64(const bigInt x, const int64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    if (!val) return x;
    bigInt res;
    if (x.n == 0) { if (bigInt_new_u64(&res, __MAG_I64__(val)) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else if (x.n == 1) {
        dnml_status err_check = bigInt_new_u64(&res, ___GCD_UI64___(x.limbs[0], __MAG_I64__(val)));
        if (err_check == DNML_ALLOC_OOM) func_ret_oom(err);
    } else {
        if (bigInt_snew(&res, min(x.n, 1)) == DNML_ALLOC_OOM) func_ret_oom(err)
        dnml_arena *_DASI_UI64_ARENA = _USE_ARENA(); arena_poison_mut(_DASI_UI64_ARENA, err);
        size_t tmp_mark = arena_mark(_DASI_UI64_ARENA); dnml_status err_check;
        limb_t *tmp_limbs = arena_galloc(_DASI_UI64_ARENA, 1, &err_check);
        arena_alloc_oom_mut(err_check, _DASI_UI64_ARENA, err);

        bigInt y = { .limbs = tmp_limbs, .n = 1, .cap = 1, .sign = 1 }; 
        y.limbs[0] = __MAG_I64__(val); __BIGINT_MAGGCD__(&res, &x, &y, &err_check);
        arena_alloc_oom_mut(err_check, _DASI_UI64_ARENA, err); arena_reset(_DASI_UI64_ARENA, tmp_mark); 
        tmp_limbs = NULL; _DASI_UI64_ARENA = NULL;
    } return res;
}
bigInt bigInt_gcd(const bigInt x, const bigInt y, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    if (x.n == 0) return y;
    else if (y.n == 0) return x;
    bigInt res;
    if (x.n == 1 && y.n == 1) {
        dnml_status err_check = bigInt_new_u64(&res, ___GCD_UI64___(x.limbs[0], y.limbs[0]));
        if (err_check == DNML_ALLOC_OOM) func_ret_oom(err);
    } else {
        if (bigInt_snew(&res, min(x.n, y.n)) == DNML_ALLOC_OOM) func_ret_oom(err);
        dnml_status err_check; __BIGINT_MAGGCD__(&res, &x, &y, &err_check);
        arena_alloc_oom_mut(err_check, &___DASI_NUMERIC_ARENA_, err);
    } return res;
}
bigInt bigInt_lcmu64(const bigInt x, const uint64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res;
    if (!val || !x.n) if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err)
    else if (x.n == 1 && x.limbs[0] == 1) { if (bigInt_new_u64(&res, val) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else if (x.n == 1 && x.limbs[0] == val) { if (bigInt_new_u64(&res, val) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else if (val == 1) { if (bigInt_binew(&res, &x) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else {
        if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err);
        dnml_arena *_DASI_LCM_UI64_ARENA = _USE_ARENA(); arena_poison_mut(_DASI_LCM_UI64_ARENA, err);
        size_t tmp_mark = arena_mark(_DASI_LCM_UI64_ARENA); dnml_status err_check;
        limb_t *tmp_limbs = arena_galloc(_DASI_LCM_UI64_ARENA, 1, &err_check);
        arena_alloc_oom_mut(err_check, _DASI_LCM_UI64_ARENA, err);

        bigInt y = { .limbs = tmp_limbs, .sign = 1, .n = 1, .cap = 1 }; 
        y.limbs[0] = val; __BIGINT_MAGLCM__(&res, &x, &y, &err_check);
        arena_alloc_oom_mut(err_check, _DASI_LCM_UI64_ARENA, err); 
        arena_reset(_DASI_LCM_UI64_ARENA, tmp_mark); _DASI_LCM_UI64_ARENA = NULL;
    } return res;
}
bigInt bigInt_lcmi64(const bigInt x, const int64_t val, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res; uint64_t mag_val = __MAG_I64__(val);
    if (!mag_val || !x.n) if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err)
    else if (x.n == 1 && x.limbs[0] == 1) { if (bigInt_new_u64(&res, mag_val) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else if (x.n == 1 && x.limbs[0] == val) { if (bigInt_new_u64(&res, mag_val) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else if (mag_val == 1) { if (bigInt_binew(&res, &x) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else {
        if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err);
        dnml_arena *_DASI_LCM_UI64_ARENA = _USE_ARENA(); arena_poison_mut(_DASI_LCM_UI64_ARENA, err);
        size_t tmp_mark = arena_mark(_DASI_LCM_UI64_ARENA); dnml_status err_check;
        limb_t *tmp_limbs = arena_galloc(_DASI_LCM_UI64_ARENA, 1, &err_check);
        arena_alloc_oom_mut(err_check, _DASI_LCM_UI64_ARENA, err);

        bigInt y = { .limbs = tmp_limbs, .sign = 1, .n = 1, .cap = 1 };
        y.limbs[0] = mag_val; __BIGINT_MAGLCM__(&res, &x, &y, &err_check); 
        arena_alloc_oom_mut(err_check, _DASI_LCM_UI64_ARENA, err);
        arena_reset(_DASI_LCM_UI64_ARENA, tmp_mark); _DASI_LCM_UI64_ARENA = NULL;
    }
}
bigInt bigInt_lcm(const bigInt x, const bigInt y, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(y), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    if (x.n == 1 && x.limbs[0] == 1) return y; // lcm(1, y) = y
    else if (y.n == 1 && y.limbs[0] == 1) return x; // lcm(x, 1) = x
    else if (!__BIGINT_MAGCOMP__(&x, &y)) return x; // lcm(x, y) = x WHEN x = y
    bigInt res; if (!y.n || !x.n) if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err) // lcm(0, x) || lcm(x, 0) = 0
    else {
        if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err);
        dnml_status err_check; __BIGINT_MAGLCM__(&res, &x, &y, &err_check);
        arena_alloc_oom_mut(err_check, &___DASI_NUMERIC_ARENA_, err);
    } return res;
}
bool bigInt_is_prime(const bigInt x, dnml_status *err) {
    test_assert_mut(bigInt_validate(x), full_contract, clear_arena, err, BIGINT_ERR_INVAL, false);
    if (x.sign == -1) return false;
    if (x.n == 1) { uint64_t val = x.limbs[0];
        if (val <= 1) return false;
        else if (val == 2 || val == 3 || val == 5) return true;
        else if (!(val & 1) || val % 3 == 0 || val % 5 == 0) return false;
    } else { if (!(x.limbs[0] & 1)) return true;
        else if (x.limbs[0] % 10 == 5 || !(x.limbs[0] % 10)) return true;
    }
    dnml_arena *_DASI_LPRIME_ARENA = _USE_LOW_ARENA();
    arena_grow(_DASI_LPRIME_ARENA, __BIGINT_PTEST_WS__(x.n));
    calc_ctx _isprime_ctx = {
        .alloc = &arena_alloc_adapter,
        .mark = &arena_mark_adapter,
        .reset = &arena_reset_adapter,
        .clear = &arena_clear_adapter,
        .destruct = &arena_destruct_adapter,
        .state  = _DASI_LPRIME_ARENA
    }; return (bool)(__BIGINT_PTEST_DISPATCH__(&x, _isprime_ctx));
}
/* ---------------- Modular Reduction ---------------- */
dnml_status bigInt_mut_emodu64(bigInt *x, const uint64_t mod) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
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
dnml_status bigInt_mut_emodi64(bigInt *x, const int64_t mod) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
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
dnml_status bigInt_mut_emod(bigInt *x, const bigInt mod) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x) && bigInt_validate(mod), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!mod.n) return BIGINT_ERR_INVAL;
    else if (!x->n);
    else if (mod.n == 1 && mod.limbs[0] == 1) bigInt_reset(x);
    else if (__BIGINT_MAGCOMP__(x, &mod) == -1 && x->sign == 1);
    else { dnml_arena *_DASI_MUT_MODULO_ARENA = _USE_ARENA(); arena_poisoined(_DASI_MUT_MODULO_ARENA);
        size_t tmp_mark = arena_mark(_DASI_MUT_MODULO_ARENA); dnml_status err_check;
        limb_t *tmp_limbs = arena_galloc(_DASI_MUT_MODULO_ARENA, mod.n, &err_check);
        arena_alloc_oom(err_check, _DASI_MUT_MODULO_ARENA);

        bigInt tmp_res = {  .limbs = tmp_limbs, .n = 0, .cap = mod.n, .sign = 1 }; 
        __BIGINT_MAGEMOD__(&tmp_res, x, &mod, &err_check);
        arena_alloc_oom(err_check, _DASI_MUT_MODULO_ARENA);
        if (x->sign == -1 && tmp_res.n) {
            __BIGINT_MAGSUB__(&tmp_res, &mod, &tmp_res, &err_check); arena_alloc_oom(err_check, _DASI_MUT_MODULO_ARENA);
            __BIGINT_MAGEMOD__(&tmp_res, &tmp_res, &mod, &err_check); arena_alloc_oom(err_check, _DASI_MUT_MODULO_ARENA);
        } __BIGINT_INTERNAL_COPY__(x, &tmp_res);
        arena_reset(_DASI_MUT_MODULO_ARENA, tmp_mark); _DASI_MUT_MODULO_ARENA = NULL;
    } return BIGINT_SUCCESS;
}
uint64_t bigInt_emodu64(const bigInt x, const uint64_t mod, dnml_status *err) {
    test_assert_mut(bigInt_validate(x), full_contract, clear_arena, err, BIGINT_ERR_INVAL, -1);
    if (!mod) mut_gret(err, BIGINT_ERR_INVAL, -1);
    else if (mod == 1 || !x.n) mut_gret(err, BIGINT_SUCCESS, 0);
    else if ( x.n == 1 && 
        x.limbs[0] < mod && 
        x.sign == 1) mut_gret(err, BIGINT_SUCCESS, x.limbs[0]);
    uint64_t res; __BIGINT_MAGEMOD_U64__(&res, &x, mod);
    if (x.sign == -1 && res) res = (mod - res) % mod;
    *err = BIGINT_SUCCESS; return res;
}
uint64_t bigInt_emodi64(const bigInt x, const int64_t mod, dnml_status *err) {
    test_assert_mut(bigInt_validate(x), full_contract, clear_arena, err, BIGINT_ERR_INVAL, -1);
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
bigInt bigInt_emod(const bigInt x, const bigInt mod, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x) && bigInt_validate(mod), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    if (!mod.n) mut_err(err, BIGINT_ERR_INVAL);
    bigInt res; if (!x.n) if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err)
    else if (mod.n == 1 && mod.limbs[0] == 1) if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err)
    else if (__BIGINT_MAGCOMP__(&x, &mod) == -1  && x.sign == 1) { 
        if (bigInt_binew(&res, &x) == DNML_ALLOC_OOM) func_ret_oom(err); 
    } else {
        if (bigInt_snew(&res, mod.n) == DNML_ALLOC_OOM) func_ret_oom(err);
        dnml_status err_check; __BIGINT_MAGEMOD__(&res, &x, &mod, &err_check);
        arena_alloc_oom_mut(err_check, &___DASI_NUMERIC_ARENA_, err);
        if (x.sign == -1 && res.n) {
            __BIGINT_MAGSUB__(&res, &mod, &res, &err_check); arena_alloc_oom_mut(err_check, &___DASI_NUMERIC_ARENA_, err);
            __BIGINT_MAGEMOD__(&res, &res, &mod, &err_check); arena_alloc_oom_mut(err_check, &___DASI_NUMERIC_ARENA_, err);
        }
    } *err = BIGINT_SUCCESS; return res;
}
/* ---------------- SMALL Modular Arithmetic --------------- */
dnml_status bigInt_mut_modadd_u64(bigInt *x, const bigInt y, const uint64_t mod) {}
dnml_status bigInt_mut_modsub_u64(bigInt *x, const bigInt y, const uint64_t mod) {}
dnml_status bigInt_mut_modadd(bigInt *x, const bigInt y, const bigInt mod) {}
dnml_status bigInt_mut_modsub(bigInt *x, const bigInt y, const bigInt mod) {}
uint64_t bigInt_modadd_u64(const bigInt x, const bigInt y, const uint64_t mod) {}
uint64_t bigInt_modsub_u64(const bigInt x, const bigInt y, const uint64_t mod) {}
bigInt bigInt_modadd(const bigInt x, const bigInt y, const bigInt mod) {}
bigInt bigInt_modsub(const bigInt x, const bigInt y, const bigInt mod) {}
/* ---------------- LARGE Modular Arithmetic --------------- */
dnml_status bigInt_mut_modmul_u64(bigInt *x, const bigInt y, const uint64_t mod) {}
dnml_status bigInt_mut_moddiv_u64(bigInt *x, const bigInt y, const uint64_t mod) {}
dnml_status bigInt_mut_modmul(bigInt *x, const bigInt y, const bigInt mod) {}
dnml_status bigInt_mut_moddiv(bigInt *x, const bigInt y, const bigInt mod) {}
uint64_t bigInt_modmul_u64(const bigInt x, const bigInt y, const uint64_t mod) {}
uint64_t bigInt_moddiv_u64(const bigInt x, const bigInt y, const uint64_t mod) {}
bigInt bigInt_modmul(const bigInt x, const bigInt y, const bigInt mod) {}
bigInt bigInt_moddiv(const bigInt x, const bigInt y, const bigInt mod) {}
/* ---------------------- Modular Algebraic ------------------ */
dnml_status bigInt_mut_modexp_u64(bigInt *x, const bigInt y, const uint64_t mod) {}
dnml_status bigInt_mut_modsqr_u64(bigInt *x, const uint64_t mod) {}
dnml_status bigInt_mut_modinv_u64(bigInt *x, const uint64_t mod) {}
dnml_status bigInt_mut_modexp(bigInt *x, const bigInt y, const bigInt mod) {}
dnml_status bigInt_mut_modsqr(bigInt *x, const bigInt mod) {}
dnml_status bigInt_mut_modinv(bigInt *x, const bigInt mod) {}
uint64_t bigInt_modexp_u64(const bigInt x, const bigInt y, const uint64_t mod) {}
uint64_t bigInt_modsqr_u64(const bigInt x, const uint64_t mod) {}
uint64_t bigInt_modinv_u64(const bigInt x, const uint64_t mod) {}
bigInt bigInt_modexp(const bigInt x, const bigInt y, const bigInt mod) {}
bigInt bigInt_modsqr(const bigInt x, const bigInt mod) {}
bigInt bigInt_modinv(const bigInt x, const bigInt mod) {}




//* ====================================== SIGNED ALGEBRAIC OPERATIONS ======================================= */
/* -------------- MUTATIVE ALGEBRAIC -------------- */
dnml_status bigInt_mut_sqr(bigInt *x) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (x->n = 1) {
        if (x->limbs[0] < UINT32_MAX) x->limbs[0] *= x->limbs[0];
        else if (x->limbs[0] != 1) { 
            bigInt_reserve(x, 2);
            x->limbs[0] = __MUL_UI64__(x->limbs[0], x->limbs[0], &x->limbs[1] );
            x->n = 2;
        } 
        x->sign = 1;
    } else if (x->n) {
        dnml_status err_check = bigInt_reserve(x, x->n << 1); heap_alloc_oom(err_check);
        dnml_arena *_DASI_MUTSQR_ARENA = _USE_ARENA(); arena_poisoined(_DASI_MUTSQR_ARENA);
        size_t mutsqr_mark = arena_mark(_DASI_MUTSQR_ARENA);
        limb_t *tmp_limb = arena_galloc(_DASI_MUTSQR_ARENA, x->n * 2, &err_check);
        arena_alloc_oom(err_check, _DASI_MUTSQR_ARENA);

        bigInt tmp_res = {.limbs = tmp_limb, .sign = 1, .n = 0, .cap = x->n * 2};
        __BIGINT_MAGSQR__(&tmp_res, x, &err_check);
        arena_alloc_oom(err_check, _DASI_MUTSQR_ARENA); tmp_res.sign = 1;
        err_check = bigInt_mut_ocopy(x, tmp_res); ocopy_check(err_check, _DASI_MUTSQR_ARENA);
        arena_reset(_DASI_MUTSQR_ARENA, mutsqr_mark);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_pow(bigInt *x, const uint64_t exp) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
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
         dnml_status err_check = bigInt_reserve(x, x->n * exp); heap_alloc_oom(err_check);
        dnml_arena *_DASI_MUTPOW_ARENA = _USE_ARENA(); arena_poisoined(_DASI_MUTPOW_ARENA);
        size_t mutpow_mark = arena_mark(_DASI_MUTPOW_ARENA);
        limb_t *tmp_limbs = arena_galloc(_DASI_MUTPOW_ARENA, x->n * exp, &err_check);
        arena_alloc_oom(err_check, _DASI_MUTPOW_ARENA);
        
        bigInt tmp_res = {.limbs = tmp_limbs, .sign = 1, .n = 0, .cap = x->n * exp};
        __BIGINT_MAGPOW__(&tmp_res, x, exp, &err_check); 
        arena_alloc_oom(err_check, _DASI_MUTPOW_ARENA); tmp_res.sign = (!(exp & 1)) ? 1 : x->sign;
        err_check = bigInt_mut_ocopy(x, tmp_res); ocopy_check(err_check, _DASI_MUTPOW_ARENA);
        arena_reset(_DASI_MUTPOW_ARENA, mutpow_mark);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_sqrt(bigInt *x) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (x->sign == -1) return BIGINT_ERR_DOMAIN;
    if (x->n == 1 && x->limbs[0] == 1);
    else if (x->n) {
        /* The square root of any integer x will ALWAYS be <= x */
        dnml_arena *_DASI_MUTSQRT_ARENA = _USE_ARENA(); arena_poisoined(_DASI_MUTSQRT_ARENA);
        size_t mutsqrt_mark = arena_mark(_DASI_MUTSQRT_ARENA); dnml_status err_check;
        limb_t *tmp_limbs = arena_galloc(_DASI_MUTSQRT_ARENA, (x->n >> 1), &err_check);
        arena_alloc_oom(err_check, _DASI_MUTSQRT_ARENA);

        bigInt tmp_res = {.limbs = tmp_limbs, .sign = 1, .n = 0, .cap = (x->n >> 1)};
        __BIGINT_MAGSQRT__(&tmp_res, x, &err_check);
        arena_alloc_oom(err_check, _DASI_MUTSQRT_ARENA); tmp_res.sign = 1;
        err_check = bigInt_mut_ocopy(x, tmp_res); ocopy_check(err_check, _DASI_MUTSQRT_ARENA);
        arena_reset(_DASI_MUTSQRT_ARENA, mutsqrt_mark);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_cbrt(bigInt *x) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (x->n == 1 && x->limbs[0] == 1);
    else if (x->n) {
        /* The square root of any integer x will ALWAYS be <= x */
        dnml_arena *_DASI_MUTCBRT_ARENA = _USE_ARENA(); arena_poisoined(_DASI_MUTCBRT_ARENA);
        size_t mutcbrt_mark = arena_mark(_DASI_MUTCBRT_ARENA); dnml_status err_check;
        limb_t *tmp_limbs = arena_galloc(_DASI_MUTCBRT_ARENA, (x->n >> 1), &err_check);
        arena_alloc_oom(err_check, _DASI_MUTCBRT_ARENA);

        bigInt tmp_res = {.limbs = tmp_limbs, .sign = 1, .n = 0, .cap = (x->n / 3)};
        __BIGINT_MAGCBRT__(&tmp_res, x, &err_check);
        arena_alloc_oom(err_check, _DASI_MUTCBRT_ARENA); tmp_res.sign = x->sign;
        err_check = bigInt_mut_ocopy(x, tmp_res); ocopy_check(err_check, _DASI_MUTCBRT_ARENA);
        arena_reset(_DASI_MUTCBRT_ARENA, mutcbrt_mark);
    } return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_nrt(bigInt *x, const uint64_t root) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(bigInt_pvalidate(x), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (!root) return BIGINT_ERR_INVAL;
    if (!(root & 1) && x->sign == -1) return BIGINT_ERR_DOMAIN;
    if (x->n == 1 && x->limbs[0] == 1);
    else if (x->n) {
        dnml_arena *_DASI_MUTNRT_ARENA = _USE_ARENA(); arena_poisoined(_DASI_MUTNRT_ARENA);
        size_t mutnrt_mark = arena_mark(_DASI_MUTNRT_ARENA); dnml_status err_check;
        size_t alloc_size = (__IS_2POW__(root)) ? (x->n >> __CTZ_UI64__(root)) : (x->n / root);
        limb_t *tmp_limbs = arena_galloc(_DASI_MUTNRT_ARENA, alloc_size, &err_check);
        arena_alloc_oom(err_check, _DASI_MUTNRT_ARENA);

        bigInt tmp_res = {.limbs = tmp_limbs, .sign = 1, .n = 0, .cap = alloc_size};
        __BIGINT_MAGNRT__(&tmp_res, x, root, &err_check);
        arena_alloc_oom(err_check, _DASI_MUTNRT_ARENA); tmp_res.sign = (!(root & 1)) ? 1 : x->sign;
        err_check = bigInt_mut_ocopy(x, tmp_res); ocopy_check(err_check, _DASI_MUTNRT_ARENA);
        arena_reset(_DASI_MUTNRT_ARENA, mutnrt_mark);
    }
}
/* -------------- FUNCTIONAL ALGEBRAIC -------------- */
bigInt bigInt_sqr(const bigInt x, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res; if (x.n == 0) if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err)
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
        dnml_status err_check; __BIGINT_MAGSQR__(&res, &x, &err_check); 
        arena_alloc_oom_mut(err_check, &___DASI_NUMERIC_ARENA_, err); res.sign = 1;
    } return res;
}
bigInt bigInt_pow(const bigInt x, const uint64_t exp, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    if (exp == 2) return bigInt_sqr(x, err);
    bigInt res; if (!exp) { if (bigInt_new_u64(&res, 1) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else if (!x.n) if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err)
    else if (x.n == 1 && x.limbs[0] == 1) { 
        dnml_status err_check = bigInt_new_i64 (&res, 1 * (!(exp & 1) ? 1 : x.sign));
        if (err_check == DNML_ALLOC_OOM) func_ret_oom(err); 
    }
    else if (x.n == 1 && __SAFE_EXP__(x.limbs[0], exp)) {
        uint64_t exp_res = (uint64_t)(pow((double)x.limbs[0], (double)exp));
        if (bigInt_new_u64(&res, exp_res) == DNML_ALLOC_OOM) func_ret_oom(err);
        res.sign = (!(exp & 1)) ? 1 : x.sign;
    } 
    else if (exp == 1) { if (bigInt_binew(&res, &x) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else { 
        if (bigInt_snew(&res, x.n * exp) == DNML_ALLOC_OOM) func_ret_oom(err);
        dnml_status err_check; __BIGINT_MAGPOW__(&res, &x, exp, &err_check);
        arena_alloc_oom_mut(err_check, &___DASI_NUMERIC_ARENA_, err); res.sign = (!(exp & 1)) ? 1 : x.sign;
    } return res;
}
bigInt bigInt_sqrt(const bigInt x, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    if (x.sign == -1) mut_err(err, BIGINT_ERR_DOMAIN);
    bigInt res; if (!x.n) if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err)
    else if (x.n == 1 && x.limbs[0] == 1) { if (bigInt_new_u64(&res, 1) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else { 
        if (bigInt_snew(&res, (x.n >> 1)) == DNML_ALLOC_OOM) func_ret_oom(err);
        dnml_status err_check; __BIGINT_MAGSQR__(&res, &x, &err_check);
        arena_alloc_oom_mut(err_check, &___DASI_NUMERIC_ARENA_, err); res.sign = 1;
    } return res;
}
bigInt bigInt_cbrt(const bigInt x, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    bigInt res; if (!x.n) if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err)
    else if (x.n == 1 && x.limbs[0] == 1) { if (bigInt_new_i64(&res, (1 * x.sign)) == DNML_ALLOC_OOM) func_ret_oom(err); }
    else {
        if (bigInt_snew(&res, x.n / 3) == DNML_ALLOC_OOM) func_ret_oom(err);
        dnml_status err_check; __BIGINT_MAGCBRT__(&res, &x, &err_check);
        arena_alloc_oom_mut(err_check, &___DASI_NUMERIC_ARENA_, err); res.sign = x.sign;
    } return res;
}
bigInt bigInt_nrt(const bigInt x, const uint64_t root, dnml_status *err) {
    test_assert_mut(
        bigInt_validate(x), full_contract, clear_arena, 
        err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__()
    );
    if (!root) mut_err(err, BIGINT_ERR_INVAL);
    else if (!(root & 1) && x.sign == -1) mut_err(err, BIGINT_ERR_DOMAIN);
    bigInt res; if (!x.n) if (bigInt_new(&res) == DNML_ALLOC_OOM) func_ret_oom(err)
    else if (x.n == 1 && x.limbs[0] == 1) { 
        bigInt_new_u64(&res, 1); 
        res.sign = (!(root & 1)) ? 1 : x.sign; 
    } else { 
        size_t alloc_size = (__IS_2POW__(root)) ? (x.n >> __CTZ_UI64__(root)) : (x.n / root);
        if (bigInt_snew(&res, alloc_size) == DNML_ALLOC_OOM) func_ret_oom(err);
        dnml_status err_check; __BIGINT_MAGNRT__(&res, &x, root, &err_check);
        arena_alloc_oom_mut(err_check, &___DASI_NUMERIC_ARENA_, err); res.sign = (!(root & 1)) ? 1 : x.sign;
    } return res;
}




//* ================================================= COPIES ================================================= */
/* -------------  Mutative SMALL Copies ------------- */
dnml_status bigInt_mut_copyu64(bigInt *dst, const uint64_t src) {
    test_assert(dst != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(__BIGINT_INTERNAL_SVALID__(dst), storage_inval, clear_arena, BIGINT_ERR_STORE_IN);
    bigInt_canonicalize(dst);
    if (dst->n == 0 && !src) return BIGINT_SUCCESS;
    if (dst->n == 1 && dst->limbs[0] == src) return BIGINT_SUCCESS;
    dst->limbs[0] = src;
    dst->n        = src ? 1 : 0;
    dst->sign     = 1;
}
dnml_status bigInt_mut_dcopyu64(bigInt *dst, const uint64_t src) {
    test_assert(dst != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(__BIGINT_INTERNAL_SVALID__(dst), storage_inval, clear_arena, BIGINT_ERR_STORE_IN);
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
    dst->limbs[0] = src;
    dst->n        = src ? 1 : 0;
    dst->sign     = 1;
}
dnml_status bigInt_mut_copyi64(bigInt *dst, const int64_t src) {
    test_assert(dst != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(__BIGINT_INTERNAL_SVALID__(dst), storage_inval, clear_arena, BIGINT_ERR_STORE_IN);
    bigInt_canonicalize(dst);
    if (dst->n == 0 && !src) return BIGINT_SUCCESS;
    if (dst->n == 1 && dst->limbs[0] == __MAG_I64__(src)) {
        dst->sign = (src < 0) ? -1 : 1; return BIGINT_SUCCESS;
    }
    dst->limbs[0] = __MAG_I64__(src);
    dst->n        = src ? 1 : 0;
    dst->sign     = (src< 0 ? -1 : 1);
}
dnml_status bigInt_mut_dcopyi64(bigInt *dst, const int64_t src) {
    test_assert(dst != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(__BIGINT_INTERNAL_SVALID__(dst), storage_inval, clear_arena, BIGINT_ERR_STORE_IN);
    bigInt_canonicalize(dst);
    // Always reallocate and resize if dst->cap is more than 1
    if (dst->cap > 1) {
        uint64_t* __BUFFER_P = realloc(dst->limbs, sizeof(uint64_t));
        if (__BUFFER_P == NULL) return DNML_ALLOC_OOM;
        dst->limbs = __BUFFER_P;
        dst->cap     = 1;
    }
    if (dst->n == 0 && !src) return BIGINT_SUCCESS;
    if (dst->n == 1 && dst->limbs[0] == __MAG_I64__(src)) {
        dst->sign = (src < 0) ? -1 : 1;
        return BIGINT_SUCCESS;
    }
    dst->limbs[0] = __MAG_I64__(src);
    dst->n        = src ? 1 : 0;
    dst->sign     = (src< 0 ? -1 : 1);
}
/* -------------  Mutative LARGE Copies ------------- */
dnml_status bigInt_mut_copyf128(bigInt *dst, long double src) {}
dnml_status bigInt_mut_dcopyf128(bigInt *dst, long double src) {}
dnml_status bigInt_mut_ocopyf128(bigInt *dst, long double src) {}
dnml_status bigInt_mut_tover_copyf128(bigInt *dst, long double src) {}
dnml_status bigInt_mut_copy(bigInt *dst, const bigInt src) {
    test_assert(dst != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(__BIGINT_INTERNAL_SVALID__(dst), storage_inval, clear_arena, BIGINT_ERR_STORE_IN);
    test_assert(bigInt_validate(src), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (dst->limbs = src.limbs) {
        dst->n = src.n;
        dst->cap = src.n;
        dst->sign = src.sign;
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
    if (dst->cap < src.n) { dnml_status err_check = bigInt_reserve(dst, src.n); heap_alloc_oom(err_check); }
    memcpy(dst->limbs, src.limbs, src.n);
    dst->n    = src.n;
    dst->sign = src.sign;
}
dnml_status bigInt_mut_dcopy(bigInt *dst, const bigInt src) {
    test_assert(dst != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(__BIGINT_INTERNAL_SVALID__(dst), storage_inval, clear_arena, BIGINT_ERR_STORE_IN);
    test_assert(bigInt_validate(src), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (dst->limbs = src.limbs) {
        dst->n = src.n;
        dst->cap = src.n;
        dst->sign = src.sign;
    } bigInt_canonicalize(dst); // Enforce contracts, ESPECAILLY Contract 3
    if (dst->cap != src.cap) if (bigInt_resize(dst, src.cap) == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    /* Fast Paths */
    // The equal fast path (dst != 0 && src != 0) is not here since
    // Reallocation and Resizing may tamper with the size metadata,
    //  -----> Tampering with the validity of memcmp()
    if (dst->n == 0 && src.n == 0) return BIGINT_SUCCESS;

    /* Standard Path */
    memcpy(dst->limbs, src.limbs, src.n * sizeof(uint64_t));
    dst->n    = src.n;
    dst->sign = src.sign;
}
dnml_status bigInt_mut_ocopy(bigInt *dst, const bigInt src) {
    test_assert(dst != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(__BIGINT_INTERNAL_SVALID__(dst), storage_inval, clear_arena, BIGINT_ERR_STORE_IN);
    test_assert(bigInt_validate(src), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (dst->limbs = src.limbs) {
        dst->n = src.n;
        dst->cap = src.n;
        dst->sign = src.sign;
    } bigInt_canonicalize(dst); // Enforce contracts, ESPECAILLY Contract 3
    /* Fast Paths */
    // Since they're equal, and due to Contract 3
    //  ------> They're not subjected to errors if these cases are true
    if (dst->n == 0 && src.n == 0) return BIGINT_SUCCESS;
    if (dst->n == src.n && !memcmp(dst->limbs, src.limbs, src.n * sizeof(uint64_t))) {
        dst->sign = src.sign;
        return BIGINT_SUCCESS;
    }
    /* Standard Route */
    if (dst->cap < src.n) return BIGINT_ERR_RANGE;
    memcpy(dst->limbs, src.limbs, src.n * sizeof(uint64_t));
    dst->n = src.n; dst->sign = src.sign; return BIGINT_SUCCESS;
}
dnml_status bigInt_mut_tover_copy(bigInt *dst, const bigInt src) { 
    test_assert(dst != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(__BIGINT_INTERNAL_SVALID__(dst), storage_inval, clear_arena, BIGINT_ERR_STORE_IN);
    test_assert(bigInt_validate(src), full_contract, clear_arena, BIGINT_ERR_INVAL);
    if (dst->limbs = src.limbs) {
        dst->n = src.n;
        dst->cap = src.n;
        dst->sign = src.sign;
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
    dst->n = operation_range; dst->sign = src.sign;
}
/* -------------  Functional SMALL Copies ------------- */
bigInt bigInt_copyu64(const uint64_t src, dnml_status *err) {
    bigInt dst; 
    if (bigInt_new(&dst) == DNML_ALLOC_OOM) func_ret_oom(err)
    if (src) {
        dst.limbs[0] = src;
        dst.n        = 1;
    }
    return dst;
}
bigInt bigInt_copyi64(const int64_t src, dnml_status *err) {
    bigInt dst; 
    if (bigInt_new(&dst) == DNML_ALLOC_OOM) func_ret_oom(err)
    if (src) {
        dst.limbs[0] = __MAG_I64__(src);
        dst.n        = 1;
        dst.sign     = (src < 0) ? -1 : 1;
    }
    return dst;
}
/* -------------  Functional LARGE Copies ------------- */
bigInt bigInt_copyf128(long double src, dnml_status *err) {}
bigInt bigInt_ocopyf128(long double src, size_t output_cap, dnml_status *err) {}
bigInt bigInt_tover_copyf128(long double src, size_t output_cap, dnml_status *err) {}
bigInt bigInt_copy(const bigInt src, dnml_status *err) {
    test_assert_mut(bigInt_validate(src), full_contract, clear_arena, err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__());
    bigInt dst;
    if (src.n == 0) {
        if (bigInt_new(&dst) == DNML_ALLOC_OOM) func_ret_oom(err)
        return dst;
    }
    if (bigInt_snew(&dst, src.n) == DNML_ALLOC_OOM) func_ret_oom(err);
    memcpy(dst.limbs, src.limbs, src.n * sizeof(uint64_t));
    dst.n = src.n; dst.sign  = src.sign; return dst;
}
bigInt bigInt_ocopy(const bigInt src, size_t output_cap, dnml_status *err) {
    test_assert_mut(bigInt_validate(src), full_contract, clear_arena, err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__());
    if (output_cap < src.n) {
        if (err != NULL) *err = BIGINT_ERR_RANGE;
        return __BIGINT_ERROR_VALUE__();
    }
    bigInt dst;  
    if (bigInt_snew(&dst, output_cap) == DNML_ALLOC_OOM) func_ret_oom(err);
    memcpy(dst.limbs, src.limbs, src.n * sizeof(uint64_t));
    dst.n = src.n; dst.sign = src.sign;
    *err = BIGINT_SUCCESS; return dst;
}
bigInt bigInt_tover_copy(const bigInt src, size_t output_cap, dnml_status *err) {
    test_assert_mut(bigInt_validate(src), full_contract, clear_arena, err, BIGINT_ERR_INVAL, __BIGINT_ERROR_VALUE__());
    bigInt dst;
    if (output_cap == 0) if (bigInt_new(&dst) == DNML_ALLOC_OOM) func_ret_oom(err)
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
dnml_status bigInt_canonicalize(bigInt *x) {
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
dnml_status bigInt_normalize(bigInt *x) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    while (x->n > 0 && x->limbs[x->n - 1] == 0) --x->n; // Delete trailing/leading zeros
    if (x->n == 0) x->sign = 1; // Guarantees 0, not -0
    return BIGINT_SUCCESS;
}
dnml_status bigInt_resize(bigInt *x, size_t k) { //* Exact Capacity resize
    test_assert(__BIGINT_INTERNAL_SVALID__(x), storage_inval, clear_arena, BIGINT_ERR_STORE_IN);
    k = (!k) ? 1 : k;
    limb_t *__BUFFER_P = realloc(x->limbs, k * sizeof(limb_t));
    if (__BUFFER_P == NULL) return DNML_ALLOC_OOM;
    x->limbs = __BUFFER_P; x->cap = k;
    if (x->n > x->cap) x->n = x->cap;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_reserve(bigInt *x, size_t k) { //* Minimum Capacity
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(__BIGINT_INTERNAL_SVALID__(x), storage_inval, clear_arena, BIGINT_ERR_STORE_IN);
    if (x->cap >= k) return BIGINT_SUCCESS;
    size_t new_cap = x->cap;
    // Capacity doubles instead of incrementation, 
    // ---> Ensure less reallocation ---> Enhanced performance
    while (new_cap < k) new_cap *= 2;
    limb_t *__BUFFER_P = realloc(x->limbs, new_cap * sizeof(limb_t));
    if (__BUFFER_P == NULL) return DNML_ALLOC_OOM;
    x->limbs = __BUFFER_P; x->cap = new_cap;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_shrink(bigInt *x, size_t k) { //* Maximum Capacity
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(__BIGINT_INTERNAL_SVALID__(x), storage_inval, clear_arena, BIGINT_ERR_STORE_IN);
    k = (!k) ? 1 : k; if (x->cap <= k) return BIGINT_SUCCESS;
    limb_t *__BUFFER_P = realloc(x->limbs, k * sizeof(limb_t));
    if (__BUFFER_P == NULL) return DNML_ALLOC_OOM;
    x->limbs = __BUFFER_P; x->cap = k;
    if (x->n < x->cap) x->n = x->cap;
    return BIGINT_SUCCESS;
}
dnml_status bigInt_reset(bigInt *x) {
    test_assert(x != NULL, input_null, clear_arena, BIGINT_NULL);
    test_assert(__BIGINT_INTERNAL_PVALID__(x), state_contract, clear_arena, BIGINT_ERR_SINVAL);
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
bool bigInt_pvalidate(bigInt *x) {
    DNML_TEST_ASSERT(x != NULL, input_null, {});
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


