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
#include <debug_util.h>
#include "../../util/aconv_macros.h"
/** ------------------- General BigInt Division -------------------
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
 *      - div_newton.c (Implementation of Newton-Raphson Scaled-Reciprocal Division)
 */
/* ------ WORKSPACE FUNCTIONS ------ */
size_t iter_cnt = 0; int burk_depth = 0;
#define compf(x) (x < 0) ? "[Decrease -]" : ((x == 0) ? "[Same o]" : "[Increase +]")
int8_t __BIGINT_SIGN_COMP__(const bigInt *x, const bigInt *y) {
    if (x->sign != y->sign) return (x->sign > y->sign) ? 1 : -1;
    if (x->n != y->n) return (x->sign == 1) ? 
        ((x->n > y->n) ? 1 : -1) : 
        ((x->n < y->n) ? 1 : -1)
    ;
    for (size_t i = x->n; i > 0; --i) {
        if (x->limbs[i - 1] != y->limbs[i - 1]) return (x->sign == 1) ? 
            ((x->limbs[i - 1] > y->limbs[i - 1]) ? 1 : -1) :
            ((x->limbs[i - 1] < y->limbs[i - 1]) ? 1 : -1);    
        ;
    } return 0;
}
void print_bi_limbs(const char *xname, const bigInt *x, FILE* f) {
    fprintf(f, "%s = {", xname);
    if (x->n <= 6) { fputc(' ', f);
        for (size_t i = 0; i < x->n; ++i) {
            fprintf(f, "%" PRIX64 "", x->limbs[i]);
            if (likely(i < x->n - 1)) fputs(", ", f);
        } fputs(" } ", f); fprintf(f, "[%c]", (x->sign < 0) ? '-' : '+');
    }
    else { fputs("\n", f);
        for (size_t i = 0; i < x->n; i += 8) {
            // Doing the loop like this makes it easier to maintain an 8-column row layout
            fputs("    ", f); 
            for (uint8_t j = i; j < i + 7; ++j) { 
                if (j >= x->n) break;
                fprintf(f, "%" PRIX64 ", ", x->limbs[j]); 
            } fputc('\n', f); // Yeah ig bro
        } fprintf(f, "} [%c]", (x->sign < 0) ? '-' : '+');
    }
}
size_t __BIGINT_SHORTDIV_WS__(size_t a_size, size_t b_size) { return 0; }
size_t __BIGINT_BURK_WS__(size_t a_size, size_t b_size) {
    size_t k = b_size >> 1;
    // BURNIKEL FUNCTION
    size_t q1_q2_size = (k << 1) + (k << 2); // 2k + 4k = 6k
    size_t rsize = k << 1; // 2k
    // 3-BY-2 HELPER
    size_t csize = k + b_size; // 2k
    size_t iq_size = k << 1; // 2k
    size_t dsize = (k << 1) + k; // 3k
    return (3 * (q1_q2_size + rsize + csize + iq_size + dsize) >> 1) + a_size;
    // a_size has been updated/halved from recursion.
}
size_t __BIGINT_BURNIKEL_WS__(size_t a_size, size_t b_size, bool normalize) {
    if (!normalize) return __BIGINT_BURK_WS__(a_size, b_size);
    size_t anorm_size = a_size + 1; size_t bnorm_size = b_size;
    size_t burk_size = __BIGINT_BURK_WS__(anorm_size, bnorm_size);
    return anorm_size + bnorm_size + burk_size;
}
size_t __BIGINT_DIV_WS__(size_t a_size, size_t b_size, bool normalize) {
    if (b_size < BIGINT_SHORT) return __BIGINT_SHORTDIV_WS__(a_size, b_size);
    else if (b_size < BIGINT_BURNIKEL) return __BIGINT_BURNIKEL_WS__(a_size, b_size, normalize);
    else return __BIGINT_NEWTON_WS__(a_size, b_size);
}




/* -------- ALGORITHM FUNCTIONS - SHORT DIVISION -------- */
void __BIGINT_SHORT_DIVISION__(PCONST_BIGINT a, uint64_t b, P_BIGINT quot, P_BIGINT rem) {
    uint64_t remainder = 0; uint8_t ovf_check;
    for (size_t i = a->n; i > 0; --i) {
        quot->limbs[i - 1] = __DIV_HELPER_UI64__(a->limbs[i - 1], remainder, b, &remainder, &ovf_check);
    } quot->n = a->n; __BIGINT_INTERNAL_TRIM_LZ__(quot);
    if (rem != NULL) { rem->limbs[0] = remainder; /**/ rem->n = !!(remainder); /**/ rem->sign = 1; }
}
void __RBIGINT_SHORT_DIVISION__(PCONST_BIGINT a, uint64_t b, P_BIGINT rem) {
    uint64_t remainder = 0, dummy = 0; uint8_t ovf_check;
    for (size_t i = a->n; i > 0; --i) {
        dummy = __DIV_HELPER_UI64__(remainder, a->limbs[i - 1], b, &remainder, &ovf_check);
    } rem->limbs[0] = remainder; /**/ rem->n = !!(remainder); /**/ rem->sign = 1;
}




/* -------- ALGORITHM FUNCTIONS - BURNIKEL-ZIEGLER -------- */
void __burk_3b2(
    PCONST_BIGINT a1, PCONST_BIGINT a2, PCONST_BIGINT a3,
    PCONST_BIGINT b1, PCONST_BIGINT b2, PCONST_BIGINT B,
    P_BIGINT q, P_BIGINT r, calc_ctx *burk_helper_ctx, dnml_status *err, FILE *f, bool inspect
) {
    dnml_status echeck = BIGINT_SUCCESS, rec_err = BIGINT_SUCCESS;
    size_t burk_helper_mark = scratch_mark(burk_helper_ctx);
    BIGINT_TEMP(iq, a1->n + a2->n, burk_helper_ctx, burk_helper_mark, echeck, err,);
    BIGINT_TEMP(c, b1->n + a3->n + 2, burk_helper_ctx, burk_helper_mark, echeck, err,);
    __BIGINT_BURK__(a1, a2, b1, &iq, &c, burk_helper_ctx, &rec_err, f);
    SCRATCH_OVF(rec_err, burk_helper_ctx, burk_helper_mark, err,)

    BIGINT_TEMP(d, (iq.n + b2->n), burk_helper_ctx, burk_helper_mark, echeck, err,);
    __BIGINT_MUL_DISP__(&iq, b2, &d, burk_helper_ctx, &echeck);
    SCRATCH_OVF(rec_err, burk_helper_ctx, burk_helper_mark, err,)

    // Quotient and Remainder Correction step AND Loop (Burnikel-Ziegler lowk the goat)
    // We first subtract the fused [remainder + lower dividend limbs] ([c, a3]) by the
    // product of the [intermediate quotient * lower divisor limbs] ([iq * b2] = d)
    __BIGINT_INTERNAL_LLSHIFT__(&c, a3->n);
    if (a3->limbs) {
        memcpy(c.limbs, a3->limbs, a3->n * U64_BYTES); 
        c.n = max(c.n, a3->n); __BIGINT_INTERNAL_TRIM_LZ__(&c);
    }
    __BIGINT_SUB_SAW__(&c, &c, &d);
    BIGINT_TEMP(prev_c, c.cap, burk_helper_ctx, burk_helper_mark, echeck, err,);
    BIGINT_TEMP(prev_iq, iq.cap, burk_helper_ctx, burk_helper_mark, echeck, err,); 
    memset(prev_c.limbs, 0, prev_c.cap * U64_BYTES); memset(prev_iq.limbs, 0, prev_iq.cap * U64_BYTES);
    if (inspect) {
        fputs(     "===================== BURK 3B2 INSPECTION =====================\n", f);
        fprintf(f, "Current Recursion depth (burk_depth): %d\n", burk_depth);
        fprintf(f, "Current iq metadata [iq.n, iq.cap]: [%zu, %zu]\n", iq.n, iq.cap);
        fprintf(f, "Current c metadata [c.size, c.cap]: [%zu, %zu]\n", c.n, c.cap);
        fputs(     "===================== BURK 3B2 LOOP =====================\n", f);
    }
    uint64_t a[1] = {1}; bigInt one = {.limbs = a, .sign = 1, .n = 1, .cap = 1};
    while (c.sign < 0) { ++iter_cnt;
        if (inspect) {
            fprintf(f, "- Current Iteration Count: %zu\n", iter_cnt);
            fprintf(f, "    o) Current iq metadata [iq.n, iq.cap, iq.sign]: [%zu, %zu, %" PRId8 "]\n", iq.n, iq.cap, iq.sign);
            fprintf(f, "    o) Current c metadata [c.size, c.cap, c.sign]: [%zu, %zu, %" PRId8 "]\n", c.n, c.cap, c.sign);
            __BIGINT_INTERNAL_COPY__(&prev_iq, &iq); __BIGINT_INTERNAL_COPY__(&prev_c, &c);
        } 
        __BIGINT_SUB_WB__(&iq, &iq, &one); __BIGINT_ADD_SAW__(&c, &c, B);
        if (inspect) {
            fprintf(f, "    o) Updated iq metadata [iq.n, iq.cap]: [%zu, %zu, %" PRId8 "]\n", iq.n, iq.cap, iq.sign);
            fprintf(f, "    o) Updated c metadata [c.size, c.cap]: [%zu, %zu, %" PRId8 "]\n", c.n, c.cap, c.sign);
            int8_t piq_change = __BIGINT_SIGN_COMP__(&iq, &prev_iq); int8_t pc_change = __BIGINT_SIGN_COMP__(&c, &prev_c);
            fprintf(f, "    o) Updated verdict [iq, c]: [%s, %s]\n", compf(piq_change), compf(pc_change));
            print_bi_limbs("    o) prev_iq", &prev_iq, f); fputc('\n', f); print_bi_limbs("    o) iq", &iq, f); fputc('\n', f);
            print_bi_limbs("    o) prev_c", &prev_c, f); fputc('\n', f); print_bi_limbs("    o) c", &c, f); fputc('\n', f);
        } 
        if (iter_cnt > 1000) { scratch_clear(burk_helper_ctx); scratch_destruct(burk_helper_ctx); fclose(f); exit(SIGABRT); }
    } iter_cnt = 0;
    __BIGINT_INTERNAL_COPY__(q, &iq); __BIGINT_INTERNAL_COPY__(r, &c);
    scratch_rewind(burk_helper_ctx, burk_helper_mark); *err = BIGINT_SUCCESS;
}
void __BIGINT_BURK__(
    PCONST_BIGINT AH, PCONST_BIGINT AL,
    PCONST_BIGINT b, P_BIGINT quot, P_BIGINT rem, 
    calc_ctx *burk_ctx, dnml_status *err, FILE *f
) {
    if (b->n <= BIGINT_SHORT) {
        dnml_status echeck = BIGINT_SUCCESS; size_t base_mark = scratch_mark(burk_ctx);
        BIGINT_TEMP(a, AH->n + AL->n, burk_ctx, base_mark, echeck, err,);
        memcpy(a.limbs, AL->limbs, AL->n * U64_BYTES); 
        memcpy(a.limbs + AL->n, AH->limbs, AH->n * U64_BYTES);
        a.n = AH->n + AL->n; __BIGINT_INTERNAL_TRIM_LZ__(&a);
        __BIGINT_SHORT_DIVISION__(&a, b->limbs[0], quot, rem); 
        scratch_rewind(burk_ctx, base_mark); *err = BIGINT_SUCCESS; return;
    }
    //* -------- 1. SPLIT ---------- *//
    size_t k = b->n >> 1; ++burk_depth;
    /* Dividend - A - QUARTERS */
    size_t a4n = min(k, AL->n); size_t a3n = (AL->n > k) ? min(k, AL->n - k) : 0;
    size_t a2n = min(k, AH->n); size_t a1n = (AH->n > k) ? min(k, AH->n - k) : 0;
    bigInt a4 = {.limbs = AL->limbs,                            .sign = 1, .n = a4n, .cap = a4n};
    bigInt a3 = {.limbs = (AL->n > k) ? (AL->limbs + k) : NULL, .sign = 1, .n = a3n, .cap = a3n};
    bigInt a2 = {.limbs = AH->limbs,                            .sign = 1, .n = a2n, .cap = a2n};
    bigInt a1 = {.limbs = (AH->n > k) ? (AH->limbs + k) : NULL, .sign = 1, .n = a1n, .cap = a1n};
    /* Divisors - B - HALVES */
    bigInt b2 = {.limbs = b->limbs,     .sign = 1,  /**/    .n = k,        .cap = k};
    bigInt b1 = {.limbs = b->limbs + k, .sign = 1,  /**/    .n = b->n - k, .cap = b->n - k};

    //* --------- 2. ACTUAL OPERATION --------- *//
    // q2 has 2x q1's cap due to it being used to also accomodate q1 to later acts like the returning quotient
    dnml_status echeck = BIGINT_SUCCESS, rec_err = BIGINT_SUCCESS; size_t burk_mark = scratch_mark(burk_ctx);
    BIGINT_TEMP(q1, (k << 1), burk_ctx, burk_mark, echeck, err,);
    BIGINT_TEMP(q2, (k << 2), burk_ctx, burk_mark, echeck, err,);
    BIGINT_TEMP(r,  (k << 1), burk_ctx, burk_mark, echeck, err,);
    if (burk_depth) {
        fputs(     "\n===================== BURK INSPECTION =====================\n", f);
        fprintf(f, "Current Recursion depth (burk_depth): %d\n", burk_depth);
        fprintf(f, "Current r metadata [r.n, r.cap]: [%zu, %zu]\n", r.n, r.cap);
    } 
    __burk_3b2(
        &a1, &a2, &a3,  // Dividends
        &b1, &b2, b,    // Divisors
        &q1, &r,  /* Quotient + Remainders */ burk_ctx, &rec_err, f, true
    ); SCRATCH_OVF(rec_err, burk_ctx, burk_mark, err,) /**/ size_t r2n = (r.n > k) ? r.n - k : 0;
    bigInt r1 = {.limbs = r.limbs,                    .sign = 1, .n = min(r.n, k), .cap = min(r.n, k)};
    bigInt r2 = {.limbs = (r2n) ? r.limbs + k : NULL, .sign = 1, .n = r2n,         .cap = r2n};
    __burk_3b2(
        &r1, &r2, &a4,  // Dividends
        &b1, &b2, b,    // Divisors
        &q2, &r,  /* Quotient + Remainders*/ burk_ctx, &rec_err, f, true
    ); SCRATCH_OVF(rec_err, burk_ctx, burk_mark, err,)

    //* ---------- 3. RECOMPOSITION ---------- *//
    memset(q2.limbs + q2.n, 0, (q2.cap - q2.n) * U64_BYTES); // Clear the top limbs for q1
    memcpy(q2.limbs + q2.n, q1.limbs, q1.n * U64_BYTES); // Copy q1 into q2 top halves (combining q1 and q2)
    q2.n = q2.n + q1.n; __BIGINT_INTERNAL_TRIM_LZ__(&q2); // q1 is the top half, q2 is the bottom half, of the quotient
    if (rem != NULL) __BIGINT_INTERNAL_COPY__(rem, &r); /**/ __BIGINT_INTERNAL_COPY__(quot, &q2);
    scratch_rewind(burk_ctx, burk_mark); *err = BIGINT_SUCCESS; --burk_depth;
}
void __BIGINT_BURNIKEL__(PCONST_BIGINT a, PCONST_BIGINT b, P_BIGINT quot, calc_ctx *burk_ctx, dnml_status *err, FILE *f) {
    // Divisor normalization (prevent radical over-estimation of iq and c)
    uint8_t shift = __CLZ_UI64__(b->limbs[b->n - 1]);
    if (shift) {
        size_t burnikel_mark = scratch_mark(burk_ctx); dnml_status echeck;
        BIGINT_TEMP(A_norm, a->n + 1, burk_ctx, burnikel_mark, echeck, err,); /**/ A_norm.n = a->n;
        BIGINT_TEMP(B_norm, b->n, burk_ctx, burnikel_mark, echeck, err,); /**/ B_norm.n = b->n;
        uint64_t next = 0; /**/ uint64_t iso_mask = (UINT64_C(1) << shift) - 1;
        // Copy + Shift Infusion for A_norm (Normalization of A)
        for (size_t i = 0; i < a->n; ++i) {
            A_norm.limbs[i] = (a->limbs[i] << shift) | next;
            next = (a->limbs[i] >> (U64_BITS - shift)) & iso_mask;;
        } if (next) A_norm.limbs[A_norm.n++] = next; /**/ __BIGINT_INTERNAL_TRIM_LZ__(&A_norm); next = 0;
 

        // Copy + Shift Infusion for B_norm (Normalization of B)
        for (size_t i = 0; i < b->n; ++i) {
            B_norm.limbs[i] = (b->limbs[i] << shift) | next;
            next = (b->limbs[i] >> (U64_BITS - shift)) & iso_mask;;
        } if (next) B_norm.limbs[B_norm.n++] = next; /**/ __BIGINT_INTERNAL_TRIM_LZ__(&B_norm);


        // Splitting a into AH and AL for __BIGINT_BURK__
        size_t k = (B_norm.n + 1) >> 1; /**/ size_t al_range = min(A_norm.n, (k << 1));
        size_t ah_range = (A_norm.n < (k << 1)) ? 0 : (A_norm.n - (k << 1));
        bigInt AL = { .limbs = A_norm.limbs,            .n = al_range, .cap = al_range, .sign = 1 };
        bigInt AH = { .limbs = A_norm.limbs + al_range, .n = ah_range, .cap = ah_range, .sign = 1 };
        __BIGINT_BURK__(&AH, &AL, &B_norm, quot, NULL, burk_ctx, err, f); if (shift) scratch_rewind(burk_ctx, burnikel_mark);
    } else {
        // Splitting a into AH and AL for __BIGINT_BURK__
        size_t k = (b->n + 1) >> 1; /**/ size_t al_range = min(a->n, (k << 1));
        size_t ah_range = (a->n < (k << 1)) ? 0 : (a->n - (k << 1));
        bigInt AL = { .limbs = a->limbs,            .n = al_range, .cap = al_range, .sign = 1 };
        bigInt AH = { .limbs = a->limbs + al_range, .n = ah_range, .cap = ah_range, .sign = 1 };
        __BIGINT_BURK__(&AH, &AL, b, quot, NULL, burk_ctx, err, f);
    }
}




/* --------------- ALGORITHM DISPATCHER --------------- */
void __BIGINT_DIV_DISP__(PCONST_BIGINT a, PCONST_BIGINT b, P_BIGINT quot, calc_ctx *div_ctx, dnml_status *err) {
    if (b->n < BIGINT_SHORT) {  __BIGINT_SHORT_DIVISION__(a, b->limbs[0], quot, NULL); *err = BIGINT_SUCCESS; }
    else if (b->n < BIGINT_BURNIKEL) __BIGINT_BURNIKEL__(a, b, quot, div_ctx, err, NULL);
    else __BIGINT_NEWTON__(a, b, quot, NULL, div_ctx, err);
} 
