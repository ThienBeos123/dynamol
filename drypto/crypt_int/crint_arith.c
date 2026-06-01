#include "cryptInt_func.h"
#include "crint_algo_core.h"


//* ========================================= MAGNITUDE MATHEMATICA ========================================== *//
/*
* IMPORTANT NOTE FOR LIB-DNML DEVELOPERS - REPETITION OF API-DESING.md:
*   1. Any usage of MAGNITUDED MATHEMATICAL FUNCTIONS MUST ensure that the result 
*      destination CryptInt has satisafctory capacity, non-poisoned, and has a valid 
*      pointer address to allocated buffers to reduce error handling flow from upstream
*
*   2. Any usage of MAGNITUDED MATHEMATICAL FUNCTIONS MUST ensure that the input
*      CryptInt be filtered, handled, and saturated multiplexed to prevent any unwanted errors
*      regarding segmentation faults, mathematically impossible outputs, test-assert triggering,
*      or allocation failures. This is also mainly to reduce error handling work from upstream
*
*   3. You MUST use heap-allocation for the temporary destination of operation results
*      for easier memory management and opaque, up-front OOM handling. This is for 
*      the same reason as detailed in (1), in which the public-facing functions
*      is responsible for up-front error signals passing and falsehood buffers multiplexing, 
*      while the magnituded engine are only responsible for the brute, algorithmic work
*      and setup, with minimal DNML_TEST_ASSERT for testing. At most, there will be
*      error returns for function that utilizes arena allocation for its algorithmic core
*/
/* ------------------- MAGNITUDED ARITHMETIC ------------------- */
void __CRINT_MAGADD__(crint *res, crint *a, crint *b) {
    _pre_assert(res, { crint_free(res); crint_free(a); crint_free(b); });
    _pre_assert(a, { crint_free(res); crint_free(a); crint_free(b); });
    _pre_assert(b, { crint_free(res); crint_free(a); crint_free(b); });
    DNML_TEST_ASSERT((_lib_crt_geq(res->cap, crtmax(a->n, b->n) + 1)),
        "Insufficient Sum Buffer: Capacity Unsatisfactory for a + b (-Eadd_insufficient_cap)", 
        { crint_free(res); crint_free(a); crint_free(b); }
    ); __CRINT_ADD_WC__(res, a, b);
}
void __CRINT_MAGSUB__(crint *res, crint *a, crint *b) {
    _pre_assert(res, { crint_free(res); crint_free(a); crint_free(b); });
    _pre_assert(a, { crint_free(res); crint_free(a); crint_free(b); });
    _pre_assert(b, { crint_free(res); crint_free(a); crint_free(b); });
    DNML_TEST_ASSERT((__CRINT_INTERNAL_CMP__(a, b) != -1),
        "Subtraction Underflow: Subtrahend's magnitude is too large for Minuend"
        " (-Esub_underflow)", { crint_free(res); crint_free(a); crint_free(b); }
    ); __CRINT_SUB_WC__(res, a, b);
}
void __CRINT_MAGMUL___(crint *res, crint *a, crint *b) {
    _pre_assert(res, { crint_free(res); crint_free(a); crint_free(b); });
    _pre_assert(a, { crint_free(res); crint_free(a); crint_free(b); });
    _pre_assert(b, { crint_free(res); crint_free(a); crint_free(b); });
    DNML_TEST_ASSERT(
        (_lib_crt_geq(res->cap, a->n + b->n)), // Result Capacity >= Sum of multiplicand's length
        "Insufficient Product Capacity: Capacity insatisfactory for a * b (-Emul_insufficient_cap)",
        { crint_free(res); crint_free(a); crint_free(b); }
    ); __CRINT_MUL_DISP__(a, b, res);
}
void __CRINT_MAGDIVMOD__(crint *quot, crint *rem, crint *a, crint *b) {
    _pre_assert(quot, { crint_free(quot); crint_free(rem); crint_free(a); crint_free(b); })
    _pre_assert(rem, { crint_free(quot); crint_free(rem); crint_free(a); crint_free(b); })
    _pre_assert(a, { crint_free(quot); crint_free(rem); crint_free(a); crint_free(b); })
    _pre_assert(b, { crint_free(quot); crint_free(rem); crint_free(a); crint_free(b); })
    DNML_TEST_ASSERT((b->n),
        "Mathematical Undefinindness: Division by 0 (-Ediv_by_zero)", 
        { crint_free(quot); crint_free(rem); crint_free(a); crint_free(b); }
    );
    DNML_TEST_ASSERT((quot->cap >= a->n),
        "Insufficient Quotient Capacity: Capacity unsatisfactory for a / b (-Ediv_insufficient_qcap)",
        { crint_free(quot); crint_free(rem); crint_free(a); crint_free(b); }
    );
    DNML_TEST_ASSERT((rem->cap >= b->n),
        "Insufficient Remainder Capacity: Capacity unsatisfactory for a mod(b) (-Ediv_insufficient_rcap)",
        { crint_free(quot); crint_free(rem); crint_free(a); crint_free(b); }
    ); __CRINT_DIVMOD_DISP__(a, b, quot, rem);
}
void __CRINT_MAGMUL_U64__(crint *res, crint *x, uint64_t val) {
    DNML_TEST_ASSERT(
        (_lib_crt_geq(res->cap, x->n + 1)), // Result Capacity >= Sum of multiplicand's length
        "Insufficient Product Capacity: Capacity insatisfactory for a * b (-Emul_insufficient_cap)",
        { crint_free(res); crint_free(x); val = 0; }
    ); uint64_t carry = 0, lo, hi, sum;
    for (size_t i = 0; _lib_crt_lt(i, x->n); ++i) {
        lo = __CRT_MUL_U64__(x->limbs[i], val, &hi); sum = lo + carry;
        carry = hi + (_lib_crt_lt(sum, lo)) + (_lib_crt_lt(sum, carry));
        res->limbs[i] = sum;
    }
    res->n = x->n + (!!(carry)); uint64_t curr = res->limbs[res->n - !(carry)];
    CHOOSE_OPTION((res->limbs[res->n - !(carry)]), (carry), (carry), (curr)); // clang-format off
    carry = 0; lo = 0; hi = 0; sum = 0; curr = 0; val = 0; // clang-format on
}
void __CRINT_MAGDIVMOD_U64__(crint *quot, crint *rem, crint *a, uint64_t val) {
    DNML_TEST_ASSERT( /* Check for a zero divisor */
        val, "Mathematical Undefinindness: Division by 0 (-Ediv_by_zero)",
        { crint_free(quot); crint_free(rem); crint(a); val = 0; }
    );
    DNML_TEST_ASSERT((quot->cap >= a->n),
        "Insufficient Quotient Capacity: Capacity unsatisfactory for a / b (-Ediv_insufficient_qcap)",
        { crint_free(quot); crint_free(rem); crint_free(a); val = 0; }
    );
    DNML_TEST_ASSERT((rem->cap >= 1),
        "Insufficient Remainder Capacity: Capacity unsatisfactory for a mod(b) (-Ediv_insufficient_rcap)",
        { crint_free(quot); crint_free(rem); crint_free(a); val = 0; }
    ); __CRINT_SHORT_DIVISION__(a, val, quot, rem);
}




//* ============================================ SIGNED ARITHMETIC ========================================== */
/* ------------------- MUTATIVE ARITHMETIC -------------------- */
/* ------------------ FUNCTIONAL ARITHMETIC ------------------- */






