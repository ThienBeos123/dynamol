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



#include "div.h"


//* NOTE: +) THE WORKSPACE SIZE FUNCTION IS A SAFE UPPERBOUND
//*       +) THE WORKSPACE SIZE FUNCTION DOES NOT COMPUTE EXACTLY THE
//*          CORRECT SIZE WITH CORRECT ALIGNMENT PADDINGS TAKEN INTO ACCOUNT

/* ------ WORKSPACE FUNCTIONS ------ */
size_t __BIGINT_SHORTDIV_WS__(size_t a_size, size_t b_size) { return 0; }
size_t __BIGINT_KNUTH_WS__(size_t a_size, size_t b_size) { return (a_size + 1 + b_size) + (a_size + b_size); }
size_t __BIGINT_BURNIKEL_WS__(size_t a_size, size_t b_size) {
    size_t k = (size_t)(b_size >> 1) + 1;
    // BURNIKEL FUNCTION
    size_t q1_q2_size = (k << 1) + (k << 2); // 2k + 4k = 6k
    size_t rsize = k << 1; // 2k
    // 3-BY-2 HELPER
    size_t csize = k << 1; // 2k
    size_t iq_size = k << 1; // 2k
    size_t dsize = (k << 1) + k; // 3k
    return 3*(q1_q2_size + rsize + csize + iq_size + dsize) + a_size;
    // a_size has been updated/halved from recursion.
}
size_t __BIGINT_NEWTON_WS__(size_t a_size, size_t b_size) { return 0; }
size_t __BIGINT_DIV_WS__(size_t a_size, size_t b_size) {
    if      (b_size < BIGINT_SHORT) return __BIGINT_SHORTDIV_WS__(a_size, b_size);
    else if (b_size < BIGINT_KNUTH) return __BIGINT_KNUTH_WS__(a_size, b_size);
    else return __BIGINT_NEWTON_WS__(a_size, b_size);
}


/* ------ MAIN ALGORITHMS HELPERS ------ */
static void __burk_3b2(
    PCONST_BIGINT a1, PCONST_BIGINT a2, PCONST_BIGINT a3,
    PCONST_BIGINT b1, PCONST_BIGINT b2, PCONST_BIGINT B,
    P_BIGINT q, P_BIGINT r, calc_ctx burk_helper_ctx, dnml_status *err
) {
    dnml_status echeck = BIGINT_SUCCESS, rec_err = BIGINT_SUCCESS;
    size_t burk_helper_mark = scratch_mark(&burk_helper_ctx);
    BIGINT_TEMP(c, B->n, burk_helper_ctx, burk_helper_mark, echeck, err,);
    BIGINT_TEMP(iq, a1->n + a2->n, burk_helper_ctx, burk_helper_mark, echeck, err,);
    __BIGINT_BURNIKEL__(a1, a2, b1, q, &c, burk_helper_ctx, &rec_err);
    SCRATCH_OVF(rec_err, burk_helper_ctx, burk_helper_mark, err,)

    BIGINT_TEMP(d, (iq.n + b2->n), burk_helper_ctx, burk_helper_mark, echeck, err,);
    uint64_t a[1] = {1}; bigInt one = {.limbs = a, .sign = 1, .n = 1, .cap = 1};
    __BIGINT_MUL_DISP__(&iq, b2, &d, burk_helper_ctx, &echeck);
    SCRATCH_OVF(rec_err, burk_helper_ctx, burk_helper_mark, err,)
    __BIGINT_ADD_WC__(&c, &c, a3);
    while (__BIGINT_INTERNAL_COMP__(&c, &d) == -1) {
        __BIGINT_SUB_WB__(&iq, &iq, &one); __BIGINT_ADD_WC__(&c, &c, B);
    } __BIGINT_SUB_WB__(&c, &c, &d); __BIGINT_INTERNAL_COPY__(q, &iq); __BIGINT_INTERNAL_COPY__(r, &c);
    scratch_rewind(&burk_helper_ctx, burk_helper_mark); *err = BIGINT_SUCCESS;
}


/* ------ ALGORITHMS FUNCTIONS - QUOTIENT BIASED ------ */
void __BIGINT_SHORT_DIVISION__(PCONST_BIGINT a, uint64_t b, P_BIGINT quot, P_BIGINT rem) {
    uint64_t remainder = 0; uint8_t overflow_check;
    for (size_t i = a->n; i > 0; --i) {
        quot->limbs[i - 1] = __DIV_HELPER_UI64__(remainder, a->limbs[i - 1], b, &remainder, &overflow_check);
    }
    limb_t quot_first = quot->limbs[0]; size_t quot_n = quot->n; size_t quot_sign = quot->sign;
    rem->limbs[0] = remainder; /**/ rem->n = (remainder) ? 1 : 0; /**/ rem->sign = 1;
    if (rem->limbs == quot->limbs) { quot->limbs[0] = quot_first; quot->n = quot_n; quot_sign = quot->sign; }
    __BIGINT_INTERNAL_TRIM_LZ__(quot); /**/ if (quot->n == 0) quot->sign = 1;
}
void __BIGINT_KNUTH_D__(PCONST_BIGINT a, PCONST_BIGINT b, P_BIGINT quot, P_BIGINT rem, calc_ctx knuth_ctx, dnml_status *err) {
    /* ---- Setup ---- */ dnml_status echeck;
    uint8_t shift = __CLZ_UI64__(b->limbs[b->n - 1]);
    size_t m = a->n, n = b->n, knuth_mark = scratch_mark(&knuth_ctx);
    BIGINT_TEMP(a_copy, m + 1, knuth_ctx, knuth_mark, echeck, err,);
    BIGINT_TEMP(b_copy, n, knuth_ctx, knuth_mark, echeck, err,);
    BIGINT_TEMP(ret_quot, m, knuth_ctx, knuth_mark, echeck, err,);
    BIGINT_TEMP(ret_rem, n, knuth_ctx, knuth_mark, echeck, err,);


    /* 1. Normalization */
    /*  - This stage basically make sure b is large enough to be divided by a
    *     by making b's most significant limb's highest bit is 1  */
    uint64_t carry = 0;
    for (size_t i = 0; i < m; ++i) {
        uint64_t x = a->limbs[i];
        a_copy.limbs[i] = (x << shift) | carry;
        carry = (shift ? x >> (U64_BITS - shift) : 0);
    }
    a_copy.limbs[m] = carry;
    a_copy.n = m + 1;
    carry = 0;
    for (size_t i = 0; i < n; ++i) {
        uint64_t x = b->limbs[i];
        b_copy.limbs[i] = (x << shift) | carry;
        carry = (shift ? x >> (U64_BITS - shift) : 0);
    }
    b_copy.n = n;
    ret_quot.n = m - n + 1;

    /* 3-5. Main Loop */
    for (size_t j = m - n + 1; j > 0; --j) {
        /* 3. Estimation */
        /*  - Get 2 limb of a (128 bit ----> a2 + a1) / 1 limb of b -------> Estimated Quotient (qhat)
        *   - Get 2 limb of a (128 bit ----> a2 + a1) % 1 limb of b -------> Remainder of that estimated quotient (rhat)
        *   ------> qhat = (a2 * 2^64 + a1) / b1
        *   ------> rhat = (a2 * 2^64 + a1) % b1
        *   --------------> a2 * 2^64 + a1 = qhat * b1 + rhat   (Important identity D)
        *   --------------> a2 * 2^64 + a1 - qhat * b1 = rhat   (Call this P)
        */
        uint64_t a2 = a_copy.limbs[j + n];                      // 1st highest limb of a
        uint64_t a1 = a_copy.limbs[j + n - 1];                  // 2nd highest limb of a
        uint64_t a0 = (n >= 2) ? a_copy.limbs[j + n - 2] : 0;   // 3rd highest limb of a (DETECT OVERESTIMATION)
        uint64_t b1 = b_copy.limbs[n - 1];                      // 1st highest limb of b
        uint64_t b0 = (n >= 2) ? b_copy.limbs[n - 2] : 0;       // 2nd highest limb of b (used to validate quotient estimation - DETECT OVERESTIMATION)
        uint64_t qhat, rhat; uint8_t overflow_check;
        qhat = __DIV_HELPER_UI64__(a2, a1, b1, &rhat, &overflow_check);
        if (overflow_check) { scratch_rewind(&knuth_ctx, knuth_mark); *err = BIGINT_ERR_RANGE; return; }

        // Validating quotient estimation (Prevent overestimation before multi-limb subtraction)
        if (qhat == UINT64_MAX) --qhat; // Check if estimates quotient is too large
        while (qhat * b0 > ((uint128)rhat << U64_BITS) + a0) {
            /* We've already got: (note: B = 2^64)
            *    +) Dividend (3 limbs of a) = a2 * B^2 + a1 * B + a0
            *    +) Divisor  (2 limbs of b) = b1 * B + b0
            * -------> +) qhat.Divisor = qhat.b1.B + q.b0
            *             ------> -qhat.b1.B = q.B0 (Call this L)
            *          +) Dividend - qhat.b1.B = (a2 * B^2 + a1 * B + a0) - qhat.b1.B
            *                                  = a2 * B^2 + a1 * B + a0 - qhat.b1.B
            *                                  = B(a2.B + a1 + a0.B^-1) - B.qhat.b1
            *                                  = B(a2.B + a1 + a0.b^-1 - qhat.b1)
            *                                  = B((a2.B + a1 - qhat.b1) + a0.b^-1)
            *                                  = B(rhat + a0.b^-1) (Proven from P)
            *                                  = rhat.B + a0        (Call this identity O)
            * -------> From O + L, we've got:
            *          +) Dividend > qhat.b1.B ------> rhat.B + a0 > q.b0 (Quotient small enough)
            *          +) Dividend = qhat.b1.B ------> rhat.B + a0 = q.b0 (Quotient small enough)
            *          +) Dividend < qhat.b1.B ------> rhat.B + a0 < q.b0 (Quotient too large)
            * -------> Check if quotient too large through (qhat * b0) > (rhat.2^64 + a0)
            * -------> Check (qhat * b0 > rhat << 64 + a0) -----> Decrement
            */ --qhat;
            /* Identity D (a2.B + a1 = qhat.b1 + rhat) must stay true
            * -------> When we decrement qhat, identity D must still be true
            * -------> (qhat - 1).b1 + rhat + ???  = q.b1 + r
            * -------> qhat.b1 - b1 + rhat + b1    = q.b1 + r
            */ rhat += b1;
            if (rhat < b1) break; // At most 2 decrements (Knuth approved)
        }

        /* 4. Multiply-subtract */
        // Basically gets the difference between the current limb range of a - qhat.b
        // -------> The remainder * B + next limb range will continue to divide by b
        // -------> Represents long division (remainder * 10 + next dividend digit) / divisor
        uint64_t borrow = 0;
        for (size_t i = 0; i < n; ++i) { uint64_t low, high;
            low = __MUL_UI64__(qhat, b_copy.limbs[i], &high); /* Multi-limb multiplication */
            uint64_t x = a_copy.limbs[j + 1]; /**/ uint64_t t = x - low - borrow;
            borrow = (t > x) + high; /**/ a_copy.limbs[j + i] = t;
        } uint64_t x = a_copy.limbs[j + n]; /**/ a_copy.limbs[j + n] = x - borrow;

        /* 5. Correction */
        /*  - The subtraction aboves follow the form of (a{j+n} + a{j+n-1 .. j}) - (borrow + qhat.b)
        *   -------> a{j+n} - borrow = a{j+n-1 ... j} - qhat.b
        *   -------> If borrow > a[j + n] -------> a[j + n] - borrow < 0
        *   -------> a[j+n-1 ... j] - qhat.b < 0
        *   -------------> qhat is still too large to be divided
        *   -------------> qhat needs to be decremented
        */
        if (x < borrow) {
            --qhat; /* if x underflows -> qhat was still too large -> Decrement */
            uint64_t carry2 = 0;
            /* Doing the operation a + b by:
            *   +) Adding each limb back + handle carries
            *       -----> Basically multi-limb addition
            *   +) Why? Because we want a - qhat.b >= 0 when qhat is decremented
            *       -----> a - (qhat - 1).b >= 0
            *       -----> a - qhat.b + b   >= 0
            *       -----> a + b will corect the underflow from qhat being too big
            */
            for (size_t i = 0; i < n; ++i) {
                uint64_t t = a_copy.limbs[j + i] + b_copy.limbs[i] + carry2;
                carry2 = (t < a_copy.limbs[j + i]);
                a_copy.limbs[j + i] = t;
            } a_copy.limbs[j + n] += carry2; // Handles remaining carry
        }
        ret_quot.limbs[j] = qhat; // Add estimated quotient of: a's 2 limbs (!28 bit) / b's 1 limb (64 bit)
    }

    /* 6. Denormalize */
    carry = 0;
    for (size_t i = n; i > 0; --i) {
        uint64_t x = a_copy.limbs[i];
        ret_rem.limbs[i] = (x >> shift) | carry;
        carry = (shift ? x << (U64_BITS - shift) : 0);
    } ret_rem.n = n;
    __BIGINT_INTERNAL_TRIM_LZ__(&ret_quot); /**/ __BIGINT_INTERNAL_TRIM_LZ__(&ret_rem);
    if (!ret_quot.n) ret_quot.sign = 1; /**/ if (!ret_rem.n) ret_rem.sign = 1;
    __BIGINT_INTERNAL_COPY__(rem, &ret_rem); __BIGINT_INTERNAL_COPY__(quot, &ret_quot);
    scratch_rewind(&knuth_ctx, knuth_mark); *err = BIGINT_SUCCESS; // Free all temporaries
}
void __BIGINT_BURNIKEL__(
    PCONST_BIGINT AH, PCONST_BIGINT AL,
    PCONST_BIGINT b, P_BIGINT quot, P_BIGINT rem, 
    calc_ctx burk_ctx, dnml_status *err
) {
    if (AH->n + AL->n <= (BIGINT_SHORT << 1) && b->n <= BIGINT_SHORT) {
        dnml_status echeck = BIGINT_SUCCESS; size_t base_mark = scratch_mark(&burk_ctx);
        BIGINT_TEMP(a, AH->n + AL->n, burk_ctx, base_mark, echeck, err,);
        memcpy(a.limbs, AL->limbs, AL->n * U64_BYTES);
        memcpy(a.limbs + AH->n, AH->limbs, AH->n * U64_BYTES);
        __BIGINT_SHORT_DIVISION__(&a, b->limbs[0], quot, rem); 
        scratch_rewind(&burk_ctx, base_mark); *err = BIGINT_SUCCESS;
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
    // q2 has 2x q1's cap due to it being used to also accomodate q1 to later acts like the returning quotient
    dnml_status echeck = BIGINT_SUCCESS, rec_err = BIGINT_SUCCESS; size_t burk_mark = scratch_mark(&burk_ctx);
    BIGINT_TEMP(q1, (k << 1), burk_ctx, burk_mark, echeck, err,);
    BIGINT_TEMP(q2, (k << 2), burk_ctx, burk_mark, echeck, err,);
    BIGINT_TEMP(r,  (k << 1), burk_ctx, burk_mark, echeck, err,);
    __burk_3b2(
        &a1, &a2, &a3,  // Dividends
        &b1, &b2, b,    // Divisors
        &q1, &r,  /* Quotient + Remainders */ burk_ctx, &rec_err
    ); SCRATCH_OVF(rec_err, burk_ctx, burk_mark, err,)
    bigInt r1 = {.limbs = r.limbs,      .sign = 1, .n = k,       .cap = k};
    bigInt r2 = {.limbs = r.limbs + k,  .sign = 1, .n = r.n - k, .cap = r.n - k};
    __burk_3b2(
        &r1, &r2, &a4,  // Dividends
        &b1, &b2, b,    // Divisors
        &q2, &r,  /* Quotient + Remainders*/ burk_ctx, &rec_err
    ); SCRATCH_OVF(rec_err, burk_ctx, burk_mark, err,)

    //* ---------- 3. RECOMPOSITION ---------- *//
    memcpy(q2.limbs + q2.cap, q1.limbs, q1.cap * U64_BYTES);
    q2.n = 2*k; __BIGINT_INTERNAL_TRIM_LZ__(&q2);
    __BIGINT_INTERNAL_COPY__(rem, &r); __BIGINT_INTERNAL_COPY__(quot, &q2);
    scratch_rewind(&burk_ctx, burk_mark); *err = BIGINT_SUCCESS;
}
void __BIGINT_NEWTON__(PCONST_BIGINT a, PCONST_BIGINT b, P_BIGINT quot, P_BIGINT rem, calc_ctx newton_ctx, dnml_status *err) {}
/* ------ ALGORITHMS FUNCTIONS - REMAINDER BIASED ------ */
// Only algorithms included in the modulo dispatching function is included here
void __RBIGINT_SHORT_DIVISION__(PCONST_BIGINT a, uint64_t b, P_BIGINT quot, P_BIGINT rem) {
    uint64_t remainder = 0; uint8_t overflow_check;
    for (size_t i = a->n; i > 0; --i) {
        quot->limbs[i - 1] = __DIV_HELPER_UI64__(remainder, a->limbs[i - 1], b, &remainder, &overflow_check);
    }
    __BIGINT_INTERNAL_TRIM_LZ__(quot); /**/ if (quot->n == 0) quot->sign = 1;
    rem->limbs[0] = remainder; /**/ rem->n = (remainder) ? 1 : 0; /**/ rem->sign = 1;
}
void __RBIGINT_KNUTH_D__(PCONST_BIGINT a, PCONST_BIGINT b, P_BIGINT quot, P_BIGINT rem, calc_ctx knuth_ctx, dnml_status *err) {
    /* ---- Setup ---- */ dnml_status echeck;
    uint8_t shift = __CLZ_UI64__(b->limbs[b->n - 1]);
    size_t m = a->n, n = b->n, knuth_mark = scratch_mark(&knuth_ctx);
    BIGINT_TEMP(a_copy, m + 1, knuth_ctx, knuth_mark, echeck, err,);
    BIGINT_TEMP(b_copy, n, knuth_ctx, knuth_mark, echeck, err,);
    BIGINT_TEMP(ret_quot, m, knuth_ctx, knuth_mark, echeck, err,);
    BIGINT_TEMP(ret_rem, n, knuth_ctx, knuth_mark, echeck, err,);

    /* 1. Normalization */
    /*  - This stage basically make sure b is large enough to be divided by a
    *     by making b's most significant limb's highest bit is 1  */
    uint64_t carry = 0;
    for (size_t i = 0; i < m; ++i) {
        uint64_t x = a->limbs[i];
        a_copy.limbs[i] = (x << shift) | carry;
        carry = (shift ? x >> (U64_BITS - shift) : 0);
    }
    a_copy.limbs[m] = carry;
    a_copy.n = m + 1;
    carry = 0;
    for (size_t i = 0; i < n; ++i) {
        uint64_t x = b->limbs[i];
        b_copy.limbs[i] = (x << shift) | carry;
        carry = (shift ? x >> (U64_BITS - shift) : 0);
    }
    b_copy.n = n;
    ret_quot.n = m - n + 1;

    /* 3-5. Main Loop */
    for (size_t j = m - n + 1; j > 0; --j) {
        /* 3. Estimation */
        /*  - Get 2 limb of a (128 bit ----> a2 + a1) / 1 limb of b -------> Estimated Quotient (qhat)
        *   - Get 2 limb of a (128 bit ----> a2 + a1) % 1 limb of b -------> Remainder of that estimated quotient (rhat)
        *   ------> qhat = (a2 * 2^64 + a1) / b1
        *   ------> rhat = (a2 * 2^64 + a1) % b1
        *   --------------> a2 * 2^64 + a1 = qhat * b1 + rhat   (Important identity D)
        *   --------------> a2 * 2^64 + a1 - qhat * b1 = rhat   (Call this P)
        */
        uint64_t a2 = a_copy.limbs[j + n];                      // 1st highest limb of a
        uint64_t a1 = a_copy.limbs[j + n - 1];                  // 2nd highest limb of a
        uint64_t a0 = (n >= 2) ? a_copy.limbs[j + n - 2] : 0;   // 3rd highest limb of a (DETECT OVERESTIMATION)
        uint64_t b1 = b_copy.limbs[n - 1];                      // 1st highest limb of b
        uint64_t b0 = (n >= 2) ? b_copy.limbs[n - 2] : 0;       // 2nd highest limb of b (used to validate quotient estimation - DETECT OVERESTIMATION)
        uint64_t qhat, rhat; uint8_t overflow_check;
        qhat = __DIV_HELPER_UI64__(a2, a1, b1, &rhat, &overflow_check);
        if (overflow_check) { scratch_rewind(&knuth_ctx, knuth_mark); *err = BIGINT_ERR_RANGE; return; }

        // Validating quotient estimation (Prevent overestimation before multi-limb subtraction)
        if (qhat == UINT64_MAX) --qhat; // Check if estimates quotient is too large
        while (qhat * b0 > ((uint128)rhat << U64_BITS) + a0) {
            /* We've already got: (note: B = 2^64)
            *    +) Dividend (3 limbs of a) = a2 * B^2 + a1 * B + a0
            *    +) Divisor  (2 limbs of b) = b1 * B + b0
            * -------> +) qhat.Divisor = qhat.b1.B + q.b0
            *             ------> -qhat.b1.B = q.B0 (Call this L)
            *          +) Dividend - qhat.b1.B = (a2 * B^2 + a1 * B + a0) - qhat.b1.B
            *                                  = a2 * B^2 + a1 * B + a0 - qhat.b1.B
            *                                  = B(a2.B + a1 + a0.B^-1) - B.qhat.b1
            *                                  = B(a2.B + a1 + a0.b^-1 - qhat.b1)
            *                                  = B((a2.B + a1 - qhat.b1) + a0.b^-1)
            *                                  = B(rhat + a0.b^-1) (Proven from P)
            *                                  = rhat.B + a0        (Call this identity O)
            * -------> From O + L, we've got:
            *          +) Dividend > qhat.b1.B ------> rhat.B + a0 > q.b0 (Quotient small enough)
            *          +) Dividend = qhat.b1.B ------> rhat.B + a0 = q.b0 (Quotient small enough)
            *          +) Dividend < qhat.b1.B ------> rhat.B + a0 < q.b0 (Quotient too large)
            * -------> Check if quotient too large through (qhat * b0) > (rhat.2^64 + a0)
            * -------> Check (qhat * b0 > rhat << 64 + a0) -----> Decrement
            */ --qhat;
            /* Identity D (a2.B + a1 = qhat.b1 + rhat) must stay true
            * -------> When we decrement qhat, identity D must still be true
            * -------> (qhat - 1).b1 + rhat + ???  = q.b1 + r
            * -------> qhat.b1 - b1 + rhat + b1    = q.b1 + r
            */ rhat += b1;
            if (rhat < b1) break; // At most 2 decrements (Knuth approved)
        }

        /* 4. Multiply-subtract */
        // Basically gets the difference between the current limb range of a - qhat.b
        // -------> The remainder * B + next limb range will continue to divide by b
        // -------> Represents long division (remainder * 10 + next dividend digit) / divisor
        uint64_t borrow = 0;
        for (size_t i = 0; i < n; ++i) { uint64_t low, high;
            low = __MUL_UI64__(qhat, b_copy.limbs[i], &high); /* Multi-limb multiplication */
            uint64_t x = a_copy.limbs[j + 1]; /**/ uint64_t t = x - low - borrow;
            borrow = (t > x) + high; /**/ a_copy.limbs[j + i] = t;
        } uint64_t x = a_copy.limbs[j + n]; /**/ a_copy.limbs[j + n] = x - borrow;

        /* 5. Correction */
        /*  - The subtraction aboves follow the form of (a{j+n} + a{j+n-1 .. j}) - (borrow + qhat.b)
        *   -------> a{j+n} - borrow = a{j+n-1 ... j} - qhat.b
        *   -------> If borrow > a[j + n] -------> a[j + n] - borrow < 0
        *   -------> a[j+n-1 ... j] - qhat.b < 0
        *   -------------> qhat is still too large to be divided
        *   -------------> qhat needs to be decremented
        */
        if (x < borrow) {
            --qhat; /* if x underflows -> qhat was still too large -> Decrement */
            uint64_t carry2 = 0;
            /* Doing the operation a + b by:
            *   +) Adding each limb back + handle carries
            *       -----> Basically multi-limb addition
            *   +) Why? Because we want a - qhat.b >= 0 when qhat is decremented
            *       -----> a - (qhat - 1).b >= 0
            *       -----> a - qhat.b + b   >= 0
            *       -----> a + b will corect the underflow from qhat being too big
            */
            for (size_t i = 0; i < n; ++i) {
                uint64_t t = a_copy.limbs[j + i] + b_copy.limbs[i] + carry2;
                carry2 = (t < a_copy.limbs[j + i]);
                a_copy.limbs[j + i] = t;
            } a_copy.limbs[j + n] += carry2; // Handles remaining carry
        }
        ret_quot.limbs[j] = qhat; // Add estimated quotient of: a's 2 limbs (!28 bit) / b's 1 limb (64 bit)
    }

    /* 6. Denormalize */
    carry = 0;
    for (size_t i = n; i > 0; --i) {
        uint64_t x = a_copy.limbs[i];
        ret_rem.limbs[i] = (x >> shift) | carry;
        carry = (shift ? x << (U64_BITS - shift) : 0);
    } ret_rem.n = n;
    __BIGINT_INTERNAL_TRIM_LZ__(&ret_quot); /**/ __BIGINT_INTERNAL_TRIM_LZ__(&ret_rem);
    if (!ret_quot.n) ret_quot.sign = 1; /**/ if (!ret_rem.n) ret_rem.sign = 1;
    __BIGINT_INTERNAL_COPY__(quot, &ret_quot); __BIGINT_INTERNAL_COPY__(rem, &ret_rem); 
    scratch_rewind(&knuth_ctx, knuth_mark); *err = BIGINT_SUCCESS; // Free all temporaries
}
void __RBIGINT_NEWTON__(PCONST_BIGINT a, PCONST_BIGINT b, P_BIGINT quot, P_BIGINT rem, calc_ctx newton_ctx, dnml_status *err) {}
/* --------------- ALGORITHM DISPATCHER --------------- */
void __BIGINT_DIV_DISP__(PCONST_BIGINT a, PCONST_BIGINT b, P_BIGINT quot, P_BIGINT tmp_rem, calc_ctx div_ctx, dnml_status *err) {
    if (b->n < BIGINT_SHORT) {  __BIGINT_SHORT_DIVISION__(a, b->limbs[0], quot, tmp_rem); *err = BIGINT_SUCCESS; }
    else if (b->n < BIGINT_KNUTH) __BIGINT_KNUTH_D__(a, b, quot, tmp_rem, div_ctx, err);
    else if (b->n < BIGINT_BURNIKEL) { 
        size_t k = (size_t)(b->n >> 1) + 1;
        bigInt AL = { .limbs = a->limbs, .sign = a->sign, .n = max(a->n, 2*k), .cap = max(a->n, 2*k)};
        bigInt AH = {
            .limbs = a->limbs + max(a->n, 2*k),
            .sign = a->sign,
            .n = (a->n < 2*k) ? 0 : 2*k - a->n,
            .cap = (a->n < 2*k) ? 0 : 2*k - a->n
        }; __BIGINT_BURNIKEL__(&AH, &AL, b, quot, tmp_rem, div_ctx, err);
    } else __BIGINT_NEWTON__(a, b, quot, tmp_rem, div_ctx, err);
}
