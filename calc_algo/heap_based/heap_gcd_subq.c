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
#include "heap_gcd.h"
#include <debug_util.h>
#include "../../util/aconv_macros.h"
#include "hmv_matmul/_hmv_matmul_.h"
/** ----------- Heap-based BigInt Greatest Common Divisor -----------
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
// Helper functions
dnml_status _RT_INIT(struct rt_matrix *x) {
    // Identity Matrix:
    //  [1, 0]
    //  [0, 1]
    dnml_status echeck = BIGINT_SUCCESS; bigInt *early_free[4] = {0}; uint8_t early_cnt = 0;
    __BIGINT_INTERNAL_LINIT__(&x->A, 1); HEAP_FOOM(echeck, early_free, early_cnt); x->A.limbs[0] = 1; x->A.n = 1; early_free[early_cnt++] = &x->A;
    __BIGINT_INTERNAL_LINIT__(&x->B, 0); HEAP_FOOM(echeck, early_free, early_cnt); early_free[early_cnt++] = &x->B;
    __BIGINT_INTERNAL_LINIT__(&x->C, 1); HEAP_FOOM(echeck, early_free, early_cnt); x->C.limbs[0] = 1; x->C.n = 1; early_free[early_cnt++] = &x->C;
    __BIGINT_INTERNAL_LINIT__(&x->D, 0); HEAP_FOOM(echeck, early_free, early_cnt); early_free[early_cnt++] = &x->D;
    return BIGINT_SUCCESS;
}
void _RT_MAT_FREE(struct rt_matrix *x) {
    __BIGINT_INTERNAL_FREE__(&x->A); __BIGINT_INTERNAL_FREE__(&x->B);
    __BIGINT_INTERNAL_FREE__(&x->C); __BIGINT_INTERNAL_FREE__(&x->D);
}
void _RT_MAT_MOVE(struct rt_matrix *dst, struct rt_matrix *src) {
    __BIGINT_INTERNAL_MOVE__(&dst->A, &src->A); __BIGINT_INTERNAL_MOVE__(&dst->B, &src->B);
    __BIGINT_INTERNAL_MOVE__(&dst->C, &src->C); __BIGINT_INTERNAL_MOVE__(&dst->D, &src->D);
}




/** ------------ Machine-word Base-case Half-GCD ------------
 * This function, for all intend and purposes, is exclusively an internal function
 * utilized inside the "gcd_subq.c" file as the base-case of __hgcd_reduct().
 * This function computes the base-case of __hgcd_reduct() to compute the Half-GCD
 * Transformation/Reduction 2x2 matrix T. 
 *
 * this function computates exclusively inputs a and b of 2-limbs long OR a single machine-word long. 
 * They implement an efficient Euclidean GCD at such small scale to compute the linear combination of 
 * our transformation matrices.
 */
void __hgcd1_base(struct rt_matrix *T, bigInt *const a, bigInt *const b) {
    _RT_INIT(T); // Initializing for _hgcd_reduct as the base-case with an idenitty matrix
    // Actual loop/operation
    uint64_t rem = 0, quot = 0;
    uint64_t a1 = a->limbs[0], b1 = b->limbs[0];
    uint64_t A = (T->A.n) ? T->A.limbs[0] : 0; uint64_t B = (T->B.n) ? T->B.limbs[0] : 0;
    uint64_t C = (T->C.n) ? T->C.limbs[0] : 0; uint64_t D = (T->D.n) ? T->D.limbs[0] : 0;
    while ((a1 % b1)) {
        quot = a1 / b1; /**/ rem = a1 % b1;
        // Equation: [0,  1] x [A, B] = [(0A +  1C), (0B +  1D)] = [  C,      D   ]
        //           [1, -q]   [C, D]   [(1A + -qC), (1B + -qD)]   [A - qC, B - qD]
        uint64_t qc_hi = 0, qc_lo = __MUL_UI64__(quot, C, &qc_hi);
        uint64_t qd_hi = 0, qd_lo = __MUL_UI64__(quot, D, &qd_hi);
        // Overflowing/Underflowing, current progression matrix tracker is still
        if (qc_hi || qd_hi || qc_lo < A || qd_lo < B) break;
        uint64_t nextA = C; uint64_t nextB = D;
        uint64_t nextC = qc_lo - A; uint64_t nextD = qd_lo - B; 
        // qC and qD is guaranteed to be larger than A and B
        A = nextA; B = nextB; C = nextC; D = nextD;
        a1 = b1; b1 = rem;
    }
    // Updating our matrices
    T->A.limbs[0] = A; T->B.limbs[0] = B; /**/ T->A.n = !!(A); T->B.n = !!(B);
    T->C.limbs[0] = C; T->D.limbs[0] = D; /**/ T->C.n = !!(C); T->D.n = !!(D);
    a->limbs[0] = a1; b->limbs[0] = b1; /**/ a->n = !!(a1); b->n = !!(b1);
}




/** ------------- Cross Matrix Composition -------------
 * This function is reserved for computing the matrix composition between
 * two 2x2 matrices. It is most notably used to compute the expression
 *
 *      T1 = [A, B]     T2 = [E, F] ---> T1 x T2 = [(AE + BG), (AF + BH)]
 *           [C, D]          [G, H]                [(CE + DG), (CF + DH)]
 */
dnml_status __hgcd_mat_compose(struct rt_matrix *T1, struct rt_matrix *T2, struct rt_matrix *T) {
    // tmp_AE is shared for the calculation of AE, AF, CE, CF
    // tmp_BG is shared for the calculation of BG, BH, DG, DH
    dnml_status echeck = BIGINT_SUCCESS; bigInt *alloc_list[2], *early_free[2]; uint8_t alloc_cnt = 0, early_cnt = 0;
    size_t ae_size = max(max(max(T1->A.n + T2->A.n, T1->A.n + T2->B.n), T1->C.n + T2->A.n), T1->C.n + T2->B.n);
    size_t bg_size = max(max(max(T1->B.n + T2->C.n, T1->B.n + T2->D.n), T1->D.n + T2->C.n), T1->D.n + T2->D.n);
    BIHEAP_FTEMP(tmp_AE, ae_size, echeck, early_free, early_cnt, alloc_list, alloc_cnt);
    BIHEAP_FTEMP(tmp_BG, bg_size, echeck, early_free, early_cnt, alloc_list, alloc_cnt);
    __HMV_MATMUL_21__(&T1->A, &T2->A, &T1->B, &T2->C, &tmp_AE, &tmp_BG); // (AE + BG)
    HEAP_FOOM(echeck, early_free, early_cnt); __BIGINT_ADD_WC__(&T->A, &tmp_AE, &tmp_BG);
    __HMV_MATMUL_21__(&T1->A, &T2->B, &T1->B, &T2->D, &tmp_AE, &tmp_BG); // (AF + BH)
    HEAP_FOOM(echeck, early_free, early_cnt); __BIGINT_ADD_WC__(&T->B, &tmp_AE, &tmp_BG);
    __HMV_MATMUL_21__(&T1->C, &T2->A, &T1->D, &T2->C, &tmp_AE, &tmp_BG); // (CE + DG)
    HEAP_FOOM(echeck, early_free, early_cnt); __BIGINT_ADD_WC__(&T->C, &tmp_AE, &tmp_BG);
    __HMV_MATMUL_21__(&T1->C, &T2->B, &T1->D, &T2->D, &tmp_AE, &tmp_BG); // (CF + DH)
    HEAP_FOOM(echeck, early_free, early_cnt); __BIGINT_ADD_WC__(&T->D, &tmp_AE, &tmp_BG);
    _free_alloc_list(alloc_list, alloc_cnt); return BIGINT_SUCCESS;
}





/** ------------- 2x4 Cross Matrix-Vector Multiplication -------------
 * This function is reserved for computing the linear combination between a 2x2 matrix
 * and a 1x2 column vector. It is most notably used to compute the expression
 *
 *      a_new = Aa + Bb     WHERE A, B, C, D ∈ [A, B]   AND a,b ∈ [a]
 *      b_new = Ca + Db                        [C, D]             [b]
 */
dnml_status __hgcd_matmul(bigInt *const a, bigInt *const b, struct rt_matrix *T) {
    // tmp_Aa is shared for the calculation of Aa and Ca
    // tmp_Bb is shared for the calculation of Bb and Db
    dnml_status echeck = BIGINT_SUCCESS; bigInt *alloc_list[4], *early_free[4]; uint8_t alloc_cnt = 0, early_cnt = 0;
    BIHEAP_FTEMP(tmp_Aa, max(a->n + T->A.n, a->n + T->C.n), echeck, early_free, early_cnt, alloc_list, alloc_cnt);
    BIHEAP_FTEMP(tmp_Bb, max(b->n + T->B.n, b->n + T->D.n), echeck, early_free, early_cnt, alloc_list, alloc_cnt);
    BIHEAP_FTEMP(res_AaBb, max(a->n + T->A.n, b->n + T->B.n) + 1, echeck, early_free, early_cnt, alloc_list, alloc_cnt);
    BIHEAP_FTEMP(res_CaDb, max(a->n + T->C.n, b->n + T->D.n) + 1, echeck, early_free, early_cnt, alloc_list, alloc_cnt);
    __HMV_MATMUL_21__(&T->A, a, &T->B, b, &tmp_Aa, &tmp_Bb);
    HEAP_FOOM(echeck, early_free, early_cnt); __BIGINT_ADD_WC__(&res_AaBb, &tmp_Aa, &tmp_Bb);
    __HMV_MATMUL_21__(&T->C, a, &T->D, b, &tmp_Aa, &tmp_Bb);
    HEAP_FOOM(echeck, early_free, early_cnt); __BIGINT_ADD_WC__(&res_CaDb, &tmp_Aa, &tmp_Bb);
    __BIGINT_INTERNAL_MOVE__(a, &res_AaBb); __BIGINT_INTERNAL_MOVE__(b, &res_CaDb);
    _free_alloc_list(alloc_list, alloc_cnt); return BIGINT_SUCCESS;
}





/** --------- Half-GCD Function ---------
 * This function, for all intend and purposes, is exclusively an internal function
 * utilized inside the "gcd_subq.c" file as a helper of __BIGINT_SUBQ__().
 * This function computes a transformation matrix T, in which when apply
 * onto the two inputs u and v, with sizes of N limbs, into floor(N/2) + 1,
 * or, more accurately, ceil(N/2). 
 */
dnml_status _hgcd_reduct(struct rt_matrix *T, bigInt *const a, bigInt *const b) {
    size_t n = a->n;
    // Base-cases
    if (!n) { // Return an identity matrix (a fully reduced --> GCD(0, b) == b)
        /* Identity matrix: */ _RT_MAT_FREE(T);
        /*  [1, 0] */ __BIGINT_INTERNAL_LINIT__(&T->A, 2); T->A.limbs[0] = 1; T->A.n = 1;
        /*  [0, 1] */ __BIGINT_INTERNAL_LINIT__(&T->D, 2); T->D.limbs[0] = 1; T->D.n = 1;
        __BIGINT_INTERNAL_LINIT__(&T->B, 0); __BIGINT_INTERNAL_LINIT__(&T->C, 0);
    }
    if (n == 1) { __hgcd1_base(T, a, b); return BIGINT_SUCCESS; }
    size_t m = n >> 1; // n / 2 (The predicted sizes of our elements in matrix T)
    if (b->n <= m) { // b is already small enough
        /* Identity matrix: */ _RT_MAT_FREE(T);
        /*  [1, 0] */ __BIGINT_INTERNAL_LINIT__(&T->A, 2); T->A.limbs[0] = 1; T->A.n = 1;
        /*  [0, 1] */ __BIGINT_INTERNAL_LINIT__(&T->D, 2); T->D.limbs[0] = 1; T->D.n = 1;
        __BIGINT_INTERNAL_LINIT__(&T->B, 0); __BIGINT_INTERNAL_LINIT__(&T->C, 0);
    }
    /* Standard Case */ dnml_status echeck = BIGINT_SUCCESS;
    bigInt *alloc_list[8], *early_free[12]; uint8_t alloc_cnt = 0, early_cnt = 0;
    struct rt_matrix R1 = {0}, R2 = {0}, temp_T = {0};
    early_free[early_cnt++] = &R1.A; early_free[early_cnt++] = &R1.B;
    early_free[early_cnt++] = &R1.C; early_free[early_cnt++] = &R1.D;
    // Recursive Calculation of R1
    bigInt ahi = { .limbs = a->limbs + m, .n = a->n - m, .cap = a->n - m, .sign = 1 };
    bigInt bhi = { .limbs = b->limbs + m, .n = b->n - m, .cap = b->n - m, .sign = 1 };
    _hgcd_reduct(&R1, &ahi, &bhi); HEAP_FOOM(echeck, early_free, early_cnt);

    // Intermediate Update of the full integer through R1
    __hgcd_matmul(a, b, &R1); HEAP_FOOM(echeck, early_free, early_cnt);
    if (b->n <= m) { _RT_MAT_MOVE(T, &R1); _free_alloc_list(alloc_list, alloc_cnt); return BIGINT_SUCCESS; }

    // Second Recursive Calculation on R2 with updated integer
    n = a->n; /**/ size_t new_m = n >> 1; // Recaculating n and m based on ab reduced size after the Euclidean's division step
    echeck = __BIGINT_INTERNAL_LINIT__(&temp_T.A, new_m + m + 1); HEAP_FOOM(echeck, early_free, early_cnt);     
    echeck = __BIGINT_INTERNAL_LINIT__(&temp_T.B, new_m + m + 1); HEAP_FOOM(echeck, early_free, early_cnt);    
    echeck = __BIGINT_INTERNAL_LINIT__(&temp_T.C, new_m + m + 1); HEAP_FOOM(echeck, early_free, early_cnt);   
    echeck = __BIGINT_INTERNAL_LINIT__(&temp_T.D, new_m + m + 1); HEAP_FOOM(echeck, early_free, early_cnt); 
    early_free[early_cnt++] = &temp_T.A; alloc_list[alloc_cnt++] = &R1.A;
    early_free[early_cnt++] = &temp_T.B; alloc_list[alloc_cnt++] = &R1.B;
    early_free[early_cnt++] = &temp_T.C; alloc_list[alloc_cnt++] = &R1.C;
    early_free[early_cnt++] = &temp_T.D; alloc_list[alloc_cnt++] = &R1.D;
    // Rewindow ahi and bhi to now match with truncated a and b after the Euclidean GCD division step
    ahi = (bigInt){ .limbs = a->limbs + m, .n = a->n - m, .cap = a->n - m, .sign = 1 };
    bhi = (bigInt){ .limbs = b->limbs + m, .n = b->n - m, .cap = b->n - m, .sign = 1 };
    _hgcd_reduct(&R2, &ahi, &bhi); HEAP_FOOM(echeck, early_free, early_cnt);
    early_free[early_cnt++] = &R2.A; alloc_list[alloc_cnt++] = &R2.A;
    early_free[early_cnt++] = &R2.B; alloc_list[alloc_cnt++] = &R2.B;
    early_free[early_cnt++] = &R2.C; alloc_list[alloc_cnt++] = &R2.C;
    early_free[early_cnt++] = &R2.D; alloc_list[alloc_cnt++] = &R2.D;
    __hgcd_matmul(a, b, &R2); HEAP_FOOM(echeck, early_free, early_cnt);
    __hgcd_mat_compose(&R1, &R2, &temp_T); HEAP_FOOM(echeck, early_free, early_cnt);
    _RT_MAT_MOVE(T, &temp_T); _free_alloc_list(alloc_list, alloc_cnt); return BIGINT_SUCCESS;
}



/* ---------- Main Orchestrating Function ---------- */
void __BIHEAP_SUBQ__(bigInt *const res, const bigInt *const u, const bigInt *const v, dnml_status *err) {
    // Allocate safe capacity amount for M's matrix elements (Half-GCD element upperbound is ceil(u->n / 2))
    struct rt_matrix M = {0}; dnml_status echeck = BIGINT_SUCCESS;
    bigInt *alloc_list[6] = {0}, *early_free[6] = {0};
    uint8_t alloc_cnt = 0, early_cnt = 0;
    early_free[early_cnt++] = &M.A; alloc_list[alloc_cnt++] = &M.A;
    early_free[early_cnt++] = &M.B; alloc_list[alloc_cnt++] = &M.B;
    early_free[early_cnt++] = &M.C; alloc_list[alloc_cnt++] = &M.C;
    early_free[early_cnt++] = &M.D; alloc_list[alloc_cnt++] = &M.D;
    // Allocate temporaries mirroring/mimicking u and v + a temporary 
    // These allocations solely acts as an initial starting point, and will be freed and replace later with move-semantics
    BIHEAP_RET(u_copy, u->n, echeck, err, early_free, early_cnt,); memcpy(u_copy.limbs, u->limbs, u->n * U64_BYTES);
    BIHEAP_TEMP(v_copy, v->n, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    memcpy(v_copy.limbs, v->limbs, v->n * U64_BYTES);
    // We assume and expect u >= v (AND when v == 0 ---> GCD(u, v) = u)
    while (u->n > BIGINT_LEHMER && v->n) {
        // Produces the linear-steps reduction matrices M --> Matrix Multiply on u and v to reduce their sizes in half
        // Every iteration, M is refreshed and allocate differently with a smaller element size
        echeck = _hgcd_reduct(&M, &u_copy, &v_copy); HEAP_OOM(echeck, err, early_free, early_cnt,);
        echeck = __hgcd_matmul(&u_copy, &v_copy, &M); HEAP_OOM(echeck, err, early_free, early_cnt,);
        if (!(v_copy.n)) break; // According to Euclidean GCD, when b == 0 (remainder == 0) --> GCD found


        /** Standard Euclidean Division for Correction - According to Schonhage's design following Euclidean GCD.
         * We reuse M.A as both the quotient and remainder buffer. This is safe due to:
         *
         *      1. M.A is allocated with a size of ceil(u->n / 2), and the Half-GCD reduction also reduced
         *         u_copy and v_copy in size by half, which makes M.A compatible to be the quotient and
         *         remainder buffer for our step
         *
         *      2. Using SOLELY M.A is still safe for our Modular Reduction routine since our Modular
         *         Reduction Dispatcher selects remainder-biased functions, in which they either
         *         do not touch anything with a quotient-buffer OR copy the remainder result
         *         AFTER the quotient copy so on the case of double-aliasing, we still end up with the remainder
         */
        __BIHEAP_MOD_DISP__(&u_copy, &v_copy, &M.A, &M.A, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
        __BIGINT_INTERNAL_MOVE__(&u_copy, &v_copy); __BIGINT_INTERNAL_MOVE__(&v_copy, &M.A);
    }

    // Fallback to Lehmer is fell below Subquadratic range for speed at lower ranges
    if (v_copy.n) { alloc_list[alloc_cnt++] = &u_copy;
        __BIHEAP_LEHMER__(res, &u_copy, &v_copy, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    } else __BIGINT_INTERNAL_MOVE__(res, &u_copy); // According to Euclidean GCD, if v_copy == 0 --> GCD found
    _free_alloc_list(alloc_list, alloc_cnt); *err = BIGINT_SUCCESS;
}
