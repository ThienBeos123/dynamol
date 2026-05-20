#include "../crt_util.h"


void __BICRT_TRIM_LZ__(bigInt *x) {
    uint8_t found_msl = 0;
    for (size_t i = x->cap - 1; i >= 0; --i) {
        uint8_t behind_n = (i < x->n);
        uint8_t zero = !(x->limbs[i]);
        // Checks if i < n AND i == 0 AND msl has not been set
        x->n -= ((behind_n & zero) & (~found_msl));
        // Checks if i < n AND i != 0 AND msl has not been set
        found_msl |= (behind_n & (~zero));
    }
}