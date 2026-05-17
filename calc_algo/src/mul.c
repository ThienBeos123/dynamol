#include "../header/mul.h"


/* BIGINT WORKSPACE SIZE */
static size_t __BIGINT_KARATSUBA_WS__(size_t x_size, size_t y_size) {
    size_t m = (size_t)(max(x_size, y_size) / 2);
    size_t  x0_range = m,  x1_range = x_size - m;
    size_t  y0_range = m,  y1_range = y_size - m;

    size_t tmp1_size = max(x0_range, x1_range) + 1;
    size_t tmp2_size = max(y0_range, y1_range) + 1;
    size_t z0_size = x0_range + y0_range;
    size_t z1_size = max(x1_range + y0_range, x0_range + y1_range) + m + 1;
    size_t z2_size = max(max(z1_size, x1_range + y1_range + 2*m), x0_range + y0_range) + 1;
    return 3*(tmp1_size + tmp2_size + z0_size + z1_size + z2_size);
}
static size_t __BIGINT_TOOM_3_WS__(size_t m_size, size_t n_size) {
    size_t k = (size_t)(max(m_size, n_size) / 3) + 1;
    size_t total_points_p = (k << 2 + 6);
    size_t total_points_q = (k << 2 + 6);
    size_t total_points_r = ((k << 3) + (k << 1) + 32);
    size_t res_alias = (k << 1) + 14;
    return 3*(total_points_p + total_points_p + total_points_r + res_alias) >> 1;
}
static size_t __BIGINT_TOOM_4_WS__(size_t m_size, size_t n_size) {}
static size_t __BIGINT_TOOM_5_WS__(size_t m_size, size_t n_size) {}
static size_t __BIGINT_TOOM_6p5_WS__(size_t m_size, size_t n_size) {}
static size_t __BIGINT_TOOM_7p5_WS__(size_t m_size, size_t n_size) {}
static size_t __BIGINT_TOOM_8p5_WS__(size_t m_size, size_t n_size) {}
static size_t __BIGINT_SSA_WS__(size_t a_size, size_t b_size) {}
size_t __BIGINT_MUL_WS__(size_t a_size, size_t b_size) {
    if (a_size <= BIGINT_SCHOOLBOOK && b_size <= BIGINT_SCHOOLBOOK) return 0; // Doesn't need any
    else if (min(a_size, b_size) * 2 <= max(a_size, b_size)) return 0;
    else if (a_size < BIGINT_KARATSUBA && b_size < BIGINT_KARATSUBA) return __BIGINT_KARATSUBA_WS__(a_size, b_size);
    else if (a_size < BIGINT_TOOM_3 && b_size < BIGINT_TOOM_3) return __BIGINT_TOOM_3_WS__(a_size, b_size);
    else if (a_size <= BIGINT_TOOM_4 && b_size <= BIGINT_TOOM_4) return __BIGINT_TOOM_4_WS__(a_size, b_size);
    else if (a_size <= BIGINT_TOOM_5 && b_size <= BIGINT_TOOM_5) return __BIGINT_TOOM_5_WS__(a_size, b_size);
    else if (a_size <= BIGINT_TOOM_6p5 && b_size <= BIGINT_TOOM_6p5) return __BIGINT_TOOM_6p5_WS__(a_size, b_size);
    else if (a_size <= BIGINT_TOOM_7p5 && b_size <= BIGINT_TOOM_7p5) return __BIGINT_TOOM_7p5_WS__(a_size, b_size);
    else if (a_size <= BIGINT_TOOM_8p5 && b_size <= BIGINT_TOOM_8p5) return __BIGINT_TOOM_8p5_WS__(a_size, b_size);
    else return __BIGINT_SSA_WS__(a_size, b_size);
}

/* BIGINT ALGORITHMS */
static void __BIGINT_SCHOOLBOOK__(const bigInt *a, const bigInt *b, bigInt *res) {
    memset(res->limbs, 0, (a->n + b->n) * sizeof(uint64_t)); // Set every bytes to 0 in res, basically CALLOC() without the MALLOC()
    // Implementing schoolbook multiplication, treating each limb like a digit
    // -----> Inner loop access each limb of b and multiplying by 1 limb of a before going to the next a's limb
    for (size_t i = 0; i < a->n; ++i) {
        uint64_t carry = 0;
        for (size_t j = 0; j < b->n; ++j) {
            uint64_t low, high;
            low = __MUL_UI64__(a->limbs[i], b->limbs[j], &high);
            // Stored and calculated kinda in a staircase pattern seen in the sums of Schoolbook
            uint64_t sum =      res->limbs[i + j] /* Potential data from last iteration */ 
                            +   low /* Lower half (64 bit) */ 
                            +   carry /* Carry from the last iteration */;

            carry =     high /* The remaining half (64 bit) */ 
                    +   (sum < low) /* Overflow from adding the lower half */ 
                    +   (sum < carry) /* Overflow from adding the last iteration's carry */;

            res->limbs[i + j] = sum; // Apply the new sum
        }
        res->limbs[i + b->n] += carry; // Add the remaining carry to the largest, 
                                       // most significant limb of the current sum
    }
    res->n = a->n + b->n;
}
static void __BIGINT_KARATSUBA__(const bigInt *x, const bigInt *y, bigInt *res, calc_ctx karat_ctx) {
    if (x->n <= BIGINT_SCHOOLBOOK && y->n <= BIGINT_SCHOOLBOOK) {
        __BIGINT_SCHOOLBOOK__(x, y, res); return;
    } //* ---- 1. SETUP ---- *?/
    size_t m = (size_t)(max(x->n, y->n) / 2);
    size_t  x0_range = m,  x1_range = x->n - m;
    size_t  y0_range = m,  y1_range = y->n - m;
    bigInt x0 = {.limbs = x->limbs,             .n = x0_range, .cap = x0_range};
    bigInt x1 = {.limbs = x->limbs + x0_range,  .n = x1_range, .cap = x1_range};
    bigInt y0 = {.limbs = y->limbs,             .n = y0_range, .cap = y0_range};
    bigInt y1 = {.limbs = y->limbs + y0_range,  .n = y1_range, .cap = y1_range};

    size_t karat_mark = scratch_mark(&karat_ctx);
    size_t z1_size = max(x1_range + y0_range, x0_range + y1_range) + m + 1;
    size_t z2_size = max(max(z1_size, x1_range + y1_range + 2*m), x0_range + y0_range) + 1;

    limb_t *tmp1_limbs = scratch_alloc(&karat_ctx, (max(x0_range, x1_range) + 1) * BYTES_IN_UINT64_T);
    limb_t *tmp2_limbs = scratch_alloc(&karat_ctx, (max(y0_range, y1_range) + 1) * BYTES_IN_UINT64_T);
    limb_t *z0_limbs = scratch_alloc(&karat_ctx, (x0_range + y0_range) * BYTES_IN_UINT64_T);
    limb_t *z1_limbs = scratch_alloc(&karat_ctx, z1_size * BYTES_IN_UINT64_T);
    limb_t *z2_limbs = scratch_alloc(&karat_ctx, z2_size * BYTES_IN_UINT64_T);

    bigInt tmp1 = {.limbs = tmp1_limbs, .n = 0, .cap = max(x0_range, x1_range) + 1};
    bigInt tmp2 = {.limbs = tmp2_limbs, .n = 0, .cap = max(y0_range, y1_range) + 1};
    bigInt z0 = {.limbs = z0_limbs, .n = 0, .cap = x0_range + y0_range};
    bigInt z1 = {.limbs = z1_limbs, .n = 0, .cap = z1_size};
    bigInt z2 = {.limbs = z2_limbs, .n = 0, .cap = z2_size};

    //* ------- 2. QUADRATIC COMPONENTS CALCULATION -------- *//
    // The procedure basically gpes:
    //  z3 = (x1 + x0)(y1 + y0)
    //  z2 = x1 * y1
    //  z0 = x0 * y0
    //  z1 = z3 - z2 - z1
    __BIGINT_KARATSUBA__(&x0, &y0, &z0, karat_ctx);
    __BIGINT_KARATSUBA__(&x1, &y1, &z2, karat_ctx);
    __BIGINT_ADD_WC__(&tmp1, &x1, &x0); __BIGINT_ADD_WC__(&tmp2, &y1, &y0);
    __BIGINT_KARATSUBA__(&tmp1, &tmp2, &z1, karat_ctx);
    __BIGINT_SUB_WB__(&z1, &z1, &z2); __BIGINT_SUB_WB__(&z1, &z1, &z0);

    //* ------------ 3. FINAL CALCULATION -------------- *//
    __BIGINT_INTERNAL_LLSHIFT__(&z2, 2*m); __BIGINT_INTERNAL_LLSHIFT__(&z1, m);
    __BIGINT_ADD_WC__(&z2, &z2, &z1); __BIGINT_ADD_WC__(&z2, &z2, &z0);
    __BIGINT_INTERNAL_COPY__(res, &z2);
    scratch_reset(&karat_ctx, karat_mark);
}
static void __BIGINT_TOOM_3__(const bigInt *m, const bigInt *n, bigInt *res, calc_ctx toom_ctx) {
    if (m->n <= BIGINT_SCHOOLBOOK && n->n <= BIGINT_SCHOOLBOOK) {
        __BIGINT_SCHOOLBOOK__(m, n, res); return;
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
    size_t toom_mark = scratch_mark(&toom_ctx);
    /*  ----------------- EVALUATION ---------------------
    *   +) pOuter = m0 + m2                          | +) qOuter = n0 + n2
    *   +) p(0)   = m0          (NO FULL TEMPORARY)  | +) q(0)   = n0          (NO FULL TEMPORARY)
    *   +) p(1)   = pOuter + m1                      | +) q(1)   = qOuter + n1
    *   +) p(-1)  = pOuter - m1                      | +) q(-1)  = qOuter - n1   
    *   +) p(-2)  = 2*(p(-1) + m2) - m0              | +) q(-2)  = 2*(q(-1) + n2) - n0
    *   +) p(inf) = m2          (NO FULL TEMPORARY)  | +) q(inf) = n2          (NO FULL TEMPORARY) */
    // p(x) TEMPORARIES                                  // q(x) TEMPORARIES
    BIGINT_TEMP(p_outer, k + 1, toom_ctx);     BIGINT_TEMP(q_outer, k + 1, toom_ctx);
    BIGINT_TEMP(p1,      k + 2, toom_ctx);     BIGINT_TEMP(q1,      k + 2, toom_ctx);
    BIGINT_TEMP(p_neg1,  k + 1, toom_ctx);     BIGINT_TEMP(q_neg1,  k + 1, toom_ctx);
    BIGINT_TEMP(p_neg2,  k + 2, toom_ctx);     BIGINT_TEMP(q_neg2,  k + 2, toom_ctx);
    // p(x) CALCULATIONS                                // q(x) CALCULATIONS
    __BIGINT_ADD_WC__(&p_outer, &m0, &m2);              __BIGINT_ADD_WC__(&q_outer, &m0, &n2);
    __BIGINT_ADD_WC__(&p1, &p_outer, &m1);              __BIGINT_ADD_WC__(&q1, &q_outer, &n1);
    __BIGINT_SUB_SAW__(&p_neg1, &p_outer, &m1);         __BIGINT_SUB_SAW__(&q_neg1, &q_outer, &n1);
    __BIGINT_ADD_SAW__(&p_neg2, &p_neg1, &m2);          __BIGINT_ADD_SAW__(&q_neg2, &q_neg1, &n2);
    __BIGINT_INTERNAL_LSHIFT__(&p_neg2, 1);             __BIGINT_INTERNAL_LSHIFT__(&q_neg2, 1);
    __BIGINT_SUB_SAW__(&p_neg2, &p_neg2, &m0);          __BIGINT_SUB_SAW__(&q_neg2, &q_neg2, &n0);
    /* ------------ POINT-WISE MULTIPLICATION ------------
    *   +) r(0)   = p(0)   * q(0)     
    *   +) r(1)   = p(1)   * q(1)
    *   +) r(-1)  = p(-1)  * q(-1)    
    *   +) r(-2)  = p(-2)  * q(-2)
    *   +) r(inf) = p(inf) * q(inf)                    */ 
    BIGINT_TEMP(r0,     (k << 1),               toom_ctx); // 2k
    BIGINT_TEMP(r1,     (k << 1) + 9,           toom_ctx); // 2k + 4 (original) --> 2k + 8 (interpolation - r1)
    BIGINT_TEMP(r_neg1, (k << 1) + 9,           toom_ctx); // 2k + 2 (original) --> 2k + 7 (interpolation - r2)
    BIGINT_TEMP(r_neg2, (k << 1) + 10,          toom_ctx); // 2k + 4 (original) --> 2k + 7 (interpolation - r3)
    BIGINT_TEMP(rinf,    m2size + n2size + 4,   toom_ctx); // 2k (original) ---> 2k + 4 (bit-shifts accounted)
    __BIGINT_TOOM_3__(&m0, &n0, &r0, toom_ctx); __BIGINT_TOOM_3__(&p1, &q1, &r1, toom_ctx);
    __BIGINT_TOOM_3__(&p_neg1, &q_neg1, &r_neg1, toom_ctx);
    __BIGINT_TOOM_3__(&p_neg2, &q_neg2, &r_neg2, toom_ctx);
    __BIGINT_TOOM_3__(&m2, &n2, &rinf, toom_ctx);


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
    BIGINT_TEMP(final_res, (k << 1) + 14, toom_ctx);
    __BIGINT_INTERNAL_LLSHIFT__(&rinf, 4);   __BIGINT_INTERNAL_LLSHIFT__(&r_neg2, 3);
    __BIGINT_INTERNAL_LLSHIFT__(&r_neg1, 2); __BIGINT_INTERNAL_LLSHIFT__(&r1, 1);
    __BIGINT_ADD_WC__(&final_res, &rinf, &r_neg2); __BIGINT_ADD_WC__(&final_res, &final_res, &r_neg1);
    __BIGINT_ADD_WC__(&final_res, &final_res, &r1); __BIGINT_ADD_WC__(&final_res, &final_res, &r0);
    __BIGINT_INTERNAL_COPY__(res, &final_res); scratch_reset(&toom_ctx, toom_mark);
}
static void __BIGINT_TOOM_4__(const bigInt *m, const bigInt *n, bigInt *res, calc_ctx toom_ctx) {}
static void __BIGINT_TOOM_5__(const bigInt *m, const bigInt *n, bigInt *res, calc_ctx toom_ctx) {}
static void __BIGINT_TOOM_6p5__(const bigInt *m, const bigInt *n, bigInt *res, calc_ctx toom_ctx) {}
static void __BIGINT_TOOM_7p5__(const bigInt *m, const bigInt *n, bigInt *res, calc_ctx toom_ctx) {}
static void __BIGINT_TOOM_8p5__(const bigInt *m, const bigInt *n, bigInt *res, calc_ctx toom_ctx) {}
static void __BIGINT_SSA__(const bigInt *a, const bigInt *b, bigInt *res, calc_ctx ssa_ctx) {}
void __BIGINT_MUL_DISPATCH__(const bigInt *a, const bigInt *b, bigInt *res, calc_ctx mul_ctx) {
    if (a->n <= BIGINT_SCHOOLBOOK && b->n <= BIGINT_SCHOOLBOOK) __BIGINT_SCHOOLBOOK__(a, b, res);
    else if (min(a->n, b->n) * 2 <= max(a->n, b->n)) __BIGINT_SCHOOLBOOK__(a, b, res);
    else if (a->n <= BIGINT_KARATSUBA && b->n <= BIGINT_KARATSUBA) __BIGINT_KARATSUBA__(a, b, res, mul_ctx);
    else if (a->n <= BIGINT_TOOM_3 && b->n <= BIGINT_TOOM_3) __BIGINT_TOOM_3__(a, b, res, mul_ctx);
    else if (a->n <= BIGINT_TOOM_4 && b->n <= BIGINT_TOOM_4) __BIGINT_TOOM_4__(a, b, res, mul_ctx);
    else if (a->n <= BIGINT_TOOM_5 && b->n <= BIGINT_TOOM_5) __BIGINT_TOOM_5__(a, b, res, mul_ctx);
    else if (a->n <= BIGINT_TOOM_6p5 && b->n <= BIGINT_TOOM_6p5) __BIGINT_TOOM_6p5__(a, b, res, mul_ctx);
    else if (a->n <= BIGINT_TOOM_7p5 && b->n <= BIGINT_TOOM_7p5) __BIGINT_TOOM_7p5__(a, b, res, mul_ctx);
    else if (a->n <= BIGINT_TOOM_8p5 && b->n <= BIGINT_TOOM_8p5) __BIGINT_TOOM_8p5__(a, b, res, mul_ctx);
    else __BIGINT_SSA__(a, b, res, mul_ctx);
}