#ifndef DNML_UTIL_H
#define DNML_UTIL_H

#include <libdnml_types.h>
#include <include.h>
#include <dnml_sys/sys.h>
#include <_libdnml_config/numeric_config.h>
#include <_libdnml_mem/_ctx.h>
#include <dnml_status.h>
#include "../intrinsics/intrinsics.h"
#include "aconv_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

#define min(x, y) ( ((x) < (y)) ? (x) : (y) )
#define max(x, y) ( ((x) > (y)) ? (x) : (y) )


/* ---------------------- */
/* rng_util.c */
/* ---------------------- */
typedef struct { uint64_t s[4]; } xoshiro256_state;
uint64_t splitmix64(uint64_t x);
void seed_xoshiro256(xoshiro256_state *state, uint64_t x);
uint64_t xoshiro256pp_next(xoshiro256_state *state);
float xoshiro256pp_fnext01(xoshiro256_state *state);
xoshiro256_state mix_xoshiro256(xoshiro256_state *stateA, xoshiro256_state *stateB);
uint64_t __rng_skrange(xoshiro256_state *state, uint64_t min, uint64_t max, float median_dist);
uint64_t __rng_range(xoshiro256_state *state, uint64_t min, uint64_t max);
float __rng_frange(xoshiro256_state *state, float min, float max);
float __froll(xoshiro256_state *state);


/* ---------------------- */
/* str_parse.c */
/* ---------------------- */
size_t _actual_len(const char *str, size_t buflen, size_t *actual_len);
uint16_t _fskip_whitespace__(FILE *stream);
size_t _skip_whitespace(const char *str, size_t len, size_t *pos);
size_t _skip_leading_zeros(const char *str, size_t len, size_t *pos);
uint8_t _is_valid_digit__(uint16_t *curr_char);
uint8_t _sign_handle_(const char *str, size_t *curr_pos, uint8_t *sign);
uint8_t _sign_handle_nlen_(const char *str, size_t *curr_pos, uint8_t *sign, size_t len);
uint8_t _prefix_handle_(const char *str, size_t *curr_pos, uint8_t *base);
uint8_t _prefix_handle_nlen_(const char *str, size_t *curr_pos, uint8_t *base, size_t len);
uint8_t _prefix_handle_stream__(FILE* stream, uint8_t *base, uint16_t *curr_char);

/* ---------------------- */
/* misc_utils.c */
/* ---------------------- */
size_t __BITCOUNT___(size_t digit_count, uint8_t base);
uint8_t __BASEN_DCOUNT__(uint64_t val, uint8_t base);
uint64_t __MAG_I64__(int64_t val);
uint64_t _stou64(const char *buf, int buflen);
int _itosn(uint64_t x, char *buf, int buflen);
uint64_t _dnml_ipower_u64(uint64_t base, uint8_t power);


/* ---------------------- */
/* bigNum_utils.c */
/* ---------------------- */
dnml_status __BIGINT_INTERNAL_EMPINIT__(bigInt *x);
dnml_status __BIGINT_INTERNAL_LINIT__(bigInt *x, size_t k);
dnml_status __BIGINT_INTERNAL_ENSCAP__(bigInt *x, size_t k);
dnml_status __BIGINT_INTERNAL_REALLOC__(bigInt *x, size_t k);
void __BIGINT_INTERNAL_FREE__(bigInt *x);

uint8_t __BIGINT_INTERNAL_VALID__(const bigInt *x);
uint8_t __BIGINT_INTERNAL_SVALID__(const bigInt *x);
uint8_t __BIGINT_INTERNAL_PVALID__(const bigInt *x);
bigInt __BIGINT_ERROR_VALUE__(void);

void __BIGINT_INTERNAL_COPY__(bigInt *dst, const bigInt *source);
void __BIGINT_INTERNAL_TRIM_LZ__(bigInt *x);
void __BIGINT_INTERNAL_ZSET__(bigInt *x);
void __BIGINT_INTERNAL_SWAP__(bigInt *x, bigInt *y);
size_t __BIGINT_COUNTDB__(const bigInt *x, uint8_t base);
size_t __BIGINT_MAXCDB__(size_t lcnt, uint8_t base);
size_t __BIGINT_LIMBS_NEEDED__(size_t bits);
uint8_t __BIGINT_WILL_OVERFLOW__(const bigInt *x, uint64_t threshold);
size_t __BIGINT_CTZ__(const bigInt *x);

int8_t __BIGINT_INTERNAL_COMP__(const bigInt *x, const bigInt *y);
uint8_t __BIGINT_IS_EVEN__(const bigInt *x);
void __BIGINT_INTERNAL_ADD_UI64__(bigInt *x, uint64_t val);
void __BIGINT_INTERNAL_MUL_UI64__(bigInt *x, uint64_t val);
void __BIGINT_DIV3__(bigInt *a);
uint64_t __BIGINT_INTERNAL_DIVMOD_UI64__(bigInt *x, uint64_t val);
void __BIGINT_INTERNAL_RSHIFT__(bigInt *x, size_t k);
void __BIGINT_INTERNAL_LSHIFT__(bigInt *x, size_t k);
void __BIGINT_INTERNAL_RLSHIFT__(bigInt *x, size_t klimbs);
void __BIGINT_INTERNAL_LLSHIFT__(bigInt *x, size_t klimbs);



#ifdef __cplusplus
}
#endif

#endif

