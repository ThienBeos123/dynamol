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
 *      - Short Division (64-bit Divisor ONLY)
 *      - Knuth Algorithm D (General)
 *      - Burnikel-Ziegler Division (General)
 *
 * This file is generally the main algorithm file for bigInt division, containnig
 * the division algorithm dispatcher, as well as the workspace sizing function dispatcher.
 * It only contains the 3-simplest division algorithms to keep its focus on being the central,
 * simple point of authority, and delegation of complexity is in other files, including:
 *
 *      - heap_div_newt.c (Implementation of Newton-Raphson Scaled-Reciprocal Division)
 */
/* -------- ALGORITHM FUNCTIONS - SHORT DIVISION -------- */
void __BIHEAP_SHORT_DIVISION__(PCONST_BIGINT a, uint64_t b, P_BIGINT quot, P_BIGINT rem) {
    uint64_t remainder = 0; uint8_t ovf_check;
    for (size_t i = a->n; i > 0; --i) {
        quot->limbs[i - 1] = __DIV_HELPER_UI64__(a->limbs[i - 1], remainder, b, &remainder, &ovf_check);
    } quot->n = a->n; __BIGINT_INTERNAL_TRIM_LZ__(quot);
    if (rem != NULL) { rem->limbs[0] = remainder; /**/ rem->n = !!(remainder); /**/ rem->sign = 1; }
}
void __RBIHEAP_SHORT_DIVISION__(PCONST_BIGINT a, uint64_t b, P_BIGINT rem) {
    uint64_t remainder = 0, dummy = 0; uint8_t overflow_check;
    for (size_t i = a->n; i > 0; --i) {
        dummy = __DIV_HELPER_UI64__(remainder, a->limbs[i - 1], b, &remainder, &overflow_check);
    } rem->limbs[0] = remainder; /**/ rem->n = (remainder) ? 1 : 0; /**/ rem->sign = 1;
}





/* -------- ALGORITHM FUNCTIONS - BURNIKEL-ZIEGLER -------- */
void __heap_burk_3b2(
    PCONST_BIGINT a1, PCONST_BIGINT a2, PCONST_BIGINT a3,
    PCONST_BIGINT b1, PCONST_BIGINT b2, PCONST_BIGINT B,
    bigInt *q, P_BIGINT r, dnml_status *err
) {
    dnml_status echeck = BIGINT_SUCCESS, rec_err = BIGINT_SUCCESS;
    bigInt *alloc_list[1], *early_free[3]; uint8_t early_cnt = 0, alloc_cnt = 0;
    bigInt iq = {0}, c = {0}; // These will be allocated at the base case
    early_free[early_cnt++] = &iq; // Setting up iq for potential early exit clearance (iq.cap = a1->n + a2->n)
    early_free[early_cnt++] = &c; // Setting up c for potential early exit clearance (c.cap = B->n)
    __BIHEAP_BURK__(a1, a2, b1, &iq, &c, &rec_err); HEAP_OOM(rec_err, err, early_free, early_cnt,);

    BIHEAP_TEMP(d, (iq.n + b2->n), echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    uint64_t a[1] = {1}; bigInt one = {.limbs = a, .sign = 1, .n = 1, .cap = 1};
    __BIHEAP_MUL_DISP__(&iq, b2, &d, &echeck); HEAP_OOM(rec_err, err, early_free, early_cnt,);
    __BIGINT_ADD_WC__(&c, &c, a3);
    while (__BIGINT_INTERNAL_COMP__(&c, &d) == -1) {
        __BIGINT_SUB_WB__(&iq, &iq, &one); __BIGINT_ADD_WC__(&c, &c, B);
    } __BIGINT_SUB_WB__(&c, &c, &d); __BIGINT_INTERNAL_MOVE__(q, &iq);
    __BIGINT_INTERNAL_MOVE__(r, &c); *err = BIGINT_SUCCESS;
}
void __BIHEAP_BURK__(PCONST_BIGINT AH, PCONST_BIGINT AL, PCONST_BIGINT b, P_BIGINT quot, P_BIGINT rem, dnml_status *err) {
    if (b->n <= BIGINT_SHORT) { 
        dnml_status echeck = BIGINT_SUCCESS; 
        bigInt *alloc_list[1], *early_free[3]; uint8_t alloc_cnt = 0, early_cnt = 0;
        BIHEAP_TEMP(a, AH->n + AL->n, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
        memcpy(a.limbs, AL->limbs, AL->n * U64_BYTES); memcpy(a.limbs + AH->n, AH->limbs, AH->n * U64_BYTES);

        // Allocating at the base the buffer for quot and rem to propagate upwards
        BIHEAP_ALLOC(quot, AH->n + AL->n, echeck, err, early_free, early_cnt,);
        BIHEAP_ALLOC(rem, 1, echeck, err, early_free, early_cnt,)
        __BIHEAP_SHORT_DIVISION__(&a, b->limbs[0], quot, rem); 
        _free_alloc_list(alloc_list, alloc_cnt); *err = BIGINT_SUCCESS; return;
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
    // Ensures r has enough capacity to be the remainder (2k limbs)
    echeck = __BIGINT_INTERNAL_ENSCAP__(&r, k << 1); HEAP_OOM(echeck, err, early_free, early_cnt,);
    __heap_burk_3b2(
        &r1, &r2, &a4,  // Dividends
        &b1, &b2, b,    // Divisors
        &q2, &r,  /* Quotient + Remainders*/ &rec_err
    ); HEAP_OOM(rec_err, err, early_free, early_cnt,);

    //* ---------- 3. RECOMPOSITION ---------- *//
    __BIGINT_INTERNAL_MOVE__(rem, &r); __BIGINT_INTERNAL_MOVE__(quot, &q2); 
    __BIGINT_INTERNAL_ENSCAP__(quot, quot->cap + q1.cap); HEAP_OOM(echeck, err, early_free, early_cnt,);
    memcpy(quot->limbs + quot->cap, q1.limbs, q1.cap * U64_BYTES); 
    quot->n = k << 1; /**/ __BIGINT_INTERNAL_TRIM_LZ__(quot); 
    _free_alloc_list(alloc_list, alloc_cnt); *err = BIGINT_SUCCESS;
}
void __BIHEAP_BURNIKEL__(PCONST_BIGINT a, PCONST_BIGINT b, P_BIGINT quot, dnml_status *err) {
    size_t k = (b->n + 1) >> 1; /**/ size_t al_range= min(a->n, (k << 1)); 
    size_t ah_range = (a->n < (k << 1)) ? 0 : (a->n - (k << 1));
    bigInt AL = { .limbs = a->limbs,            .n = al_range, .cap = al_range, .sign = 1 };
    bigInt AH = { .limbs = a->limbs + al_range, .n = ah_range, .cap = ah_range, .sign = 1 };
    __BIHEAP_BURK__(&AH, &AL, b, quot, NULL, err);
}





/* ----------- ALGORITHM DISPATCHERS ----------- */
void __BIHEAP_DIV_DISP__(PCONST_BIGINT a, PCONST_BIGINT b, P_BIGINT quot, dnml_status *err) {
    if (b->n < BIGINT_SHORT) {  __BIHEAP_SHORT_DIVISION__(a, b->limbs[0], quot, NULL); *err = BIGINT_SUCCESS; }
    else if (b->n < BIGINT_BURNIKEL) __BIHEAP_BURNIKEL__(a, b, quot, err);
    else __BIHEAP_NEWTON__(a, b, quot, NULL, err);
}
