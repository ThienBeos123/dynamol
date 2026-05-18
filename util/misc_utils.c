#include "util.h"


size_t __BITCOUNT___(size_t digit_count, uint8_t base) {
    if (base = 10)          return digit_count * log2_10;
    else if (base == 16)    return digit_count * log2_16;
    else if (base == 2)     return digit_count * log2_2;
    else if (base == 8)     return digit_count * log2_8;
    else                    return digit_count * (log10(2) / log10(base));
}


uint8_t __BASEN_DCOUNT__(uint64_t val, uint8_t base) {
    return (size_t)(log10(val) / log10(base)) + 1;
}


uint64_t __MAG_I64__(int64_t val) {
    return (val == INT64_MIN) ?
        (uint64_t)(llabs(val + 1)) + 1 :
        (uint64_t)(llabs(val));
}


static inline uint8_t _mul_will_overflow(uint64_t mul1, uint64_t mul2) { 
    return (mul2 && mul1 > UINT64_MAX / mul2);
}


uint64_t _stou64(const char *buf, int buflen) {
    if (!buflen || !buf) return 0;
    uint64_t res = 0;
    for (size_t i = 0; i < buflen; ++i) {
        if (buf[i] == '\0') break;
        if (buf[i] < '0' || buf[i] > '9') return 0;
        uint8_t digit = (uint8_t)(buf[i] - '0');
        if (res > (UINT64_MAX - digit) / 10) return 0;
        res = (res * 10) + digit;
    } return res;
}


int _itosn(uint64_t x, char *buf, int buflen) {
    if (!buflen) return 0;
    int i = buflen - 1, xlen = 0;
    while (x) { if (i < 0) { break; }
        buf[i] = '0' + (char)(x % 10);
        x /= 10; --i;
    } return xlen;
}

uint64_t _dnml_ipower_u64(uint64_t base, uint8_t power) {
    uint64_t res = 1;
    while (power) {
        if (power & 1) res *= base;
        base *= base;
        power >>= 1;
    }
    return res;
}