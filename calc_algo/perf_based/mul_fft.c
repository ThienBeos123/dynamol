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
/* Sizing Function */
size_t __BIGINT_FFT_WS__(size_t a_size, size_t b_size) { return 0; }




/* ------------- Helper function ------------- */
static size_t __fft_best_metadata(size_t a_size, size_t b_size, size_t *outd, size_t *outm, size_t *outn) {
    size_t max_bits = max(a_size * U64_BITS, b_size * U64_BITS);
    size_t k = 2, d = 0, m = 0, n = 0;
    for (;;) {
        d = (size_t)1 << k; // d = 2^k
        m = (max_bits + d - 1) / d; // ceil(max_bits / d)
        n = d * m; // 2^n = 2^DM --> n = DM
        if (k + (m << 1) + 2 <= n) break; // k + 2M + slack <= n?
        ++k; // Increasing k until satisfies condition
    } *outd = d; *outm = m; *outn = n; return k;
}
static void __add_mod_fermat(bigInt *const out, const bigInt *const a, const bigInt *const b, size_t n, size_t nlimbs) {
    uint8_t carry = 0, nonzero_lows = 0;
    for (size_t i = 0; i < max(a->n, b->n); ++i) {
        limb_t x = (i < a->n) ? a->limbs[i] : 0;
        limb_t y = (i < b->n) ? b->limbs[i] : 0;
        out->limbs[i] = __ADD_UI64__(x, y, &carry);
        // Set the flag for lower-limbs being non-zero for correction later
        nonzero_lows |= out->limbs[i];
    }
    //* If result >= 2^n+1, -= 2^n+1 (Subtract) --> MODULAR CORRECTION
    // Subtracting 2^n+1 = clear bit n, Only do this
    // if the value is strictly > 2^n (bit n set AND low bits nonzero).
    size_t top_bit = n & ((UINT16_C(1) << 6) - 1); // Modulo 64
    if (((out->limbs[nlimbs - 1] >> top_bit) & 1) && nonzero_lows) {
        out->limbs[nlimbs - 1] &= ~(UINT16_C(1) << top_bit); // clear bit n
        uint64_t borrow = 1;
        for (size_t i = 0; i < nlimbs && borrow; ++i) {
            uint64_t prev = out->limbs[i];
            out->limbs[i] -= borrow;
            borrow = (out->limbs[i] > prev);
        }
    }
}
static void __sub_mod_fermat(bigInt *const out, const bigInt *const a, const bigInt *const b, size_t n, size_t nlimbs) {
    __BIGINT_SUB_SAW__(out, a, b);
    //* Modular Correction upon underflow (out_view < 0)
    //  We would achieve via subtraction of 2^n + 1 by the negative value,
    //  as -b + a == a - b. We would do this by having prefixed dummy values
    //  representing the first and last limb of a simulated bigInt == 2^n + 1
    if (out->sign < 0) {
        size_t bit_pos = n & ((UINT16_C(1) << 6) - 1); // Modulo 64
        limb_t first_limb = 0; /**/ limb_t last_limb = (UINT16_C(1) << bit_pos); 
        limb_t curr_limb = 0; /**/ uint8_t borrow = 0;
        size_t top_nonzero = 0;
        // Main subtraction operation
        for (size_t i = 0; i < nlimbs; ++i) {
            limb_t y = (i < out->n) ? out->limbs[i] : 0;
            if (unlikely(!i)) curr_limb = first_limb;
            else if (unlikely((i == nlimbs - 1))) curr_limb = last_limb;
            else curr_limb = 0;
            out->limbs[i] = __SUB_UI64__(curr_limb, y, &borrow);
            if (!(out->limbs[i])) top_nonzero = i;
        } out->sign = 1; out->n = top_nonzero;
    }
}
static void __negate_mod_fermat(bigInt *const out, const bigInt *in, size_t n, size_t nlimbs) {
    /**
     * This function is specifically designed to negate positive ins into negative, 
     * and is strictly advised to not use this on negative bigInt's ever inside this file.
     * It's purpose/existance is purely for specific optimization within's the domain of
     * implementing Schonhage-Strassen Integer Multiplication.
     */
    // The correction sequence for -in mod (2^n + 1) is just (2^n + 1) - in
    size_t bit_pos = n & ((UINT16_C(1) << 6) - 1); // Modulo 64
    limb_t first_limb = 0; /**/ limb_t last_limb = (UINT16_C(1) << bit_pos); 
    limb_t curr_limb = 0; /**/ uint8_t borrow = 0;
    size_t top_nonzero = 0;
    // Main subtraction operation
    for (size_t i = 0; i < nlimbs; ++i) {
        limb_t y = (i < out->n) ? in->limbs[i] : 0;
        if (unlikely(!i)) curr_limb = first_limb;
        else if (unlikely((i == nlimbs - 1))) curr_limb = last_limb;
        else curr_limb = 0;
        out->limbs[i] = __SUB_UI64__(curr_limb, y, &borrow);
        if (!(out->limbs[i])) top_nonzero = i;
    } out->sign = 1; out->n = top_nonzero;
}




/** ------------ Large 2^2n Ring-element Modular Reduction ------------
 * This function handles the modular reduction of roughly ~2n products produced
 * by Pointwise Multiplication. Since ring-eleent arithmetic functions, which includes
 * __add_mod_fermat and __sub_mod_fermat operates on the assumption of limbs bounded by
 * n+1 limbs, this function handles that doubling-scale outside of such arithmetic's capability.
 *
 * It operates by firstly splitting x (in) into 2 halves, represented now as such:
 *          
 *          x = (lo) + (hi * 2^n) 
 *          WHERE:
 *              lo = x[0 : n] bits
 *              hi = x[n + 1 : nlimbs] bits
 * 
 * We can now algebraically derive, in the algebraic field of Z/(2^n + 1)Z as so:
 *                2^n ≡ -1 (mod 2^n + 1)
 *  -->      hi * 2^n ≡ -hi
 *  --> lo + hi * 2^n ≡ lo + (-hi)         -------> [x ≡ lo - hi]
 */
static inline size_t _reduct_lo_req_(size_t n) { return (n >> 6) + !!(n & ((UINT16_C(1) << 6) - 1)); }
static inline size_t _reduct_hi_req_(size_t n) { return (n >> 6) + !!(n & ((UINT16_C(1) << 6) - 1)); }
static void __reduce_mod_fermat(
    bigInt *const out, const bigInt *const in, 
    limb_t *const lo_buf, limb_t *const hi_buf, size_t n
) {
    size_t top_limb = n >> 6; // Division by 64
    size_t top_bit = n & ((UINT16_C(1) << 6) - 1); // Modulo 64
    memset(lo_buf + top_limb, 0, (top_limb + !!(top_bit)) * U64_BYTES);
    memset(hi_buf + top_limb, 0, (top_limb + !!(top_bit)) * U64_BYTES);

    // Copying into lo from in (bit range: [0:n])
    memcpy(lo_buf, in, top_limb * U64_BYTES);
    if (top_bit) lo_buf[top_limb] = in->limbs[top_limb] & ((UINT16_C(1) << top_bit) - 1);
    // Copying into hi from in (bit range: [n+1:...])
    if (top_bit == 0) memcpy(hi_buf, in + top_limb, top_limb * sizeof(limb_t));
    else {
        for (size_t i = 0; i < top_limb - 1; ++i) {
            hi_buf[i] = (in->limbs[i + top_limb] >> top_bit) | (in->limbs[i + top_limb + 1] << (64 - top_bit));
        } hi_buf[top_limb - 1] = in->limbs[(top_limb << 1) - 1] >> top_bit;
    }
    // Final step: in mod(2^n + 1) ≡ lo - hi
    bigInt lo_view = { .limbs = lo_buf, .n = (top_limb + !!(top_bit)), .cap = (top_limb + !!(top_bit)), .sign = 1 };
    bigInt hi_view = { .limbs = hi_buf, .n = (top_limb + !!(top_bit)), .cap = (top_limb + !!(top_bit)), .sign = 1 };
    __sub_mod_fermat(out, &lo_view, &hi_view, n, top_limb);
}




/** ------------ Cyclic Ring-element Multiplication ------------
 * This function entire purpose is to calculate x * 2^s mod (2^n + 1)
 * in the algebraic ring of Z/(2^n + 1)Z with a ring-element of x.
 * This can be done with some simple bit-shifts and subtraction, as demonstrated below;
 *      
 *  +) 2^n ≡ -1 (mod 2^n + 1) (1)
 *  +) x = (lo) + (hi * 2^(n - s)) (2)
 *     WHERE:
 *         lo = x[0 : (n-s)] bits ----> lo_size = s bits
 *         hi = x[(n - s) : n] bits --> hi_size = (n - s) bits
 * 
 * THEREFORE, FROM (1) and (2), WE GOT:
 * --> x * 2^s  = lo * 2^s  +  hi * 2^(n-s) * 2^s
 *              = lo * 2^s  +  hi * 2^n
 *              ≡ lo * 2^s  -  hi
 */
static inline size_t _cshift_buf_req_(size_t s, size_t n, size_t nlimbs, size_t *out_hi) {
    size_t lo_bits = n - s;
    size_t lo_limbs = lo_bits >> 6;
    size_t lo_top_bit = (lo_bits & ((UINT16_C(1) << 6) - 1));
    *out_hi = (nlimbs - lo_limbs); return lo_limbs;
}
static void __cyclic_shift_mod(
    bigInt *const out, const limb_t *const x, 
    limb_t *const lo_buf, limb_t *const hi_buf,
    size_t s, size_t n, size_t nlimbs
) {
    //       2^n     = -1 mod(2^n + 1) 
    // ----> (2^n)^2 = (-1)^2 mod(2^n + 1)
    // ----> 2^(2n)  = 1 mod(2^n + 1)
    s %= (n << 1); // Handle cyclic period of s (2n)
    if (!s) { memcpy(out, x, nlimbs * U64_BYTES); return; } // x * 2^0 = x
    if (s >= n) s -= n; // Reducing s < n

    // Size pre-calculations
    size_t lo_bits = n - s; // lo = x[0 : (n-s)] bits
    size_t lo_limbs = lo_bits >> 6; // Dividing by 64
    size_t lo_top_bit = lo_bits & ((UINT16_C(1) << 6) - 1); // Modulo 64
    size_t hi_limbs = (nlimbs - lo_limbs);

    // Extracting from x into lo
    memcpy(lo_buf, x, lo_limbs * U64_BYTES); // Copying the first nlimbs-worth of bits
    if (lo_top_bit) lo_buf[lo_limbs] = x[lo_limbs] & ((UINT16_C(1) << lo_top_bit) - 1); // Copy the remaining bits

    // Extracting from x into hi
    if (!lo_top_bit) memcpy(hi_buf, x, hi_limbs * U64_BYTES);
    else {
        for (size_t i = 0; i + lo_limbs < nlimbs - 1; ++i) {
            hi_buf[i] = (x[i + lo_limbs] >> lo_top_bit) | (x[i + lo_limbs + 1] << (U64_BITS - lo_top_bit));
        } hi_buf[nlimbs - lo_limbs - 1] = x[nlimbs - 1] >> lo_top_bit;
    } bigInt hi_view = { .limbs = hi_buf, .n = hi_limbs, .cap = hi_limbs, .sign = 1 };

    // Final Calculations
    size_t s_llshift = s >> 6, s_lshift = s & ((UINT16_C(1) << 6) - 1);
    memcpy(out->limbs, lo_buf, (lo_limbs + !!(lo_top_bit)) * U64_BYTES); out->n = (lo_limbs + !!(lo_top_bit));
    __BIGINT_INTERNAL_LLSHIFT__(out, s_llshift); __BIGINT_INTERNAL_LSHIFT__(out, s_lshift);
    __sub_mod_fermat(out, out, &hi_view, n, nlimbs); // out = (lo * 2^s) - hi mod(2^n + 1)
    __BIGINT_INTERNAL_TRIM_LZ__(out); if (!out->n) out->sign = 1;
    // This negation of out upon s >= n is due to 2n being the full-period in Z/(2^n + 1)Z, 
    // and hence n being the half-period. If the full-period has the identity 2^(2n) = 1 mod(2^n + 1),
    // and the half-period has an indentity of 2^n = -1 mod(2^n + 1). Therefore, Multiplying by 2^s, 
    // where s is normalized to be period-normalized under 2n and larger than n, can only mean multiplying by -2^(s-n).
    if (s >= n && !(out->n)) __negate_mod_fermat(out, out, n, nlimbs);
}




/** ------------- Fast Fourier Transform -------------
* These 2 function is within only the scope of mul_fft.c as a helper function.
* It's purpose is to implement the Fast Fourier Transform, or, better suited for
* multiplication purposes, the Number Theoretic Transform, through the Cooley-Tukey Algorithm.
*/
static void _bigint_ctk_fft(limb_t *const data, size_t omega_shift, size_t d, size_t n) {}
static void _bigint_ctk_ifft(limb_t *const data, size_t omega_shift, size_t d, size_t n) {}




/* ------------- Main Orchestrating Function ------------- */
void __BIGINT_FFT__(PCONST_BIGINT a, PCONST_BIGINT b, P_BIGINT res, calc_ctx fft_ctx, dnml_status *err) {
    if (a->n <= BIGINT_SCHOOLBOOK && b->n <= BIGINT_SCHOOLBOOK) {
        __BIGINT_SCHOOLBOOK__(a, b, res); return; // Base-case
    } //* -------- 1. SETUP & SPLIT -------- *//
    // Splitting and Convolution Variables
    size_t fft_mark = scratch_mark(&fft_ctx); dnml_status err_check;
    size_t d = 0, m = 0, n = 0, k = __fft_best_metadata(a->n, b->n, &d, &m, &n);
    size_t mlimbs = (m + U64_BITS - 1) / U64_BITS; // Limbs per D-splitted Windows - ceil(M / U64_BITS)
    size_t nlimbs = (n + U64_BITS) / U64_BITS; // Limbs per Ring Element
    bigInt a_windows[d], b_windows[d];
    for (size_t i = 0; i < d; ++i) {
        size_t a_offset = i * mlimbs; size_t a_len = (a_offset < a->n) ? min(mlimbs, a->n - a_offset) : 0;
        size_t b_offset = i * mlimbs; size_t b_len = (b_offset < b->n) ? min(mlimbs, b->n - b_offset) : 0;
        a_windows[i] = (bigInt){ .limbs = a->limbs + a_offset, .n = a_len, .cap = mlimbs, .sign = 1 };
        b_windows[i] = (bigInt){ .limbs = b->limbs + b_offset, .n = b_len, .cap = mlimbs, .sign = 1 };
    }

}
