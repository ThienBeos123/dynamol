#include "crt_div.h"

/* CRYPTINT WORKSPACE SIZE */



/* CRYPTINT ALGORITHMS */
drypto_stat __BIGINT_SHORT_DIVISION__(const cryptInt *a, uint64_t b, cryptInt *quot, cryptInt *rem) {
    // Static Analysis
    cryptInt_poison(a); cryptInt_poison(quot); 
    cryptInt_poison(rem); DNML_ASSERT((b),
        "Division by zero (b == 0) (-Ediv_by_zero)"
    );
    DNML_ASSERT((quot->cap >= a->n),
        "quot->cap < a->n --- (a / ui64)"
        " (-Eshort_div_insufficient_cap)"
    );
    // Main Algorithms
    if (a->poisoned || quot->poisoned || rem->poisoned) return CRYPTINT_POISOINED;
    uint64_t remainder = 0; for (size_t i = a->n; i > 0; --i) {
        quot->limbs[i - 1] = __DIV_HELPER_UI64__(remainder, a->limbs[i - 1], b, &remainder);
    } __CRINT_TRIM_LZ__(rem);
    quot->sign = !!(quot->n);
    rem->limbs[0] = remainder;
    rem->n = !!(rem); rem->sign = 1; 
    return CRYPTINT_SUCCESS;
}
drypto_stat __BIGINT_KNUTH_D__(const cryptInt *a, const cryptInt *b, cryptInt *quot, cryptInt *rem, calc_ctx knuth_ctx) {}