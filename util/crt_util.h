#ifndef _CRT_UTIL_H
#define _CRT_UTIL_H


#include <debug_util.h>
#include <libdnml_types.h>
#include <include.h>
#include <dnml_status.h>
#include <dnml_sys/sys.h>
#include <_libdnml_config/numeric_config.h>
#include <_libdnml_mem/_ctx.h>
#include "../intrinsics/intrinsics.h"


#define CHOOSE_OPTION(dest, cond, a, b) do { \
    uint64_t a_copy = (a); \
    (a_copy) ^= ((a) ^ (b)) & -((int8_t)(cond)); \
    (dest) = (a_copy); \
} while(0);
#define RETURN_OPTION(cond, a, b) do { \
    uint64_t a_copy = (a); \
    (a_copy) ^= ((a) ^ (b)) & -((int8_t)(cond)); \
    return (a_copy); \
} while(0);


/* crt_misc_util.c */
void __libdnml_memcpy_strict(void *buf, const void* src, size_t len, size_t start, size_t end);
void __libdnml_memset_strict(void *buf, uint8_t val, size_t len, size_t start, size_t end);
void __libdnml_memwipe_strict(void *buf, size_t len);
uint64_t __MAG_I64__(int64_t x);


/* crt_bnum_util.c */
drypto_stat __CRINT_TRIM_LZ__(cryptInt *x);
cryptInt __CRINT_ERRVAL__(void);






#endif