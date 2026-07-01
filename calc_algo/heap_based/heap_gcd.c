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
/** ----------- Heap-based BigInt Greatest Cmmmon Divisor -----------
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
 *      - heap_gcd_subq.c (Implementation of Subquadratic GCD through Half-GCD)
 *      - hmv_matmul/_hmv_matmul_disp.c (Algorithm Dispatcher of Matrix-Vector Multiplication Algorithms)
 *      - hmv_matmul/_hmv_matmul_ws.c (Workspace Size Returning Dispatcher of Matrix-Vector Multiplication Algorithms)
 *      - ...
 *
 * For the ... at the end, it basically means that every single file inside /mv_matmul is, considerably,
 * under the direct implementation scope of GCD, as it was initially implemented as efficient
 * algorithms for calculating the transformation matrix T for Half-GCD, but was then generalized for future uses (xGCD)
 */



//* ======== GCD - ALGORITHMS RETURNER ======== */
static uint64_t __BINGCD_U64__(uint64_t u, uint64_t v) {
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
void __BIHEAP_STEIN__(P_BIGINT res, PCONST_BIGINT u, PCONST_BIGINT v, dnml_status *err) {
    // Base case - Identity #1 - gcd(u, 0) = u
    if (u->n == 0) { __BIGINT_INTERNAL_LINIT__(res, v->n); __BIGINT_INTERNAL_COPY__(res, v); return; }
    else if (v->n == 0) { __BIGINT_INTERNAL_LINIT__(res, u->n); __BIGINT_INTERNAL_COPY__(res, u); return; }

    // Setup - Identity #2 - gcd(2u, 2v) = gcd(u, v)
    dnml_status echeck = BIGINT_SUCCESS;
    bigInt *alloc_list[1], *early_free[2]; uint8_t alloc_cnt = 0, early_cnt = 0;
    size_t maxsize = max(u->n, v->n); // maxsize is used for SWAP
    BIHEAP_RET(u_copy, maxsize, echeck, err, early_free, early_cnt,); u_copy.n = u->n;
    BIHEAP_TEMP(v_copy, maxsize, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,); v_copy.n = v->n;
    memcpy(u_copy.limbs, u->limbs, u->n * U64_BYTES); memcpy(v_copy.limbs, v->limbs, v->n * U64_BYTES);
    size_t i = __BIGINT_CTZ__(u); __BIGINT_INTERNAL_RSHIFT__(&u_copy, i);
    size_t j = __BIGINT_CTZ__(v); __BIGINT_INTERNAL_RSHIFT__(&v_copy, j);
    size_t k = min(i, j);

    // Procedure
    int8_t comp_res = __BIGINT_INTERNAL_COMP__(&u_copy, &v_copy);
    while (comp_res) {
        if (comp_res < 0) __BIGINT_INTERNAL_SWAP__(&u_copy, &v_copy);
        // Identity #4: gcd(u, v) = gcd(u, v - u)
        //  WHEN: (u & v is ODD) AND (u <= v)
        __BIGINT_SUB_WB__(&u_copy, &u_copy, &v_copy);
        // Identity #3 - gcd(u, 2v) = gcd(u, v)
        i = __BIGINT_CTZ__(&u_copy); __BIGINT_INTERNAL_RSHIFT__(&u_copy, i);
        comp_res = __BIGINT_INTERNAL_COMP__(&u_copy, &v_copy);
    } __BIGINT_INTERNAL_LSHIFT__(&u_copy, k); __BIGINT_INTERNAL_MOVE__(res, &u_copy);
    _free_alloc_list(alloc_list, alloc_cnt); *err = BIGINT_SUCCESS;
}
void __BIHEAP_LEHMER__(P_BIGINT res, PCONST_BIGINT u, PCONST_BIGINT v, dnml_status *err) {
    // Setup
    dnml_status echeck = BIGINT_SUCCESS; size_t max_size = max(u->n + 1, v->n + 1) + 1;
    bigInt *alloc_list[4] = {0}, *early_free[6] = {0}; uint64_t alloc_cnt = 0, early_cnt = 0;
    BIHEAP_RET(u_copy, max_size, echeck, err, early_free, early_cnt,);
    BIHEAP_TEMP(u_tmp1, u->n + 1, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    BIHEAP_TEMP(u_tmp2, max_size, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    BIHEAP_RET(v_copy, max_size, echeck, err, early_free, early_cnt,);
    BIHEAP_TEMP(v_tmp1, v->n + 1, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    BIHEAP_TEMP(v_tmp2, max_size, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    memcpy(u_copy.limbs, u->limbs, u->n << 6); u_copy.n = u->n;
    memcpy(v_copy.limbs, v->limbs, v->n << 6); v_copy.n = v->n;

    // Main Loop
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
        __BIHEAP_SCHOOLBOOK__(&u_copy, &matrix_view, &u_tmp1); matrix_view.limbs = &b; matrix_view.n = !!(b); 
        __BIHEAP_SCHOOLBOOK__(&v_copy, &matrix_view, &v_tmp1); __BIGINT_ADD_WC__(&u_tmp1, &v_tmp1, &u_tmp2);
        /* Updating matrix views -> Store in v_tmp2 */ matrix_view.limbs = &c; matrix_view.n = !!(c);
        __BIHEAP_SCHOOLBOOK__(&u_copy, &matrix_view, &u_tmp1); matrix_view.limbs = &d; matrix_view.n = !!(d);
        __BIHEAP_SCHOOLBOOK__(&v_copy, &matrix_view, &v_tmp1); __BIGINT_ADD_WC__(&u_tmp1, &v_tmp1, &v_tmp2);

        /* Updating u_copy and v_copy from u_tmp2 and v_tmp2 */
        // Shallow struct swaps here is desirable and safe due to u_copy and u_tmp2 (and therefore
        // subsequently v_cpopy and v_tmp2) sharing the same arena buffer size, and we're
        // basically trashing away the buffer values inside u_tmp2 and v_tmp2 after operation anyways
        __BIGINT_INTERNAL_SWAP__(&u_copy, &u_tmp2); __BIGINT_INTERNAL_SWAP__(&v_copy, &v_tmp2);
    }
    // Whatever non-zero value remains is the GCD (according to Euclidean GCD algo)
    if (!u_copy.n) { __BIGINT_INTERNAL_MOVE__(res, &u_copy); alloc_list[alloc_cnt++] = &v_copy; }
    else { __BIGINT_INTERNAL_MOVE__(res, &v_copy); alloc_list[alloc_cnt++] = &u_copy; }
    _free_alloc_list(alloc_list, alloc_cnt); *err = BIGINT_SUCCESS;
}
void __BIHEAP_GCD_DISP__(P_BIGINT res, PCONST_BIGINT u, PCONST_BIGINT v, dnml_status *err) {
    size_t op_size = min(u->n, v->n);
    if (u->n == 1 && v->n == 1) { res->limbs[0] = __BINGCD_U64__(u->limbs[0], v->limbs[0]); res->n = 1; }
    else if (op_size <= BIGINT_STEIN) __BIHEAP_STEIN__(res, u, v, err);
    else if (op_size <= BIGINT_LEHMER) __BIHEAP_LEHMER__(res, u, v, err);
    else __BIHEAP_SUBQ__(res, u, v, err);
}