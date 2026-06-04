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



#ifndef _BIGINT_INV_FUNC_H
#define _BIGINT_INV_FUNC_H


#include <include.h>
#include <libdnml_types.h>
#include "../../../test_ui/_strui.h"
#include "../../../dynamol/bigint/bigInt_func.h"
#include "../_ioconv.h"
#include "bi_indef.h"


/* Note 1 - Inverses:
*   +) A rule of thumb for inverses functions are: 
*      - Function with deterministic and self-contained base, which that includes 
*        purely base-10 or with base-prefix, goes also with inverses function that
*        accepts and scans for self-contained base
*
*      - Eg 01: bigInt_to_strf() outputs a string with a base-prefix, meaning its bases are
*               deterministic and self-contained. Therefore, it inverses are bigInt_get_strn(),
*               where it scans for base-prefixes
*
*      - Eg 02: bigInt_to_str() outputs a purely base-10 decimal with no base-prefix, meaning its bases
*               are deterministic. Therefore, bigInt_get_strn() is also its inverse due to the function's
*               interpreting the string as base-10 by default if no base-prefix were detected
*
*      - Eg 03: bigInt_to_strb() outputs an arbitrary base string with no base-prefix, meaning its bases
*               are indeterministic, and therefore needs further clarifying from user's input. Therefore,
*               bigInt_get_strnb() is its inverses due to it not needing to scan for base-prefix, and
*               outputs a bigInt based on the user's base input

*   +) Lossless functions, functions in which full input information
*      might be lost, will NOT have an inverse, for the input
*      is unrecoverable autonomously, and therefore, 
*      such functions resort to EVALUATION (see Note 2)
*      ------> Truncative Variants do NOT have inverses
*/


/* Note 2 - Evaluation:
*   +) Each evaluation function are semantic/format based
*      ---> Each evaluation function fits one format of output
*   +) Example: tto_str & tto_strn has output of purely base-10 with no base-prefix
*      -------> We use to_strn as the full output evaluator since
*               its output is also purely base-19 with no base-prefix
*/

// Any settings of other metadata outside of out->status are for safety measures
#define BITOS_INVALCAP(out) do { \
    out->status = STR_INVALID_CAP;              \
    out->type = STRING;                        \
    out->data.len = 0;                          \
    out->pstr[0] = '\0'; out->cap = 0;           \
} while (0);
#define BITOS_INVALBASE(out) do { \
    out->status = STR_INVALID_BASE;             \
    out->type = STRING;                        \
    out->data.len = 0;                          \
    out->pstr[0] = '\0'; out->cap = 0;           \
} while (0);
#define BITOS_SUCCESS(out) do { \
    out->status = STR_SUCCESS;                  \
    out->type = STRING;                        \
    out->data.len = 0;                          \
    out->pstr[0] = '\0'; out->cap = 0;           \
} while (0);


//* ========================= BITOS EVALUATION WRAPPERS ======================= *//
// BITOS Status Checkers
stinl bool stat_bitos_print(cvoid *vin, dnml_status res_status, str_res *out) { 
    return (res_status == STR_SUCCESS); 
}
stinl bool stat_bitos_conv_nob(cvoid *vin, dnml_status res_status, str_res *out) {
    bitos_conv_in *in = (bitos_conv_in*)vin;
    uint8_t sign_space = (in->x.sign == -1) ? 1 : 0;
    if (in->len <= sign_space) { BITOS_INVALCAP(out); return (res_status == STR_INVALID_CAP); }
    size_t digit_needed = __BIGINT_COUNTDB__(&in->x, 10);
    if (in->len < digit_needed + sign_space) { BITOS_INVALCAP(out); return (res_status == STR_INVALID_CAP); }
    BITOS_SUCCESS(out); return (res_status == STR_INVALID_CAP); 
}
stinl bool stat_bitos_conv_b(cvoid *vin, dnml_status res_status, str_res *out) {
    bitos_conv_in *in = (bitos_conv_in*)vin;
    if (!in->base) { BITOS_INVALBASE(out); return (res_status == STR_INVALID_BASE); }
    uint8_t sign_space = (in->x.sign == -1) ? 1 : 0;
    if (in->len <= sign_space) { BITOS_INVALCAP(out); return (res_status == STR_INVALID_CAP); }
    size_t digit_needed = __BIGINT_COUNTDB__(&in->x, in->base);
    if (in->len < digit_needed + sign_space) { BITOS_INVALCAP(out); return (res_status == STR_INVALID_CAP); }
    BITOS_SUCCESS(out); return (res_status == STR_INVALID_CAP); 
}
stinl bool stat_bitos_conv_f(cvoid *vin, dnml_status res_status, str_res *out) {
    bitos_conv_in *in = (bitos_conv_in*)vin;
    if (!in->base) { BITOS_INVALBASE(out); return (res_status == STR_INVALID_BASE); }
    uint8_t sign_space = (in->x.sign == -1) ? 1 : 0,
    prefix_space = (in->base == 2 || in->base == 8 || in->base == 16) ? 2 : 0;
    if (in->len <= sign_space + prefix_space) { BITOS_INVALCAP(out); return (res_status == STR_INVALID_CAP); }
    size_t digit_needed = __BIGINT_COUNTDB__(&in->x, in->base);
    if (in->len < digit_needed + sign_space + prefix_space) { 
        BITOS_INVALCAP(out);
        return (res_status == STR_INVALID_CAP);
    } BITOS_SUCCESS(out);
    return (res_status == STR_SUCCESS);
}
stinl bool stat_bitos_tconv_nob(cvoid *vin, dnml_status res_status, str_res *out) {
    bitos_conv_in *in = (bitos_conv_in*)vin;
    uint8_t sign_space = (in->x.sign == -1) ? 1 : 0;
    if (in->len <= sign_space) { BITOS_INVALCAP(out); return (res_status == STR_INVALID_CAP); }
    BITOS_SUCCESS(out); return (res_status == STR_SUCCESS);
}
stinl bool stat_bitos_tconv_b(cvoid *vin, dnml_status res_status, str_res *out) {
    bitos_conv_in *in = (bitos_conv_in*)vin;
    if (!in->base) { BITOS_INVALBASE(out); return (res_status == STR_INVALID_BASE); }
    uint8_t sign_space = (in->x.sign == -1) ? 1 : 0;
    if (in->len <= sign_space) { BITOS_INVALCAP(out); return (res_status == STR_INVALID_CAP); }
    BITOS_SUCCESS(out); return (res_status == STR_SUCCESS);
}
// BITOS Conversions Inverses & Evaluators
stinl void inv_bitos_conv_nob(cvoid *vin, csres *out, void *recon, void *vctx) {
    size_t bcount = __BITCOUNT___(out->data.len, 10);
    size_t lcnt = __BIGINT_LIMBS_NEEDED__(bcount);
    limb_t *tmp_limbs = (limb_t*)vctx;
    bigInt tmp = { .limbs = tmp_limbs, .n = 0, .sign = 1, .cap = lcnt };
    if (bigInt_get_strn(&tmp, out->pstr, out->data.len) == BIGINT_ERR_RANGE) {
        fputs("Testing allocation / internal processes gone wrong\nTerminating session...", stderr);
        abort();
    } *(bigInt*)recon = tmp;
}
stinl void inv_bitos_conv_b(cvoid *vin, csres *out, void *recon, void *vctx) {
    bitos_conv_in *in = (bitos_conv_in*)vin;
    size_t bcount = __BITCOUNT___(out->data.len, 10);
    size_t lcnt = __BIGINT_LIMBS_NEEDED__(bcount);
    limb_t *tmp_limbs = (limb_t*)vctx;
    bigInt tmp = { .limbs = tmp_limbs, .n = 0, .sign = 1, .cap = lcnt };
    if (bigInt_get_strnb(&tmp, out->pstr, out->data.len, in->base) == BIGINT_ERR_RANGE) {
        fputs("Testing allocation / internal processes gone wrong\nTerminating session...", stderr);
        abort();
    } *(bigInt*)recon = tmp;
}
stinl void eval_bitos_tto_str(cvoid *vin, str_res *exp, void *vctx) {
    // For: tto_str, tto_strn
    const bitos_conv_in *in = (bitos_conv_in*)in;
    size_t needed = __BIGINT_COUNTDB__(&in->x, 10);
    exp->status = bigInt_to_strn(exp->pstr, needed + 1, in->x, &needed);
    exp->data.len = needed; exp->type = STRING;
}
stinl void eval_bitos_tto_strb(cvoid *vin, str_res *exp, void *vctx) {
    // For: tto_strb, tto_strnb
    const bitos_conv_in *in = (bitos_conv_in*)in;
    size_t needed = __BIGINT_COUNTDB__(&in->x, in->base);
    exp->status = bigInt_to_strnb(
        exp->pstr, needed + 1, in->x, 
        in->base, &needed
    ); exp->data.len = needed; exp->type = STRING;
}
stinl void eval_bitos_tto_strf(cvoid *vin, str_res *exp, void *vctx) {
    // For: tto_strf
    const bitos_conv_in *in = (bitos_conv_in*)in;
    size_t needed = __BIGINT_COUNTDB__(&in->x, in->base);
    exp->status = bigInt_to_strf(
        exp->pstr, needed + 1, 
        in->x, in->base, 
        in->uppercase, &needed
    ); exp->data.len = needed; exp->type = STRING;
}
// BITOS Printing & Raw Output Inverses
stinl void inv_bitos_fput_nob(cvoid *vin, csres *out, void *recon, void *vctx) {
    size_t bcount = __BITCOUNT___(out->data.len, 10);
    size_t lcnt = __BIGINT_LIMBS_NEEDED__(bcount);
    limb_t *tmp_limbs = (limb_t*)vctx;
    bigInt tmp = { .limbs = tmp_limbs, .n = 0, .sign = 1, .cap = lcnt };
    if (bigInt_get_strn(&tmp, out->pstr, out->data.len) == BIGINT_ERR_RANGE) {
        fputs("Testing allocation / internal processes gone wrong\nTerminating session...", stderr);
        abort();
    } *(bigInt*)recon = tmp;
}
stinl void inv_bitos_fput_b(cvoid *vin, csres *out, void *recon, void *vctx) {
    bitos_print_in *in = (bitos_print_in*)vin;
    size_t bcount = __BITCOUNT___(out->data.len, in->base);
    size_t lcnt = __BIGINT_LIMBS_NEEDED__(bcount);
    limb_t *tmp_limbs = (limb_t*)vctx;
    bigInt tmp = { .limbs = tmp_limbs, .n = 0, .sign = 1, .cap = lcnt };
    if (bigInt_get_strnb(&tmp, out->pstr, out->data.len, in->base) == BIGINT_ERR_RANGE) {
        fputs("Testing allocation / internal processes gone wrong\nTerminating session...", stderr);
        abort();
    } *(bigInt*)recon = tmp;
}
stinl void inv_bitos_serial(cvoid *vin, csres *out, void *recon, void *vctx) { DNML_UNFINISHED(); }
stinl void eval_bitos_tserialize(cvoid *vin, str_res *exp, void *vctx) { DNML_UNFINISHED(); }


//* ========================= STOBI EVALUATION WRAPPERS ======================= *//
// STOBI Initialization Inverses
stinl void inv_stobi_init_nob(cvoid *vin, csres *out, void *recon, void *vctx) {
    // for bigInt_strint & bigInt_strninit size_t tmp = 0,
    size_t tmp = 0, len = __BIGINT_COUNTDB__(&out->data.bi, 10);
    char* idk = (char*)vctx;
    if (bigInt_to_strn(idk, len, out->data.bi, &tmp) == STR_INVALID_CAP) {
        fputs("Testing allocation / internal processes gone wrong\nTerminating session...", stderr);
        abort();
    } *(char**)recon = idk;
}
stinl void inv_stobi_init_b(cvoid *vin, csres *out, void *recon, void *vctx) {
    // for bigInt_strbint & bigInt_strnbinit
    stobi_init_in *in = (stobi_init_in*)vin; size_t tmp = 0,
    len = __BIGINT_COUNTDB__(&out->data.bi, 10);
    char* idk = (char*)vctx;
    if (bigInt_to_strnb(idk, len, out->data.bi, in->base, &tmp) == STR_INVALID_CAP) {
        fputs("Testing allocation / internal processes gone wrong\nTerminating session...", stderr);
        abort();
    } *(char**)recon = idk;
}
// STOBI Conversion Inverses
stinl void inv_stobi_conv_nob(cvoid *vin, csres *out, void *recon, void *vctx) {
    // for bigInt_from_str & bigInt_from_strn size_t tmp = 0,
    size_t tmp = 0, len = __BIGINT_COUNTDB__(&out->data.bi, 10);
    char* idk = (char*)vctx;
    if (bigInt_to_strn(idk, len, out->data.bi, &tmp) == STR_INVALID_CAP) {
        fputs("Testing allocation / internal processes gone wrong\nTerminating session...", stderr);
        abort();
    } *(char**)recon = idk;
}
stinl void inv_stobi_conv_b(cvoid *vin, csres *out, void *recon, void *vctx) {
    // for bigInt_from_strb & bigInt_from_strnb
    stobi_conv_in *in = (stobi_conv_in*)vin; size_t tmp = 0,
    len = __BIGINT_COUNTDB__(&out->data.bi, 10);
    char* idk = (char*)vctx;
    if (bigInt_to_strnb(idk, len, out->data.bi, in->base, &tmp) == STR_INVALID_CAP) {
        fputs("Testing allocation / internal processes gone wrong\nTerminating session...", stderr);
        abort();
    } *(char**)recon = idk;
}
// STOBI Assignment Inverses & Evaluators
stinl void inv_stobi_assign_nob(cvoid *vin, csres *out, void *recon, void *vctx) {
    // for:
    //  - GROWS: bigInt_get_str & bigInt_get_strn
    //  - SAFE: bigInt_sget_str & bigInt_sget_strn size_t tmp = 0,
    size_t tmp = 0, len = __BIGINT_COUNTDB__(&out->data.bi, 10);
    char* idk = (char*)vctx;
    if (bigInt_to_strn(idk, len, out->data.bi, &tmp) == STR_INVALID_CAP) {
        fputs("Testing allocation / internal processes gone wrong\nTerminating session...", stderr);
        abort();
    } *(char**)recon = idk;
}
stinl void inv_stobi_assign_b(cvoid *vin, csres *out, void *recon, void *vctx) {
    // for:
    //  - GROWS: bigInt_get_strb & bigInt_get_strnb
    //  - SAFE: bigInt_sget_strb & bigInt_sget_strnb
    stobi_assign_in *in = (stobi_assign_in*)vin; size_t tmp = 0,
    len = __BIGINT_COUNTDB__(&out->data.bi, 10);
    char* idk = (char*)vctx;
    if (bigInt_to_strnb(idk, len, out->data.bi, in->base, &tmp) == STR_INVALID_CAP) {
        fputs("Testing allocation / internal processes gone wrong\nTerminating session...", stderr);
        abort();
    } *(char**)recon = idk;
}
stinl void eval_stobi_tget_str(cvoid *vin, str_res *exp, void *vctx) {
    // For bigInt_tget_str() & bigInt_tget_strn()
    const stobi_assign_in *in = (stobi_assign_in*)in; uint8_t base = 10;
    size_t cap_lim = __BIGINT_MAXCDB__(in->bi_size, 10),
    finval_i = _finval_char(in->str, in->len, &base);
    size_t valid_len = min(cap_lim, finval_i);

    size_t lcnt = __BIGINT_LIMBS_NEEDED__(__BITCOUNT___(valid_len, base));
    limb_t *tmp_limbs = (limb_t*)vctx;
    exp->data.bi.limbs = tmp_limbs; exp->data.bi.sign = 1;
    exp->data.bi.n = 0; exp->data.bi.cap = lcnt;
    dnml_status fn_stat = bigInt_get_strn(&exp->data.bi, in->str, valid_len);

    // Explicit status returns (Precendence-accounted)
    exp->status = (finval_i < cap_lim) ? fn_stat : 
                 ((cap_lim < finval_i) ? STR_TRUNC_SUCCESS : STR_SUCCESS);
    exp->type = BIGINT;
}
stinl void eval_stobi_tget_strb(cvoid *vin, str_res *exp, void *vctx) {
    // For bigInt_tget_str() & bigInt_tget_strn()
    // For bigInt_tget_str() & bigInt_tget_strn()
    const stobi_assign_in *in = (stobi_assign_in*)in;
    size_t cap_lim = __BIGINT_MAXCDB__(in->bi_size, in->base),
    finval_i = _finval_charb(in->str, in->len, in->base);
    size_t valid_len = min(cap_lim, finval_i);

    size_t lcnt = __BIGINT_LIMBS_NEEDED__(__BITCOUNT___(valid_len, in->base));
    limb_t *tmp_limbs = (limb_t*)vctx;
    exp->data.bi.limbs = tmp_limbs; exp->data.bi.sign = 1;
    exp->data.bi.n = 0; exp->data.bi.cap = lcnt;
    dnml_status fn_stat = bigInt_get_strnb(&exp->data.bi, in->str, valid_len, in->base);

    // Explicit status returns (Precendence-accounted)
    exp->status = (finval_i < cap_lim) ? fn_stat : 
                 ((cap_lim < finval_i) ? STR_TRUNC_SUCCESS : STR_SUCCESS);
    exp->type = BIGINT;
}
// STOBI Scanning Inversese & Evaluators
stinl void inv_stobi_scan_nob(cvoid *vin, csres *out, void *recon, void *vctx) {
    // for: bigInt_fscan & bigInt_fsscan size_t tmp = 0,
    size_t tmp, len = __BIGINT_COUNTDB__(&out->data.bi, 10);
    char* idk = (char*)vctx;
    if (bigInt_to_strn(idk, len, out->data.bi, &tmp) == STR_INVALID_CAP) {
        fputs("Testing allocation / internal processes gone wrong\nTerminating session...", stderr);
        abort();
    } *(char**)recon = idk;
}
stinl void inv_stobi_scan_b(cvoid *vin, csres *out, void *recon, void *vctx) {
    // for: bigInt_fscanb & bigInt_fsscanb
    stobi_scan_in *in = (stobi_scan_in*)vin; size_t tmp = 0,
    len = __BIGINT_COUNTDB__(&out->data.bi, 10);
    char* idk = (char*)vctx;
    if (bigInt_to_strnb(idk, len, out->data.bi, in->base, &tmp) == STR_INVALID_CAP) {
        fputs("Testing allocation / internal processes gone wrong\nTerminating session...", stderr);
        abort();
    } *(char**)recon = idk;
}
stinl void eval_stobi_ftscan(cvoid *vin, str_res *exp, void *vctx) {
    // For bigInt_ftscan()
    const stobi_scan_in *in = (const stobi_scan_in*)in;
    char buf[STR_CAP] = {0};
    rewind(in->stream);

    size_t len = fread(buf, sizeof(char), STR_CAP, in->stream);
    uint8_t base = 10;
    size_t finval_i = _finval_char(buf, len, &base);
    size_t cap_lim = __BIGINT_MAXCDB__(in->bi_size, base);
    size_t valid_len = (finval_i < cap_lim) ? finval_i : cap_lim;

    size_t lcnt = __BIGINT_LIMBS_NEEDED__(__BITCOUNT___(len + 10, base));
    limb_t *tmp_limbs = (limb_t*)vctx;
    exp->data.bi.limbs = tmp_limbs; exp->data.bi.sign = 1;
    exp->data.bi.n = 0; exp->data.bi.cap = lcnt;
    dnml_status fn_stat = bigInt_get_strn(&exp->data.bi, buf, valid_len);
    exp->status = (finval_i < cap_lim) ? fn_stat :
                 ((cap_lim < finval_i) ? STR_TRUNC_SUCCESS : STR_SUCCESS);
    exp->type = BIGINT; 
}
stinl void eval_stobi_ftscanb(cvoid *vin, str_res *exp, void *vctx) {
    // For bigInt_ftscanb()
    const stobi_scan_in *in = (stobi_scan_in*)in;
    char buf[STR_CAP] = {0};
    rewind(in->stream);

    size_t len = fread(buf, sizeof(char), STR_CAP, in->stream);
    size_t finval_i = _finval_charb(buf, len, in->base),
    cap_lim = __BIGINT_MAXCDB__(in->bi_size, in->base);
    size_t valid_len = (finval_i < cap_lim) ? finval_i : cap_lim;

    size_t lcnt = __BIGINT_LIMBS_NEEDED__(__BITCOUNT___(len + 10, in->base));
    limb_t *tmp_limbs = (limb_t*)vctx;
    exp->data.bi.limbs = tmp_limbs; exp->data.bi.sign = 1;
    exp->data.bi.n = 0; exp->data.bi.cap = lcnt;
    dnml_status fn_stat = bigInt_get_strnb(&exp->data.bi, buf, valid_len, in->base);
    exp->status = (finval_i < cap_lim) ? fn_stat :
                 ((cap_lim < finval_i) ? STR_TRUNC_SUCCESS : STR_SUCCESS);
    exp->type = BIGINT; 
}
// STOBI Raw / Serial Input Inverses & Evaluators
stinl void inv_stobi_fread(cvoid *vin, csres *out, void *recon, void *vctx) { DNML_UNFINISHED(); }
stinl void inv_stobi_deserial(cvoid *vin, csres *out, void *recon, void *vctx) { DNML_UNFINISHED(); }
stinl void eval_stobi_ftread(cvoid *vin, str_res *exp, void *vctx) { DNML_UNFINISHED(); }


// Any settings of other metadata outside of out->status are for safety measures
#define STOBI_STAT_SETOUT(out, received_status) do { \
    out->status = received_status;      \
    out->type = BIGINT;                \
    out->data.bi.limbs = NULL;          \
    out->data.bi.sign = 0;              \
    out->data.bi.cap = 0;               \
    out->data.bi.n = 1;                 \
    out->pstr[0] = '\0'; out->cap = 0;   \
} while (0);

//* ========================= STOBI STATUS CHECKING WRAPPERS ======================= *//
// STOBI_INIT Status Checkers
stinl bool stat_stobi_strinit(cvoid *vin, dnml_status res_status, str_res *out) {
    stobi_init_in *in = (stobi_init_in*)vin;
    dnml_status exp; uint8_t base = 0;
    bigInt_get_size(in->str, strlen(in->str), &base, &exp);
    STOBI_STAT_SETOUT(out, exp);
    return (res_status == exp);
}
stinl bool stat_stobi_strbinit(cvoid *vin, dnml_status res_status, str_res *out) {
    stobi_init_in *in = (stobi_init_in*)vin;
    dnml_status exp; uint8_t base = 0;
    bigInt_get_sizeb(in->str, strlen(in->str), in->base, &exp);
    STOBI_STAT_SETOUT(out, exp);
    return (res_status == exp);
}
stinl bool stat_stobi_strninit(cvoid *vin, dnml_status res_status, str_res *out) {
    stobi_init_in *in = (stobi_init_in*)vin;
    dnml_status exp; uint8_t base = 0;
    bigInt_get_size(in->str, in->len, &base, &exp);
    STOBI_STAT_SETOUT(out, exp);
    return (res_status == exp);
}
stinl bool stat_stobi_strnbinit(cvoid *vin, dnml_status res_status, str_res *out) {
    stobi_init_in *in = (stobi_init_in*)vin;
    dnml_status exp; uint8_t base = 0;
    bigInt_get_sizeb(in->str, in->len, in->base, &exp);
    STOBI_STAT_SETOUT(out, exp);
    return (res_status == exp);
}
// STOBI_CONV Status Checkers
stinl bool stat_stobi_from_str(cvoid *vin, dnml_status res_status, str_res *out) {
    stobi_conv_in *in = (stobi_conv_in*)vin;
    dnml_status exp; uint8_t base = 0;
    bigInt_get_size(in->str, strlen(in->str), &base, &exp);
    STOBI_STAT_SETOUT(out, exp);
    return (res_status == exp);
}
stinl bool stat_stobi_from_strb(cvoid *vin, dnml_status res_status, str_res *out) {
    stobi_conv_in *in = (stobi_conv_in*)vin;
    dnml_status exp; uint8_t base = 0;
    bigInt_get_sizeb(in->str, strlen(in->str), in->base, &exp);
    STOBI_STAT_SETOUT(out, exp);
    return (res_status == exp);
}
stinl bool stat_stobi_from_strn(cvoid *vin, dnml_status res_status, str_res *out) {
    stobi_conv_in *in = (stobi_conv_in*)vin;
    dnml_status exp; uint8_t base = 0;
    bigInt_get_size(in->str, in->len, &base, &exp);
    STOBI_STAT_SETOUT(out, exp);
    return (res_status == exp);
}
stinl bool stat_stobi_from_strnb(cvoid *vin, dnml_status res_status, str_res *out) {
    stobi_conv_in *in = (stobi_conv_in*)vin;
    dnml_status exp; uint8_t base = 0;
    bigInt_get_sizeb(in->str, in->len, in->base, &exp);
    STOBI_STAT_SETOUT(out, exp);
    return (res_status == exp);
}
// STOBI_ASSIGN Status Checkers
/* Subnote 1:
    - stat_stobi_get_str and its subsequent variants are used for both:
        +) get_str (and its subsequent variants) - GROWTH API
        +) tget_str (and its subsequent variants) - TRUNCATIVE API
    - stat_stobi_sget_str and its subsequent variants are only used for,
      as the name suggest:
        +) sget_Str (and its subsequent variants) - SAFE/STRICT API
*/
stinl bool stat_stobi_get_str(cvoid *vin, dnml_status res_status, str_res *out) {
    stobi_assign_in *in = (stobi_assign_in*)vin;
    dnml_status exp; uint8_t base = 0;
    bigInt_get_size(in->str, strlen(in->str), &base, &exp);
    STOBI_STAT_SETOUT(out, exp);
    return (res_status == exp);
}
stinl bool stat_stobi_get_strb(cvoid *vin, dnml_status res_status, str_res *out) {
    stobi_assign_in *in = (stobi_assign_in*)vin;
    dnml_status exp; uint8_t base = 0;
    bigInt_get_sizeb(in->str, strlen(in->str), in->base, &exp);
    STOBI_STAT_SETOUT(out, exp);
    return (res_status == exp);
}
stinl bool stat_stobi_get_strn(cvoid *vin, dnml_status res_status, str_res *out) {
    stobi_assign_in *in = (stobi_assign_in*)vin;
    dnml_status exp; uint8_t base = 0;
    bigInt_get_size(in->str, in->len, &base, &exp);
    STOBI_STAT_SETOUT(out, exp);
    return (res_status == exp);
}
stinl bool stat_stobi_get_strnb(cvoid *vin, dnml_status res_status, str_res *out) {
    stobi_assign_in *in = (stobi_assign_in*)vin;
    dnml_status exp; uint8_t base = 0;
    bigInt_get_sizeb(in->str, in->len, in->base, &exp);
    STOBI_STAT_SETOUT(out, exp);
    return (res_status == exp);
}
stinl bool stat_stobi_sget_str(cvoid *vin, dnml_status res_status, str_res *out) {
    stobi_assign_in *in = (stobi_assign_in*)vin;
    dnml_status exp; uint8_t base = 0;
    bigInt_get_sizesa(in->str, strlen(in->str), &base, in->bi_size, &exp);
    STOBI_STAT_SETOUT(out, exp);
    return (res_status == exp);
}
stinl bool stat_stobi_sget_strb(cvoid *vin, dnml_status res_status, str_res *out) {
    stobi_assign_in *in = (stobi_assign_in*)vin; dnml_status exp;
    bigInt_get_sizebsa(in->str, strlen(in->str), in->base, in->bi_size, &exp);
    STOBI_STAT_SETOUT(out, exp);
    return (res_status == exp);
}
stinl bool stat_stobi_sget_strn(cvoid *vin, dnml_status res_status, str_res *out) {
    stobi_assign_in *in = (stobi_assign_in*)vin;
    dnml_status exp; uint8_t base = 0;
    bigInt_get_sizesa(in->str, in->len, &base, in->bi_size, &exp);
    STOBI_STAT_SETOUT(out, exp);
    return (res_status == exp);
}
stinl bool stat_stobi_sget_strnb(cvoid *vin, dnml_status res_status, str_res *out) {
    stobi_assign_in *in = (stobi_assign_in*)vin; dnml_status exp;
    bigInt_get_sizebsa(in->str, in->len, in->base, in->bi_size, &exp);
    STOBI_STAT_SETOUT(out, exp);
    return (res_status == exp);
}
// STOBI_SCAN Status Checkers
/* Subnote 1:
    +) nob      = No Base Parameter -----> Parses Base-prefix
    +) b        = Has Base Parameter ----> Use base parameter
    +) nobsa    = nob + Size-awarenes ---> Consider output buffer's size
    +) bsa      = b + Size-awareness ----> Consider output buffer's size
*/
stinl bool stat_stobi_fscan_nob(cvoid *vin, dnml_status res_status, str_res *out) {
    stobi_scan_in *in = (stobi_scan_in*)vin;
    dnml_status exp; uint8_t base = 0;
    bigInt_fscan_size(in->stream, &base, &exp);
    STOBI_STAT_SETOUT(out, exp);
    return (res_status == exp);
}
stinl bool stat_stobi_fscan_b(cvoid *vin, dnml_status res_status, str_res *out) {
    stobi_scan_in *in = (stobi_scan_in*)vin; dnml_status exp;
    bigInt_fscanb_size(in->stream, in->base, &exp);
    STOBI_STAT_SETOUT(out, exp);
    return (res_status == exp);
}
stinl bool stat_stobi_fscan_nobsa(cvoid *vin, dnml_status res_status, str_res *out) {
    stobi_scan_in *in = (stobi_scan_in*)vin;
    dnml_status exp; uint8_t base = 0;
    bigInt_fscansa_size(in->stream, &base, in->bi_size, &exp);
    STOBI_STAT_SETOUT(out, exp);
    return (res_status == exp);
}
stinl bool stat_stobi_fscan_bsa(cvoid *vin, dnml_status res_status, str_res *out) {
    stobi_scan_in *in = (stobi_scan_in*)vin; dnml_status exp;
    bigInt_fscanbsa_size(in->stream, in->base, in->bi_size, &exp);
    STOBI_STAT_SETOUT(out, exp);
    return (res_status == exp);
}



#endif