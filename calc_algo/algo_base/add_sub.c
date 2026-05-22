#include "add_sub.h"


//* =============== ADDITION ARITHMETIC ENGINE =============== *//
void __BIGINT_ADD_WC__(bigInt *res, const bigInt *a, const bigInt *b) {
    DNML_TEST_ASSERT(
        (res->cap >= max(a->n, b->n) + 1),
        "Insufficient Sum Buffer: Capacity Unsatisfactory for a + b"
        " (-Eadd_insufficient_cap)"
    ); size_t max = max(a->n, b->n); uint64_t carry = 0;
    for (size_t i = 0; i < max; ++i) {
        uint64_t x = (i < a->n) ? a->limbs[i] : 0; // Assigning limb at position i of a to x
        uint64_t y = (i < b->n) ? b->limbs[i] : 0; // Assigning limb at position i of b to x
        res->limbs[i] = __ADD_UI64__(x, y, &carry); // Do single-limb addition with carry (if have)
        // Stores the carry
    }
    if (carry) res->limbs[max] = carry; // If there is still a carry
    res->n = max + (carry != 0);
}
void __BIGINT_ADD_SAW__(bigInt *res, const bigInt *x, const bigInt *y) {
    // Static analysis
    if ((x->sign == 1 && y->sign == 1) ||
        (x->sign == -1 && y->sign == -1)
    ) DNML_TEST_ASSERT(
        (res->cap >= max(x->n, y->n) + 1),
        (x->sign == 1 && y->sign == 1) ?
        "Insufficient Sum Buffer:"
        " Capacity Unsatisfactory for a + b"
        " (-Eadd_saw_insufficient_cap)" :
        "Insufficient Sum Buffer:"
        " Capacity Unsatisfactory for (-a) + (-b)"
        " (-Eadd_saw_insufficient_cap)"
    );
    // Main operation
    if (!y->n) return;
    else if (!x->n) __BIGINT_INTERNAL_COPY__(res, y);
    else if (x->sign == y->sign) {
        __BIGINT_ADD_WC__(res, x, y);
        res->sign = x->sign;
    } else {
        int8_t comp_res = __BIGINT_INTERNAL_COMP__(x, y);
        if (!comp_res) __BIGINT_INTERNAL_ZSET__(res);
        else {
            if (comp_res > 0) { __BIGINT_SUB_WB__(res, x, y); res->sign = x->sign; }
            else { __BIGINT_SUB_WB__(res, x, y); res->sign = y->sign; }
        }
    }
}

//* =============== SUBTRACTION ARITHMETIC ENGINE =============== *//
void __BIGINT_SUB_WB__(bigInt *res, const bigInt *a, const bigInt *b) {
    DNML_TEST_ASSERT(
        (__BIGINT_INTERNAL_COMP__(a, b) != -1),
        "Subtraction Underflow: Subtrahend's magnitude is too large for Minuend"
        " (-Esub_underflow)"
    ); uint64_t borrow = 0;
    for (size_t i = 0; i < a->n; ++i) {
        uint64_t y = (i < b->n) ? b->limbs[i] : 0;
        res->limbs[i] = __SUB_UI64__(a->limbs[i], y, &borrow);
        // Do single-limb subtraction with borrow ---> Stores the borrow
    } res->n = a->n;
}
void __BIGINT_SUB_SAW__(bigInt *res, const bigInt *x, const bigInt *y) {
    // Static Analysis
    if ((x->sign == 1 && y->sign == -1) ||
        (x->sign == -1 && y->sign == 1)
    ) DNML_TEST_ASSERT(
        (res->cap >= max(x->n, y->n) + 1),
        (x->sign == 1 && y->sign == -1) ?
        "Insufficient Difference Buffer:"
        " Capacity Unsatisfactory for a - (-b)"
        " (-Esub_saw_insufficient_cap)" :
        "Insufficient Difference Buffer:"
        " Capacity Unsatisfactory for (-a) - b"
        " (-Esub_saw_insufficient_cap)"
    );
    // Main Operation
    if (!y->n) return;
    else if (!x->n) { __BIGINT_INTERNAL_COPY__(res, y);  res->sign = -y->sign; }
    else if (x->sign == y->sign) {
        int8_t comp_res = __BIGINT_INTERNAL_COMP__(x, y);
        if (!comp_res) __BIGINT_INTERNAL_ZSET__(x);
        else {
            if (comp_res > 0) { __BIGINT_SUB_WB__(res, x, y); res->sign = x->sign; }
            else { __BIGINT_SUB_WB__(res, x, y); res->sign = -x->sign; }
        }
    } else {
        __BIGINT_ADD_WC__(res, x, y);
        res->sign = x->sign;
    }
}


//* =============== ADDITION + SUBTRACTION CONSTANT ENGINE =============== *//
drypto_stat __CRINT_ADD_WC__(cryptint *res, const cryptint *a, const cryptint *b) {
    // Static Analysis
    crint_poison(a); crint_poison(b); 
    crint_poison(res); DNML_TEST_ASSERT(
        (res->cap >= max(a->n, b->n) + 1),
        "Insufficient Sum Buffer: Capacity Unsatisfactory for a + b"
        " (-Eadd_insufficient_cap)"
    ); // Main Algorithms
    if (res->poisoned || a->poisoned || b->poisoned) return CRYPTINT_POISOINED;
    size_t max = max(a->n, b->n); uint64_t carry = 0;
    for (size_t i = 0; i < max; ++i) {
        uint64_t a_curr = a->limbs[i], b_curr = b->limbs[i], x, y;
        // uint64_t x = (i < a->n) ? a->limbs[i] : 0;
        // uint64_t y = (i < b->n) ? b->limbs[i] : 0;
        CHOOSE_OPTION(x, (i < a->n), a_curr, 0);
        CHOOSE_OPTION(y, (i < b->n), b_curr, 0);
        res->limbs[i] = __ADD_UI64__(x, y, &carry);
    } res->limbs[max] = carry; res->n = max + (!!carry);
    __libdnml_memset_strict(res->limbs, 0, res->cap, res->n, res->cap - 1);
    return CRYPTINT_SUCCESS;
}
drypto_stat __CRINT_SUB_WC__(cryptint *res, const cryptint *a, const cryptint *b) {
    crint_poison(a); crint_poison(b); 
    crint_poison(res); DNML_TEST_ASSERT(
        (__BIGINT_INTERNAL_COMP__(a, b) != -1),
        "Subtraction Underflow: Subtrahend's magnitude is too large for Minuend"
        " (-Esub_underflow)"
    ); // Main Algorithms
    if (res->poisoned || a->poisoned || b->poisoned) 
    return CRYPTINT_POISOINED;  uint64_t borrow = 0;
    for (size_t i = 0; i < a->n; ++i) {
        uint64_t curr = b->limbs[i], y;
        // uint64_t y = (i < b->n) ? b->limbs[i] : 0;
        CHOOSE_OPTION(y, (i < b->n), curr, 0);
        res->limbs[i] = __SUB_UI64__(a->limbs[i], y, &borrow);
    } res->n = a->n; __BICRT_TRIM_LZ__(res);
    __libdnml_memset_strict(res->limbs, 0, res->cap, res->n, res->cap - 1);
    return CRYPTINT_SUCCESS;
}