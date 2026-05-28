#include "__vanillc_conn__.h"

// 64 bit Count Leading Zeros
uint8_t _cintrin_clz64(uint64_t x) {
    if (!x) return 64;
    uint8_t n = 0;
    while (!(x & SIGN_BIT_MASK)) { x <<= 1; ++n; }
    return n;
}

// 64 bit Count Trailing Zeros
uint8_t _cintrin_ctz64(uint64_t x) {
    if (!x) return 64;
    uint8_t n = 0;
    while (!(x & 1)) { x >>= 1; ++n; }
    return n;
}

// 64 bit Byte Swapping
uint64_t _cintrin_bswap64(uint64_t x) {
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
uint8_t _cintrin_pcnt64(uint64_t x) {
    uint8_t n = 0;
    while (x) { if ((x & 1)) ++n; x >>= 1; }
    return n;
}