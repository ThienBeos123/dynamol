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



#include "gcd.h"
#include <debug_util.h>
#include "../../util/aconv_macros.h"
#include "mul_fft.c" // Import FFT, IFFT, and Ring Element Operations

/**
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
/* ------------ Sizing Function ------------ */
size_t __BIGINT_MAT_TOOM3_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size) { return 0; }
size_t __BIGINT_MAT_TOOM4_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size) { return 0; }
size_t __BIGINT_MAT_TOOM5_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size) { return 0; }
size_t __BIGINT_MAT_SSA_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size) { return 0; }



/* ------- BigInt Matrix Multiplication Toom-cook 3-way ------- */
dnml_status __BIGINT_MATMUL_TOOM3__(
    P_BIGINT x, P_BIGINT z, /**/ P_BIGINT y, P_BIGINT w, 
    P_BIGINT xz_res, P_BIGINT yw_res, calc_ctx toom_ctx
) {
    size_t xz_k = (size_t)(max(x->n, z->n) / 3) + 1;
    size_t yw_k = (size_t)(max(y->n, w->n) / 3) + 1;
    size_t max_k = max(xz_k, yw_k);
    //* ============== X and Z MULTIPLICATION PAIR ============== *//
    /* ------- 1a. Setup & Splitting ------- */
    size_t x2size = (x->n > (xz_k << 1)) ? (x->n - (xz_k << 1)) : 0;
    size_t z2size = (z->n > (xz_k << 1)) ? (z->n - (xz_k << 1)) : 0;
    size_t y2size = (y->n > (yw_k << 1)) ? (y->n - (yw_k << 1)) : 0;
    size_t w2size = (w->n > (yw_k << 1)) ? (w->n - (yw_k << 1)) : 0;
    size_t max_m2size = max(x2size, y2size); size_t max_n2size = max(z2size, w2size);
    bigInt m0 = {.limbs = x->limbs,                 .n = xz_k,      .cap = xz_k};
    bigInt m1 = {.limbs = x->limbs + xz_k,          .n = xz_k,      .cap = xz_k};
    bigInt m2 = {.limbs = x->limbs + (xz_k << 1),   .n = x2size,    .cap = x2size};
    bigInt n0 = {.limbs = z->limbs,                 .n = xz_k,      .cap = xz_k};
    bigInt n1 = {.limbs = z->limbs + xz_k,          .n = xz_k,      .cap = xz_k};
    bigInt n2 = {.limbs = z->limbs + (xz_k << 1),   .n = z2size,    .cap = z2size};

    /* ------------ 2a. Evaluation & Point-wise Multiplication ------------ */
    dnml_status echeck; size_t toom_mark = scratch_mark(&toom_ctx);
    // p(x) TEMPORARIES
    //  +) pOuter   = m0 + m2                           |   +) p(-1)    = pOuter - m1
    //  +) p(0)     = m0          (NO FULL TEMPORARY)   |   +) p(-2)    = 2*(p(-1) + m2) - m0
    //  +) p(1)     = pOuter + m1                       |   +) p(inf)   = m2                    (NO FULL TEMPORARY)
    BIGINT_FTEMP(p_outer, max_k + 1, toom_ctx, toom_mark, echeck); p_outer.cap = xz_k + 1;
    BIGINT_FTEMP(p1,      max_k + 2, toom_ctx, toom_mark, echeck); p1.cap = xz_k + 2;
    BIGINT_FTEMP(p_neg1,  max_k + 1, toom_ctx, toom_mark, echeck); p_neg1.cap = xz_k + 1;
    BIGINT_FTEMP(p_neg2,  max_k + 1, toom_ctx, toom_mark, echeck); p_neg2.cap = xz_k + 1;
    // q(x) TEMPORARIES
    //  +) qOuter   = n0 + n2                           |   +) q(-1)    = qOuter - n1
    //  +) q(0)     = n0          (NO FULL TEMPORARY)   |   +) q(-2)    = 2*(q(-1) + n2) - n0
    //  +) q(1)     = qOuter + n1                       |   +) q(inf)   = n2                    (NO FULL TEMPORARY)
    BIGINT_FTEMP(q_outer, max_k + 1, toom_ctx, toom_mark, echeck); q_outer.cap = xz_k + 1;
    BIGINT_FTEMP(q1,      max_k + 2, toom_ctx, toom_mark, echeck); q1.cap = xz_k + 2;
    BIGINT_FTEMP(q_neg1,  max_k + 1, toom_ctx, toom_mark, echeck); q_neg1.cap = xz_k + 1;
    BIGINT_FTEMP(q_neg2,  max_k + 1, toom_ctx, toom_mark, echeck); q_neg2.cap = xz_k + 1;
    // p(x) CALCULATIONS                                // q(x) CALCULATIONS
    __BIGINT_ADD_WC__(&p_outer, &m0, &m2);              __BIGINT_ADD_WC__(&q_outer, &n0, &n2);
    __BIGINT_ADD_WC__(&p1, &p_outer, &m1);              __BIGINT_ADD_WC__(&q1, &q_outer, &n1);
    __BIGINT_SUB_SAW__(&p_neg1, &p_outer, &m1);         __BIGINT_SUB_SAW__(&q_neg1, &q_outer, &n1);
    __BIGINT_ADD_SAW__(&p_neg2, &p_neg1, &m2);          __BIGINT_ADD_SAW__(&q_neg2, &q_neg1, &n2);
    __BIGINT_INTERNAL_LSHIFT__(&p_neg2, 1);             __BIGINT_INTERNAL_LSHIFT__(&q_neg2, 1);
    __BIGINT_SUB_SAW__(&p_neg2, &p_neg2, &m0);          __BIGINT_SUB_SAW__(&q_neg2, &q_neg2, &n0);
    /* ------------ POINT-WISE MULTIPLICATION ------------
    *   +) r(0)   = p(0)   * q(0)       ---> Cap: 2k
    *   +) r(1)   = p(1)   * q(1)       ---> Cap: 2k + 4 (original) --> 2k + 9 (interpolation - r1 + llshift)
    *   +) r(-1)  = p(-1)  * q(-1)      ---> Cap: 2k + 2 (original) --> 2k + 9 (interpolation - r2 + llshift)
    *   +) r(-2)  = p(-2)  * q(-2)      ---> Cap: 2k + 4 (original) --> 2k + 10 (interpolation - r3 + llshift)
    *   +) r(inf) = p(inf) * q(inf)     ---> Cap: 2k (original) ---> 2k + 4 (bit-shifts accounted)
    */
    BIGINT_FTEMP(r0,     (max_k << 1),       toom_ctx, toom_mark, echeck); r0.cap = (xz_k << 1);
    BIGINT_FTEMP(r1,     (max_k << 1) + 9,   toom_ctx, toom_mark, echeck); r1.cap = (xz_k << 1) + 9;
    BIGINT_FTEMP(r_neg1, (max_k << 1) + 9,   toom_ctx, toom_mark, echeck); r_neg1.cap = (xz_k << 1) + 9;
    BIGINT_FTEMP(r_neg2, (max_k << 1) + 10,  toom_ctx, toom_mark, echeck); r_neg2.cap = (xz_k << 1) + 10;
    BIGINT_FTEMP(rinf, max_m2size + max_m2size + 4, toom_ctx, toom_mark, echeck); rinf.cap = x2size + z2size;
    __BIGINT_TOOM_3__(&m0, &n0, &r0, toom_ctx, &echeck); SCRATCH_FOVF(echeck, toom_ctx, toom_mark);
    __BIGINT_TOOM_3__(&p1, &q1, &r1, toom_ctx, &echeck); SCRATCH_FOVF(echeck, toom_ctx, toom_mark);
    __BIGINT_TOOM_3__(&p_neg1, &q_neg1, &r_neg1, toom_ctx, &echeck); SCRATCH_FOVF(echeck, toom_ctx, toom_mark);
    __BIGINT_TOOM_3__(&p_neg2, &q_neg2, &r_neg2, toom_ctx, &echeck); SCRATCH_FOVF(echeck, toom_ctx, toom_mark);
    __BIGINT_TOOM_3__(&m2, &n2, &rinf, toom_ctx, &echeck); SCRATCH_FOVF(echeck, toom_ctx, toom_mark);

    /* ----------------- 3a. INTERPOLATION & RECOMPOSITION ----------------- */
    /* r3 = 2k + 5 */ __BIGINT_SUB_SAW__(&r_neg2, &r_neg2, &r_neg1); __BIGINT_DIV3__(&r_neg2);
    /* r1 = 2k + 5 */ __BIGINT_SUB_SAW__(&r1, &r1, &r_neg1); __BIGINT_INTERNAL_RSHIFT__(&r_neg1, 1);
    /* r2 = 2k + 3 */ __BIGINT_SUB_SAW__(&r_neg1, &r_neg1, &r0);
    /* r3 = 2k + 7 */ __BIGINT_SUB_SAW__(&r_neg2, &r_neg1, &r_neg2);
    __BIGINT_INTERNAL_RSHIFT__(&r_neg2, 1); __BIGINT_INTERNAL_LSHIFT__(&rinf, 1);
    __BIGINT_ADD_SAW__(&r_neg2, &r_neg2, &rinf);
    /* r2 = 2k + 7 */ __BIGINT_ADD_SAW__(&r_neg1, &r_neg1, &r1);
    __BIGINT_INTERNAL_RSHIFT__(&rinf, 1); __BIGINT_SUB_SAW__(&r_neg1, &r_neg1, &rinf);
    /* r1 = 2k + 8 */ __BIGINT_SUB_SAW__(&r1, &r1, &r_neg2);
    // ------------------ RECOMPOSITION ------------------ //
    BIGINT_FTEMP(final_res, (max_k << 1) + 14, toom_ctx, toom_mark, echeck); final_res.cap = (xz_k << 1) + 14;
    __BIGINT_INTERNAL_LLSHIFT__(&rinf, 4);   __BIGINT_INTERNAL_LLSHIFT__(&r_neg2, 3);
    __BIGINT_INTERNAL_LLSHIFT__(&r_neg1, 2); __BIGINT_INTERNAL_LLSHIFT__(&r1, 1);
    __BIGINT_ADD_WC__(&final_res, &rinf, &r_neg2); __BIGINT_ADD_WC__(&final_res, &final_res, &r_neg1);
    __BIGINT_ADD_WC__(&final_res, &final_res, &r1); __BIGINT_ADD_WC__(&final_res, &final_res, &r0);
    __BIGINT_INTERNAL_COPY__(xz_res, &final_res);




    //* ============== Y and W MULTIPLICATION PAIR ============== *//
    /* -------- 1b. Setup & Splitting ---------- */
    m0 = (bigInt){.limbs = y->limbs,                 .n = yw_k,      .cap = yw_k};
    m1 = (bigInt){.limbs = y->limbs + yw_k,          .n = yw_k,      .cap = yw_k};
    m2 = (bigInt){.limbs = y->limbs + (yw_k << 1),   .n = y2size,    .cap = y2size};
    n0 = (bigInt){.limbs = w->limbs,                 .n = yw_k,      .cap = yw_k};
    n1 = (bigInt){.limbs = w->limbs + yw_k,          .n = yw_k,      .cap = yw_k};
    n2 = (bigInt){.limbs = w->limbs + (yw_k << 1),   .n = w2size,    .cap = w2size};

    /* --------------------- 2b. Evaluation & Pointwise Multiplication ---------------------
    *   +) pOuter = m0 + m2                         | +) qOuter = n0 + n2
    *   +) p(0)   = m0          (NO FULL TEMPORARY) | +) q(0)   = n0          (NO FULL TEMPORARY)
    *   +) p(1)   = pOuter + m1                     | +) q(1)   = qOuter + n1
    *   +) p(-1)  = pOuter - m1                     | +) q(-1)  = qOuter - n1
    *   +) p(-2)  = 2*(p(-1) + m2) - m0             | +) q(-2)  = 2*(q(-1) + n2) - n0
    *   +) p(inf) = m2          (NO FULL TEMPORARY) | +) q(inf) = n2          (NO FULL TEMPORARY) */
    // p(x) TEMPORARIES + CALCULATIONS              // q(x) TEMPORARIES + CALCULATIONS
    p_outer.cap = yw_k + 1; p1.cap = yw_k + 2;      /**/ q_outer.cap = xz_k + 1; q1.cap = xz_k + 2;
    p_neg1.cap = yw_k + 1; p_neg2.cap = yw_k + 1;   /**/ q_neg1.cap = xz_k + 1; q_neg2.cap = xz_k + 1;
    __BIGINT_ADD_WC__(&p_outer, &m0, &m2);              __BIGINT_ADD_WC__(&q_outer, &n0, &n2);
    __BIGINT_ADD_WC__(&p1, &p_outer, &m1);              __BIGINT_ADD_WC__(&q1, &q_outer, &n1);
    __BIGINT_SUB_SAW__(&p_neg1, &p_outer, &m1);         __BIGINT_SUB_SAW__(&q_neg1, &q_outer, &n1);
    __BIGINT_ADD_SAW__(&p_neg2, &p_neg1, &m2);          __BIGINT_ADD_SAW__(&q_neg2, &q_neg1, &n2);
    __BIGINT_INTERNAL_LSHIFT__(&p_neg2, 1);             __BIGINT_INTERNAL_LSHIFT__(&q_neg2, 1);
    __BIGINT_SUB_SAW__(&q_neg2, &q_neg2, &m0);          __BIGINT_SUB_SAW__(&q_neg2, &q_neg2, &n0);
    /* ------------ POINT-WISE MULTIPLICATION ------------
    *   +) r(0)   = p(0)   * q(0)       ---> Cap: 2k
    *   +) r(1)   = p(1)   * q(1)       ---> Cap: 2k + 4 (original) --> 2k + 9 (interpolation - r1 + llshift)
    *   +) r(-1)  = p(-1)  * q(-1)      ---> Cap: 2k + 2 (original) --> 2k + 9 (interpolation - r2 + llshift)
    *   +) r(-2)  = p(-2)  * q(-2)      ---> Cap: 2k + 4 (original) --> 2k + 10 (interpolation - r3 + llshift)
    *   +) r(inf) = p(inf) * q(inf)     ---> Cap: 2k (original) ---> 2k + 4 (bit-shifts accounted)
    */
    r0.cap = yw_k << 1; /**/ r1.cap = (yw_k << 1) + 9; /**/ r_neg1.cap = (yw_k << 1) + 9;
    r_neg2.cap = (yw_k << 1) + 9; /**/ rinf.cap = (yw_k << 1) + 10;
    __BIGINT_TOOM_3__(&m0, &n0, &r0, toom_ctx, &echeck); SCRATCH_FOVF(echeck, toom_ctx, toom_mark);
    __BIGINT_TOOM_3__(&p1, &q1, &r1, toom_ctx, &echeck); SCRATCH_FOVF(echeck, toom_ctx, toom_mark);
    __BIGINT_TOOM_3__(&p_neg1, &q_neg1, &r_neg1, toom_ctx, &echeck); SCRATCH_FOVF(echeck, toom_ctx, toom_mark);
    __BIGINT_TOOM_3__(&p_neg2, &q_neg2, &r_neg2, toom_ctx, &echeck); SCRATCH_FOVF(echeck, toom_ctx, toom_mark);
    __BIGINT_TOOM_3__(&m2, &n2, &rinf, toom_ctx, &echeck); SCRATCH_FOVF(echeck, toom_ctx, toom_mark);
    
    /* -------------------------- 3b. INTERPOLATION & RECOMPOSITION -------------------------- */
    /* r3 = 2k + 5 */ __BIGINT_SUB_SAW__(&r_neg2, &r_neg2, &r_neg1); __BIGINT_DIV3__(&r_neg2);
    /* r1 = 2k + 5 */ __BIGINT_SUB_SAW__(&r1, &r1, &r_neg1); __BIGINT_INTERNAL_RSHIFT__(&r_neg1, 1);
    /* r2 = 2k + 3 */ __BIGINT_SUB_SAW__(&r_neg1, &r_neg1, &r0);
    /* r3 = 2k + 7 */ __BIGINT_SUB_SAW__(&r_neg2, &r_neg1, &r_neg2);
    __BIGINT_INTERNAL_RSHIFT__(&r_neg2, 1); __BIGINT_INTERNAL_LSHIFT__(&rinf, 1);
    __BIGINT_ADD_SAW__(&r_neg2, &r_neg2, &rinf);
    /* r2 = 2k + 7 */ __BIGINT_ADD_SAW__(&r_neg1, &r_neg1, &r1);
    __BIGINT_INTERNAL_RSHIFT__(&rinf, 1); __BIGINT_SUB_SAW__(&r_neg1, &r_neg1, &rinf);
    /* r1 = 2k + 8 */ __BIGINT_SUB_SAW__(&r1, &r1, &r_neg2);
    /* ------------------ RECOMPOSITION ------------------ */ final_res.cap = (yw_k << 1) + 14;
    __BIGINT_INTERNAL_LLSHIFT__(&rinf, 4);   __BIGINT_INTERNAL_LLSHIFT__(&r_neg2, 3);
    __BIGINT_INTERNAL_LLSHIFT__(&r_neg1, 2); __BIGINT_INTERNAL_LLSHIFT__(&r1, 1);
    __BIGINT_ADD_WC__(&final_res, &rinf, &r_neg2); __BIGINT_ADD_WC__(&final_res, &final_res, &r_neg1);
    __BIGINT_ADD_WC__(&final_res, &final_res, &r1); __BIGINT_ADD_WC__(&final_res, &final_res, &r0);
    __BIGINT_INTERNAL_COPY__(yw_res, &final_res); scratch_rewind(&toom_ctx, toom_mark); return BIGINT_SUCCESS;
}




/* ------- BigInt Matrix Multiplication Toom-cook 3-way ------- */
dnml_status __BIGINT_MATMUL_TOOM4__(P_BIGINT x, P_BIGINT z, P_BIGINT y, P_BIGINT w, calc_ctx toom_ctx, dnml_status *echeck) { return BIGINT_SUCCESS; }




/* ------- BigInt Matrix Multiplication Toom-cook 3-way ------- */
dnml_status __BIGINT_MATMUL_TOOM5__(P_BIGINT x, P_BIGINT z, P_BIGINT y, P_BIGINT w, calc_ctx toom_ctx, dnml_status *echeck) { return BIGINT_SUCCESS; }





/* ------- BigInt Matrix Multiplication Toom-cook 3-way ------- */
dnml_status __BIGINT_MATMUL_SSA__(P_BIGINT x, P_BIGINT z, P_BIGINT y, P_BIGINT w, calc_ctx fft_ctx, dnml_status *echeck) { return BIGINT_SUCCESS; }





