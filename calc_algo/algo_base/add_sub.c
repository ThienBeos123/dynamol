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

//* =============== ADDITION ARITHMETIC ENGINE =============== *//
void __BIGINT_ADD_WC__(bigInt *res, const bigInt *a, const bigInt *b) {
    DNML_TEST_ASSERT(
        (res->cap >= max(a->n, b->n) + 1),
        "Insufficient Sum Buffer: Capacity Unsatisfactory for a + b"
        " (-Eadd_insufficient_cap)", {}
    ); size_t max = max(a->n, b->n); uint64_t carry = 0;
    for (size_t i = 0; i < max; ++i) {
        uint64_t x = (i < a->n) ? a->limbs[i] : 0; // Assigning limb at position i of a to x
        uint64_t y = (i < b->n) ? b->limbs[i] : 0; // Assigning limb at position i of b to x
        uint8_t u8_carry = (uint8_t)carry;
        res->limbs[i] = __ADD_UI64__(x, y, &u8_carry); // Do single-limb addition with carry (if have)
        // Stores the carry
    }
    if (carry) res->limbs[max] = carry; // If there is still a carry
    res->n = max + (carry != 0);
}
void __BIGINT_ADD_SAW__(bigInt *res, const bigInt *x, const bigInt *y) {
    // Static analysis
    if ((x->sign == 1 && y->sign == 1) ||
        (x->sign == -1 && y->sign == -1)
    ) DNML_TEST_ASSERT(
        (res->cap >= max(x->n, y->n) + 1),
        (x->sign == 1 && y->sign == 1) ?
        "Insufficient Sum Buffer:"
        " Capacity Unsatisfactory for a + b"
        " (-Eadd_saw_insufficient_cap)" :
        "Insufficient Sum Buffer:"
        " Capacity Unsatisfactory for (-a) + (-b)"
        " (-Eadd_saw_insufficient_cap)", {}
    );
    // Main operation
    if (!y->n) return;
    else if (!x->n) __BIGINT_INTERNAL_COPY__(res, y);
    else if (x->sign == y->sign) {
        __BIGINT_ADD_WC__(res, x, y);
        res->sign = x->sign;
    } else {
        int8_t comp_res = __BIGINT_INTERNAL_COMP__(x, y);
        if (!comp_res) __BIGINT_INTERNAL_ZSET__(res);
        else {
            if (comp_res > 0) { __BIGINT_SUB_WB__(res, x, y); res->sign = x->sign; }
            else { __BIGINT_SUB_WB__(res, x, y); res->sign = y->sign; }
        }
    }
}

//* =============== SUBTRACTION ARITHMETIC ENGINE =============== *//
void __BIGINT_SUB_WB__(bigInt *res, const bigInt *a, const bigInt *b) {
    DNML_TEST_ASSERT(
        (__BIGINT_INTERNAL_COMP__(a, b) != -1),
        "Subtraction Underflow: Subtrahend's magnitude is too large for Minuend"
        " (-Esub_underflow)", {}
    ); uint64_t borrow = 0;
    for (size_t i = 0; i < a->n; ++i) {
        uint64_t y = (i < b->n) ? b->limbs[i] : 0;
        uint8_t u8_borrow = (uint64_t)borrow;
        res->limbs[i] = __SUB_UI64__(a->limbs[i], y, &u8_borrow);
        // Do single-limb subtraction with borrow ---> Stores the borrow
    } res->n = a->n;
}
void __BIGINT_SUB_SAW__(bigInt *res, const bigInt *x, const bigInt *y) {
    // Static Analysis
    if ((x->sign == 1 && y->sign == -1) ||
        (x->sign == -1 && y->sign == 1)
    ) DNML_TEST_ASSERT(
        (res->cap >= max(x->n, y->n) + 1),
        (x->sign == 1 && y->sign == -1) ?
        "Insufficient Difference Buffer:"
        " Capacity Unsatisfactory for a - (-b)"
        " (-Esub_saw_insufficient_cap)" :
        "Insufficient Difference Buffer:"
        " Capacity Unsatisfactory for (-a) - b"
        " (-Esub_saw_insufficient_cap)", {}
    );
    // Main Operation
    if (!y->n) return;
    else if (!x->n) { __BIGINT_INTERNAL_COPY__(res, y);  res->sign = -y->sign; }
    else if (x->sign == y->sign) {
        int8_t comp_res = __BIGINT_INTERNAL_COMP__(x, y);
        if (!comp_res) __BIGINT_INTERNAL_ZSET__(res);
        else {
            if (comp_res > 0) { __BIGINT_SUB_WB__(res, x, y); res->sign = x->sign; }
            else { __BIGINT_SUB_WB__(res, x, y); res->sign = -x->sign; }
        }
    } else {
        __BIGINT_ADD_WC__(res, x, y);
        res->sign = x->sign;
    }
}


//* =============== ADDITION + SUBTRACTION CONSTANT ENGINE =============== *//
dnml_status __CRINT_ADD_WC__(crint *res, crint *a, crint *b) {
    // Static Analysis
    crint_poison(a); crint_poison(b);
    crint_poison(res); DNML_TEST_ASSERT(
        (res->cap >= crtmax(a->n, b->n) + 1),
        "Insufficient Sum Buffer: Capacity Unsatisfactory for a + b (-Eadd_insufficient_cap)",
        { __CRINT_IFREE__(res); __CRINT_IFREE__(a); __CRINT_IFREE__(b); }
    ); // Main Algorithms
    size_t max = crtmax(a->n, b->n); uint64_t carry = 0;
    for (size_t i = 0; _lib_crt_lt(i, max); ++i) {
        uint64_t a_curr = a->limbs[i], b_curr = b->limbs[i], x, y;
        // uint64_t x = (i < a->n) ? a->limbs[i] : 0;
        // uint64_t y = (i < b->n) ? b->limbs[i] : 0;
        CHOOSE_OPTION(x, (_lib_crt_lt(i, a->n)), a_curr, 0);
        CHOOSE_OPTION(y, (_lib_crt_lt(i, b->n)), b_curr, 0);
        uint8_t u8_carry = (uint8_t)carry;
        res->limbs[i] = __CRT_ADD_U64__(x, y, &u8_carry);
        a_curr = 0; b_curr = 0; u8_carry = 0;
    } res->limbs[max] = carry; res->n = max + (!!carry);
    __libdnml_smemset_u64(res->limbs, 0, res->cap, res->n, res->cap - 1, false);
    return CRINT_SUCCESS;
}
dnml_status __CRINT_SUB_WC__(crint *res, crint *a, crint *b) {
    crint_poison(a); crint_poison(b);
    crint_poison(res); DNML_TEST_ASSERT(
        (__CRINT_INTERNAL_CMP__(a, b) != -1),
        "Subtraction Underflow: Subtrahend's magnitude is too large for Minuend (-Esub_underflow)", 
        { __CRINT_IFREE__(res); __CRINT_IFREE__(a); __CRINT_IFREE__(b); }
    ); // Main Algorithms
    uint64_t borrow = 0;
    for (size_t i = 0; _lib_crt_lt(i, a->n); ++i) {
        uint64_t curr = b->limbs[i], y;
        // uint64_t y = (i < b->n) ? b->limbs[i] : 0;
        CHOOSE_OPTION(y, (_lib_crt_lt(i, b->n)), curr, 0);
        uint8_t u8_borrow = (uint8_t)borrow;
        res->limbs[i] = __CRT_SUB_U64__(a->limbs[i], y, &u8_borrow);
        curr = 0; u8_borrow = 0;
    } res->n = a->n; __CRINT_TRIM_LZ__(res);
    __libdnml_smemset_u64(res->limbs, 0, res->cap, res->n, res->cap - 1, false); // clang-format off
    borrow = 0; res = 0; a = 0; return CRINT_SUCCESS;
}
