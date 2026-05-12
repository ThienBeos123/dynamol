#ifndef _IOCONV_H
#define _IOCONV_H



#include "../../test_ui/_strui.h"
#include "../../util/util.h"
// STDLIB utilities
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define stinl static inline
#define cvoid const void
#define csres const str_res

typedef xoshiro256_state rstate;
typedef _libdnml_str_suite suite;
typedef _libdnml_scase scase;
typedef str_res result;
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

#define INVAL_BI() .data.bi = { .limbs = NULL, .n = 1, .cap = 0, .sign = 0 }
#define BITOS_OUTPUT_MAX 3072 // 48 libms --> 3072 binary digits (Base-2)

stinl void _print_base(FILE* f, u8 base, bool endl) {
    switch (base) {
        case 1:     fputs("1 - (Unary)", f); break;
        case 2:     fputs("2 - (Binary)", f); break;
        case 3:     fputs("3 - (Ternary)", f); break;
        case 8:     fputs("8 - (Octal)", f); break;
        case 10:    fputs("10 - (Decimal)", f); break;
        case 16:    fputs("16 - (Hexadecimal)", f); break;
        default:    fprintf(f, "%" PRIu8 "", base); break;
    } if (endl) fputc('\n', f);
}

stinl void _dist_buf(
    void* *distbuf, void *full_buf, 
    size_t parts_per_suite, 
    u8 suite_num, size_t size_per_part
) {
    for (size_t i = 0; i < suite_num; ++i) {
        size_t increments = i * parts_per_suite * size_per_part;
        distbuf[i] = full_buf + increments;
    } return;
}






#endif