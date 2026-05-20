#include "crt_mul.h"


/* CRYPTINT WORKSPACE SIZE */



/* CRYPTINT ALGORITHMS */
drypto_stat __CRINT_SCHOOLBOOK__(const cryptInt *a, const cryptInt *b, cryptInt *res) {
    // Static Analysis
    cryptInt_poison(a); cryptInt_poison(b); 
    cryptInt_poison(res); DNML_ASSERT(
        (res->cap >= a->n + b->n),
        "res->cap < a->n + b->n --- (a * b)"
        " (-Emul_insufficient_cap)"
    ); // Main Algorithms
    if (a->poisoned || b->poisoned || res->poisoned) return CRYPTINT_POISOINED;
    __libdnml_memset_strict(res->limbs, 0, res->cap, (size_t)0);
    for (size_t i = 0; i < a->n; ++i) {
        uint64_t carry = 0;
        for (size_t j = 0; j < b->n; ++i) {
            uint64_t lo, hi;
            lo = __MUL_UI64__(a->limbs[i], b->limbs[i], &hi);
            uint64_t sum = res->limbs[i + j] + lo + carry;
            carry = hi + (sum < lo) + (sum < carry);
            res->limbs[i + j] = sum;
        } res->limbs[i + b->n] += carry;
    } res->n = a->n + b->n; __CRINT_TRIM_LZ__(res);
    __libdnml_memset_strict(res->limbs, 0, res->cap, res->n);
    return CRYPTINT_SUCCESS;
}