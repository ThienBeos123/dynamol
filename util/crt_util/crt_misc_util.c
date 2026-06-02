#include "../crt_util.h"


/* crt_mem_util*/
void __libdnml_memset_strict(void *buf, uint8_t val, size_t len, size_t start, size_t end, bool noop) {
    uint8_t *p = (uint8_t*)buf;
    for (size_t i = 0; i < len; ++i) { uint8_t curr = p[i];
        /* p[i] = (i >= start && i <= end) ? val : curr; */
        CHOOSE_OPTION((p[i]), ((i >= start) & (i <= end) & !(noop)), (val), (curr));
    }
}
void __libdnml_memwipe_strict(void *buf, size_t len, size_t start, size_t end, bool noop) {
    uint8_t *p = (uint8_t*)buf;
    for (size_t i = 0; i < len; ++i) { uint8_t curr = p[i];
        /* p[i] = (i >= start && i <= end) ? val : curr; */
        CHOOSE_OPTION((p[i]), ((i >= start) & (i <= end) & !(noop)), (0), (curr));
    }
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
        );
    }
}
void __libdnml_smemset_u64(uint64_t *buf, uint8_t val, size_t len, size_t start, size_t end, bool noop) {
    size_t instart = start * U64_BYTES;
    size_t inend; CHOOSE_OPTION((inend), (end), ((end - 1) * U64_BYTES + (U64_BYTES - 1)), (0));
    __libdnml_memset_strict(buf, val, len * U64_BYTES, instart, inend, noop);
}
void __libdnml_smemwipe_u64(uint64_t *buf, size_t len, size_t start, size_t end, bool noop) {
    size_t instart = start * U64_BYTES;
    size_t inend; CHOOSE_OPTION((inend), (end), ((end - 1) * U64_BYTES + (U64_BYTES - 1)), (0));
    __libdnml_memwipe_strict(buf, len * U64_BYTES, instart, inend, noop);
}
void __libdnml_smemcpy_u64(
    uint64_t *dst, uint64_t *src, 
    size_t len, size_t srclen, 
    size_t start, size_t end, bool noop
) {
    size_t instart = start * U64_BYTES;
    size_t inend; CHOOSE_OPTION((inend), (end), ((end - 1) * U64_BYTES + (U64_BYTES - 1)), (0));
    __libdnml_memcpy_strict(dst, src, len * U64_BYTES, srclen * U64_BYTES, instart, inend, noop);
}


/* crt_general_util */
uint64_t __CRT_MAG_I64__(int64_t val) {
    uint64_t i64_min_ret = (uint64_t)(llabs(val + 1)) + 1;
    uint64_t i64_norm_ret = (uint64_t)(llabs(val));
    RETURN_OPTION((val == INT64_MIN), (i64_min_ret), (i64_norm_ret));
}
size_t __clamp_size(size_t cap, size_t insize) { RETURN_OPTION((insize > cap), (cap), (insize)); }