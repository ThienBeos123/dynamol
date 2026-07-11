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




#include "_mv_matmul_.h"
#include <debug_util.h>
#include "../../util/aconv_macros.h"
#include "../mul_fft.c"

/** ----------- Matrix-Vector Multiplication Linear Combination -----------
 * THIS FILE CONTAINS THE FOLLOWING ALGORITHMS FOR MATRIX MULTIPLICATION:
 *
 *      - Toom-cook 3-way 
 *      - Toom-cook 4-way
 *      - Toom-cook 5-way
 *      - Schonhage-Strassen
 *
 * In which they would be structurally modify to accompany the multiplications
 * of a single linear-combination pair of the form xz + yw (as utilized by hgcd).
 * This file is a specialization version of the generic, balance-assuming version in 
 * "_matmul_fft.c", where algorithms here assume arguments of x and z are unbalanced,
 * with a size difference magnitudes of larger than 2 (>2x), as well as y and w
 */
/* ------------ Sizing Function ------------ */
size_t __SYM_MATMUL_TOOM3_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size) {
    // Metadata slices pre-calculations - XZ
    size_t xz_Bsize = min(x_size, z_size); // Beta size lol
    size_t xz_Asize = max(x_size, z_size); // Alpha chad size lol
    size_t xz_slice = xz_Bsize, xz_lslice = xz_Asize % xz_Bsize;
    // Metadata slices pre-calculations - YW
    size_t yw_Bsize = min(y_size, w_size); // Beta size lol
    size_t yw_Asize = max(y_size, w_size); // Alpha chad size lol
    size_t yw_slice = yw_Bsize, yw_lslice = yw_Asize % yw_Bsize;
    // Maximum k-values calculation + Splitting Sizes
    size_t xz_k = (max(xz_slice, xz_Bsize) + 2) / 3;
    size_t yw_k = (max(yw_slice, yw_Bsize) + 2) / 3;
    size_t max_k = max(xz_k, yw_k);
    /* -------- 1a. Setup and Splitting Metadatas -------- */
    size_t x2size = (xz_slice > (xz_k << 1)) ? (xz_slice - (xz_k << 1)) : 0;
    size_t z2size = (xz_Bsize > (xz_k << 1)) ? (xz_Bsize - (xz_k << 1)) : 0;
    size_t y2size = (yw_slice > (yw_k << 1)) ? (yw_slice - (yw_k << 1)) : 0;
    size_t w2size = (yw_Bsize > (yw_k << 1)) ? (yw_Bsize - (yw_k << 1)) : 0;
    size_t max_m2size = max(x2size, y2size); size_t max_n2size = max(z2size, w2size);
    // Raw buffer lengths calculations
    size_t eval_bufs = (max_k << 3) + 12; size_t ptmul_bufs = ((max_k << 3) + (max_m2size + max_n2size) + 22);
    size_t fres_size = max(xz_Bsize + xz_slice, yw_Bsize + yw_slice); size_t xz_tres_size = max(x_size + z_size, y_size + w_size);
    size_t max_fcall = max(__BIGINT_TOOM_3_WS__(max_k + 2, max_k + 2), __BIGINT_TOOM_3_WS__(max_m2size, max_n2size));
    if (xz_lslice > BIGINT_SCHOOLBOOK) max_fcall = max(max_fcall, __BIGINT_ASYM_MUL_WS__(xz_Bsize, xz_lslice));
    if (yw_lslice > BIGINT_SCHOOLBOOK) max_fcall = max(max_fcall, __BIGINT_ASYM_MUL_WS__(yw_Bsize, yw_lslice));
    return ((eval_bufs + ptmul_bufs + fres_size) + max_fcall) + xz_tres_size; // Follows the path of the largest input size
}
size_t __SYM_MATMUL_TOOM4_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size) { return 0; }
size_t __SYM_MATMUL_TOOM5_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size) { return 0; }
size_t __SYM_MATMUL_SSA_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size) {
    // Metadata slices pre-calculations - XZ
    size_t xz_Bsize = min(x_size, z_size); // Beta size lol
    size_t xz_Asize = max(x_size, z_size); // Alpha chad size lol
    size_t xz_slice = xz_Bsize, xz_lslice = xz_Asize % xz_Bsize;
    // Metadata slices pre-calculations - YW
    size_t yw_Bsize = min(y_size, w_size); // Beta size lol
    size_t yw_Asize = max(y_size, w_size); // Alpha chad size lol
    size_t yw_slice = yw_Bsize, yw_lslice = yw_Asize % yw_Bsize;
    // Pre-oepration Allocation Size Calculations
    size_t xz_n, xz_k = __fft_best_metadata(xz_Bsize, xz_slice, NULL, NULL, &xz_n);
    size_t yw_n, yw_k = __fft_best_metadata(yw_Bsize, yw_slice, NULL, NULL, &yw_n);
    size_t yw_nlimbs = (max(xz_n, yw_n) + U64_BITS) >> 6;
    size_t xz_nlimbs = (max(xz_n, yw_n) + U64_BITS) >> 6;
    size_t max_k = max(xz_k, yw_k); /**/ size_t max_nlimbs = max(xz_nlimbs, yw_nlimbs);
    // Raw buffer size requirements
    size_t tmp_bufs = (max_nlimbs << 2) + 5;
    size_t flat_bufs = ((max_nlimbs + 1) << max_k) << 1; // d(nlimbs + 1) * 2;
    size_t tres_size = max(xz_Bsize + xz_slice, yw_Bsize + yw_slice);
    size_t acum_tres_size = max(x_size + z_size, y_size + w_size); /**/ size_t ptmp_size = (max_nlimbs << 1) + 2;
    size_t max_fcall = __BIGINT_FFT_WS__(max_nlimbs, max_nlimbs); 
    size_t xz_scaled = ___schoolbook_scale(xz_Bsize), yw_scaled = ___schoolbook_scale(yw_Bsize);
    if (xz_lslice > xz_scaled) max_fcall = max(max_fcall, __BIGINT_ASYM_MUL_WS__(xz_Bsize, xz_lslice));
    if (yw_lslice > yw_scaled) max_fcall = max(max_fcall, __BIGINT_ASYM_MUL_WS__(yw_Bsize, yw_lslice));
    return tmp_bufs + flat_bufs + tres_size + ptmp_size + acum_tres_size + max_fcall;
}



/* ------- BigInt Matrix Multiplication Toom-cook 3-way ------- */
dnml_status __SYM_MATMUL_TOOM3__(
    P_BIGINT x, P_BIGINT z, /**/ P_BIGINT y, P_BIGINT w, 
    P_BIGINT xz_res, P_BIGINT yw_res, calc_ctx *toom_ctx
) {
    // Metadata slices pre-calculations - XZ
    size_t xz_Bsize = min(x->n, z->n); // Beta size lol
    size_t xz_Asize = max(x->n, z->n); // Alpha chad size lol
    size_t xz_splits = (xz_Asize / xz_Bsize) + !!(xz_Asize % xz_Bsize);
    size_t xz_slice = xz_Bsize, xz_lslice = xz_Asize % xz_Bsize;
    bigInt *xz_alpha = (xz_Asize == x->n) ? x : z; /**/ bigInt *xz_beta = (xz_Bsize == z->n) ? z : x;
    // Metadata slices pre-calculations - YW
    size_t yw_Bsize = min(y->n, w->n); // Beta size lol
    size_t yw_Asize = max(y->n, w->n); // Alpha chad size lol
    size_t yw_splits = (yw_Asize / yw_Bsize) + !!(yw_Asize % yw_Bsize);
    size_t yw_slice = yw_Bsize, yw_lslice = yw_Asize % yw_Bsize;
    bigInt *yw_alpha = (yw_Asize == y->n) ? y : w; /**/ bigInt *yw_beta = (yw_Bsize == w->n) ? w : y;

    //* ================== PRE-OPERATION ALLOCATIONS ================== *//
    dnml_status echeck; size_t toom_mark = scratch_mark(toom_ctx);
    size_t xz_k = (max(xz_slice, xz_Bsize) + 2) / 3;
    size_t yw_k = (max(yw_slice, yw_Bsize) + 2) / 3;
    size_t max_k = max(xz_k, yw_k);
    /* -------- 1a. Setup and Splitting Metadatas -------- */
    size_t x0size = min(xz_k, xz_slice); size_t z0size = min(xz_k, xz_Bsize);
    size_t x1size = (xz_slice > xz_k) ? xz_slice - xz_k : 0; // Maximum = k
    size_t z1size = (xz_Bsize > xz_k) ? xz_Bsize - xz_k : 0; // Maximum = k
    size_t x2size = (xz_slice > (xz_k << 1)) ? (xz_slice - (xz_k << 1)) : 0;
    size_t z2size = (xz_Bsize > (xz_k << 1)) ? (xz_Bsize - (xz_k << 1)) : 0;
    size_t y0size = min(yw_k, yw_slice); size_t w0size = min(yw_k, yw_Bsize);
    size_t y1size = (yw_slice > yw_k) ? yw_slice - yw_k : 0; // Maximum = k
    size_t w1size = (yw_Bsize > yw_k) ? yw_Bsize - yw_k : 0; // Maximum = k
    size_t y2size = (yw_slice > (yw_k << 1)) ? (yw_slice - (yw_k << 1)) : 0;
    size_t w2size = (yw_Bsize > (yw_k << 1)) ? (yw_Bsize - (yw_k << 1)) : 0;
    size_t max_m2size = max(x2size, y2size); size_t max_n2size = max(z2size, w2size);
    bigInt m0 = {0}; bigInt m1 = {0}; bigInt m2 = {0}; /**/ bigInt n0 = {0}; bigInt n1 = {0}; bigInt n2 = {0};
    /* --------- 2a. Evaluation & Point-wise Multiplication ---------
    *   +) pOuter = m0 + m2                                        | +) qOuter = n0 + n2
    *   +) p(0)   = m0          (NO FULL TEMPORARY)                | +) q(0)   = n0          (NO FULL TEMPORARY)
    *   +) p(1)   = pOuter + m1                                    | +) q(1)   = qOuter + n1
    *   +) p(-1)  = pOuter - m1                                    | +) q(-1)  = qOuter - n1
    *   +) p(-2)  = 2*(p(-1) + m2) - m0                            | +) q(-2)  = 2*(q(-1) + n2) - n0
    *   +) p(inf) = m2          (NO FULL TEMPORARY)                | +) q(inf) = n2          (NO FULL TEMPORARY) */
    // p(x) TEMPORARIES                                            // q(x) TEMPORARIES
    BIGINT_FTEMP(p_outer, max_k + 1, toom_ctx, toom_mark, echeck); BIGINT_FTEMP(q_outer, max_k + 1, toom_ctx, toom_mark, echeck);
    BIGINT_FTEMP(p1,      max_k + 2, toom_ctx, toom_mark, echeck); BIGINT_FTEMP(q1,      max_k + 2, toom_ctx, toom_mark, echeck);
    BIGINT_FTEMP(p_neg1,  max_k + 1, toom_ctx, toom_mark, echeck); BIGINT_FTEMP(q_neg2,  max_k + 2, toom_ctx, toom_mark, echeck);
    BIGINT_FTEMP(p_neg2,  max_k + 2, toom_ctx, toom_mark, echeck); BIGINT_FTEMP(q_neg1,  max_k + 1, toom_ctx, toom_mark, echeck);
    /* ------------ POINT-WISE MULTIPLICATION ------------
    *   +) r(0)   = p(0)   * q(0)       ---> Cap: 2k
    *   +) r(1)   = p(1)   * q(1)       ---> Cap: 2k + 4 (original) --> 2k + 8 (interpolation - r1)
    *   +) r(-1)  = p(-1)  * q(-1)      ---> Cap: 2k + 2 (originxal) --> 2k + 7 (interpolation - r2)
    *   +) r(-2)  = p(-2)  * q(-2)      ---> Cap: 2k + 4 (original) --> 2k + 7 (interpolation - r3)
    *   +) r(inf) = p(inf) * q(inf)     ---> Cap: 2k (original)
    */
    BIGINT_FTEMP(r_neg1, (max_k << 1) + 7,   toom_ctx, toom_mark, echeck);
    BIGINT_FTEMP(r1,     (max_k << 1) + 8,   toom_ctx, toom_mark, echeck);
    BIGINT_FTEMP(r0,     (max_k << 1),       toom_ctx, toom_mark, echeck);
    BIGINT_FTEMP(r_neg2, (max_k << 1) + 7,   toom_ctx, toom_mark, echeck);
    BIGINT_FTEMP(rinf, max_m2size + max_n2size, toom_ctx, toom_mark, echeck);
    /* ---------------- 3a. INTERPOLATION & RECOMPOSITION ---------------- */
    BIGINT_FTEMP(final_res, max(xz_Bsize + xz_slice, yw_Bsize + yw_slice), toom_ctx, toom_mark, echeck); 
    BIGINT_FTEMP(acum_tres, max(x->n + z->n, y->n + w->n), toom_ctx, toom_mark, echeck); acum_tres.cap = x->n + z->n;
    final_res.cap = xz_Bsize + xz_slice; /**/ memset(acum_tres.limbs, 0, acum_tres.cap * U64_BYTES);




    //* ============== X and Z MULTIPLICATION PAIR - LOOP OF SLICES ============== *//
    bigInt alpha_window = {0}; size_t offset = 0;
    for (size_t i = 0; i < xz_splits; ++i) {
        size_t curr_slice = xz_slice; if (unlikely(i = xz_splits - 1) && xz_lslice) curr_slice = xz_lslice;
        alpha_window = (bigInt){.limbs = xz_alpha->limbs + offset, .n = curr_slice, .cap = curr_slice, .sign = 1};
        if ((xz_Bsize <= BIGINT_SCHOOLBOOK || curr_slice <= BIGINT_SCHOOLBOOK)) { 
            __BIGINT_SCHOOLBOOK__(xz_beta, &alpha_window, &final_res);
            __BIGINT_ADD_SHIFT__(&acum_tres, &final_res, offset); continue; // (tmp_res <<<= xz_slice) + tmp
        } 
        else if (xz_Bsize != curr_slice) {
            __BIGINT_ASYM_MUL_DISP__(xz_beta, &alpha_window, &final_res, toom_ctx, &echeck); SCRATCH_FOVF(echeck, toom_ctx, toom_mark);
            __BIGINT_ADD_SHIFT__(&acum_tres, &final_res, offset); continue; // (tmp_res <<<= slice) + tmp
        }
        // Recalculation of Metadta on xz_lslice's iteration
        if (i == xz_splits - 1 && xz_lslice) {
            xz_k = (max(curr_slice, xz_Bsize) + 2) / 3; // Recalculating xz_k
            x0size = min(xz_k, curr_slice); /**/ x1size = (curr_slice > (xz_k)) ? (curr_slice - (xz_k)) : 0;
            x2size = (curr_slice > (xz_k << 1)) ? (curr_slice - (xz_k << 1)) : 0;
        } 
        
        /* ------- 1a. Setup & Splitting ------- */
        m0 = (bigInt){.limbs = alpha_window.limbs,                  .n = x0size,   .cap = x0size};
        m1 = (bigInt){.limbs = alpha_window.limbs + xz_k,           .n = x1size,   .cap = x1size};
        m2 = (bigInt){.limbs = alpha_window.limbs + (xz_k << 1),    .n = x2size,   .cap = x2size};
        n0 = (bigInt){.limbs = xz_beta->limbs,                      .n = z0size,   .cap = z0size};
        n1 = (bigInt){.limbs = xz_beta->limbs + xz_k,               .n = z1size,   .cap = z1size};
        n2 = (bigInt){.limbs = xz_beta->limbs + (xz_k << 1),        .n = z2size,   .cap = z2size};

        /* ------------ 2a. Evaluation & Point-wise Multiplication ------------ */
        p_outer.cap = xz_k + 1; p1.cap = xz_k + 2;          q_outer.cap = xz_k + 1; q1.cap = xz_k + 2;
        p_neg1.cap = xz_k + 1; p_neg2.cap = xz_k + 1;       q_neg1.cap = xz_k + 1; q_neg2.cap = xz_k + 2;
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
        r0.cap = (xz_k << 1); /**/ r1.cap = (xz_k << 1) + 8; /**/ r_neg1.cap = (xz_k << 1) + 7;
        r_neg2.cap = (xz_k << 1) + 7; /**/ rinf.cap = x2size + z2size;
        __BIGINT_TOOM_3__(&m0, &n0, &r0, toom_ctx, &echeck); SCRATCH_FOVF(echeck, toom_ctx, toom_mark);
        __BIGINT_TOOM_3__(&p1, &q1, &r1, toom_ctx, &echeck); SCRATCH_FOVF(echeck, toom_ctx, toom_mark);
        __BIGINT_TOOM_3__(&p_neg1, &q_neg1, &r_neg1, toom_ctx, &echeck); SCRATCH_FOVF(echeck, toom_ctx, toom_mark);
        __BIGINT_TOOM_3__(&p_neg2, &q_neg2, &r_neg2, toom_ctx, &echeck); SCRATCH_FOVF(echeck, toom_ctx, toom_mark);
        __BIGINT_TOOM_3__(&m2, &n2, &rinf, toom_ctx, &echeck); SCRATCH_FOVF(echeck, toom_ctx, toom_mark);
        r1.sign = p1.sign * q1.sign; /**/ r_neg1.sign = p_neg1.sign * q_neg1.sign; /**/ r_neg2.sign = p_neg2.sign * q_neg2.sign;

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
        /* ------------------ RECOMPOSITION ------------------ */ final_res.n = 0; final_res.sign = 1;
        __BIGINT_ADD_SHIFT__(&final_res, &rinf, 4); __BIGINT_ADD_SHIFT__(&final_res, &r_neg2, 3);
        __BIGINT_ADD_SHIFT__(&final_res, &r_neg1, 2); __BIGINT_ADD_SHIFT__(&final_res, &r1, 1);
        __BIGINT_ADD_WC__(&final_res, &final_res, &r0); __BIGINT_ADD_SHIFT__(&acum_tres, &final_res, offset); offset += curr_slice;
    } __BIGINT_INTERNAL_COPY__(xz_res, &acum_tres);




    //* ============== Y and W MULTIPLICATION PAIR - LOOP OF SLICES ============== *//
    acum_tres.cap = y->n + w->n; memset(acum_tres.limbs, 0, acum_tres.cap * U64_BYTES);
    offset = 0; alpha_window = (bigInt){0}; final_res.cap = yw_Bsize + yw_slice;
    for (size_t i = 0; i < yw_splits; ++i) {
        size_t curr_slice = yw_slice; if (unlikely(i = yw_splits - 1) && yw_lslice) curr_slice = yw_lslice;
        alpha_window = (bigInt){.limbs = yw_alpha->limbs + offset, .n = curr_slice, .cap = curr_slice, .sign = 1};
        if ((yw_Bsize <= BIGINT_SCHOOLBOOK || curr_slice <= BIGINT_SCHOOLBOOK)) { 
            __BIGINT_SCHOOLBOOK__(yw_beta, &alpha_window, &final_res);
            __BIGINT_ADD_SHIFT__(&acum_tres, &final_res, offset); continue; // (tmp_res <<<= yw_slice) + tmp
        } 
        else if (yw_Bsize != curr_slice) {
            __BIGINT_ASYM_MUL_DISP__(yw_beta, &alpha_window, &final_res, toom_ctx, &echeck); SCRATCH_FOVF(echeck, toom_ctx, toom_mark);
            __BIGINT_ADD_SHIFT__(&acum_tres, &final_res, offset); continue; // (tmp_res <<<= slice) + tmp
        }
        // Recalculation of Metadta on yw_lslice's iteration
        if (i == yw_splits - 1 && yw_lslice) {
            yw_k = (max(curr_slice, yw_Bsize) + 2) / 3; // Recalculating yw_k
            y0size = min(yw_k, curr_slice); /**/ y1size = (curr_slice > (yw_k)) ? (curr_slice - (yw_k)) : 0;
            y2size = (curr_slice > (yw_k << 1)) ? (curr_slice - (yw_k << 1)) : 0;
        } 
        
        /* ------- 1a. Setup & Splitting ------- */
        m0 = (bigInt){.limbs = alpha_window.limbs,                  .n = y0size,   .cap = y0size};
        m1 = (bigInt){.limbs = alpha_window.limbs + yw_k,           .n = y1size,   .cap = y1size};
        m2 = (bigInt){.limbs = alpha_window.limbs + (yw_k << 1),    .n = y2size,   .cap = y2size};
        n0 = (bigInt){.limbs = yw_beta->limbs,                      .n = w0size,   .cap = w0size};
        n1 = (bigInt){.limbs = yw_beta->limbs + yw_k,               .n = w1size,   .cap = w1size};
        n2 = (bigInt){.limbs = yw_beta->limbs + (yw_k << 1),        .n = w2size,   .cap = w2size};

        /* ------------ 2a. Evaluation & Point-wise Multiplication ------------ */
        p_outer.cap = yw_k + 1; p1.cap = yw_k + 2;          q_outer.cap = yw_k + 1; q1.cap = yw_k + 2;
        p_neg1.cap = yw_k + 1; p_neg2.cap = yw_k + 1;       q_neg1.cap = yw_k + 1; q_neg2.cap = yw_k + 2;
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
        r0.cap = (yw_k << 1); /**/ r1.cap = (yw_k << 1) + 8; /**/ r_neg1.cap = (yw_k << 1) + 7;
        r_neg2.cap = (yw_k << 1) + 7; /**/ rinf.cap = y2size + w2size;
        __BIGINT_TOOM_3__(&m0, &n0, &r0, toom_ctx, &echeck); SCRATCH_FOVF(echeck, toom_ctx, toom_mark);
        __BIGINT_TOOM_3__(&p1, &q1, &r1, toom_ctx, &echeck); SCRATCH_FOVF(echeck, toom_ctx, toom_mark);
        __BIGINT_TOOM_3__(&p_neg1, &q_neg1, &r_neg1, toom_ctx, &echeck); SCRATCH_FOVF(echeck, toom_ctx, toom_mark);
        __BIGINT_TOOM_3__(&p_neg2, &q_neg2, &r_neg2, toom_ctx, &echeck); SCRATCH_FOVF(echeck, toom_ctx, toom_mark);
        __BIGINT_TOOM_3__(&m2, &n2, &rinf, toom_ctx, &echeck); SCRATCH_FOVF(echeck, toom_ctx, toom_mark);
        r1.sign = p1.sign * q1.sign; /**/ r_neg1.sign = p_neg1.sign * q_neg1.sign; /**/ r_neg2.sign = p_neg2.sign * q_neg2.sign;

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
        /* ------------------ RECOMPOSITION ------------------ */ final_res.n = 0; final_res.sign = 1;
        __BIGINT_ADD_SHIFT__(&final_res, &rinf, 4); __BIGINT_ADD_SHIFT__(&final_res, &r_neg2, 3);
        __BIGINT_ADD_SHIFT__(&final_res, &r_neg1, 2); __BIGINT_ADD_SHIFT__(&final_res, &r1, 1);
        __BIGINT_ADD_WC__(&final_res, &final_res, &r0); __BIGINT_ADD_SHIFT__(&acum_tres, &final_res, offset); offset += curr_slice;
    } __BIGINT_INTERNAL_COPY__(yw_res, &acum_tres); scratch_rewind(toom_ctx, toom_mark); return BIGINT_SUCCESS;
}




/* ------- BigInt Matrix Multiplication Toom-cook 3-way ------- */
dnml_status __SYM_MATMUL_TOOM4__(
    P_BIGINT x, P_BIGINT z, /**/ P_BIGINT y, P_BIGINT w,
    P_BIGINT xz_res, P_BIGINT yw_res, calc_ctx *toom_ctx
) { return BIGINT_SUCCESS; }




/* ------- BigInt Matrix Multiplication Toom-cook 3-way ------- */
dnml_status __SYM_MATMUL_TOOM5__(
    P_BIGINT x, P_BIGINT z, /**/ P_BIGINT y, P_BIGINT w,
    P_BIGINT xz_res, P_BIGINT yw_res, calc_ctx *toom_ctx
) { return BIGINT_SUCCESS; }





/* ------- BigInt Matrix Multiplication Toom-cook 3-way ------- */
dnml_status __SYM_MATMUL_SSA__(
    P_BIGINT x, P_BIGINT z, /**/ P_BIGINT y, P_BIGINT w,
    P_BIGINT xz_res, P_BIGINT yw_res, calc_ctx *fft_ctx
) {
    size_t fft_mark = scratch_mark(fft_ctx); dnml_status echeck = BIGINT_SUCCESS;
    // Metadata slices pre-calculations - XZ
    size_t xz_Bsize = min(x->n, z->n); // Beta size lol
    size_t xz_Asize = max(x->n, z->n); // Alpha chad size lol
    size_t xz_splits = (xz_Asize / xz_Bsize) + !!(xz_Asize % xz_Bsize);
    size_t xz_slice = xz_Bsize, xz_lslice = xz_Asize % xz_Bsize;
    bigInt *xz_alpha = (xz_Asize == x->n) ? x : z; /**/ bigInt *xz_beta = (xz_Bsize == z->n) ? z : x;
    // Metadata slices pre-calculations - YW
    size_t yw_Bsize = min(y->n, w->n); // Beta size lol
    size_t yw_Asize = max(y->n, w->n); // Alpha chad size lol
    size_t yw_splits = (yw_Asize / yw_Bsize) + !!(yw_Asize % yw_Bsize);
    size_t yw_slice = yw_Bsize, yw_lslice = yw_Asize % yw_Bsize;
    bigInt *yw_alpha = (yw_Asize == y->n) ? y : w; /**/ bigInt *yw_beta = (yw_Bsize == w->n) ? w : y;
    //* ============================ PRE-OPERATION ALLOCATIONS ============================ *//
    size_t xz_d, xz_m, xz_n, xz_k = __fft_best_metadata(xz_Bsize, xz_slice, &xz_d, &xz_m, &xz_n);
    size_t yw_d, yw_m, yw_n, yw_k = __fft_best_metadata(yw_Bsize, yw_slice, &yw_d, &yw_m, &yw_n);
    size_t max_d = max(xz_d, yw_d); size_t max_m = max(xz_m, yw_m);
    size_t max_n = max(xz_n, yw_n); size_t max_k = max(xz_k, yw_k);
    /* ------------- 1. Setup & Split ------------- */
    size_t xz_mlimbs = (xz_m + U64_BITS - 1) >> 6; /**/ size_t xz_nlimbs = (xz_n + U64_BITS) >> 6;
    size_t yw_mlimbs = (yw_m + U64_BITS - 1) >> 6; /**/ size_t yw_nlimbs = (yw_n + U64_BITS) >> 6;
    size_t max_mlimbs = max(xz_mlimbs, yw_mlimbs); /**/ size_t max_nlimbs = max(xz_nlimbs, yw_nlimbs);
    bigInt a_windows[max_d], b_windows[max_d];

    /* ---------- 2+3. PRE-EVALUATION NEGACYCLIC OPTIMIZATION + FFT EVALUATION ---------- */
    bigInt eval_a[max_d], eval_b[max_d]; // Each of the D elements requires an array of limbs of size 'nlimbs'
    RAW_FTEMP(lo_buf, max_nlimbs + 1, fft_ctx, fft_mark, echeck);
    RAW_FTEMP(hi_buf, max_nlimbs + 1, fft_ctx, fft_mark, echeck); 
    RAW_FTEMP(tbuf,   max_nlimbs + 2, fft_ctx, fft_mark, echeck); // Will be used later in recomposition
    RAW_FTEMP(usave,  max_nlimbs + 1, fft_ctx, fft_mark, echeck);
    size_t psi_step = 0; // Exponent step for negacyclic weight: ψ = 2^(n/D)
    // Flat Evaluation Buffer
    size_t total_limbs_needed = (max_nlimbs + 1) << max_k; // d(nlimbs + 1)
    RAW_FTEMP(flat_evala, total_limbs_needed, fft_ctx, fft_mark, echeck);
    RAW_FTEMP(flat_evalb, total_limbs_needed, fft_ctx, fft_mark, echeck);

    /* ----------- 4+5+6. Pointwise Multiplication + Inverse Evaluation + Recomposition ----------- */
    // Output of multiplication is up to 2 * nlimbs long before reduction
    BIGINT_FTEMP(prod_tmp, (max_nlimbs << 1) + 2, fft_ctx, fft_mark, echeck);
    BIGINT_FTEMP(tmp_res, max(xz_Bsize + xz_slice, yw_Bsize + yw_slice), fft_ctx, fft_mark, echeck);
    BIGINT_FTEMP(acum_tres, max(x->n + z->n, y->n + w->n), fft_ctx, fft_mark, echeck); // Accumulator for xz_res
    memset(tmp_res.limbs, 0, tmp_res.cap * U64_BYTES); /**/ bigInt tbuf_view = {0}; // bigInt view into tbuf





    //* ========================== X and Z MULTIPLICATION PAIR - LOOP OF SLICES ========================== *//
    bigInt alpha_window = {0}; size_t offset = 0, scaled_threshold = BIGINT_SCHOOLBOOK * ___schoolbook_scale(xz_Bsize);
    for (size_t i = 0; i < xz_splits; ++i) {
        size_t curr_slice = xz_slice; if (unlikely(i = xz_splits - 1) && xz_lslice) curr_slice = xz_lslice;
        alpha_window = (bigInt){.limbs = xz_alpha->limbs + offset, .n = curr_slice, .cap = curr_slice, .sign = 1};
        if ((xz_Bsize <= scaled_threshold || curr_slice <= scaled_threshold)) { 
            __BIGINT_SCHOOLBOOK__(xz_beta, &alpha_window, &tmp_res);
            __BIGINT_ADD_SHIFT__(&acum_tres, &tmp_res, offset); continue; // (tmp_res <<<= xz_slice) + tmp
        } 
        else if (xz_Bsize != curr_slice) {
            __BIGINT_ASYM_MUL_DISP__(xz_beta, &alpha_window, &tmp_res, fft_ctx, &echeck); SCRATCH_FOVF(echeck, fft_ctx, fft_mark);
            __BIGINT_ADD_SHIFT__(&acum_tres, &tmp_res, offset); continue; // (tmp_res <<<= slice) + tmp
        }
        // Recalculation of Metadata on xz_lslice's iteration
        if (i == xz_splits - 1 && xz_lslice) {
            xz_k = __fft_best_metadata(xz_Bsize, curr_slice, &xz_d, &xz_m, &xz_n);
            xz_mlimbs = (xz_m + U64_BITS - 1) >> 6; /**/ xz_nlimbs = (xz_n + U64_BITS) >> 6;
        }
        
        /* ------------------------ 1. Setup & Split ------------------------ */
        for (size_t i = 0; i < xz_d; ++i) {
            size_t a_offset = i * xz_mlimbs; size_t a_len = (a_offset < xz_slice) ? min(xz_mlimbs, xz_slice - a_offset) : 0;
            size_t b_offset = i * xz_mlimbs; size_t b_len = (b_offset < xz_Bsize) ? min(xz_mlimbs, xz_Bsize - b_offset) : 0;
            a_windows[i] = (bigInt){ .limbs = alpha_window.limbs + a_offset, .n = a_len, .cap = xz_mlimbs, .sign = 1 };
            b_windows[i] = (bigInt){ .limbs = xz_beta->limbs + b_offset,        .n = b_len, .cap = xz_mlimbs, .sign = 1 };
        }

        /* ---------- 2a+3a. PRE-EVALUATION NEGACYCLIC OPTIMIZATION + FFT EVALUATION ---------- */
        // 2. Looping over D of each windows and Pre-scale them through Negacyclic Convolutions
        // Allocating two flat, contiguous memory blocks for all D windows at once
        // (improve Cache Locality and Prefetch Efficiency)
        psi_step = xz_n >> xz_k;
        for (size_t i = 0; i < xz_d; ++i) {
            eval_a[i] = (bigInt){ .limbs = flat_evala + (i * (xz_nlimbs + 1)), .n = xz_nlimbs, .cap = xz_nlimbs + 1, .sign = 1 };
            eval_b[i] = (bigInt){ .limbs = flat_evalb + (i * (xz_nlimbs + 1)), .n = xz_nlimbs, .cap = xz_nlimbs + 1, .sign = 1 };
            // Perform cyclic shifts straight into these perfectly localized targets
            size_t weight_exp = i * psi_step;
            __cyclic_shift_mod(&eval_a[i], &a_windows[i], lo_buf, hi_buf, weight_exp, xz_n, xz_nlimbs);
            __cyclic_shift_mod(&eval_b[i], &b_windows[i], lo_buf, hi_buf, weight_exp, xz_n, xz_nlimbs);
        }
        // 3. Execute forward Cooley-Tukey NTT to move elements into frequency domain
        _bigint_ctk_fft(eval_a, tbuf, usave, lo_buf, hi_buf, xz_d, xz_k, xz_n, xz_nlimbs);
        _bigint_ctk_fft(eval_b, tbuf, usave, lo_buf, hi_buf, xz_d, xz_k, xz_n, xz_nlimbs);

        /* ----------- 4a. RECURSIVE POINT-WISE MULTIPLICATIONS OF RING ELEMENTS ----------- */
        prod_tmp.cap = (xz_nlimbs << 1) + 2;
        for (size_t i = 0; i < xz_d; ++i) {
            // Recursive Multiply step: eval_a[i] * eval_b[i]
            __BIGINT_FFT__(&eval_a[i], &eval_b[i], &prod_tmp, fft_ctx, &echeck); SCRATCH_FOVF(echeck, fft_ctx, fft_mark);
            __reduce_mod_fermat(&eval_a[i], &prod_tmp, lo_buf, hi_buf, xz_n, xz_nlimbs);
            // Immediate Ring Reduction: Reduce back to Z/(2^n + 1)Z
        }

        /* ---------------- 5a+6a. INTERPOLATION & RECOMPOSITION ---------------- */
        // Execute Inverse NTT (eval_a now holds the point-wise products)
        _bigint_ctk_ifft(eval_a, tbuf, usave, lo_buf, hi_buf, xz_d, xz_k, xz_n, xz_nlimbs);
        for (size_t i = 0; i < xz_d; ++i) {
            size_t wexp = i * psi_step; // Post-FFT Unscaling: Multiply by inverse negacyclic weights ψ^(-i)
            size_t iwexp = ((xz_n << 1) - wexp) % (xz_n << 1); // Exponent cycle of 2n;
            __cyclic_shift_mod(&eval_a[i], &eval_a[i], lo_buf, hi_buf, iwexp, xz_n, xz_nlimbs);
        }
        // Clear and prepare your final destination container
        tmp_res.cap = xz_Bsize + xz_slice; memset(tmp_res.limbs, 0, tmp_res.cap * U64_BYTES);
        // Overlap-Add Recomposition into Final ab PRODUCT
        for (size_t i = 0; i < xz_d; ++i) {
            if (!eval_a[i].n) continue;
            size_t total_bshift = i * xz_m;
            size_t mlimb_shift = total_bshift >> 6;
            size_t m_bshift = total_bshift & 63;

            // Actual recomposition operations
            if (!m_bshift) __BIGINT_ADD_SHIFT__(&tmp_res, &eval_a[i], mlimb_shift);
            else {
                // Copy + Shift fused together
                // This part only handles the modularly-reduced single machine-word bit shifts
                tbuf_view.n = eval_a[i].n;
                uint64_t discarded_bits = 0, mask = U64_BITS - m_bshift;
                for (size_t j = 0; j < tbuf_view.n; ++j) {
                    uint64_t tmp = eval_a[i].limbs[j];
                    tbuf_view.limbs[j] = (tmp << m_bshift) | discarded_bits;
                    discarded_bits = tmp >> mask;
                } if (discarded_bits) tbuf_view.limbs[tbuf_view.n++] = discarded_bits;
                __BIGINT_ADD_SHIFT__(&tmp_res, &tbuf_view, mlimb_shift); // Addition with actual limb shifts
            }
        } __BIGINT_INTERNAL_TRIM_LZ__(&tmp_res); // Accumulating results into tmp_res, Same principle as schoolbook
        __BIGINT_ADD_SHIFT__(&acum_tres, &tmp_res, offset); offset += curr_slice;
    } __BIGINT_INTERNAL_COPY__(xz_res, &acum_tres);




    //* ========================== X and Z MULTIPLICATION PAIR - LOOP OF SLICES ========================== *//
    offset = 0; alpha_window = (bigInt){0}; scaled_threshold = BIGINT_SCHOOLBOOK * ___schoolbook_scale(yw_Bsize);
    for (size_t i = 0; i < yw_splits; ++i) {
        size_t curr_slice = yw_slice; if (unlikely(i = yw_splits - 1) && yw_lslice) curr_slice = yw_lslice;
        alpha_window = (bigInt){.limbs = yw_alpha->limbs + offset, .n = curr_slice, .cap = curr_slice, .sign = 1};
        if ((yw_Bsize <= scaled_threshold || curr_slice <= scaled_threshold)) { 
            __BIGINT_SCHOOLBOOK__(yw_beta, &alpha_window, &tmp_res);
            __BIGINT_ADD_SHIFT__(&acum_tres, &tmp_res, offset); continue; // (tmp_res <<<= yw_slice) + tmp
        }
        else if (yw_Bsize != curr_slice) {
            __BIGINT_ASYM_MUL_DISP__(yw_beta, &alpha_window, &tmp_res, fft_ctx, &echeck); SCRATCH_FOVF(echeck, fft_ctx, fft_mark);
            __BIGINT_ADD_SHIFT__(&acum_tres, &tmp_res, offset); continue; // (tmp_res <<<= slice) + tmp
        }
        // Recalculation of Metadata on yw_lslice's iteration
        if (i == yw_splits - 1 && yw_lslice) {
            yw_k = __fft_best_metadata(yw_Bsize, curr_slice, &yw_d, &yw_m, &yw_n);
            yw_mlimbs = (yw_m + U64_BITS - 1) >> 6; /**/ yw_nlimbs = (yw_n + U64_BITS) >> 6;
        }
        

        /* ------------------------ 1. Setup & Split ------------------------ */
        for (size_t i = 0; i < yw_d; ++i) {
            size_t a_offset = i * yw_mlimbs; size_t a_len = (a_offset < yw_slice) ? min(yw_mlimbs, yw_slice - a_offset) : 0;
            size_t b_offset = i * yw_mlimbs; size_t b_len = (b_offset < yw_Bsize) ? min(yw_mlimbs, yw_Bsize - b_offset) : 0;
            a_windows[i] = (bigInt){ .limbs = alpha_window.limbs + a_offset, .n = a_len, .cap = yw_mlimbs, .sign = 1 };
            b_windows[i] = (bigInt){ .limbs = yw_beta->limbs + b_offset,     .n = b_len, .cap = yw_mlimbs, .sign = 1 };
        }
        /* ---------- 2a+3a. PRE-EVALUATION NEGACYCLIC OPTIMIZATION + FFT EVALUATION ---------- */
        // 2. Looping over D of each windows and Pre-scale them through Negacyclic Convolutions
        // Allocating two flat, contiguous memory blocks for all D windows at once
        // (improve Cache Locality and Prefetch Efficiency)
        psi_step = yw_n >> yw_k;
        for (size_t i = 0; i < yw_d; ++i) {
            eval_a[i] = (bigInt){ .limbs = flat_evala + (i * (yw_nlimbs + 1)), .n = yw_nlimbs, .cap = yw_nlimbs + 1, .sign = 1 };
            eval_b[i] = (bigInt){ .limbs = flat_evalb + (i * (yw_nlimbs + 1)), .n = yw_nlimbs, .cap = yw_nlimbs + 1, .sign = 1 };
            // Perform cyclic shifts straight into these perfectly localized targets
            size_t weight_exp = i * psi_step;
            __cyclic_shift_mod(&eval_a[i], &a_windows[i], lo_buf, hi_buf, weight_exp, yw_n, yw_nlimbs);
            __cyclic_shift_mod(&eval_b[i], &b_windows[i], lo_buf, hi_buf, weight_exp, yw_n, yw_nlimbs);
        }
        // 3. Execute forward Cooley-Tukey NTT to move elements into frequency domain
        _bigint_ctk_fft(eval_a, tbuf, usave, lo_buf, hi_buf, yw_d, yw_k, yw_n, yw_nlimbs);
        _bigint_ctk_fft(eval_b, tbuf, usave, lo_buf, hi_buf, yw_d, yw_k, yw_n, yw_nlimbs);

        /* ----------- 4a. RECURSIVE POINT-WISE MULTIPLICATIONS OF RING ELEMENTS ----------- */
        prod_tmp.cap = (yw_nlimbs << 1) + 2;
        for (size_t i = 0; i < yw_d; ++i) {
            // Recursive Multiply step: eval_a[i] * eval_b[i]
            __BIGINT_FFT__(&eval_a[i], &eval_b[i], &prod_tmp, fft_ctx, &echeck); SCRATCH_FOVF(echeck, fft_ctx, fft_mark);
            __reduce_mod_fermat(&eval_a[i], &prod_tmp, lo_buf, hi_buf, yw_n, yw_nlimbs);
            // Immediate Ring Reduction: Reduce back to Z/(2^n + 1)Z
        }

        /* ---------------- 5a+6a. INTERPOLATION & RECOMPOSITION ---------------- */
        // Execute Inverse NTT (eval_a now holds the point-wise products)
        _bigint_ctk_ifft(eval_a, tbuf, usave, lo_buf, hi_buf, yw_d, yw_k, yw_n, yw_nlimbs);
        for (size_t i = 0; i < yw_d; ++i) {
            size_t wexp = i * psi_step; // Post-FFT Unscaling: Multiply by inverse negacyclic weights ψ^(-i)
            size_t iwexp = ((yw_n << 1) - wexp) % (yw_n << 1); // Exponent cycle of 2n;
            __cyclic_shift_mod(&eval_a[i], &eval_a[i], lo_buf, hi_buf, iwexp, yw_n, yw_nlimbs);
        }
        // Clear and prepare your final destination container
        tmp_res.cap = yw_Bsize + yw_slice; memset(tmp_res.limbs, 0, tmp_res.cap * U64_BYTES);
        // Overlap-Add Recomposition into Final ab PRODUCT
        for (size_t i = 0; i < yw_d; ++i) {
            if (!eval_a[i].n) continue;
            size_t total_bshift = i * yw_m;
            size_t mlimb_shift = total_bshift >> 6;
            size_t m_bshift = total_bshift & 63;

            // Actual recomposition operations
            if (!m_bshift) __BIGINT_ADD_SHIFT__(&tmp_res, &eval_a[i], mlimb_shift);
            else {
                // Copy + Shift fused together
                // This part only handles the modularly-reduced single machine-word bit shifts
                tbuf_view.n = eval_a[i].n;
                uint64_t discarded_bits = 0, mask = U64_BITS - m_bshift;
                for (size_t j = 0; j < tbuf_view.n; ++j) {
                    uint64_t tmp = eval_a[i].limbs[j];
                    tbuf_view.limbs[j] = (tmp << m_bshift) | discarded_bits;
                    discarded_bits = tmp >> mask;
                } if (discarded_bits) tbuf_view.limbs[tbuf_view.n++] = discarded_bits;
                __BIGINT_ADD_SHIFT__(&tmp_res, &tbuf_view, mlimb_shift); // Addition with actual limb shifts
            }
        } __BIGINT_INTERNAL_TRIM_LZ__(&tmp_res); // Accumulating results into tmp_res, Same principle as schoolbook
        __BIGINT_ADD_SHIFT__(&acum_tres, &tmp_res, offset); offset += curr_slice;
    } __BIGINT_INTERNAL_COPY__(yw_res, &acum_tres); scratch_rewind(fft_ctx, fft_mark); return BIGINT_SUCCESS;
}
