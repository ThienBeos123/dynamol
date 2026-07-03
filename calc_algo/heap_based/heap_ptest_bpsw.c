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



#include "heap_ptest.h"
#include <tables.h>
#include <debug_util.h>
#include "../../util/aconv_macros.h"
/** */
uint64_t __biheap_mod_word(const bigInt *const x, uint64_t mod) {
    uint64_t rem = 0, dummy = 0; uint8_t ovf_check;
    for (size_t i = x->n; i > 0; --i) {
        dummy = __DIV_HELPER_UI64__(rem, x->limbs[i - 1], mod, &rem, &ovf_check);
    } return rem;
}




/** ---------- 64-bit Binary Jacobi Symbol Algorithm ----------
 * This algorithm, for nearly all intend and purposes, is specifically used
 * inside this file as a mathematical tool aiding our modified Baillie-PSW
 * primality test to calculate strong parameters for Lucas Probable Prime tests.
 * It operates solely on machine-word integers (64-bit), and test the output
 * of the jacobi symbol of (D/n), where D is a signed integer and n is a bigInt,
 * with the outputs solely being either [0, 1, -1]
 */
int8_t _hbinary_jacobi(int64_t d, const bigInt *const n) {
    if (!(n->limbs[0] & 1)) return 0; // Jacobi Symbol is undefined on even inputs
    if (!d) return 0; /**/ if (d == 1) return 1;

    // Setup
    int8_t jacobi = 1; uint64_t a = 0, b = 0;
    if (d < 0 && (n->limbs[0] & 3) == 3) jacobi = -jacobi;
    a = __MAG_I64__(d); // Convert d to unsigned equivalent safely
    b = __biheap_mod_word(n, a); // Reduce n into a single machine word

    // Main loop - Binary Jacobi Symbol
    while (a) {
        uint64_t _bmod8_ = b % 8;
        while (!(a & 1)) { a >>= 1; /**/ if (_bmod8_ == 3 || _bmod8_ == 5) jacobi = -jacobi; }
        // Swap - forces to ALWAYS be larger than b
        if (a < b) {
            uint64_t tmp = a; a = b; b = tmp;
            if (a % 4 == 3 && b % 4 == 3) jacobi = -jacobi;
        }
        a = (a - b) >> 1; /**/ _bmod8_ = b % 8;
        if (_bmod8_ == 3 || _bmod8_ == 5) jacobi = -jacobi;
    }
    if (b == 1) return jacobi; /**/ else return 0;
}





/** ------------- Lucas/BPSW Early Exit conditions -------------
 * This function, for all intend and purposes, is strictly used as a helper
 * for __BIGINT_BPSW__ inside "ptest_bpsw.c", where conveniently track 
 * conditions that renders a particular bigInt n as a composite number.
 * This function does not include the early exit conditions for full-fledge
 * perfect squares checking due to the requirement of scratch-allocation and cleanups,
 * in which we lost the benefit of saving scratch spaces upon early exits
 */
uint8_t _lucas_edge_check(const bigInt *const n) {
    // Early exit conditions - Even/Odd + Divisibility by 5
    if (n->sign < 0) return 0;
    if (!n->n || (n->n == 1 && n->limbs[0] == 1)) return 0;
    if (!(n->limbs[0] & 1)) return 0;
    if (!(n->limbs[0] % 5) || n->limbs[0] % 5 == 5) return 0;
    // Early exit conditions - Perfect squares checking (Light checks)
    uint64_t limb_1 = n->limbs[0]; uint64_t _mod10_ = limb_1 % 10;
    uint64_t _mod16_ = limb_1 & 15; uint64_t _mod64_ = limb_1 & 63; 
    uint64_t _mod256_ = limb_1 & UINT8_MAX;
    if (_mod10_ == 1 || _mod10_ == 9) return 0; // Check in mod(10)
    if (_mod16_ == 1 || _mod16_ == 9) return 0; // Check in mod(16)
    for (uint8_t i = 0; i < _PFSQR_MOD64_CNT; ++i) if (_mod64_ == pfsqr_filter_mod64[i]) return 0;
    for (uint8_t i = 0; i < _PFSQR_MOD256_CNT; ++i) if (_mod256_ == pfsqr_filter_mod256[i]) return 0;
    return 1;
}






/** ---------- Probabilistic Lucas BPSW Test ----------
 * This test is designed to SPECIFICALLY Mirror the second part of the
 * Baillie-PSW test. We negate the Fermat Probable Prime test in Base-2
 * (A.K.A running Miller-Rabin inside Baillie-PSW), and go straight to finding Jacobian
 * symbols for our variables D, P, Q, and perform a Lucas Prime Test
 *
 * This reduces the test's strength quite a bit in exchange for much greater
 * performance. For greater security of primality certainty, there are compilation
 * options for mixing Miller-Rabin rounds with random-bases alongside our Lucas BPSW test
 */
uint8_t __BIHEAP_BPSW__(PCONST_BIGINT n, dnml_status *err) {
    if (!_lucas_edge_check(n)) { *err = BIGINT_SUCCESS; return 0; } // Early exit conditions - Light checks
    // Early exit conditions - Perfect squares checking (Full check)
    dnml_status echeck = BIGINT_SUCCESS;
    bigInt *alloc_list[10] = {0}, *early_free[10] = {0};
    uint8_t alloc_cnt = 0, early_cnt = 0;
    BIHEAP_TEMP(tmp1, n->n << 1, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt, 0); tmp1.cap = (n->n >> 1) + 1;
    BIHEAP_TEMP(tmp2, n->n, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt, 0);
    __BIHEAP_SQRT_DISP__(&tmp1, n, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt, 0);
    __BIHEAP_MUL_DISP__(&tmp1, &tmp1, &tmp2, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt, 0);
    if (__BIGINT_INTERNAL_COMP__(n, &tmp2)) { _free_alloc_list(alloc_list, alloc_cnt); *err = BIGINT_SUCCESS; return 0; }
    

    

    /* ----------- Normal Operation - Lucas Probable Prime Test ----------- */
    // Precomputation with D, P, and Q
    int64_t d = 5, q = 0; uint64_t p = 1; // Starting with 5: [5, -7, 9, -11, ...]
    while (_hbinary_jacobi(d, n) != -1) { d = llabs(d) + 2; /**/ d = -d; } // Add 2 then flip sign
    if (d == 5) { p = 5; q = 5; } // Better testing strength (A* method on Wikipedia)
    else q = (1 - d) / 4; // We use a standard signed division here to be safe
    uint64_t mag_q = __MAG_I64__(q); uint64_t mag_d = __MAG_I64__(d);
    

    // Precomputing d, s, and Lucas Sequence's indicies (n + 1 = d * 2^s)
    // s is guaranteed to != 0 due to n being odd, and n + 1, being even, and therefore s >= 1
    // d is also guaranteed to be odd due s representing (n + 1)'s trailing zeros
    uint64_t a = 1; bigInt one = { .limbs = &a, .n = 1, .cap = 1, .sign = 1 };
    tmp2.cap += 1; __BIGINT_ADD_WC__(&tmp2, n, &one); // tmp2 = n + 1
    size_t s = __BIGINT_CTZ__(&tmp2); size_t limb_shift = s >> 6; uint8_t bshift = s & 63;
    __BIGINT_INTERNAL_RLSHIFT__(&tmp2, limb_shift); __BIGINT_INTERNAL_RSHIFT__(&tmp2, bshift); // tmp2 = d


    // Pre-Phase A: Setting up U1, V1, and Q1 for d's loop
    tmp1.cap = n->n << 1; 
    mont_ctx mont_ctx = {.n = n, .nprime = __MODINV_UI64__(n->limbs[0]), .k = n->n};
    BIHEAP_TEMP(r, n->n + 1, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt, 0); r.n = n->n + 1; r.limbs[n->n] = 1;
    BIHEAP_TEMP(r_mod_n, n->n + 1, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt, 0); r_mod_n.cap -= 1;
    __BIHEAP_MOD_DISP__(&r, n, &r_mod_n, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt, 0);
    __BIHEAP_MUL_DISP__(&r_mod_n, &r_mod_n, &tmp1, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt, 0);
    __BIHEAP_MOD_DISP__(&tmp1, n, &tmp1, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt, 0);
    mont_ctx.r2 = &tmp1; const bigInt *const r2 = mont_ctx.r2;
    // Conversins of p and q into Montgomery form for V1 and Q1
    bigInt pview = { .limbs = &p, .n = 1, .cap = 1, .sign = 1 };
    bigInt qview = { .limbs = &mag_q, .n = !!(q), .cap = 1, .sign = (q < 0) ? -1 : 1 };
    bigInt dview = { .limbs = &mag_d, .n = !!(d), .cap = 1, .sign = (d < 0) ? -1 : 1 };
    BIHEAP_TEMP(U, n->n , echeck, err, early_free, early_cnt, alloc_list, alloc_cnt, 0); U.limbs[0] = 1; U.n = 1; // U1 = 1
    BIHEAP_TEMP(V, n->n + 1, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt, 0); // V1 = p
    BIHEAP_TEMP(Q, n->n, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt, 0); // Q1 = q
    BIHEAP_TEMP(Dmont, n->n, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt, 0); Dmont.sign = (d < 0) ? -1 : 1;
    BIHEAP_TEMP(Qmont, n->n, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt, 0); bigInt Pmont = {0};
    if (unlikely(p == 5)) { echeck = __BIGINT_INTERNAL_LINIT__(&Pmont, n->n); HEAP_OOM(echeck, err, early_free, early_cnt, 0); }
    __BIHEAP_MONTMUL__(&pview, r2, mont_ctx, &V, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt, 0);
    __BIHEAP_MONTMUL__(&qview, r2, mont_ctx, &Q, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt, 0);
    __BIHEAP_MONTMUL__(&dview, r2, mont_ctx, &dview, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt, 0);
    if (unlikely(p == 5)) __BIGINT_INTERNAL_COPY__(&Pmont, &V); /**/ __BIGINT_INTERNAL_COPY__(&Qmont, &Q);


    

    // Phase A: Lucas Sequence recurrence up to Ud, Vd, and Qd
    size_t dbits = (tmp2.n << 6) - __CLZ_UI64__(tmp2.limbs[tmp2.n - 1]); /**/ r.cap = n->n; r_mod_n.cap += 1;
    for (size_t i = dbits - 1; i > 0; --i) { // Loop from second-most MSB --> LSB (Skip MSB due to it being a NOP)
        /** -------- Doubling recurrence relationship model --------
         * U{2k} = U{k} * V{k}
         * V{2k} = V{k}^2 - 2Q{k}
         * Q{2k} = Q{k}^2 
         */
        // We temporarily store Q{2k} on r_mod_n (same size) due to Q{k} being used for V{2k}
        // We also temporarily store V{2k} on r (>= in size) due to V{k} being used for U{2k}
        __BIHEAP_MONTMUL__(&Q, &Q, mont_ctx, &r_mod_n, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt, 0);
        __BIHEAP_MONTMUL__(&V, &V, mont_ctx, &r, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt, 0);
        __BIGINT_SUB_SAW__(&r, &r, &Q); __BIGINT_SUB_SAW__(&r, &r, &Q); // r - Q twice = r - 2Q
        if (r.sign == -1) __BIGINT_ADD_SAW__(&r, &r, n); // Underflow correction in mod(n) == adding n
        // We now compute U{2k} (We also, from now on, treat r as having only n->n as its capacity)
        __BIHEAP_MONTMUL__(&U, &V, mont_ctx, &U, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt, 0);
        __BIGINT_INTERNAL_SWAP__(&V, &r); __BIGINT_INTERNAL_SWAP__(&Q, &r_mod_n);


        /** ------- Doubling +1 recurrence relationship model -------
         * This case only occurs ONLY when our current bit is set/1
         * U{2k+1} = (PU{2k} + V{2k}) / 2
         * V{2k+1} = (DU{2k} + PV{2k}) / 2
         * Q{2k+1} = (Q * Q{2k})
         */
        size_t dlimb_idx = i >> 6; uint8_t dbit_idx = i & 63;
        if ((tmp2.limbs[dlimb_idx] >> (dbit_idx - 1)) & 1) {
            // 1. Computing V{2k+1} (Store it on r_mod_n for more stability due to size compatibility)
            __BIHEAP_MONTMUL__(&Dmont, &U, mont_ctx, &r_mod_n, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt, 0);
            bigInt *op = &V;
            if (unlikely(p == 5)) { // Normally, if (p == 1) --> PV{2k} = V{2k}
                __BIHEAP_MONTMUL__(&Pmont, &V, mont_ctx, &r, &echeck);
                HEAP_OOM(echeck, err, early_free, early_cnt, 0); /**/ op = &r;
            } 
            r_mod_n.sign = Dmont.sign; __BIGINT_ADD_SAW__(&r_mod_n, &r_mod_n, op); uint8_t correction = 1;
            if (r_mod_n.sign < 0) { __BIGINT_SUB_WB__(&r_mod_n, &r_mod_n, &r); r.sign = 1; correction = 0; } // Correction step for mod(n) 
            __BIGINT_INTERNAL_RSHIFT__(&r_mod_n, 1); // We ONLY apply correction when the result is positive
            if (correction) { // Due to an negative r being modularly corrected to be in mod(n) already
                int8_t cmp_res = __BIGINT_INTERNAL_COMP__(&r_mod_n, n);
                if (!cmp_res || cmp_res > 0) __BIGINT_SUB_WB__(&r_mod_n, &r_mod_n, n);
            }


            // 2. Computing U{2k+1} + Q{2k+1} (Simpler, both operated inlined on U and Q)
            if (unlikely(p == 5)) { // Normally, if (p == 1) --> PV{2k} = V{2k}
                __BIHEAP_MONTMUL__(&Pmont, &U, mont_ctx, &U, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt, 0);
            } __BIGINT_ADD_WC__(&U, &U, &V); __BIGINT_INTERNAL_RSHIFT__(&U, 1);
            int8_t cmp_res = __BIGINT_INTERNAL_COMP__(&U, n); // r_mod_n is gauranteed to be positive
            if (!cmp_res || cmp_res > 0) __BIGINT_SUB_WB__(&U, &U, n); // --> more frequent correction checks
            __BIGINT_INTERNAL_SWAP__(&V, &r_mod_n); // V{2k+1} = r_mod_n (Shallow struct swap)
            __BIHEAP_MONTMUL__(&Qmont, &Q, mont_ctx, &Q, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt, 0);
        }
    }  //* 1st Congruence Condition: U{d} = 0 mod(n) ---> n is a Lucas Probable prime
    if (!U.n) { _free_alloc_list(alloc_list, alloc_cnt); return 1; }




    // Phase B: Lucas Sequence recurrence continuation up to U{n+1}, V{n+1}, and Q{n+1}
    // This phase utilizes solely the "Doubling Recurrence Relationship" due to s being a power of 2,
    // representing (n+1)'s CTZ, and therefore its bits is always 0 from [MSB-1:0]
    for (size_t i = 0; i < s; ++i) {
        /** -------- Doubling recurrence relationship model --------
         * V{2k} = V{k}^2 - 2Q{k}
         * Q{2k} = Q{k}^2 
         */
        // We temporarily store Q{2k} on r_mod_n (same size) due to Q{k} being used for V{2k}
        // We also temporarily store V{2k} on r (>= in size) due to V{k} being used for U{2k}
        __BIHEAP_MONTMUL__(&V, &V, mont_ctx, &r, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt, 0);
        __BIGINT_SUB_SAW__(&r, &r, &Q); __BIGINT_SUB_SAW__(&r, &r, &Q); // r - Q twice = r - 2Q
        if (r.sign == -1) __BIGINT_ADD_SAW__(&r, &r, n); /**/ __BIGINT_INTERNAL_SWAP__(&V, &r);
        __BIHEAP_MONTMUL__(&Q, &Q, mont_ctx, &Q, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt, 0);

        //* 2nd Congruence Condition: V{d * 2^i} = 0 mod(n) ---> n is a Lucas Probable Prime
        // This congruence condition is naturally checked by our squaring/doubling loop.
        // Our Lucas sequences term V and Q all started from V{d} and Q{d}, and move closer
        // to V{n+1} and Q{n+1}, or also can be expressed as V{d * 2^s} and Q{d * 2^s}. Therefore,
        // i represents the amount of doubling steps to go from term D to term D * 2^s (final term) in our Lucas sequence.
        if (!V.n) { _free_alloc_list(alloc_list, alloc_cnt); return 0; }
    } _free_alloc_list(alloc_list, alloc_cnt); return 0; //* n is Most Definitely Composite
}
