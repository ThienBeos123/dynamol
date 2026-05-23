#include "cryptInt_func.h"
#include <stdbool.h>



//* ========================== COMMON ASSERT ERRORS CATALOG ========================== *//
#define calloc_null "Allocation Failure: calloc() returned NULL (-Ealloc_calloc_fail)" /* CALLOC returns NULL */
#define realloc_null "Allocation Failure: realloc() returned NULL (-Ealloc_realloc_fail)" /* REALLOC returns NULL */
#define full_contract "Contract Violation: Invalid CryptInt (-Ecrypt_int_invalid)" /* FULL Contract Violation */
#define store_inval "\
Partial Contract Violation: CryptInt invalid for storage (-Ecrypt_int_sinvalid)" /* Partial Contract Violation - Storage */


//* ========================== COMMON !TEST! ASSERT ERRORS CATALOG ========================== *//
#define poisoined "Mathematical Error: CryptInt Poisoned (-Ecrypt_int_invalid)" /* CryptInt Poisoined - Testing */
#define null_err "Parameter Error: Status/Error parameter-based returns is null (-Enull_err_param)" /* err == NULL - testing */




//* ===================================== TYPE SETUP FUNCTION ===================================== *//
void crint_free(cryptint *x) {
    if (x->limbs == NULL) return;
    __libdnml_memwipe_strict(x->limbs, x->cap * BYTES_IN_UINT64_T);
    free(x->limbs); x->limbs = 0;
    x->n = 0; x->cap = 0; x->sign = 0;
    x->poisoned = 0;
}
dnml_status crint_new(cryptint *x) {
    if (x->limbs != NULL) return; // already initialized
    limb_t *P_BUFFER__ = calloc(1, BYTES_IN_UINT64_T);
    if (P_BUFFER__ == NULL) {
        x->limbs = 0; x->n = 0; x->cap = 0;
        x->poisoned = 0; x->sign = 0;
        return DNML_ALLOC_OOM;
    }
    x->limbs = P_BUFFER__; x->cap = 1;
    x->n = 0; x->sign = 1; x->poisoned = false;
}
dnml_status crint_snew(cryptint *x, const size_t n) {
    if (x->limbs != NULL) return; // already initialized
    size_t salloc; NORMALIZE_0_TO_1(salloc, n);
    limb_t *__BUFFER_P = calloc(salloc, BYTES_IN_UINT64_T);
    if (__BUFFER_P == NULL) {
        x->limbs = 0; x->n = 0; x->cap = 0;
        x->poisoned = 0; x->sign = 0;
        return DNML_ALLOC_OOM;
    }
    x->limbs = __BUFFER_P; x->cap = n;
    x->n = 0; x->sign = 1; x->poisoned = false;
    return CRYPTINT_SUCCESS;
}
dnml_status crint_cinew(cryptint *x, cryptint *y) {
    // Pre-operation Validation & Static Analysis
    if (x->limbs != NULL) return; // already initialized
    DNML_ASSERT((crint_pvalidate(y)), full_contract,
        { /* Set everything to 0 to completely wipe out memory */
          /* We can only guarantee wiping out the first limb safely */
            if (y->limbs != NULL) *(y->limbs) = 0;
            crint_free(y); x->limbs = 0; x->n = 0;
            x->cap = 0; x->sign = 0; x->poisoned = 0;
        }
    ); DNML_TEST_ASSERT((!(y->poisoned)), poisoined);
    if (y->poisoned) { x->n = 0; x->sign = 0; x->cap = 0; x->limbs = 0; return CRYPTINT_POISOINED; }
    /* Main Operations */
    size_t alloc_size; CHOOSE_OPTION((alloc_size), (y->n), (y->n), (1));
    limb_t *__BUFFER_P = calloc(y->n, BYTES_IN_UINT64_T);
    if (__BUFFER_P == NULL) {
        x->limbs = 0; x->n = 0; x->cap = 0;
        x->poisoned = 0; x->sign = 0;
        return DNML_ALLOC_OOM;
    }
    x->limbs = __BUFFER_P;
    if (y->n) __libdnml_memcpy_strict(
        x->limbs, y->limbs,
        y->n * BYTES_IN_UINT64_T, 0,
        (y->n - 1) * BYTES_IN_UINT64_T + (BYTES_IN_UINT64_T - 1)
    ); 
    x->n = y->n; x->cap = alloc_size; 
    x->sign = (y->n) ? y->sign : 1; x->poisoned = false;
    return CRYPTINT_SUCCESS;
}
dnml_status crint_new_u64(cryptint *x, const uint64_t in) {
    if (x->limbs != NULL) return; // already initialized
    limb_t *__BUFFER_P = calloc(1, BYTES_IN_UINT64_T);
    if (__BUFFER_P == NULL) {
        x->limbs = 0; x->n = 0; x->cap = 0;
        x->poisoned = 0; x->sign = 0;
        return DNML_ALLOC_OOM;
    }
    x->limbs = __BUFFER_P; x->limbs[0] = in;
    x->cap = 1; x->n = !!(in); x->sign = 1; x->poisoned = false;
    return CRYPTINT_SUCCESS;
}
dnml_status crint_new_i64(cryptint *x, const int64_t in) {
    if (x->limbs != NULL) return; // already initialized
    limb_t *__BUFFER_P = calloc(1, BYTES_IN_UINT64_T);
    if (__BUFFER_P == NULL) {
        x->limbs = 0; x->n = 0; x->cap = 0;
        x->poisoned = 0; x->sign = 0;
        return DNML_ALLOC_OOM;
    }
    x->limbs = __BUFFER_P; x->limbs[0] = __MAG_I64__(in); 
    x->cap = 1; x->n = !!(in); x->poisoned = false;
    CHOOSE_OPTION((x->sign), (in < 0), (-1), (1));
    return CRYPTINT_SUCCESS;
}
dnml_status crint_new_f128(cryptint *x, long double in) {}





//* =============================================== COMPARISONS ============================================== */
static int8_t __CRINT_MAGCMP64__(cryptint *x, const uint64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_TEST_ASSERT((crint_pvalidate(x)), full_contract);
    DNML_TEST_ASSERT((!(x->poisoned)), poisoined);
    /* Main Operation - Comparison */
    // We set ret to 2 as a safety mask to check if ret is previously set or not
    int8_t ret = 2, curr = 0;
    // Check 1: if (x->n > 1) return 1;
    CHOOSE_OPTION((curr), (x->n > 1), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    // Check 2: if (x->limbs[0] > val) return 1;
    CHOOSE_OPTION((curr), (x->limbs[0] > val), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    // Check 3: if (x->limbs[0] < val) return -1;
    CHOOSE_OPTION((curr), (x->limbs[0] < val), (-1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    // ALL CHECKS WEREN'T SATIFIES --> EQUAL
    CHOOSE_OPTION((ret), (ret == 2), (0), (ret)); return ret;
}
static int8_t __CRINT_MAGCMP__(cryptint *x, cryptint *y) {
    /* Pre-operation Validation & Static Analysis */
    DNML_TEST_ASSERT((crint_pvalidate(x)), full_contract);
    DNML_TEST_ASSERT((!(x->poisoned)), poisoined);
    /* Main Operation - Comparison */
    // We set ret to 2 as a safety mask to check if ret is previously set or not
    int8_t ret = 2, curr = 0;
    // Check 1: if (x->n > y->n) return 1;
    CHOOSE_OPTION((curr), (x->n > y->n), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    // Check 2: if (x->n < y->n) return -1;
    CHOOSE_OPTION((curr), (x->n < y->n), (-1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    // Check 3: Sequential checks
    size_t upperbound = max(x->cap, y->cap);
    uint64_t x_curr, y_curr;
    for (size_t i = upperbound - 1; i >= 0; --i) {
        /* 
        * Guarantees that each iteration always access an index to normalize timing
        * We universally chosen the first limb of each cryptint as a placeholder
        * with not semantic meaning 
        */
        CHOOSE_OPTION((x_curr), (i < x->cap), (x->limbs[i]), (x->limbs[0]));
        CHOOSE_OPTION((x_curr), (i < x->cap), (y->limbs[i]), (y->limbs[0]));
        // Actually Getting the right value
        CHOOSE_OPTION((x_curr), (i < x->n), (x_curr), (0));
        CHOOSE_OPTION((y_curr), (i < y->n), (y_curr), (0));
        // Comparing values
        /* 
        * If our x_curr == x->limbs[i], in which i > y->n,
        * then such case was already covered above in our pre-checks
        * due to the fact that x->n > y->n. This also works inversely for y
        */
       CHOOSE_OPTION((curr), (x_curr > y_curr), (1), (0));
       CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
       CHOOSE_OPTION((curr), (x_curr < y_curr), (-1), (0));
       CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    }
     // ALL CHECKS WEREN'T SATIFIES --> EQUAL
    CHOOSE_OPTION((ret), (ret == 2), (0), (ret)); return ret;
}
/* ---------------- Integer - I64 ---------------- */
bool crint_equal_i64(cryptint x, const int64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); x.limbs = 0; x.cap = 0; 
        x.n = 0; x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err);
    DNML_TEST_ASSERT((!(x.poisoned)), poisoined);
    dnml_status ret_stat = CRYPTINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (err == NULL & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    CHOOSE_OPTION((ret_stat), (x.poisoned & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr; int8_t vsign;
    CHOOSE_OPTION((vsign), (val < 0), (-1), (1));
    uint64_t mag_val = __MAG_I64__(val);
    /* if (!x.n) return (val) ? false : true;  */
    // Check 1: if (!x.n && !val) return true
    CHOOSE_OPTION((curr), (!(x.n) & !(val)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    // Check 2: if (!x.n && val) return false
    CHOOSE_OPTION((curr), (!(x.n) & (val)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));

    // Check 3: if (val_sign != x.sign) return false;
    CHOOSE_OPTION((curr), (vsign != x.sign), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 4: if (x.n > 1) return false;
    CHOOSE_OPTION((curr), (x.n > 1), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 5: if (x.limbs[0] != __MAG_I64__(val)) return false;
    CHOOSE_OPTION((curr), (x.limbs[0] != mag_val), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    CHOOSE_OPTION((ret), (ret == 2), (1), (ret)); 
    *err = ret_stat; return (bool)(ret);
}
bool crint_less_i64(cryptint x, const int64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); x.limbs = 0; x.cap = 0; 
        x.n = 0; x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err);
    DNML_TEST_ASSERT((!(x.poisoned)), poisoined);
    dnml_status ret_stat = CRYPTINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (err == NULL & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    CHOOSE_OPTION((ret_stat), (x.poisoned & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr; int8_t vsign;
    CHOOSE_OPTION((vsign), (val < 0), (-1), (1));
    uint64_t mag_val = __MAG_I64__(val);
    /* if (!x.n) return (val > 0) ? 1 : 0 */
    // Check 1: if (!x.n && val > 0) return true
    CHOOSE_OPTION((curr), (!(x.n) & (val > 0)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    // Check 2: if (!x.n && val < 0) return false
    CHOOSE_OPTION((curr), (!(x.n) & (val < 0)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));

    /* if (val_sign != x.sign) return (x.sign < val_sign); */
    // Check 3: if (val_sign != x.sign && x.sign > val_sign) return false;
    CHOOSE_OPTION((curr), ((vsign != x.sign) & (x.sign > vsign)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 4: if (val_sign != x.sign && x.sign < val_sign) return true;
    CHOOSE_OPTION((curr), ((vsign != x.sign) & (x.sign < vsign)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));

    /* if (x.n > 1) return (x.sign == -1) */
    // Check 5: if (x.n > 1 && x.sign == 1) return false;
    CHOOSE_OPTION((curr), ((x.n > 1) & (x.sign == 1)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 5: if (x.n > 1 && x.sign == -1) return true;
    CHOOSE_OPTION((curr), ((x.n > 1) & (x.sign == -1)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));

    /* if (x.limbs[0] > __MAG_I64__(val)) return (x.sign == -1) */
    // Check 6: if (x.limbs[0] > mag_val && x.sign == 1) return false;
    CHOOSE_OPTION((curr), ((x.limbs[0] > mag_val) & (x.sign == 1)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 7: if (x.limbs[0] > mag_val && x.sign == -1) return true;
    CHOOSE_OPTION((curr), ((x.limbs[0] > mag_val) & (x.sign == -1)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));

    /* return (x.limbs[0] < mag_val) && (x.sign == 1) */
    // Check 8: if (x->limbs[0] < mag_val && (x.sign == -1)) return false;
    CHOOSE_OPTION((curr), ((x.limbs[0] < mag_val) & (x.sign == -1)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    CHOOSE_OPTION((ret), (ret == 2), (1), (ret)); 
    *err = ret_stat; return (bool)(ret);
}
bool crint_more_i64(cryptint x, const int64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); x.limbs = 0; x.cap = 0; 
        x.n = 0; x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err);
    DNML_TEST_ASSERT((!(x.poisoned)), poisoined);
    dnml_status ret_stat = CRYPTINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (err == NULL & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    CHOOSE_OPTION((ret_stat), (x.poisoned & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr; int8_t vsign;
    CHOOSE_OPTION((vsign), (val < 0), (-1), (1));
    uint64_t mag_val = __MAG_I64__(val);
    /* if (!x.n) return (val > 0) ? 1 : 0 */
    // Check 1: if (!x.n && val < 0) return true
    CHOOSE_OPTION((curr), (!(x.n) & (val < 0)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    // Check 2: if (!x.n && val > 0) return false
    CHOOSE_OPTION((curr), (!(x.n) & (val > 0)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));

    /* if (val_sign != x.sign) return (x.sign < val_sign); */
    // Check 3: if (val_sign != x.sign && x.sign < val_sign) return false;
    CHOOSE_OPTION((curr), ((vsign != x.sign) & (x.sign < vsign)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 4: if (val_sign != x.sign && x.sign > val_sign) return true;
    CHOOSE_OPTION((curr), ((vsign != x.sign) & (x.sign > vsign)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));

    /* if (x.n > 1) return (x.sign == -1) */
    // Check 5: if (x.n > 1 && x.sign == 1) return false;
    CHOOSE_OPTION((curr), ((x.n > 1) & (x.sign == 1)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 5: if (x.n > 1 && x.sign == -1) return true;
    CHOOSE_OPTION((curr), ((x.n > 1) & (x.sign == -1)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));

    /* if (x.limbs[0] < __MAG_I64__(val)) return (x.sign == -1) */
    // Check 6: if (x.limbs[0] < mag_val && x.sign == 1) return false;
    CHOOSE_OPTION((curr), ((x.limbs[0] < mag_val) & (x.sign == 1)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 7: if (x.limbs[0] < mag_val && x.sign == -1) return true;
    CHOOSE_OPTION((curr), ((x.limbs[0] < mag_val) & (x.sign == -1)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));

    /* return (x.limbs[0] > __MAG_I64__(val)) && (x.sign == 1) */
    // Check 8: if (x->limbs[0] > mag_val && (x.sign == -1)) return false;
    CHOOSE_OPTION((curr), ((x.limbs[0] > mag_val) & (x.sign == -1)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    CHOOSE_OPTION((ret), (ret == 2), (1), (ret)); 
    *err = ret_stat; return (bool)(ret);
}
bool crint_lequal_i64(cryptint x, const int64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); x.limbs = 0; x.cap = 0; 
        x.n = 0; x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err);
    DNML_TEST_ASSERT((!(x.poisoned)), poisoined);
    dnml_status ret_stat = CRYPTINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (err == NULL & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    CHOOSE_OPTION((ret_stat), (x.poisoned & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr; int8_t vsign;
    CHOOSE_OPTION((vsign), (val < 0), (-1), (1));
    uint64_t mag_val = __MAG_I64__(val);
    /* if (!x.n) return (val >= 0) ? 1 : 0 */
    // Check 1: if (!x.n && val >= 0) return true
    CHOOSE_OPTION((curr), (!(x.n) & (val >= 0)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    // Check 2: if (!x.n && val < 0) return false
    CHOOSE_OPTION((curr), (!(x.n) & (val < 0)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));

    /* if (val_sign != x.sign) return (x.sign < val_sign); */
    // Check 3: if (val_sign != x.sign && x.sign > val_sign) return false;
    CHOOSE_OPTION((curr), ((vsign != x.sign) & (x.sign > vsign)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 4: if (val_sign != x.sign && x.sign < val_sign) return true;
    CHOOSE_OPTION((curr), ((vsign != x.sign) & (x.sign < vsign)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));

    /* if (x.n > 1) return (x.sign == -1) */
    // Check 5: if (x.n > 1 && x.sign == 1) return false;
    CHOOSE_OPTION((curr), ((x.n > 1) & (x.sign == 1)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 5: if (x.n > 1 && x.sign == -1) return true;
    CHOOSE_OPTION((curr), ((x.n > 1) & (x.sign == -1)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));

    /* if (x.limbs[0] > __MAG_I64__(val)) return (x.sign == -1) */
    // Check 6: if (x.limbs[0] > mag_val && x.sign == 1) return false;
    CHOOSE_OPTION((curr), ((x.limbs[0] > mag_val) & (x.sign == 1)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 7: if (x.limbs[0] > mag_val && x.sign == -1) return true;
    CHOOSE_OPTION((curr), ((x.limbs[0] > mag_val) & (x.sign == -1)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));

    /* return (x.sign == 1) */
    // Check 8: if (x.sign == -1) return false;
    CHOOSE_OPTION((curr), (x.sign == -1), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    CHOOSE_OPTION((ret), (ret == 2), (1), (ret)); 
    *err = ret_stat; return (bool)(ret);
}
bool crint_mequal_i64(cryptint x, const int64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); x.limbs = 0; x.cap = 0; 
        x.n = 0; x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err);
    DNML_TEST_ASSERT((!(x.poisoned)), poisoined);
    dnml_status ret_stat = CRYPTINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (err == NULL & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    CHOOSE_OPTION((ret_stat), (x.poisoned & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr; int8_t vsign;
    CHOOSE_OPTION((vsign), (val < 0), (-1), (1));
    uint64_t mag_val = __MAG_I64__(val);
    /* if (!x.n) return (val <= 0) ? 1 : 0 */
    // Check 1: if (!x.n && val <= 0) return true
    CHOOSE_OPTION((curr), (!(x.n) & (val <= 0)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    // Check 2: if (!x.n && val > 0) return false
    CHOOSE_OPTION((curr), (!(x.n) & (val > 0)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));

    /* if (val_sign != x.sign) return (x.sign > val_sign); */
    // Check 3: if (val_sign != x.sign && x.sign < val_sign) return false;
    CHOOSE_OPTION((curr), ((vsign != x.sign) & (x.sign < vsign)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 4: if (val_sign != x.sign && x.sign > val_sign) return true;
    CHOOSE_OPTION((curr), ((vsign != x.sign) & (x.sign > vsign)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));

    /* if (x.n > 1) return (x.sign == 1) */
    // Check 5: if (x.n > 1 && x.sign == -1) return false;
    CHOOSE_OPTION((curr), ((x.n > 1) & (x.sign == -1)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 5: if (x.n > 1 && x.sign == 1) return true;
    CHOOSE_OPTION((curr), ((x.n > 1) & (x.sign == 1)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));

    /* if (x.limbs[0] > __MAG_I64__(val)) return (x.sign == 1) */
    // Check 6: if (x.limbs[0] > mag_val && x.sign == 1) return false;
    CHOOSE_OPTION((curr), ((x.limbs[0] > mag_val) & (x.sign == -1)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 7: if (x.limbs[0] > mag_val && x.sign == -1) return true;
    CHOOSE_OPTION((curr), ((x.limbs[0] > mag_val) & (x.sign == 1)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));

    /* return (x.sign == -1) */
    // Check 8: if (x.sign == 1) return false;
    CHOOSE_OPTION((curr), (x.sign == 1), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    CHOOSE_OPTION((ret), (ret == 2), (1), (ret)); 
    *err = ret_stat; return (bool)(ret);
}
/* ----------- Unsigned Integer - UI64 ----------- */
bool crint_equal_u64(cryptint x, const uint64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); x.limbs = 0; x.cap = 0; 
        x.n = 0; x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err);
    DNML_TEST_ASSERT((!(x.poisoned)), poisoined);
    dnml_status ret_stat = CRYPTINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (err == NULL & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    CHOOSE_OPTION((ret_stat), (x.poisoned & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr;
    /* if (!x.n) return (val) ? false : true;  */
    // Check 1: if (!x.n && !val) return true
    CHOOSE_OPTION((curr), (!(x.n) & !(val)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    // Check 2: if (!x.n && val) return false
    CHOOSE_OPTION((curr), (!(x.n) & (val)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));

    // Check 3: if (x.sign == -1) return false;
    CHOOSE_OPTION((curr), (x.sign == -1), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 4: if (x.n > 1) return false;
    CHOOSE_OPTION((curr), (x.n > 1), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 5: if (x.limbs[0] != val) return false;
    CHOOSE_OPTION((curr), (x.limbs[0] != val), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    CHOOSE_OPTION((ret), (ret == 2), (1), (ret)); 
    *err = ret_stat; return (bool)(ret);
}
bool crint_less_u64(cryptint x, const uint64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); x.limbs = 0; x.cap = 0; 
        x.n = 0; x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err);
    DNML_TEST_ASSERT((!(x.poisoned)), poisoined);
    dnml_status ret_stat = CRYPTINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (err == NULL & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    CHOOSE_OPTION((ret_stat), (x.poisoned & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr;
    /* if (!x.n) return (val) ? true : false;  */
    // Check 1: if (!x.n && val) return true
    CHOOSE_OPTION((curr), (!(x.n) & (val)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    // Check 2: if (!x.n && !val) return false
    CHOOSE_OPTION((curr), (!(x.n) & !(val)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));

    // Check 3: if (x.sign == -1) return true;
    CHOOSE_OPTION((curr), (x.sign == -1), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    // Check 4: if (x.n > 1) return false;
    CHOOSE_OPTION((curr), (x.n > 1), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 5: if (x.limbs[0] >= val) return false;
    CHOOSE_OPTION((curr), (x.limbs[0] >= val), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    CHOOSE_OPTION((ret), (ret == 2), (1), (ret)); 
    *err = ret_stat; return (bool)(ret);
}
bool crint_more_u64(cryptint x, const uint64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); x.limbs = 0; x.cap = 0; 
        x.n = 0; x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err);
    DNML_TEST_ASSERT((!(x.poisoned)), poisoined);
    dnml_status ret_stat = CRYPTINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (err == NULL & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    CHOOSE_OPTION((ret_stat), (x.poisoned & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr;
    /* if (!x.n) return (val) ? false : true;  */
    // Check 1: if (!x.n && !val) return true
    CHOOSE_OPTION((curr), (!(x.n) & !(val)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    // Check 2: if (!x.n && val) return false
    CHOOSE_OPTION((curr), (!(x.n) & (val)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));

    // Check 3: if (x.sign == -1) return false;
    CHOOSE_OPTION((curr), (x.sign == -1), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 4: if (x.n > 1) return true;
    CHOOSE_OPTION((curr), (x.n > 1), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    // Check 5: if (x.limbs[0] <= val) return false;
    CHOOSE_OPTION((curr), (x.limbs[0] <= val), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    CHOOSE_OPTION((ret), (ret == 2), (1), (ret)); 
    *err = ret_stat; return (bool)(ret);
}
bool crint_lequal_u64(cryptint x, const uint64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); x.limbs = 0; x.cap = 0; 
        x.n = 0; x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err);
    DNML_TEST_ASSERT((!(x.poisoned)), poisoined);
    dnml_status ret_stat = CRYPTINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (err == NULL & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    CHOOSE_OPTION((ret_stat), (x.poisoned & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr;
    // Check 1: if (!x.n) return true
    CHOOSE_OPTION((curr), (!(x.n)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    // Check 2: if (x.sign == -1) return true;
    CHOOSE_OPTION((curr), (x.sign == -1), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    // Check 4: if (x.n > 1) return false;
    CHOOSE_OPTION((curr), (x.n > 1), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 5: if (x.limbs[0] > val) return false;
    CHOOSE_OPTION((curr), (x.limbs[0] > val), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    CHOOSE_OPTION((ret), (ret == 2), (1), (ret)); 
    *err = ret_stat; return (bool)(ret);
}
bool crint_mequal_u64(cryptint x, const uint64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); x.limbs = 0; x.cap = 0; 
        x.n = 0; x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err);
    DNML_TEST_ASSERT((!(x.poisoned)), poisoined);
    dnml_status ret_stat = CRYPTINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (err == NULL & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    CHOOSE_OPTION((ret_stat), (x.poisoned & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr;
    /* if (!x.n) return (val) ? false : true;  */
    // Check 1: if (!x.n && !val) return true
    CHOOSE_OPTION((curr), (!(x.n) & !(val)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    // Check 2: if (!x.n && val) return false
    CHOOSE_OPTION((curr), (!(x.n) & (val)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));

    // Check 3: if (x.sign == -1) return false;
    CHOOSE_OPTION((curr), (x.sign == -1), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 4: if (x.n > 1) return true;
    CHOOSE_OPTION((curr), (x.n > 1), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    // Check 5: if (x.limbs[0] < val) return false;
    CHOOSE_OPTION((curr), (x.limbs[0] < val), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    CHOOSE_OPTION((ret), (ret == 2), (1), (ret)); 
    *err = ret_stat; return (bool)(ret);
}
/* ------------------- Cryptint ------------------ */
bool crint_equal(cryptint x, cryptint y, dnml_status *err) {
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
    DNML_TEST_ASSERT((err != NULL), null_err);
    DNML_TEST_ASSERT((!(x.poisoned)), poisoined);
    dnml_status ret_stat = CRYPTINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (err == NULL & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    CHOOSE_OPTION((ret_stat), (x.poisoned & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr;
    /* if (!x.n) return (val) ? false : true;  */
    // Check 1: if (!x.n && !y.n) return true
    CHOOSE_OPTION((curr), (!(x.n) & !(y.n)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    // Check 2: if (!x.n && y.n) return false
    CHOOSE_OPTION((curr), (!(x.n) & (y.n)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));

    /* if (a.sign != b.sign) return false; */
    CHOOSE_OPTION((curr), (x.sign != y.sign), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    /* if (a.n != b.n) return false; */
    CHOOSE_OPTION((curr), (x.n != y.n), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    /* if (a.limbs == b.limbs) return true; */
    CHOOSE_OPTION((curr), (x.limbs == y.limbs), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));

    /* Main Validation Loop */
    int8_t mag_ret = __CRINT_MAGCMP__(&x, &y);
    CHOOSE_OPTION((curr), (!(mag_ret)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    CHOOSE_OPTION((ret), (ret == 2), (0), (ret));
    *err = ret_stat; return (bool)(ret);
}
bool crint_less(cryptint x, cryptint y, dnml_status *err) {
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
    DNML_TEST_ASSERT((err != NULL), null_err);
    DNML_TEST_ASSERT((!(x.poisoned)), poisoined);
    dnml_status ret_stat = CRYPTINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (err == NULL & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    CHOOSE_OPTION((ret_stat), (x.poisoned & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr;
    /* if (x.sign != y.sign) return (x.sign < y.sign) */
    // Check 1: if (x.sign != y.sign & x.sign > y.sign) return false
    CHOOSE_OPTION((curr), ((x.sign != y.sign) & (x.sign > y.sign)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 2: if (x.sign != y.sign & x.sign < y.sign) return true
    CHOOSE_OPTION((curr), ((x.sign != y.sign) & (x.sign < y.sign)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));

    /* if (x.n != y.n) return (x.sign == 1) ? (x.n < y.n) : (x.n > y.n); */
    // Check 3: if (x.n != y.n && x.sign == 1 && x.n > y.n) return false
    CHOOSE_OPTION((curr), ((x.n != y.n) & (x.sign == 1) & (x.n > y.n)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 4: if (x.n != y.n && x.sign == 1 && x.n < y.n) return true
    CHOOSE_OPTION((curr), ((x.n != y.n) & (x.sign == 1) & (x.n < y.n)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    // Check 5: if (x.n != y.n && x.sign == -1 && x.n < y.n) return false
    CHOOSE_OPTION((curr), ((x.n != y.n) & (x.sign == -1) & (x.n < y.n)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 6: if (x.n != y.n && x.sign == -1 && x.n > y.n) return true
    CHOOSE_OPTION((curr), ((x.n != y.n) & (x.sign == -1) & (x.n > y.n)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));

    /* if (a.limbs == b.limbs) return false; */
    CHOOSE_OPTION((curr), (x.limbs == y.limbs), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));

    /* Main Validation Loop */
    /* return (a.sign == 1) ? __CRINT_MAGCMP__(&x, &y) < 0 : __CRINT_MAGCMP__(&x, &y) > 0; */
    int8_t mag_ret = __CRINT_MAGCMP__(&x, &y);
    // Check 7+8: return (a.sign == 1 && mag_ret < 0);
    CHOOSE_OPTION((curr), ((x.sign == 1) & (mag_ret >= 0)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    CHOOSE_OPTION((curr), ((x.sign == 1) & (mag_ret < 0)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    // Check 9+10: return (a.sign == -1 && mag_ret > 0);
    CHOOSE_OPTION((curr), ((x.sign == -1) & (mag_ret <= 0)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    CHOOSE_OPTION((curr), ((x.sign == -1) & (mag_ret > 0)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    // Check 11: mag_ret == 0
    CHOOSE_OPTION((ret), (ret == 2), (0), (ret));
    *err = ret_stat; return (bool)(ret);
}
bool crint_more(cryptint x, cryptint y, dnml_status *err) {
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
    DNML_TEST_ASSERT((err != NULL), null_err);
    DNML_TEST_ASSERT((!(x.poisoned)), poisoined);
    dnml_status ret_stat = CRYPTINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (err == NULL & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    CHOOSE_OPTION((ret_stat), (x.poisoned & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr;
    /* if (x.sign != y.sign) return (x.sign > y.sign) */
    // Check 1: if (x.sign != y.sign & x.sign < y.sign) return false
    CHOOSE_OPTION((curr), ((x.sign != y.sign) & (x.sign < y.sign)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 2: if (x.sign != y.sign & x.sign > y.sign) return true
    CHOOSE_OPTION((curr), ((x.sign != y.sign) & (x.sign > y.sign)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));

    /* if (x.n != y.n) return (x.sign == 1) ? (x.n > y.n) : (x.n < y.n); */
    // Check 3: if (x.n != y.n && x.sign == 1 && x.n < y.n) return false
    CHOOSE_OPTION((curr), ((x.n != y.n) & (x.sign == 1) & (x.n < y.n)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 4: if (x.n != y.n && x.sign == 1 && x.n > y.n) return true
    CHOOSE_OPTION((curr), ((x.n != y.n) & (x.sign == 1) & (x.n > y.n)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    // Check 5: if (x.n != y.n && x.sign == -1 && x.n > y.n) return false
    CHOOSE_OPTION((curr), ((x.n != y.n) & (x.sign == -1) & (x.n > y.n)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 6: if (x.n != y.n && x.sign == -1 && x.n < y.n) return true
    CHOOSE_OPTION((curr), ((x.n != y.n) & (x.sign == -1) & (x.n < y.n)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));

    /* if (a.limbs == b.limbs) return false; */
    CHOOSE_OPTION((curr), (x.limbs == y.limbs), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));

    /* Main Validation Loop */
    /* return (a.sign == 1) ? __CRINT_MAGCMP__(&x, &y) > 0 : __CRINT_MAGCMP__(&x, &y) < 0; */
    int8_t mag_ret = __CRINT_MAGCMP__(&x, &y);
    // Check 7+8: return (a.sign == 1 && mag_ret > 0);
    CHOOSE_OPTION((curr), ((x.sign == 1) & (mag_ret <= 0)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    CHOOSE_OPTION((curr), ((x.sign == 1) & (mag_ret > 0)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    // Check 9+10: return (a.sign == -1 && mag_ret < 0);
    CHOOSE_OPTION((curr), ((x.sign == -1) & (mag_ret >= 0)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    CHOOSE_OPTION((curr), ((x.sign == -1) & (mag_ret < 0)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    // Check 11: mag_ret == 0
    CHOOSE_OPTION((ret), (ret == 2), (0), (ret));
    *err = ret_stat; return (bool)(ret);
}
bool crint_lequal(cryptint x, cryptint y, dnml_status *err) {
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
    DNML_TEST_ASSERT((err != NULL), null_err);
    DNML_TEST_ASSERT((!(x.poisoned)), poisoined);
    dnml_status ret_stat = CRYPTINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (err == NULL & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    CHOOSE_OPTION((ret_stat), (x.poisoned & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr;
    /* if (x.sign != y.sign) return (x.sign < y.sign) */
    // Check 1: if (x.sign != y.sign & x.sign > y.sign) return false
    CHOOSE_OPTION((curr), ((x.sign != y.sign) & (x.sign > y.sign)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 2: if (x.sign != y.sign & x.sign < y.sign) return true
    CHOOSE_OPTION((curr), ((x.sign != y.sign) & (x.sign < y.sign)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));

    /* if (x.n != y.n) return (x.sign == 1) ? (x.n < y.n) : (x.n > y.n); */
    // Check 3: if (x.n != y.n && x.sign == 1 && x.n > y.n) return false
    CHOOSE_OPTION((curr), ((x.n != y.n) & (x.sign == 1) & (x.n > y.n)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 4: if (x.n != y.n && x.sign == 1 && x.n < y.n) return true
    CHOOSE_OPTION((curr), ((x.n != y.n) & (x.sign == 1) & (x.n < y.n)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    // Check 5: if (x.n != y.n && x.sign == -1 && x.n < y.n) return false
    CHOOSE_OPTION((curr), ((x.n != y.n) & (x.sign == -1) & (x.n < y.n)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 6: if (x.n != y.n && x.sign == -1 && x.n > y.n) return true
    CHOOSE_OPTION((curr), ((x.n != y.n) & (x.sign == -1) & (x.n > y.n)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));

    /* if (a.limbs == b.limbs) return true; */
    CHOOSE_OPTION((curr), (x.limbs == y.limbs), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));

    /* Main Validation Loop */
    /* return (a.sign == 1) ? __CRINT_MAGCMP__(&x, &y) < 0 : __CRINT_MAGCMP__(&x, &y) > 0; */
    int8_t mag_ret = __CRINT_MAGCMP__(&x, &y);
    // Check 7+8: return (a.sign == 1 && mag_ret <= 0);
    CHOOSE_OPTION((curr), ((x.sign == 1) & (mag_ret > 0)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    CHOOSE_OPTION((curr), ((x.sign == 1) & (mag_ret <= 0)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    // Check 9+10: return (a.sign == -1 && mag_ret >= 0);
    CHOOSE_OPTION((curr), ((x.sign == -1) & (mag_ret < 0)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    CHOOSE_OPTION((curr), ((x.sign == -1) & (mag_ret >= 0)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    *err = ret_stat; return (bool)(ret);
}
bool crint_mequal(cryptint x, cryptint y, dnml_status *err) {
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
    DNML_TEST_ASSERT((err != NULL), null_err);
    DNML_TEST_ASSERT((!(x.poisoned)), poisoined);
    dnml_status ret_stat = CRYPTINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (err == NULL & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    CHOOSE_OPTION((ret_stat), (x.poisoned & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    /* Main Operation - Comparison */
    uint8_t ret = 2, curr;
    /* if (x.sign != y.sign) return (x.sign > y.sign) */
    // Check 1: if (x.sign != y.sign & x.sign < y.sign) return false
    CHOOSE_OPTION((curr), ((x.sign != y.sign) & (x.sign < y.sign)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 2: if (x.sign != y.sign & x.sign > y.sign) return true
    CHOOSE_OPTION((curr), ((x.sign != y.sign) & (x.sign > y.sign)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));

    /* if (x.n != y.n) return (x.sign == 1) ? (x.n > y.n) : (x.n < y.n); */
    // Check 3: if (x.n != y.n && x.sign == 1 && x.n < y.n) return false
    CHOOSE_OPTION((curr), ((x.n != y.n) & (x.sign == 1) & (x.n < y.n)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 4: if (x.n != y.n && x.sign == 1 && x.n > y.n) return true
    CHOOSE_OPTION((curr), ((x.n != y.n) & (x.sign == 1) & (x.n > y.n)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    // Check 5: if (x.n != y.n && x.sign == -1 && x.n > y.n) return false
    CHOOSE_OPTION((curr), ((x.n != y.n) & (x.sign == -1) & (x.n > y.n)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    // Check 6: if (x.n != y.n && x.sign == -1 && x.n < y.n) return true
    CHOOSE_OPTION((curr), ((x.n != y.n) & (x.sign == -1) & (x.n < y.n)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));

    /* if (a.limbs == b.limbs) return true; */
    CHOOSE_OPTION((curr), (x.limbs == y.limbs), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));

    /* Main Validation Loop */
    /* return (a.sign == 1) ? __CRINT_MAGCMP__(&x, &y) >= 0 : __CRINT_MAGCMP__(&x, &y) <= 0; */
    int8_t mag_ret = __CRINT_MAGCMP__(&x, &y);
    // Check 7+8: return (a.sign == 1 && mag_ret >= 0);
    CHOOSE_OPTION((curr), ((x.sign == 1) & (mag_ret < 0)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    CHOOSE_OPTION((curr), ((x.sign == 1) & (mag_ret >= 0)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    // Check 9+10: return (a.sign == -1 && mag_ret <= 0);
    CHOOSE_OPTION((curr), ((x.sign == -1) & (mag_ret > 0)), (0), (1));
    CHOOSE_OPTION((ret), (!(curr) & (ret == 2)), (curr), (ret));
    CHOOSE_OPTION((curr), ((x.sign == -1) & (mag_ret <= 0)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (ret == 2)), (curr), (ret));
    *err = ret_stat; return (bool)(ret);
}





//* ================================================= COPIES ================================================= */
/* -------------  Mutative SMALL Copies ------------- */
dnml_status crint_mut_copyu64(cryptint *dst, const uint64_t src) {
    // Pre-operation Validation & Static Analysis
    DNML_ASSERT(__STORAGE_VAL__(dst), store_inval, {
        if (dst->limbs != NULL) *(dst->limbs) = 0;
        dst->limbs = 0; dst->cap = 0; dst->n = 0; 
        dst->poisoned = 0; dst->sign = 0;
    });
    DNML_TEST_ASSERT((!(dst->poisoned)), poisoined);
    dnml_status ret_stat = CRYPTINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (dst->poisoned), (CRYPTINT_POISOINED), (ret_stat));
    /* Main Operation - Copy */
    crint_canonicalize(dst); dst->limbs[0] = src;
    dst->n = !!(src); dst->sign = 1; 
    __libdnml_memset_strict(
        dst->limbs, 0, 
        dst->cap * BYTES_IN_UINT64_T, 1, 
        (dst->cap - 1) * BYTES_IN_UINT64_T + (BYTES_IN_UINT64_T - 1)
    ); return ret_stat;
}
dnml_status crint_mut_dcopyu64(cryptint *dst, const uint64_t src) {
    // Pre-operation Validation & Static Analysis
    DNML_ASSERT(__STORAGE_VAL__(dst), store_inval, {
        if (dst->limbs != NULL) *(dst->limbs) = 0;
        dst->limbs = 0; dst->cap = 0; dst->n = 0; 
        dst->poisoned = 0; dst->sign = 0;
    });
    DNML_TEST_ASSERT((!(dst->poisoned)), poisoined);
    dnml_status ret_stat = CRYPTINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (dst->poisoned), (CRYPTINT_POISOINED), (ret_stat));

    /* Main Operation - Copy */
    crint_canonicalize(dst); limb_t TMP_P = 1;
    limb_t *__BUFFER_P = realloc(dst->limbs, BYTES_IN_UINT64_T);
    DNML_TEST_ASSERT(__BUFFER_P != NULL, realloc_null);
    CHOOSE_OPTION((ret_stat), 
        (__BUFFER_P == NULL & 
        (ret_stat = CRYPTINT_SUCCESS)), 
        (DNML_ALLOC_OOM), (CRYPTINT_SUCCESS)
    );
    uint64_t oom; CHOOSE_OPTION((oom), (__BUFFER_P == NULL), (0), (UINT64_MAX));
    dst->limbs = (__BUFFER_P == NULL) ? TMP_P : __BUFFER_P; // Forced to use ternary operator
    dst->limbs[0] = src; dst->n = !!(src); dst->cap = 1; dst->sign = 1;

    /* Invalid Metadata Fill */ 
    dst->limbs[0] &= oom; dst->n &= oom; dst->cap &= oom;
    dst->sign &= oom; dst->poisoned &= oom;
    return ret_stat;
}
dnml_status crint_mut_copyi64(cryptint *dst, const int64_t src) {
    // Pre-operation Validation & Static Analysis
    DNML_ASSERT(__STORAGE_VAL__(dst),
    "Partial Contract Violation: CryptInt invalid for storage "
    "(-Ecrypt_int_sinvalid)", {
        if (dst->limbs != NULL) *(dst->limbs) = 0;
        dst->limbs = 0; dst->cap = 0; dst->n = 0; 
        dst->poisoned = 0; dst->sign = 0;
    });
    DNML_TEST_ASSERT((!(dst->poisoned)), poisoined);
    dnml_status ret_stat = CRYPTINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (dst->poisoned), (CRYPTINT_POISOINED), (ret_stat))
    /* Main Operation - Copy */
    crint_canonicalize(dst); dst->limbs[0] = __MAG_I64__(src); 
    dst->n = !!(src); CHOOSE_OPTION((dst->sign), (src < 0), (-1), (1));
    __libdnml_memset_strict(
        dst->limbs, 0,
        dst->cap * BYTES_IN_UINT64_T, 1, 
        (dst->cap - 1) * BYTES_IN_UINT64_T + (BYTES_IN_UINT64_T - 1)
    ); return ret_stat;
}
dnml_status crint_mut_dcopyi64(cryptint *dst, const int64_t src) {
    // Pre-operation Validation & Static Analysis
    DNML_ASSERT(__STORAGE_VAL__(dst), store_inval, {
        if (dst->limbs != NULL) *(dst->limbs) = 0;
        dst->limbs = 0; dst->cap = 0; dst->n = 0; 
        dst->poisoned = 0; dst->sign = 0;
    });
    DNML_TEST_ASSERT((!(dst->poisoned)), poisoined);
    dnml_status ret_stat = CRYPTINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (dst->poisoned), (CRYPTINT_POISOINED), (ret_stat));

    /* Main Operation - Copy */
    crint_canonicalize(dst); limb_t TMP_P = 1;
    limb_t *__BUFFER_P = realloc(dst->limbs, BYTES_IN_UINT64_T);
    DNML_TEST_ASSERT(__BUFFER_P != NULL, realloc_null);
    CHOOSE_OPTION((ret_stat), 
        (__BUFFER_P == NULL & 
        (ret_stat == CRYPTINT_SUCCESS)), 
        (DNML_ALLOC_OOM), (CRYPTINT_SUCCESS)
    );
    uint64_t oom; CHOOSE_OPTION((oom), (__BUFFER_P == NULL), (0), (UINT64_MAX));
    dst->limbs = (__BUFFER_P == NULL) ? TMP_P : __BUFFER_P; // Forced to use ternary operator
    dst->limbs[0] = __MAG_I64__(src); dst->n = !!(src); dst->cap = 1;
    CHOOSE_OPTION((dst->sign), (src < 0), (-1), (1));

    /* Invalid Metadata Fill */ 
    dst->limbs[0] &= oom; dst->n &= oom; dst->cap &= oom;
    dst->sign &= oom; dst->poisoned &= oom;
    return ret_stat;
}
/* -------------  Mutative LARGE Copies ------------- */
dnml_status crint_mut_copyf128(cryptint *dst, long double src) {}
dnml_status crint_mut_dcopyf128(cryptint *dst, long double src) {}
dnml_status crint_mut_ocopyf128(cryptint *dst, long double src) {}
dnml_status crint_mut_tover_copyf128(cryptint *dst, long double src) {}
dnml_status crint_mut_copy(cryptint *dst, cryptint src) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(src)), full_contract,
    { /* Set everything to 0 to completely wipe out memory */
        if (src.limbs != NULL) *(src.limbs) = 0;
        crint_free(&src); dst->limbs = 0; dst->n = 0;
        dst->cap = 0; dst->sign = 0; dst->poisoned = 0;
    });
    DNML_ASSERT((__STORAGE_VAL__(dst)), store_inval,
    { /* Set everything to 0 to completely wipe out memory */
        __libdnml_memwipe_strict(src.limbs, src.cap * BYTES_IN_UINT64_T);
        crint_free(&src); dst->limbs = 0; dst->n = 0;
        dst->cap = 0; dst->sign = 0; dst->poisoned = 0;
    }); DNML_TEST_ASSERT((!(dst->poisoned)) && (!(src.poisoned)), poisoined);
    dnml_status ret_stat = CRYPTINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (dst->poisoned), (CRYPTINT_POISOINED), (ret_stat));

    /* Main Operation - Copy */
    crint_canonicalize(dst); dnml_status rcap_stat; uint64_t correctly_set;
    if (dst->cap < src.n) {
        rcap_stat = crint_reserve(dst, src.n);
        CHOOSE_OPTION((ret_stat), 
            (rcap_stat == DNML_ALLOC_OOM &
            (ret_stat == CRYPTINT_SUCCESS)),
            (rcap_stat), (ret_stat)
        );
    }
    CHOOSE_OPTION((correctly_set), (ret_stat != DNML_ALLOC_OOM), (UINT64_MAX), (0));
    limb_t dst_buf[512] = {0}, src_buf[512] = {0}; size_t end;
    limb_t *dst_limbs = (ret_stat != DNML_ALLOC_OOM) ? dst->limbs : dst_buf;
    const limb_t *src_limbs = (ret_stat != DNML_ALLOC_OOM) ? src.limbs : src_buf;
    CHOOSE_OPTION((dst->cap), (ret_stat != DNML_ALLOC_OOM), (dst->cap), (512));
    CHOOSE_OPTION((end), (ret_stat != DNML_ALLOC_OOM), (src.n), (512));
    dst->n = src.n; dst->sign = src.sign;
    __libdnml_memcpy_strict(
        dst_limbs, src_limbs, dst->cap * BYTES_IN_UINT64_T, 
        0, (end - 1) * BYTES_IN_UINT64_T + (BYTES_IN_UINT64_T - 1)
    );
    size_t clear_start, clear_end;
    CHOOSE_OPTION((clear_start), (ret_stat != DNML_ALLOC_OOM), (dst->n), (0));
    CHOOSE_OPTION((clear_end), (ret_stat != DNML_ALLOC_OOM), (dst->cap), (512));
    __libdnml_memset_strict(
        dst_limbs, 0, dst->cap * BYTES_IN_UINT64_T, 
        clear_start * BYTES_IN_UINT64_T,
        (clear_end - 1) * BYTES_IN_UINT64_T + (BYTES_IN_UINT64_T - 1)
    ); 
    
    /* Invalid Metadata Wiping */
    dst->poisoned &= correctly_set; dst->sign &= correctly_set;
    dst->n &= correctly_set; dst->cap &= correctly_set; 
    return ret_stat;
}
dnml_status crint_mut_dcopy(cryptint *dst, cryptint src) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(src)), full_contract,
    { /* Set everything to 0 to completely wipe out memory */
        if (src.limbs != NULL) *(src.limbs) = 0;
        crint_free(&src); dst->limbs = 0; dst->n = 0;
        dst->cap = 0; dst->sign = 0; dst->poisoned = 0;
    });
    DNML_ASSERT((__STORAGE_VAL__(dst)), store_inval,
    { /* Set everything to 0 to completely wipe out memory */
        __libdnml_memwipe_strict(src.limbs, src.cap * BYTES_IN_UINT64_T);
        crint_free(&src); dst->limbs = 0; dst->n = 0;
        dst->cap = 0; dst->sign = 0; dst->poisoned = 0;
    }); DNML_TEST_ASSERT((!(dst->poisoned)) && (!(src.poisoned)), poisoined);
    dnml_status ret_stat = CRYPTINT_SUCCESS; CHOOSE_OPTION(
        (ret_stat), (dst->poisoned | src.poisoned), 
        (CRYPTINT_POISOINED), (ret_stat)
    );

    /* Main Operation - Copy */
    crint_canonicalize(dst); dnml_status resize_stat;
    resize_stat = crint_dnml_resize(dst, src.n);
    CHOOSE_OPTION((ret_stat), 
        (resize_stat == DNML_ALLOC_OOM & 
        (ret_stat == CRYPTINT_SUCCESS)), 
        (resize_stat), (ret_stat)
    );
    uint64_t oom_filter;
    limb_t dst_buf[512] = {0}, src_buf[512] = {0}; size_t end;
    limb_t *dst_limbs = (ret_stat != DNML_ALLOC_OOM) ? dst->limbs : dst_buf;
    const limb_t *src_limbs = (ret_stat != DNML_ALLOC_OOM) ? src.limbs : src_buf;
    CHOOSE_OPTION((dst->cap), (ret_stat != DNML_ALLOC_OOM), (dst->cap), (512));
    CHOOSE_OPTION((end), (ret_stat != DNML_ALLOC_OOM), (src.n), (512));
    CHOOSE_OPTION((oom_filter), (ret_stat != DNML_ALLOC_OOM), (UINT64_MAX), (0));
    dst->n = src.n; dst->sign = src.sign;
    __libdnml_memcpy_strict(
        dst_limbs, src_limbs, dst->cap * BYTES_IN_UINT64_T,
        0, (end - 1) * BYTES_IN_UINT64_T + (BYTES_IN_UINT64_T - 1)
    ); 
    /* Clearing Out Values to Invalidity */
    dst->n &= oom_filter; dst->cap &= oom_filter;
    dst->poisoned &= oom_filter; dst->sign &= oom_filter;
    return ret_stat;
}
dnml_status crint_mut_ocopy(cryptint *dst, cryptint src) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(src)), full_contract,
    { /* Set everything to 0 to completely wipe out memory */
        if (src.limbs != NULL) *(src.limbs) = 0;
        crint_free(&src); dst->limbs = 0; dst->n = 0;
        dst->cap = 0; dst->sign = 0; dst->poisoned = 0;
    });
    DNML_ASSERT((__STORAGE_VAL__(dst)), store_inval,
    { /* Set everything to 0 to completely wipe out memory */
        __libdnml_memwipe_strict(src.limbs, src.cap * BYTES_IN_UINT64_T);
        crint_free(&src); dst->limbs = 0; dst->n = 0;
        dst->cap = 0; dst->sign = 0; dst->poisoned = 0;
    }); DNML_TEST_ASSERT((!(dst->poisoned)) && (!(src.poisoned)), poisoined);
    dnml_status ret_stat = CRYPTINT_SUCCESS; CHOOSE_OPTION(
        (ret_stat), (dst->poisoned | src.poisoned), 
        (CRYPTINT_POISOINED), (ret_stat)
    );

    /* Main Operation - Copy */
    CHOOSE_OPTION(
        (ret_stat), ((dst->cap < src.n) & (ret_stat == CRYPTINT_SUCCESS)),
        (CRYPTINT_ERR_RANGE), (ret_stat)
    );
    limb_t dst_buf[512] = {0}, src_buf[512] = {0};
    limb_t *dst_limbs = (ret_stat != CRYPTINT_ERR_RANGE) ? dst->limbs : dst_buf;
    const limb_t *src_limbs = (ret_stat != CRYPTINT_ERR_RANGE) ? src.limbs : src_buf;
    size_t cap_end, copy_end, clean_start;
    CHOOSE_OPTION((cap_end), (ret_stat != CRYPTINT_ERR_RANGE), (dst->cap), (512));
    CHOOSE_OPTION((copy_end), (ret_stat != CRYPTINT_ERR_RANGE), (src.n), (512));
    CHOOSE_OPTION((clean_start), (ret_stat != CRYPTINT_ERR_RANGE), (dst->n), (0));

    __libdnml_memcpy_strict(
        dst->limbs, src.limbs, cap_end * BYTES_IN_UINT64_T, 
        0, (copy_end - 1) * BYTES_IN_UINT64_T + (BYTES_IN_UINT64_T - 1)
    );
    dst->n = src.n; dst->sign = src.sign;
    __libdnml_memset_strict(
        dst->limbs, 0, cap_end * BYTES_IN_UINT64_T,
        clean_start * BYTES_IN_UINT64_T,
        (cap_end - 1) * BYTES_IN_UINT64_T + (BYTES_IN_UINT64_T - 1)
    ); return ret_stat;
}
dnml_status crint_mut_tover_copy(cryptint *dst, cryptint src) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(src)), full_contract,
    { /* Set everything to 0 to completely wipe out memory */
        if (src.limbs != NULL) *(src.limbs) = 0;
        crint_free(&src); dst->limbs = 0; dst->n = 0;
        dst->cap = 0; dst->sign = 0; dst->poisoned = 0;
    });
    DNML_ASSERT((__STORAGE_VAL__(dst)), store_inval,
    { /* Set everything to 0 to completely wipe out memory */
        __libdnml_memwipe_strict(src.limbs, src.cap * BYTES_IN_UINT64_T);
        crint_free(&src); dst->limbs = 0; dst->n = 0;
        dst->cap = 0; dst->sign = 0; dst->poisoned = 0;
    }); DNML_TEST_ASSERT((!(dst->poisoned)) && (!(src.poisoned)), poisoined);
    dnml_status ret_status = CRYPTINT_SUCCESS; CHOOSE_OPTION(
        (ret_status), (dst->poisoned | src.poisoned), 
        (CRYPTINT_POISOINED), (ret_status)
    );

    /* Main Operation - Copy */
    size_t op_range; CHOOSE_OPTION((op_range), (dst->cap < src.n), (dst->cap), (src.n));
    __libdnml_memcpy_strict(
        dst->limbs, src.limbs, dst->cap * BYTES_IN_UINT64_T,
        0, (op_range - 1) * BYTES_IN_UINT64_T + (BYTES_IN_UINT64_T - 1)
    ); dst->n = op_range; dst->sign = src.sign;
    __libdnml_memset_strict(
        dst->limbs, 0,
        (dst->cap) * BYTES_IN_UINT64_T,
        dst->n * BYTES_IN_UINT64_T, 
        (dst->cap - 1) * BYTES_IN_UINT64_T + (BYTES_IN_UINT64_T - 1)
    ); return ret_status;
}
/* -------------  Functional SMALL Copies ------------- */
cryptint crint_copyu64(const uint64_t src, dnml_status *err) {
    cryptint dst; dnml_status new_stat; uint64_t correctly_set;
    limb_t dst_tmp_p[1] = {0}; new_stat = crint_new(&dst);
    dst.limbs = (new_stat != DNML_ALLOC_OOM) ? dst.limbs : dst_tmp_p;
    CHOOSE_OPTION((correctly_set), (new_stat != DNML_ALLOC_OOM), (UINT64_MAX), (0));
    /* Setting Up correctly - Standard Case */
    dst.limbs[0] = src; dst.n = !!(src);
    dst.cap = 1; dst.sign = 1;
    /* Setting Up invalid metadata - DNML_ALLOC_OOM */
    dst.limbs[0] &= correctly_set; dst.n &= correctly_set;
    dst.cap &= correctly_set; dst.sign &= correctly_set;
    CHOOSE_OPTION((*err), (new_stat != DNML_ALLOC_OOM), (CRYPTINT_SUCCESS), (DNML_ALLOC_OOM));
    return dst;
}
cryptint crint_copyi64(const int64_t src, dnml_status *err) {
    cryptint dst; dnml_status new_stat; uint64_t correctly_set;
    limb_t dst_tmp_p[1] = {0}; new_stat = crint_new(&dst);
    dst.limbs = (new_stat != DNML_ALLOC_OOM) ? dst.limbs : dst_tmp_p;
    CHOOSE_OPTION((correctly_set), (new_stat != DNML_ALLOC_OOM), (UINT64_MAX), (0));
    /* Setting Up correctly - Standard Case */
    dst.limbs[0] = __MAG_I64__(src); dst.n = !!(src); dst.cap = 1;
    CHOOSE_OPTION((dst.sign), (src < 0), (-1), (1));
    /* Setting Up invalid metadata - DNML_ALLOC_OOM */
    dst.limbs[0] &= correctly_set; dst.n &= correctly_set;
    dst.cap &= correctly_set; dst.sign &= correctly_set;
    CHOOSE_OPTION((*err), (new_stat != DNML_ALLOC_OOM), (CRYPTINT_SUCCESS), (DNML_ALLOC_OOM));
    return dst;
}
/* -------------  Functional LARGE Copies ------------- */
cryptint crint_copyf128(long double src, dnml_status *err) {}
cryptint crint_ocopyf128(long double src, size_t output_cap, dnml_status *err) {}
cryptint crint_tover_copyf128(long double src, size_t output_cap, dnml_status *err) {}
cryptint crint_copy(cryptint src, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(src)), full_contract,
    { /* Set everything to 0 to completely wipe out memory */
        if (src.limbs != NULL) *(src.limbs) = 0;
        crint_free(&src);
    });
    DNML_TEST_ASSERT((err != NULL), null_err);
    DNML_TEST_ASSERT((!(src.poisoned)), poisoined);
    dnml_status ret_stat = CRYPTINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (err == NULL & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    CHOOSE_OPTION((ret_stat), (src.poisoned & ret_stat == CRYPTINT_SUCCESS), (CRYPTINT_POISOINED), (ret_stat));

    /* Main Operation - Copy */
    cryptint dst; dnml_status new_stat; uint64_t correctly_set;
    limb_t dst_tmp_p[512] = {0}, src_tmp_p[512] = {0};
    new_stat = crint_snew(&dst, src.n); // Guaraneed dst->cap >= 1
    dst.limbs = (new_stat != DNML_ALLOC_OOM) ? dst.limbs : dst_tmp_p;
    CHOOSE_OPTION((correctly_set), (new_stat != DNML_ALLOC_OOM), (UINT64_MAX), (0));
    const limb_t* src_limbs = (new_stat != DNML_ALLOC_OOM) ? src.limbs : src_tmp_p;

    /* Setting Up correctly - Standard Case */
    size_t iter_cnt = (size_t)(src.n / 512 + 1);
    size_t end; CHOOSE_OPTION(
        (end), (!(src.n)), 0, /* 2nd option: Aligns to the last byte */
        ((src.n - 1) * BYTES_IN_UINT64_T + (BYTES_IN_UINT64_T - 1))
    ); CHOOSE_OPTION((end), (new_stat != DNML_ALLOC_OOM), (end), (511));
    CHOOSE_OPTION((dst.cap), (new_stat != DNML_ALLOC_OOM), (dst.cap), (512));
    while (iter_cnt--) __libdnml_memcpy_strict(dst.limbs, src.limbs, dst.cap, 0, end);
    dst.n = src.n; dst.sign = src.sign; *err = CRYPTINT_SUCCESS;

    /* Setting Up invalid metadata - DNML_ALLOC_OOM */
    dst.limbs[0] &= correctly_set; dst.n &= correctly_set;
    dst.cap &= correctly_set; dst.sign &= correctly_set;
    CHOOSE_OPTION((ret_stat), (new_stat != DNML_ALLOC_OOM & (ret_stat == CRYPTINT_SUCCESS)), (new_stat), (ret_stat));
    *err = ret_stat; return dst;
}
cryptint crint_ocopy(cryptint src, size_t output_cap, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(src)), full_contract,
    { /* Set everything to 0 to completely wipe out memory */
        if (src.limbs != NULL) *(src.limbs) = 0;
        crint_free(&src);
    });
    DNML_TEST_ASSERT((err != NULL), null_err);
    DNML_TEST_ASSERT((!(src.poisoned)), poisoined);
    dnml_status ret_stat = CRYPTINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (err == NULL & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    CHOOSE_OPTION((ret_stat), (src.poisoned & ret_stat == CRYPTINT_SUCCESS), (CRYPTINT_POISOINED), (ret_stat));

    /* Main Operation - Copy */
    cryptint dst; uint64_t correctly_set; dnml_status new_stat;
    CHOOSE_OPTION((ret_stat), (output_cap < src.n), (CRYPTINT_ERR_RANGE), (ret_stat));
    limb_t dst_tmp_p[512] = {0}, src_tmp_p[512] = {0};
    new_stat = crint_snew(&dst, src.n); // Guaraneed dst->cap >= 1
    CHOOSE_OPTION((ret_stat), ((new_stat != CRYPTINT_SUCCESS) & (ret_stat = CRYPTINT_SUCCESS)), (new_stat), (ret_stat));
    CHOOSE_OPTION((correctly_set), (ret_stat != DNML_ALLOC_OOM), (UINT64_MAX), (0));
    limb_t* dst_limbs = (ret_stat != DNML_ALLOC_OOM & ret_stat != CRYPTINT_ERR_RANGE) ? dst.limbs : dst_tmp_p;
    const limb_t* src_limbs = (ret_stat != DNML_ALLOC_OOM & ret_stat != CRYPTINT_ERR_RANGE) ? src.limbs : src_tmp_p;

    /* Setting Up correctly - Standard Case */
    size_t iter_cnt = (size_t)(src.n / 512 + 1);
    size_t end; CHOOSE_OPTION(
        (end), (!(src.n)), 0, /* 2nd option: Aligns to the last byte */
        ((src.n - 1) * BYTES_IN_UINT64_T + (BYTES_IN_UINT64_T - 1))
    ); CHOOSE_OPTION((end), (ret_stat != DNML_ALLOC_OOM & ret_stat != CRYPTINT_ERR_RANGE), (end), (511));
    CHOOSE_OPTION((dst.cap), (ret_stat != DNML_ALLOC_OOM & ret_stat != CRYPTINT_ERR_RANGE), (dst.cap), (512));
    while (iter_cnt--) __libdnml_memcpy_strict(dst_limbs, src_limbs, dst.cap, 0, end);
    dst.n = src.n; dst.sign = src.sign;

    /* Setting Up invalid metadata - DNML_ALLOC_OOM */
    dst.limbs[0] &= correctly_set; dst.n &= correctly_set;
    dst.cap &= correctly_set; dst.sign &= correctly_set;
    *err = ret_stat; return dst;
}
cryptint crint_tover_copy(cryptint src, size_t output_cap, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(src)), full_contract,
    { /* Set everything to 0 to completely wipe out memory */
        if (src.limbs != NULL) *(src.limbs) = 0;
        crint_free(&src);
    });
    DNML_TEST_ASSERT((err != NULL), null_err);
    DNML_TEST_ASSERT((!(src.poisoned)), poisoined);
    dnml_status ret_stat = CRYPTINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (err == NULL & ret_stat == CRYPTINT_SUCCESS), (DNML_NULL_EPARAM), (ret_stat));
    CHOOSE_OPTION((ret_stat), (src.poisoned & ret_stat == CRYPTINT_SUCCESS), (CRYPTINT_POISOINED), (ret_stat));

    /* Main Operation - Copy */
    cryptint dst; dnml_status new_stat; uint64_t correctly_set;
    size_t op_range; CHOOSE_OPTION((op_range), (output_cap < src.n), (output_cap), (src.n));
    limb_t dst_tmp_p[512] = {0}, src_tmp_p[512] = {0};
    new_stat = crint_snew(&dst, src.n); // Guaraneed dst->cap >= 1
    CHOOSE_OPTION((correctly_set), (new_stat != DNML_ALLOC_OOM), (UINT64_MAX), (0));
    limb_t* dst_limbs = (new_stat != DNML_ALLOC_OOM) ? dst.limbs : dst_tmp_p;
    const limb_t* src_limbs = (new_stat != DNML_ALLOC_OOM) ? src.limbs : src_tmp_p;

    /* Setting Up correctly - Standard Case */
    size_t iter_cnt = (size_t)(src.n / 512 + 1);
    size_t end; CHOOSE_OPTION(
        (end), (!(op_range)), 0, /* 2nd option: Aligns to the last byte */
        ((op_range - 1) * BYTES_IN_UINT64_T + (BYTES_IN_UINT64_T - 1))
    ); CHOOSE_OPTION((end), (new_stat != DNML_ALLOC_OOM), (end), (511));
    CHOOSE_OPTION((dst.cap), (new_stat != DNML_ALLOC_OOM), (dst.cap), (512));
    while (iter_cnt--) __libdnml_memcpy_strict(dst.limbs, src.limbs, dst.cap, 0, end);
    dst.n = src.n; dst.sign = src.sign;

    /* Setting Up invalid metadata - DNML_ALLOC_OOM */
    dst.limbs[0] &= correctly_set; dst.n &= correctly_set;
    dst.cap &= correctly_set; dst.sign &= correctly_set;
    CHOOSE_OPTION((ret_stat), (new_stat != CRYPTINT_SUCCESS & ret_stat == CRYPTINT_SUCCESS), (new_stat), (ret_stat));
    *err = ret_stat; return dst;
}





//* ===================================== STATE ALTERATION FUNCTIONS ===================================== *//
void crint_canonicalize(cryptint *x) {
    // Fix invalid capacity
    uint8_t cap_invalid = (x->cap < 1);
    CT_COND_ASSIGN(x->cap, cap_invalid, 1);
    CT_COND_ASSIGN(x->n, cap_invalid, 0);
    CT_COND_ASSIGN(x->sign, cap_invalid, 1);
    
    // Clamp n to capacity
    uint8_t n_overflow = (x->n > x->cap);
    CT_COND_ASSIGN(x->n, n_overflow, x->cap);
    
    // Fix invalid sign
    uint8_t sign_invalid = (x->sign != 1) & (x->sign != -1);
    CT_COND_ASSIGN(x->n, sign_invalid, 0);
    CT_COND_ASSIGN(x->sign, sign_invalid, 1);
}
void crint_normalize(cryptint *x) { __CRINT_TRIM_LZ__(x); CHOOSE_OPTION((x->sign), (!x->n), (1), (-1)); }
dnml_status crint_resize(cryptint *x, size_t k) {
    // Pre-operation Validation & Static Analysis
    DNML_ASSERT(__STORAGE_VAL__(x), store_inval, {
        if (x->limbs != NULL) *(x->limbs) = 0;
        x->limbs = 0; x->cap = 0; x->n = 0; 
        x->poisoned = 0; x->sign = 0;
    }); 
    DNML_TEST_ASSERT((!(x->poisoned)), poisoined);
    DNML_TEST_ASSERT((k), "Invalid Capacity Request (-Einval_cap_request)");
    if (x->poisoned) return CRYPTINT_POISOINED;
    if (!k) return DNML_INVAL_CAP_REQUEST;

    // Main Resizing
    limb_t *__BUFFER_P = realloc(x->limbs, k * BYTES_IN_UINT64_T);
    DNML_TEST_ASSERT(__BUFFER_P != NULL, realloc_null);
    if (__BUFFER_P == NULL) {
        x->n = 0; x->limbs = 0; x->cap = 0;
        x->poisoned = 0; x->sign = 0; __BUFFER_P = 0;
        return DNML_ALLOC_OOM;
    }
    x->limbs = __BUFFER_P; x->cap = k;
    CHOOSE_OPTION((x->n), (x->n > x->cap), (x->cap), (x->n));
    return CRYPTINT_SUCCESS;
}
dnml_status crint_reserve(cryptint *x, size_t k) {
    // Pre-operation Validation & Static Analysis
    DNML_ASSERT(__STORAGE_VAL__(x), store_inval, {
        if (x->limbs != NULL) *(x->limbs) = 0;
        x->limbs = 0; x->cap = 0; x->n = 0;
        x->poisoned = 0; x->sign = 0;
    });
    DNML_TEST_ASSERT((!(x->poisoned)), poisoined);
    if (x->poisoned) return CRYPTINT_POISOINED;

    // Main Resizing
    size_t new_cap = x->cap; while (new_cap < k) new_cap *= 2;
    limb_t *__BUFFER_P = realloc(x->limbs, new_cap * BYTES_IN_UINT64_T);
    DNML_TEST_ASSERT(__BUFFER_P != NULL, realloc_null);
    if (__BUFFER_P == NULL) {
        x->n = 0; x->limbs = 0; x->cap = 0;
        x->poisoned = 0; x->sign = 0; __BUFFER_P = 0;
        return DNML_ALLOC_OOM;
    }
    x->limbs = __BUFFER_P; x->cap = new_cap;
}
dnml_status crint_shrink(cryptint *x, size_t k) {
    // Pre-operation Validation & Static Analysis
    DNML_ASSERT(__STORAGE_VAL__(x), store_inval, {
        if (x->limbs != NULL) *(x->limbs) = 0;
        x->limbs = 0; x->cap = 0; x->n = 0; 
        x->poisoned = 0; x->sign = 0;
    });
    DNML_TEST_ASSERT((!(x->poisoned)), poisoined);
    DNML_TEST_ASSERT((k), "Invalid Capacity Request (-Einval_cap_request)");
    if (x->poisoned) return CRYPTINT_POISOINED;
    if (!k) return DNML_INVAL_CAP_REQUEST;

    // Main Resizing
    size_t new_cap = x->cap; while (new_cap > k) --new_cap;
    limb_t *__BUFFER_P = realloc(x->limbs, new_cap * BYTES_IN_UINT64_T);
    DNML_TEST_ASSERT(__BUFFER_P != NULL, realloc_null);
    if (__BUFFER_P == NULL) {
        x->n = 0; x->limbs = 0; x->cap = 0;
        x->poisoned = 0; x->sign = 0; __BUFFER_P = 0;
        return DNML_ALLOC_OOM;
    }
    x->limbs = __BUFFER_P; x->cap = new_cap;
}
dnml_status crint_reset(cryptint *x) {
    DNML_ASSERT(__STORAGE_VAL__(x),
    "Partial Contract Violation: CryptInt State Invariant is violated "
    "(-Ecrypt_int_state_invalid)", {
        if (x->limbs != NULL) *(x->limbs) = 0;
        x->limbs = 0; x->cap = 0; x->n = 0;
        x->poisoned = 0; x->sign = 0;
    });
    DNML_TEST_ASSERT((!(x->poisoned)), poisoined);
    if (x->poisoned) return CRYPTINT_POISOINED;
    __libdnml_memwipe_strict(x->limbs, x->cap * BYTES_IN_UINT64_T);
    x->n = 0; x->sign = 1;
}
static inline uint8_t __STORAGE_VAL__(cryptint *x) {
    if (x == NULL) return 0;
    if (x->limbs == NULL) return 0;
    if (x->cap < 1) return 0;
    return 1;
}
bool crint_validate(cryptint x) {
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
bool crint_pvalidate(cryptint *x) {
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


