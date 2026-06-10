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



#include "../util.h"

const uint64_t inv3 = 0xAAAAAAAAAAAAAAABULL;

/* Safety & State Utilities */
void __BIGINT_INTERNAL_FREE__(bigInt *x) {
    if (x->limbs != NULL) free(x->limbs);
    x->n = 1; x->cap = 0; x->sign = 0;
}
uint8_t __BIGINT_INTERNAL_VALID__(const bigInt *x) { /* BigInt Validity */
    if (x == NULL) return 0;
    /* State Validation */
    if (x->limbs == NULL) return 0;
    if (x->cap < 1) return 0;
    if (x->n > x->cap) return 0;
    if (x->sign != 1 && x->sign != -1) return 0;
    /* Arithmetic Validation */
    if (x->n == 0) return (x->sign == 1);
    if (x->limbs[x->n - 1] == 0) return 0;
    return 1;
}
uint8_t __BIGINT_INTERNAL_PVALID__(const bigInt *x) { /* BigInt Pointer State Validity */
    if (x == NULL) return 0;
    if (x->limbs == NULL) return 0;
    if (x->cap < 1) return 0;
    if (x->n > x->cap) return 0;
    if (x->sign != 1 && x->sign != -1) return 0;
    return 1;
}
uint8_t __BIGINT_INTERNAL_SVALID__(const bigInt *x) { /* BigInt Storage Validity */
    if (x == NULL) return 0;
    if (x->limbs == NULL) return 0;
    if (x->cap < 1) return 0;
    return 1;
}
bigInt __BIGINT_ERROR_VALUE__(void) {
    return (bigInt){ .limbs = NULL, .cap = 0,   /**/   .n = 1, .sign = 0 };
}

/* General Utilities */
void __BIGINT_INTERNAL_COPY__(bigInt *dst, const bigInt *source) {
    if (source->limbs == NULL || !source->n) { __BIGINT_INTERNAL_ZSET__(dst); return; }
    memcpy(dst->limbs, source->limbs, source->n * U64_BYTES);
    dst->n = source->n; /**/ dst->sign = source->sign;
}
void __BIGINT_INTERNAL_TRIM_LZ__(bigInt *x) { while (x->n && x->limbs[x->n - 1] == 0) --(x->n); }
void __BIGINT_INTERNAL_ZSET__(bigInt *x) { x->n = 0; x->sign = 1; }
void __BIGINT_INTERNAL_SWAP__(bigInt *x, bigInt *y) { bigInt tmp = *x; *x = *y; *y = tmp; }
size_t __BIGINT_COUNTDB__(const bigInt *x, uint8_t base) {
    if (!x->n || (x->n == 1 && x->limbs[0] == 0)) return 0; 
    size_t leading_bits = BIGINT_LIMBS_BITS - __CLZ_UI64__(x->limbs[x->n - 1]);
    size_t total_bits = (x->n - 1) * BIGINT_LIMBS_BITS + leading_bits;
    // 2. Handle pure powers of 2 exactly using integer math
    // (base & (base - 1)) == 0 checks if 'base' is a power of 2
    if (base > 1 && (base & (base - 1)) == 0) {
        uint8_t bits_per_digit = __CTZ_UI64__(base); // equivalent to log2(base)
        return (total_bits + bits_per_digit - 1) / bits_per_digit;
    }
    // 3. Handle arbitrary bases (like 10) using high-precision log
    // Approximate using the top limb + the second top limb for 128-bit precision approximation
    long double top_val = x->limbs[x->n - 1];
    if (x->n > 1) { // Normalize the second-highest limb into the fractional part
        top_val += (long double)x->limbs[x->n - 2] / (long double)(UINT64_MAX) + 1; 
    }
    long double total_log2 = log2l(top_val) + (long double)(x->n - 1) * BIGINT_LIMBS_BITS;
    long double bits_per_digit = log2l(base);
    return (size_t)(total_log2 / bits_per_digit) + 1;
}
size_t __BIGINT_MAXCDB__(size_t lcnt, uint8_t base) { return (size_t)(U64_BITS * lcnt * (log10(2) / log10(base))) + 1; }
size_t __BIGINT_LIMBS_NEEDED__(size_t bits) { 
    if (!bits) return 0;
    return (size_t)((bits + BIGINT_LIMBS_BITS - 1) / BIGINT_LIMBS_BITS);
}
uint8_t __BIGINT_WILL_OVERFLOW__(const bigInt *x, uint64_t threshold) {
    if (x->n == 0) return 0; // Would never underflow if x == 0;
    return (x->n == x->cap && x->limbs[x->n - 1] > threshold);
}
size_t __BIGINT_CTZ__(const bigInt *x) {
    if (!x->n) return 0;
    size_t total_tz = 0;
    for (size_t i = 0; i < x->n; ++i) {
        if (x->limbs[i] != 0) return total_tz + __CTZ_UI64__(x->limbs[i]);
        total_tz += U64_BITS; 
    } return total_tz;
}

/* Internal Arithmetic */
int8_t __BIGINT_INTERNAL_COMP__(const bigInt *x, const bigInt *y) {
    if (x->n != y->n) return (x->n > y->n) ? 1 : -1;
    for (size_t i = x->n - 1; i != (size_t)-1; --i) {
        if (x->limbs[i] != y->limbs[i]) return (x->limbs[i] > y->limbs[i]) ? 1 : -1;
    } return 0;
}
uint8_t __BIGINT_IS_EVEN__(const bigInt *x) { return !(x->limbs[0] & 1); }
void __BIGINT_INTERNAL_ADD_UI64__(bigInt *x, uint64_t val) {
    if (!val) return;
    DNML_TEST_ASSERT(x->cap >= x->n + 1, 
        "Internal Additio Error: Utility failed due to "
        "insufficient sum capacity", {}
    );
    uint64_t carry = val;
    for (size_t i = 0; (carry && i < x->n) ; ++i) {
        uint8_t u8_carry = (uint8_t)carry;
        x->limbs[i] = __ADD_UI64__(x->limbs[i], carry, &u8_carry);
    } if (carry) x->limbs[x->n++] = carry;
}
void __BIGINT_INTERNAL_MUL_UI64__(bigInt *x, uint64_t val) {
    if (val == 0) __BIGINT_INTERNAL_ZSET__(x);
    if (val == 2) __BIGINT_INTERNAL_LSHIFT__(x, 1);
    else if (!(val & 1)) __BIGINT_INTERNAL_LSHIFT__(x, __CTZ_UI64__(val));
    else if (val != 1) {
        uint64_t carry = 0;
        for (size_t i = 0; i < x->n; ++i) {
            uint64_t high, low = __MUL_UI64__(x->limbs[i], val, &high);
            uint64_t sum = x->limbs[i] + low + carry;
            carry = high + (sum < low) + (sum < carry);
            x->limbs[i] = sum;
        } if (carry) { x->limbs[x->n] = carry; x->n += 1; }
    }
}
void __BIGINT_DIV3__(bigInt *a) {
    uint64_t carry = 0, borrow, q, t;
    for (size_t i = 0; i < a->n; ++i) {
        t = a->limbs[i] - carry;
        borrow = (a->limbs[i] < carry) ? 1 : 0;
        q = __MUL_UI64__(t, inv3, &carry);
        a->limbs[i] = q; carry += borrow;
    } __BIGINT_INTERNAL_TRIM_LZ__(a);
}
uint64_t __BIGINT_INTERNAL_DIVMOD_UI64__(bigInt *x, uint64_t val) {
    uint64_t remainder;
    if (val == 1) remainder = 0;
    else if (val == 2) { remainder = (x->limbs[0] & 1); __BIGINT_INTERNAL_LSHIFT__(x, 1); }
    else if (!(val & 1)) { 
        uint8_t n = __CTZ_UI64__(val);
        remainder = (x->limbs[0] & ((1ULL << n) - 1));
        __BIGINT_INTERNAL_RSHIFT__(x, n);
    } else {
        remainder = 0; uint8_t ovf_check;
        for (size_t i = x->n - 1; i != (size_t)-1; --i) {
            x->limbs[i] = __DIV_HELPER_UI64__(remainder, x->limbs[i], val, &remainder, &ovf_check);
            DNML_TEST_ASSERT(ovf_check, "CRITICIAL DEBUG ERROR: Division quotient's overflowed", {});
        }
        __BIGINT_INTERNAL_TRIM_LZ__(x);
        if (!x->n) x->sign = 1;
    } return remainder;
}
void __BIGINT_INTERNAL_RSHIFT__(bigInt *x, size_t k) {
    if (!k) return;
    uint64_t discarded_bits = 0;
    for (size_t i = 0; i < x->n; ++i) {
        uint64_t positioned_bits = discarded_bits << (U64_BITS - k);
        discarded_bits = x->limbs[i] & ((1U << k) - 1);
        x->limbs[i] = (x->limbs[i] >> k) | positioned_bits;
    } __BIGINT_INTERNAL_TRIM_LZ__(x);
}
void __BIGINT_INTERNAL_LSHIFT__(bigInt *x, size_t k) {
    if (!k) return;
    uint64_t discarded_bits = 0;
    for (size_t i = 0; i < x->n; ++i) {
        uint64_t previous_dbits = discarded_bits;
        uint64_t iso_mask = (1U << k) - 1;
        discarded_bits = x->limbs[i] & (iso_mask << (U64_BITS - k));
        x->limbs[i] = (x->limbs[i] << k) | previous_dbits;
    } __BIGINT_INTERNAL_TRIM_LZ__(x);
}
void __BIGINT_INTERNAL_RLSHIFT__(bigInt *x, size_t klimbs) {}
void __BIGINT_INTERNAL_LLSHIFT__(bigInt *x, size_t klimbs) {}
