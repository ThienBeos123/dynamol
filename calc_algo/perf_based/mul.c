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



#include "mul.h"
#include <debug_util.h>
#include "../../util/aconv_macros.h"
/** ----------- General BigInt Multiplication -----------
 * THIS FILE CONTAINS THE FOLLOWING ALGORITHMS:
 *
 *      - Schoolbook Multiplication (General)
 *      - Karatsuba Multiplication (General)
 *      - Toom-cook 3-way (General)
 *
 * This file is generally the main algorithm file for bigInt multiplication, containing
 * the multiplication algorithm dispatcher, as well as the workspace sizing function dispatcher.
 * It only contains the 3-simplest multiplication algorithms to keep its focus of being the central,
 * simple point of authority, and delegation of complexity is in other files, including:
 *
 *      - mul_fft.c (Implementation of Schonhage-Strassen Algorithm)
 *      - mul_toom_45.c (Implementation of Toom-cook 4 and 5-way)
 *      - mul_toom_p5.c (implementation of Toom-cook 6.5, 7.5, and 8.5-way)
 */



/* BIGINT WORKSPACE SIZE */
size_t __BIGINT_KARATSUBA_WS__(size_t x_size, size_t y_size) {
    size_t m = (size_t)(max(x_size, y_size) >> 1);
    size_t  x0_range = m,  x1_range = x_size - m;
    size_t  y0_range = m,  y1_range = y_size - m;

    size_t tmp1_size = max(x0_range, x1_range) + 1;
    size_t tmp2_size = max(y0_range, y1_range) + 1;
    size_t z0_size = x0_range + y0_range;
    size_t z1_size = max(x1_range + y0_range, x0_range + y1_range) + m + 1;
    size_t z2_size = max(max(z1_size, x1_range + y1_range + (m << 1)), x0_range + y0_range) + 1;
    return 3*(tmp1_size + tmp2_size + z0_size + z1_size + z2_size);
}
size_t __BIGINT_TOOM_3_WS__(size_t m_size, size_t n_size) {
    size_t k = (size_t)(max(m_size, n_size) / 3) + 1;
    size_t m2size = (m_size > (k << 1)) ? (m_size - (k << 1)) : 0;
    size_t n2size = (n_size > (k << 1)) ? (n_size - (k << 1)) : 0;
    size_t total_points_p = ((k << 2) + 6);
    size_t total_points_q = ((k << 2) + 6);
    size_t total_points_r = ((k << 3) + (m2size + n2size) + 32);
    size_t res_alias = (k << 1) + 14;
    return 3*(total_points_p + total_points_p + total_points_r + res_alias) >> 1;
}
size_t __BIGINT_ASYM_KARAT_WS__(size_t x_size, size_t y_size) {
    /* Block splitting Pre-calculations */
    size_t Bsize = min(x_size, y_size); // Beta size lol
    size_t Asize = max(x_size, y_size); // Alpha chad size lol
    size_t splits = ((size_t)(Asize / Bsize) + 1);
    size_t slice = (Asize / splits);
    /* Metadata precalculations */
    size_t m = (size_t)(max(Bsize, slice) >> 6);
    size_t x0_range = m, x1_range = x_size - m;
    size_t y0_range = m, y1_range = slice - m;
    /* Raw buffers */
    size_t tmp1_size = max(x0_range, x1_range) + 1;
    size_t tmp2_size = max(y0_range, y1_range) + 1;
    size_t z0_size = x0_range + y0_range;
    size_t z1_size = max(x1_range + y0_range, x0_range + y1_range) + m + 1;
    size_t z2_size = max(max(z1_size, x1_range + y1_range + (m << 1)), x0_range + y0_range) + 1;
    return 3*(tmp1_size + tmp2_size + z0_size + z1_size + z2_size) + (x_size + y_size); // raw_bufs + tmp_res
}
size_t __BIGINT_ASYM_TOOM3_WS__(size_t m_size, size_t n_size) {
    /* Block splitting Pre-calculations */
    size_t Bsize = min(m_size, n_size); // Beta size lol
    size_t Asize = max(m_size, n_size); // Alpha chad size lol
    size_t splits = ((size_t)(Asize / Bsize) + 1);
    size_t slice = (Asize / splits);
    /* Metadata precalculations */
    size_t k = (size_t)(max(Bsize, slice) / 3) + 1;
    size_t m2size = (Bsize > (k << 1)) ? (Bsize - (k << 1)) : 0;
    size_t n2size = (slice > (k << 1)) ? (slice - (k << 1)) : 0;
    size_t eval_buf = ((k << 3) + 12); /**/ size_t res_alias = (k << 1) + 14;
    size_t pwmul_buf = ((k << 3) + (m2size + n2size) + 32);
    return (3*(eval_buf + pwmul_buf + res_alias) >> 1) + (m_size + n_size); // raw_bufs + tmp_Res
}
size_t __BIGINT_ASYM_MUL_WS__(size_t a_size, size_t b_size) { 
    // Metadata pre-calculations - slices
    size_t Bsize = min(a_size, b_size); // Beta size lol
    size_t Asize = max(a_size, b_size); // Alpha chad size lol
    size_t slice = (Asize / ((size_t)(Asize / Bsize) + 1));
    if (Bsize <= BIGINT_KARATSUBA && slice <= BIGINT_KARATSUBA) return __BIGINT_ASYM_KARAT_WS__(a_size, b_size);
    else if (Bsize <= BIGINT_TOOM_3 && slice <= BIGINT_TOOM_3) return __BIGINT_ASYM_TOOM3_WS__(a_size, b_size);
    // else if (Bsize <= BIGINT_TOOM_4 && slice <= BIGINT_TOOM_4) return __BIGINT_ASYM_TOOM4_WS__(a_size, b_size);
    // else if (Bsize <= BIGINT_TOOM_5 && slice <= BIGINT_TOOM_5) return __BIGINT_ASYM_TOOM5_WS__(a_size, b_size);
    // else if (Bsize <= BIGINT_TOOM_6p5 && slice <= BIGINT_TOOM_6p5) return __BIGINT_ASYM_TOOM6p5_WS__(a_size, b_size);
    // else if (Bsize <= BIGINT_TOOM_7p5 && slice <= BIGINT_TOOM_7p5) return __BIGINT_ASYM_TOOM7p5_WS__(a_size, b_size);
    // else if (Bsize <= BIGINT_TOOM_8p5 && slice <= BIGINT_TOOM_8p5) return __BIGINT_ASYM_TOOM8p5_WS__(a_size, b_size);
    else return __BIGINT_ASYM_FFT_WS__(a_size, b_size);
}
size_t __BIGINT_MUL_WS__(size_t a_size, size_t b_size) {
    if (a_size <= BIGINT_SCHOOLBOOK || b_size <= BIGINT_SCHOOLBOOK) return 0; // Doesn't need any
    else if (min(a_size, b_size) * 2 <= max(a_size, b_size)) return __BIGINT_ASYM_MUL_WS__(a_size, b_size);
    else if (a_size < BIGINT_KARATSUBA && b_size < BIGINT_KARATSUBA) return __BIGINT_KARATSUBA_WS__(a_size, b_size);             
    else if (a_size < BIGINT_TOOM_3 && b_size < BIGINT_TOOM_3) return __BIGINT_TOOM_3_WS__(a_size, b_size);
    // else if (a_size <= BIGINT_TOOM_4 && b_size <= BIGINT_TOOM_4) return __BIGINT_TOOM_4_WS__(a_size, b_size);
    // else if (a_size <= BIGINT_TOOM_5 && b_size <= BIGINT_TOOM_5) return __BIGINT_TOOM_5_WS__(a_size, b_size);
    // else if (a_size <= BIGINT_TOOM_6p5 && b_size <= BIGINT_TOOM_6p5) return __BIGINT_TOOM_6p5_WS__(a_size, b_size);           
    // else if (a_size <= BIGINT_TOOM_7p5 && b_size <= BIGINT_TOOM_7p5) return __BIGINT_TOOM_7p5_WS__(a_size, b_size);
    // else if (a_size <= BIGINT_TOOM_8p5 && b_size <= BIGINT_TOOM_8p5) return __BIGINT_TOOM_8p5_WS__(a_size, b_size);
    else return __BIGINT_FFT_WS__(a_size, b_size);
}




/* BIGINT ALGORITHMS - BALANCED */
void __BIGINT_SCHOOLBOOK__(PCONST_BIGINT a, PCONST_BIGINT b, P_BIGINT res) {
    memset(res->limbs, 0, a->n + b->n);
    for (size_t i = 0; i < a->n; ++i) {
        uint64_t carry = 0;
        for (size_t j = 0; j < b->n; ++j) {
            uint64_t low, high;
            low = __MUL_UI64__(a->limbs[i], b->limbs[j], &high);
            uint64_t sum = res->limbs[i + j] + low; uint8_t c1 = (sum < low);
            uint64_t sum2 = sum + carry; uint8_t c2 = (sum2 < carry);
            carry = high + (c1 | c2); res->limbs[i + j] = sum2;
        } res->limbs[i + b->n] += carry; // Add the remaining carry to the MSL
    } res->n = a->n + b->n; __BIGINT_INTERNAL_TRIM_LZ__(res);
}
void __BIGINT_KARATSUBA__(PCONST_BIGINT x, PCONST_BIGINT y, P_BIGINT res, calc_ctx karat_ctx, dnml_status *err) {
    if (x->n <= BIGINT_SCHOOLBOOK || y->n <= BIGINT_SCHOOLBOOK) { 
        __BIGINT_SCHOOLBOOK__(x, y, res); /**/ *err = BIGINT_SUCCESS; return; 
    } //* ---- 1. SETUP ---- *?/
    size_t m = (size_t)(max(x->n, y->n) >> 1);
    size_t  x0_range = m,  x1_range = x->n - m;
    size_t  y0_range = m,  y1_range = y->n - m;
    bigInt x0 = {.limbs = x->limbs,             .n = x0_range, .cap = x0_range};
    bigInt x1 = {.limbs = x->limbs + x0_range,  .n = x1_range, .cap = x1_range};
    bigInt y0 = {.limbs = y->limbs,             .n = y0_range, .cap = y0_range};
    bigInt y1 = {.limbs = y->limbs + y0_range,  .n = y1_range, .cap = y1_range};

    dnml_status echeck;
    size_t karat_mark = scratch_mark(&karat_ctx);
    size_t z1_size = max(x1_range + y0_range, x0_range + y1_range) + m + 1;
    size_t z2_size = max(max(z1_size, x1_range + y1_range + (m << 1)), x0_range + y0_range) + 1;
    BIGINT_TEMP(tmp1, max(x0_range, x1_range) + 1, karat_ctx, karat_mark, echeck, err,);
    BIGINT_TEMP(tmp2, max(y0_range, y1_range) + 1, karat_ctx, karat_mark, echeck, err,);
    BIGINT_TEMP(z0, x0_range + y0_range, karat_ctx, karat_mark, echeck, err,);
    BIGINT_TEMP(z1, z1_size, karat_ctx, karat_mark, echeck, err,);
    BIGINT_TEMP(z2, z2_size, karat_ctx, karat_mark, echeck, err,);


    //* ------- 2. QUADRATIC COMPONENTS CALCULATION -------- *//
    // The procedure goes:
    //  z3 = (x1 + x0)(y1 + y0)
    //  z2 = x1 * y1
    //  z0 = x0 * y0
    //  z1 = z3 - z2 - z1
    __BIGINT_KARATSUBA__(&x0, &y0, &z0, karat_ctx, &echeck); SCRATCH_OVF(echeck, karat_ctx, karat_mark, err,);
    __BIGINT_KARATSUBA__(&x1, &y1, &z2, karat_ctx, &echeck); SCRATCH_OVF(echeck, karat_ctx, karat_mark, err,);
    __BIGINT_ADD_WC__(&tmp1, &x1, &x0); __BIGINT_ADD_WC__(&tmp2, &y1, &y0);
    __BIGINT_KARATSUBA__(&tmp1, &tmp2, &z1, karat_ctx, &echeck); SCRATCH_OVF(echeck, karat_ctx, karat_mark, err,);
    __BIGINT_SUB_WB__(&z1, &z1, &z2); __BIGINT_SUB_WB__(&z1, &z1, &z0);

    //* ------------ 3. FINAL CALCULATION -------------- *//
    __BIGINT_INTERNAL_LLSHIFT__(&z2, m << 1); __BIGINT_INTERNAL_LLSHIFT__(&z1, m);
    __BIGINT_ADD_WC__(&z2, &z2, &z1); __BIGINT_ADD_WC__(&z2, &z2, &z0);
    __BIGINT_INTERNAL_COPY__(res, &z2); scratch_rewind(&karat_ctx, karat_mark); *err = BIGINT_SUCCESS;
}
void __BIGINT_TOOM_3__(PCONST_BIGINT m, PCONST_BIGINT n, P_BIGINT res, calc_ctx toom_ctx, dnml_status *err) {
    if (m->n <= BIGINT_SCHOOLBOOK || n->n <= BIGINT_SCHOOLBOOK) { 
        __BIGINT_SCHOOLBOOK__(m, n, res); /**/ *err = BIGINT_SUCCESS; return; 
    } //* -------- 1. SETUP & SPLITTING -------- *//
    size_t k = (size_t)(max(m->n, n->n) / 3) + 1;
    size_t m2size = (m->n > (k << 1)) ? (m->n - (k << 1)) : 0;
    size_t n2size = (n->n > (k << 1)) ? (n->n - (k << 1)) : 0;
    bigInt m0 = {.limbs = m->limbs,             .n = k,         .cap = k};
    bigInt m1 = {.limbs = m->limbs + k,         .n = k,         .cap = k};
    bigInt m2 = {.limbs = m->limbs + (k << 1),  .n = m2size,  .cap = m2size};
    bigInt n0 = {.limbs = n->limbs,             .n = k,         .cap = k};
    bigInt n1 = {.limbs = n->limbs + k,         .n = k,         .cap = k};
    bigInt n2 = {.limbs = n->limbs + (k << 1),  .n = n2size,  .cap = n2size};


    //* -------- 2. EVALUATION & POINT-WISE MULTIPLICATION -------- *//
    dnml_status echeck; size_t toom_mark = scratch_mark(&toom_ctx);
    /*  ---------------------------------- EVALUATION ------------------------------
    *   +) pOuter = m0 + m2                                         | +) qOuter = n0 + n2
    *   +) p(0)   = m0          (NO FULL TEMPORARY)                 | +) q(0)   = n0          (NO FULL TEMPORARY)
    *   +) p(1)   = pOuter + m1                                     | +) q(1)   = qOuter + n1
    *   +) p(-1)  = pOuter - m1                                     | +) q(-1)  = qOuter - n1
    *   +) p(-2)  = 2*(p(-1) + m2) - m0                             | +) q(-2)  = 2*(q(-1) + n2) - n0
    *   +) p(inf) = m2          (NO FULL TEMPORARY)                 | +) q(inf) = n2          (NO FULL TEMPORARY) */
    // p(x) TEMPORARIES                                             // q(x) TEMPORARIES
    BIGINT_TEMP(p_outer, k + 1, toom_ctx, toom_mark, echeck, err,); BIGINT_TEMP(q_outer, k + 1, toom_ctx, toom_mark, echeck, err,);
    BIGINT_TEMP(p1,      k + 2, toom_ctx, toom_mark, echeck, err,); BIGINT_TEMP(q1,      k + 2, toom_ctx, toom_mark, echeck, err,);
    BIGINT_TEMP(p_neg1,  k + 1, toom_ctx, toom_mark, echeck, err,); BIGINT_TEMP(q_neg1,  k + 1, toom_ctx, toom_mark, echeck, err,);
    BIGINT_TEMP(p_neg2,  k + 2, toom_ctx, toom_mark, echeck, err,); BIGINT_TEMP(q_neg2,  k + 2, toom_ctx, toom_mark, echeck, err,);
    // p(x) CALCULATIONS                                            // q(x) CALCULATIONS
    __BIGINT_ADD_WC__(&p_outer, &m0, &m2);                          __BIGINT_ADD_WC__(&q_outer, &n0, &n2);
    __BIGINT_ADD_WC__(&p1, &p_outer, &m1);                          __BIGINT_ADD_WC__(&q1, &q_outer, &n1);
    __BIGINT_SUB_SAW__(&p_neg1, &p_outer, &m1);                     __BIGINT_SUB_SAW__(&q_neg1, &q_outer, &n1);
    __BIGINT_ADD_SAW__(&p_neg2, &p_neg1, &m2);                      __BIGINT_ADD_SAW__(&q_neg2, &q_neg1, &n2);
    __BIGINT_INTERNAL_LSHIFT__(&p_neg2, 1);                         __BIGINT_INTERNAL_LSHIFT__(&q_neg2, 1);
    __BIGINT_SUB_SAW__(&p_neg2, &p_neg2, &m0);                      __BIGINT_SUB_SAW__(&q_neg2, &q_neg2, &n0);
    /* ------------ POINT-WISE MULTIPLICATION ------------
    *   +) r(0)   = p(0)   * q(0)       ---> Cap: 2k
    *   +) r(1)   = p(1)   * q(1)       ---> Cap: 2k + 4 (original) --> 2k + 9 (interpolation - r1 + llshift)
    *   +) r(-1)  = p(-1)  * q(-1)      ---> Cap: 2k + 2 (originxal) --> 2k + 9 (interpolation - r2 + llshift)
    *   +) r(-2)  = p(-2)  * q(-2)      ---> Cap: 2k + 4 (original) --> 2k + 10 (interpolation - r3 + llshift)
    *   +) r(inf) = p(inf) * q(inf)     ---> Cap: 2k (original) ---> 2k + 4 (bit-shifts accounted)
    */
    BIGINT_TEMP(r0,     (k << 1),       toom_ctx, toom_mark, echeck, err,);
    BIGINT_TEMP(r1,     (k << 1) + 9,   toom_ctx, toom_mark, echeck, err,);
    BIGINT_TEMP(r_neg1, (k << 1) + 9,   toom_ctx, toom_mark, echeck, err,);
    BIGINT_TEMP(r_neg2, (k << 1) + 10,  toom_ctx, toom_mark, echeck, err,);
    BIGINT_TEMP(rinf,    m2size + n2size + 4, toom_ctx, toom_mark, echeck, err,);
    __BIGINT_TOOM_3__(&m0, &n0, &r0, toom_ctx, &echeck); SCRATCH_OVF(echeck, toom_ctx, toom_mark, err,);
    __BIGINT_TOOM_3__(&p1, &q1, &r1, toom_ctx, &echeck); SCRATCH_OVF(echeck, toom_ctx, toom_mark, err,);
    __BIGINT_TOOM_3__(&p_neg1, &q_neg1, &r_neg1, toom_ctx, &echeck); SCRATCH_OVF(echeck, toom_ctx, toom_mark, err,);
    __BIGINT_TOOM_3__(&p_neg2, &q_neg2, &r_neg2, toom_ctx, &echeck); SCRATCH_OVF(echeck, toom_ctx, toom_mark, err,);
    __BIGINT_TOOM_3__(&m2, &n2, &rinf, toom_ctx, &echeck); SCRATCH_OVF(echeck, toom_ctx, toom_mark, err,);


    /* ------------- 3. INTERPOLATION & RECOMPOSITION ---------------- */
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
    BIGINT_TEMP(final_res, (k << 1) + 14, toom_ctx, toom_mark, echeck, err,);
    __BIGINT_INTERNAL_LLSHIFT__(&rinf, 4);   __BIGINT_INTERNAL_LLSHIFT__(&r_neg2, 3);
    __BIGINT_INTERNAL_LLSHIFT__(&r_neg1, 2); __BIGINT_INTERNAL_LLSHIFT__(&r1, 1);
    __BIGINT_ADD_WC__(&final_res, &rinf, &r_neg2); __BIGINT_ADD_WC__(&final_res, &final_res, &r_neg1);
    __BIGINT_ADD_WC__(&final_res, &final_res, &r1); __BIGINT_ADD_WC__(&final_res, &final_res, &r0);
    __BIGINT_INTERNAL_COPY__(res, &final_res); scratch_rewind(&toom_ctx, toom_mark); *err = BIGINT_SUCCESS;
}




/* BIGINT ALGORITHMS - UNBALANCED */
void __BIGINT_ASYM_KARAT__(PCONST_BIGINT x, PCONST_BIGINT y, P_BIGINT res, calc_ctx karat_ctx, dnml_status *err) {
    size_t Bsize = min(x->n, y->n); // Beta size lol
    size_t Asize = max(x->n, y->n); // Alpha chad size lol
    size_t splits = ((size_t)(Asize / Bsize) + 1);
    size_t slice = (Asize / splits), last_slice = Asize - slice;
    const bigInt *const alpha = (Asize == x->n) ? x : y; 
    const bigInt *const beta = (Bsize == y->n) ? y : x;
    //* ============= Pre-operation Calculations & Allocations ============= *//
    dnml_status echeck = BIGINT_SUCCESS; size_t karat_mark = scratch_mark(&karat_ctx);
    /* --------- 1. Setup --------- */ size_t m = (size_t)(max(Bsize, slice) >> 1);
    bigInt x0 = {0}, x1 = {0}, y0 = {0}, y1 = {0};
    size_t x0_range = m, x1_range = Bsize - m;
    size_t y0_range = m, y1_range = slice - m;
    size_t z1_size = max(x1_range + y0_range, x0_range + y1_range) + m + 1;
    size_t z2_size = max(max(z1_size, x1_range + y1_range + (m << 1)), x0_range + y0_range) + 1;
    BIGINT_TEMP(tmp1, max(x0_range, x1_range) + 1, karat_ctx, karat_mark, echeck, err,);
    BIGINT_TEMP(tmp2, max(y0_range, y1_range) + 1, karat_ctx, karat_mark, echeck, err,);
    BIGINT_TEMP(z0, x0_range + y0_range, karat_ctx, karat_mark, echeck, err,);
    BIGINT_TEMP(z1, z1_size, karat_ctx, karat_mark, echeck, err,);
    BIGINT_TEMP(z2, z2_size, karat_ctx, karat_mark, echeck, err,);
    BIGINT_TEMP(tmp_res, x->n + y->n, karat_ctx, karat_mark, echeck, err,);
    

    //* ================ Main sliced window Multiplication loop  ================ *//
    bigInt window = {0}; size_t offset = 0;
    for (size_t i = 0; i < splits; ++i) {
        size_t curr_slice = (i == splits - 1) ? last_slice : slice; /**/ offset = i * curr_slice;
        window = (bigInt){.limbs = alpha->limbs + offset, .n = curr_slice, .cap = curr_slice, .sign = 1};
        if (
            min(Bsize, curr_slice) * 2 <= max(Bsize, curr_slice) || 
            (Bsize <= BIGINT_SCHOOLBOOK || curr_slice <= BIGINT_SCHOOLBOOK)
        ) { 
            __BIGINT_SCHOOLBOOK__(beta, &window, &z2);
            __BIGINT_ADD_SHIFT__(&tmp_res, &z2, offset); continue; // (tmp_res <<<= slice) + tmp
        }
        if (i == splits - 1) {
            // Recalculation of size metadatas for last_slice's case
            /* Normal Setups */ m = (size_t)(max(Bsize, last_slice) >> 1);
            x0_range = m; x1_range = Bsize - m;
            y0_range = m; y1_range = slice - m;
            /* Qudratic Components Setup */
            z1_size = max(x1_range + y0_range, x0_range + y1_range) + m + 1;
            z2_size = max(max(z1_size, x1_range + y1_range + (m << 1)), x0_range + y0_range) + 1;
            tmp1.cap = max(x0_range, x1_range) + 1; /**/ tmp2.cap = max(y0_range, y1_range) + 1;
            z0.cap = x0_range + y0_range; /**/ z1.cap = z1_size; /**/ z2.cap = z2_size;
        }
        //* -------------- 1. SETUP -------------- *//
        x0 = (bigInt){.limbs = window.limbs,            .n = x0_range, .cap = x0_range};
        x1 = (bigInt){.limbs = window.limbs + x0_range, .n = x1_range, .cap = x1_range};
        y0 = (bigInt){.limbs = beta->limbs,             .n = y0_range, .cap = y0_range};
        y1 = (bigInt){.limbs = beta->limbs + y0_range,  .n = y1_range, .cap = y1_range};


        //* ------- 2. QUADRATIC COMPONENTS CALCULATION -------- *//
        // The procedure goes:
        //  z3 = (x1 + x0)(y1 + y0)
        //  z2 = x1 * y1
        //  z0 = x0 * y0
        //  z1 = z3 - z2 - z1
        __BIGINT_KARATSUBA__(&x0, &y0, &z0, karat_ctx, &echeck); SCRATCH_OVF(echeck, karat_ctx, karat_mark, err,);
        __BIGINT_KARATSUBA__(&x1, &y1, &z2, karat_ctx, &echeck); SCRATCH_OVF(echeck, karat_ctx, karat_mark, err,);
        __BIGINT_ADD_WC__(&tmp1, &x1, &x0); __BIGINT_ADD_WC__(&tmp2, &y1, &y0);
        __BIGINT_KARATSUBA__(&tmp1, &tmp2, &z1, karat_ctx, &echeck); SCRATCH_OVF(echeck, karat_ctx, karat_mark, err,);
        __BIGINT_SUB_WB__(&z1, &z1, &z2); __BIGINT_SUB_WB__(&z1, &z1, &z0);

        //* ------------ 3. FINAL CALCULATION -------------- *//
        __BIGINT_INTERNAL_LLSHIFT__(&z2, m << 1); __BIGINT_INTERNAL_LLSHIFT__(&z1, m);
        __BIGINT_ADD_WC__(&z2, &z2, &z1); __BIGINT_ADD_WC__(&z2, &z2, &z0);
        __BIGINT_ADD_SHIFT__(&tmp_res, &z2, offset); // Accumulate into tmp_res
    } __BIGINT_INTERNAL_COPY__(res, &tmp_res); scratch_rewind(&karat_ctx, karat_mark); *err = BIGINT_SUCCESS;
}
void __BIGINT_ASYM_TOOM3__(PCONST_BIGINT m, PCONST_BIGINT n, P_BIGINT res, calc_ctx toom_ctx, dnml_status *err) {
    size_t Bsize = min(m->n, n->n); // Beta size lol
    size_t Asize = max(m->n, n->n); // Alpha chad size lol
    size_t splits = ((size_t)(Asize / Bsize) + 1);
    size_t slice = (Asize / splits), last_slice = Asize - slice;
    const bigInt *const alpha = (Asize == m->n) ? m : n; 
    const bigInt *const beta = (Bsize == n->n) ? n : m;
    //* ============= Pre-operation Calculations & Allocations ============= *//
    dnml_status echeck = BIGINT_SUCCESS; size_t toom_mark = scratch_mark(&toom_ctx);
    /* -------- 1. Setup & Splittings -------- */
    size_t k = (size_t)(max(Bsize, slice) / 3) + 1;
    size_t m2size = (Bsize > (k << 1)) ? (Bsize - (k << 1)) : 0;
    size_t n2size = (slice > (k << 1)) ? (slice - (k << 1)) : 0;
    bigInt m0 = {0}, m1 = {0}, m2 = {0}; /**/ bigInt n0 = {0}, n1 = {0}, n2 = {0};

    /* ----------- 2. Evaluation & Pointwise Multiplication -----------
    *   +) pOuter = m0 + m2                                         | +) qOuter = n0 + n2
    *   +) p(0)   = m0          (NO FULL TEMPORARY)                 | +) q(0)   = n0          (NO FULL TEMPORARY)
    *   +) p(1)   = pOuter + m1                                     | +) q(1)   = qOuter + n1
    *   +) p(-1)  = pOuter - m1                                     | +) q(-1)  = qOuter - n1
    *   +) p(-2)  = 2*(p(-1) + m2) - m0                             | +) q(-2)  = 2*(q(-1) + n2) - n0
    *   +) p(inf) = m2          (NO FULL TEMPORARY)                 | +) q(inf) = n2          (NO FULL TEMPORARY) */
    BIGINT_TEMP(p_outer, k + 1, toom_ctx, toom_mark, echeck, err,); BIGINT_TEMP(q_outer, k + 1, toom_ctx, toom_mark, echeck, err,);
    BIGINT_TEMP(p1,      k + 2, toom_ctx, toom_mark, echeck, err,); BIGINT_TEMP(q1,      k + 2, toom_ctx, toom_mark, echeck, err,);
    BIGINT_TEMP(p_neg1,  k + 1, toom_ctx, toom_mark, echeck, err,); BIGINT_TEMP(q_neg1,  k + 1, toom_ctx, toom_mark, echeck, err,);
    BIGINT_TEMP(p_neg2,  k + 2, toom_ctx, toom_mark, echeck, err,); BIGINT_TEMP(q_neg2,  k + 2, toom_ctx, toom_mark, echeck, err,);
    /* ------------ POINT-WISE MULTIPLICATION ------------
    *   +) r(0)   = p(0)   * q(0)       ---> Cap: 2k
    *   +) r(1)   = p(1)   * q(1)       ---> Cap: 2k + 4 (original) --> 2k + 9 (interpolation - r1 + llshift)
    *   +) r(-1)  = p(-1)  * q(-1)      ---> Cap: 2k + 2 (originxal) --> 2k + 9 (interpolation - r2 + llshift)
    *   +) r(-2)  = p(-2)  * q(-2)      ---> Cap: 2k + 4 (original) --> 2k + 10 (interpolation - r3 + llshift)
    *   +) r(inf) = p(inf) * q(inf)     ---> Cap: 2k (original) ---> 2k + 4 (bit-shifts accounted)
    */
    BIGINT_TEMP(r0,     (k << 1),       toom_ctx, toom_mark, echeck, err,);
    BIGINT_TEMP(r1,     (k << 1) + 9,   toom_ctx, toom_mark, echeck, err,);
    BIGINT_TEMP(r_neg1, (k << 1) + 9,   toom_ctx, toom_mark, echeck, err,);
    BIGINT_TEMP(r_neg2, (k << 1) + 10,  toom_ctx, toom_mark, echeck, err,);
    BIGINT_TEMP(rinf,    m2size + n2size + 4, toom_ctx, toom_mark, echeck, err,);
    BIGINT_TEMP(final_res, (k << 1) + 14, toom_ctx, toom_mark, echeck, err,);
    BIGINT_TEMP(tmp_res, m->n + n->n, toom_ctx, toom_mark, echeck, err,);


    //* ================ Main sliced window Multiplication loop  ================ *//
    bigInt window = {0}; size_t offset = 0;
    for (size_t i = 0;i < splits; ++i) {
        size_t curr_slice = (i == splits - 1) ? last_slice : slice; /**/ offset = i * curr_slice;
        window = (bigInt){.limbs = alpha->limbs + offset, .n = curr_slice, .cap = curr_slice, .sign = 1};
        if (
            min(Bsize, curr_slice) * 2 <= max(Bsize, curr_slice) || 
            (Bsize <= BIGINT_SCHOOLBOOK || curr_slice <= BIGINT_SCHOOLBOOK)
        ) { 
            __BIGINT_SCHOOLBOOK__(beta, &window, &final_res);
            __BIGINT_ADD_SHIFT__(&tmp_res, &final_res, offset); continue; // (tmp_res <<<= slice) + tmp
        }
        if (i == splits - 1) {
            // Recalculation of size metadatas for last_slice's case
            k = (size_t)(max(Bsize, curr_slice) / 3) + 1;
            m2size = (Bsize      > (k << 1)) ? (Bsize      - (k << 1)) : 0;
            n2size = (curr_slice > (k << 1)) ? (curr_slice - (k << 1)) : 0;
            // Modifying the capacity metadata of buffers - Evaluation buffers
            p_outer.cap = k + 1; p1.cap = k + 2;        q_outer.cap = k + 1; q1.cap = k + 2;
            p_neg1.cap = k + 1; p_neg2.cap = k + 2;     q_neg1.cap = k + 2; q_neg2.cap = k + 2;
            // Modifying the capacity metadata of buffers - Point-wise Mul
            r0.cap = (k << 1); /**/ r1.cap = (k << 1) + 9; /**/ r_neg1.cap = (k << 1) + 9;
            r_neg2.cap = (k << 1) + 10; /**/ rinf.cap = m2size + n2size + 4; /**/ final_res.cap = (k << 1) + 14;
        }
        /* ---------------- 1. SETUP & SPLITTING ---------------- */
        bigInt m0 = {.limbs = window.limbs,            .n = k,      .cap = k};
        bigInt m1 = {.limbs = window.limbs + k,        .n = k,      .cap = k};
        bigInt m2 = {.limbs = window.limbs + (k << 1), .n = m2size, .cap = m2size};
        bigInt n1 = {.limbs = beta->limbs + k,         .n = k,      .cap = k};
        bigInt n0 = {.limbs = beta->limbs,             .n = k,      .cap = k};
        bigInt n2 = {.limbs = beta->limbs + (k << 1),  .n = n2size, .cap = n2size};


        /* -------- 2. EVALUATION & POINT-WISE MULTIPLICATION --------
        *   +) pOuter = m0 + m2                             | +) qOuter = n0 + n2
        *   +) p(0)   = m0          (NO FULL TEMPORARY)     | +) q(0)   = n0          (NO FULL TEMPORARY)
        *   +) p(1)   = pOuter + m1                         | +) q(1)   = qOuter + n1
        *   +) p(-1)  = pOuter - m1                         | +) q(-1)  = qOuter - n1
        *   +) p(-2)  = 2*(p(-1) + m2) - m0                 | +) q(-2)  = 2*(q(-1) + n2) - n0
        *   +) p(inf) = m2          (NO FULL TEMPORARY      | +) q(inf) = n2          (NO FULL TEMPORARY) */
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
        *   +) r(-1)  = p(-1)  * q(-1)      ---> Cap: 2k + 2 (originxal) --> 2k + 9 (interpolation - r2 + llshift)
        *   +) r(-2)  = p(-2)  * q(-2)      ---> Cap: 2k + 4 (original) --> 2k + 10 (interpolation - r3 + llshift)
        *   +) r(inf) = p(inf) * q(inf)     ---> Cap: 2k (original) ---> 2k + 4 (bit-shifts accounted)
        */
        __BIGINT_TOOM_3__(&m0, &n0, &r0, toom_ctx, &echeck); SCRATCH_OVF(echeck, toom_ctx, toom_mark, err,);
        __BIGINT_TOOM_3__(&p1, &q1, &r1, toom_ctx, &echeck); SCRATCH_OVF(echeck, toom_ctx, toom_mark, err,);
        __BIGINT_TOOM_3__(&p_neg1, &q_neg1, &r_neg1, toom_ctx, &echeck); SCRATCH_OVF(echeck, toom_ctx, toom_mark, err,);
        __BIGINT_TOOM_3__(&p_neg2, &q_neg2, &r_neg2, toom_ctx, &echeck); SCRATCH_OVF(echeck, toom_ctx, toom_mark, err,);
        __BIGINT_TOOM_3__(&m2, &n2, &rinf, toom_ctx, &echeck); SCRATCH_OVF(echeck, toom_ctx, toom_mark, err,);


        /* ------------- 3. INTERPOLATION & RECOMPOSITION ---------------- */
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
        __BIGINT_INTERNAL_LLSHIFT__(&rinf, 4);   __BIGINT_INTERNAL_LLSHIFT__(&r_neg2, 3);
        __BIGINT_INTERNAL_LLSHIFT__(&r_neg1, 2); __BIGINT_INTERNAL_LLSHIFT__(&r1, 1);
        __BIGINT_ADD_WC__(&final_res, &rinf, &r_neg2); __BIGINT_ADD_WC__(&final_res, &final_res, &r_neg1);
        __BIGINT_ADD_WC__(&final_res, &final_res, &r1); __BIGINT_ADD_WC__(&final_res, &final_res, &r0);
        __BIGINT_ADD_SHIFT__(&tmp_res, &final_res, offset); // Accumulate into the final result (like truly final)
    } __BIGINT_INTERNAL_COPY__(res, &tmp_res); scratch_rewind(&toom_ctx, toom_mark); *err = BIGINT_SUCCESS;
}




/* BIGINT MULTIPLICATION ALGORITHM DISPATCHER */
void __BIGINT_ASYM_MUL_DISP__(PCONST_BIGINT a, PCONST_BIGINT b, P_BIGINT res, calc_ctx mul_ctx, dnml_status *err) {
    // Metadata pre-calculations - slices
    size_t Bsize = min(a->n, b->n); // Beta size lol
    size_t Asize = max(a->n, b->n); // Alpha chad size lol
    size_t slice = (Asize / ((size_t)(Asize / Bsize) + 1));
    if (Bsize <= BIGINT_KARATSUBA && slice <= BIGINT_KARATSUBA) __BIGINT_ASYM_KARAT__(a, b, res, mul_ctx, err);
    else if (Bsize <= BIGINT_TOOM_3 && slice <= BIGINT_TOOM_3) __BIGINT_ASYM_TOOM3__(a, b, res, mul_ctx, err);
    // else if (Bsize <= BIGINT_TOOM_4 && slice <= BIGINT_TOOM_4) __BIGINT_ASYM_TOOM4__(a, b, res, mul_ctx, err);
    // else if (Bsize <= BIGINT_TOOM_5 && slice <= BIGINT_TOOM_5) __BIGINT_ASYM_TOOM5__(a, b, res, mul_ctx, err);
    // else if (Bsize <= BIGINT_TOOM_6p5 && slice <= BIGINT_TOOM_6p5) __BIGINT_ASYM_TOOM6p5__(a, b, res, mul_ctx, err);
    // else if (Bsize <= BIGINT_TOOM_7p5 && slice <= BIGINT_TOOM_7p5) __BIGINT_ASYM_TOOM7p5__(a, b, res, mul_ctx, err);
    // else if (Bsize <= BIGINT_TOOM_8p5 && slice <= BIGINT_TOOM_8p5) __BIGINT_ASYM_TOOM8p5__(a, b, res, mul_ctx, err);
    else __BIGINT_ASYM_FFT__(a, b, res, mul_ctx, err);
}
void __BIGINT_MUL_DISP__(PCONST_BIGINT a, PCONST_BIGINT b, P_BIGINT res, calc_ctx mul_ctx, dnml_status *err) {
    if (a->n <= BIGINT_SCHOOLBOOK || b->n <= BIGINT_SCHOOLBOOK) { __BIGINT_SCHOOLBOOK__(a, b, res); *err = BIGINT_SUCCESS; }
    else if (min(a->n, b->n) * 2 <= max(a->n, b->n)) __BIGINT_ASYM_MUL_DISP__(a, b, res, mul_ctx, err);
    else if (a->n <= BIGINT_KARATSUBA && b->n <= BIGINT_KARATSUBA) __BIGINT_KARATSUBA__(a, b, res, mul_ctx, err);
    else if (a->n <= BIGINT_TOOM_3 && b->n <= BIGINT_TOOM_3) __BIGINT_TOOM_3__(a, b, res, mul_ctx, err);
    // else if (a->n <= BIGINT_TOOM_4 && b->n <= BIGINT_TOOM_4) __BIGINT_TOOM_4__(a, b, res, mul_ctx);
    // else if (a->n <= BIGINT_TOOM_5 && b->n <= BIGINT_TOOM_5) __BIGINT_TOOM_5__(a, b, res, mul_ctx);
    // else if (a->n <= BIGINT_TOOM_6p5 && b->n <= BIGINT_TOOM_6p5) __BIGINT_TOOM_6p5__(a, b, res, mul_ctx);
    // else if (a->n <= BIGINT_TOOM_7p5 && b->n <= BIGINT_TOOM_7p5) __BIGINT_TOOM_7p5__(a, b, res, mul_ctx);
    // else if (a->n <= BIGINT_TOOM_8p5 && b->n <= BIGINT_TOOM_8p5) __BIGINT_TOOM_8p5__(a, b, res, mul_ctx);
    else __BIGINT_FFT__(a, b, res, mul_ctx, err);
}
