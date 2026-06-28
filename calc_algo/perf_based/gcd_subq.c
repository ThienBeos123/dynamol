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
#include "hgcd_matmul.c"
#include "hgcd_matmul_toom.c"
struct rt_matrix {
    bigInt A; bigInt B;
    bigInt C; bigInt D;
}; // Workspace Returner
static inline size_t __BIGINT_HALF_WS__(size_t a_size, size_t b_size) { return 0; }
size_t __BIGINT_SUBQ_WS__(size_t u_size, size_t v_size) { return 0; }



/** ------------ 2-limb Base-case Half-GCD ------------
 * This function, for all intend and purposes, is exclusively an internal function
 * utilized inside the "gcd_subq.c" file as the base-case of __hgcd_reduct().
 * This function computes the base-case of __hgcd_reduct() to compute the Half-GCD
 * Transformation/Reduction 2x2 matrix T. This function computates exclusively inputs
 * a and b of 2-limbs long
 */
static void __hgcd2_base(
    struct rt_matrix *T,
    bigInt *const a, bigInt *const b, dnml_status *err
) {

}




/** ------------- 2x4 Cross Matrix-Vector Multiplication -------------
 * This function is reserved for computing the linear combination between a 2x2 matrix
 * and a 1x2 column vector. It is most notably used to compute the expression
 *
 *      a_new = Aa + Bb     WHERE A, B, C, D ∈ [A, B]   AND a,b ∈ [a]
 *      b_new = Ca + Db                        [C, D]             [b]
 */
static inline size_t __hgcd_matmul_ws(struct rt_matrix *T, size_t a_size, size_t b_size) {
    if ( // Toom-cook 3-way
        a_size <= BIGINT_TOOM_3 && b_size <= BIGINT_TOOM_3 && 
        ((T->A.n <= BIGINT_TOOM_3 && T->B.n <= BIGINT_TOOM_3) ||
         (T->C.n <= BIGINT_TOOM_3 && T->D.n <= BIGINT_TOOM_3))
    ) return max(
        __BIGINT_MAT_TOOM3_WS__(a_size, T->A.n, b_size, T->B.n), 
        __BIGINT_MAT_TOOM3_WS__(a_size, T->C.n, b_size, T->D.n)
    );
    // else if ( // Toom-cook 4-way
    //     a_size <= BIGINT_TOOM_4 && b_size <= BIGINT_TOOM_4 && 
    //     ((T->A.n <= BIGINT_TOOM_4 && T->B.n <= BIGINT_TOOM_4) ||
    //      (T->C.n <= BIGINT_TOOM_4 && T->D.n <= BIGINT_TOOM_4))
    // ) return max(
    //     __BIGINT_MAT_TOOM4_WS__(a_size, T->A.n, b_size, T->B.n), 
    //     __BIGINT_MAT_TOOM4_WS__(a_size, T->C.n, b_size, T->D.n)
    // );
    // else if ( // Toom-cook 5-way
    //     a_size <= BIGINT_TOOM_5 && b_size <= BIGINT_TOOM_5 && 
    //     ((T->A.n <= BIGINT_TOOM_5 && T->B.n <= BIGINT_TOOM_5) ||
    //      (T->C.n <= BIGINT_TOOM_5 && T->D.n <= BIGINT_TOOM_5))
    // ) return max(
    //     __BIGINT_MAT_TOOM5_WS__(a_size, T->A.n, b_size, T->B.n), 
    //     __BIGINT_MAT_TOOM5_WS__(a_size, T->C.n, b_size, T->D.n)
    // );
    // else if ( // Toom-cook 6.5-way
    //     a_size <= BIGINT_TOOM_6p5 && b_size <= BIGINT_TOOM_6p5 && 
    //     ((T->A.n <= BIGINT_TOOM_6p5 && T->B.n <= BIGINT_TOOM_6p5) ||
    //      (T->C.n <= BIGINT_TOOM_6p5 && T->D.n <= BIGINT_TOOM_6p5))
    // ) return max(
    //     __BIGINT_MAT_TOOM6p5_WS__(a_size, T->A.n, b_size, T->B.n), 
    //     __BIGINT_MAT_TOOM6p5_WS__(a_size, T->C.n, b_size, T->D.n)
    // );
    // else if ( // Toom-cook 7.5-way
    //     a_size <= BIGINT_TOOM_7p5 && b_size <= BIGINT_TOOM_7p5 && 
    //     ((T->A.n <= BIGINT_TOOM_7p5 && T->B.n <= BIGINT_TOOM_7p5) ||
    //      (T->C.n <= BIGINT_TOOM_7p5 && T->D.n <= BIGINT_TOOM_7p5))
    // ) return max(
    //     __BIGINT_MAT_TOOM7p5_WS__(a_size, T->A.n, b_size, T->B.n), 
    //     __BIGINT_MAT_TOOM7p5_WS__(a_size, T->C.n, b_size, T->D.n)
    // );
    // else if ( // Toom-cook 8.5-way
    //     a_size <= BIGINT_TOOM_8p5 && b_size <= BIGINT_TOOM_8p5 && 
    //     ((T->A.n <= BIGINT_TOOM_8p5 && T->B.n <= BIGINT_TOOM_8p5) ||
    //      (T->C.n <= BIGINT_TOOM_8p5 && T->D.n <= BIGINT_TOOM_8p5))
    // ) return max(
    //     __BIGINT_MAT_TOOM8p5_WS__(a_size, T->A.n, b_size, T->B.n), 
    //     __BIGINT_MAT_TOOM8p5_WS__(a_size, T->C.n, b_size, T->D.n)
    // );
    else return max(
        __BIGINT_MAT_SSA_WS__(a_size, T->A.n, b_size, T->B.n), 
        __BIGINT_MAT_SSA_WS__(a_size, T->C.n, b_size, T->D.n)
    );
}
static dnml_status __hgcd_matmul(
    bigInt *const a, bigInt *const b, 
    struct rt_matrix *T
) {
    
    return BIGINT_SUCCESS;
}




/** --------- Half-GCD Function ---------
 * This function, for all intend and purposes, is exclusively an internal function
 * utilized inside the "gcd_subq.c" file as a helper of __BIGINT_SUBQ__().
 * This function computes a transformation matrix T, in which when apply
 * onto the two inputs u and v, with sizes of N limbs, into floor(N/2) + 1,
 * or, more accurately, ceil(N/2). 
 */
static void _hgcd_reduct(
    struct rt_matrix *T, 
    bigInt *const a, bigInt *const b, 
    calc_ctx hgcd_ctx, dnml_status *err
) {

}



/* ---------- Main Orchestrating Function ---------- */
void __BIGINT_SUBQ__(P_BIGINT res, PCONST_BIGINT u, PCONST_BIGINT v, calc_ctx subq_ctx, dnml_status *err) {



}
