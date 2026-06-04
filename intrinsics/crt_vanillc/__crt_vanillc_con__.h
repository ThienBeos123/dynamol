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



#ifndef __libdnml_CRT_VANILLC_CONN_H__
#define __libdnml_CRT_VANILLC_CONN_H__



#include <stdint.h>
#include <_libdnml_config/numeric_config.h>
#include <dnml_sys/sys.h>


/* _crt_vanillc_cmp.c */
uint8_t _vanillc_crt_lt(uint64_t x, uint64_t y);
uint8_t _vanillc_crt_gt(uint64_t x, uint64_t y);
uint8_t _vanillc_crt_leq(uint64_t x, uint64_t y);
uint8_t _vanillc_crt_geq(uint64_t x, uint64_t y);
uint8_t _vanillc_crt_lti64(int64_t x, int64_t y);
uint8_t _vanillc_crt_gti64(int64_t x, int64_t y);
uint8_t _vanillc_crt_leqi64(int64_t x, int64_t y);
uint8_t _vanillc_crt_geqi64(int64_t x, int64_t y);
uint8_t _vanillc_crt_ispos(int64_t x);
uint8_t _vanillc_crt_isneg(int64_t x);
uint8_t _vanillc_crt_eq(uint64_t x, uint64_t y);
uint8_t _vanillc_crt_neq(uint64_t x, uint64_t y);
uint64_t _vanillc_crt_select(uint8_t cond, uint64_t a, uint64_t b);


/* _crt_vanillc_arith.c */
uint64_t _crtintrin_add64c(uint64_t a, uint64_t b, uint8_t *carry);
uint64_t _crtintrin_sub64b(uint64_t a, uint64_t b, uint8_t *borrow);
uint64_t _crtintrin_wmul128(uint64_t a, uint64_t b, uint64_t *hi);
uint64_t _crtintrin_wdiv128(uint64_t lo, uint64_t hi, uint64_t div, uint64_t *rhat, uint8_t *overflowed);


/* _crt_vanillc_bitops.c */
uint8_t _crtintrin_clz64(uint64_t x);
uint8_t _crtintrin_ctz64(uint64_t x);
uint64_t _crtintrin_bswap64(uint64_t x);
uint8_t _crtintrin_pcnt64(uint64_t x);


/* _crt_vanillc_alg.c */


#endif