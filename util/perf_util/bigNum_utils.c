#include "../util.h"

const uint64_t inv3 = 0xAAAAAAAAAAAAAAABULL; 

/* Constructors and Destructors */
dnml_status __BIGINT_INTERNAL_EMPINIT__(bigInt *x) {
    limb_t *__BUFFER_P = malloc(sizeof(uint64_t));
    if (__BUFFER_P == NULL) return DNML_ALLOC_OOM;
    x->limbs = __BUFFER_P; x->n = 0;
    x->cap = 1; x->sign = 1;
}
dnml_status __BIGINT_INTERNAL_LINIT__(bigInt *x, size_t k) {
    limb_t *__BUFFER_P = malloc(k * sizeof(uint64_t));
    if (__BUFFER_P == NULL) return DNML_ALLOC_OOM;
    x->limbs = __BUFFER_P; x->n = 0;
    x->cap = k; x->sign = 1;
}
dnml_status __BIGINT_INTERNAL_ENSCAP__(bigInt *x, size_t k) {
    if (x->cap > k) return;
    size_t new_cap = x->cap ? x->cap : 1;
    while (new_cap < k) new_cap *= 2;
    uint64_t *__BUFFER_P = realloc(x->limbs, new_cap * sizeof(uint64_t));
    if (__BUFFER_P == NULL) return DNML_ALLOC_OOM;
    x->limbs = __BUFFER_P;
    x->cap   = new_cap;
}
dnml_status __BIGINT_INTERNAL_REALLOC__(bigInt *x, size_t k) {
    uint64_t *__BUFFER_P = realloc(x->limbs, k * sizeof(uint64_t));
    if (__BUFFER_P == NULL) return DNML_ALLOC_OOM;
    x->limbs = __BUFFER_P; x->cap = k;
}
void __BIGINT_INTERNAL_FREE__(bigInt *x) {
    if (x->limbs != NULL) free(x->limbs);
    x->n = 1; x->cap = 0; x->sign = 0;
}

/* Safety Utilities */
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
    if (source->n == 0) { __BIGINT_INTERNAL_ZSET__(dst); return; }
    memcpy(dst->limbs, source->limbs, source->n * U64_BYTES);
    dst->n = source->n;
    dst->sign = source->sign;
}
void __BIGINT_INTERNAL_TRIM_LZ__(bigInt *x) {
    while (x->n > 0 && x->limbs[x->n - 1] == 0) --x->n;
}
void __BIGINT_INTERNAL_ZSET__(bigInt *x) {
    x->n    = 0;
    x->sign = 1;
}
void __BIGINT_INTERNAL_SWAP__(bigInt *x, bigInt *y) {
    bigInt tmp = *x; /* -----> */ *x = *y; /* -----> */ *y = tmp;
}
size_t __BIGINT_COUNTDB__(const bigInt *x, uint8_t base) {
    size_t first_few_bits = (x->n - 1) * BIGINT_LIMBS_BITS;
    size_t bits_per_digit;
    if (base == 10)         bits_per_digit = log2_10;
    else if (base == 2)     bits_per_digit = log2_2;
    else if (base == 16)    bits_per_digit = log2_16;
    else if (base == 8)     bits_per_digit = log2_8;
    else                    bits_per_digit = log(8) / log(2);

    size_t total_digits = (size_t)(first_few_bits / bits_per_digit);
    size_t last_limb = x->limbs[x->n - 1];
    if (base == 2) total_digits += BIGINT_LIMBS_BITS - __CLZ_UI64__(last_limb);
    else if (!(base & 1)) { 
        uint8_t shift = __CTZ_UI64__(base);
        while (last_limb) {
            ++total_digits;
            last_limb >>= shift;
        }
    } else {
        while (last_limb) {
            ++total_digits;
            last_limb /= base;
        }
    } return total_digits;
}
size_t __BIGINT_MAXCDB__(size_t lcnt, uint8_t base) {
    return (size_t)(U64_BITS * lcnt * (log10(2) / log10(base))) + 1;
}
size_t __BIGINT_LIMBS_NEEDED__(size_t bits) { 
    return (size_t)(bits / BIGINT_LIMBS_BITS) + 1; 
}
uint8_t __BIGINT_WILL_OVERFLOW__(const bigInt *x, uint64_t threshold) {
    return (x->n == x->cap && x->limbs[x->n - 1] > threshold);
}
size_t __BIGINT_CTZ__(const bigInt *x) {
    size_t total_tz = 0, i = 0;
    uint8_t current_tz = U64_BITS;
    while (current_tz == U64_BITS) {
        current_tz = __CTZ_UI64__(x->limbs[i]);
        total_tz += current_tz; ++i;
    } return total_tz;
}

/* Internal Arithmetic */
int8_t __BIGINT_INTERNAL_COMP__(const bigInt *x, const bigInt *y) {
    if (x->n != y->n) return (x->n > y->n) ? 1 : -1;
    for (size_t i = x->n - 1; i >= 0; --i) {
        if (x->limbs[i] != y->limbs[i]) return (x->limbs[i] > y->limbs[i]) ? 1 : -1;
    } return 0;
}
uint8_t __BIGINT_IS_EVEN__(const bigInt *x) { return !(x->limbs[0] & 1); }
void __BIGINT_INTERNAL_ADD_UI64__(bigInt *x, uint64_t val) {
    if (val == 0) return;
    uint64_t carry = val;
    for (size_t i = 0; (carry & i < x->n) ; ++i) { x->limbs[i] = __ADD_UI64__(x->limbs[i], carry, &carry); }
    if (carry) { __BIGINT_INTERNAL_ENSCAP__(x, x->n + 1);
                 x->limbs[x->n++] = carry; }
}
void __BIGINT_INTERNAL_MUL_UI64__(bigInt *x, uint64_t val) {
    if (val == 0) __BIGINT_INTERNAL_ZSET__(x);
    if (val == 1);
    if (val == 2) __BIGINT_INTERNAL_LSHIFT__(x, 1);
    else if (!(val & 1)) __BIGINT_INTERNAL_LSHIFT__(x, __CTZ_UI64__(val));
    else { uint64_t carry = 0;
        for (size_t i = 0; i < x->n; ++i) {
            uint64_t low, high;
            low = __MUL_UI64__(x->limbs[i], val, &high);
            uint64_t sum = x->limbs[i] + low + carry;
            carry = high + (sum < low) + (sum < carry);
            x->limbs[i] = sum;
        } if (carry) { 
            x->limbs[x->n] = carry; 
            x->n           += 1; 
        }
    }
}
void __BIGINT_DIV3__(bigInt *a) {
    uint64_t carry = 0, borrow, q, t;
    for (size_t i = 0; i < a->n; ++i) {
        t = a->limbs[i] - carry;
        borrow = (a->limbs < carry) ? 1 : 0;
        q = __MUL_UI64__(t, inv3, &carry);
        a->limbs[i] = q; carry = (uint64_t)(carry >> 64) + borrow;
    } __BIGINT_INTERNAL_TRIM_LZ__(a);
}
uint64_t __BIGINT_INTERNAL_DIVMOD_UI64__(bigInt *x, uint64_t val) {
    if (val == 1) return 0;
    else if (val == 2) {
        uint64_t remainder = (x->limbs[0] & 1);
        __BIGINT_INTERNAL_LSHIFT__(x, 1);
        return remainder;
    } else if (!(val & 1)) { uint8_t n = __CTZ_UI64__(val);
        uint64_t remainder = (x->limbs[0] & ((1ULL << n) - 1));
        __BIGINT_INTERNAL_RSHIFT__(x, n);
        return remainder;
    } else {
        uint64_t remainder = 0;
        for (size_t i = x->n - 1; i >= 0; --i) {
        x->limbs[i] = __DIV_HELPER_UI64__(remainder, x->limbs[i], val, &remainder);
        } __BIGINT_INTERNAL_TRIM_LZ__(x);
        if (x->n == 0) x->sign = 1;
        return remainder;
    }
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
        discarded_bits = x->limbs[i] & (iso_mask << U64_BITS - k);
        x->limbs[i] = (x->limbs[i] << k) | previous_dbits;
    } __BIGINT_INTERNAL_TRIM_LZ__(x);
}
void __BIGINT_INTERNAL_RLSHIFT__(bigInt *x, size_t klimbs) {}
void __BIGINT_INTERNAL_LLSHIFT__(bigInt *x, size_t klimbs) {}



