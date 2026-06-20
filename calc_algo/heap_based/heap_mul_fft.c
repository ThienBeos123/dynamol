#include "heap_mul.h"



/* ========== HELPER FUNCTIONS ========== */
static size_t ___biheap_fft_best_metadata(size_t a_size, size_t b_size, size_t *outd, size_t *outm, size_t *outn) {
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
static void ___biheap_cooley_tukey(limb_t *data, size_t omega_shift, size_t d, size_t n) {}
static void ___biheap_ifft(limb_t *data, size_t omega_shift, size_t d, size_t n) {}
/* ============ MAIN FUNCTIONS ============ */
void __BIHEAP_FFT__(const bigInt *a, const bigInt *b, bigInt *res, dnml_status *err) {
    if (a->n <= BIGINT_SCHOOLBOOK && b->n <= BIGINT_SCHOOLBOOK) {
        __BIHEAP_SCHOOLBOOK__(a, b, res); return; // Base-case
    } //* -------- 1. SETUP & SPLIT -------- *//
    /* Splitting and Convolution Variables */ dnml_status err_check;
    size_t d = 0, m = 0, n = 0, k = ___biheap_fft_best_metadata(a->n, b->n, &d, &m, &n);
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