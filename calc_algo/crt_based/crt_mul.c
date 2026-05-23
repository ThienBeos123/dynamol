#include "crt_mul.h"


/* CRYPTINT WORKSPACE SIZE */
size_t __CRINT_NTT_WS__(size_t a_size, size_t b_size) {}
size_t __CRINT_MUL_WS__(size_t a_size, size_t b_size) {
    if (a_size <= BIGINT_SCHOOLBOOK && b_size <= BIGINT_SCHOOLBOOK) return 0;
    else return __CRINT_NTT_WS__(a_size, b_size);
}



/* CRYPTINT ALGORITHMS */
dnml_status __CRINT_SCHOOLBOOK__(const cryptint *a, const cryptint *b, cryptint *res) {
    // Static Analysis
    cryptInt_poison(a); cryptInt_poison(b); 
    cryptInt_poison(res); DNML_TEST_ASSERT(
        (res->cap >= a->n + b->n),
        "Insufficient Product Capacity: Capacity insatisfactory for a * b"
        "(-Emul_insufficient_cap)"
    ); // Main Algorithms
    __libdnml_memset_strict(res->limbs, 0, res->cap, (size_t)0, (size_t)res->cap);
    for (size_t i = 0; i < a->n; ++i) {
        uint64_t carry = 0;
        for (size_t j = 0; j < b->n; ++j) {
            uint64_t lo, hi;
            lo = __MUL_UI64__(a->limbs[i], b->limbs[j], &hi);
            uint64_t sum = res->limbs[i + j] + lo; uint8_t c1 = (sum < lo);
            uint64_t sum2 = sum += carry; uint8_t c2 = (sum2 < carry);
            carry = hi + (c1 | c2); res->limbs[i + j] = sum2;
        } res->limbs[i + b->n] += carry;
    } res->n = a->n + b->n; __CRINT_TRIM_LZ__(res);
    __libdnml_memset_strict(res->limbs, 0, res->cap, res->n, res->cap - 1);
    return CRYPTINT_SUCCESS;
}
dnml_status __CRINT_NTT__(const cryptint *a, const cryptint *b, cryptint *res, calc_ctx *ntt_ctx) {}
dnml_status __CRINT_MUL_DISP__(const cryptint *a, const cryptint *b, cryptint *res, calc_ctx *mul_ctx) {
    if (a->n <= BIGINT_SCHOOLBOOK && b->n <= BIGINT_SCHOOLBOOK) 
        return __CRINT_SCHOOLBOOK__(a, b, res);
    else return __CRINT_NTT__(a, b, res, mul_ctx);
}