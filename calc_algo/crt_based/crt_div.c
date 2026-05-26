#include "crt_div.h"

/* CRYPTINT WORKSPACE SIZE */
size_t __CRINT_NEWTON_WS__(size_t dend_size, size_t div_size) {}
size_t __CRINT_DIV_WS__(size_t dend_size, size_t div_size) {
    if (div_size < BIGINT_SHORT) return 0;
    else return __CRINT_NEWTON_WS__(dend_size, div_size);
}


/* CRYPTINT ALGORITHMS */
dnml_status __CRINT_SHORT_DIVISION__(const crint *a, uint64_t b, crint *quot, crint *rem) {
    // Static Analysis
    crint_poison(a); crint_poison(quot); 
    crint_poison(rem); DNML_TEST_ASSERT((b),
        "Mathematical Undefinindness: Division by 0 "
        "(-Ediv_by_zero)", {}
    );
    DNML_TEST_ASSERT((quot->cap >= a->n),
        "Insufficient Quotient Capacity: Capacity unsatisfactory for a / b "
        "(-Eshort_div_insufficient_cap)", {}
    );
    // Main Algorithms
    uint64_t remainder = 0; for (size_t i = a->n; i > 0; --i) {
        quot->limbs[i - 1] = __DIV_HELPER_UI64__(remainder, a->limbs[i - 1], b, &remainder);
    } __CRINT_TRIM_LZ__(rem);
    CHOOSE_OPTION((quot->sign), (!(quot->n)), (1), (quot->sign));
    rem->limbs[0] = remainder; rem->n = !!(rem); rem->sign = 1;
    return CRINT_SUCCESS;
}
dnml_status __CRINT_NEWTON_RECP__(
    const crint *dend, const crint *div,
    crint *quot, crint *rem, 
    calc_ctx newton_ctx
) {}
dnml_status __CRINT_DIVMOD_DISP__(
    const crint *a, const crint *b,
    crint *quot, crint *rem,
    calc_ctx dvmod_ctx
) {
    if (b->n < BIGINT_SHORT) return __CRINT_SHORT_DIVISION__(a, b->limbs[0], quot, rem);
    else return __CRINT_NEWTON_RECP__(a, b, quot, rem, dvmod_ctx);
}