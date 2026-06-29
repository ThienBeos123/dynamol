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
uint64_t __BIHEAP_EUCLID__(uint64_t u, uint64_t v) {
    uint64_t remainder = (u < v) ? u : v;
    uint64_t dividend = (u >= v) ? u : v;
    uint64_t old_remainder;
    while (remainder) {
        old_remainder = remainder;
        remainder = dividend % remainder;
        dividend = old_remainder;
    } return dividend;
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
void __BIHEAP_LEHMER__(P_BIGINT res, PCONST_BIGINT u, PCONST_BIGINT v, dnml_status *err) {}
void __BIHEAP_GCD_DISP__(P_BIGINT res, PCONST_BIGINT u, PCONST_BIGINT v, dnml_status *err) {}