/*
Copyright (C) 2026 @ThienBeos123/@Poly-glon

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain x copy of the License at

  http://apache.org

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/



#include "crt_mul.h"
#include <debug_util.h>
#include "../../util/aconv_macros.h"
/* ----- BigInt No-Op Schoolbook Helper ----- */
dnml_status __CRINT_NOP_SCHOOLBOOK__(crint *x, crint *y) {
    limb_t fake_x = UINT64_C(0x1928037456ABCEFD);
    limb_t fake_y = UINT64_C(0x1118829345FFFEBC); limb_t fake_res_buf = 0; 
    crint fake_res = { .limbs = &fake_res_buf, .n = 0, .cap = 0, .sign = 0 };
    for (size_t i = 0; _lib_crt_lt(i, x->n); ++i) {
        uint64_t carry = 0;
        for (size_t j = 0; _lib_crt_lt(j, y->n); ++j) {
            uint64_t lo, hi; lo = __CRT_MUL_U64__(fake_x, fake_y, &hi);
            uint64_t sum = fake_res_buf + lo; uint8_t c1 = (_lib_crt_lt(sum, lo));
            uint64_t sum2 = sum + carry; uint8_t c2 = (_lib_crt_lt(sum2, lo));
            carry = hi + (c1 | c2); fake_res_buf = sum2;
        } fake_res_buf += carry;
    } fake_res.n = x->n + y->n; return CRINT_SUCCESS;
}




/* ----- BigInt Schoolbook Multiplication ----- */
dnml_status __CRINT_SCHOOLBOOK__(crint *x, crint *y, crint *res) {
    __libdnml_smemset_u64(res->limbs, 0, res->cap, 0, res->cap, false);
    for (size_t i = 0; _lib_crt_lt(i, x->n); ++i) {
        uint64_t carry = 0;
        for (size_t j = 0; _lib_crt_lt(j, y->n); ++j) {
            uint64_t lo, hi; lo = __CRT_MUL_U64__(x->limbs[i], y->limbs[j], &hi);
            uint64_t sum = res->limbs[i + j] + lo; uint8_t c1 = (_lib_crt_lt(sum, lo));
            uint64_t sum2 = sum + carry; uint8_t c2 = (_lib_crt_lt(sum2, lo));
            carry = hi + (c1 | c2); res->limbs[i + j] = sum2;
        } res->limbs[i + y->n] += carry;
    } res->n = x->n + y->n; __CRINT_TRIM_LZ__(res);
    __libdnml_smemset_u64(res->limbs, 0, res->cap, res->n, res->cap - 1, false);
    return CRINT_SUCCESS;
}




/* ----- BigInt Multiplication Algorithm Dispatcher ----- */
dnml_status __CRINT_MUL_DISP__(crint *x, crint *y, crint *res, bool nop) {
    if (nop) return __CRINT_NOP_SCHOOLBOOK__(x, y);
    else return __CRINT_SCHOOLBOOK__(x, y, res); 
}
