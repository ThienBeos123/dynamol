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
#define null_err "Parameter Error: Status/Error parameter-based returns is null (-Enull_err_param)" /* err = NULL - testing */




//* ===================================== TYPE SETUP FUNCTION ===================================== *//
void crint_free(cryptint *x) {
    if (x->limbs == NULL) return;
    __libdnml_memwipe_strict(x->limbs, x->cap * BYTES_IN_UINT64_T);
    free(x->limbs); x->limbs = NULL;
    x->n = 0; x->cap = 0; x->sign = 0;
    x->poisoned = 0;
}
void crint_new(cryptint *x) {
    if (x->limbs != NULL) return; // already initialized
    limb_t *P_BUFFER__ = calloc(1, BYTES_IN_UINT64_T);
    DNML_ASSERT(P_BUFFER__ != NULL,calloc_null,
    { /* Set everything to 0 to completely wipe out memory */
        x->n = 0; x->sign = 0; x->cap = 0;
        x->poisoned = 0; x->limbs = 0; P_BUFFER__ = 0;
    });
    x->limbs = P_BUFFER__; x->cap = 1;
    x->n = 0; x->sign = 1; x->poisoned = false;
}
void crint_snew(cryptint *x, const size_t n) {
    if (x->limbs != NULL) return; // already initialized
    size_t salloc; NORMALIZE_0_TO_1(salloc, n);
    limb_t *__BUFFER_P = calloc(salloc, BYTES_IN_UINT64_T);
    DNML_ASSERT(__BUFFER_P != NULL, calloc_null,
    { /* Set everything to 0 to completely wipe out memory */
        x->n = 0; x->sign = 0; x->cap = 0;
        x->poisoned = 0; x->limbs = 0; __BUFFER_P = 0;
    });
    x->limbs = __BUFFER_P; x->cap = n;
    x->n = 0; x->sign = 1; x->poisoned = false;
}
drypto_stat crint_cinew(cryptint *x, cryptint *y) {
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
    DNML_ASSERT(__BUFFER_P != NULL, calloc_null,
    { /* Set everything to 0 to completely wipe out memory */
        crint_free(y); y->limbs = 0; x->limbs = 0; x->n = 0;
        x->cap = 0; x->sign = 0; x->poisoned = 0; __BUFFER_P = 0;
    }); 
    x->limbs = __BUFFER_P;
    if (y->n) __libdnml_memcpy_strict(
        x->limbs, y->limbs,
        y->n * BYTES_IN_UINT64_T, 0,
        (y->n - 1) * BYTES_IN_UINT64_T + (BYTES_IN_UINT64_T - 1)
    ); 
    x->n = y->n; x->cap = alloc_size; 
    x->sign = (y->n) ? y->sign : 1; x->poisoned = false;
}
drypto_stat crint_new_u64(cryptint *x, const uint64_t in) {
    if (x->limbs != NULL) return; // already initialized
    limb_t *__BUFFER_P = calloc(1, BYTES_IN_UINT64_T);
    DNML_ASSERT(__BUFFER_P != NULL, calloc_null,
    { /* Set everything to 0 to completely wipe out memory */
        x->n = 0; x->sign = 0; x->cap = 0;
        x->poisoned = 0; x->limbs = 0; __BUFFER_P = 0;
    }); 
    x->limbs = __BUFFER_P; x->limbs[0] = in;
    x->cap = 1; x->n = !!(in); x->sign = 1; x->poisoned = false;
}
drypto_stat crint_new_i64(cryptint *x, const int64_t in) {
    if (x->limbs != NULL) return; // already initialized
    limb_t *__BUFFER_P = calloc(1, BYTES_IN_UINT64_T);
    DNML_ASSERT(__BUFFER_P != NULL, calloc_null,
    { /* Set everything to 0 to completely wipe out memory */
        x->n = 0; x->sign = 0; x->cap = 0;
        x->poisoned = 0; x->limbs = 0; __BUFFER_P = 0;
    });
    x->limbs = __BUFFER_P; x->limbs[0] = __MAG_I64__(in); 
    x->cap = 1; x->n = !!(in); x->poisoned = false;
    CHOOSE_OPTION((x->sign), (in < 0), (-1), (1));
}
drypto_stat crint_new_f128(cryptint *x, long double in) {}





//* =============================================== COMPARISONS ============================================== */
static int8_t __CRINT_MAGCMP64__(cryptint *x, const uint64_t val, drypto_stat *err) {
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
bool crint_equal_i64(cryptint x, const int64_t val, drypto_stat *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        x.limbs = 0; x.cap = 0; x.n = 0;
        x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err);
    DNML_TEST_ASSERT((!(x.poisoned)), poisoined);
    if (err = NULL) { *err = CRYPTINT_NUL_EPARAM; return false; }
    if (x.poisoned) { *err = CRYPTINT_POISOINED; return false; }
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
    CHOOSE_OPTION((ret), (ret == 2), (1), (ret)); return (bool)(ret);
}
bool crint_less_i64(cryptint x, const int64_t val, drypto_stat *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        x.limbs = 0; x.cap = 0; x.n = 0;
        x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err);
    DNML_TEST_ASSERT((!(x.poisoned)), poisoined);
    if (err = NULL) { *err = CRYPTINT_NUL_EPARAM; return false; }
    if (x.poisoned) { *err = CRYPTINT_POISOINED; return false; }
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
    CHOOSE_OPTION((ret), (ret == 2), (1), (ret)); return (bool)(ret);
}
bool crint_more_i64(cryptint x, const int64_t val, drypto_stat *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        x.limbs = 0; x.cap = 0; x.n = 0;
        x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err);
    DNML_TEST_ASSERT((!(x.poisoned)), poisoined);
    if (err = NULL) { *err = CRYPTINT_NUL_EPARAM; return false; }
    if (x.poisoned) { *err = CRYPTINT_POISOINED; return false; }
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
    CHOOSE_OPTION((ret), (ret == 2), (1), (ret)); return (bool)(ret);
}
bool crint_lequal_i64(cryptint x, const int64_t val, drypto_stat *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        x.limbs = 0; x.cap = 0; x.n = 0;
        x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err);
    DNML_TEST_ASSERT((!(x.poisoned)), poisoined);
    if (err = NULL) { *err = CRYPTINT_NUL_EPARAM; return false; }
    if (x.poisoned) { *err = CRYPTINT_POISOINED; return false; }
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
    CHOOSE_OPTION((ret), (ret == 2), (1), (ret)); return (bool)(ret);
}
bool crint_mequal_i64(cryptint x, const int64_t val, drypto_stat *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        x.limbs = 0; x.cap = 0; x.n = 0;
        x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err);
    DNML_TEST_ASSERT((!(x.poisoned)), poisoined);
    if (err = NULL) { *err = CRYPTINT_NUL_EPARAM; return false; }
    if (x.poisoned) { *err = CRYPTINT_POISOINED; return false; }
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
    CHOOSE_OPTION((ret), (ret == 2), (1), (ret)); return (bool)(ret);
}
/* ----------- Unsigned Integer - UI64 ----------- */
bool crint_equal_u64(cryptint x, const uint64_t val, drypto_stat *err) {}
bool crint_less_u64(cryptint x, const uint64_t val, drypto_stat *err) {}
bool crint_more_u64(cryptint x, const uint64_t val, drypto_stat *err) {}
bool crint_lequal_u64(cryptint x, const uint64_t val, drypto_stat *err) {}
bool crint_mequal_u64(cryptint x, const uint64_t val, drypto_stat *err) {}
/* ------------------- Cryptint ------------------ */
bool crint_equal(cryptint x, cryptint y, drypto_stat *err) {}
bool crint_less(cryptint x, cryptint y, drypto_stat *err) {}
bool crint_more(cryptint x, cryptint y, drypto_stat *err) {}
bool crint_lequal(cryptint x, cryptint y, drypto_stat *err) {}
bool crint_mequal(cryptint x, cryptint y, drypto_stat *err) {}



//* ================================================= COPIES ================================================= */
/* -------------  Mutative SMALL Copies ------------- */
drypto_stat crint_mut_copyu64(cryptint *dst__, const uint64_t source__) {
    // Pre-operation Validation & Static Analysis
    DNML_ASSERT(__STORAGE_VAL__(dst__), store_inval, {
        if (dst__->limbs != NULL) *(dst__->limbs) = 0;
        dst__->limbs = 0; dst__->cap = 0; dst__->n = 0; 
        dst__->poisoned = 0; dst__->sign = 0;
    });
    DNML_TEST_ASSERT((!(dst__->poisoned)), poisoined);
    if (dst__->poisoned) return CRYPTINT_POISOINED;
    /* Main Operation - Copy */
    crint_canonicalize(dst__);
    dst__->limbs[0] = source__;
    dst__->n = !!(source__); dst__->sign = 1; 
    __libdnml_memset_strict(
        dst__->limbs, 0, 
        dst__->cap * BYTES_IN_UINT64_T, 1, 
        (dst__->cap - 1) * BYTES_IN_UINT64_T + (BYTES_IN_UINT64_T - 1)
    );
    return CRYPTINT_SUCCESS;
}
drypto_stat crint_mut_dcopyu64(cryptint *dst__, const uint64_t source__) {
    // Pre-operation Validation & Static Analysis
    DNML_ASSERT(__STORAGE_VAL__(dst__), store_inval, {
        if (dst__->limbs != NULL) *(dst__->limbs) = 0;
        dst__->limbs = 0; dst__->cap = 0; dst__->n = 0; 
        dst__->poisoned = 0; dst__->sign = 0;
    });
    DNML_TEST_ASSERT((!(dst__->poisoned)), poisoined);
    if (dst__->poisoned) return CRYPTINT_POISOINED;
    /* Main Operation - Copy */
    crint_canonicalize(dst__);
    limb_t *__BUFFER_P = realloc(dst__->limbs, BYTES_IN_UINT64_T);
    DNML_ASSERT(__BUFFER_P != NULL,
    realloc_null,
    { /* Set everything to 0 to completely wipe out memory */
        dst__->n = 0; dst__->sign = 0; dst__->cap = 0;
        dst__->poisoned = 0; dst__->limbs = 0; __BUFFER_P = 0;
    });
    dst__->limbs = __BUFFER_P; dst__->limbs[0] = source__;
    dst__->n = !!(source__); dst__->cap = 1; dst__->sign = 1;
    __libdnml_memset_strict(
        dst__->limbs, 0, 
        dst__->cap * BYTES_IN_UINT64_T, 1, 
        (dst__->cap - 1) * BYTES_IN_UINT64_T + (BYTES_IN_UINT64_T - 1)
    );
    return CRYPTINT_SUCCESS;
}
drypto_stat crint_mut_copyi64(cryptint *dst__, const int64_t source__) {
    // Pre-operation Validation & Static Analysis
    DNML_ASSERT(__STORAGE_VAL__(dst__),
    "Partial Contract Violation: CryptInt invalid for storage "
    "(-Ecrypt_int_sinvalid)", {
        if (dst__->limbs != NULL) *(dst__->limbs) = 0;
        dst__->limbs = 0; dst__->cap = 0; dst__->n = 0; 
        dst__->poisoned = 0; dst__->sign = 0;
    });
    DNML_TEST_ASSERT((!(dst__->poisoned)), poisoined);
    if (dst__->poisoned) return CRYPTINT_POISOINED;
    /* Main Operation - Copy */
    crint_canonicalize(dst__); dst__->limbs[0] = __MAG_I64__(source__); 
    dst__->n = !!(source__); CHOOSE_OPTION((dst__->sign), (source__ < 0), (-1), (1));
    __libdnml_memset_strict(
        dst__->limbs, 0,
        dst__->cap * BYTES_IN_UINT64_T, 1, 
        (dst__->cap - 1) * BYTES_IN_UINT64_T + (BYTES_IN_UINT64_T - 1)
    );
    return CRYPTINT_SUCCESS;
}
drypto_stat crint_mut_dcopyi64(cryptint *dst__, const int64_t source__) {
    // Pre-operation Validation & Static Analysis
    DNML_ASSERT(__STORAGE_VAL__(dst__), store_inval, {
        if (dst__->limbs != NULL) *(dst__->limbs) = 0;
        dst__->limbs = 0; dst__->cap = 0; dst__->n = 0; 
        dst__->poisoned = 0; dst__->sign = 0;
    });
    DNML_TEST_ASSERT((!(dst__->poisoned)), poisoined);
    if (dst__->poisoned) return CRYPTINT_POISOINED;
    /* Main Operation - Copy */
    crint_canonicalize(dst__);
    limb_t *__BUFFER_P = realloc(dst__->limbs, BYTES_IN_UINT64_T);
    DNML_ASSERT(__BUFFER_P != NULL,
    realloc_null,
    { /* Set everything to 0 to completely wipe out memory */
        dst__->n = 0; dst__->sign = 0; dst__->cap = 0;
        dst__->poisoned = 0; dst__->limbs = 0; __BUFFER_P = 0;
    });
    dst__->limbs = __BUFFER_P; 
    dst__->limbs[0] = __MAG_I64__(source__);
    dst__->n = !!(source__); dst__->cap = 1;
    CHOOSE_OPTION((dst__->sign), (source__ < 0), (-1), (1));
    __libdnml_memset_strict(
        dst__->limbs, 0,
        dst__->cap * BYTES_IN_UINT64_T, 1,
        (dst__->cap - 1) * BYTES_IN_UINT64_T + (BYTES_IN_UINT64_T - 1)
    );
    return CRYPTINT_SUCCESS;
}
/* -------------  Mutative LARGE Copies ------------- */
drypto_stat crint_mut_copyf128(cryptint *dst__, long double source__) {}
drypto_stat crint_mut_dcopyf128(cryptint *dst__, long double source__) {}
drypto_stat crint_mut_ocopyf128(cryptint *dst__, long double source__) {}
drypto_stat crint_mut_tover_copyf128(cryptint *dst__, long double source__) {}
drypto_stat crint_mut_copy(cryptint *dst__, cryptint source__) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(source__)), full_contract,
    { /* Set everything to 0 to completely wipe out memory */
        if (source__.limbs != NULL) *(source__.limbs) = 0;
        crint_free(&source__); dst__->limbs = 0; dst__->n = 0;
        dst__->cap = 0; dst__->sign = 0; dst__->poisoned = 0;
    });
    DNML_ASSERT((__STORAGE_VAL__(dst__)), store_inval,
    { /* Set everything to 0 to completely wipe out memory */
        __libdnml_memwipe_strict(source__.limbs, source__.cap * BYTES_IN_UINT64_T);
        crint_free(&source__); dst__->limbs = 0; dst__->n = 0;
        dst__->cap = 0; dst__->sign = 0; dst__->poisoned = 0;
    }); DNML_TEST_ASSERT((!(dst__->poisoned)) && (!(source__.poisoned)), poisoined);
    if ((dst__->poisoned) || !(source__.poisoned)) return CRYPTINT_POISOINED;

    /* Main Operation - Copy */
    crint_canonicalize(dst__);
    if (dst__->cap < source__.n) crint_reserve(dst__, source__.n);
    __libdnml_memcpy_strict(
        dst__->limbs, source__.limbs, dst__->cap * BYTES_IN_UINT64_T, 
        0, (source__.n - 1) * BYTES_IN_UINT64_T + (BYTES_IN_UINT64_T - 1)
    ); 
    dst__->n = source__.n; dst__->sign = source__.sign;
    __libdnml_memset_strict(
        dst__->limbs, 0, 
        dst__->cap * BYTES_IN_UINT64_T, 
        dst__->n * BYTES_IN_UINT64_T,
        (dst__->cap - 1) * BYTES_IN_UINT64_T + (BYTES_IN_UINT64_T - 1)
    ); return CRYPTINT_SUCCESS;
}
drypto_stat crint_mut_dcopy(cryptint *dst__, cryptint source__) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(source__)), full_contract,
    { /* Set everything to 0 to completely wipe out memory */
        if (source__.limbs != NULL) *(source__.limbs) = 0;
        crint_free(&source__); dst__->limbs = 0; dst__->n = 0;
        dst__->cap = 0; dst__->sign = 0; dst__->poisoned = 0;
    });
    DNML_ASSERT((__STORAGE_VAL__(dst__)), store_inval,
    { /* Set everything to 0 to completely wipe out memory */
        __libdnml_memwipe_strict(source__.limbs, source__.cap * BYTES_IN_UINT64_T);
        crint_free(&source__); dst__->limbs = 0; dst__->n = 0;
        dst__->cap = 0; dst__->sign = 0; dst__->poisoned = 0;
    }); DNML_TEST_ASSERT((!(dst__->poisoned)) && (!(source__.poisoned)), poisoined);
    if ((dst__->poisoned) || !(source__.poisoned)) return CRYPTINT_POISOINED;

    /* Main Operation - Copy */
    crint_canonicalize(dst__); crint_resize(dst__, source__.cap);
    __libdnml_memcpy_strict(
        dst__->limbs, source__.limbs, dst__->cap * BYTES_IN_UINT64_T,
        0, (source__.n - 1) * BYTES_IN_UINT64_T + (BYTES_IN_UINT64_T - 1)
    );
    dst__->n = source__.n; dst__->sign = source__.sign;
    __libdnml_memset_strict(
        dst__->limbs, 0, 
        dst__->cap * BYTES_IN_UINT64_T,
        dst__->n * BYTES_IN_UINT64_T, 
        (dst__->cap - 1) * BYTES_IN_UINT64_T + (BYTES_IN_UINT64_T - 1)
    ); return CRYPTINT_SUCCESS;
}
drypto_stat crint_mut_ocopy(cryptint *dst__, cryptint source__) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(source__)), full_contract,
    { /* Set everything to 0 to completely wipe out memory */
        if (source__.limbs != NULL) *(source__.limbs) = 0;
        crint_free(&source__); dst__->limbs = 0; dst__->n = 0;
        dst__->cap = 0; dst__->sign = 0; dst__->poisoned = 0;
    });
    DNML_ASSERT((__STORAGE_VAL__(dst__)), store_inval,
    { /* Set everything to 0 to completely wipe out memory */
        __libdnml_memwipe_strict(source__.limbs, source__.cap * BYTES_IN_UINT64_T);
        crint_free(&source__); dst__->limbs = 0; dst__->n = 0;
        dst__->cap = 0; dst__->sign = 0; dst__->poisoned = 0;
    }); DNML_TEST_ASSERT((!(dst__->poisoned)) && (!(source__.poisoned)), poisoined);
    if ((dst__->poisoned) || !(source__.poisoned)) return CRYPTINT_POISOINED;

    /* Main Operation - Copy */
    if (dst__->cap < source__.n) return BIGINT_ERR_RANGE;
    __libdnml_memcpy_strict(
        dst__->limbs, source__.limbs, dst__->cap * BYTES_IN_UINT64_T, 
        0, (source__.n - 1) * BYTES_IN_UINT64_T + (BYTES_IN_UINT64_T - 1)
    );
    dst__->n = source__.n; dst__->sign = source__.sign;
    __libdnml_memset_strict(
        dst__->limbs, 0, dst__->cap * BYTES_IN_UINT64_T,
        dst__->n * BYTES_IN_UINT64_T,
        (dst__->cap - 1) * BYTES_IN_UINT64_T + (BYTES_IN_UINT64_T - 1)
    );
}
drypto_stat crint_mut_tover_copy(cryptint *dst__, cryptint source__) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(source__)), full_contract,
    { /* Set everything to 0 to completely wipe out memory */
        if (source__.limbs != NULL) *(source__.limbs) = 0;
        crint_free(&source__); dst__->limbs = 0; dst__->n = 0;
        dst__->cap = 0; dst__->sign = 0; dst__->poisoned = 0;
    });
    DNML_ASSERT((__STORAGE_VAL__(dst__)), store_inval,
    { /* Set everything to 0 to completely wipe out memory */
        __libdnml_memwipe_strict(source__.limbs, source__.cap * BYTES_IN_UINT64_T);
        crint_free(&source__); dst__->limbs = 0; dst__->n = 0;
        dst__->cap = 0; dst__->sign = 0; dst__->poisoned = 0;
    }); DNML_TEST_ASSERT((!(dst__->poisoned)) && (!(source__.poisoned)), poisoined);
    if ((dst__->poisoned) || !(source__.poisoned)) return CRYPTINT_POISOINED;

    /* Main Operation - Copy */
    size_t op_range; CHOOSE_OPTION((op_range), (dst__->cap < source__.n), (dst__->cap), (source__.n));
    __libdnml_memcpy_strict(
        dst__->limbs, source__.limbs, dst__->cap * BYTES_IN_UINT64_T,
        0, (op_range - 1) * BYTES_IN_UINT64_T + (BYTES_IN_UINT64_T - 1)
    ); dst__->n = op_range; dst__->sign = source__.sign;
    __libdnml_memset_strict(
        dst__->limbs, 0,
        (dst__->cap) * BYTES_IN_UINT64_T,
        dst__->n * BYTES_IN_UINT64_T, 
        (dst__->cap - 1) * BYTES_IN_UINT64_T + (BYTES_IN_UINT64_T - 1)
    );
}
/* -------------  Functional SMALL Copies ------------- */
cryptint crint_copyu64(const uint64_t source__) {
    cryptint dst__; crint_new(&dst__);
    dst__.limbs[0] = source__;
    dst__.n = !!(source__);
    dst__.cap = 1; dst__.sign = 1;
    return dst__;
}
cryptint crint_copyi64(const int64_t source__) {
    cryptint dst__; crint_new(&dst__);
    dst__.limbs[0] = __MAG_I64__(source__);
    dst__.n = !!(source__); dst__.cap = 1;
    CHOOSE_OPTION((dst__.sign), (source__ < 0), (-1), (1));
    return dst__;
}
/* -------------  Functional LARGE Copies ------------- */
cryptint crint_copyf128(long double source__, drypto_stat *__err__) {}
cryptint crint_ocopyf128(long double source__, size_t output_cap, drypto_stat *__err__) {}
cryptint crint_tover_copyf128(long double source__, size_t output_cap, drypto_stat *__err__) {}
cryptint crint_copy(cryptint source__, drypto_stat *__err__) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(source__)), full_contract,
    { /* Set everything to 0 to completely wipe out memory */
        if (source__.limbs != NULL) *(source__.limbs) = 0;
        crint_free(&source__);
    });
    DNML_TEST_ASSERT((__err__ != NULL), null_err);
    DNML_TEST_ASSERT((!(source__.poisoned)), poisoined);
    if (__err__ == NULL) { *__err__ = CRYPTINT_NUL_EPARAM; return __CRINT_ERRVAL__(); }
    if (!(source__.poisoned)) { *__err__ = CRYPTINT_POISOINED; return __CRINT_ERRVAL__(); }

    /* Main Operation - Copy */
    cryptint dst__; crint_snew(&dst__, source__.n); // Guaranteed dst__->cap >= 1
    size_t end; CHOOSE_OPTION(
        (end), (!(source__.n)), 0, /* 2nd option: Aligns to the last byte */
        ((source__.n - 1) * BYTES_IN_UINT64_T + (BYTES_IN_UINT64_T - 1))
    ); __libdnml_memcpy_strict(dst__.limbs, source__.limbs, dst__.cap, 0, end);
    dst__.n = source__.n; dst__.sign = source__.sign; *__err__ = CRYPTINT_SUCCESS;
    return dst__;
}
cryptint crint_ocopy(cryptint source__, size_t output_cap, drypto_stat *__err__) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(source__)), full_contract,
    { /* Set everything to 0 to completely wipe out memory */
        if (source__.limbs != NULL) *(source__.limbs) = 0;
        crint_free(&source__);
    });
    DNML_TEST_ASSERT((__err__ != NULL), null_err);
    DNML_TEST_ASSERT((!(source__.poisoned)), poisoined);
    if (__err__ == NULL) { *__err__ = CRYPTINT_NUL_EPARAM; return __CRINT_ERRVAL__(); }
    if (!(source__.poisoned)) { *__err__ = CRYPTINT_POISOINED; return __CRINT_ERRVAL__(); }

    /* Main Operation - Copy */
    if (output_cap < source__.n) { *__err__ = CRYPTINT_ERR_RANGE; return __CRINT_ERRVAL__(); }
    cryptint dst__; crint_snew(&dst__, output_cap); 
    size_t end; CHOOSE_OPTION(
        (end), (!(source__.n)), 0, /* 2nd option: Aligns to the last byte */
        ((source__.n - 1) * BYTES_IN_UINT64_T + (BYTES_IN_UINT64_T - 1))
    ); __libdnml_memcpy_strict(dst__.limbs, source__.limbs, dst__.cap, 0, end);
    dst__.n = source__.n; dst__.sign = source__.sign;
    *__err__ = CRYPTINT_SUCCESS; return dst__;
}
cryptint crint_tover_copy(cryptint source__, size_t output_cap, drypto_stat *__err__) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(source__)), full_contract,
    { /* Set everything to 0 to completely wipe out memory */
        if (source__.limbs != NULL) *(source__.limbs) = 0;
        crint_free(&source__);
    });
    DNML_TEST_ASSERT((__err__ != NULL), null_err);
    DNML_TEST_ASSERT((!(source__.poisoned)), poisoined);
    if (__err__ == NULL) { *__err__ = CRYPTINT_NUL_EPARAM; return __CRINT_ERRVAL__(); }
    if (!(source__.poisoned)) { *__err__ = CRYPTINT_POISOINED; return __CRINT_ERRVAL__(); }

    /* Main Operation - Copy */
    cryptint dst__; crint_snew(&dst__, output_cap);
    size_t op_range; CHOOSE_OPTION((op_range), (output_cap < source__.n), (output_cap), (source__.n));
    size_t end; CHOOSE_OPTION(
        (end), (!(op_range)), 0, /* 2nd option: Aligns to the last byte */
        ((op_range - 1) * BYTES_IN_UINT64_T + (BYTES_IN_UINT64_T - 1))
    ); __libdnml_memcpy_strict(dst__.limbs, source__.limbs, dst__.cap, 0, end);
    dst__.n = source__.n; dst__.sign = source__.sign;
    *__err__ = CRYPTINT_SUCCESS; return dst__;
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
drypto_stat crint_resize(cryptint *x, size_t k) {
    // Pre-operation Validation & Static Analysis
    DNML_ASSERT(__STORAGE_VAL__(x), store_inval, {
        if (x->limbs != NULL) *(x->limbs) = 0;
        x->limbs = 0; x->cap = 0; x->n = 0; 
        x->poisoned = 0; x->sign = 0;
    }); 
    DNML_TEST_ASSERT((!(x->poisoned)), poisoined);
    DNML_TEST_ASSERT((k), "Invalid Capacity Request (-Einval_cap_request)");
    if (x->poisoned) return CRYPTINT_POISOINED;
    if (!k) return CRYPTINT_INVAL_CAP_REQUEST;

    // Main Resizing
    limb_t *__BUFFER_P = realloc(x->limbs, k * BYTES_IN_UINT64_T);
    DNML_ASSERT(__BUFFER_P != NULL, realloc_null,
    { /* Set everything to 0 to completely wipe out memory */
        x->limbs = 0; x->n = 0; x->cap = 0; 
        x->sign = 0; x->poisoned = 0; __BUFFER_P = 0;
    });
    x->limbs = __BUFFER_P; x->cap = k;
    CHOOSE_OPTION((x->n), (x->n > x->cap), (x->cap), (x->n));
    return CRYPTINT_SUCCESS;
}
drypto_stat crint_reserve(cryptint *x, size_t k) {
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
    DNML_ASSERT(__BUFFER_P != NULL, realloc_null,
    { /* Set everything to 0 to completely wipe out memory */
        x->limbs = 0; x->n = 0; x->cap = 0; 
        x->sign = 0; x->poisoned = 0; __BUFFER_P = 0;
    });
    x->limbs = __BUFFER_P; x->cap = new_cap;
}
drypto_stat crint_shrink(cryptint *x, size_t k) {
    // Pre-operation Validation & Static Analysis
    DNML_ASSERT(__STORAGE_VAL__(x), store_inval, {
        if (x->limbs != NULL) *(x->limbs) = 0;
        x->limbs = 0; x->cap = 0; x->n = 0; 
        x->poisoned = 0; x->sign = 0;
    });
    DNML_TEST_ASSERT((!(x->poisoned)), poisoined);
    DNML_TEST_ASSERT((k), "Invalid Capacity Request (-Einval_cap_request)");
    if (x->poisoned) return CRYPTINT_POISOINED;
    if (!k) return CRYPTINT_INVAL_CAP_REQUEST;

    // Main Resizing
    size_t new_cap = x->cap; while (new_cap > k) --new_cap;
    limb_t *__BUFFER_P = realloc(x->limbs, new_cap * BYTES_IN_UINT64_T);
    DNML_ASSERT(__BUFFER_P != NULL, realloc_null,
    { /* Set everything to 0 to completely wipe out memory */
        x->limbs = 0; x->n = 0; x->cap = 0; 
        x->sign = 0; x->poisoned = 0; __BUFFER_P = 0;
    });
    x->limbs = __BUFFER_P; x->cap = new_cap;
}
drypto_stat crint_reset(cryptint *x) {
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