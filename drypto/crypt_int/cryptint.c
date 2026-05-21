#include "cryptInt_func.h"




//* ===================================== TYPE SETUP FUNCTION ===================================== *//
void crint_free(cryptInt *x) {
    if (x->limbs == NULL) return;
    __libdnml_memwipe_strict(x->limbs, x->cap);
    free(x->limbs); x->limbs = NULL;
    x->n = 0; x->cap = 0; x->sign = 0;
    x->poisoned = 0;
}
void crint_new(cryptInt *x){
    if (x->limbs != NULL) return; // already initialized
    limb_t *P_BUFFER__ = calloc(1, sizeof(limb_t));
    DNML_ASSERT(P_BUFFER__ != NULL,
    "Allocation Failure: calloc() returned NULL (-Ealloc_malloc_fail)",
    { /* Set everything to 0 to completely wipe out memory */
        x->n = 0; x->sign = 0; x->cap = 0;
        x->poisoned = 0; x->limbs = 0;
    }); x->limbs = P_BUFFER__; x->cap = 1;
    x->n = 0; x->sign = 1; x->poisoned = false;
}
void crint_snew(cryptInt *x, const size_t n) {
    if (x->limbs != NULL) return; // already initialized
    limb_t *__BUFFER_P = calloc(n, sizeof(limb_t));
    if (__BUFFER_P == NULL) abort();
    x->limbs = __BUFFER_P; x->cap = n;
    x->n = 0; x->sign = 1; x->poisoned = false;
}
drypto_stat crint_cinew(cryptInt *x, cryptInt *y) {
    // Pre-operation Validation & Static Analysis
    if (x->limbs != NULL) return; // already initialized
    DNML_ASSERT((crint_pvalidate(y)), 
    "Contract Violation: Invalid CryptInt (-Ecrypt_int_invalid)",
        { /* Set everything to 0 to completely wipe out memory */
            __libdnml_memwipe_strict(y->limbs, y->cap);
            crint_free(y); x->limbs = NULL; x->n = 0;
            x->cap = 0; x->sign = 0; x->poisoned = 0;
        }
    ); DNML_TEST_ASSERT((!(y->poisoned)), "Mathematical Error: CryptInt Poisoned (-Ecrypt_int_invalid)");
    if (y->poisoned) { x->n = 0; x->sign = 0; x->cap = 0; x->limbs = 0; return CRYPTINT_POISOINED; }
    // Main Operations
    if (x->limbs == y->limbs) {
        if (x == y) return;
        x->n = y->n; x->cap = y->cap;
        x->sign = y->sign; x->poisoned = y->poisoned;
    } size_t alloc_size = (y->n) ? y->n : 1;
    limb_t *__BUFFER_P = calloc(y->n, sizeof(limb_t));
    DNML_ASSERT(__BUFFER_P != NULL,
    "Allocation Failure: calloc() returned NULL (-Ealloc_malloc_fail)",
    { /* Set everything to 0 to completely wipe out memory */
        crint_free(y); y->limbs = 0; x->limbs = 0; x->n = 0;
        x->cap = 0; x->sign = 0; x->poisoned = 0;
    }
    ); x->limbs = __BUFFER_P;
    if (y->n) __libdnml_memcpy_strict(
        x->limbs, y->limbs, 
        y->n * sizeof(limb_t), 0, 
        y->n * sizeof(limb_t)
    ); x->n = y->n; x->cap = alloc_size; 
    x->sign = (y->n) ? y->sign : 1; x->poisoned = false;
}
drypto_stat crint_new_u64(cryptInt *x, const uint64_t in) {
    if (x->limbs != NULL) return; // already initialized
    limb_t *__BUFFER_P = calloc(1, sizeof(limb_t));
    DNML_ASSERT(__BUFFER_P != NULL,
    "Allocation Failure: calloc() returned NULL (-Ealloc_malloc_fail)",
    { /* Set everything to 0 to completely wipe out memory */
        x->n = 0; x->sign = 0; x->cap = 0;
        x->poisoned = 0; x->limbs = 0;
    }); x->limbs = __BUFFER_P; x->cap = 1;
    x->limbs[0] = in; x->n = !!(in); x->sign = 1;
    x->poisoned = false;
}
drypto_stat crint_new_i64(cryptInt *x, const int64_t in) {
    if (x->limbs != NULL) return; // already initialized
    limb_t *__BUFFER_P = calloc(1, sizeof(limb_t));
    DNML_ASSERT(__BUFFER_P != NULL,
    "Allocation Failure: calloc() returned NULL (-Ealloc_malloc_fail)",
    { /* Set everything to 0 to completely wipe out memory */
        x->n = 0; x->sign = 0; x->cap = 0;
        x->poisoned = 0; x->limbs = 0;
    }); x->limbs = __BUFFER_P; 
    x->limbs[0] = __MAG_I64__(in); x->cap = 1; x->n = !!(in); 
    x->sign = (in < 0) ? 1 : -1; x->poisoned = false;
}
drypto_stat crint_new_f128(cryptInt *x, long double in) {}






//* ===================================== STATE ALTERATION FUNCTIONS ===================================== *//
void crint_canonicalize(cryptInt *x) {}
void crint_normalize(cryptInt *x) {}
void crint_resize(cryptInt *x, size_t k) {}
void crint_reserve(cryptInt *x, size_t k) {}
void crint_shrink(cryptInt *x, size_t k) {}
void crint_reset(cryptInt *x, size_t k) {}
static inline uint8_t __MUT_SUBJ_VAL__(cryptInt *x) { return (x->limbs != NULL); }
bool crint_validate(cryptInt x) {
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
bool crint_pvalidate(cryptInt *x) {
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