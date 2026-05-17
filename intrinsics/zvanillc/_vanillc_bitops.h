#ifndef ____DNML_VANILLC_UTILS
#define ____DNML_VANILLC_UTILS



#include <stdint.h>
#include <math.h>
#include <system/sys.h>

#define msb_mask ((UINT64_MAX >> 1) + 1)

// 64 bit Count Leading Zeros
static inline uint8_t _cintrin_clz64(uint64_t x) {
    uint8_t n = 0;
    while (!(x & msb_mask)) { x <<= 1; ++n; }
    return n;
}

// 64 bit Count Trailing Zeros
static inline uint8_t _cintrin_ctz64(uint64_t x) {
    uint8_t n = 0;
    while (!(x & 1)) { x >>= 1; ++n; }
    return n;
}

// 64 bit Byte Swapping
static inline uint64_t _cintrin_bswap64(uint64_t x) {
    return ((x << 56) |
    ((x << 40) & 0x00FF000000000000ULL) |
    ((x << 24) & 0x0000FF0000000000ULL) |
    ((x << 8)  & 0x000000FF00000000ULL) |
    ((x >> 8)  & 0x00000000FF000000ULL) |
    ((x >> 24) & 0x0000000000FF0000ULL) |
    ((x >> 40) & 0x000000000000FF00ULL) |
    (x >> 56));
}

// 64 bit Popoulation Counting
static inline uint8_t _cintrin_pcnt64(uint64_t x) {
    uint8_t n = 0;
    while (x) { if (!(x & 1)) ++n; x >>= 1; }
    return n;
}



#endif