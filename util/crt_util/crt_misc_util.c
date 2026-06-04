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
void __libdnml_memset_strict(void *buf, uint8_t val, size_t len, size_t start, size_t end, bool noop) {
    uint8_t *p = (uint8_t*)buf;
    for (size_t i = 0; i < len; ++i) { uint8_t curr = p[i];
        /* p[i] = (i >= start && i <= end) ? val : curr; */
        CHOOSE_OPTION((p[i]), ((i >= start) & (i <= end) & !(noop)), (val), (curr));
        curr = 0; // clang-format off
    } p = 0; buf = 0; val = 0; len = 0; start = 0; end = 0; noop = 0; // clang-format on
}
void __libdnml_memwipe_strict(void *buf, size_t len, size_t start, size_t end, bool noop) {
    uint8_t *p = (uint8_t*)buf;
    for (size_t i = 0; i < len; ++i) { uint8_t curr = p[i];
        /* p[i] = (i >= start && i <= end) ? val : curr; */
        CHOOSE_OPTION((p[i]), ((i >= start) & (i <= end) & !(noop)), (0), (curr));
        curr = 0; // clang-format off
    } p = 0; buf = 0; len = 0; start = 0; end = 0; noop = 0; // clang-format on
}
void __libdnml_memcpy_strict(void *buf, const void* src, size_t len, size_t srclen, size_t start, size_t end, bool noop) {
    uint8_t *p = (uint8_t*)buf; uint8_t *ps = (uint8_t*)src;
    size_t opsize = crtmax(len, srclen);
    for (size_t i = 0; i < opsize; ++i) {
        uint8_t curr, src_curr; size_t index;
        CHOOSE_OPTION((curr), (i >= len), (p[i]), (p[len - 1]));
        CHOOSE_OPTION((curr), (i >= srclen), (ps[i]), (0));
        CHOOSE_OPTION((index), (i >= len), (i), (len - 1))
        CHOOSE_OPTION((p[index]),
            ((i >= start) & (i <= end) & 
            (!noop) & (i < len)), 
            (src_curr), (curr)
        ); // clang-format off
        curr = 0; src_curr = 0; index = 0;
    } opsize = 0; p = 0; ps = 0; buf = 0; src = 0;
    len = 0; len = 0; srclen = 0; start = 0; end = 0; noop = 0; // clang-format on
}
void __libdnml_smemset_u64(uint64_t *buf, uint8_t val, size_t len, size_t start, size_t end, bool noop) {
    size_t instart = start * U64_BYTES;
    size_t inend; CHOOSE_OPTION((inend), (end), ((end - 1) * U64_BYTES + (U64_BYTES - 1)), (0));
    __libdnml_memset_strict(buf, val, len * U64_BYTES, instart, inend, noop); // clang-format off
    instart = 0; inend = 0; buf = 0; val = 0; len = 0; start = 0; end = 0; noop = 0; // clang-format on
}
void __libdnml_smemwipe_u64(uint64_t *buf, size_t len, size_t start, size_t end, bool noop) {
    size_t instart = start * U64_BYTES;
    size_t inend; CHOOSE_OPTION((inend), (end), ((end - 1) * U64_BYTES + (U64_BYTES - 1)), (0));
    __libdnml_memwipe_strict(buf, len * U64_BYTES, instart, inend, noop); // clang-format on
    instart = 0; inend = 0; buf = 0; len = 0; start = 0; end = 0; noop = 0; // clang-format off
}
void __libdnml_smemcpy_u64(
    uint64_t *dst, uint64_t *src, 
    size_t len, size_t srclen, 
    size_t start, size_t end, bool noop
) {
    size_t instart = start * U64_BYTES;
    size_t inend; CHOOSE_OPTION((inend), (end), ((end - 1) * U64_BYTES + (U64_BYTES - 1)), (0));
    __libdnml_memcpy_strict(dst, src, len * U64_BYTES, srclen * U64_BYTES, instart, inend, noop); // clang-format off
    instart = 0; inend = 0; dst = 0; src = 0; len = 0; srclen = 0; start = 0; end = 0; noop = 0; // clang-format on
}


/* crt_general_util */
uint64_t __CRT_MAG_I64__(int64_t val) {
    uint64_t i64_min_ret = (uint64_t)(llabs(val + 1)) + 1;
    uint64_t i64_norm_ret = (uint64_t)(llabs(val));
    uint64_t ret = _lib_crt_select((_lib_crt_eq(val, INT64_MIN)), i64_min_ret, i64_norm_ret); // clang-format off
    i64_min_ret = 0; i64_norm_ret = 0; val = 0; return ret; // clang-format on
}
size_t __clamp_size(size_t cap, size_t insize) { RETURN_OPTION((insize > cap), (cap), (insize)); }