#include "cryptInt_func.h"
#include <stdbool.h>




//* ========================================= CONSTRUCTORS & DESTRUCTORS ===================================== *//
void crint_free(cryptint *x) {
    if (x->limbs != NULL) free(x->limbs);
    x->limbs = 0; x->cap = 0; x->n = 0;
    x->poisoned = 0; x->sign = 0;
}
dnml_status crint_new(cryptint *x) {
    uint64_t oom_mask = UINT64_MAX; 
    dnml_status ret_stat = CRYPTINT_SUCCESS;
    limb_t *__BUFFER_P = calloc(1, U64_BYTES);
    DNML_TEST_ASSERT( __BUFFER_P != NULL, realloc_null, {});
    CHOOSE_OPTION((ret_stat), ((__BUFFER_P == NULL) & (x->limbs == NULL)), (DNML_ALLOC_OOM), (ret_stat));
    CHOOSE_OPTION((oom_mask), ((__BUFFER_P == NULL) & (x->limbs == NULL)), (0), (oom_mask));

    uint8_t uninit = (x->limbs == NULL);
    x->limbs = (uninit) ? ((uintptr_t)(__BUFFER_P) & oom_mask) : x->limbs;
    CHOOSE_OPTION((x->cap), (uninit), (1 & oom_mask), (x->cap));
    CHOOSE_OPTION((x->n), (uninit), (0 & oom_mask), (x->n));
    CHOOSE_OPTION((x->sign), (uninit), (1 & oom_mask), (x->sign));
    CHOOSE_OPTION((x->poisoned), (uninit), (0 & oom_mask), (x->poisoned));
    /* Post operation Aggrestive Clearance */
    oom_mask = 0; __BUFFER_P = 0; uninit = 0; return ret_stat;
}
dnml_status crint_snew(cryptint *x, const size_t n) {
    uint64_t oom_mask = UINT64_MAX;
    dnml_status ret_stat = CRYPTINT_SUCCESS;
    size_t salloc; NORMALIZE_0_TO_1(salloc, n);
    limb_t *__BUFFER_P = calloc(salloc, U64_BYTES);
    DNML_TEST_ASSERT( __BUFFER_P != NULL, realloc_null, {});
    CHOOSE_OPTION((ret_stat), ((__BUFFER_P == NULL) & (x->limbs == NULL)), (DNML_ALLOC_OOM), (ret_stat));
    CHOOSE_OPTION((oom_mask), ((__BUFFER_P == NULL) & (x->limbs == NULL)), (0), (oom_mask));

    uint8_t uninit = (x->limbs = NULL);
    x->limbs = (uninit) ? ((uintptr_t)(__BUFFER_P) & oom_mask) : x->limbs;
    CHOOSE_OPTION((x->cap), (uninit), (salloc & oom_mask), (x->cap));
    CHOOSE_OPTION((x->n), (uninit), (0 & oom_mask), (x->n));
    CHOOSE_OPTION((x->sign), (uninit), (1 & oom_mask), (x->sign));
    CHOOSE_OPTION((x->poisoned), (uninit), (0 & oom_mask), (x->poisoned));
    /* Post-operation Aggrestive Clearance */
    oom_mask = 0; salloc = 0; __BUFFER_P = 0; uninit = 0; return ret_stat;
}
dnml_status crint_cinew(cryptint *x, cryptint *y) {
    // Pre-operation Validation & Static Analysis
    DNML_ASSERT((crint_pvalidate(y)), full_contract,
        { /* Set everything to 0 to completely wipe out memory */
          /* We can only guarantee wiping out the first limb safely */
            if (y->limbs != NULL) *(y->limbs) = 0;
            crint_free(y); x->limbs = 0; x->n = 0;
            x->cap = 0; x->sign = 0; x->poisoned = 0;
        }
    ); DNML_TEST_ASSERT((y->poisoned), poisoined, { crint_free(y); });
    dnml_status ret_stat = CRYPTINT_SUCCESS; uint64_t mask = UINT64_MAX; bool noop_toggle = false;
    CHOOSE_OPTION((ret_stat), ((y->poisoned) & (ret_stat == CRYPTINT_SUCCESS)), (CRYPTINT_POISOINED), (mask));
    CHOOSE_OPTION((noop_toggle), (y->poisoned), (true), (noop_toggle));

    /* Main Operations */
    size_t alloc_size; CHOOSE_OPTION((alloc_size), (y->n), (y->n), (1));
    limb_t *__BUFFER_P = calloc(alloc_size, U64_BYTES);
    DNML_TEST_ASSERT( __BUFFER_P != NULL, realloc_null, {});
    CHOOSE_OPTION((ret_stat), (
        (__BUFFER_P == NULL) & (x->limbs == NULL) & (ret_stat == CRYPTINT_SUCCESS)), 
        (DNML_ALLOC_OOM), (ret_stat)
    );
    CHOOSE_OPTION((mask), (
        (__BUFFER_P == NULL) & (x->limbs == NULL) & 
        (mask == UINT64_MAX) & (ret_stat != CRYPTINT_POISOINED)), 
        (0), (mask)
    ); uint8_t uninit = (x->limbs == NULL); 
    limb_t* dst_limbs = (uninit) ? __BUFFER_P : y->limbs;
    size_t end; CHOOSE_OPTION((end), (!y->n), (0), ((y->n - 1) * U64_BYTES + (U64_BYTES - 1)))
    __libdnml_memcpy_strict(dst_limbs, y->limbs, alloc_size * U64_BYTES, end, y->n * U64_BYTES, 0, false);
    x->limbs = (uninit) ? ((uintptr_t)(__BUFFER_P) & mask) : x->limbs;
    CHOOSE_OPTION((x->cap), (uninit), (alloc_size & mask), (x->cap));
    CHOOSE_OPTION((x->sign), (uninit), (y->sign & mask), (x->sign));
    CHOOSE_OPTION((x->poisoned), (uninit), (y->poisoned & mask), (x->poisoned));
    CHOOSE_OPTION((x->n), (uninit), (y->n), (x->n)); x->sign = !!(x->n); 
    /* Post-operation Aggrestive Clearance */
    mask = 0; alloc_size = 0; __BUFFER_P = 0; uninit = 0; 
    dst_limbs = 0; return ret_stat;
}
dnml_status crint_new_u64(cryptint *x, const uint64_t in) {
    uint64_t oom_mask = UINT64_MAX;
    dnml_status ret_stat = CRYPTINT_SUCCESS;
    limb_t *__BUFFER_P = calloc(1, U64_BYTES);
    DNML_TEST_ASSERT( __BUFFER_P != NULL, realloc_null, {});
    CHOOSE_OPTION((ret_stat), ((__BUFFER_P == NULL) & (x->limbs == NULL)), (DNML_ALLOC_OOM), (ret_stat));
    CHOOSE_OPTION((oom_mask), ((__BUFFER_P == NULL) & (x->limbs == NULL)), (0), (oom_mask));

    uint8_t uninit = (x->limbs = NULL); uint64_t first_val = x->limbs[0];
    x->limbs = (uninit) ? ((uintptr_t)(__BUFFER_P) & oom_mask) : x->limbs;
    CHOOSE_OPTION((x->limbs[0]), (uninit), (in & oom_mask), (first_val));
    CHOOSE_OPTION((x->cap), (uninit), (1 & oom_mask), (x->cap));
    CHOOSE_OPTION((x->n), (uninit), ((!!(in)) & oom_mask), (x->n));
    CHOOSE_OPTION((x->sign), (uninit), (1 & oom_mask), (x->sign));
    CHOOSE_OPTION((x->poisoned), (uninit), (0 & oom_mask), (x->poisoned));
    /* Post-operation Aggresive Clearance */
    oom_mask = 0; __BUFFER_P = 0; uninit = 0; first_val = 0;
    return ret_stat;
}
dnml_status crint_new_i64(cryptint *x, const int64_t in) {
    uint64_t oom_mask = UINT64_MAX;
    dnml_status ret_stat = CRYPTINT_SUCCESS;
    limb_t *__BUFFER_P = calloc(1, U64_BYTES);
    DNML_TEST_ASSERT( __BUFFER_P != NULL, realloc_null, {});
    CHOOSE_OPTION((ret_stat), ((__BUFFER_P == NULL) & (x->limbs == NULL)), (DNML_ALLOC_OOM), (ret_stat));
    CHOOSE_OPTION((oom_mask), ((__BUFFER_P == NULL) & (x->limbs == NULL)), (0), (oom_mask));

    uint8_t uninit = (x->limbs = NULL); uint64_t first_val = x->limbs[0];
    int8_t new_sign; CHOOSE_OPTION((new_sign), (in < 0), (-1), (1));
    x->limbs = (uninit) ? ((uintptr_t)(__BUFFER_P) & oom_mask) : x->limbs;
    CHOOSE_OPTION((x->limbs[0]), (uninit), (__MAG_I64__(in) & oom_mask), (first_val));
    CHOOSE_OPTION((x->cap), (uninit), (1 & oom_mask), (x->cap));
    CHOOSE_OPTION((x->n), (uninit), ((!!(in)) & oom_mask), (x->n));
    CHOOSE_OPTION((x->sign), (uninit), (new_sign & oom_mask), (x->sign));
    CHOOSE_OPTION((x->poisoned), (uninit), (0 & oom_mask), (x->poisoned));
    /* Post-operation Aggrestive Clearance */
    oom_mask = 0; __BUFFER_P = 0; uninit = 0;
    first_val = 0; new_sign = 0; return ret_stat;
}
dnml_status crint_new_f128(cryptint *x, long double in) {}



//* =============================================== ASSIGNMENTS ============================================== */
dnml_status crint_set(const cryptint x, cryptint *receiver) {}
dnml_status crint_set_safe(const cryptint x, cryptint *receiver) {}
/* --------- CryptInt --> Primitive Types --------- */
dnml_status crint_setu64(uint64_t* receiver, const cryptint x) {}
dnml_status crint_seti64(int64_t* receiver, const cryptint x) {}
dnml_status crint_setf128(long double* receiver, const cryptint x) {}
dnml_status crint_setu64_safe(uint64_t* receiver, const cryptint x) {}
dnml_status crint_seti64_safe(int64_t* receiver, const cryptint x) {}
dnml_status crint_setf128_safe(long double* receiver, const cryptint x) {}
/* --------- Primitive Types --> CryptInt --------- */
dnml_status crint_getu64(cryptint *receiver, const uint64_t val) {}
dnml_status crint_geti64(cryptint *receiver, const int64_t val) {}
dnml_status crint_getf128(cryptint *receiver, const long double val) {}
dnml_status crint_getf128_safe(cryptint *receiver, const long double val) {}



//* =============================================== CONVERSIONS ============================================== */
/* --------- CryptInt --> Primitive Types --------- */
uint64_t crint_tou64(const cryptint x, dnml_status *err) {}
int64_t crint_toi64(const cryptint x, dnml_status *err) {}
long double crint_tof128(const cryptint x, dnml_status *err) {}
uint64_t crint_tou64_safe(const cryptint x, dnml_status *err) {}
int64_t crint_toi64_safe(const cryptint x, dnml_status *err) {}
long double crint_tof128_safe(const cryptint x, dnml_status *err) {}
/* --------- Primitive Types --> CryptInt --------- */
cryptint crint_fromu64(const uint64_t x, dnml_status *err) {}
cryptint crint_fromi64(const int64_t x, dnml_status *err) {}
cryptint crint_fromf128(const long double x, dnml_status *err) {}
cryptint crint_fromf128_safe(const long double x, dnml_status *err) {}




//* =============================================== COMPARISONS ============================================== */
static int8_t __CRINT_MAGCMP64__(cryptint *x, const uint64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_TEST_ASSERT((crint_pvalidate(x)), full_contract, {});
    DNML_TEST_ASSERT((x->poisoned), poisoined, {});
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
    CHOOSE_OPTION((ret), (ret == 2), (0), (ret)); 
    curr = 0; return ret; // set curr to sanitize memory
}
static int8_t __CRINT_MAGCMP__(cryptint *x, cryptint *y) {
    /* Pre-operation Validation & Static Analysis */
    DNML_TEST_ASSERT((crint_pvalidate(x)), full_contract, {});
    DNML_TEST_ASSERT((x->poisoned), poisoined, {});
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
    } // ALL CHECKS WEREN'T SATIFIES --> EQUAL
    CHOOSE_OPTION((ret), (ret == 2), (0), (ret));
    /* Aggresive Memory Sanitization */
    curr = 0; upperbound = 0; x_curr = 0; y_curr = 0;
    return ret;
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
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((x.poisoned), poisoined, {});
    dnml_status ret_stat = CRYPTINT_SUCCESS;
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
    /* Post-operation Aggrestive Cleanup */
    if (err != NULL) *err = ret_stat; // Conditional Branching here is aceptable
    ret_stat = 0; curr = 0; vsign = 0; mag_val = 0; return (bool)(ret);
}
bool crint_less_i64(cryptint x, const int64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); x.limbs = 0; x.cap = 0; 
        x.n = 0; x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((x.poisoned), poisoined, {});
    dnml_status ret_stat = CRYPTINT_SUCCESS;
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
    /* Post-operation Aggrestive Cleanup */
    if (err != NULL) *err = ret_stat; // Conditional Branching here is aceptable
    ret_stat = 0; curr = 0; vsign = 0; mag_val = 0; return (bool)(ret);
}
bool crint_more_i64(cryptint x, const int64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); x.limbs = 0; x.cap = 0; 
        x.n = 0; x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((x.poisoned), poisoined, {});
    dnml_status ret_stat = CRYPTINT_SUCCESS;
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
    /* Post-operation Aggrestive Cleanup */
    if (err != NULL) *err = ret_stat; // Conditional Branching here is aceptable
    ret_stat = 0; curr = 0; vsign = 0; mag_val = 0; return (bool)(ret);
}
bool crint_lequal_i64(cryptint x, const int64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); x.limbs = 0; x.cap = 0; 
        x.n = 0; x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((x.poisoned), poisoined, {});
    dnml_status ret_stat = CRYPTINT_SUCCESS;
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
    /* Post-operation Aggrestive Cleanup */
    if (err != NULL) *err = ret_stat; // Conditional Branching here is aceptable
    ret_stat = 0; curr = 0; vsign = 0; mag_val = 0; return (bool)(ret);
}
bool crint_mequal_i64(cryptint x, const int64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); x.limbs = 0; x.cap = 0; 
        x.n = 0; x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((x.poisoned), poisoined, {});
    dnml_status ret_stat = CRYPTINT_SUCCESS;
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
    /* Post-operation Aggrestive Cleanup */
    if (err != NULL) *err = ret_stat; // Conditional Branching here is aceptable
    ret_stat = 0; curr = 0; vsign = 0; mag_val = 0; return (bool)(ret);
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
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((x.poisoned), poisoined, {});
    dnml_status ret_stat = CRYPTINT_SUCCESS;
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
    /* Post-operation Aggresive Cleanup */
    if (err != NULL) *err = ret_stat; // Conditional Branching here is aceptable
    ret_stat = 0; curr = 0; return (bool)(ret);
}
bool crint_less_u64(cryptint x, const uint64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); x.limbs = 0; x.cap = 0; 
        x.n = 0; x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((x.poisoned), poisoined, {});
    dnml_status ret_stat = CRYPTINT_SUCCESS;
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
    /* Post-operation Aggresive Cleanup */
    if (err != NULL) *err = ret_stat; // Conditional Branching here is aceptable
    ret_stat = 0; curr = 0; return (bool)(ret);
}
bool crint_more_u64(cryptint x, const uint64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); x.limbs = 0; x.cap = 0; 
        x.n = 0; x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((x.poisoned), poisoined, {});
    dnml_status ret_stat = CRYPTINT_SUCCESS;
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
    /* Post-operation Aggresive Cleanup */
    if (err != NULL) *err = ret_stat; // Conditional Branching here is aceptable
    ret_stat = 0; curr = 0; return (bool)(ret);
}
bool crint_lequal_u64(cryptint x, const uint64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); x.limbs = 0; x.cap = 0; 
        x.n = 0; x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((x.poisoned), poisoined, {});
    dnml_status ret_stat = CRYPTINT_SUCCESS;
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
    /* Post-operation Aggresive Cleanup */
    if (err != NULL) *err = ret_stat; // Conditional Branching here is aceptable
    ret_stat = 0; curr = 0; return (bool)(ret);
}
bool crint_mequal_u64(cryptint x, const uint64_t val, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(x)), full_contract, {
        if (x.limbs != NULL) *(x.limbs) = 0;
        if (err != NULL) *err = 0;
        crint_free(&x); x.limbs = 0; x.cap = 0; 
        x.n = 0; x.poisoned = 0; x.sign = 0; err = 0;
    });
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((x.poisoned), poisoined, {});
    dnml_status ret_stat = CRYPTINT_SUCCESS;
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
    /* Post-operation Aggresive Cleanup */
    if (err != NULL) *err = ret_stat; // Conditional Branching here is aceptable
    ret_stat = 0; curr = 0; return (bool)(ret);
}
/* ------------------- Cryptint ------------------ */
bool crint_equal(cryptint x, cryptint y, dnml_status *err) {
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
    dnml_status ret_stat = CRYPTINT_SUCCESS;
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
    /* Post-operation Aggresive Cleanup */
    if (err != NULL) *err = ret_stat; // Conditional Branching here is aceptableerr = ret_stat; 
    ret_stat = 0; curr = 0; mag_ret = 0; return (bool)(ret);
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
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((x.poisoned), poisoined, {});
    dnml_status ret_stat = CRYPTINT_SUCCESS;
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
    /* Post-operation Aggresive Cleanup */
    if (err != NULL) *err = ret_stat; // Conditional Branching here is aceptable
    ret_stat = 0; curr = 0; mag_ret = 0; return (bool)(ret);
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
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((x.poisoned), poisoined, {});
    dnml_status ret_stat = CRYPTINT_SUCCESS;
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
    /* Post-operation Aggresive Cleanup */
    if (err != NULL) *err = ret_stat; // Conditional Branching here is aceptable
    ret_stat = 0; curr = 0; mag_ret = 0; return (bool)(ret);
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
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((x.poisoned), poisoined, {});
    dnml_status ret_stat = CRYPTINT_SUCCESS;
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
    /* Post-operation Aggresive Cleanup */
    if (err != NULL) *err = ret_stat; // Conditional Branching here is aceptable
    ret_stat = 0; curr = 0; mag_ret = 0; return (bool)(ret);
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
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((x.poisoned), poisoined, {});
    dnml_status ret_stat = CRYPTINT_SUCCESS;
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
    /* Post-operation Aggresive Cleanup */
    if (err != NULL) *err = ret_stat; // Conditional Branching here is aceptable
    ret_stat = 0; curr = 0;  mag_ret = 0; return (bool)(ret);
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
    DNML_TEST_ASSERT((dst->poisoned), poisoined, {});
    dnml_status ret_stat = CRYPTINT_SUCCESS; bool noop_toggle = false;
    CHOOSE_OPTION((ret_stat), (dst->poisoned), (CRYPTINT_POISOINED), (ret_stat));
    CHOOSE_OPTION((noop_toggle), (dst->poisoned), (true), (noop_toggle));
    /* Main Operation - Copy */
    crint_canonicalize(dst); uint64_t val = dst->limbs[0];
    CHOOSE_OPTION((dst->limbs[0]), (noop_toggle), (val), (0));
    CHOOSE_OPTION((dst->n), (noop_toggle), (dst->n), (!!(src)));
    CHOOSE_OPTION((dst->sign), (noop_toggle), (dst->sign), (1));
    __libdnml_memset_strict(
        dst->limbs, 0, dst->cap * U64_BYTES, 1, 
        (dst->cap - 1) * U64_BYTES + (U64_BYTES - 1),
        noop_toggle
    ); val = 0; return ret_stat;
}
dnml_status crint_mut_dcopyu64(cryptint *dst, const uint64_t src) {
    // Pre-operation Validation & Static Analysis
    DNML_ASSERT(__STORAGE_VAL__(dst), store_inval, {
        if (dst->limbs != NULL) *(dst->limbs) = 0;
        dst->limbs = 0; dst->cap = 0; dst->n = 0; 
        dst->poisoned = 0; dst->sign = 0;
    });
    DNML_TEST_ASSERT((dst->poisoned), poisoined, {});
    dnml_status ret_stat = CRYPTINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (dst->poisoned), (CRYPTINT_POISOINED), (ret_stat));

    /* Main Operation - Copy */
    crint_canonicalize(dst); limb_t TMP_P = 1;
    limb_t *__BUFFER_P = realloc(dst->limbs, U64_BYTES);
    DNML_TEST_ASSERT(__BUFFER_P != NULL, realloc_null, {});
    CHOOSE_OPTION((ret_stat), 
        (__BUFFER_P == NULL & (ret_stat == CRYPTINT_SUCCESS)), 
        (DNML_ALLOC_OOM), (CRYPTINT_SUCCESS)
    );
    uint64_t oom; CHOOSE_OPTION((oom), (__BUFFER_P == NULL), (0), (UINT64_MAX));
    dst->limbs = (__BUFFER_P == NULL) ? &TMP_P : __BUFFER_P; // Forced to use ternary operator
    dst->limbs[0] = src; dst->n = !!(src); dst->cap = 1; dst->sign = 1;

    /* Invalid Metadata Fill & Aggresive Cleanup */ 
    dst->limbs[0] &= oom; dst->n &= oom; dst->cap &= oom;
    dst->sign &= oom; dst->poisoned &= oom;
    TMP_P = 0; __BUFFER_P = 0; oom = 0; return ret_stat;
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
    DNML_TEST_ASSERT((dst->poisoned), poisoined, {});
    dnml_status ret_stat = CRYPTINT_SUCCESS; bool noop_toggle = false;
    CHOOSE_OPTION((ret_stat), (dst->poisoned), (CRYPTINT_POISOINED), (ret_stat))
    CHOOSE_OPTION((noop_toggle), (dst->poisoned), (true), (noop_toggle));
    /* Main Operation - Copy */
    crint_canonicalize(dst); int8_t vsign;
    CHOOSE_OPTION((vsign), (src < 0), (-1), (1));
    uint64_t val = __MAG_I64__(dst->limbs[0]);
    CHOOSE_OPTION((dst->limbs[0]), (noop_toggle), (val), (0));
    CHOOSE_OPTION((dst->n), (noop_toggle), (dst->n), (!!(src)));
    CHOOSE_OPTION((dst->sign), (noop_toggle), (dst->sign), (vsign));
    __libdnml_memset_strict(
        dst->limbs, 0, dst->cap * U64_BYTES, 1, 
        (dst->cap - 1) * U64_BYTES + (U64_BYTES - 1), noop_toggle
    ); return ret_stat;
}
dnml_status crint_mut_dcopyi64(cryptint *dst, const int64_t src) {
    // Pre-operation Validation & Static Analysis
    DNML_ASSERT(__STORAGE_VAL__(dst), store_inval, {
        if (dst->limbs != NULL) *(dst->limbs) = 0;
        dst->limbs = 0; dst->cap = 0; dst->n = 0; 
        dst->poisoned = 0; dst->sign = 0;
    });
    DNML_TEST_ASSERT((dst->poisoned), poisoined, {});
    dnml_status ret_stat = CRYPTINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (dst->poisoned), (CRYPTINT_POISOINED), (ret_stat));

    /* Main Operation - Copy */
    crint_canonicalize(dst); limb_t TMP_P = 1;
    limb_t *__BUFFER_P = realloc(dst->limbs, U64_BYTES);
    DNML_TEST_ASSERT(__BUFFER_P != NULL, realloc_null, {});
    CHOOSE_OPTION((ret_stat), 
        (__BUFFER_P == NULL & 
        (ret_stat == CRYPTINT_SUCCESS)), 
        (DNML_ALLOC_OOM), (CRYPTINT_SUCCESS)
    );
    uint64_t oom; CHOOSE_OPTION((oom), (__BUFFER_P == NULL), (0), (UINT64_MAX));
    dst->limbs = (__BUFFER_P == NULL) ? &TMP_P : __BUFFER_P; // Forced to use ternary operator
    dst->limbs[0] = __MAG_I64__(src); dst->n = !!(src); dst->cap = 1;
    CHOOSE_OPTION((dst->sign), (src < 0), (-1), (1));

    /* Invalid Metadata Fill */ 
    dst->limbs[0] &= oom; dst->n &= oom; dst->cap &= oom;
    dst->sign &= oom; dst->poisoned &= oom;
    TMP_P = 0; __BUFFER_P = 0; oom = 0; return ret_stat;
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
        __libdnml_memwipe_strict(src.limbs, src.cap * U64_BYTES, false);
        crint_free(&src); dst->limbs = 0; dst->n = 0;
        dst->cap = 0; dst->sign = 0; dst->poisoned = 0;
    }); DNML_TEST_ASSERT((dst->poisoned) && ((src.poisoned)), poisoined, {});
    dnml_status ret_stat = CRYPTINT_SUCCESS; bool noop_toggle = false;
    CHOOSE_OPTION((ret_stat), (dst->poisoned), (CRYPTINT_POISOINED), (ret_stat));
    CHOOSE_OPTION((noop_toggle), (dst->poisoned), (true), (noop_toggle));

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
    limb_t dst_buf[1024] = {0}, src_buf[1024] = {0}; size_t end;
    limb_t *dst_limbs = (ret_stat != DNML_ALLOC_OOM) ? dst->limbs : dst_buf;
    limb_t *src_limbs = (ret_stat != DNML_ALLOC_OOM) ? src.limbs : src_buf;
    CHOOSE_OPTION((dst->cap), (ret_stat != DNML_ALLOC_OOM), (dst->cap), (1024));
    CHOOSE_OPTION((end), (ret_stat != DNML_ALLOC_OOM), (src.n), (1024));
    dst->n = src.n; dst->sign = src.sign;
    __libdnml_memcpy_strict(
        dst_limbs, src_limbs, dst->cap * U64_BYTES, src.n * U64_BYTES,
        0, ((end - 1) * U64_BYTES + (U64_BYTES - 1)), (noop_toggle & rcap_stat)
    );
    size_t clear_start, clear_end;
    CHOOSE_OPTION((clear_start), (ret_stat != DNML_ALLOC_OOM), (dst->n), (0));
    CHOOSE_OPTION((clear_end), (ret_stat != DNML_ALLOC_OOM), (dst->cap), (1024));
    __libdnml_memset_strict(
        dst_limbs, 0, dst->cap * U64_BYTES, clear_start * U64_BYTES,
        (clear_end - 1) * U64_BYTES + (U64_BYTES - 1), 
        (noop_toggle & rcap_stat != DNML_ALLOC_OOM)
    ); 
    
    /* Invalid Metadata Wiping & Aggressive  */
    dst->poisoned &= correctly_set; dst->sign &= correctly_set;
    dst->n &= correctly_set; dst->cap &= correctly_set;
    rcap_stat = 0; correctly_set = 0;
    __libdnml_memwipe_strict(dst_buf, 1024 * U64_BYTES, false);
    __libdnml_memwipe_strict(src_buf, 1024 * U64_BYTES, false);
    end = 0; dst_limbs = 0; src_limbs = 0;
    clear_start = 0; clear_end = 0; return ret_stat;
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
        __libdnml_memwipe_strict(src.limbs, src.cap * U64_BYTES, false);
        crint_free(&src); dst->limbs = 0; dst->n = 0;
        dst->cap = 0; dst->sign = 0; dst->poisoned = 0;
    }); DNML_TEST_ASSERT(((dst->poisoned) && (src.poisoned)), poisoined, {});
    dnml_status ret_stat = CRYPTINT_SUCCESS; bool noop_toggle = false;
    CHOOSE_OPTION((ret_stat), (dst->poisoned | src.poisoned), (CRYPTINT_POISOINED), (ret_stat));
    CHOOSE_OPTION((noop_toggle), (dst->poisoned | src.poisoned), (true), (noop_toggle));

    /* Main Operation - Copy */
    crint_canonicalize(dst); dnml_status resize_stat;
    resize_stat = crint_dnml_resize(dst, src.n);
    CHOOSE_OPTION((ret_stat), 
        (resize_stat == DNML_ALLOC_OOM & 
        (ret_stat == CRYPTINT_SUCCESS)), 
        (resize_stat), (ret_stat)
    );
    uint64_t oom_filter = UINT64_MAX;
    limb_t dst_buf[1024] = {0}, src_buf[1024] = {0}; size_t end;
    limb_t *dst_limbs = (ret_stat != DNML_ALLOC_OOM) ? dst->limbs : dst_buf;
    limb_t *src_limbs = (ret_stat != DNML_ALLOC_OOM) ? src.limbs : src_buf;
    CHOOSE_OPTION((dst->cap), (ret_stat != DNML_ALLOC_OOM), (dst->cap), (1024));
    CHOOSE_OPTION((end), (ret_stat != DNML_ALLOC_OOM), (src.n), (1024));
    CHOOSE_OPTION((oom_filter), (ret_stat != DNML_ALLOC_OOM), (oom_filter), (0));
    dst->n = src.n; dst->sign = src.sign;
    __libdnml_memcpy_strict(
        dst_limbs, src_limbs, dst->cap * U64_BYTES, src.n * U64_BYTES,
        0, ((end - 1) * U64_BYTES + (U64_BYTES - 1)), 
        (noop_toggle & resize_stat != DNML_ALLOC_OOM)
    ); 
    /* Clearing Out Values to Invalidity & Aggresive Cleanup */
    dst->n &= oom_filter; dst->cap &= oom_filter;
    dst->poisoned &= oom_filter; dst->sign &= oom_filter;
    resize_stat = 0; oom_filter = 0; end = 0;
    __libdnml_memwipe_strict(dst_buf, 1024 * U64_BYTES, false);
    __libdnml_memwipe_strict(src_buf, 1024 * U64_BYTES, false);
    dst_limbs = 0; src_limbs = 0; return ret_stat;
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
        __libdnml_memwipe_strict(src.limbs, src.cap * U64_BYTES, false);
        crint_free(&src); dst->limbs = 0; dst->n = 0;
        dst->cap = 0; dst->sign = 0; dst->poisoned = 0;
    }); DNML_TEST_ASSERT(((dst->poisoned) && ((src.poisoned))), poisoined, {});
    dnml_status ret_stat = CRYPTINT_SUCCESS; bool noop_toggle = false;
    CHOOSE_OPTION((ret_stat), (dst->poisoned | src.poisoned), (CRYPTINT_POISOINED), (ret_stat));
    CHOOSE_OPTION((noop_toggle), (dst->poisoned | src.poisoned), (true), (noop_toggle));

    /* Main Operation - Copy */
    CHOOSE_OPTION((ret_stat), ((dst->cap < src.n) & (ret_stat == CRYPTINT_SUCCESS)), (CRYPTINT_ERR_RANGE), (ret_stat));
    CHOOSE_OPTION((noop_toggle), ((dst->cap < src.n) & (!noop_toggle)), (true), (noop_toggle));
    __libdnml_memcpy_strict(
        dst->limbs, src.limbs, dst->cap * U64_BYTES, src.n * U64_BYTES,
        0, (src.n - 1) * U64_BYTES + (U64_BYTES - 1), noop_toggle
    ); dst->n = src.n; dst->sign = src.sign;
    __libdnml_memset_strict(
        dst->limbs, 0, dst->cap * U64_BYTES, dst->n * U64_BYTES,
        (dst->cap - 1) * U64_BYTES + (U64_BYTES - 1), noop_toggle
    );
    /* Aggresive Post-Operation Clean-up */
    noop_toggle = 0; noop_toggle = 0; return ret_stat;
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
        __libdnml_memwipe_strict(src.limbs, src.cap * U64_BYTES, false);
        crint_free(&src); dst->limbs = 0; dst->n = 0;
        dst->cap = 0; dst->sign = 0; dst->poisoned = 0;
    }); DNML_TEST_ASSERT(((dst->poisoned) && (!(src.poisoned))), poisoined, {});
    dnml_status ret_status = CRYPTINT_SUCCESS; bool noop_toggle = false;
    CHOOSE_OPTION((ret_status), (dst->poisoned | src.poisoned), (CRYPTINT_POISOINED), (ret_status));
    CHOOSE_OPTION((noop_toggle), (dst->poisoned | src.poisoned), (true), (noop_toggle));

    /* Main Operation - Copy */
    size_t op_range; CHOOSE_OPTION((op_range), (dst->cap < src.n), (dst->cap), (src.n));
    __libdnml_memcpy_strict(
        dst->limbs, src.limbs, dst->cap * U64_BYTES, src.n * U64_BYTES,
        0, (op_range - 1) * U64_BYTES + (U64_BYTES - 1), noop_toggle
    ); dst->n = op_range; dst->sign = src.sign;
    __libdnml_memset_strict(
        dst->limbs, 0, (dst->cap) * U64_BYTES, dst->n * U64_BYTES, 
        (dst->cap - 1) * U64_BYTES + (U64_BYTES - 1), noop_toggle
    );
    /* Aggrestive Post-Operation Clean-up */
    op_range = 0; noop_toggle = 0; return ret_status;
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
    if (err != NULL) {
        CHOOSE_OPTION((*err), (new_stat != DNML_ALLOC_OOM), (CRYPTINT_SUCCESS), (DNML_ALLOC_OOM));
    } dst_tmp_p[0] = 0; new_stat = 0; correctly_set = 0; return dst;
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
    if (err != NULL) {
        CHOOSE_OPTION((*err), (new_stat != DNML_ALLOC_OOM), (CRYPTINT_SUCCESS), (DNML_ALLOC_OOM));
    } dst_tmp_p[0] = 0; new_stat = 0; correctly_set = 0; return dst;
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
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((src.poisoned), poisoined, {});
    dnml_status ret_stat = CRYPTINT_SUCCESS; bool noop_toggle = false;
    CHOOSE_OPTION((ret_stat), (src.poisoned & ret_stat == CRYPTINT_SUCCESS), (CRYPTINT_POISOINED), (ret_stat));
    CHOOSE_OPTION((noop_toggle), (src.poisoned), (true), (noop_toggle));

    /* Main Operation - Copy */
    cryptint dst; dnml_status new_stat; uint64_t correctly_set;
    limb_t dst_tmp_p[1024] = {0}, src_tmp_p[1024] = {0};
    new_stat = crint_snew(&dst, src.n); // Guaraneed dst->cap >= 1
    dst.limbs = (new_stat != DNML_ALLOC_OOM) ? dst.limbs : dst_tmp_p;
    CHOOSE_OPTION((correctly_set), (new_stat != DNML_ALLOC_OOM), (UINT64_MAX), (0));
    limb_t* src_limbs = (new_stat != DNML_ALLOC_OOM) ? src.limbs : src_tmp_p;

    /* Setting Up correctly - Standard Case */
    size_t iter_cnt = (size_t)(src.n / 1024 + 1);
    size_t end; CHOOSE_OPTION(
        (end), (!(src.n)), 0, /* 2nd option: Aligns to the last byte */
        ((src.n - 1) * U64_BYTES + (U64_BYTES - 1))
    ); CHOOSE_OPTION((end), (new_stat != DNML_ALLOC_OOM), (end), (511));
    CHOOSE_OPTION((dst.cap), (new_stat != DNML_ALLOC_OOM), (dst.cap), (1024));
    while (iter_cnt--) __libdnml_memcpy_strict(
        dst.limbs, src.limbs, dst.cap, src.n, 0, end,
        (noop_toggle & new_stat != DNML_ALLOC_OOM)
    ); dst.n = src.n; dst.sign = src.sign; *err = CRYPTINT_SUCCESS;

    /* Setting Up invalid metadata + Agressive Stack Cleanup */
    dst.limbs[0] &= correctly_set; dst.n &= correctly_set;
    dst.cap &= correctly_set; dst.sign &= correctly_set;
    CHOOSE_OPTION((ret_stat), (new_stat != DNML_ALLOC_OOM & (ret_stat == CRYPTINT_SUCCESS)), (new_stat), (ret_stat));
    if (err != NULL) *err = ret_stat; // Conditional Branching here is acceptable
    new_stat = 0; correctly_set = 0; noop_toggle = 0;
    __libdnml_memwipe_strict(dst_tmp_p, 1024 * U64_BYTES, false);
    __libdnml_memwipe_strict(src_tmp_p, 1024 * U64_BYTES, false);
    src_limbs = 0; iter_cnt = 0; end = 0; ret_stat = 0; return dst;
}
cryptint crint_ocopy(cryptint src, size_t output_cap, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(src)), full_contract,
    { /* Set everything to 0 to completely wipe out memory */
        if (src.limbs != NULL) *(src.limbs) = 0;
        crint_free(&src);
    });
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((src.poisoned), poisoined, {});
    dnml_status ret_stat = CRYPTINT_SUCCESS; bool noop_toggle = false;
    CHOOSE_OPTION((ret_stat), (src.poisoned & ret_stat == CRYPTINT_SUCCESS), (CRYPTINT_POISOINED), (ret_stat));
    CHOOSE_OPTION((noop_toggle), (src.poisoned & (!(noop_toggle))), (true), (noop_toggle));

    /* Main Operation - Copy */
    cryptint dst; uint64_t correctly_set; dnml_status new_stat;
    CHOOSE_OPTION((ret_stat), (output_cap < src.n), (CRYPTINT_ERR_RANGE), (ret_stat));
    CHOOSE_OPTION((noop_toggle), ((output_cap < src.n) & (!(noop_toggle))), (true), (noop_toggle));
    limb_t dst_tmp_p[1024] = {0}, src_tmp_p[1024] = {0}; new_stat = crint_snew(&dst, src.n); // Guaraneed dst->cap >= 1
    CHOOSE_OPTION((ret_stat), ((new_stat != CRYPTINT_SUCCESS) & (ret_stat == CRYPTINT_SUCCESS)), (new_stat), (ret_stat));
    CHOOSE_OPTION((correctly_set), (ret_stat != DNML_ALLOC_OOM), (UINT64_MAX), (0));
    limb_t* dst_limbs = (ret_stat != DNML_ALLOC_OOM) ? dst.limbs : dst_tmp_p;
    limb_t* src_limbs = (ret_stat != DNML_ALLOC_OOM) ? src.limbs : src_tmp_p;

    /* Setting Up correctly - Standard Case */
    size_t iter_cnt = (size_t)(src.n / 1024 + 1);
    size_t end; CHOOSE_OPTION(
        (end), (!(src.n)), 0, /* 2nd option: Aligns to the last byte */
        ((src.n - 1) * U64_BYTES + (U64_BYTES - 1))
    ); CHOOSE_OPTION((end), (ret_stat != DNML_ALLOC_OOM), (end), (511));
    CHOOSE_OPTION((dst.cap), (ret_stat != DNML_ALLOC_OOM), (dst.cap), (1024));
    while (iter_cnt--) __libdnml_memcpy_strict(
        dst_limbs, src_limbs, dst.cap, src.n, 0, end,
        (noop_toggle & (new_stat != DNML_ALLOC_OOM))
    ); dst.n = src.n; dst.sign = src.sign;

    /* Setting Up invalid metadata + Agressive Stack Cleanup */
    dst.limbs[0] &= correctly_set; dst.n &= correctly_set;
    dst.cap &= correctly_set; dst.sign &= correctly_set;
    if (err != NULL) *err = ret_stat; // Conditional Branching here is acceptable
    ret_stat = 0; correctly_set = 0; new_stat = 0; noop_toggle = 0;
    __libdnml_memwipe_strict(dst_tmp_p, 1024 * U64_BYTES, false);
    __libdnml_memwipe_strict(src_tmp_p, 1024 * U64_BYTES, false);
    dst_limbs = 0; src_limbs = 0; iter_cnt = 0; end = 0;
    return dst;
}
cryptint crint_tover_copy(cryptint src, size_t output_cap, dnml_status *err) {
    /* Pre-operation Validation & Static Analysis */
    DNML_ASSERT((crint_validate(src)), full_contract,
    { /* Set everything to 0 to completely wipe out memory */
        if (src.limbs != NULL) *(src.limbs) = 0;
        crint_free(&src);
    });
    DNML_TEST_ASSERT((err != NULL), null_err, {});
    DNML_TEST_ASSERT((src.poisoned), poisoined, {});
    dnml_status ret_stat = CRYPTINT_SUCCESS; bool noop_toggle = false;
    CHOOSE_OPTION((ret_stat), (src.poisoned & ret_stat == CRYPTINT_SUCCESS), (CRYPTINT_POISOINED), (ret_stat));
    CHOOSE_OPTION((noop_toggle), (src.poisoned), (true), (noop_toggle));

    /* Main Operation - Copy */
    cryptint dst; dnml_status new_stat; uint64_t correctly_set;
    size_t op_range; CHOOSE_OPTION((op_range), (output_cap < src.n), (output_cap), (src.n));
    limb_t dst_tmp_p[1024] = {0}, src_tmp_p[1024] = {0}; new_stat = crint_snew(&dst, src.n); // Guaraneed dst->cap >= 1
    CHOOSE_OPTION((correctly_set), (new_stat != DNML_ALLOC_OOM), (UINT64_MAX), (0));
    limb_t* dst_limbs = (new_stat != DNML_ALLOC_OOM) ? dst.limbs : dst_tmp_p;
    limb_t* src_limbs = (new_stat != DNML_ALLOC_OOM) ? src.limbs : src_tmp_p;

    /* Setting Up correctly - Standard Case */
    size_t iter_cnt = (size_t)(src.n / 1024 + 1); size_t end;
    CHOOSE_OPTION((end), (!(op_range)), 0, ((op_range - 1) * U64_BYTES + (U64_BYTES - 1)));
    CHOOSE_OPTION((end), (new_stat != DNML_ALLOC_OOM), (end), (511));
    CHOOSE_OPTION((dst.cap), (new_stat != DNML_ALLOC_OOM), (dst.cap), (1024));
    while (iter_cnt--) __libdnml_memcpy_strict(
        dst.limbs, src.limbs, dst.cap, src.n, 0, end,
        (noop_toggle & (new_stat != DNML_ALLOC_OOM))
    ); dst.n = src.n; dst.sign = src.sign;

    /* Setting Up invalid metadata - DNML_ALLOC_OOM */
    dst.limbs[0] &= correctly_set; dst.n &= correctly_set;
    dst.cap &= correctly_set; dst.sign &= correctly_set;
    CHOOSE_OPTION((ret_stat), (new_stat != CRYPTINT_SUCCESS & ret_stat == CRYPTINT_SUCCESS), (new_stat), (ret_stat));
    if (err != NULL) *err = ret_stat; // Conditional Branching here is acceptable
    ret_stat = 0; new_stat = 0; correctly_set = 0; op_range = 0;
    __libdnml_memwipe_strict(dst_tmp_p, 1024 * U64_BYTES, false); dst_limbs = 0;
    __libdnml_memwipe_strict(src_tmp_p, 1024 * U64_BYTES, false); src_limbs = 0;
    iter_cnt = 0; end = 0; noop_toggle = 0; return dst;
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
    cap_invalid = 0; n_overflow = 0; sign_invalid = 0;
}
void crint_normalize(cryptint *x) { __CRINT_TRIM_LZ__(x); CHOOSE_OPTION((x->sign), (!x->n), (1), (-1)); }
dnml_status crint_resize(cryptint *x, size_t k) {
    // Pre-operation Validation & Static Analysis
    DNML_ASSERT(__STORAGE_VAL__(x), store_inval, {
        x->limbs = 0; x->cap = 0; x->n = 0; 
        x->poisoned = 0; x->sign = 0;
    }); 
    DNML_TEST_ASSERT((x->poisoned), poisoined, {});
    DNML_TEST_ASSERT((k), "Invalid Capacity Request (-Einval_cap_request)", {});
    dnml_status ret_stat = CRYPTINT_SUCCESS; uint64_t oom_mask = UINT64_MAX;
    CHOOSE_OPTION((ret_stat), (x->poisoned & (ret_stat == CRYPTINT_SUCCESS)), (CRYPTINT_POISOINED), (ret_stat));


    /* Memory Clearance on Resizing to a smaller size */
    size_t start, end, op_cap;
    CHOOSE_OPTION((start), (k < x->cap), (k - 1), (0));
    CHOOSE_OPTION((end), (k < x->cap), (x->cap - 1), (0));
    __libdnml_memset_strict(
        x->limbs, 0, x->cap * U64_BITS, start * U64_BYTES, 
        end * U64_BYTES + (U64_BYTES - 1), (x->poisoned)
    );

    /* Main Resizing */
    limb_t* operated = (ret_stat == CRYPTINT_POISOINED) ? malloc(1) : x->limbs;
    size_t normalized_size; NORMALIZE_0_TO_1(normalized_size, k); size_t op_size; 
    CHOOSE_OPTION((op_size), (ret_stat == CRYPTINT_POISOINED), (1), (normalized_size));
    limb_t *__BUFFER_P = realloc(operated, op_size * U64_BYTES);
    DNML_TEST_ASSERT((__BUFFER_P != NULL), realloc_null, { if (operated != x->limbs) free(operated); });
    CHOOSE_OPTION((ret_stat), ((__BUFFER_P == NULL) & (ret_stat == CRYPTINT_SUCCESS)), (DNML_ALLOC_OOM), (ret_stat));
    CHOOSE_OPTION((oom_mask), ((__BUFFER_P == NULL) & (ret_stat != CRYPTINT_POISOINED)), (0), (oom_mask));

    x->limbs = (ret_stat != CRYPTINT_POISOINED) ? ((uintptr_t)(__BUFFER_P) & oom_mask) : x->limbs;
    CHOOSE_OPTION((x->cap), (ret_stat != CRYPTINT_POISOINED), (normalized_size & oom_mask), (x->cap));
    CHOOSE_OPTION((x->n), (ret_stat != CRYPTINT_POISOINED), (min(normalized_size, x->cap) & oom_mask), (x->n));
    /* Post-operation Aggrestive Clearance */
    if (ret_stat == CRYPTINT_POISOINED & __BUFFER_P != NULL) free(__BUFFER_P);
    start = 0; end = 0; op_cap = 0; operated = 0; oom_mask = 0;
    __BUFFER_P = 0; normalized_size = 0; op_size = 0; return ret_stat;
}
dnml_status crint_reserve(cryptint *x, size_t k) {
    // Pre-operation Validation & Static Analysis
    DNML_ASSERT(__STORAGE_VAL__(x), store_inval, {
        x->limbs = 0; x->cap = 0; x->n = 0;
        x->poisoned = 0; x->sign = 0;
    });
    DNML_TEST_ASSERT((x->poisoned), poisoined, {});
    dnml_status ret_stat = CRYPTINT_SUCCESS; uint64_t oom_mask = UINT64_MAX;
    CHOOSE_OPTION((ret_stat), (x->poisoned & (ret_stat == CRYPTINT_SUCCESS)), (CRYPTINT_POISOINED), (ret_stat));

    // Main Resizing
    limb_t* operated = (ret_stat == CRYPTINT_POISOINED) ? malloc(1) : x->limbs;
    size_t new_cap = x->cap; while (new_cap < k) new_cap *= 2;
    limb_t* __BUFFER_P = realloc(operated, new_cap * U64_BYTES);
    DNML_TEST_ASSERT(__BUFFER_P != NULL, realloc_null, {});
    CHOOSE_OPTION((ret_stat), ((__BUFFER_P == NULL) & (ret_stat == CRYPTINT_SUCCESS)), (DNML_ALLOC_OOM), (ret_stat));
    CHOOSE_OPTION((oom_mask), ((__BUFFER_P == NULL) & (ret_stat != CRYPTINT_POISOINED)), (0), (oom_mask));
    x->limbs = (ret_stat != CRYPTINT_POISOINED) ? ((uintptr_t)(__BUFFER_P) & oom_mask) : x->limbs;
    CHOOSE_OPTION((x->cap), (ret_stat != CRYPTINT_POISOINED), (new_cap & oom_mask), (x->cap));
    /* Post-operation Aggrestive Clearance */
    if (ret_stat == CRYPTINT_POISOINED & __BUFFER_P != NULL) free(__BUFFER_P);
    oom_mask = 0; operated = 0; new_cap = 0; __BUFFER_P = 0; return ret_stat;
    
}
dnml_status crint_shrink(cryptint *x, size_t k) { /* Maximum capacity */
    // Pre-operation Validation & Static Analysis
    DNML_ASSERT(__STORAGE_VAL__(x), store_inval, {
        x->limbs = 0; x->cap = 0; x->n = 0; 
        x->poisoned = 0; x->sign = 0;
    });
    DNML_TEST_ASSERT((x->poisoned), poisoined, {});
    DNML_TEST_ASSERT((k), "Invalid Capacity Request (-Einval_cap_request)", {});
    dnml_status ret_stat = CRYPTINT_SUCCESS; uint64_t oom_mask = UINT64_MAX;
    CHOOSE_OPTION((ret_stat), (x->poisoned & (ret_stat == CRYPTINT_SUCCESS)), (CRYPTINT_POISOINED), (ret_stat));

    /* Memory Clearance on Resizing to a smaller size */
    size_t start, end, op_cap, new_cap = x->cap; while (new_cap > k) --new_cap;
    CHOOSE_OPTION((start), (new_cap < x->cap), (new_cap - 1), (0));
    CHOOSE_OPTION((end), (new_cap < x->cap), (x->cap - 1), (0));
    __libdnml_memset_strict(
        x->limbs, 0, x->cap * U64_BITS, start * U64_BYTES, 
        end * U64_BYTES + (U64_BYTES - 1), (x->poisoned)
    );

    // Main Resizing
    limb_t* operated = (ret_stat == CRYPTINT_POISOINED) ? malloc(1) : x->limbs;
    size_t normalized_size; NORMALIZE_0_TO_1(normalized_size, k); size_t opsize;
    CHOOSE_OPTION((opsize), (ret_stat == CRYPTINT_POISOINED), (1), (normalized_size));
    limb_t* __BUFFER_P = realloc(operated, opsize * U64_BYTES);
    DNML_TEST_ASSERT((__BUFFER_P != NULL), realloc_null, { if (operated != x->limbs) free(operated); });
    CHOOSE_OPTION((ret_stat), ((__BUFFER_P == NULL) & (ret_stat == CRYPTINT_SUCCESS)), (DNML_ALLOC_OOM), (ret_stat));
    CHOOSE_OPTION((oom_mask), ((__BUFFER_P == NULL) & (ret_stat != CRYPTINT_POISOINED)), (0), (oom_mask));

    x->limbs = (ret_stat != CRYPTINT_POISOINED) ? ((uintptr_t)(__BUFFER_P) & oom_mask) : x->limbs;
    CHOOSE_OPTION((x->cap), (ret_stat != CRYPTINT_POISOINED), (normalized_size & oom_mask), (x->cap));
    CHOOSE_OPTION((x->n), (ret_stat != CRYPTINT_POISOINED), (min(normalized_size, x->cap) & oom_mask), (x->n));
    /* Post-operation Aggrestive Clearance */
    if (ret_stat == CRYPTINT_POISOINED & __BUFFER_P != NULL) free(__BUFFER_P);
    start = 0; end = 0; op_cap = 0; operated = 0; oom_mask = 0;
    __BUFFER_P = 0; normalized_size = 0; opsize = 0; return ret_stat;
}
dnml_status crint_reset(cryptint *x) {
    DNML_ASSERT(__STORAGE_VAL__(x),
    "Partial Contract Violation: CryptInt State Invariant is violated "
    "(-Ecrypt_int_state_invalid)", {
        x->limbs = 0; x->cap = 0; x->n = 0;
        x->poisoned = 0; x->sign = 0;
    });
    DNML_TEST_ASSERT((x->poisoned), poisoined, {});
    dnml_status ret_stat = CRYPTINT_SUCCESS;
    CHOOSE_OPTION((ret_stat), (x->poisoned), (CRYPTINT_POISOINED), (ret_stat));
    __libdnml_memwipe_strict(x->limbs, x->cap * U64_BYTES, (x->poisoned));
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
    if (x == NULL) return false;
    if (x->limbs == NULL) return false;
    if (x->cap < 1) return false;
    if (x->n > x->cap) return false;
    if (x->sign != 1 && x->sign != -1) return false;
    /* Arithmetic Validation */
    if (x->limbs[x->n - 1] == 0) return false;
    if (x->n == 0 && x->sign != 1) return false;
    return true;
}


