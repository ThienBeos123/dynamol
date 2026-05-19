#include "add_sub.h"



void __BIGINT_ADD_WC__(bigInt *res, const bigInt *a, const bigInt *b) {
    size_t max = max(a->n, b->n);
    uint64_t carry = 0;
    for (size_t i = 0; i < max; ++i) {
        uint64_t x = (i < a->n) ? a->limbs[i] : 0; // Assigning limb at position i of a to x
        uint64_t y = (i < b->n) ? b->limbs[i] : 0; // Assigning limb at position i of b to x
        res->limbs[i] = __ADD_UI64__(x, y, &carry); // Do single-limb addition with carry (if have) --> Stores the carry
    }
    if (carry) res->limbs[max] = carry; // If carry still needed ---> stores the carry in the (res->cap - 1) limb
    res->n = max + (carry != 0);
}
void __BIGINT_ADD_SAW__(bigInt *res, const bigInt *x, const bigInt *y) {
    if (!y->n);
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


void __BIGINT_SUB_WB__(bigInt *res, const bigInt *a, const bigInt *b) {
    uint64_t borrow = 0;
    for (size_t i = 0; i < a->n; ++i) {
        uint64_t y = (i < b->n) ? b->limbs[i] : 0;
        res->limbs[i] = __SUB_UI64__(a->limbs[i], y, &borrow);
        // Do single-limb subtraction with borrow ---> Stores the borrow
    } res->n = a->n;
}
void __BIGINT_SUB_SAW__(bigInt *res, const bigInt *x, const bigInt *y) {
    if (!y->n);
    else if (!x->n) { __BIGINT_INTERNAL_COPY__(res, y);  res->sign = -y->sign; }
    else if (x->sign == y->sign) {
        int8_t comp_res = __BIGINT_INTERNAL_COMP__(x, y);
        if (!comp_res) __BIGINT_INTERNAL_ZSET__(x);
        else {
            if (comp_res > 0) { __BIGINT_SUB_WB__(res, x, y); res->sign = x->sign; }
            else              { __BIGINT_SUB_WB__(res, x, y); res->sign = -x->sign; }
        }
    } else {
        __BIGINT_ADD_WC__(res, x, y);
        res->sign = x->sign;
    }
}
