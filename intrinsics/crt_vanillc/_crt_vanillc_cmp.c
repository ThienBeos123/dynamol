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



#include "__crt_vanillc_con__.h"


/* U64 crt_choice_util */
uint8_t _vanillc_crt_lt(uint64_t x, uint64_t y) {
    uint64_t x_msb = (x >> (U64_BITS - 1));
    uint64_t y_msb = (y >> (U64_BITS - 1)); // clang-format off
    uint8_t res = (uint8_t)(((x_msb ^ 1) & y_msb) | (~(x_msb ^ y_msb) & ((x - y) >> (U64_BITS - 1))));
    x = 0; y = 0; x_msb = 0; y_msb = 0; return res; // clang-format on
}
uint8_t _vanillc_crt_gt(uint64_t x, uint64_t y) { return _vanillc_crt_lt(y, x); }
uint8_t _vanillc_crt_leq(uint64_t x, uint64_t y) { return _vanillc_crt_lt(x, y) ^ 1; }
uint8_t _vanillc_crt_geq(uint64_t x, uint64_t y) { return _vanillc_crt_gt(x, y) ^ 1; }

/* I64 crt_choice_util */
uint8_t _vanillc_crt_lti64(int64_t x, int64_t y) {
    int64_t sx = (x >> (U64_BITS - 1)) & 1;
    int64_t sy = (y >> (U64_BITS - 1)) & 1;
    int64_t sd = ((x - y) >> (U64_BITS - 1)) & 1;
    uint8_t res = (uint8_t)((sx & !sy) | (!(sx ^ sy) & sd)); // clang-format off
    sx = 0; sy = 0; sd = 0; x = 0; y = 0; return res; // clang-format on
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