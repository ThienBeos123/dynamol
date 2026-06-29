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
/** ----------- General BigInt Greatest Cmmmon Divisor -----------
 * THIS FILE CONTAINS THE FOLLOWING ALGORITHMS:
 *
 *      - 64-bit Binary GCD (64-bit operands only)
 *      - Binary/Stein GCd (General)
 *      - Lehmer GCD (General)
 *
 * This file is generally the main algorithm file for bigInt multiplication, containing
 * the multiplication algorithm dispatcher, as well as the workspace sizing function dispatcher.
 * It only contains the 2 simplest multiplication algorithms to keep its focus of being the central,
 * simple point of authority, and delegation of complexity is in other files, including:
 *
 *      - gcd_subq.c (Implementation of Subquadratic GCD through Half-GCD)
 *      - mv_matmul/_mv_matmul_disp.c (Algorithm Dispatcher of Matrix-Vector Multiplication Algorithms)
 *      - mv_matmul/_mv_matmul_ws.c (Workspace Size Returning Dispatcher of Matrix-Vector Multiplication Algorithms)
 *      - ...
 *
 * For the ... at the end, it basically means that every single file inside /mv_matmul is, considerably,
 * under the direct implementation scope of GCD, as it was initially implemented as efficient
 * algorithms for calculating the transformation matrix T for Half-GCD, but was then generalized for future uses (xGCD)
 */
/* ======== GCD - WORKSPACE RETURNER ======== */
size_t __BIGINT_STEIN_WS__(size_t u_size, size_t v_size) { 
    // This function doesn't utilize any external function call 
    // requiring arena allocation for temporaries
    return u_size + v_size;
}
size_t __BIGINT_LEHMER_WS__(size_t u_size, size_t v_size) {
    // This function doesn't utilize any external function call 
    // requiring arena allocation for temporaries
    size_t max_size = max(u_size + 1, v_size + 1) + 1;
    size_t u_tmps = (max_size << 1) + (u_size + 1);
    size_t v_tmps = (max_size << 1) + (v_size + 1);
    return u_tmps + v_tmps;
} 
size_t __BIGINT_GCD_WS__(size_t u_size, size_t v_size) {
    if (u_size == 1 && v_size == 1) return 0; // Euclid 64 bit doesn't require arena
    size_t op_size = min(u_size, v_size);
    if (op_size <= BIGINT_STEIN) return __BIGINT_STEIN_WS__(u_size, v_size);
    else if (op_size <= BIGINT_LEHMER) return __BIGINT_LEHMER_WS__(u_size, v_size);
    else return __BIGINT_SUBQ_WS__(u_size, v_size);
}



/* ======== GCD - ALGORITHMS ======== */
uint64_t __BINARY_GCDU64__(uint64_t u, uint64_t v) {
    // GCD(u, 0) == u && GCD(0, v) == v
    if (!u) return v; /**/ if (!v) return u;
    // GCD(2u, 2v) = GCD(u, v)
    uint8_t i = __CTZ_UI64__(u); u >>= i;
    uint8_t j = __CTZ_UI64__(v); v >>= j;
    uint8_t k = min(i, j);
    while (u != v) {
        if (u < v) { uint64_t tmp = v; u = v; v = u; }
        u -= v; // gcd(u, v) == gcd(u, v - u) WHEN (u & v == ODD) && (u <= v)
        u >>= __CTZ_UI64__(u); // gcd(u, 2v) == gcd(u, v)
    } return u;
}
void __BIGINT_STEIN__(P_BIGINT res, PCONST_BIGINT u, PCONST_BIGINT v, calc_ctx stein_ctx, dnml_status *err) {
    // Base case - Identity #1 - gcd(u, 0) = u
    if (u->n == 0) { __BIGINT_INTERNAL_COPY__(res, v); return; }
    else if (v->n == 0) { __BIGINT_INTERNAL_COPY__(res, u); return; }

    // Setup - Identity #2 - gcd(2u, 2v) = gcd(u, v)
    dnml_status echeck;
    size_t stein_mark = scratch_mark(&stein_ctx), maxsize = max(u->n, v->n); // maxsize is used for SWAP
    BIGINT_TEMP(u_copy, maxsize, stein_ctx, stein_mark, echeck, err,); u_copy.n = u->n;
    BIGINT_TEMP(v_copy, maxsize, stein_ctx, stein_mark, echeck, err,); v_copy.n = v->n;
    memcpy(u_copy.limbs, u->limbs, u->n * U64_BYTES); memcpy(v_copy.limbs, v->limbs, v->n * U64_BYTES);
    size_t i = __BIGINT_CTZ__(u); __BIGINT_INTERNAL_RSHIFT__(&u_copy, i);
    size_t j = __BIGINT_CTZ__(v); __BIGINT_INTERNAL_RSHIFT__(&v_copy, j); 
    size_t k = min(i, j);

    // Procedure
    int8_t comp_res = __BIGINT_INTERNAL_COMP__(&u_copy, &v_copy);
    while (comp_res) {
        if (comp_res == -1) __BIGINT_INTERNAL_SWAP__(&u_copy, &v_copy);
        // Identity #4: gcd(u, v) == gcd(u, v - u)
        //  WHEN: (u & v is ODD) && (u <= v)
        __BIGINT_SUB_WB__(&u_copy, &u_copy, &v_copy);
        // Identity #3 - gcd(u, 2v) == gcd(u, v)
        i = __BIGINT_CTZ__(&u_copy);
        __BIGINT_INTERNAL_RSHIFT__(&u_copy, i);
        comp_res = __BIGINT_INTERNAL_COMP__(&u_copy, &v_copy);
    }
    __BIGINT_INTERNAL_LSHIFT__(&u_copy, k); __BIGINT_INTERNAL_COPY__(res, &u_copy);
    scratch_rewind(&stein_ctx, stein_mark); *err = BIGINT_SUCCESS;
}
void __BIGINT_LEHMER__(P_BIGINT res, PCONST_BIGINT u, PCONST_BIGINT v, calc_ctx lehmer_ctx, dnml_status *err) {
    if (u->n == 0) { __BIGINT_INTERNAL_COPY__(res, v); return; }
    else if (v->n == 0) { __BIGINT_INTERNAL_COPY__(res, u); return; }

    // Setup
    dnml_status echeck; size_t lehmer_mark = scratch_mark(&lehmer_ctx);
    size_t max_size = max(u->n + 1, v->n + 1) + 1;
    BIGINT_TEMP(u_copy, max_size, lehmer_ctx, lehmer_mark, echeck, err,);
    BIGINT_TEMP(u_tmp1, u->n + 1, lehmer_ctx, lehmer_mark, echeck, err,);
    BIGINT_TEMP(u_tmp2, max_size, lehmer_ctx, lehmer_mark, echeck, err,);
    BIGINT_TEMP(v_copy, max_size, lehmer_ctx, lehmer_mark, echeck, err,);
    BIGINT_TEMP(v_tmp1, v->n + 1, lehmer_ctx, lehmer_mark, echeck, err,);
    BIGINT_TEMP(v_tmp2, max_size, lehmer_ctx, lehmer_mark, echeck, err,);
    memcpy(u_copy.limbs, u->limbs, u->n << 6); u_copy.n = u->n;
    memcpy(v_copy.limbs, v->limbs, v->n << 6); v_copy.n = v->n;

    // Main Loop
    while (u_copy.n && v_copy.n) {
        limb_t uhat = u_copy.limbs[u_copy.n - 1];
        limb_t vhat = v_copy.limbs[v_copy.n - 1];
        limb_t a = 1, b = 0, c = 0, d = 1;
        // Identity matrix:
        //      [1, 0, uhat]
        //      [0, 1, vhat]
        while (uhat + c && vhat + d) {
            limb_t w1 = (uhat + a) / (vhat + c); // Absolute top quotient (uhat + 1) / (vhat + 1)
            limb_t w2 = (uhat + b) / (vhat + d); // Absolute bottom quotient (uhat) / (vhat)
            if (w1 != w2) break; // Top limb approximation diverges --> Correction NOW

            // Updating the single-word limb variables
            // Matrix Product:
            // [0,  1] . [A, B, uhat] = [   C,      D,        vhat     ]
            // [1, -w]   [C, D, vhat]   [ A - wC, B - wD, uhat - Wvhat ]
            limb_t uhat_tmp = vhat, /**/ vhat_tmp = uhat - w1 * vhat;
            uhat = uhat_tmp; /**/ vhat = vhat_tmp;
            // Updating the matrix approximation variables
            limb_t new_a = c, /**/ new_b = d;
            limb_t new_c = a - w1 * c;
            limb_t new_d = b - w1 * d;
            a = new_a; b = new_b; c = new_c; d = new_d;
        }

        // u_copy = (u_copy * a) + (v_copy * b)
        // v_copy = (u_copy * c) + (v_copy * d)
        bigInt matrix_view = { .limbs = &a, .n = !!(a), .cap = 1, .sign = 1 }; // A, B, C, and D share this
        /* Updating matrix views -> Store in u_tmp2 */
        __BIGINT_SCHOOLBOOK__(&u_copy, &matrix_view, &u_tmp1); matrix_view.limbs = &b; matrix_view.n = !!(b); 
        __BIGINT_SCHOOLBOOK__(&v_copy, &matrix_view, &v_tmp1); __BIGINT_ADD_WC__(&u_tmp1, &v_tmp1, &u_tmp2);
        /* Updating matrix views -> Store in v_tmp2 */ matrix_view.limbs = &c; matrix_view.n = !!(c);
        __BIGINT_SCHOOLBOOK__(&u_copy, &matrix_view, &u_tmp1); matrix_view.limbs = &d; matrix_view.n = !!(d);
        __BIGINT_SCHOOLBOOK__(&v_copy, &matrix_view, &v_tmp1); __BIGINT_ADD_WC__(&u_tmp1, &v_tmp1, &v_tmp2);

        /* Updating u_copy and v_copy from u_tmp2 and v_tmp2 */
        // Shallow struct swaps here is desirable and safe due to u_copy and u_tmp2 (and therefore
        // subsequently v_cpopy and v_tmp2) sharing the same arena buffer size, and we're
        // basically trashing away the buffer values inside u_tmp2 and v_tmp2 after operation anyways
        __BIGINT_INTERNAL_SWAP__(&u_copy, &u_tmp2); __BIGINT_INTERNAL_SWAP__(&v_copy, &v_tmp2);
    }
    // Whatever non-zero value remains is the GCD (according to Euclidean GCD algo)
    if (!u_copy.n) __BIGINT_INTERNAL_COPY__(res, &u_copy);
    else __BIGINT_INTERNAL_COPY__(res, &v_copy);
    scratch_rewind(&lehmer_ctx, lehmer_mark); *err = BIGINT_SUCCESS;
}
void __BIGINT_GCD_DISP__(P_BIGINT res, PCONST_BIGINT u, PCONST_BIGINT v, calc_ctx gcd_ctx, dnml_status *err) {
    size_t op_size = min(u->n, v->n);
    if (u->n == 1 && v->n == 1) { res->limbs[0] = __BINARY_GCDU64__(u->limbs[0], v->limbs[0]); res->n = 1; }
    else if (op_size <= BIGINT_STEIN) __BIGINT_STEIN__(res, u, v, gcd_ctx, err);
    else if (op_size <= BIGINT_LEHMER) __BIGINT_LEHMER__(res, u, v, gcd_ctx, err);
    else __BIGINT_SUBQ__(res, u, v, gcd_ctx, err);
}
