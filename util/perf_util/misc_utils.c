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



#include "../util.h"


size_t __BITCOUNT___(size_t digit_count, uint8_t base) {
    if (base == 10)          return digit_count * log2_10;
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


uint64_t _stou64(const char *buf, size_t buflen) {
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
    if (buflen < 2) { buf[0] = '\0'; return 0; }
    if (x == 0) { buf[0] = '0'; buf[1] = '\0'; return 1; }
    // Fill buffer from the end backwards
    int i = buflen - 2, count = 0;
    while (x && i >= 0) {
        buf[i] = '0' + (char)(x % 10);
        x /= 10; --i; ++count;
    }
    if (x) return 0;
    // Shift string to beginning of buffer
    int start = i + 1;
    if (start > 0) {
        for (int j = 0; j < count; ++j) {
            buf[j] = buf[start + j];
        }
    } buf[count] = '\0'; return count;
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
