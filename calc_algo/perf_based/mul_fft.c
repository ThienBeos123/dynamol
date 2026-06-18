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
/* Sizing Function */
size_t __BIGINT_FFT_WS__(size_t a_size, size_t b_size) { return 0; }

/* Helper function */
typedef struct { size_t k; size_t D; size_t M; size_t n; } fft_table;
static fft_table ___bigint_fft_best_metadata(size_t a_size, size_t b_size) {
    size_t max_bits = max(a_size * U64_BITS, b_size * U64_BITS);
    fft_table ret = {2, 0, 0, 0};
    for (;;) {
        ret.D = (size_t)1 << ret.k; // D = 2^k
        ret.M = (max_bits + ret.D - 1) / ret.D; // ceil(max_bits / D)
        ret.n = ret.D * ret.M; // 2^n = 2^DM --> n = DM
        if (ret.k + (ret.M << 1) + 2 <= ret.n) break; // k + 2M + slack <= n?
        ++ret.k; // Increasing k until satisfies condition
    } return ret;
}
/** Fast Fourier Transform
* These 2 function is within only the scope of mul_fft.c as a helper function.
* It's purpose is to implement the Fast Fourier Transform, or, better suited for
* multiplication purposes, the Number Theoretic Transform, through the Cooley-Tukey Algorithm.
*/
static void ___bigint_cooley_tukey(limb_t *data, size_t omega_shift, size_t D, size_t n) {}
static void ___bigint_ifft(limb_t *data, size_t omega_shift, size_t D, size_t n) {}
/* Main Orchestrating Function */
void __BIGINT_FFT__(const bigInt *a, const bigInt *b, bigInt *res, calc_ctx fft_ctx) {
    if (a->n <= BIGINT_SCHOOLBOOK && b->n <= BIGINT_SCHOOLBOOK) {
        __BIGINT_SCHOOLBOOK__(a, b, res); return; // Base-case
    } //* -------- 1. SETUP & SPLIT -------- *//
    fft_table ret = ___bigint_fft_best_metadata(a->n, b->n); // Splitting and Convolution Variables
    size_t k = ret.k; size_t D = ret.D; size_t M = ret.M; size_t n = ret.n;
    
}
