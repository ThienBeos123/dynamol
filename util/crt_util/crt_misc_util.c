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



#include "../crt_util.h"


/* crt_mem_util*/
NO_INLINE void __libdnml_memset_strict(volatile void *dst, uint8_t val, size_t len, size_t start, size_t end, bool noop) {
    volatile uint8_t *p = (uint8_t*)dst;
    for (size_t i = 0; i < len; ++i) { uint8_t curr = p[i];
        /* p[i] = (i >= start && i <= end) ? val : curr; */
        CHOOSE_OPTION((p[i]), ((i >= start) & (i <= end) & !(noop)), (val), (curr));
        curr = 0; // clang-format off
    } p = 0; dst = 0; val = 0; len = 0; start = 0; end = 0; noop = 0; // clang-format on
}
NO_INLINE void __libdnml_memwipe_strict(volatile void *dst, size_t len, size_t start, size_t end, bool noop) {
    volatile uint8_t *p = (uint8_t*)dst;
    for (size_t i = 0; i < len; ++i) { uint8_t curr = p[i];
        /* p[i] = (i >= start && i <= end) ? val : curr; */
        CHOOSE_OPTION((p[i]), ((i >= start) & (i <= end) & !(noop)), (0), (curr));
        curr = 0; // clang-format off
    } p = 0; dst = 0; len = 0; start = 0; end = 0; noop = 0; // clang-format on
}
NO_INLINE void __libdnml_memcpy_strict(
    volatile void *dst, const void* src, 
    size_t len, size_t srclen, 
    size_t start, size_t end, bool noop
) {
    volatile uint8_t *p = (uint8_t*)dst; uint8_t *ps = (uint8_t*)src;
    size_t opsize = crtmax(len, srclen);
    for (size_t i = 0; i < opsize; ++i) {
        size_t dst_idx = _lib_crt_select(_lib_crt_lt(i, len), i, 0);
        size_t src_idx = _lib_crt_select(_lib_crt_lt(i, srclen), i, 0);
        uint8_t curr = p[dst_idx], src_curr = ps[src_idx];
        CHOOSE_OPTION((p[dst_idx]), ((i >= start) & (i <= end) & (!noop) & (i < len) & (i < srclen)), (src_curr), (curr));
        curr = 0; src_curr = 0; dst_idx = 0; src_idx = 0;
    }
    /* Aggresive Post-operation Cleanup */
    opsize = 0; p = 0; ps = 0; dst = 0; src = 0;
    len = 0; srclen = 0; start = 0; end = 0; noop = 0; // clang-format on
}
NO_INLINE void __libdnml_memmove_strict(
    volatile void *dst, size_t cap, size_t dst_start,
    size_t src_start, size_t len, bool noop
) {
    /* Notice: This function is specifically preserved for intrabuffer moves */
    volatile uint8_t *buf = (volatile uint8_t *)dst;
    // Maximum lengths & BOUNDS calculations
    size_t max_dst_len = _lib_crt_select(_lib_crt_lt(dst_start, cap), cap - dst_start, 0);
    size_t max_src_len = _lib_crt_select(_lib_crt_lt(src_start, cap), cap - src_start, 0);
    // Safe, dispatched lengths for TRUNCATION
    size_t safe_len = _lib_crt_select(_lib_crt_lt(max_dst_len, len), max_dst_len, len);
    safe_len = _lib_crt_select(_lib_crt_lt(max_src_len, safe_len), max_src_len, safe_len);
    safe_len = _lib_crt_select(noop, 0, safe_len);
    // Compute concrete index boundaries
    size_t dst_end = dst_start + safe_len;
    size_t src_end = src_start + safe_len;

    // 2. Loop setup & configurations
    uint8_t forward = _lib_crt_lt(dst_start, src_start);
    size_t i = _lib_crt_select(forward, 0, cap), bound = _lib_crt_select(forward, cap, 0);
    while (_lib_crt_select(forward, _lib_crt_lt(i, bound), _lib_crt_gt(i, bound))) {
        size_t idx = _lib_crt_select(forward, i, i - 1);

        // Condition: Is the mapped dst_idx valid within the safe dst window?
        uint8_t in_dst = !!(_lib_crt_geq(idx, dst_start)) & !!(_lib_crt_lt(idx, dst_end));
        // Condition: Is the mapped src_idx valid within the safe src window?
        size_t src_idx = idx - dst_start + src_start; // Map the current dst_idx back to its src_idx counterpart
        uint8_t in_src = !!(_lib_crt_geq(src_idx, src_start)) & !!(_lib_crt_lt(src_idx, src_end));
        size_t safe_src_read_idx = _lib_crt_select(!!(in_dst) & !!(in_src), src_idx, 0);

        // ALWAYS read from memory to retain Memory-Access Uniformity
        uint8_t old_val = buf[idx], new_val = buf[safe_src_read_idx];
        buf[idx] = _lib_crt_select(!!(in_dst) & !!(in_src) & !(noop), new_val, old_val);
        if (forward) { ++i; } else --i; /* Loop-internal cleanup */ // clang-format off
        idx = 0; in_dst = 0; src_idx = 0; in_src = 0; safe_src_read_idx = 0; old_val = 0; // clang-format on
    }
    /* Aggressive Post-operation Cleanup */ // clang-format off
    dst = 0; cap = 0; dst_start = 0; src_start = 0; len = 0; noop = 0; buf = 0;
    max_dst_len = 0; max_src_len = 0; safe_len = 0; dst_end = 0; src_end = 0; forward = 0; // clang-format off
}

NO_INLINE void __libdnml_smemset_u64(volatile uint64_t *dst, uint8_t val, size_t len, size_t start, size_t end, bool noop) {
    size_t instart = start * U64_BYTES;
    size_t inend; CHOOSE_OPTION((inend), (_lib_crt_lt(end, 1)), (U64_BYTES), ((end * U64_BYTES + (U64_BYTES - 1))));
    __libdnml_memset_strict(dst, val, len * U64_BYTES, instart, inend, noop); // clang-format off
    instart = 0; inend = 0; dst = 0; val = 0; len = 0; start = 0; end = 0; noop = 0; // clang-format on
}
NO_INLINE void __libdnml_smemwipe_u64(volatile uint64_t *dst, size_t len, size_t start, size_t end, bool noop) {
    size_t instart = start * U64_BYTES;
    size_t inend; CHOOSE_OPTION((inend), (_lib_crt_lt(end, 1)), (U64_BYTES), ((end * U64_BYTES + (U64_BYTES - 1))));
    __libdnml_memwipe_strict(dst, len * U64_BYTES, instart, inend, noop); // clang-format on
    instart = 0; inend = 0; dst = 0; len = 0; start = 0; end = 0; noop = 0; // clang-format off
}
NO_INLINE void __libdnml_smemcpy_u64(
    volatile uint64_t *dst, uint64_t *src,
    size_t len, size_t srclen,
    size_t start, size_t end, bool noop
) {
    size_t instart = start * U64_BYTES;
    size_t inend; CHOOSE_OPTION((inend), (_lib_crt_lt(end, 1)), (U64_BYTES), ((end * U64_BYTES + (U64_BYTES - 1))));
    __libdnml_memcpy_strict(dst, src, len * U64_BYTES, srclen * U64_BYTES, instart, inend, noop); // clang-format off
    instart = 0; inend = 0; dst = 0; src = 0; len = 0; srclen = 0; start = 0; end = 0; noop = 0; // clang-format on
}
NO_INLINE void __libdnml_smemmove_u64(
    volatile uint64_t *dst, size_t cap, size_t dst_start,
    size_t src_start, size_t len, bool noop
) { // clang-format off
    __libdnml_memmove_strict(dst, cap * U64_BYTES, dst_start * U64_BYTES, src_start * U64_BYTES, len * U64_BYTES, noop);
    dst = 0; cap = 0; dst_start = 0; src_start = 0; len = 0; noop = 0; // clang-format on
}


/* crt_general_util */
uint64_t __CRT_MAG_I64__(int64_t val) {
    uint64_t i64_min_ret = (uint64_t)(llabs(val + 1)) + 1;
    uint64_t i64_norm_ret = (uint64_t)(llabs(val));
    uint64_t ret = _lib_crt_select((_lib_crt_eq(val, INT64_MIN)), i64_min_ret, i64_norm_ret); // clang-format off
    i64_min_ret = 0; i64_norm_ret = 0; val = 0; return ret; // clang-format on
}
size_t __clamp_size(size_t cap, size_t insize) { RETURN_OPTION((insize > cap), (cap), (insize)); }
