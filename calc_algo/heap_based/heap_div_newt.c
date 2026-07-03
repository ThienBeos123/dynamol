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



#include "heap_div.h"
#include <debug_util.h>
#include "../../util/aconv_macros.h"
/** ------------------- Heap-based BigInt Division -------------------
 * THIS FILE CONTAINS THE FOLLOWING ALGORITHMS:
 *
 *      - Newton-Raphson scaled-reciprocal (General)
 *
 * This file is the complexity-delegated file, specifically containing the implementation
 * of the algorithm as detailed above. Other bigInt division files include:
 *
 *      - heap_div.c:
 *          + Algorithm Dispatcher
 *          + Short-division Implementation
 *          + Knuth Algorithm D Implementation
 *          + Burnikel-Zielger Division Implementation
 */
/* ------ WORKSPACE + HELPER FUNCTIONS ------ */
size_t __BIGINT_NEWTON_WS__(size_t n_size, size_t d_size) {
    size_t k = (d_size << 6); // The maximum, not subtracting any bits from the top
    size_t rsize = n_size; /**/ size_t est_total = ((k << 2) + 3) >> 6; // 4k + 3 bits
    size_t tres_size = n_size + d_size; // The reciprocal R maximum size is theoretically D_Size
    return rsize + est_total + tres_size;
}
static void __2KP1_MAGSUB(bigInt *const x, size_t exp_k) { // 2^(k + 1) - x
    uint8_t borrow = 0; size_t top_nonzero = 0;
    size_t klimb_cnt = (exp_k + 1) >> 6; // Division by 64
    limb_t last_limb = UINT64_C(1) << ((exp_k + 1) & 63);
    for (size_t i = 0; i < klimb_cnt; ++i) {
        limb_t y = (i < x->n) ? x->limbs[i] : 0;
        limb_t z = (i == klimb_cnt - 1) ? last_limb : 0;
        x->limbs[i] = __SUB_UI64__(z, y, &borrow);
        if (!(x->limbs[i])) top_nonzero = i;
    } x->n = top_nonzero;
}
static void __MAGSUB_2KP1(bigInt *const x, size_t exp_k) { // x - 2^(k + 1)
    uint8_t borrow = 0; size_t top_nonzero = 0;
    size_t klimb_cnt = (exp_k + 1) >> 6; // Division by 64
    limb_t last_limb = UINT64_C(1) << ((exp_k + 1) & 63);
    for (size_t i = 0; i < klimb_cnt; ++i) {
        limb_t a = (i < x->n) ? x->limbs[i] : 0;
        limb_t b = (i == klimb_cnt - 1) ? last_limb : 0;
        x->limbs[i] = __SUB_UI64__(a, b, &borrow);
        if (!(x->limbs[i])) top_nonzero = i;
    } x->n = top_nonzero;
}
static void __MAGADD_2KP1(bigInt *const x, size_t exp_k) { // x + 2^(k + 1)
    size_t klimb_cnt = (exp_k + 1) >> 6; // Division by 64
    size_t max = max(x->n, klimb_cnt); uint8_t carry = 0;
    limb_t last_limb = UINT64_C(1) << ((exp_k + 1) & 63);
    for (size_t i = 0; i< max; ++i) {
        limb_t a = (i == klimb_cnt - 1) ? last_limb : 0;
        limb_t b = (i < x->n) ? x->limbs[i] : 0;
        x->limbs[i] = __ADD_UI64__(a, b, &carry);
    }
    x->n = max; /**/ if (carry) x->limbs[x->n++] = carry;
}
static int8_t __MAGCMP_2KP1(const bigInt *const x, size_t exp_k) {
    size_t klimb_cnt = (exp_k + 1) >> 6;
    limb_t last_limb = UINT64_C(1) << ((exp_k + 1) & 63);
    if (x->n != klimb_cnt) return (x->n > klimb_cnt) ? 1 : -1;
    for (size_t i = x->n; i > 0; --i) {
        limb_t curr_limb = (i == klimb_cnt - 1) ? last_limb : 0;
        if (x->limbs[i - 1] != curr_limb) return (x->limbs[i - 1] > curr_limb) ? 1 : -1;
    } return 0;
}
static void __sub_2kp1(bigInt *const x, size_t exp_k) { // 2^(k + 1) - x (SIGN-AWARED)
    if (!x->n) { memset(x->limbs, 0, (((exp_k + 1) >> 6) - 1) * U64_BYTES); }
    if (x->sign < 0) { __MAGADD_2KP1(x, exp_k); x->sign = 1; }
    else {
        int8_t comp_res = __MAGCMP_2KP1(x, exp_k);
        if (!comp_res) __BIGINT_INTERNAL_ZSET__(x);
        else {
            if (comp_res > 0) { __MAGSUB_2KP1(x, exp_k); x->sign = -1; }
            else { __2KP1_MAGSUB(x, exp_k); x->sign = 1; }
        }
    }
}




/* --------- MAIN FUNCTION - QUOTIENT-BIASED --------- */
void __BIHEAP_NEWTON__(PCONST_BIGINT n, PCONST_BIGINT d, P_BIGINT quot, P_BIGINT rem, dnml_status *err) {
    // 1. Bootstrapping and Setting up metadatas
    limb_t dtop  = d->limbs[d->n - 1]; // The MSL (Most Significant Limb) of d
    size_t k = (d->n << 6) - __CLZ_UI64__(dtop); // Bit length of d
    limb_t rinit = (UINT64_MAX) / dtop; // Initial guess of a single machine word

    // 2. Converging correction of Fixed/Scaled Reciprocal of D
    //  - Our goal here is to compute 2^k / D using Newton's root-approximation method
    dnml_status echeck = BIGINT_SUCCESS;
    bigInt *alloc_list[1]; bigInt *early_free[3];
    uint8_t alloc_cnt = 0; uint8_t early_cnt = 0; 
    BIHEAP_RET(r, n->n, echeck, err, early_free, early_cnt,); r.cap = d->n;
    BIHEAP_TEMP(dr, ((k << 1) + 1) >> 6, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    BIHEAP_RET(rprod, max(((k + 1) << 1) >> 6, n->n + d->n), echeck, err, early_free, early_cnt,);
    size_t correct_bits = U64_BITS; limb_t carry_in = 0;
    while (correct_bits < k) {
        // Full estimation equation: R{i+1} = R{i} * (2^(k+1) - DR{i}) >> k
        __BIHEAP_MUL_DISP__(&r, d, &dr, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
        __sub_2kp1(&dr, k); __BIHEAP_MUL_DISP__(&dr, &r, &rprod, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
        __BIGINT_INTERNAL_RLSHIFT__(&rprod, (k >> 6)); size_t bshift = k & 63;
        if (!k) { __BIGINT_INTERNAL_COPY__(&r, &rprod); continue; }
        for (size_t i = rprod.n; i > 0; --i) { // Copy + Left shift fused together
            r.limbs[i] = (rprod.limbs[i - 1] >> bshift) | (carry_in << (U64_BITS - bshift));
            carry_in = rprod.limbs[i - 1] & ((UINT64_C(1) << bshift) - 1);
        }
    }

    /** 3. Retrieving the quotient after correctly estimating the reciprocal of D
     * In this step, we're basically doing floor(NR / 2^k), which is mathematically
     * equivalent to N/D due to:
     *
     *          |         |     |                 |     |       |
     *          |  N * R  | =   |   N       2^k   | =   |   N   | (QUOTIENT)
     *          | ------- |     | ----- * ------- |     | ----- |
     *          |_  2^k  _|     |_ 2^k       D   _|     |_  D  _|
     */
    rprod.cap = n->n + d->n; // We will reuse rprod in place for tmp_res
    __BIHEAP_MUL_DISP__(n, &r, &rprod, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    __BIGINT_INTERNAL_RLSHIFT__(&rprod, (k >> 6)); __BIGINT_INTERNAL_RSHIFT__(&rprod, (k & 63));
    __BIGINT_INTERNAL_TRIM_LZ__(&rprod); // Our final quotient!
    // 4. Calculating the remainder (Reusing r, since remainder is also bounded by d->n)
    if (!__BIGINT_INTERNAL_COMP__(&rprod, n)) {
        // Shrinking to save memory + 2 slack
        __BIGINT_INTERNAL_SHRINK__(&r, 2); HEAP_OOM(echeck, err, early_free, early_cnt,);
        __BIGINT_INTERNAL_SHRINK__(&rprod, rprod.n + 2); HEAP_OOM(echeck, err, early_free, early_cnt,);
        r.n = 0; r.limbs[0] = 0; r.limbs[1] = 0; r.sign = 2;
    } else {
        r.cap = n->n; // Reusing r at a capacity of n->n due to D * Quot <= N
        __BIHEAP_MUL_DISP__(d, &rprod, &r, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
        __BIGINT_SUB_WB__(&r, n, &r); // Formula: (Remainder = Dividend - (Divsor * Quotient))
        __BIGINT_INTERNAL_SHRINK__(&r, r.n + 2); HEAP_OOM(echeck, err, early_free, early_cnt,);
        __BIGINT_INTERNAL_SHRINK__(&rprod, rprod.n); HEAP_OOM(echeck, err, early_free, early_cnt,);
    } // This function is quotient-biased due to if quot and rem were alias of one another, then we
    // would copy the final remainder into rem first, and then overwriting with the quotient result
    __BIGINT_INTERNAL_MOVE__(rem, &r); __BIGINT_INTERNAL_MOVE__(quot, &rprod); *err = BIGINT_SUCCESS;
}




/* ------------ MAIN FUNCTION - REMAINDER-BIASED ------------ */
void __RBIHEAP_NEWTON__(PCONST_BIGINT n, PCONST_BIGINT d, P_BIGINT rem, dnml_status *err) {
    // 1. Bootstrapping and Setting up metadatas
    limb_t dtop  = d->limbs[d->n - 1]; // The MSL (Most Significant Limb) of d
    size_t k = (d->n << 6) - __CLZ_UI64__(dtop); // Bit length of d
    limb_t rinit = (UINT64_MAX) / dtop; // Initial guess of a single machine word

    // 2. Converging correction of Fixed/Scaled Reciprocal of D
    //  - Our goal here is to compute 2^k / D using Newton's root-approximation method
    dnml_status echeck = BIGINT_SUCCESS;
    bigInt *alloc_list[1]; bigInt *early_free[3];
    uint8_t alloc_cnt = 0; uint8_t early_cnt = 0; 
    BIHEAP_RET(r, n->n, echeck, err, early_free, early_cnt,); r.cap = d->n;
    BIHEAP_TEMP(dr, ((k << 1) + 1) >> 6, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    BIHEAP_TEMP(rprod, max(((k + 1) << 1) >> 6, n->n + d->n), echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    rprod.cap = ((k + 1) << 1) >> 6; /**/ size_t correct_bits = U64_BITS; limb_t carry_in = 0;
    while (correct_bits < k) {
        // Full estimation equation: R{i+1} = R{i} * (2^(k+1) - DR{i}) >> k
        __BIHEAP_MUL_DISP__(&r, d, &dr, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
        __sub_2kp1(&dr, k); __BIHEAP_MUL_DISP__(&dr, &r, &rprod, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
        __BIGINT_INTERNAL_RLSHIFT__(&rprod, (k >> 6)); size_t bshift = k & 63;
        if (!k) { __BIGINT_INTERNAL_COPY__(&r, &rprod); continue; }
        for (size_t i = rprod.n; i > 0; --i) { // Copy + Left shift fused together
            r.limbs[i] = (rprod.limbs[i - 1] >> bshift) | (carry_in << (U64_BITS - bshift));
            carry_in = rprod.limbs[i - 1] & ((UINT64_C(1) << bshift) - 1);
        }
    }

    /** 3. Retrieving the quotient after correctly estimating the reciprocal of D
     * In this step, we're basically doing floor(NR / 2^k), which is mathematically
     * equivalent to N/D due to:
     *
     *          |         |     |                 |     |       |
     *          |  N * R  | =   |   N       2^k   | =   |   N   | (QUOTIENT)
     *          | ------- |     | ----- * ------- |     | ----- |
     *          |_  2^k  _|     |_ 2^k       D   _|     |_  D  _|
     */
    rprod.cap = n->n + d->n; // We will reuse rprod in place for tmp_res
    __BIHEAP_MUL_DISP__(n, &r, &rprod, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    __BIGINT_INTERNAL_RLSHIFT__(&rprod, (k >> 6)); __BIGINT_INTERNAL_RSHIFT__(&rprod, (k & 63));
    __BIGINT_INTERNAL_TRIM_LZ__(&rprod); // Our final quotient!
    // 4. Calculating the remainder (Reusing r, since remainder is also bounded by d->n)
    r.cap = n->n; // Reusing r at a capacity of n->n due to D * Quot <= N
    __BIHEAP_MUL_DISP__(d, &rprod, &r, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    __BIGINT_SUB_WB__(&r, n, &r); // Formula: (Remainder = Dividend - (Divsor * Quotient))
    __BIGINT_INTERNAL_SHRINK__(&r, r.n + 2); HEAP_OOM(echeck, err, early_free, early_cnt,);
    __BIGINT_INTERNAL_MOVE__(rem, &r); _free_alloc_list(alloc_list, alloc_cnt); *err = BIGINT_SUCCESS;
}
