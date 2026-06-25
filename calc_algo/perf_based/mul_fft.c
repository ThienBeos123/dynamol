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
static size_t _tmpbuf_req_(size_t nlimbs) { return nlimbs; }
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
        limb_t first_limb = 0; /**/ limb_t last_limb = (UINT64_C(1) << bit_pos); 
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
    limb_t first_limb = 0; /**/ limb_t last_limb = (UINT64_C(1) << bit_pos); 
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
 * We can now algebraically derive, in the algebraic field of ℤ/(2^n + 1)ℤ as so:
 *                2^n ≡ -1 (mod 2^n + 1)
 *  -->      hi * 2^n ≡ -hi
 *  --> lo + hi * 2^n ≡ lo + (-hi)         -------> [x ≡ lo - hi]
 */
static void __reduce_mod_fermat(
    bigInt *const out, const bigInt *const in, 
    limb_t *const lo_buf, limb_t *const hi_buf,
    size_t n, size_t nlimbs
) {
    size_t top_bit = n & ((UINT16_C(1) << 6) - 1); // Modulo 64
    memset(lo_buf + nlimbs, 0, (nlimbs + !!(top_bit)) * U64_BYTES);
    memset(hi_buf + nlimbs, 0, (nlimbs + !!(top_bit)) * U64_BYTES);

    // Copying into lo from in (bit range: [0:n])
    memcpy(lo_buf, in->limbs, nlimbs * U64_BYTES);
    if (top_bit) lo_buf[nlimbs] = in->limbs[nlimbs] & ((UINT16_C(1) << top_bit) - 1);
    // Copying into hi from in (bit range: [n+1:...])
    if (top_bit == 0) memcpy(hi_buf, in + nlimbs, nlimbs * sizeof(limb_t));
    else {
        for (size_t i = 0; i < nlimbs - 1; ++i) {
            hi_buf[i] = (in->limbs[i + nlimbs] >> top_bit) | (in->limbs[i + nlimbs + 1] << (64 - top_bit));
        } hi_buf[nlimbs - 1] = in->limbs[(nlimbs << 1) - 1] >> top_bit;
    }
    // Final step: in mod(2^n + 1) ≡ lo - hi
    bigInt lo_view = { .limbs = lo_buf, .n = (nlimbs + !!(top_bit)), .cap = (nlimbs + !!(top_bit)), .sign = 1 };
    bigInt hi_view = { .limbs = hi_buf, .n = (nlimbs + !!(top_bit)), .cap = (nlimbs + !!(top_bit)), .sign = 1 };
    __BIGINT_INTERNAL_TRIM_LZ__(&lo_view); __BIGINT_INTERNAL_TRIM_LZ__(&hi_view);
    __sub_mod_fermat(out, &lo_view, &hi_view, n, nlimbs);
}




/** ------------ Cyclic Ring-element Multiplication ------------
 * This function entire purpose is to calculate x * 2^s mod (2^n + 1)
 * in the algebraic ring of ℤ/(2^n + 1)ℤ with a ring-element of x.
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
static void __cyclic_shift_mod(
    bigInt *const out, const bigInt *const x, 
    limb_t *const lo_buf, limb_t *const hi_buf,
    size_t s, size_t n, size_t nlimbs
) {
    //       2^n     = -1 mod(2^n + 1) 
    // ----> (2^n)^2 = (-1)^2 mod(2^n + 1) 
    // ----> 2^(2n)  = 1 mod(2^n + 1)
    s %= (n << 1); // Handle cyclic period of s (2n)
    if (!s) { memcpy(out->limbs, x->limbs, nlimbs * U64_BYTES); return; } // x * 2^0 = x
    uint8_t negate_flag = 0; if (s >= n) { s -= n; negate_flag = 1; } // Reducing s < n

    // Size pre-calculations
    size_t lo_bits = n - s; // lo = x[0 : (n-s)] bits
    size_t lo_limbs = lo_bits >> 6; // Dividing by 64
    size_t lo_top_bit = lo_bits & ((UINT16_C(1) << 6) - 1); // Modulo 64
    size_t hi_limbs = (nlimbs - lo_limbs);

    // Extracting from x into lo
    memcpy(lo_buf, x->limbs, lo_limbs * U64_BYTES); // Copying the first nlimbs-worth of bits
    if (lo_top_bit) lo_buf[lo_limbs] = x->limbs[lo_limbs] & ((UINT16_C(1) << lo_top_bit) - 1); // Copy the remaining bits

    // Extracting from x into hi
    if (!lo_top_bit) memcpy(hi_buf, x->limbs + lo_limbs, hi_limbs * U64_BYTES);
    else {
        for (size_t i = 0; i + lo_limbs < nlimbs - 1; ++i) {
            hi_buf[i] = (x->limbs[i + lo_limbs] >> lo_top_bit) | (x->limbs[i + lo_limbs + 1] << (U64_BITS - lo_top_bit));
        } hi_buf[nlimbs - lo_limbs - 1] = x->limbs[nlimbs - 1] >> lo_top_bit;
    } bigInt hi_view = { .limbs = hi_buf, .n = hi_limbs, .cap = hi_limbs, .sign = 1 };

    // Final Calculations
    size_t s_llshift = s >> 6, s_lshift = s & ((UINT16_C(1) << 6) - 1);
    memcpy(out->limbs, lo_buf, (lo_limbs + !!(lo_top_bit)) * U64_BYTES); out->n = (lo_limbs + !!(lo_top_bit));
    __BIGINT_INTERNAL_LLSHIFT__(out, s_llshift); __BIGINT_INTERNAL_LSHIFT__(out, s_lshift);
    __sub_mod_fermat(out, out, &hi_view, n, nlimbs); // out = (lo * 2^s) - hi mod(2^n + 1)
    __BIGINT_INTERNAL_TRIM_LZ__(out); if (!out->n) out->sign = 1;
    // This negation of out upon s >= n is due to 2n being the full-period in ℤ/(2^n + 1)ℤ, 
    // and hence n being the half-period. If the full-period has the identity 2^(2n) = 1 mod(2^n + 1),
    // and the half-period has an indentity of 2^n = -1 mod(2^n + 1). Therefore, Multiplying by 2^s, 
    // where s is normalized to be period-normalized under 2n and larger than n, can only mean multiplying by -2^(s-n).
    if (negate_flag && !(out->n)) __negate_mod_fermat(out, out, n, nlimbs);
}




/** ------------- Fast Fourier Transform -------------
 * This function is within only the scope of mul_fft.c as a helper function.
 * It's purpose is to implement the Fast Fourier Transform, or, better suited for
 * multiplication purposes, the Number Theoretic Transform, through the Cooley-Tukey Algorithm.
 * This implementation mostly follows the iterative radix-2 FFT implementation shown on Wikipedia 
 * for the Cooley-Tukey algorithm, with slight tweaks to calculate the
 * Discrete Fourier Transform (DFT for short) in the ℤ/(2^n + 1)ℤ field:
 *
 *      Cooley-Tukey FFT: https://en.wikipedia.org/wiki/Cooley%E2%80%93Tukey_FFT_algorithm
 *      Discrete Fourier Transform: https://en.wikipedia.org/wiki/Discrete_Fourier_transform
 *
 * We modify the Wikipedia's implementation slightly. Instead of working on scalar complex
 * values, often used for waves transformation of signals, that fits inside one micro-architectural
 * word, we operate on ring-element bigInts through modular arithmetic in ℤ/(2^n + 1)ℤ (__add_mod_fermat,
 * __sub_mod_fermat, __cyclic_shift_mod, etc). We also transform the Primitive Root of Unity (as well
 * as the Inverse Primitive Root of Unity) to be an integer in ℤ(2^n + 1)ℤ, as displayed and annotated below.
 */
static size_t __bit_reverse(size_t in, size_t k) {
    size_t r = 0;
    for (size_t i = 0; i < k; i++) { 
        r = (r << 1) | (in & 1);
        in >>= 1;
    } return r;
}
static void _bigint_ctk_fft(
    bigInt *const evalp, 
    limb_t *const tbuf, limb_t *const usave_buf,
    limb_t *const lo_buf, limb_t *const hi_buf,
    size_t d, size_t k, size_t n, size_t nlimbs
) {
    /* ----------- BIT REVERSAL ----------- */
    for (size_t i = 0; i < d; ++i) {
        size_t j = __bit_reverse(i, k);
        if (i < j) __BIGINT_INTERNAL_SWAP__(&evalp[i], &evalp[j]);
    }
    /* --------- BUTTERFLY TRANSFORM --------- */
    // This step is responsible for evaluating the DFT of the D ring-elements
    // of ℤ/(2^n + 1)ℤ in our evaluation step (as mapped structurally to Toom-Cook).
    bigInt tbuf_view = { .limbs = tbuf, .n = 0, .cap = nlimbs, .sign = 1 };
    bigInt u_save = { .limbs = usave_buf, .n = 0, .cap = nlimbs, .sign = 1 };
    for (size_t len = 2; len <= d; len <<= 1) {
        size_t half_len = len >> 1;
        size_t tw_step = (n << 1) / len;
        // The tw_step here corresponds to Wikipedia's ω_m = exp(−2πi/m).
        // This is for (ω_m)^m (due to e^(−2πi) = 1), and therefore ω_m is a primitive
        // mth-root of unity. We can, instead, switch to work on ℤ/(2^n+1)ℤ, where tw_step =
        // 2^(2n/D) will be our primitive Dth-root of unity, where we've already known that
        // 2^(2n) = 1 mod(2^n+1) (2^n = -1 mod(2^n+1)), and therefore, (tw_step)^D = 1
        for (size_t k = 0; k < d; k += len) {
            size_t tw_exp = 0; // tw_step = 1 = 2^0
            for (size_t j = 0; j < half_len; ++j) {
                // Retrieving and calculating u and t
                // Wikipedia: t = ω A[k + j + m/2] && u = A[k + j]
                bigInt *u = &evalp[k + j];
                bigInt *v = &evalp[k + j + half_len];
                __cyclic_shift_mod(&tbuf_view, v, lo_buf, hi_buf, tw_exp, n, nlimbs);
                __BIGINT_INTERNAL_TRIM_LZ__(&tbuf_view);

                // Evaluation calculation of FFT
                // u = (u + t) && v = (u - t)
                memcpy(usave_buf, u->limbs, nlimbs * U64_BYTES);
                u_save.n = nlimbs; __BIGINT_INTERNAL_TRIM_LZ__(&u_save);
                __add_mod_fermat(u, &u_save, &tbuf_view, n, nlimbs);
                __sub_mod_fermat(v, &u_save, &tbuf_view, n, nlimbs);

                // Modularly-reduce the twiddle-factor exponent so that the power is no more than 2n
                tw_exp += tw_step; /**/ if (tw_exp >= n << 1) tw_exp -= n << 1;
            }
        }
    }
}




/** --------- Inverse Fast Fourier Transform --------- 
 * This function, also only within the scope of mul_fft.c as helpers of __BIGINT_FFT__,
 * calculates the Inverse Discrete Fourier Transform through modifying Cooley-Tukey FFT.
 * This modification is marginal, and some technical discrepancies is derived as follows.
 * The definition, as detailed in Wikipedia, of the Inverse Discrete Fourier Transform
 * after discretely transform {Xk} to {xn} is:
 * 
 *      x_n = 1/N * Σ{k=0->N-1}Xk * e^(i2π * k/N * n)
 *          = 1/N * Σ{k=0->N-1}Xk * (e^(i2π/N))^(kn)
 *
 * This version of Discrete Fourier Transform is the generalization for applying the transform
 * over the field of ℤ and ℂ. We can abstract away e^(i2π/N) to generally be ω^-1, representing the 
 * Inverse Primitive N-th Root of Unity over a field of A, where ω = e^(-i2π/N), representing the
 * Primitive N-th Root of Unity. Therefore, we can formulate the abstracted version as
 *
 *      x_n = 1/N * Σ{k=0->N-1}Xk * ω^(-kn)
 *
 * Now, since we are transforming a list of bigInt under the ring of ℤ/(2^n + 1)ℤ, the Primitive
 * D-th Root of Unity, as derived from 2^n = -1 mod(2^n + 1) ---> 2^2n = 1 mod(2^n + 1), as
 * 2^(2n/D). Therefore, we can fully formulate the Inverse Discrete Fourier Transform,
 * transforming a fourier-transformed sequence of bigInt/ring-elements in ℤ/(2^n + 1)ℤ, {Xk}, 
 * with a total of D elements (D windows) into {Xn} as follows:
 *
 *      x[n] = 1/D * Σ{k=0->D-1}X[k] * (2^(2n/D))^(-kn)
 *           = 1/D * Σ{k=0->D-1}X[k] * 2^(2n/D * -kn)
 *           = 1/D * Σ{k=0->D-1}X[k] * 2^(-(2n/D) * kn)
 *           = 1/D * Σ{k=0->D-1}X[k] * 2^((2n - (2n/D)) * kn)
 *
 * The scaling term of ω^-1 is now transformed into the Inverse Primitive Dth-Root of Unity, with the scaling
 * factor of kn, and derived as such. The scaling factor was already represented correctly in _bigint_ctk_fft via
 * tw_step = (n << 1) / len, scaling with the for loop, and we're just efficiently applying the inverse of it. 
 *
 * For the correction term of 1/D, since D is defined as 2^k, it would be defined as the multiplicative inverse of 2^k
 * over ℤ/(2^n + 1)ℤ, represented mathematically as D^{-1} mod(2^n + 1). Despite multiplying by 1/2^k, or simply
 * dividing by 2^k in ℤ would typically convey a simple bit-shift by k, we're working in a finite ring, ℤ/(2^n + 1)ℤ,
 * and therefore any division is defined as the multiplicative inverse with some sort of modular reduction. This is
 * mathematically implemented as adding 2^n + 1 upon odd values and then right shift by 1, and is proven as so:
 *
 *          FOR x ∈ ℤ/(2^n + 1)ℤ
 *          IF: x mod(2) = 0 ----> x >> 1
 *          ELSE:
 *              +) We want to find an integer y that is modularly congruent to x / 2
 *                 when x is ODD. Therefore, y must satisfy the following criterion:
 *
 *                      2y = x mod(2^n + 1)
 *
 *                 When we add 2^n + 1, an ODD value, to x, an ODD value, we yield an even integer
 *                 in ℤ/(2^n + 1)ℤ, and hence is eglible for division by 2. We denote another integer z, 
 *                 in which:
 *
 *                      z  = (x + 2^n + 1) / 2 mod(2^n + 1)
 *                 ---> 2z = (x + 2^n + 1) mod(2^n + 1)
 *
 *                 Since in ℤ/(2^n + 1)ℤ, 2^n + 1 is modularly congruent to 0 mod(2^n + 1), we therefore
 *                 note that 2z = x mod(2^n + 1), and therefore conclude that z = y.
 *
 *              |-------------------------------------------|
 *              |   --> ((x + 2^n + 1) / 2) = (x / 2)       |
 *              |        WHEN x ∈ ℤ/(2^n + 1)ℤ AND x IS ODD |
 *              |-------------------------------------------|
 */
static void _bigint_ctk_ifft(
    bigInt *const evalp, 
    limb_t *const tbuf, limb_t *const usave_buf,
    limb_t *const lo_buf, limb_t *const hi_buf,
    size_t d, size_t k, size_t n, size_t nlimbs
) {
    /* ----------- BIT REVERSAL ----------- */
    for (size_t i = 0; i < d; ++i) {
        size_t j = __bit_reverse(i, k);
        if (i < j) __BIGINT_INTERNAL_SWAP__(&evalp[i], &evalp[j]);
    }
    /* --------- BUTTERFLY TRANSFORM --------- */
    // This step is responsible for evaluating the Inverse-DFT of the D ring-elements
    // of ℤ/(2^n + 1)ℤ in our evaluation step (as mapped structurally to Toom-Cook).
    bigInt tbuf_view = { .limbs = tbuf, .n = 0, .cap = nlimbs, .sign = 1 };
    bigInt u_save = { .limbs = usave_buf, .n = 0, .cap = nlimbs, .sign = 1 };
    for (size_t len = 2; len <= d; len <<= 1) {
        size_t half_len = len >> 1;
        size_t tw_step_fwd = (n << 1) / len;
        for (size_t k = 0; k < d; k += len) {
            size_t tw_exp_wd = 0; // tw_step = 1 = 2^0
            for (size_t j = 0; j < half_len; ++j) {
                // Retrieving and calculating u and t
                // Wikipedia: t = ω A[k + j + m/2] && u = A[k + j]
                bigInt *u = &evalp[k + j];
                bigInt *v = &evalp[k + j + half_len];

                // We now apply the Inverse Primtive Dth-Root of ℤ/(2^n + 1)ℤ, which
                // given as (2^(2n/D))^-1 = 2^(-(2n/D)) = 2^(2n - (2n/D))
                size_t inv_tw_exp = ((n << 1) - tw_exp_wd) % (n << 1); // Twiddle factor has a period of 2n
                __cyclic_shift_mod(&tbuf_view, v, lo_buf, hi_buf, inv_tw_exp, n, nlimbs);
                __BIGINT_INTERNAL_TRIM_LZ__(&tbuf_view);

                // Evaluation calculation of FFT
                // u = (u + t) && v = (u - t)
                memcpy(usave_buf, u->limbs, nlimbs * U64_BYTES);
                u_save.n = nlimbs; __BIGINT_INTERNAL_TRIM_LZ__(&u_save);
                __add_mod_fermat(u, &u_save, &tbuf_view, n, nlimbs);
                __sub_mod_fermat(v, &u_save, &tbuf_view, n, nlimbs);

                // Modularly-reduce the twiddle-factor exponent so that the power is no more than 2n
                tw_exp_wd += tw_step_fwd; /**/ if (tw_exp_wd >= n << 1) tw_exp_wd -= n << 1;
            }
        }
    }
    /* -------- DIVISION CORRECTION FOR IDFT -------- */
    // This step is responsible for correcting the over-scaling by D of the butterfly
    // transform when applying the Inverse Primitive Dth-Root of unity, multiplying by
    // the multiplicative inverse of D, 1/D, to each term/bigInt of {evalp}
    for (size_t i = 0; i < d; ++i) {
        // Since we're dividing by D, which is just 2^k, we want to shift k times,
        // hence k rounds of right shifting by 1. We incorporate the shifting inside a loop
        // to account for odd cases dividing by 2 
        for (size_t round = 0; round < k; ++round) {
            if (!evalp[i].n) break; // evalp[i] = 0 ---> Skip
            
            // Modular halving: if odd, add 2^n+1 first (ODD + ODD = EVEN)
            if (evalp[i].limbs[0] & 1) {
                size_t top_bit = n & ((UINT16_C(1) << 6) - 1);
                evalp[i].limbs[0] += 1;
                // The first limb only overflows when it 
                // becomes 0 from +1 into UINT64_MAX
                uint8_t carry = !(evalp[i].limbs[0]);
                for (size_t j = 1; j <= nlimbs && carry; ++j) {
                    uint64_t tmp = evalp[i].limbs[j];
                    evalp[i].limbs[j] += carry;
                    carry = (evalp[i].limbs[j] < tmp);
                } // +2n via flipping top_bit
                evalp[i].limbs[nlimbs] |= (UINT64_C(1) << top_bit); 
            } __BIGINT_INTERNAL_RSHIFT__(&evalp[i], 1);
        } __BIGINT_INTERNAL_TRIM_LZ__(&evalp[i]);
        if (evalp[i].n == 0) evalp[i].sign = 1;
    }
}




/* ------------- Main Orchestrating Function ------------- */
void __BIGINT_FFT__(PCONST_BIGINT a, PCONST_BIGINT b, P_BIGINT res, calc_ctx fft_ctx, dnml_status *err) {
    if (a->n <= BIGINT_SCHOOLBOOK && b->n <= BIGINT_SCHOOLBOOK) {
        __BIGINT_SCHOOLBOOK__(a, b, res); return; // Base-case
    } //* -------- 1. SETUP & SPLIT -------- *//
    // Splitting and Convolution Variables
    size_t fft_mark = scratch_mark(&fft_ctx); dnml_status err_check;
    size_t d = 0, m = 0, n = 0, k = __fft_best_metadata(a->n, b->n, &d, &m, &n);
    size_t mlimbs = (m + U64_BITS - 1) >> 6; // Limbs per D-splitted Windows - ceil(M / U64_BITS)
    size_t nlimbs = (n + U64_BITS) >> 6; // Upperbound limit of ring-element in ℤ/(2^n+1)ℤ
    bigInt a_windows[d], b_windows[d];
    for (size_t i = 0; i < d; ++i) {
        size_t a_offset = i * mlimbs; size_t a_len = (a_offset < a->n) ? min(mlimbs, a->n - a_offset) : 0;
        size_t b_offset = i * mlimbs; size_t b_len = (b_offset < b->n) ? min(mlimbs, b->n - b_offset) : 0;
        a_windows[i] = (bigInt){ .limbs = a->limbs + a_offset, .n = a_len, .cap = mlimbs, .sign = 1 };
        b_windows[i] = (bigInt){ .limbs = b->limbs + b_offset, .n = b_len, .cap = mlimbs, .sign = 1 };
    }

}
