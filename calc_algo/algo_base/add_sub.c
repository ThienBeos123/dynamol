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



#include "add_sub.h"
#include <debug_util.h>
#include "../../util/aconv_macros.h"

//* =============== ADDITION ARITHMETIC ENGINE =============== *//
void __BIGINT_ADD_WC__(bigInt *res, const bigInt *a, const bigInt *b) {
    if (res != a && res != b) { res->n = 0; res->sign = 1; }
    size_t max = max(a->n, b->n); uint8_t carry = 0;
    for (size_t i = 0; i < max; ++i) {
        limb_t x = (i < a->n) ? a->limbs[i] : 0; // Assigning limb at position i of a to x
        limb_t y = (i < b->n) ? b->limbs[i] : 0; // Assigning limb at position i of b to x
        res->limbs[i] = __ADD_UI64__(x, y, &carry); // Do single-limb addition with carry (if have)
    } res->n = max; /**/ if (carry) res->limbs[res->n++] = carry;
}
void __BIGINT_ADD_SHIFT__(bigInt *dst, const bigInt *src, size_t limb_shift) {
    if (!limb_shift) { __BIGINT_ADD_WC__(dst, dst, src); return; }
    if (!src->n) return; /**/ uint8_t carry = 0; size_t old_n = dst->n;
    if (dst->n < limb_shift) { // Fill in gaps if add src beyound dst->n
        memset(dst->limbs + dst->n, 0, (limb_shift - dst->n) * U64_BYTES);
        dst->n = limb_shift;
    }
    // Actual addition with src simulated to be limb shifted
    for (size_t i = 0; i < src->n; ++i) {
        size_t di = i + limb_shift;
        limb_t dst_base = (di < old_n) ? dst->limbs[di] : 0;
        dst->limbs[di] = __ADD_UI64__(dst_base, src->limbs[i], &carry);
    }
    // Propagate carry upward through dst's existing limbs
    size_t di = limb_shift + src->n;
    while (carry && di < dst->cap) { dst->limbs[di] = __ADD_UI64__(dst->limbs[di], 0, &carry); di++;}
    if (di > dst->n) dst->n = di;
}
void __BIGINT_ADD_SAW__(bigInt *res, const bigInt *x, const bigInt *y) {
    // Main operation
    if (res != x && res != y) { res->n = 0; res->sign = 1; }
    if (!y->n) return;
    else if (!x->n) __BIGINT_INTERNAL_COPY__(res, y);
    else if (x->sign == y->sign) {
        __BIGINT_ADD_WC__(res, x, y);
        res->sign = x->sign;
    } else {
        int8_t comp_res = __BIGINT_INTERNAL_COMP__(x, y);
        if (!comp_res) __BIGINT_INTERNAL_ZSET__(res);
        else if (comp_res > 0) { __BIGINT_SUB_WB__(res, x, y); res->sign = x->sign; }
        else { __BIGINT_SUB_WB__(res, y, x); res->sign = y->sign; }
    }
}

//* =============== SUBTRACTION ARITHMETIC ENGINE =============== *//
void __BIGINT_SUB_WB__(bigInt *res, const bigInt *a, const bigInt *b) {
    if (res != a && res != b) { res->n = 0; res->sign = 1; }
    uint8_t borrow = 0; size_t top_nonzero = 0;
    for (size_t i = 0; i < a->n; ++i) {
        limb_t y = (i < b->n) ? b->limbs[i] : 0;
        res->limbs[i] = __SUB_UI64__(a->limbs[i], y, &borrow);
        // Do single-limb subtraction with borrow ---> Stores the borrow
        if (res->limbs[i]) top_nonzero = i + 1;
    } res->n = top_nonzero;
}
void __BIGINT_SUB_SAW__(bigInt *res, const bigInt *x, const bigInt *y) {
    // Main Operation
    if (res != x && res != y) { res->n = 0; res->sign = 1; }
    if (!y->n) return;
    else if (!x->n) { __BIGINT_INTERNAL_COPY__(res, y); res->sign = -y->sign; }
    else if (x->sign == y->sign) {
        int8_t comp_res = __BIGINT_INTERNAL_COMP__(x, y);
        if (!comp_res) __BIGINT_INTERNAL_ZSET__(res);
        else {
            if (comp_res > 0) { __BIGINT_SUB_WB__(res, x, y); res->sign = x->sign; }
            else { __BIGINT_SUB_WB__(res, y, x); res->sign = -x->sign; }
        }
    } else { __BIGINT_ADD_WC__(res, x, y); res->sign = x->sign; }
}


//* =============== ADDITION + SUBTRACTION CONSTANT ENGINE =============== *//
dnml_status __CRINT_ADD_WC__(crint *res, crint *a, crint *b) {
    // Main Algorithms
    if (res != a && res != b) { res->n = 0; res->sign = 1; }
    size_t max = crtmax(a->n, b->n); uint8_t carry = 0;
    for (size_t i = 0; _lib_crt_lt(i, max); ++i) {
        size_t a_idx = 0, b_idx = 0;
        CHOOSE_OPTION(a_idx, (_lib_crt_lt(i, a->cap)), i, 0);
        CHOOSE_OPTION(b_idx, (_lib_crt_lt(i, b->cap)), i, 0);
        limb_t a_curr = a->limbs[a_idx], b_curr = b->limbs[b_idx];
        limb_t x = 0, y = 0;
        // limb_t x = (i < a->n) ? a->limbs[i] : 0;
        // limb_t y = (i < b->n) ? b->limbs[i] : 0;
        CHOOSE_OPTION(x, (_lib_crt_lt(i, a->n)), a_curr, 0);
        CHOOSE_OPTION(y, (_lib_crt_lt(i, b->n)), b_curr, 0);
        res->limbs[i] = __CRT_ADD_U64__(x, y, &carry);
        a_idx = 0; b_idx = 0; a_curr = 0; b_curr = 0;
    } res->limbs[max] = carry; res->n = max + (!!carry);
    __libdnml_smemset_u64(res->limbs, 0, res->cap, res->n, res->cap - 1, false); // clang-format off
    max = 0; carry = 0; res = 0; a = 0; b = 0; return CRINT_SUCCESS;
}
dnml_status __CRINT_SUB_WC__(crint *res, crint *a, crint *b) {
    // Main Algorithms
    if (res != a && res != b) { res->n = 0; res->sign = 1; }
    uint8_t borrow = 0;
    for (size_t i = 0; _lib_crt_lt(i, a->n); ++i) {
        size_t b_idx = 0; CHOOSE_OPTION(b_idx, (_lib_crt_lt(i, b->cap)), i, 0);
        limb_t curr = b->limbs[b_idx]; limb_t y = 0;
        // limb_t y = (i < b->n) ? b->limbs[i] : 0;
        CHOOSE_OPTION(y, (_lib_crt_lt(i, b->n)), curr, 0);
        res->limbs[i] = __CRT_SUB_U64__(a->limbs[i], y, &borrow);
        b_idx = 0; curr = 0;
    } res->n = a->n; __CRINT_TRIM_LZ__(res);
    __libdnml_smemset_u64(res->limbs, 0, res->cap, res->n, res->cap - 1, false); // clang-format off
    borrow = 0; res = 0; a = 0; b = 0; return CRINT_SUCCESS; // clang-format on
}
