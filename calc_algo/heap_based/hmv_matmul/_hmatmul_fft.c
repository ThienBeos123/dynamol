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




#include "_hmv_matmul_.h"
#include <debug_util.h>
#include "../../../util/aconv_macros.h"
#include "../heap_mul_fft.c"
/** ----------- Matrix-Vector Multiplication Linear Combination -----------
 * THIS FILE CONTAINS THE FOLLOWING ALGORITHMS FOR MATRIX MULTIPLICATION:
 *
 *      - Toom-cook 3-way 
 *      - Toom-cook 4-way
 *      - Toom-cook 5-way
 *      - Schonhage-Strassen
 *
 * In which they would be structurally modify to accompany the multiplications
 * of a single linear-combination pair of the form xz + yw (as utilized by hgcd)
 */
/* ------- BigInt Matrix Multiplication Toom-cook 3-way ------- */
dnml_status __BIHEAP_MATMUL_TOOM3__(
    P_BIGINT x, P_BIGINT z, /**/ P_BIGINT y, P_BIGINT w, 
    P_BIGINT xz_res, P_BIGINT yw_res
) {
    size_t xz_k = (max(x->n, z->n) + 2) / 3;
    size_t yw_k = (max(y->n, w->n) + 2) / 3;
    size_t max_k = max(xz_k, yw_k);
    //* ============== X and Z MULTIPLICATION PAIR ============== *//
    /* ------- 1a. Setup & Splitting ------- */
    size_t x0size = min(xz_k, x->n); size_t z0size = min(xz_k, z->n);
    size_t x1size = (x->n > xz_k) ? x->n - xz_k : 0; // Maximum = k
    size_t z1size = (z->n > xz_k) ? z->n - xz_k : 0; // Maximum = k
    size_t x2size = (x->n > (xz_k << 1)) ? (x->n - (xz_k << 1)) : 0;
    size_t z2size = (z->n > (xz_k << 1)) ? (z->n - (xz_k << 1)) : 0;
    size_t y0size = min(yw_k, y->n); size_t w0size = min(yw_k, w->n);
    size_t y1size = (y->n > yw_k) ? y->n - yw_k : 0; // Maximum = k
    size_t w1size = (w->n > yw_k) ? w->n - yw_k : 0; // Maximum = k
    size_t y2size = (y->n > (yw_k << 1)) ? (y->n - (yw_k << 1)) : 0;
    size_t w2size = (w->n > (yw_k << 1)) ? (w->n - (yw_k << 1)) : 0;
    size_t max_m2size = max(x2size, y2size); size_t max_n2size = max(z2size, w2size);
    bigInt m0 = {.limbs = x->limbs,                 .n = x0size,    .cap = x0size};
    bigInt m1 = {.limbs = x->limbs + xz_k,          .n = x1size,    .cap = x1size};
    bigInt m2 = {.limbs = x->limbs + (xz_k << 1),   .n = x2size,    .cap = x2size};
    bigInt n0 = {.limbs = z->limbs,                 .n = z0size,    .cap = z0size};
    bigInt n1 = {.limbs = z->limbs + xz_k,          .n = z1size,    .cap = z1size};
    bigInt n2 = {.limbs = z->limbs + (xz_k << 1),   .n = z2size,    .cap = z2size};

    /* ------------ 2a. Evaluation & Point-wise Multiplication ------------ */
    dnml_status echeck; bigInt *early_free[14]; uint8_t early_cnt = 0; /**/ bigInt *alloc_list[13]; uint8_t alloc_cnt = 0;
    // p(x) TEMPORARIES
    //  +) pOuter   = m0 + m2                           |   +) p(-1)    = pOuter - m1
    //  +) p(0)     = m0          (NO FULL TEMPORARY)   |   +) p(-2)    = 2*(p(-1) + m2) - m0
    //  +) p(1)     = pOuter + m1                       |   +) p(inf)   = m2                    (NO FULL TEMPORARY)
    BIHEAP_FTEMP(p_outer, max_k + 1, echeck, early_free, early_cnt, alloc_list, alloc_cnt); p_outer.cap = xz_k + 1;
    BIHEAP_FTEMP(p1,      max_k + 2, echeck, early_free, early_cnt, alloc_list, alloc_cnt); p1.cap = xz_k + 2;
    BIHEAP_FTEMP(p_neg1,  max_k + 1, echeck, early_free, early_cnt, alloc_list, alloc_cnt); p_neg1.cap = xz_k + 1;
    BIHEAP_FTEMP(p_neg2,  max_k + 2, echeck, early_free, early_cnt, alloc_list, alloc_cnt); p_neg2.cap = xz_k + 1;
    // q(x) TEMPORARIES
    //  +) qOuter   = n0 + n2                           |   +) q(-1)    = qOuter - n1
    //  +) q(0)     = n0          (NO FULL TEMPORARY)   |   +) q(-2)    = 2*(q(-1) + n2) - n0
    //  +) q(1)     = qOuter + n1                       |   +) q(inf)   = n2                    (NO FULL TEMPORARY)
    BIHEAP_FTEMP(q_neg2,  max_k + 2, echeck, early_free, early_cnt, alloc_list, alloc_cnt); q_neg2.cap = xz_k + 2;
    BIHEAP_FTEMP(q_outer, max_k + 1, echeck, early_free, early_cnt, alloc_list, alloc_cnt); q_outer.cap = xz_k + 1;
    BIHEAP_FTEMP(q1,      max_k + 2, echeck, early_free, early_cnt, alloc_list, alloc_cnt); q1.cap = xz_k + 2;
    BIHEAP_FTEMP(q_neg1,  max_k + 1, echeck, early_free, early_cnt, alloc_list, alloc_cnt); q_neg1.cap = xz_k + 1;
    // p(x) CALCULATIONS                                // q(x) CALCULATIONS
    __BIGINT_ADD_WC__(&p_outer, &m0, &m2);              __BIGINT_ADD_WC__(&q_outer, &n0, &n2);
    __BIGINT_ADD_WC__(&p1, &p_outer, &m1);              __BIGINT_ADD_WC__(&q1, &q_outer, &n1);
    __BIGINT_SUB_SAW__(&p_neg1, &p_outer, &m1);         __BIGINT_SUB_SAW__(&q_neg1, &q_outer, &n1);
    __BIGINT_ADD_SAW__(&p_neg2, &p_neg1, &m2);          __BIGINT_ADD_SAW__(&q_neg2, &q_neg1, &n2);
    __BIGINT_INTERNAL_LSHIFT__(&p_neg2, 1);             __BIGINT_INTERNAL_LSHIFT__(&q_neg2, 1);
    __BIGINT_SUB_SAW__(&p_neg2, &p_neg2, &m0);          __BIGINT_SUB_SAW__(&q_neg2, &q_neg2, &n0);
    /* ------------ POINT-WISE MULTIPLICATION ------------
    *   +) r(0)   = p(0)   * q(0)       ---> Cap: 2k
    *   +) r(1)   = p(1)   * q(1)       ---> Cap: 2k + 4 (original) --> 2k + 8 (interpolation - r1)
    *   +) r(-1)  = p(-1)  * q(-1)      ---> Cap: 2k + 2 (originxal) --> 2k + 7 (interpolation - r2)
    *   +) r(-2)  = p(-2)  * q(-2)      ---> Cap: 2k + 4 (original) --> 2k + 7 (interpolation - r3)
    *   +) r(inf) = p(inf) * q(inf)     ---> Cap: 2k (original)
    */
    bigInt r0 = {0}, r1 = {0}, r_neg1 = {0}, r_neg2 = {0}, rinf = {0};
    alloc_list[alloc_cnt++] = &r0; early_free[early_cnt++] = &r0;
    alloc_list[alloc_cnt++] = &r1; early_free[early_cnt++] = &r1;
    alloc_list[alloc_cnt++] = &r_neg1; early_free[early_cnt++] = &r_neg1;
    alloc_list[alloc_cnt++] = &r_neg2; early_free[early_cnt++] = &r_neg2;
    alloc_list[alloc_cnt++] = &rinf; early_free[early_cnt++] = &rinf;
    __BIHEAP_TOOM_3__(&m0, &n0, &r0, &echeck); HEAP_FOOM(echeck, early_free, early_cnt);
    __BIHEAP_TOOM_3__(&p1, &q1, &r1, &echeck); HEAP_FOOM(echeck, early_free, early_cnt);
    __BIHEAP_TOOM_3__(&p_neg1, &q_neg1, &r_neg1, &echeck); HEAP_FOOM(echeck, early_free, early_cnt);
    __BIHEAP_TOOM_3__(&p_neg2, &q_neg2, &r_neg2, &echeck); HEAP_FOOM(echeck, early_free, early_cnt);
    __BIHEAP_TOOM_3__(&m2, &n2, &rinf, &echeck); HEAP_FOOM(echeck, early_free, early_cnt);
    r1.sign = p1.sign * q1.sign; /**/ r_neg1.sign = p_neg1.sign * q_neg1.sign; /**/ r_neg2.sign = p_neg2.sign * q_neg2.sign;
    __BIGINT_INTERNAL_ENSCAP__(&r1, (xz_k << 1) + 8); HEAP_FOOM(echeck, early_free, early_cnt);
    __BIGINT_INTERNAL_ENSCAP__(&r_neg2, (xz_k << 1) + 7); HEAP_FOOM(echeck, early_free, early_cnt);
    __BIGINT_INTERNAL_ENSCAP__(&r_neg1, (xz_k << 1) + 7); HEAP_FOOM(echeck, early_free, early_cnt);

    /* ----------------- 3a. INTERPOLATION & RECOMPOSITION ----------------- */
    /* r3 = 2k + 5 */ __BIGINT_SUB_SAW__(&r_neg2, &r_neg2, &r1); __BIGINT_DIV3__(&r_neg2);
    /* r1 = 2k + 5 */ __BIGINT_SUB_SAW__(&r1, &r1, &r_neg1); __BIGINT_INTERNAL_RSHIFT__(&r1, 1);
    /* r2 = 2k + 3 */ __BIGINT_SUB_SAW__(&r_neg1, &r_neg1, &r0);
    /* r3 = 2k + 7 */ __BIGINT_SUB_SAW__(&r_neg2, &r_neg1, &r_neg2);
    __BIGINT_INTERNAL_RSHIFT__(&r_neg2, 1); __BIGINT_INTERNAL_LSHIFT__(&rinf, 1);
    __BIGINT_ADD_SAW__(&r_neg2, &r_neg2, &rinf);
    /* r2 = 2k + 7 */ __BIGINT_ADD_SAW__(&r_neg1, &r_neg1, &r1);
    __BIGINT_INTERNAL_RSHIFT__(&rinf, 1); __BIGINT_SUB_SAW__(&r_neg1, &r_neg1, &rinf);
    /* r1 = 2k + 8 */ __BIGINT_SUB_SAW__(&r1, &r1, &r_neg2);
    // ------------------ RECOMPOSITION ------------------ //
    BIHEAP_FRET(xz_tres, x->n + z->n, echeck, early_free, early_cnt);
    __BIGINT_ADD_SHIFT__(&xz_tres, &rinf, 4); __BIGINT_ADD_SHIFT__(&xz_tres, &r_neg2, 3);
    __BIGINT_ADD_SHIFT__(&xz_tres, &r_neg1, 2); __BIGINT_ADD_SHIFT__(&xz_tres, &r1, 1);
    __BIGINT_ADD_SHIFT__(&xz_tres, &r0, 0); __BIGINT_INTERNAL_MOVE__(xz_res, &xz_tres);




    //* ============== Y and W MULTIPLICATION PAIR ============== *//
    /* -------- 1b. Setup & Splitting ---------- */
    m0 = (bigInt){.limbs = y->limbs,                 .n = y0size,    .cap = y0size};
    m1 = (bigInt){.limbs = y->limbs + yw_k,          .n = y1size,    .cap = y1size};
    m2 = (bigInt){.limbs = y->limbs + (yw_k << 1),   .n = y2size,    .cap = y2size};
    n0 = (bigInt){.limbs = w->limbs,                 .n = w0size,    .cap = w0size};
    n1 = (bigInt){.limbs = w->limbs + yw_k,          .n = w1size,    .cap = w1size};
    n2 = (bigInt){.limbs = w->limbs + (yw_k << 1),   .n = w2size,    .cap = w2size};

    /* --------------------- 2b. Evaluation & Pointwise Multiplication ---------------------
    *   +) pOuter = m0 + m2                         | +) qOuter = n0 + n2
    *   +) p(0)   = m0          (NO FULL TEMPORARY) | +) q(0)   = n0          (NO FULL TEMPORARY)
    *   +) p(1)   = pOuter + m1                     | +) q(1)   = qOuter + n1
    *   +) p(-1)  = pOuter - m1                     | +) q(-1)  = qOuter - n1
    *   +) p(-2)  = 2*(p(-1) + m2) - m0             | +) q(-2)  = 2*(q(-1) + n2) - n0
    *   +) p(inf) = m2          (NO FULL TEMPORARY) | +) q(inf) = n2          (NO FULL TEMPORARY) */
    // p(x) TEMPORARIES + CALCULATIONS              // q(x) TEMPORARIES + CALCULATIONS
    p_outer.cap = yw_k + 1; p1.cap = yw_k + 2;      /**/ q_outer.cap = yw_k + 1; q1.cap = yw_k + 2;
    p_neg1.cap = yw_k + 1; p_neg2.cap = yw_k + 2;   /**/ q_neg1.cap = yw_k + 1; q_neg2.cap = yw_k + 2;
    __BIGINT_ADD_WC__(&p_outer, &m0, &m2);          __BIGINT_ADD_WC__(&q_outer, &n0, &n2);
    __BIGINT_ADD_WC__(&p1, &p_outer, &m1);          __BIGINT_ADD_WC__(&q1, &q_outer, &n1);
    __BIGINT_SUB_SAW__(&p_neg1, &p_outer, &m1);     __BIGINT_SUB_SAW__(&q_neg1, &q_outer, &n1);
    __BIGINT_ADD_SAW__(&p_neg2, &p_neg1, &m2);      __BIGINT_ADD_SAW__(&q_neg2, &q_neg1, &n2);
    __BIGINT_INTERNAL_LSHIFT__(&p_neg2, 1);         __BIGINT_INTERNAL_LSHIFT__(&q_neg2, 1);
    __BIGINT_SUB_SAW__(&p_neg2, &p_neg2, &m0);      __BIGINT_SUB_SAW__(&q_neg2, &q_neg2, &n0);
    /* ------------ POINT-WISE MULTIPLICATION ------------
    *   +) r(0)   = p(0)   * q(0)       ---> Cap: 2k
    *   +) r(1)   = p(1)   * q(1)       ---> Cap: 2k + 4 (original) --> 2k + 8 (interpolation - r1)
    *   +) r(-1)  = p(-1)  * q(-1)      ---> Cap: 2k + 2 (originxal) --> 2k + 7 (interpolation - r2)
    *   +) r(-2)  = p(-2)  * q(-2)      ---> Cap: 2k + 4 (original) --> 2k + 7 (interpolation - r3)
    *   +) r(inf) = p(inf) * q(inf)     ---> Cap: 2k (original)
    */
    __BIHEAP_TOOM_3__(&m0, &n0, &r0, &echeck); HEAP_FOOM(echeck, early_free, early_cnt);
    __BIHEAP_TOOM_3__(&p1, &q1, &r1, &echeck); HEAP_FOOM(echeck, early_free, early_cnt);
    __BIHEAP_TOOM_3__(&p_neg1, &q_neg1, &r_neg1, &echeck); HEAP_FOOM(echeck, early_free, early_cnt);
    __BIHEAP_TOOM_3__(&p_neg2, &q_neg2, &r_neg2, &echeck); HEAP_FOOM(echeck, early_free, early_cnt);
    __BIHEAP_TOOM_3__(&m2, &n2, &rinf, &echeck); HEAP_FOOM(echeck, early_free, early_cnt);
    r1.sign = p1.sign * q1.sign; /**/ r_neg1.sign = p_neg1.sign * q_neg1.sign; /**/ r_neg2.sign = p_neg2.sign * q_neg2.sign;
    __BIGINT_INTERNAL_ENSCAP__(&r1, (yw_k << 1) + 8); HEAP_FOOM(echeck, early_free, early_cnt);
    __BIGINT_INTERNAL_ENSCAP__(&r_neg2, (yw_k << 1) + 7); HEAP_FOOM(echeck, early_free, early_cnt);
    __BIGINT_INTERNAL_ENSCAP__(&r_neg1, (yw_k << 1) + 7); HEAP_FOOM(echeck, early_free, early_cnt);
    
    /* -------------------------- 3b. INTERPOLATION & RECOMPOSITION -------------------------- */
    /* r3 = 2k + 5 */ __BIGINT_SUB_SAW__(&r_neg2, &r_neg2, &r1); __BIGINT_DIV3__(&r_neg2);
    /* r1 = 2k + 5 */ __BIGINT_SUB_SAW__(&r1, &r1, &r_neg1); __BIGINT_INTERNAL_RSHIFT__(&r1, 1);
    /* r2 = 2k + 3 */ __BIGINT_SUB_SAW__(&r_neg1, &r_neg1, &r0);
    /* r3 = 2k + 7 */ __BIGINT_SUB_SAW__(&r_neg2, &r_neg1, &r_neg2);
    __BIGINT_INTERNAL_RSHIFT__(&r_neg2, 1); __BIGINT_INTERNAL_LSHIFT__(&rinf, 1);
    __BIGINT_ADD_SAW__(&r_neg2, &r_neg2, &rinf);
    /* r2 = 2k + 7 */ __BIGINT_ADD_SAW__(&r_neg1, &r_neg1, &r1);
    __BIGINT_INTERNAL_RSHIFT__(&rinf, 1); __BIGINT_SUB_SAW__(&r_neg1, &r_neg1, &rinf);
    /* r1 = 2k + 8 */ __BIGINT_SUB_SAW__(&r1, &r1, &r_neg2);
    /* ------------------ RECOMPOSITION ------------------ */ 
    BIHEAP_FRET(yw_tres, y->n + w->n, echeck, early_free, early_cnt);
    __BIGINT_ADD_SHIFT__(&yw_tres, &rinf, 4); __BIGINT_ADD_SHIFT__(&yw_tres, &r_neg2, 3);
    __BIGINT_ADD_SHIFT__(&yw_tres, &r_neg1, 2); __BIGINT_ADD_SHIFT__(&yw_tres, &r1, 1);
    __BIGINT_ADD_SHIFT__(&yw_tres, &r0, 0); __BIGINT_INTERNAL_MOVE__(yw_res, &yw_tres);
    _free_alloc_list(alloc_list, alloc_cnt); return BIGINT_SUCCESS;
}




/* ------- BigInt Matrix Multiplication Toom-cook 3-way ------- */
dnml_status __BIHEAP_MATMUL_TOOM4__(
    P_BIGINT x, P_BIGINT z, /**/ P_BIGINT y, P_BIGINT w,
    P_BIGINT xz_res, P_BIGINT yw_res
) { return BIGINT_SUCCESS; }




/* ------- BigInt Matrix Multiplication Toom-cook 3-way ------- */
dnml_status __BIHEAP_MATMUL_TOOM5__(
    P_BIGINT x, P_BIGINT z, /**/ P_BIGINT y, P_BIGINT w,
    P_BIGINT xz_res, P_BIGINT yw_res
) { return BIGINT_SUCCESS; }





/* ------- BigInt Matrix Multiplication Toom-cook 3-way ------- */
dnml_status __BIHEAP_MATMUL_SSA__(
    P_BIGINT x, P_BIGINT z, /**/ P_BIGINT y, P_BIGINT w,
    P_BIGINT xz_res, P_BIGINT yw_res
) {
    dnml_status echeck = BIGINT_SUCCESS;
    bigInt *early_free[8]; uint8_t early_cnt = 0;
    bigInt *alloc_list[7]; uint8_t alloc_cnt = 0;
    size_t xz_d, xz_m, xz_n, xz_k = __fft_best_metadata(x->n, z->n, &xz_d, &xz_m, &xz_n);
    size_t yw_d, yw_m, yw_n, yw_k = __fft_best_metadata(y->n, w->n, &yw_d, &yw_m, &yw_n);
    size_t max_d = max(xz_d, yw_d); size_t max_m = max(xz_m, yw_m);
    size_t max_n = max(xz_n, yw_n); size_t max_k = max(xz_k, yw_k);
    //* ================================ X and Z MULTIPLICATION PAIR ================================ *//
    /* ------------- 1a. Setup & Split ------------- */
    size_t xz_mlimbs = (xz_m + U64_BITS - 1) >> 6; /**/ size_t xz_nlimbs = (xz_n + U64_BITS) >> 6;
    size_t yw_mlimbs = (yw_m + U64_BITS - 1) >> 6; /**/ size_t yw_nlimbs = (yw_n + U64_BITS) >> 6;
    size_t max_mlimbs = max(xz_mlimbs, yw_mlimbs); /**/ size_t max_nlimbs = max(xz_nlimbs, yw_nlimbs);
    bigInt a_windows[max_d], b_windows[max_d];
    for (size_t i = 0; i < xz_d; ++i) {
        size_t a_offset = i * xz_mlimbs; size_t a_len = (a_offset < x->n) ? min(xz_mlimbs, x->n - a_offset) : 0;
        size_t b_offset = i * xz_mlimbs; size_t b_len = (b_offset < z->n) ? min(xz_mlimbs, z->n - b_offset) : 0;
        a_windows[i] = (bigInt){ .limbs = x->limbs + a_offset, .n = a_len, .cap = xz_mlimbs, .sign = 1 };
        b_windows[i] = (bigInt){ .limbs = z->limbs + b_offset, .n = b_len, .cap = xz_mlimbs, .sign = 1 };
    }

    /* ---------- 2a+3a. PRE-EVALUATION NEGACYCLIC OPTIMIZATION + FFT EVALUATION ---------- */
    bigInt eval_a[max_d], eval_b[max_d]; // Each of the D elements requires an array of limbs of size 'nlimbs'
    BIHEAP_FTEMP(lo_buf, max_nlimbs + 1, echeck, early_free, early_cnt, alloc_list, alloc_cnt);
    BIHEAP_FTEMP(hi_buf, max_nlimbs + 1, echeck, early_free, early_cnt, alloc_list, alloc_cnt); 
    BIHEAP_FTEMP(tbuf,   max_nlimbs + 2, echeck, early_free, early_cnt, alloc_list, alloc_cnt); // Will be used later in recomposition
    BIHEAP_FTEMP(usave,  max_nlimbs + 1, echeck, early_free, early_cnt, alloc_list, alloc_cnt);
    size_t psi_step = xz_n >> xz_k; // Exponent step for negacyclic weight: ψ = 2^(n/D)

    // 2. Looping over D of each windows and Pre-scale them through Negacyclic Convolutions
    // Allocating two flat, contiguous memory blocks for all D windows at once
    // (improve Cache Locality and Prefetch Efficiency)
    size_t total_limbs_needed = (max_nlimbs + 1) << max_k; // d(nlimbs + 1)
    BIHEAP_FTEMP(flat_evala, total_limbs_needed, echeck, early_free, early_cnt, alloc_list, alloc_cnt);
    BIHEAP_FTEMP(flat_evalb, total_limbs_needed, echeck, early_free, early_cnt, alloc_list, alloc_cnt);
    for (size_t i = 0; i < xz_d; ++i) {
        eval_a[i] = (bigInt){ .limbs = flat_evala.limbs + (i * (xz_nlimbs + 1)), .n = xz_nlimbs, .cap = xz_nlimbs + 1, .sign = 1 };
        eval_b[i] = (bigInt){ .limbs = flat_evalb.limbs + (i * (xz_nlimbs + 1)), .n = xz_nlimbs, .cap = xz_nlimbs + 1, .sign = 1 };
        // Perform cyclic shifts straight into these perfectly localized targets
        size_t weight_exp = i * psi_step;
        __cyclic_shift_mod(&eval_a[i], &a_windows[i], lo_buf.limbs, hi_buf.limbs, weight_exp, xz_n, xz_nlimbs);
        __cyclic_shift_mod(&eval_b[i], &b_windows[i], lo_buf.limbs, hi_buf.limbs, weight_exp, xz_n, xz_nlimbs);
    }
    // 3. Execute forward Cooley-Tukey NTT to move elements into frequency domain
    _biheap_ctk_fft(eval_a, tbuf.limbs, usave.limbs, lo_buf.limbs, hi_buf.limbs, xz_d, xz_k, xz_n, xz_nlimbs);
    _biheap_ctk_fft(eval_b, tbuf.limbs, usave.limbs, lo_buf.limbs, hi_buf.limbs, xz_d, xz_k, xz_n, xz_nlimbs);

    /* ----------- 4a. RECURSIVE POINT-WISE MULTIPLICATIONS OF RING ELEMENTS ----------- */
    // Output of multiplication is up to 2 * nlimbs long before reduction
    BIHEAP_FTEMP(prod_tmp, (max_nlimbs << 1) + 2, echeck, early_free, early_cnt, alloc_list, alloc_cnt); prod_tmp.cap = (xz_nlimbs << 1) + 2;
    for (size_t i = 0; i < xz_d; ++i) {
        // Recursive Multiply step: eval_a[i] * eval_b[i]
        __BIHEAP_FFT__(&eval_a[i], &eval_b[i], &prod_tmp, &echeck); HEAP_FOOM(echeck, early_free, early_cnt);
        __reduce_mod_fermat(&eval_a[i], &prod_tmp, lo_buf.limbs, hi_buf.limbs, xz_n, xz_nlimbs);
        // Immediate Ring Reduction: Reduce back to Z/(2^n + 1)Z
    }

    /* ---------------- 5a+6a. INTERPOLATION & RECOMPOSITION ---------------- */
    // Execute Inverse NTT (eval_a now holds the point-wise products)
    _biheap_ctk_ifft(eval_a, tbuf.limbs, usave.limbs, lo_buf.limbs, hi_buf.limbs, xz_d, xz_k, xz_n, xz_nlimbs);
    for (size_t i = 0; i < xz_d; ++i) {
        // Post-FFT Unscaling: Multiply by inverse negacyclic weights ψ^(-i)
        size_t wexp = i * psi_step;
        size_t iwexp = ((xz_n << 1) - wexp) % (xz_n << 1); // Exponent cycle of 2n;
        __cyclic_shift_mod(&eval_a[i], &eval_a[i], lo_buf.limbs, hi_buf.limbs, iwexp, xz_n, xz_nlimbs);
    }
    // Clear and prepare your final destination container
    BIHEAP_FRET(xz_tres, x->n + z->n, echeck, early_free, early_cnt); xz_tres.cap = x->n + z->n;
    tbuf = (bigInt){ .limbs = tbuf.limbs, .n = 0, .cap = xz_nlimbs + 2, .sign = 1 };
    memset(xz_tres.limbs, 0, max(x->n + z->n, y->n + w->n) * U64_BYTES);
    // Overlap-Add Recomposition into Final ab PRODUCT
    for (size_t i = 0; i < xz_d; ++i) {
        if (!eval_a[i].n) continue;
        size_t total_bshift = i * xz_m;
        size_t mlimb_shift = total_bshift >> 6;
        size_t m_bshift = total_bshift & 63;

        // Actual recomposition operations
        if (!m_bshift) __BIGINT_ADD_SHIFT__(&xz_tres, &eval_a[i], mlimb_shift);
        else {
            // Copy + Shift fused together
            // This part only handles the modularly-reduced single machine-word bit shifts
            tbuf.n = eval_a[i].n;
            uint64_t discarded_bits = 0, mask = U64_BITS - m_bshift;
            for (size_t j = 0; j < tbuf.n; ++j) {
                uint64_t tmp = eval_a[i].limbs[j];
                tbuf.limbs[j] = (tmp << m_bshift) | discarded_bits;
                discarded_bits = tmp >> mask;
            } if (discarded_bits) tbuf.limbs[tbuf.n++] = discarded_bits;
            __BIGINT_ADD_SHIFT__(&xz_tres, &tbuf, mlimb_shift); // Addition with actual limb shifts
        }
    } __BIGINT_INTERNAL_TRIM_LZ__(&xz_tres); __BIGINT_INTERNAL_MOVE__(xz_res, &xz_tres);





    //* ================================ Y and W MULTIPLICATION PAIR ================================ *//
    /* ----------------- 1b+2b. Window Splits + Pre-evaluation Negacylic Ooptimization ----------------- */
    for (size_t i = 0; i < yw_d; ++i) {
        size_t a_offset = i * yw_mlimbs; size_t a_len = (a_offset < y->n) ? min(yw_mlimbs, y->n - a_offset) : 0;
        size_t b_offset = i * yw_mlimbs; size_t b_len = (b_offset < w->n) ? min(yw_mlimbs, w->n - b_offset) : 0;
        a_windows[i] = (bigInt){ .limbs = y->limbs + a_offset, .n = a_len, .cap = yw_mlimbs, .sign = 1 };
        b_windows[i] = (bigInt){ .limbs = w->limbs + b_offset, .n = b_len, .cap = yw_mlimbs, .sign = 1 };
    } psi_step = yw_n >> yw_k; // Exponent step for negacylic weight: ψ = 2^(n/D)

    // 2. Looping over D of each windows and Pre-scale them through Negacyclic Convolutions
    // Allocating two flat, contiguous memory blocks for all D windows at once
    // (improve Cache Locality and Prefetch Efficiency)
    for (size_t i = 0; i < xz_d; ++i) {
        eval_a[i] = (bigInt){ .limbs = flat_evala.limbs + (i * (yw_nlimbs + 1)), .n = yw_nlimbs, .cap = yw_nlimbs + 1, .sign = 1 };
        eval_b[i] = (bigInt){ .limbs = flat_evalb.limbs + (i * (yw_nlimbs + 1)), .n = yw_nlimbs, .cap = yw_nlimbs + 1, .sign = 1 };
        // Perform cyclic shifts straight into these perfectly localized targets
        size_t weight_exp = i * psi_step;
        __cyclic_shift_mod(&eval_a[i], &a_windows[i], lo_buf.limbs, hi_buf.limbs, weight_exp, yw_n, yw_nlimbs);
        __cyclic_shift_mod(&eval_b[i], &b_windows[i], lo_buf.limbs, hi_buf.limbs, weight_exp, yw_n, yw_nlimbs);
    }

    /* ------------ 3b+4b. FFT Evaluation + Point-Wise Multiplication of Ring Elements ------------ */
    // 3. Execute forward Cooley-Tukey NTT to move elements into frequency domain
    _biheap_ctk_fft(eval_a, tbuf.limbs, usave.limbs, lo_buf.limbs, hi_buf.limbs, yw_d, yw_k, yw_n, yw_nlimbs);
    _biheap_ctk_fft(eval_b, tbuf.limbs, usave.limbs, lo_buf.limbs, hi_buf.limbs, yw_d, yw_k, yw_n, yw_nlimbs);
    // Output of multiplication is up to 2 * nlimbs long before reduction
    prod_tmp.cap = (yw_nlimbs << 1) + 2;
    for (size_t i = 0; i < xz_d; ++i) {
        // Recursive Multiply step: eval_a[i] * eval_b[i]
        __BIHEAP_FFT__(&eval_a[i], &eval_b[i], &prod_tmp, &echeck); HEAP_FOOM(echeck, early_free, early_cnt);
        __reduce_mod_fermat(&eval_a[i], &prod_tmp, lo_buf.limbs, hi_buf.limbs, yw_n, yw_nlimbs);
        // Immediate Ring Reduction: Reduce back to Z/(2^n + 1)Z
    }

    /* ---------------- 5a+6a. INTERPOLATION & RECOMPOSITION ---------------- */
    // Execute Inverse NTT (eval_a now holds the point-wise products)
    _biheap_ctk_ifft(eval_a, tbuf.limbs, usave.limbs, lo_buf.limbs, hi_buf.limbs, yw_d, yw_k, yw_n, yw_nlimbs);
    for (size_t i = 0; i < yw_d; ++i) {
        // Post-FFT Unscaling: Multiply by inverse negacyclic weights ψ^(-i)
        size_t wexp = i * psi_step;
        size_t iwexp = ((yw_n << 1) - wexp) % (yw_n << 1); // Exponent cycle of 2n;
        __cyclic_shift_mod(&eval_a[i], &eval_a[i], lo_buf.limbs, hi_buf.limbs, iwexp, yw_n, yw_nlimbs);
    }
    // Clear and prepare your final destination container
    BIHEAP_FRET(yw_tres, y->n + w->n, echeck, early_free, early_cnt); yw_tres.cap = y->n + w->n;
    memset(yw_tres.limbs, 0, yw_tres.cap * U64_BYTES);
    tbuf = (bigInt){ .limbs = tbuf.limbs, .n = 0, .cap = yw_nlimbs + 2, .sign = 1 };
    // Overlap-Add Recomposition into Final ab PRODUCT
    for (size_t i = 0; i < yw_d; ++i) {
        if (!eval_a[i].n) continue;
        size_t total_bshift = i * yw_n;
        size_t mlimb_shift = total_bshift >> 6;
        size_t m_bshift = total_bshift & 63;

        // Actual recomposition operations
        if (!m_bshift) __BIGINT_ADD_SHIFT__(&yw_tres, &eval_a[i], mlimb_shift);
        else {
            // Copy + Shift fused together
            // This part only handles the modularly-reduced single machine-word bit shifts
            tbuf.n = eval_a[i].n;
            uint64_t discarded_bits = 0, mask = U64_BITS - m_bshift;
            for (size_t j = 0; j < tbuf.n; ++j) {
                uint64_t tmp = eval_a[i].limbs[j];
                tbuf.limbs[j] = (tmp << m_bshift) | discarded_bits;
                discarded_bits = tmp >> mask;
            } if (discarded_bits) tbuf.limbs[tbuf.n++] = discarded_bits;
            __BIGINT_ADD_SHIFT__(&yw_tres, &tbuf, mlimb_shift); // Addition with actual limb shifts
        }
    } __BIGINT_INTERNAL_TRIM_LZ__(&yw_tres); __BIGINT_INTERNAL_MOVE__(yw_res, &yw_tres); 
    _free_alloc_list(alloc_list, alloc_cnt); return BIGINT_SUCCESS;
}
