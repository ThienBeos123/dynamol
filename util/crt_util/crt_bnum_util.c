#include "../crt_util.h"


dnml_status __CRINT_TRIM_LZ__(crint *x) {
    cryptInt_poison(x);
    uint8_t found_msl = 0;
    for (size_t i = x->cap - 1; i >= 0; --i) {
        uint8_t behind_n = (i < x->n);
        uint8_t zero = !(x->limbs[i]);
        // Checks if i < n AND i == 0 AND msl has not been set
        x->n -= ((behind_n & zero) & (~found_msl));
        // Checks if i < n AND i != 0 AND msl has not been set
        found_msl |= (behind_n & (~zero));
    } return CRINT_SUCCESS;
}

dnml_status __CRINT_INTERNAL_RLSHIFT__(crint *x, size_t len, size_t limb_cnt) {}
dnml_status __CRINT_INTERNAL_LLSHIFT__(crint *x, size_t len, size_t limb_cnt) {}

crint __CRINT_ERRVAL__(void) {
    return (crint){ .limbs = NULL, .n = 1, .cap = 0, .sign = 0, .poisoned = true };
}