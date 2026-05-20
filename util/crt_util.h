#ifndef _CRT_UTIL_H
#define _CRT_UTIL_H


#include <libdnml_types.h>
#include <include.h>
#include <dnml_sys/sys.h>
#include <_libdnml_config/numeric_config.h>
#include <_libdnml_mem/_ctx.h>
#include "../intrinsics/intrinsics.h"


/* crt_misc_util.c */
void __libdnml_memcpy_strict(void *buf, const void* src, size_t len, size_t start);
void __libdnml_memset_strict(void *buf, uint8_t val, size_t len, size_t start);
void __libdnml_memwipe_strict(void *buf, size_t len);



/* crt_bnum_util.c */
void __BICRT_TRIM_LZ__(bigInt *x);






#endif