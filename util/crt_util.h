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

#ifdef __cplusplus
extern "C" {
#endif

#define CHOOSE_OPTION(dest, cond, a, b) do { (dest) = _lib_crt_select((cond), (a), (b)); } while(0);
#define RETURN_OPTION(cond, a, b) do { return _lib_crt_select((cond), (a), (b)); } while(0);

#define crtmin(x, y) _lib_crt_select(_lib_crt_lt(x, y), x, y)
#define crtmax(x, y) _lib_crt_select(_lib_crt_gt(x, y), x, y)


/* crt_misc_util.c */
NO_INLINE void __libdnml_memset_strict(volatile void *dst, uint8_t val, size_t len, size_t start, size_t end, bool noop);
NO_INLINE void __libdnml_memwipe_strict(volatile void *dst, size_t len, size_t start, size_t end, bool noop);
NO_INLINE void __libdnml_memcpy_strict(
    volatile void *dst, const void* src, 
    size_t len, size_t srclen, 
    size_t start, size_t end, bool noop
);
NO_INLINE void __libdnml_memmove_strict(
    volatile void *dst, size_t cap,
    size_t dst_start, size_t src_start, 
    size_t len, bool noop
);
NO_INLINE void __libdnml_smemset_u64(volatile uint64_t *dst, uint8_t val, size_t len, size_t start, size_t end, bool noop);
NO_INLINE void __libdnml_smemwipe_u64(volatile uint64_t *dst, size_t len, size_t start, size_t end, bool noop);
NO_INLINE void __libdnml_smemcpy_u64(
    volatile uint64_t *dst, uint64_t *src,
    size_t len, size_t srclen,
    size_t start, size_t end, bool noop
);
NO_INLINE void __libdnml_smemmove_u64(
    volatile uint64_t *dst, size_t cap,
    size_t dst_start, size_t src_start, 
    size_t len, bool noop
);
uint64_t __CRT_MAG_I64__(int64_t x);
size_t __clamp_size(size_t cap, size_t insize);


/* crt_bnum_util.c */
dnml_status __CRINT_IFREE__(crint *x);
dnml_status __CRINT_INEW__(crint *x);
dnml_status __CRINT_INEWS__(crint *x, size_t n);
dnml_status __CRINT_TRIM_LZ__(crint *x);
int8_t __CRINT_INTERNAL_CMP__(crint *x, crint *y);
dnml_status __CRINT_INTERNAL_RLSHIFT__(crint *x, size_t len, size_t limb_cnt);
dnml_status __CRINT_INTERNAL_LLSHIFT__(crint *x, size_t len, size_t limb_cnt);
crint __CRINT_ERRVAL__(void);



#ifdef __cplusplus
}
#endif


#endif
