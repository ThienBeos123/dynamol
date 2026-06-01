#include "../crt_util.h"

/* ---------- State Altering Utilities ---------- */
dnml_status __CRINT_TRIM_LZ__(crint *x) {
    crint_poison(x); uint8_t found_msl = 0;
    for (size_t i = x->cap - 1; i != -1; --i) {
        uint8_t behind_n = (i < x->n);
        uint8_t zero = !(x->limbs[i]);
        // Checks if i < n AND i == 0 AND msl has not been set
        x->n -= ((behind_n & zero) & (~found_msl));
        // Checks if i < n AND i != 0 AND msl has not been set
        found_msl |= (behind_n & (~zero));
    } return CRINT_SUCCESS;
}
int8_t __CRINT_INTERNAL_CMP__(crint *x, crint *y) {
    /* Basically a more bareboned version of __CRINT_MAGCMP__ */
    crint_poison(x); int8_t ret = 2, curr = 0;
    // Check 1: if (x->n > y->n) return 1;
    CHOOSE_OPTION((curr), (_lib_crt_gt(x->n, y->n)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (x->n < y->n) return -1;
    CHOOSE_OPTION((curr), (_lib_crt_lt(x->n, y->n)), (-1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 3: Sequential checks
    size_t upperbound = crtmax(x->cap, y->cap);
    uint64_t x_curr, y_curr;
    for (size_t i = upperbound - 1; _lib_crt_neq(i, -1); --i) {
        /* 
        * Guarantees that each iteration always access an index to normalize timing
        * We universally chosen the first limb of each crint as a placeholder
        * with not semantic meaning 
        */
        CHOOSE_OPTION((x_curr), (_lib_crt_lt(i, x->cap)), (x->limbs[i]), (x->limbs[0]));
        CHOOSE_OPTION((x_curr), (_lib_crt_lt(i, x->cap)), (y->limbs[i]), (y->limbs[0]));
        // Actually Getting the right value
        CHOOSE_OPTION((x_curr), (_lib_crt_lt(i, x->n)), (x_curr), (0));
        CHOOSE_OPTION((y_curr), (_lib_crt_lt(i, y->n)), (y_curr), (0));
        // Comparing values
        /* 
        * If our x_curr == x->limbs[i], in which i > y->n,
        * then such case was already covered above in our pre-checks
        * due to the fact that _lib_crt_gt(x->n, y->n). This also works inversely for y
        */
       CHOOSE_OPTION((curr), (_lib_crt_gt(x_curr, y_curr)), (1), (0));
       CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
       CHOOSE_OPTION((curr), (_lib_crt_lt(x_curr, y_curr)), (-1), (0));
       CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    } 
    // ALL CHECKS WEREN'T SATIFIES --> EQUAL
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (0), (ret));
    /* Aggresive Memory Sanitization */
    curr = 0; upperbound = 0; x_curr = 0; y_curr = 0;
    return ret;
}
crint __CRINT_ERRVAL__(void) { return (crint){ .limbs = NULL, .n = 1, .cap = 0, .sign = 0, .poisoned = true }; }



/* ----- Mathematical Tools ----- */
dnml_status __CRINT_INTERNAL_RLSHIFT__(crint *x, size_t len, size_t limb_cnt) {}
dnml_status __CRINT_INTERNAL_LLSHIFT__(crint *x, size_t len, size_t limb_cnt) {}