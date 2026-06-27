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



#include "heap_mul.h"
#include <debug_util.h>
#include "../../util/aconv_macros.h"
/* ============== BIGINT ALGORITHMS ============== */
/* BigInt Schoolbook Multiplication - Heap-allocating */
void __BIHEAP_SCHOOLBOOK__(PCONST_BIGINT a, PCONST_BIGINT b, P_BIGINT res) {
    memset(res->limbs, 0, a->n + b->n);
    for (size_t i = 0; i < a->n; ++i) { uint64_t carry = 0;
        for (size_t j = 0; j < b->n; ++j) { uint64_t lo, hi;
            lo = __MUL_UI64__(a->limbs[i], b->limbs[j], &hi);
            uint64_t sum = res->limbs[i + j] + lo; uint8_t c1 = (sum < lo);
            uint64_t sum2 = sum + carry; uint8_t c2 = (sum2 < carry);
            carry = hi + (c1 | c2); res->limbs[i + j] = sum2;
        } res->limbs[i + b->n] += carry; // Add the remaining carry to the MSL
    } res->n = a->n + b->n; __BIGINT_INTERNAL_TRIM_LZ__(res);
}


/* BigInt Karatsuba Multiplication - Heap-allocating */
void __BIHEAP_KARATSUBA__(PCONST_BIGINT x, PCONST_BIGINT y, P_BIGINT res, dnml_status *err) {
    if (x->n <= BIGINT_SCHOOLBOOK || y->n <= BIGINT_SCHOOLBOOK) {
        dnml_status echeck = __BIGINT_INTERNAL_LINIT__(res, x->n + y->n);
        if (echeck == DNML_ALLOC_OOM) { *err = DNML_ALLOC_OOM; return; }
        __BIHEAP_SCHOOLBOOK__(x, y, res); *err = BIGINT_SUCCESS; return;
    } 
    //* ---- 1. SETUP ---- *//
    size_t m = (size_t)(max(x->n, y->n) / 2);
    size_t  x0_range = m,  x1_range = x->n - m;
    size_t  y0_range = m,  y1_range = y->n - m;
    bigInt x0 = {.limbs = x->limbs,             .n = x0_range, .cap = x0_range};
    bigInt x1 = {.limbs = x->limbs + x0_range,  .n = x1_range, .cap = x1_range};
    bigInt y0 = {.limbs = y->limbs,             .n = y0_range, .cap = y0_range};
    bigInt y1 = {.limbs = y->limbs + y0_range,  .n = y1_range, .cap = y1_range};

    dnml_status echeck = BIGINT_SUCCESS; bigInt *early_free[5];
    bigInt *alloc_arr[4]; uint8_t alloc_cnt = 0; uint8_t early_cnt = 0;
    size_t tmp1_size = max(max(x0_range, x1_range) + 1, x->n + y->n + 1);
    BIHEAP_RET(tmp1, tmp1_size, echeck, err, early_free, early_cnt,); // Will also be used as the prod in FINAL CALCULATION
    BIHEAP_TEMP(tmp2, max(y0_range, y1_range) + 1, echeck, err, early_free, early_cnt, alloc_arr, alloc_cnt,);
    bigInt z0 = {0}, z1 = {0}, z2 = {0}; // These are allocated at the base-case of BIGINT_SCHOOLBOOK
    alloc_arr[alloc_cnt++] = &z0; early_free[early_cnt++] = &z0; // Sign up z0 for both final cleanup and early cleanup
    alloc_arr[alloc_cnt++] = &z1; early_free[early_cnt++] = &z1; // Sign up z1 for both final cleanup and early cleanup
    alloc_arr[alloc_cnt++] = &z2; early_free[early_cnt++] = &z2; // Sign up z2 for both final cleanup and early cleanup


    //* ------- 2. QUADRATIC COMPONENTS CALCULATION -------- *//
    // The procedure goes:
    //  z3 = (x1 + x0)(y1 + y0)
    //  z2 = x1 * y1
    //  z0 = x0 * y0
    //  z1 = z3 - z2 - z1
    dnml_status rec_err = BIGINT_SUCCESS;
    __BIHEAP_KARATSUBA__(&x0, &y0, &z0, &rec_err); HEAP_OOM(echeck, err, early_free, early_cnt,);
    __BIHEAP_KARATSUBA__(&x1, &y1, &z2, &rec_err); HEAP_OOM(echeck, err, early_free, early_cnt,);
    __BIGINT_ADD_WC__(&tmp1, &x1, &x0); __BIGINT_ADD_WC__(&tmp2, &y1, &y0);
    __BIHEAP_KARATSUBA__(&tmp1, &tmp2, &z1, &rec_err); HEAP_OOM(echeck, err, early_free, early_cnt,);
    __BIGINT_SUB_WB__(&z1, &z1, &z2); __BIGINT_SUB_WB__(&z1, &z1, &z0);

    //* ------------ 3. FINAL CALCULATION -------------- *//
    memset(tmp1.limbs, 0, tmp1_size * U64_BYTES);
    __BIGINT_ADD_SHIFT__(&tmp1, &z0, 0); __BIGINT_ADD_SHIFT__(&tmp1, &z1, m);
    __BIGINT_ADD_SHIFT__(&tmp1, &z2, m << 1); __BIGINT_INTERNAL_MOVE__(res, &tmp1); 
    _free_alloc_list(alloc_arr, alloc_cnt); *err = BIGINT_SUCCESS;
}


/* BigInt Toom 3-way Multiplication - Heap-allocating */
void __BIHEAP_TOOM_3__(PCONST_BIGINT m, PCONST_BIGINT n, P_BIGINT res, dnml_status *err) {
    if (m->n <= BIGINT_SCHOOLBOOK || n->n <= BIGINT_SCHOOLBOOK) {
        dnml_status echeck = __BIGINT_INTERNAL_LINIT__(res, m->n + n->n);
        if (echeck == DNML_ALLOC_OOM) { *err = DNML_ALLOC_OOM; return; }
        __BIHEAP_SCHOOLBOOK__(m, n, res); *err = BIGINT_SUCCESS; return;
    } //* -------- 1. SETUP & SPLITTING -------- *//
    size_t k = (size_t)(max(m->n, n->n) / 3) + 1;
    size_t m2size = (m->n > (k << 1)) ? (m->n - (k << 1)) : 0;
    size_t n2size = (n->n > (k << 1)) ? (n->n - (k << 1)) : 0;
    bigInt m0 = {.limbs = m->limbs,             .n = k,         .cap = k};
    bigInt m1 = {.limbs = m->limbs + k,         .n = k,         .cap = k};
    bigInt m2 = {.limbs = m->limbs + (k << 1),  .n = m2size,    .cap = m2size};
    bigInt n0 = {.limbs = n->limbs,             .n = k,         .cap = k};
    bigInt n1 = {.limbs = n->limbs + k,         .n = k,         .cap = k};
    bigInt n2 = {.limbs = n->limbs + (k << 1),  .n = n2size,    .cap = n2size};


    //* -------- 2. EVALUATION & POINT-WISE MULTIPLICATION -------- *//
    dnml_status echeck = BIGINT_SUCCESS;
    bigInt *early_free[14]; uint8_t early_cnt = 0;
    bigInt *alloc_arr[13]; uint8_t alloc_cnt = 0;
    /*  ---------------------------------- EVALUATION ------------------------------ */
    /* p(x) TEMPORARIES 
    * +) pOuter = m0 + m2                   | +) p(-1)  = pOuter - m1
    * +) p(0)   = m0 (NO FULL TEMPORARY)    | +) p(-2)  = 2*(p(-1) + m2) - m0
    * +) p(1)   = pOuter + m1               | +) p(inf) = m2 (NO FULL TEMPORARY)
    */
    BIHEAP_TEMP(p_outer, k + 1, echeck, err, early_free, early_cnt, alloc_arr, alloc_cnt,);
    BIHEAP_TEMP(p1,      k + 2, echeck, err, early_free, early_cnt, alloc_arr, alloc_cnt,);
    BIHEAP_TEMP(p_neg1,  k + 1, echeck, err, early_free, early_cnt, alloc_arr, alloc_cnt,);
    BIHEAP_TEMP(p_neg2,  k + 2, echeck, err, early_free, early_cnt, alloc_arr, alloc_cnt,);
    /* q(x) TEMPORARIES 
    * +) qOuter = n0 + n2                   | +) q(-1)  = qOuter - n1
    * +) q(0)   = n0 (NO FULL TEMPORARY)    | +) q(-2)  = 2*(q(-1) + n2) - n0
    * +) q(1)   = qOuter + n1               | +) q(inf) = n2 (NO FULL TEMPORARY) 
    */
    // q_neg2 will be used later in the recomposition step as the final product carrier
    BIHEAP_TEMP(q_outer, k + 1, echeck, err, early_free, early_cnt, alloc_arr, alloc_cnt,);
    BIHEAP_TEMP(q1,      k + 2, echeck, err, early_free, early_cnt, alloc_arr, alloc_cnt,);
    BIHEAP_TEMP(q_neg1,  k + 1, echeck, err, early_free, early_cnt, alloc_arr, alloc_cnt,);
    BIHEAP_TEMP(q_neg2, (k << 1) + 14, echeck, err, early_free, early_cnt, alloc_arr, alloc_cnt,); // Actual cap: k + 2
    // p(x) CALCULATIONS                            // q(x) CALCULATIONS
    __BIGINT_ADD_WC__(&p_outer, &m0, &m2);          __BIGINT_ADD_WC__(&q_outer, &m0, &n2);
    __BIGINT_ADD_WC__(&p1, &p_outer, &m1);          __BIGINT_ADD_WC__(&q1, &q_outer, &n1);
    __BIGINT_SUB_SAW__(&p_neg1, &p_outer, &m1);     __BIGINT_SUB_SAW__(&q_neg1, &q_outer, &n1);
    __BIGINT_ADD_SAW__(&p_neg2, &p_neg1, &m2);      __BIGINT_ADD_SAW__(&q_neg2, &q_neg1, &n2);
    __BIGINT_INTERNAL_LSHIFT__(&p_neg2, 1);         __BIGINT_INTERNAL_LSHIFT__(&q_neg2, 1);
    __BIGINT_SUB_SAW__(&p_neg2, &p_neg2, &m0);      __BIGINT_SUB_SAW__(&q_neg2, &q_neg2, &n0);
    /* ------------ POINT-WISE MULTIPLICATION ------------
    *   +) r(0)   = p(0)   * q(0)
    *   +) r(1)   = p(1)   * q(1) ------> p(1)  *= q(1)
    *   +) r(-1)  = p(-1)  * q(-1) -----> p(-1) *= q(-1)
    *   +) r(-2)  = p(-2)  * q(-2) -----> p(-2) *= q(-2)
    *   +) r(inf) = p(inf) * q(inf) */
    dnml_status rec_err = BIGINT_SUCCESS;
    bigInt r0 = {0}, r1 = {0}, r_neg1 = {0}, r_neg2 = {0}, rinf = {0};
    alloc_arr[alloc_cnt++] = &r0; early_free[early_cnt++] = &r0;
    alloc_arr[alloc_cnt++] = &r1; early_free[early_cnt++] = &r1;
    alloc_arr[alloc_cnt++] = &r_neg1; early_free[early_cnt++] = &r_neg1;
    alloc_arr[alloc_cnt++] = &r_neg2; early_free[early_cnt++] = &r_neg2;
    alloc_arr[alloc_cnt++] = &rinf; early_free[early_cnt++] = &rinf;
    __BIHEAP_TOOM_3__(&m0, &n0, &r0, &rec_err); HEAP_OOM(echeck, err, early_free, early_cnt,);
    __BIHEAP_TOOM_3__(&p1, &q1, &r1, &rec_err); HEAP_OOM(echeck, err, early_free, early_cnt,);
    __BIHEAP_TOOM_3__(&p_neg1, &q_neg1, &r_neg1, &rec_err); HEAP_OOM(echeck, err, early_free, early_cnt,);
    __BIHEAP_TOOM_3__(&p_neg2, &q_neg2, &r_neg2, &rec_err); HEAP_OOM(echeck, err, early_free, early_cnt,);
    __BIHEAP_TOOM_3__(&m2, &n2, &rinf, &rec_err); HEAP_OOM(echeck, err, early_free, early_cnt,);
    __BIGINT_INTERNAL_ENSCAP__(&r1, (k << 1) + 8); HEAP_OOM(echeck, err, early_free, early_cnt,); // 2k + 4 (original) --> 2k + 8 (interpolation - r1)
    __BIGINT_INTERNAL_ENSCAP__(&r_neg1, (k << 1) + 7); HEAP_OOM(echeck, err, early_free, early_cnt,); // 2k + 2 (original) --> 2k + 7 (interpolation - r2)
    __BIGINT_INTERNAL_ENSCAP__(&r_neg2, (k << 1) + 7); HEAP_OOM(echeck, err, early_free, early_cnt,); // 2k + 4 (original) --> 2k + 7 (interpolation - r3)
    __BIGINT_INTERNAL_ENSCAP__(&rinf, m2size + n2size + 4); HEAP_OOM(echeck, err, early_free, early_cnt,); // 2k (original) ---> 2k + 4 (bit-shifts accounted)


    //* ------------- 3. INTERPOLATION & RECOMPOSITION ---------------- *//
    // ------------------ INTERPOLATION ------------------ //
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
    // <<< is equivalent to a left limb shift (__BIGINT_INTERNAL_LLSHIFT__)
    // We now REUSE one random temporary from the point-based evaluation steps.
    // We chose q_neg2 for the least memory footprint due to it having the largest size in the evaluation step
    __BIGINT_ADD_SHIFT__(&q_neg2, &rinf, 4); // final_res += (rinf <<< 4)
    __BIGINT_ADD_SHIFT__(&q_neg2, &r_neg2, 3); // final_res += (r_neg2 <<< 3)
    __BIGINT_ADD_SHIFT__(&q_neg2, &r_neg1, 2); // final_res += (r_neg1 <<< 2)
    __BIGINT_ADD_SHIFT__(&q_neg2, &r1, 1); // final_res += (r1 <<< 1)
    __BIGINT_ADD_WC__(&q_neg2, &q_neg2, &r0); // final_res += r0
    __BIGINT_INTERNAL_MOVE__(res, &q_neg2); _free_alloc_list(alloc_arr, alloc_cnt); *err = BIGINT_SUCCESS;
}


/* BigInt Toom 4-way Multiplication - Heap-allocating */
void __BIHEAP_TOOM_4__(PCONST_BIGINT m, PCONST_BIGINT n, P_BIGINT res, dnml_status *err) {}


/* BigInt Heap-allocating Multiplication Algorithm Dispatching */
void __BIHEAP_MUL_DISP__(PCONST_BIGINT a, PCONST_BIGINT b, P_BIGINT res, dnml_status *err) {
    if (a->n <= BIGINT_SCHOOLBOOK || b->n <= BIGINT_SCHOOLBOOK) { __BIHEAP_SCHOOLBOOK__(a, b, res); *err = BIGINT_SUCCESS; }
    else if (min(a->n, b->n) * 2 <= max(a->n, b->n)) { __BIHEAP_SCHOOLBOOK__(a, b, res); *err = BIGINT_SUCCESS; }
    else if (a->n <= BIGINT_KARATSUBA && b->n <= BIGINT_KARATSUBA) __BIHEAP_KARATSUBA__(a, b, res, err);
    else if (a->n <= BIGINT_TOOM_3 && b->n <= BIGINT_TOOM_3) __BIHEAP_TOOM_3__(a, b, res, err);
    // else if (a->n <= BIGINT_TOOM_4 && b->n <= BIGINT_TOOM_4) __BIHEAP_TOOM_4__(a, b, res);
    // else if (a->n <= BIGINT_TOOM_5 && b->n <= BIGINT_TOOM_5) __BIHEAP_TOOM_5__(a, b, res);
    // else if (a->n <= BIGINT_TOOM_6p5 && b->n <= BIGINT_TOOM_6p5) __BIHEAP_TOOM_6p5__(a, b, res);
    // else if (a->n <= BIGINT_TOOM_7p5 && b->n <= BIGINT_TOOM_7p5) __BIHEAP_TOOM_7p5__(a, b, res);
    // else if (a->n <= BIGINT_TOOM_8p5 && b->n <= BIGINT_TOOM_8p5) __BIHEAP_TOOM_8p5__(a, b, res);
    else __BIHEAP_FFT__(a, b, res, err);
}
