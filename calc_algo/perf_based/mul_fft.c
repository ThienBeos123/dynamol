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

/* Helper function */
static size_t ___bigint_fft_best_metadata(size_t a_size, size_t b_size, size_t *outd, size_t *outm, size_t *outn) {
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
/** Fast Fourier Transform
* These 2 function is within only the scope of mul_fft.c as a helper function.
* It's purpose is to implement the Fast Fourier Transform, or, better suited for
* multiplication purposes, the Number Theoretic Transform, through the Cooley-Tukey Algorithm.
*/
static void ___bigint_cooley_tukey(limb_t *const data, size_t omega_shift, size_t d, size_t n) {}
static void ___bigint_ifft(limb_t *const data, size_t omega_shift, size_t d, size_t n) {}
/* Main Orchestrating Function */
void __BIGINT_FFT__(PCONST_BIGINT a, PCONST_BIGINT b, P_BIGINT res, calc_ctx fft_ctx, dnml_status *err) {
    if (a->n <= BIGINT_SCHOOLBOOK && b->n <= BIGINT_SCHOOLBOOK) {
        __BIGINT_SCHOOLBOOK__(a, b, res); return; // Base-case
    } //* -------- 1. SETUP & SPLIT -------- *//
    // Splitting and Convolution Variables
    size_t fft_mark = scratch_mark(&fft_ctx); dnml_status err_check;
    size_t d = 0, m = 0, n = 0, k = ___bigint_fft_best_metadata(a->n, b->n, &d, &m, &n);
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
