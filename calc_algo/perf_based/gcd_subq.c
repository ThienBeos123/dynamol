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
#include "mv_matmul/_mv_matmul_.h"
/** ----------- General BigInt Greatest Common Divisor -----------
 * THIS FILE CONTAINS THE FOLLOWING ALGORITHMS:
 *
 *      - Half-GCD (Helper, Size-reducing algorithm)
 *      - Subquadratic-GCD (Full-fledge GCD, utilizes Half-GCD and Lehmer GCD)
 *
 * This file is a complexity-delegated file, specifically containing the implementation
 * of the algorithm as detailed above. Other bigInt algorithm GCD files include
 *
 *      - gcd.c (Algorithm Dispatcher + Implementation of Binary and Lehmer GCD)
 *      - mv_matmul/_mv_matmul_disp.c (Algorithm Dispatcher of Matrix-Vector Multiplication Algorithms)
 *      - mv_matmul/_mv_matmul_ws.c (Workspace Size Returning Dispatcher of Matrix-Vector Multiplication Algorithms)
 *      - ...
 *
 * For the ... at the end, it basically means that every single file inside /mv_matmul is, considerably,
 * under the direct implementation scope of GCD, as it was initially implemented as efficient
 * algorithms for calculating the transformation matrix T for Half-GCD, but was then generalized for future uses (xGCD)
 */
// Helper function (Mostly for Convenience)
void _RT_MAT_COPY(struct rt_matrix *dst, struct rt_matrix *src) {
    __BIGINT_INTERNAL_COPY__(&dst->A, &src->A); __BIGINT_INTERNAL_COPY__(&dst->B, &src->B);
    __BIGINT_INTERNAL_COPY__(&dst->C, &src->C); __BIGINT_INTERNAL_COPY__(&dst->D, &src->D);
}






/** ------------ 2-limb Base-case Half-GCD ------------
 * This function, for all intend and purposes, is exclusively an internal function
 * utilized inside the "gcd_subq.c" file as the base-case of __hgcd_reduct().
 * This function computes the base-case of __hgcd_reduct() to compute the Half-GCD
 * Transformation/Reduction 2x2 matrix T. This function computates exclusively inputs
 * a and b of 2-limbs long
 */
void __hgcd2_base(struct rt_matrix *T, bigInt *const a, bigInt *const b) {

}




/** ------------- Cross Matrix Composition -------------
 * This function is reserved for computing the matrix composition between
 * two 2x2 matrices. It is most notably used to compute the expression
 *
 *      T1 = [A, B]     T2 = [E, F] ---> T1 x T2 = [(AE + BG), (AF + BH)]
 *           [C, D]          [G, H]                [(CE + DG), (CF + DH)]
 */
size_t __hgcd_mat_compose_ws(
    size_t Asize, size_t Bsize, size_t Csize, size_t Dsize, 
    size_t Esize, size_t Fsize, size_t Gsize, size_t Hsize
) {
    size_t ae_size = max(max(max(Asize + Esize, Asize + Fsize), Csize + Esize), Csize + Fsize);
    size_t bg_size = max(max(max(Bsize + Gsize, Bsize + Hsize), Dsize + Gsize), Dsize + Hsize);
    size_t fcall_aebg = __MV_MATMUL_WS__(Asize, Esize, Bsize, Gsize);
    size_t fcall_afbh = __MV_MATMUL_WS__(Asize, Fsize, Bsize, Hsize);
    size_t fcall_cedg = __MV_MATMUL_WS__(Csize, Esize, Dsize, Gsize);
    size_t fcall_cfdh = __MV_MATMUL_WS__(Csize, Fsize, Dsize, Hsize);
    return ae_size + bg_size + max(max(max(fcall_aebg, fcall_afbh), fcall_cedg), fcall_cfdh);
}
dnml_status __hgcd_mat_compose(struct rt_matrix *T1, struct rt_matrix *T2, struct rt_matrix *T, calc_ctx hgcd_ctx) {
    // tmp_AE is shared for the calculation of AE, AF, CE, CF
    // tmp_BG is shared for the calculation of BG, BH, DG, DH
    size_t hgcd_mark = scratch_mark(&hgcd_ctx); dnml_status echeck = BIGINT_SUCCESS;
    size_t ae_size = max(max(max(T1->A.n + T2->A.n, T1->A.n + T2->B.n), T1->C.n + T2->A.n), T1->C.n + T2->B.n);
    size_t bg_size = max(max(max(T1->B.n + T2->C.n, T1->B.n + T2->D.n), T1->D.n + T2->C.n), T1->D.n + T2->D.n);
    BIGINT_FTEMP(tmp_AE, ae_size, hgcd_ctx, hgcd_mark, echeck);
    BIGINT_FTEMP(tmp_BG, bg_size, hgcd_ctx, hgcd_mark, echeck);
    __MV_MATMUL_21__(&T1->A, &T2->A, &T1->B, &T2->C, &tmp_AE, &tmp_BG, hgcd_ctx); // (AE + BG)
    SCRATCH_FOVF(echeck, hgcd_ctx, hgcd_mark); __BIGINT_ADD_WC__(&T->A, &tmp_AE, &tmp_BG);
    __MV_MATMUL_21__(&T1->A, &T2->B, &T1->B, &T2->D, &tmp_AE, &tmp_BG, hgcd_ctx); // (AF + BH)
    SCRATCH_FOVF(echeck, hgcd_ctx, hgcd_mark); __BIGINT_ADD_WC__(&T->B, &tmp_AE, &tmp_BG);
    __MV_MATMUL_21__(&T1->C, &T2->A, &T1->D, &T2->C, &tmp_AE, &tmp_BG, hgcd_ctx); // (CE + DG)
    SCRATCH_FOVF(echeck, hgcd_ctx, hgcd_mark); __BIGINT_ADD_WC__(&T->C, &tmp_AE, &tmp_BG);
    __MV_MATMUL_21__(&T1->C, &T2->B, &T1->D, &T2->D, &tmp_AE, &tmp_BG, hgcd_ctx); // (CF + DH)
    SCRATCH_FOVF(echeck, hgcd_ctx, hgcd_mark); __BIGINT_ADD_WC__(&T->D, &tmp_AE, &tmp_BG);
    scratch_rewind(&hgcd_ctx, hgcd_mark); return BIGINT_SUCCESS;
}





/** ------------- Cross Matrix-Vector Multiplication -------------
 * This function is reserved for computing the linear combination between a 2x2 matrix
 * and a 2s1 column vector. It is most notably used to compute the expression
 *
 *      a_new = Aa + Bb     WHERE A, B, C, D ∈ [A, B]   AND a,b ∈ [a]
 *      b_new = Ca + Db                        [C, D]             [b]
 */

size_t __hgcd_matmul_ws(size_t a_size, size_t b_size, size_t Asize, size_t Bsize, size_t Csize, size_t Dsize) {
    size_t tmp_sizes = max(a_size + Asize, a_size + Csize) + max(b_size + Bsize, b_size + Dsize);
    size_t res_AB_size = max(a_size + Asize, b_size + Bsize) + 1;
    size_t res_CD_size = max(a_size + Csize, b_size + Dsize) + 1;
    size_t fcall_AB = __MV_MATMUL_WS__(Asize, a_size, Bsize, b_size);
    size_t fcall_CD = __MV_MATMUL_WS__(Csize, a_size, Dsize, b_size);
    return tmp_sizes + res_AB_size + res_CD_size + max(fcall_AB, fcall_CD);
}
dnml_status __hgcd_matmul(bigInt *const a, bigInt *const b, struct rt_matrix *T, calc_ctx hgcd_ctx) {
    // tmp_Aa is shared for the calculation of Aa and Ca
    // tmp_Bb is shared for the calculation of Bb and Db
    size_t hgcd_mark = scratch_mark(&hgcd_ctx); dnml_status echeck = BIGINT_SUCCESS;
    BIGINT_FTEMP(tmp_Aa, max(a->n + T->A.n, a->n + T->C.n), hgcd_ctx, hgcd_mark, echeck);
    BIGINT_FTEMP(tmp_Bb, max(b->n + T->B.n, b->n + T->D.n), hgcd_ctx, hgcd_mark, echeck);
    BIGINT_FTEMP(res_AaBb, max(a->n + T->A.n, b->n + T->B.n) + 1, hgcd_ctx, hgcd_mark, echeck);
    BIGINT_FTEMP(res_CaDb, max(a->n + T->C.n, b->n + T->D.n) + 1, hgcd_ctx, hgcd_mark, echeck);
    __MV_MATMUL_21__(&T->A, a, &T->B, b, &tmp_Aa, &tmp_Bb, hgcd_ctx); 
    SCRATCH_FOVF(echeck, hgcd_ctx, hgcd_mark); __BIGINT_ADD_WC__(&res_AaBb, &tmp_Aa, &tmp_Bb);
    __MV_MATMUL_21__(&T->C, a, &T->D, b, &tmp_Aa, &tmp_Bb, hgcd_ctx);
    SCRATCH_FOVF(echeck, hgcd_ctx, hgcd_mark); __BIGINT_ADD_WC__(&res_CaDb, &tmp_Aa, &tmp_Bb);
    __BIGINT_INTERNAL_COPY__(a, &res_AaBb); __BIGINT_INTERNAL_COPY__(b, &res_CaDb);
    scratch_rewind(&hgcd_ctx, hgcd_mark); return BIGINT_SUCCESS;
}




/** --------- Half-GCD Function ---------
 * This function, for all intend and purposes, is exclusively an internal function
 * utilized inside the "gcd_subq.c" file as a helper of __BIGINT_SUBQ__().
 * This function computes a transformation matrix T, in which when apply
 * onto the two inputs u and v, with sizes of N limbs, into floor(N/2) + 1,
 * or, more accurately, ceil(N/2). 
 */
size_t _hgcd_ws(size_t TA_size, size_t TB_size, size_t TC_size, size_t TD_size, size_t a_size, size_t b_size) { return 0; }
dnml_status _hgcd_reduct(struct rt_matrix *T, bigInt *const a, bigInt *const b, calc_ctx hgcd_ctx) {
    size_t n = a->n; /**/ if (n <= 2) { __hgcd2_base(T, a, b); return BIGINT_SUCCESS; } // Base-case
    size_t m = n >> 1; // n / 2 (The predicted sizes of our elements in matrix T)
    if (b->n <= m) { // b is already small enough
        /* Identity matrix: */
        /*  [1, 0] */ T->A.n = 1; T->A.limbs[0] = 1; /**/ T->B.n = 0;
        /*  [0, 1] */ T->C.n = 0; /**/ T->D.n = 1; T->D.limbs[0] = 1; return BIGINT_SUCCESS;
    }
    /* Standard Case */ dnml_status echeck = BIGINT_SUCCESS;
    size_t hgcd_mark = scratch_mark(&hgcd_ctx); /**/ struct rt_matrix R1 = {0}, R2 = {0}, temp_T = {0};
    BIGINT_FTEMP(a_copy, a->n, hgcd_ctx, hgcd_mark, echeck); memcpy(a_copy.limbs, a->limbs, a->n * U64_BYTES); a_copy.n = a->n;
    BIGINT_FTEMP(b_copy, b->n, hgcd_ctx, hgcd_mark, echeck); memcpy(b_copy.limbs, b->limbs, b->n * U64_BYTES); b_copy.n = b->n;
    R1.A.limbs = (limb_t*)scratch_alloc(&hgcd_ctx, m, &echeck); SCRATCH_FOVF(echeck, hgcd_ctx, hgcd_mark);
    R1.B.limbs = (limb_t*)scratch_alloc(&hgcd_ctx, m, &echeck); SCRATCH_FOVF(echeck, hgcd_ctx, hgcd_mark);
    R1.C.limbs = (limb_t*)scratch_alloc(&hgcd_ctx, m, &echeck); SCRATCH_FOVF(echeck, hgcd_ctx, hgcd_mark);
    R1.D.limbs = (limb_t*)scratch_alloc(&hgcd_ctx, m, &echeck); SCRATCH_FOVF(echeck, hgcd_ctx, hgcd_mark);
    
    // Recursive Calculation of R1
    bigInt ahi = { .limbs = a_copy.limbs + m, .n = a_copy.n - m, .cap = a_copy.n - m, .sign = 1 };
    bigInt bhi = { .limbs = b_copy.limbs + m, .n = b_copy.n - m, .cap = b_copy.n - m, .sign = 1 };
    _hgcd_reduct(&R1, &ahi, &bhi, hgcd_ctx); SCRATCH_FOVF(echeck, hgcd_ctx, hgcd_mark);

    // Intermediate Update of the full integer through R1
    __hgcd_matmul(&a_copy, &b_copy, &R1, hgcd_ctx); SCRATCH_FOVF(echeck, hgcd_ctx, hgcd_mark);
    if (b->n <= m) { // Early exit, b is already small enough
        _RT_MAT_COPY(T, &R1); __BIGINT_INTERNAL_COPY__(a, &a_copy); 
        __BIGINT_INTERNAL_COPY__(b, &b_copy); scratch_rewind(&hgcd_ctx, hgcd_mark); return BIGINT_SUCCESS;
    }

    // Second Recursive Calculation on R2 with updated integer
    n = a_copy.n; /**/ m = n >> 1; // Recaculating n and m based on a/b_copy reduced size after the Euclidean's division step
    R2.A.limbs = (limb_t*)scratch_alloc(&hgcd_ctx, m, &echeck); SCRATCH_FOVF(echeck, hgcd_ctx, hgcd_mark);
    R2.B.limbs = (limb_t*)scratch_alloc(&hgcd_ctx, m, &echeck); SCRATCH_FOVF(echeck, hgcd_ctx, hgcd_mark);
    R2.C.limbs = (limb_t*)scratch_alloc(&hgcd_ctx, m, &echeck); SCRATCH_FOVF(echeck, hgcd_ctx, hgcd_mark);
    R2.D.limbs = (limb_t*)scratch_alloc(&hgcd_ctx, m, &echeck); SCRATCH_FOVF(echeck, hgcd_ctx, hgcd_mark);
    // Rewindow ahi and bhi to now match with truncated a_copy and b_copy after the Euclidean GCD division step
    ahi = (bigInt){ .limbs = a_copy.limbs + m, .n = a_copy.n - m, .cap = a_copy.n - m, .sign = 1 };
    bhi = (bigInt){ .limbs = b_copy.limbs + m, .n = b_copy.n - m, .cap = b_copy.n - m, .sign = 1 };
    _hgcd_reduct(&R2, &ahi, &bhi, hgcd_ctx); SCRATCH_FOVF(echeck, hgcd_ctx, hgcd_mark);
    __hgcd_matmul(&a_copy, &b_copy, &R2, hgcd_ctx); SCRATCH_FOVF(echeck, hgcd_ctx, hgcd_mark);
    __hgcd_mat_compose(&R1, &R2, T, hgcd_ctx); SCRATCH_FOVF(echeck, hgcd_ctx, hgcd_mark);
    __BIGINT_INTERNAL_COPY__(a, &a_copy); __BIGINT_INTERNAL_COPY__(b, &b_copy);
    scratch_rewind(&hgcd_ctx, hgcd_mark); return BIGINT_SUCCESS;
}



/* ---------- Main Orchestrating Function ---------- */
void __BIGINT_SUBQ__(P_BIGINT res, PCONST_BIGINT u, PCONST_BIGINT v, calc_ctx subq_ctx, dnml_status *err) {

}
