#include "crt_div.h"

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
    uint64_t remainder = 0; uint8_t ovf_test;
    for (size_t i = a->n - 1; _lib_crt_neq(i, -1); --i) {
        quot->limbs[i] = __CRT_DIV_U128__(remainder, a->limbs[i], b, &remainder, &ovf_test);
        DNML_TEST_ASSERT(ovf_test, "CRITICIAL DEBUG ERROR: Division quotient's overflowed", {});
    }
    __CRINT_TRIM_LZ__(rem); CHOOSE_OPTION((quot->sign), (!(quot->n)), (1), (quot->sign));
    rem->limbs[0] = remainder; rem->n = !!(rem); rem->sign = 1; return CRINT_SUCCESS;
}
dnml_status __CRINT_NEWTON_RECP__(
    const crint *dend, const crint *div,
    crint *quot, crint *rem
) {}
dnml_status __CRINT_DIVMOD_DISP__(
    const crint *a, const crint *b,
    crint *quot, crint *rem
) {
    if (b->n < BIGINT_SHORT) return __CRINT_SHORT_DIVISION__(a, b->limbs[0], quot, rem);
    else return __CRINT_NEWTON_RECP__(a, b, quot, rem);
}