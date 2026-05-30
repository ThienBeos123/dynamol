#include "__crt_vanillc_con__.h"


/* U64 crt_choice_util */
uint8_t _vanillc_crt_lt(uint64_t x, uint64_t y) {
    uint64_t x_msb = (x >> (U64_BITS - 1));
    uint64_t y_msb = (y >> (U64_BITS - 1));
    return ((x_msb ^ 1) & y_msb) | (~(x_msb ^ y_msb) & ((x - y) >> (U64_BITS - 1)));
}
uint8_t _vanillc_crt_gt(uint64_t x, uint64_t y) { return _vanillc_crt_lt(y, x); }
uint8_t _vanillc_crt_leq(uint64_t x, uint64_t y) { return _vanillc_crt_lt(x, y) ^ 1; }
uint8_t _vanillc_crt_geq(uint64_t x, uint64_t y) { return _vanillc_crt_gt(x, y) ^ 1; }

/* I64 crt_choice_util */
uint8_t _vanillc_crt_lti64(int64_t x, int64_t y) {
    int64_t sx = (x >> (U64_BITS - 1)) & 1;
    int64_t sy = (y >> (U64_BITS - 1)) & 1;
    int64_t sd = ((x - y) >> (U64_BITS - 1)) & 1;
    return (sx & !sy) | (!(sx ^ sy) & sd);
}
uint8_t _vanillc_crt_gti64(int64_t x, int64_t y) { return _vanillc_crt_lti64(y, x); }
uint8_t _vanillc_crt_leqi64(int64_t x, int64_t y) { return _vanillc_crt_lti64(x, y) ^ 1; }
uint8_t _vanillc_crt_geqi64(int64_t x, int64_t y) { return _vanillc_crt_gti64(x, y) ^ 1; }


/* Equality */
uint8_t _vanillc_crt_ispos(int64_t x) { return ~(x & SIGN_BIT_MASK); }
uint8_t _vanillc_crt_isneg(int64_t x) { return (x & SIGN_BIT_MASK); }
uint8_t _vanillc_crt_eq(uint64_t x, uint64_t y) { return ~((x ^ y) | (-(x ^ y))) >> (U64_BITS - 1); }
uint8_t _vanillc_crt_neq(uint64_t x, uint64_t y) { return ((x ^ y) | (-x ^ y)) >> (U64_BITS - 1); }
uint64_t _vanillc_crt_select(uint8_t cond, uint64_t a, uint64_t b) {
    uint64_t mask = -((int64_t)cond);
    return (a & mask) | (b & (~mask));
}