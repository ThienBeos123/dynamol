#ifndef _BIGINT_EXEC_FUNC_H
#define _BIGINT_EXEC_FUNC_H


#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "../../../test_ui/_strui.h"
#include "../../../dynamol/big_numbers/bigNums.h"
#include "../../../dynamol/big_numbers/bigInt_func.h"
#include "../_ioconv.h"
#include "bi_indef.h"



/* Note 1 - Allocation:
*   1) Most of the evaluation and inverse wrappers in bi_exec_func.h
*      utilizes scratch allocation seeded from vctx/vctx.
*   -----> Entails tester and main testing units, when seeding
*          temporary scratch spaces for testing, to always:

            1. Mark the initial offset:

                    size_t unit_start = dratch_mark(&ctx_);

            2. Reset towards that initial offset at
               the end of the testing unit:

                    dratch_reset(&ctx_, unit_start);

*   2) Due to there being a lot of functions to test, with a
*      wide variety of function categories, testers may implement
*      either a single universal scratch buffer for every functions,
*      a buffer for each category, or a buffer for each function. The choice
*      depends on the amount of functions being tested

*   3) For every scratch buffers initialized, its setup and cleanup
*      MUST follow (sub-note 1) to ensure memory-safe and efficient test runs
*/


//* ========================= BITOS EXECUTION WRAPPERS ======================= *//
/* BITOS Subnote 1 - Conversions:
    - All BITOS conversion functions (to_str, tto_str, and subsequent variants)
      mandates the setup of the buffer length inside *vin and allocate enough
      arena/heap space for such self-contained str_res headers with FAM strings
*/
// Truncative Conversions - tto_str
static inline void exec_bitos_tto_str(const void *vin, str_res *out, void *vctx) {
    const bitos_conv_in *in = vin;
    out->type = STRING; out->pstr[out->cap] = '\0';
    out->status = bigInt_tto_str(out->pstr, in->x, &out->data.len);
}
static inline void exec_bitos_tto_strb(const void *vin, str_res *out, void *vctx) {
    const bitos_conv_in *in = vin;
    out->type = STRING;
    out->pstr[out->cap] = '\0';
    out->status = bigInt_tto_strb(out->pstr, in->x, in->base, &out->data.len);
}
static inline void exec_bitos_tto_strn(const void *vin, str_res *out, void *vctx) {
    const bitos_conv_in *in = vin;
    out->type = STRING;
    out->status = bigInt_tto_strn(out->pstr, in->len, in->x, &out->data.len);
}
static inline void exec_bitos_tto_strnb(const void *vin, str_res *out, void *vctx) {
    const bitos_conv_in *in = vin;
    out->type = STRING;
    out->status = bigInt_tto_strnb(out->pstr, in->len, in->x, in->base, &out->data.len);
}
static inline void exec_bitos_tto_strf(const void *vin, str_res *out, void *vctx) {
    const bitos_conv_in *in = vin;
    out->type = STRING;
    out->status = bigInt_tto_strf(
        out->pstr, in->len, in->x, 
        in->base, in->uppercase,
        &out->data.len
    );
}
// Default / Strict Conversions - to_str
static inline void exec_bitos_to_str(const void *vin, str_res *out, void *vctx) {
    const bitos_conv_in *in = vin;
    out->type = STRING;
    out->pstr[out->cap] = '\0';
    out->status = bigInt_to_str(out->pstr, in->x, &out->data.len);
}
static inline void exec_bitos_to_strb(const void *vin, str_res *out, void *vctx) {
    const bitos_conv_in *in = vin;
    out->type = STRING;
    out->pstr[out->cap] = '\0';
    out->status = bigInt_to_strb(out->pstr, in->x, in->base, &out->data.len);
}
static inline void exec_bitos_to_strn(const void *vin, str_res *out, void *vctx) {
    const bitos_conv_in *in = vin;
    out->type = STRING;
    out->status = bigInt_to_strn(out->pstr, in->len, in->x, &out->data.len);
}
static inline void exec_bitos_to_strnb(const void *vin, str_res *out, void *vctx) {
    const bitos_conv_in *in = vin;
    out->type = STRING;
    out->status = bigInt_to_strnb(out->pstr, in->len, in->x, in->base, &out->data.len);
}
static inline void exec_bitos_to_strf(const void *vin, str_res *out, void *vctx) {
    const bitos_conv_in *in = vin;
    out->type = STRING;
    out->status = bigInt_to_strf(
        out->pstr, in->len, in->x, 
        in->base, in->uppercase, 
        &out->data.len
    );
}
// Instant / Streamed Printing - fput
static inline void exec_bitos_fput(const void *vin, str_res *out, void *vctx) {
    const bitos_print_in *in = vin; out->type = STRING;
    FILE *tmp = tmpfile(); if (tmp == NULL) { 
        perror("Unable to open tmpfile(), Terminating program...");
        abort();
    } 
    dnml_status stat = bigInt_fput(tmp, in->x);
    fflush(tmp); rewind(tmp);
    char buf[64]; size_t len = 0;
    while (1) { 
        size_t n = fread(out->pstr + len,  sizeof(char), 64, tmp); 
        len += n; if (feof(tmp)) break;
    }
    out->pstr[len] = '\0'; out->status = stat;
    out->data.len = len; fclose(tmp);
}
static inline void exec_bitos_fputb(const void *vin, str_res *out, void *vctx) {
    const bitos_print_in *in = vin; out->type = STRING;
    FILE *tmp = tmpfile(); if (tmp == NULL) { 
        perror("Unable to open tmpfile(), Terminating program...");
        abort();
    } 
    dnml_status stat = bigInt_fputb(tmp, in->x, in->base);
    fflush(tmp); rewind(tmp);
    char buf[64]; size_t len = 0;
    while (1) {
        size_t n = fread(out->pstr + len, sizeof(char), 64, tmp); 
        len += n; if (feof(tmp)) break;
    } 
    out->pstr[len] = '\0'; out->status = stat;
    out->data.len = len; fclose(tmp);
}
static inline void exec_bitos_fputf(const void *vin, str_res *out, void *vctx) {
    const bitos_print_in *in = vin; out->type = STRING;
    FILE *tmp = tmpfile(); if (tmp == NULL) { 
        perror("Unable to open tmpfile(), Terminating program...");
        abort();
    } 
    dnml_status stat = bigInt_fputf(tmp, in->x, in->base, in->uppercase);
    fflush(tmp); rewind(tmp);
    char buf[64]; size_t len = 0;
    while (1) {
        size_t n = fread(out->pstr + len, sizeof(char), 64, tmp);
        len += n; if (feof(tmp)) break;
    } 
    out->pstr[len] = '\0'; out->status = stat;
    out->data.len = len; fclose(tmp);
}
// Buffered Printing - sfput
static inline void exec_bitos_sfput(const void *vin, str_res *out, void *vctx) {
    const bitos_print_in *in = vin; out->type = STRING;
    FILE *tmp = tmpfile(); if (tmp == NULL) { 
        perror("Unable to open tmpfile(), Terminating program...");
        abort();
    } 
    dnml_status stat = bigInt_sfput(tmp, in->x);
    fflush(tmp); rewind(tmp);
    char buf[64]; size_t len = 0;
    while (1) {
        size_t n = fread(out->pstr + len, sizeof(char), 64, tmp);
        len += n; if (feof(tmp)) break;
    } 
    out->pstr[len] = '\0'; out->status = stat;
    out->data.len = len; fclose(tmp);
}
static inline void exec_bitos_sfputb(const void *vin, str_res *out, void *vctx) {
    const bitos_print_in *in = vin; out->type = STRING;
    FILE *tmp = tmpfile(); if (tmp == NULL) { 
        perror("Unable to open tmpfile(), Terminating program...");
        abort();
    } 
    dnml_status stat = bigInt_sfputb(tmp, in->x, in->base);
    fflush(tmp); rewind(tmp);
    char buf[64]; size_t len = 0;
    while (1) {
        size_t n = fread(out->pstr + len, sizeof(char), 64, tmp);
        len += n; if (feof(tmp)) break;
    } 
    out->pstr[len] = '\0'; out->status = stat;
    out->data.len = len; fclose(tmp);
}
static inline void exec_bitos_sfputf(const void *vin, str_res *out, void *vctx) {
    const bitos_print_in *in = vin; out->type = STRING;
    FILE *tmp = tmpfile(); if (tmp == NULL) { 
        perror("Unable to open tmpfile(), Terminating program...");
        abort();
    }
    dnml_status stat = bigInt_sfputf(tmp, in->x, in->base, in->uppercase);
    fflush(tmp); rewind(tmp);
    char buf[64]; size_t len = 0;
    while (1) {
        size_t n = fread(out->pstr + len, sizeof(char), 64, tmp);
        len += n; if (feof(tmp)) break;
    } 
    out->pstr[len] = '\0'; out->status = stat;
    out->data.len = len; fclose(tmp);
}
// Stream-based Raw Output - fwrite
static inline void exec_bitos_fwrite(const void *vin, str_res *out, void *vctx) { DNML_UNFINISHED(); }
static inline void exec_bitos_serial(const void *vin, str_res *out, void *vctx) { DNML_UNFINISHED(); }
static inline void exec_bitos_tserial(const void *vin, str_res *out, void *vctx) { DNML_UNFINISHED(); }
static inline void exec_bitos_sserial(const void *vin, str_res *out, void *vctx) { DNML_UNFINISHED(); }
// Utilities
static inline void exec_bitos_ldump(const void *vin, str_res *out, void *vctx) {
    const bitos_util_in *in = vin; out->type = STRING;
    FILE *tmp = tmpfile(); if (tmp == NULL) { 
        perror("Unable to open tmpfile(), Terminating program...");
        abort();
    } bigInt_limb_dump(tmp, in->x);
    fflush(tmp); rewind(tmp);
    char buf[256]; size_t len = 0;
    while (1) {
        size_t n = fread(out->pstr + len, sizeof(char), 256, tmp); 
        len += n; if (feof(tmp)) break;
    } out->pstr[len] = '\0'; 
    out->data.len = len; fclose(tmp);
}
static inline void exec_bitos_hdump(const void *vin, str_res *out, void *vctx) {
    const bitos_util_in *in = vin; out->type = STRING;
    FILE *tmp = tmpfile(); if (tmp == NULL) { 
        perror("Unable to open tmpfile(), Terminating program...");
        abort();
    } bigInt_hexdump(tmp, in->x, in->uppercase);
    fflush(tmp); rewind(tmp);
    char buf[256]; size_t len = 0;
    while (1) {
        size_t n = fread(out->pstr + len, sizeof(char), 256, tmp);
        len += n; if (feof(tmp)) break;
    } out->pstr[len] = '\0'; 
    out->data.len = len; fclose(tmp);
}
static inline void exec_bitos_bdump(const void *vin, str_res *out, void *vctx) {
    const bitos_util_in *in = vin; out->type = STRING;
    FILE *tmp = tmpfile(); if (tmp == NULL) { 
        perror("Unable to open tmpfile(), Terminating program...");
        abort();
    } bigInt_bindump(tmp, in->x);
    fflush(tmp); rewind(tmp);
    char buf[256]; size_t len = 0;
    while (1) {
        size_t n = fread(out->pstr + len, sizeof(char), 256, tmp);
        len += n; if (feof(tmp)) break;
    } out->pstr[len] = '\0'; 
    out->data.len = len; fclose(tmp);
}
static inline void exec_bitos_info(const void *vin, str_res *out, void *vctx) {
    const bitos_util_in *in = vin; out->type = STRING;
    FILE *tmp = tmpfile(); if (tmp == NULL) { 
        perror("Unable to open tmpfile(), Terminating program...");
        abort();
    } bigInt_info(tmp, in->x);
    fflush(tmp); rewind(tmp);
    char buf[256]; size_t len = 0;
    while (1) {
        size_t n = fread(out->pstr + len, sizeof(char), 256, tmp);
        len += n; if (feof(tmp)) break;
    } out->pstr[len] = '\0'; 
    out->data.len = len; fclose(tmp);
}



//* ========================= STOBI EXECUTION WRAPPERS ======================= *//
// Initialization - strinint
static inline void exec_stobi_strinit(const void *vin, str_res *out, void *vctx) {
    const stobi_init_in *in = vin;
    out->type = BIGINT; bigInt tmp;
    out->status = bigInt_strinit(&tmp, in->str);

    limb_t *tmp_limbs = (limb_t*)vctx;
    memcpy(tmp_limbs, tmp.limbs, tmp.n * BYTES_IN_UINT64_T);
    out->data.bi.limbs = tmp_limbs;     out->data.bi.sign = tmp.sign;
    out->data.bi.n = tmp.n;             out->data.bi.cap = tmp.cap;
    __BIGINT_INTERNAL_FREE__(&tmp);
}
static inline void exec_stobi_strbinit(const void *vin, str_res *out, void *vctx) {
    const stobi_init_in *in = vin;
    out->type = BIGINT; bigInt tmp;
    out->status = bigInt_strbinit(&tmp, in->str, in->base);
    
    limb_t *tmp_limbs = (limb_t*)vctx;
    memcpy(tmp_limbs, tmp.limbs, tmp.n * BYTES_IN_UINT64_T);
    out->data.bi.limbs = tmp_limbs;     out->data.bi.sign = tmp.sign;
    out->data.bi.n = tmp.n;             out->data.bi.cap = tmp.cap;
    __BIGINT_INTERNAL_FREE__(&tmp);
}
static inline void exec_stobi_strninit(const void *vin, str_res *out, void *vctx) {
    const stobi_init_in *in = vin;
    out->type = BIGINT; bigInt tmp;
    out->status = bigInt_strninit(&tmp, in->str, in->len);
    
    limb_t *tmp_limbs = (limb_t*)vctx;
    memcpy(tmp_limbs, tmp.limbs, tmp.n * BYTES_IN_UINT64_T);
    out->data.bi.limbs = tmp_limbs;     out->data.bi.sign = tmp.sign;
    out->data.bi.n = tmp.n;             out->data.bi.cap = tmp.cap;
    __BIGINT_INTERNAL_FREE__(&tmp);
}
static inline void exec_stobi_strnbinit(const void *vin, str_res *out, void *vctx) {
    const stobi_init_in *in = vin;
    out->type = BIGINT; bigInt tmp;
    out->status = bigInt_strnbinit(&tmp, in->str, in->len, in->base);
    
    limb_t *tmp_limbs = (limb_t*)vctx;
    memcpy(tmp_limbs, tmp.limbs, tmp.n * BYTES_IN_UINT64_T);
    out->data.bi.limbs = tmp_limbs;     out->data.bi.sign = tmp.sign;
    out->data.bi.n = tmp.n;             out->data.bi.cap = tmp.cap;
    __BIGINT_INTERNAL_FREE__(&tmp);
}
// Conversions - from_str
static inline void exec_stobi_from_str(const void *vin, str_res *out, void *vctx) {
    const stobi_conv_in *in = vin;
    out->type = BIGINT; bigInt tmp;
    tmp = bigInt_from_str(in->str, &out->status);

    limb_t *tmp_limbs = (limb_t*)vctx;
    memcpy(tmp_limbs, tmp.limbs, tmp.n * BYTES_IN_UINT64_T);
    out->data.bi.limbs = tmp_limbs;     out->data.bi.sign = tmp.sign;
    out->data.bi.n = tmp.n;             out->data.bi.cap = tmp.cap;
    __BIGINT_INTERNAL_FREE__(&tmp);
}
static inline void exec_stobi_from_strb(const void *vin, str_res *out, void *vctx) {
    const stobi_conv_in *in = vin;
    out->type = BIGINT; bigInt tmp;
    tmp = bigInt_from_strb(in->str, in->base, &out->status);

    limb_t *tmp_limbs = (limb_t*)vctx;
    memcpy(tmp_limbs, tmp.limbs, tmp.n * BYTES_IN_UINT64_T);
    out->data.bi.limbs = tmp_limbs;     out->data.bi.sign = tmp.sign;
    out->data.bi.n = tmp.n;             out->data.bi.cap = tmp.cap;
    __BIGINT_INTERNAL_FREE__(&tmp);
}
static inline void exec_stobi_from_strn(const void *vin, str_res *out, void *vctx) {
    const stobi_conv_in *in = vin;
    out->type = BIGINT; bigInt tmp;
    tmp = bigInt_from_strn(in->str, in->len, &out->status);

    limb_t *tmp_limbs = (limb_t*)vctx;
    memcpy(tmp_limbs, tmp.limbs, tmp.n * BYTES_IN_UINT64_T);
    out->data.bi.limbs = tmp_limbs;     out->data.bi.sign = tmp.sign;
    out->data.bi.n = tmp.n;             out->data.bi.cap = tmp.cap;
    __BIGINT_INTERNAL_FREE__(&tmp);
}
static inline void exec_stobi_from_strnb(const void *vin, str_res *out, void *vctx) {
    const stobi_conv_in *in = vin;
    out->type = BIGINT; bigInt tmp;
    tmp = bigInt_from_strnb(in->str, in->len, in->base, &out->status);

    limb_t *tmp_limbs = (limb_t*)vctx;
    memcpy(tmp_limbs, tmp.limbs, tmp.n * BYTES_IN_UINT64_T);
    out->data.bi.limbs = tmp_limbs;     out->data.bi.sign = tmp.sign;
    out->data.bi.n = tmp.n;             out->data.bi.cap = tmp.cap;
    __BIGINT_INTERNAL_FREE__(&tmp);
}
// Default Assignments - get_str - GROWS
static inline void exec_stobi_get_str(const void *vin, str_res *out, void *vctx) {
    const stobi_assign_in *in = vin;
    out->type = BIGINT; uint8_t base; dnml_status prestat;
    size_t digit = bigInt_get_size(in->str, strlen(in->str), &base, &prestat);
    size_t lcnt = __BIGINT_LIMBS_NEEDED__(__BITCOUNT___(digit, base));
    if (prestat != STR_SUCCESS) {
        limb_t *tmp_limb = (limb_t*)vctx;
        bigInt temp = { .limbs = tmp_limb, .n = 0, .sign = 1, .cap = (lcnt) ? lcnt : 1 };
        if (digit) bigInt_get_strn(&temp, in->str, digit);
        out->data.bi = temp;
    } else {
        limb_t *tmp_limb = (limb_t*)vctx;
        bigInt temp = { .limbs = tmp_limb, .n = 0, .sign = 1, .cap = lcnt };
        bigInt_get_str(&temp, in->str); out->data.bi = temp;
    } out->status = prestat;
}
static inline void exec_stobi_get_strb(const void *vin, str_res *out, void *vctx) {
    const stobi_assign_in *in = vin;
    out->type = BIGINT; dnml_status prestat;
    size_t digit = bigInt_get_sizeb(in->str, strlen(in->str), in->base, &prestat);
    size_t lcnt = __BIGINT_LIMBS_NEEDED__(__BITCOUNT___(digit, in->base));
    if (prestat != STR_SUCCESS) {
        limb_t *tmp_limb = (limb_t*)vctx;
        bigInt temp = { .limbs = tmp_limb, .n = 0, .sign = 1, .cap = (lcnt) ? lcnt : 1 };
        if (digit) bigInt_get_strnb(&temp, in->str, digit, in->base);
        out->data.bi = temp;
    } else {
        limb_t *tmp_limb = (limb_t*)vctx;
        bigInt temp = { .limbs = tmp_limb, .n = 0, .sign = 1, .cap = lcnt };
        bigInt_get_strb(&temp, in->str, in->base); out->data.bi = temp;
    } out->status = prestat;
}
static inline void exec_stobi_get_strn(const void *vin, str_res *out, void *vctx) {
    const stobi_assign_in *in = vin;
    out->type = BIGINT; uint8_t base; dnml_status prestat;
    size_t digit = bigInt_get_size(in->str, in->len, &base, &prestat);
    size_t lcnt = __BIGINT_LIMBS_NEEDED__(__BITCOUNT___(digit, base));
    if (prestat != STR_SUCCESS) {
        limb_t *tmp_limb = (limb_t*)vctx;
        bigInt temp = { .limbs = tmp_limb, .n = 0, .sign = 1, .cap = (lcnt) ? lcnt : 1 };
        if (digit) bigInt_get_strn(&temp, in->str, digit);
        out->data.bi = temp;
    } else {
        limb_t *tmp_limb = (limb_t*)vctx;
        bigInt temp = { .limbs = tmp_limb, .n = 0, .sign = 1, .cap = lcnt };
        bigInt_get_strn(&temp, in->str, in->len); out->data.bi = temp;
    } out->status = prestat;
}
static inline void exec_stobi_get_strnb(const void *vin, str_res *out, void *vctx) {
    const stobi_assign_in *in = vin;
    out->type = BIGINT; dnml_status prestat;
    size_t digit = bigInt_get_sizeb(in->str, in->len, in->base, &prestat);
    size_t lcnt = __BIGINT_LIMBS_NEEDED__(__BITCOUNT___(digit, in->base));
    if (prestat != STR_SUCCESS) {
        limb_t *tmp_limb = (limb_t*)vctx;
        bigInt temp = { .limbs = tmp_limb, .n = 0, .sign = 1, .cap = (lcnt) ? lcnt : 1 };
        if (digit) bigInt_get_strnb(&temp, in->str, digit, in->base);
        out->data.bi = temp;
    } else {
        limb_t *tmp_limb = (limb_t*)vctx;
        bigInt temp = { .limbs = tmp_limb, .n = 0, .sign = 1, .cap = lcnt };
        bigInt_get_strnb(&temp, in->str, in->len, in->base); out->data.bi = temp;
    } out->status = prestat;
}
// Truncative Assignments - tget_str - TRUNCATE
static inline void exec_stobi_tget_str(const void *vin, str_res *out, void *vctx) {
    const stobi_assign_in *in = vin;
    out->type = BIGINT;
    limb_t *tmp_limbs = (limb_t*)vctx;
    bigInt tmp = { .limbs = tmp_limbs, .n = 0, .cap = in->bi_size, .sign = 1 };
    out->status = bigInt_tget_str(&tmp, in->str); out->data.bi = tmp;
}
static inline void exec_stobi_tget_strb(const void *vin, str_res *out, void *vctx) {
    const stobi_assign_in *in = vin;
    out->type = BIGINT;
    limb_t *tmp_limbs = (limb_t*)vctx;
    bigInt tmp = { .limbs = tmp_limbs, .n = 0, .cap = in->bi_size, .sign = 1 };
    out->status = bigInt_tget_strb(&tmp, in->str, in->base); out->data.bi = tmp;
}
static inline void exec_stobi_tget_strn(const void *vin, str_res *out, void *vctx) {
    const stobi_assign_in *in = vin;
    out->type = BIGINT;
    limb_t *tmp_limbs = (limb_t*)vctx;
    bigInt tmp = { .limbs = tmp_limbs, .n = 0, .cap = in->bi_size, .sign = 1 };
    out->status = bigInt_tget_strn(&tmp, in->str, in->len); out->data.bi = tmp;
}
static inline void exec_stobi_tget_strnb(const void *vin, str_res *out, void *vctx) {
    const stobi_assign_in *in = vin;
    out->type = BIGINT;
    limb_t *tmp_limbs = (limb_t*)vctx;
    bigInt tmp = { .limbs = tmp_limbs, .n = 0, .cap = in->bi_size, .sign = 1 };
    out->status = bigInt_tget_strnb(&tmp, in->str, in->len, in->base); out->data.bi = tmp;
}
// Safe / Strict Assignments - sget_str - RETURNS AN ERROR
static inline void exec_stobi_sget_str(const void *vin, str_res *out, void *vctx) {
    const stobi_assign_in *in = vin;
    out->type = BIGINT;
    limb_t *tmp_limbs = (limb_t*)vctx;
    bigInt tmp = { .limbs = tmp_limbs, .n = 0, .cap = in->bi_size, .sign = 1 };
    out->status = bigInt_sget_str(&tmp, in->str); out->data.bi = tmp;
}
static inline void exec_stobi_sget_strb(const void *vin, str_res *out, void *vctx) {
    const stobi_assign_in *in = vin;
    out->type = BIGINT;
    limb_t *tmp_limbs = (limb_t*)vctx;
    bigInt tmp = { .limbs = tmp_limbs, .n = 0, .cap = in->bi_size, .sign = 1 };
    out->status = bigInt_sget_strb(&tmp, in->str, in->base); out->data.bi = tmp;
}
static inline void exec_stobi_sget_strn(const void *vin, str_res *out, void *vctx) {
    const stobi_assign_in *in = vin;
    out->type = BIGINT;
    limb_t *tmp_limbs = (limb_t*)vctx;
    bigInt tmp = { .limbs = tmp_limbs, .n = 0, .cap = in->bi_size, .sign = 1 };
    out->status = bigInt_sget_strn(&tmp, in->str, in->len); out->data.bi = tmp;
}
static inline void exec_stobi_sget_strnb(const void *vin, str_res *out, void *vctx) {
    const stobi_assign_in *in = vin;
    out->type = BIGINT;
    limb_t *tmp_limbs = (limb_t*)vctx;
    bigInt tmp = { .limbs = tmp_limbs, .n = 0, .cap = in->bi_size, .sign = 1 };
    out->status = bigInt_sget_strnb(&tmp, in->str, in->len, in->base); out->data.bi = tmp;
}
// Stream-based Input Scanning - fscan
static inline void exec_stobi_fscan(const void *vin, str_res *out, void *vctx) {
    const stobi_scan_in *in = vin;
    out->type = BIGINT; uint8_t base = 10; dnml_status prestat;
    size_t digits = bigInt_fscan_size(in->stream, &base, &prestat);
    size_t lcnt = __BIGINT_LIMBS_NEEDED__(__BITCOUNT___(digits, base));
    if (prestat != STR_SUCCESS) {
        limb_t *tmp_limbs = (limb_t*)vctx;
        bigInt tmp = { .limbs = tmp_limbs, .n = 0, .sign = 1, .cap = 1};
        tmp.limbs[0] = 0;
    } else {
        limb_t *tmp_limb = (limb_t*)vctx;
        bigInt temp = { .limbs = tmp_limb, .n = 0, .sign = 1, .cap = lcnt };
        bigInt_fscan(in->stream, &temp); out->data.bi = temp;
    } out->status = prestat;
}
static inline void exec_stobi_fscanb(const void *vin, str_res *out, void *vctx) {
    const stobi_scan_in *in = vin;
    out->type = BIGINT; dnml_status prestat;
    size_t digits = bigInt_fscanb_size(in->stream, in->base, &prestat);
    size_t lcnt = __BIGINT_LIMBS_NEEDED__(__BITCOUNT___(digits, in->base));
    if (prestat != STR_SUCCESS) {
        limb_t *tmp_limbs = (limb_t*)vctx;
        bigInt tmp = { .limbs = tmp_limbs, .n = 0, .sign = 1, .cap = 1};
        tmp.limbs[0] = 0;
    } else {
        limb_t *tmp_limb = (limb_t*)vctx;
        bigInt temp = { .limbs = tmp_limb, .n = 0, .sign = 1, .cap = lcnt };
        bigInt_fscanb(in->stream, &temp, in->base); out->data.bi = temp;
    } out->status = prestat;
}
static inline void exec_stobi_fsscan(const void *vin, str_res *out, void *vctx) {
    const stobi_scan_in *in = vin; out->type = BIGINT;
    limb_t *tmp_limbs = (limb_t*)vctx;
    bigInt tmp = { .limbs = tmp_limbs, .n = 0, .cap = in->bi_size, .sign = 1 };
    out->status = bigInt_fsscan(in->stream, &tmp); out->data.bi = tmp;
}
static inline void exec_stobi_fsscanb(const void *vin, str_res *out, void *vctx) {
    const stobi_scan_in *in = vin; out->type = BIGINT;
    limb_t *tmp_limbs = (limb_t*)vctx;
    bigInt tmp = { .limbs = tmp_limbs, .n = 0, .cap = in->bi_size, .sign = 1 };
    out->status = bigInt_fsscanb(in->stream, &tmp, in->base); out->data.bi = tmp;
}
static inline void exec_stobi_ftscan(const void *vin, str_res *out, void *vctx) {
    const stobi_scan_in *in = vin; out->type = BIGINT;
    limb_t *tmp_limbs = (limb_t*)vctx;
    bigInt tmp = { .limbs = tmp_limbs, .n = 0, .cap = in->bi_size, .sign = 1 };
    out->status = bigInt_ftscan(in->stream, &tmp); out->data.bi = tmp;
}
static inline void exec_stobi_ftscanb(const void *vin, str_res *out, void *vctx) {
    const stobi_scan_in *in = vin; out->type = BIGINT;
    limb_t *tmp_limbs = (limb_t*)vctx;
    bigInt tmp = { .limbs = tmp_limbs, .n = 0, .cap = in->bi_size, .sign = 1 };
    out->status = bigInt_ftscanb(in->stream, &tmp, in->base); out->data.bi = tmp;
}
// Stream-based Raw Input - fread
static inline void exec_stobi_fread(const void *vin, str_res *out, void *vctx) { DNML_UNFINISHED(); }
static inline void exec_stobi_deserialize(const void *vin, str_res *out, void *vctx) { DNML_UNFINISHED(); }


#endif