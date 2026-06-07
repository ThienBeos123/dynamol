/*
Copyright (C) 2026 @ThienBeos123/@Poly-glon

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

  http://apache.org

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/



#include "crt_mul.h"

/* CRYPTINT ALGORITHMS */
dnml_status __CRINT_SCHOOLBOOK__(crint *a, crint *b, crint *res) {
    // Static Analysis
    crint_poison(a); crint_poison(b);
    crint_poison(res); DNML_TEST_ASSERT(
        (_lib_crt_geq(res->cap, a->n + b->n)),
        "Insufficient Product Capacity: Capacity insatisfactory for a * b (-Emul_insufficient_cap)", 
        {__CRINT_IFREE__(a);__CRINT_IFREE__(b);__CRINT_IFREE__(res); }
    ); // Main Algorithms
    __libdnml_smemset_u64(res->limbs, 0, res->cap, (size_t)0, (size_t)res->cap, false);
    for (size_t i = 0; _lib_crt_lt(i, a->n); ++i) {
        uint64_t carry = 0;
        for (size_t j = 0; _lib_crt_lt(j, b->n); ++j) {
            uint64_t lo, hi; lo = __CRT_MUL_U64__(a->limbs[i], b->limbs[j], &hi);
            uint64_t sum = res->limbs[i + j] + lo; uint8_t c1 = (_lib_crt_lt(sum, lo));
            uint64_t sum2 = sum += carry; uint8_t c2 = (_lib_crt_lt(sum2, lo));
            carry = hi + (c1 | c2); res->limbs[i + j] = sum2;
        } res->limbs[i + b->n] += carry;
    } res->n = a->n + b->n; __CRINT_TRIM_LZ__(res);
    __libdnml_smemset_u64(res->limbs, 0, res->cap, res->n, res->cap - 1, false);
    return CRINT_SUCCESS;
}
dnml_status __CRINT_NTT__(crint *a, crint *b, crint *res) { return CRINT_SUCCESS; }
dnml_status __CRINT_MUL_DISP__(crint *a, crint *b, crint *res) {
    if (a->n <= BIGINT_SCHOOLBOOK && b->n <= BIGINT_SCHOOLBOOK)
        return __CRINT_SCHOOLBOOK__(a, b, res);
    else return __CRINT_NTT__(a, b, res);
}
