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
// Workspace Returner
static inline size_t __hgcd_ws(bigInt *const a, bigInt *const b, struct rt_matrix *T) { return 0; }
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
