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


/* ------ MAIN ALGORITHMS HELPERS ------ */
static void __heap_burk_3b2(
    PCONST_BIGINT a1, PCONST_BIGINT a2, PCONST_BIGINT a3,
    PCONST_BIGINT b1, PCONST_BIGINT b2, PCONST_BIGINT B,
    bigInt *q, P_BIGINT r, dnml_status *err
) {
    dnml_status echeck = BIGINT_SUCCESS, rec_err = BIGINT_SUCCESS;
    bigInt *alloc_list[1], *early_free[3]; uint8_t early_cnt = 0, alloc_cnt = 0;
    bigInt iq = {0}, c = {0}; // These will be allocated at the base case
    early_free[early_cnt++] = &iq; // Setting up iq for potential early exit clearance (iq.cap = a1->n + a2->n)
    early_free[early_cnt++] = &c; // Setting up c for potential early exit clearance (c.cap = B->n)
    __BIHEAP_BURNIKEL__(a1, a2, b1, &iq, &c, &rec_err); HEAP_OOM(rec_err, err, early_free, early_cnt,);

    BIHEAP_TEMP(d, (iq.n + b2->n), echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    uint64_t a[1] = {1}; bigInt one = {.limbs = a, .sign = 1, .n = 1, .cap = 1};
    __BIHEAP_MUL_DISP__(&iq, b2, &d, &echeck); HEAP_OOM(rec_err, err, early_free, early_cnt,);
    __BIGINT_ADD_WC__(&c, &c, a3);
    while (__BIGINT_INTERNAL_COMP__(&c, &d) == -1) {
        __BIGINT_SUB_WB__(&iq, &iq, &one); __BIGINT_ADD_WC__(&c, &c, B);
    } __BIGINT_SUB_WB__(&c, &c, &d); __BIGINT_INTERNAL_MOVE__(q, &iq);
    __BIGINT_INTERNAL_MOVE__(r, &c); *err = BIGINT_SUCCESS;
}   

/* --------- ALGORITHM FUNCTIONS ---------  */
void __BIHEAP_SHORT_DIVISION__(PCONST_BIGINT a, uint64_t b, P_BIGINT quot, P_BIGINT rem) {
    uint64_t remainder = 0; uint8_t overflow_check;
    for (size_t i = a->n; i > 0; --i) {
        quot->limbs[i - 1] = __DIV_HELPER_UI64__(remainder, a->limbs[i - 1], b, &remainder, &overflow_check);
        DNML_TEST_ASSERT(overflow_check, "CRITICIAL DEBUG ERROR: Division quotient's overflowed", {});
    }
    __BIGINT_INTERNAL_TRIM_LZ__(rem);
    if (quot->n == 0) quot->sign = 1;
    rem->limbs[0] = remainder;
    rem->n = (remainder) ? 1 : 0;
    rem->sign = 1;
}
void __BIHEAP_KNUTH_D__(PCONST_BIGINT a, PCONST_BIGINT b, P_BIGINT quot, P_BIGINT rem, dnml_status *err) {
    /* ---- Setup ---- */ dnml_status echeck;
    uint8_t shift = __CLZ_UI64__(b->limbs[b->n - 1]);
    size_t m = a->n, n = b->n; bigInt *alloc_list[2], *early_free[2]; uint8_t early_cnt = 0, alloc_cnt = 0;
    BIHEAP_TEMP(a_copy, m + 1, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    BIHEAP_TEMP(b_copy, n, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);

    /* 1. Normalization */
    uint64_t carry = 0;
    for (size_t i = 0; i < m; ++i) {
        uint64_t x = a->limbs[i];
        a_copy.limbs[i] = (x << shift) | carry;
        carry = (shift ? x >> (U64_BITS - shift) : 0);
    } 
    a_copy.limbs[m] = carry;
    a_copy.n = m + 1; /**/ carry = 0;
    for (size_t i = 0; i < n; ++i) {
        uint64_t x = b->limbs[i];
        b_copy.limbs[i] = (x << shift) | carry;
        carry = (shift ? x >> (U64_BITS - shift) : 0);
    } b_copy.n = n; /**/ quot->n = m - n + 1;

    /* 3-5. Main Loop */
    for (size_t j = m - n + 1; j > 0; --j) {
        /* 3. Estimation */
        uint64_t a2 = a_copy.limbs[j + n];                      // 1st highest limb of a
        uint64_t a1 = a_copy.limbs[j + n - 1];                  // 2nd highest limb of a
        uint64_t a0 = (n >= 2) ? a_copy.limbs[j + n - 2] : 0;   // 3rd highest limb of a (DETECT OVERESTIMATION)
        uint64_t b1 = b_copy.limbs[n - 1];                      // 1st highest limb of b
        uint64_t b0 = (n >= 2) ? b_copy.limbs[n - 2] : 0;       // 2nd highest limb of b (used to validate quotient estimation - DETECT OVERESTIMATION)
        uint64_t qhat, rhat; uint8_t overflow_check;
        qhat = __DIV_HELPER_UI64__(a2, a1, b1, &rhat, &overflow_check);
        if (overflow_check) { _free_alloc_list(early_free, early_cnt); *err = DARENA_OVERFLOW; return; }

        // Validating quotient estimation (Prevent overestimation before multi-limb subtraction (expensive & risky))
        if (qhat == UINT64_MAX) --qhat; // Check if estimates quotient is too large
        while (qhat * b0 > ((uint128)rhat << U64_BITS) + a0) {
            --qhat; /**/ rhat += b1;
            if (rhat < b1) break; // At most 2 decrements (Knuth approved)
        }

        /* 4. Multiply-subtract */
        uint64_t borrow = 0;
        for (size_t i = 0; i < n; ++i) { uint64_t low, high;
            low = __MUL_UI64__(qhat, b_copy.limbs[i], &high); /* Multi-limb multiplication */
            uint64_t x = a_copy.limbs[j + 1]; /**/ uint64_t t = x - low - borrow;
            borrow = (t > x) + high; /**/ a_copy.limbs[j + i] = t;
        } uint64_t x = a_copy.limbs[j + n]; /**/ a_copy.limbs[j + n] = x - borrow;

        /* 5. Correction */
        if (x < borrow) {
            --qhat; /* if x underflows -> qhat was still too large -> Decrement */
            uint64_t carry2 = 0;
            for (size_t i = 0; i < n; ++i) {
                uint64_t t = a_copy.limbs[j + i] + b_copy.limbs[i] + carry2;
                carry2 = (t < a_copy.limbs[j + i]);
                a_copy.limbs[j + i] = t;
            } a_copy.limbs[j + n] += carry2; // Handles remaining carry
        }
        quot->limbs[j] = qhat; // Add estimated quotient of: a's 2 limbs (!28 bit) / b's 1 limb (64 bit)
    }

    /* 6. Denormalize */ carry = 0;
    for (size_t i = n; i > 0; --i) {
        uint64_t x = a_copy.limbs[i];
        rem->limbs[i] = (x >> shift) | carry;
        carry = (shift ? x << (U64_BITS - shift) : 0);
    }
    rem->n = n;
    __BIGINT_INTERNAL_TRIM_LZ__(quot); /**/ __BIGINT_INTERNAL_TRIM_LZ__(rem);
    if (quot->n == 0) quot->sign = 1; /**/ if (rem->n == 0) rem->sign = 1;
    _free_alloc_list(alloc_list, alloc_cnt); *err = BIGINT_SUCCESS; // Free all temporaries
}
void __BIHEAP_BURNIKEL__(
    PCONST_BIGINT AH, PCONST_BIGINT AL,
    PCONST_BIGINT b, P_BIGINT quot, P_BIGINT rem, dnml_status *err
) {
    if (b->n <= BIGINT_SHORT) { dnml_status echeck = BIGINT_SUCCESS; 
        // Setting up the full dividend a from AH & AL
        bigInt a; echeck = __BIGINT_INTERNAL_LINIT__(&a, AH->n + AL->n);
        if (echeck == DNML_ALLOC_OOM) { *err = DNML_ALLOC_OOM; return; }
        memcpy(a.limbs, AL->limbs, AL->n * U64_BYTES); memcpy(a.limbs + AH->n, AH->limbs, AH->n * U64_BYTES);
        // Allocating at the base the buffer for quot and rem to propagate upwards
        echeck = __BIGINT_INTERNAL_LINIT__(quot, AH->n + AL->n);
        if (echeck == DNML_ALLOC_OOM) { __BIGINT_INTERNAL_FREE__(&a); *err = DNML_ALLOC_OOM; return; }
        echeck = __BIGINT_INTERNAL_LINIT__(rem, 1); // This is the because utilizing 64-bit divisor for short division
        if (echeck == DNML_ALLOC_OOM) { 
            __BIGINT_INTERNAL_FREE__(quot); 
            __BIGINT_INTERNAL_FREE__(&a); 
            *err = DNML_ALLOC_OOM; return; 
        } 
        __BIHEAP_SHORT_DIVISION__(&a, b->limbs[0], quot, rem); 
        __BIGINT_INTERNAL_FREE__(&a); *err = BIGINT_SUCCESS;
    }
    //* -------- 1. SPLIT ---------- *//
    size_t k = (size_t)(b->n >> 1) + 1;
    /* Dividend - A - QUARTERS */
    bigInt a4 = {.limbs = AL->limbs,        .sign = 1,  /**/    .n = k,         .cap = k};
    bigInt a3 = {.limbs = AL->limbs + k,    .sign = 1,  /**/    .n = AL->n - k, .cap = AL->n - k};
    bigInt a2 = {.limbs = AH->limbs,        .sign = 1,  /**/    .n = k,         .cap = k};
    bigInt a1 = {.limbs = AH->limbs + k,    .sign = 1,  /**/    .n = AH->n - k, .cap = AH->n - k};
    /* Divisors - B - HALVES */
    bigInt b2 = {.limbs = b->limbs,     .sign = 1,  /**/    .n = k,        .cap = k};
    bigInt b1 = {.limbs = b->limbs + k, .sign = 1,  /**/    .n = b->n - k, .cap = b->n - k};

    //* --------- 2. ACTUAL OPERATION --------- *//
    dnml_status echeck = BIGINT_SUCCESS, rec_err = BIGINT_SUCCESS;
    bigInt *alloc_list[1], *early_free[3]; uint8_t early_cnt = 0, alloc_cnt = 0;
    bigInt q1 = {0}, q2 = {0}, r = {0}; // These will be allocated at the base case
    alloc_list[alloc_cnt++] = &q1; early_free[early_cnt++] = &q1; // Setting up for cleanup (q1.cap = 2k)
    early_free[early_cnt++] = &q2; // Will be returned/swap with quot, Setup for potential early clerance (q2.cap = 2k)
    early_free[early_cnt++] = &r; // Will be returned/swap with rem, Setup for potential early clerance (r.cap = 2k)
    __heap_burk_3b2(
        &a1, &a2, &a3,  // Dividends
        &b1, &b2, b,    // Divisors
        &q1, &r,  /* Quotient + Remainders */ &rec_err
    ); HEAP_OOM(rec_err, err, early_free, early_cnt,);
    bigInt r1 = {.limbs = r.limbs,      .sign = 1, .n = k,       .cap = k};
    bigInt r2 = {.limbs = r.limbs + k,  .sign = 1, .n = r.n - k, .cap = r.n - k};
    __BIGINT_INTERNAL_ENSCAP__(&r, k << 1); // Ensures r has enough capacity to be the remainder (2k limbs)
    __heap_burk_3b2(
        &r1, &r2, &a4,  // Dividends
        &b1, &b2, b,    // Divisors
        &q2, &r,  /* Quotient + Remainders*/ &rec_err
    ); HEAP_OOM(rec_err, err, early_free, early_cnt,);

    //* ---------- 3. RECOMPOSITION ---------- *//
    __BIGINT_INTERNAL_MOVE__(quot, &q2); __BIGINT_INTERNAL_ENSCAP__(quot, quot->cap + q1.cap);
    memcpy(quot->limbs + quot->cap, q1.limbs, q1.cap * U64_BYTES); quot->n = k << 1; 
    __BIGINT_INTERNAL_TRIM_LZ__(quot); __BIGINT_INTERNAL_MOVE__(rem, &r);
    _free_alloc_list(alloc_list, alloc_cnt); *err = BIGINT_SUCCESS;
}
void __BIHEAP_NEWTON__(PCONST_BIGINT a, PCONST_BIGINT b, P_BIGINT quot, P_BIGINT rem, dnml_status *err) {}
void __BIHEAP_DIV_DISP__(PCONST_BIGINT a, PCONST_BIGINT b, P_BIGINT quot, P_BIGINT tmp_rem, dnml_status *err) {
    if (b->n < BIGINT_SHORT) {  __BIHEAP_SHORT_DIVISION__(a, b->limbs[0], quot, tmp_rem); *err = BIGINT_SUCCESS; }
    else if (b->n < BIGINT_KNUTH) __BIHEAP_KNUTH_D__(a, b, quot, tmp_rem, err);
    else if (b->n < BIGINT_BURNIKEL) { 
        size_t k = (size_t)(b->n >> 1) + 1;
        bigInt AL = { .limbs = a->limbs, .sign = a->sign, .n = max(a->n, 2*k), .cap = max(a->n, 2*k)};
        bigInt AH = {
            .limbs = a->limbs + max(a->n, 2*k),
            .sign = a->sign,
            .n = (a->n < 2*k) ? 0 : 2*k - a->n,
            .cap = (a->n < 2*k) ? 0 : 2*k - a->n
        }; __BIHEAP_BURNIKEL__(&AH, &AL, b, quot, tmp_rem, err);
    } else __BIHEAP_NEWTON__(a, b, quot, tmp_rem, err);
}
