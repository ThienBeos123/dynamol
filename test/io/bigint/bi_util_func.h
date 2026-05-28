#ifndef _BIGINT_UTIL_FUNC_H
#define _BIGINT_UTIL_FUNC_H



#include <include.h>
#include "bi_indef.h"
#include <libdnml_types.h>
#include "../../../test_ui/_strui.h"
#include "../_ioconv.h"


void _suite_slices(
    void** slices, void* storage, 
    size_t elements_per_suite, size_t element_size, 
    u8 suite_count
) {
    for (size_t i = 0; i < suite_count; ++i) {
        size_t increments = i * elements_per_suite * element_size;
        slices[i] = (void*)((char*)storage + increments);
    } return;
}


//* ========================= COMPARISONS WRAPPERS ======================= *//
// Invsere Comparator - BITOS
stinl bool cmp_inv_bitos_conv(cvoid *orin, csres *out, cvoid *recon, void *vctx) {
    const bitos_conv_in *orinbi = (bitos_conv_in*)orin;
    const bigInt *reconbi = (bigInt*)recon;
    return (__BIGINT_INTERNAL_COMP__(&orinbi->x, reconbi) == 0);
}
stinl bool cmp_inv_bitos_put(cvoid *orin, csres *out, cvoid *recon, void *vctx) {
    const bitos_print_in *orinbi = (bitos_print_in*)orin;
    const bigInt *reconbi = (bigInt*)recon;
    return (__BIGINT_INTERNAL_COMP__(&orinbi->x, reconbi) == 0);
}
stinl bool cmp_inv_bitos_fwrite(cvoid *orin, csres *out, cvoid *recon, void *vctx) { DNML_UNFINISHED(); } 
stinl bool cmp_inv_bitos_serial(cvoid *orin, csres *out, cvoid *recon, void *vctx) { DNML_UNFINISHED(); } 
// Inverse Comparator - STOBI
stinl bool cmp_inv_stobi_init(cvoid *orin, csres *out, cvoid *recon, void *vctx) {
    const char *recons = (char*)recon;
    size_t bcount = __BITCOUNT___(strlen(recons), 10);
    size_t len = __BIGINT_LIMBS_NEEDED__(bcount);
    limb_t *tmp_limb = (limb_t*)vctx;
    bigInt tmp = { .limbs = tmp_limb, .n = 0, .cap = len, .sign = 1 };
    if (bigInt_sget_strn(&tmp, recons, strlen(recons)) == BIGINT_ERR_RANGE) {
        fputs("Testing allocation / internal processes gone wrong\nTerminating session...", stderr);
        abort();
    } return (__BIGINT_INTERNAL_COMP__(&out->data.bi, &tmp) == 0);
}
stinl bool cmp_inv_stobi_initb(cvoid *orin, csres *out, cvoid *recon, void *vctx) {
    const stobi_init_in *orins = (stobi_init_in*)orin;
    const char *recons = (char*)recon;
    size_t bcount = __BITCOUNT___(strlen(recons), orins->base);
    size_t len = __BIGINT_LIMBS_NEEDED__(bcount);
    limb_t *tmp_limb = (limb_t*)vctx;
    bigInt tmp = { .limbs = tmp_limb, .n = 0, .cap = len, .sign = 1 };
    if (bigInt_sget_strnb(&tmp, recons, strlen(recons), orins->base) == BIGINT_ERR_RANGE) {
        fputs("Testing allocation / internal processes gone wrong\nTerminating session...", stderr);
        abort();
    } return (__BIGINT_INTERNAL_COMP__(&out->data.bi, &tmp) == 0);
}
stinl bool cmp_inv_stobi_conv(cvoid *orin, csres *out, cvoid *recon, void *vctx) {
    const char *recons = (char*)recon;
    size_t bcount = __BITCOUNT___(strlen(recons), 10);
    size_t len = __BIGINT_LIMBS_NEEDED__(bcount);
    limb_t *tmp_limb = (limb_t*)vctx;
    bigInt tmp = { .limbs = tmp_limb, .n = 0, .cap = len, .sign = 1 };
    if (bigInt_sget_strn(&tmp, recons, strlen(recons)) == BIGINT_ERR_RANGE) {
        fputs("Testing allocation / internal processes gone wrong\nTerminating session...", stderr);
        abort();
    } return (__BIGINT_INTERNAL_COMP__(&out->data.bi, &tmp) == 0);
}
stinl bool cmp_inv_stobi_convb(cvoid *orin, csres *out, cvoid *recon, void *vctx) {
    const stobi_conv_in *orins = (stobi_conv_in*)orin;
    const char *recons = (char*)recon;
    size_t bcount = __BITCOUNT___(strlen(recons), orins->base);
    size_t len = __BIGINT_LIMBS_NEEDED__(bcount);
    limb_t *tmp_limb = (limb_t*)vctx;
    bigInt tmp = { .limbs = tmp_limb, .n = 0, .cap = len, .sign = 1 };
    if (bigInt_sget_strnb(&tmp, recons, strlen(recons), orins->base) == BIGINT_ERR_RANGE) {
        fputs("Testing allocation / internal processes gone wrong\nTerminating session...", stderr);
        abort();
    } return (__BIGINT_INTERNAL_COMP__(&out->data.bi, &tmp) == 0);
}
stinl bool cmp_inv_stobi_assign(cvoid *orin, csres *out, cvoid *recon, void *vctx) {
    const char *recons = (char*)recon;
    size_t bcount = __BITCOUNT___(strlen(recons), 10);
    size_t len = __BIGINT_LIMBS_NEEDED__(bcount);
    limb_t *tmp_limb = (limb_t*)vctx;
    bigInt tmp = { .limbs = tmp_limb, .n = 0, .cap = len, .sign = 1 };
    if (bigInt_sget_strn(&tmp, recons, strlen(recons)) == BIGINT_ERR_RANGE) {
        fputs("Testing allocation / internal processes gone wrong\nTerminating session...", stderr);
        abort();
    } return (__BIGINT_INTERNAL_COMP__(&out->data.bi, &tmp) == 0);
}
stinl bool cmp_inv_stobi_assignb(cvoid *orin, csres *out, cvoid *recon, void *vctx) {
    const stobi_assign_in *orins = (stobi_assign_in*)orin;
    const char *recons = (char*)recon;
    size_t bcount = __BITCOUNT___(strlen(recons), orins->base);
    size_t len = __BIGINT_LIMBS_NEEDED__(bcount);
    limb_t *tmp_limb = (limb_t*)vctx;
    bigInt tmp = { .limbs = tmp_limb, .n = 0, .cap = len, .sign = 1 };
    if (bigInt_sget_strnb(&tmp, recons, strlen(recons), orins->base) == BIGINT_ERR_RANGE) {
        fputs("Testing allocation / internal processes gone wrong\nTerminating session...", stderr);
        abort();
    } return (__BIGINT_INTERNAL_COMP__(&out->data.bi, &tmp) == 0);
}
stinl bool cmp_inv_stobi_scan(cvoid *orin, csres *out, cvoid *recon, void *vctx) {
    const char *recons = (char*)recon;
    size_t bcount = __BITCOUNT___(strlen(recons), 10);
    size_t len = __BIGINT_LIMBS_NEEDED__(bcount);
    limb_t *tmp_limb = (limb_t*)vctx;
    bigInt tmp = { .limbs = tmp_limb, .n = 0, .cap = len, .sign = 1 };
    if (bigInt_sget_strn(&tmp, recons, strlen(recons)) == BIGINT_ERR_RANGE) {
        fputs("Testing allocation / internal processes gone wrong\nTerminating session...", stderr);
        abort();
    } return (__BIGINT_INTERNAL_COMP__(&out->data.bi, &tmp) == 0);
}
stinl bool cmp_inv_stobi_scanb(cvoid *orin, csres *out, cvoid *recon, void *vctx) {
    const stobi_scan_in *orins = (stobi_scan_in*)orin;
    const char *recons = (char*)recon;
    size_t bcount = __BITCOUNT___(strlen(recons), orins->base);
    size_t len = __BIGINT_LIMBS_NEEDED__(bcount);
    limb_t *tmp_limb = (limb_t*)vctx;
    bigInt tmp = { .limbs = tmp_limb, .n = 0, .cap = len, .sign = 1 };
    if (bigInt_sget_strnb(&tmp, recons, strlen(recons), orins->base) == BIGINT_ERR_RANGE) {
        fputs("Testing allocation / internal processes gone wrong\nTerminating session...", stderr);
        abort();
    } return (__BIGINT_INTERNAL_COMP__(&out->data.bi, &tmp) == 0);
}
stinl bool cmp_inv_stobi_fread(cvoid *orin, csres *out, cvoid *recon, void *vctx) { DNML_UNFINISHED(); }
stinl bool cmp_inv_stobi_deserial(cvoid *orin, csres *out, cvoid *recon, void *vctx) { DNML_UNFINISHED(); }
// Evaluation Comparator
stinl bool cmp_eval_bitos(csres *exp, csres *res) {
    // Property Check
    if (res->type != exp->type) return false;
    if (res->status != exp->status) return false;
    if (res->status != STR_SUCCESS || res->status != STR_TRUNC_SUCCESS) return true;
    // Main Check
    if (res->data.len == exp->data.len) return strcmp(res->pstr, exp->pstr) == 0;
    else if (res->data.len > exp->data.len) return false;
    else return strncmp(res->pstr, exp->pstr, res->data.len) == 0;
}
stinl bool cmp_eval_stobi(csres *exp, csres *res) {
    if (res->status != exp->status) return false;
    else if (res->status == STR_SUCCESS || res->status == STR_TRUNC_SUCCESS) {
        return (__BIGINT_INTERNAL_COMP__(&res->data.bi, &exp->data.bi));
    } else return true;
}


//* ========================= INVERSE BITOS FORMATTERS ======================= *//
/* Example Format - BITOS:
        - Input: < -- BITOS_PRINT --
            +) Input 1 - <FILE*>: 0x0123456789ABCDEF
            +) Input 2 - <bigInt>: <
                --- Limb Count: 1024 (limbs)
                --- Sign: 1 (Positive +)
                --- Low Limbs: [12345678901234567890, ...]
                --- High Limbs: [..., 12345678901234567890]
            >
            +) Input 3 - <uint8_t>: 16 - (Hexadecimal)
            +) Input 4 - <bool>: NULL - <NON-UTILIZED-PARAMETER>
        >
*/
stinl void fmt_recon_bitos(FILE *f, cvoid *vrecon, int tab_depth) {
    const bigInt *recon = (bigInt*)vrecon;
    _print_bigint(f, recon, tab_depth);
}
// Inverse Formatter - BITOS_CONV
stinl void fmt_in_to_str(FILE *f, cvoid *vin, int tab_depth) {
    const bitos_conv_in *in = (bitos_conv_in*)vin;
    fputs("< -- BITOS_CONV --\n", f);

    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 1 <size_t>:  NULL - <NON-UTILIZED-PARAMETER\n", f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 2 <bigInt>:", f); _print_bigint(f, &in->x, tab_depth); fputc('\n', f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 3 <uint8_t>: NULL - <NON-UTILIZED-PARAMETER\n", f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 4 <bool>:    NULL - <NON-UTILIZED-PARAMETER\n", f);

    for (int i = 0; i < tab_depth; ++i) fputs(TAB, f); 
    fputs("\n>", f);
}
stinl void fmt_in_to_strb(FILE *f, cvoid *vin, int tab_depth) {
    const bitos_conv_in *in = (bitos_conv_in*)vin;
    fputs("< -- BITOS_CONV --\n", f);

    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 1 <size_t>:  NULL - <NON-UTILIZED-PARAMETER\n", f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 2 <bigInt>:", f); _print_bigint(f, &in->x, tab_depth); fputc('\n', f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 3 <uint8_t>: ", f);  _print_base(f, in->base, true);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 4 <bool>:    NULL - <NON-UTILIZED-PARAMETER\n", f);

    for (int i = 0; i < tab_depth; ++i) fputs(TAB, f); 
    fputs("\n>", f);
}
stinl void fmt_in_to_strn(FILE *f, cvoid *vin, int tab_depth) {
    const bitos_conv_in *in = (bitos_conv_in*)vin;
    fputs("< -- BITOS_CONV --\n", f);

    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fprintf(f, "+) Input 1 <size_t>:  %zu\n", in->len);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 2 <bigInt>:", f); _print_bigint(f, &in->x, tab_depth); fputc('\n', f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 3 <uint8_t>: NULL - <NON-UTILIZED-PARAMETER\n", f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 4 <bool>:    NULL - <NON-UTILIZED-PARAMETER\n", f);

    for (int i = 0; i < tab_depth; ++i) fputs(TAB, f); 
    fputs("\n>", f);
}
stinl void fmt_in_to_strnb(FILE *f, cvoid *vin, int tab_depth) {
    const bitos_conv_in *in = (bitos_conv_in*)vin;
    fputs("< -- BITOS_CONV --\n", f);

    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fprintf(f, "+) Input 1 <size_t>:  %zu\n", in->len);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 2 <bigInt>:", f); _print_bigint(f, &in->x, tab_depth); fputc('\n', f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 3 <uint8_t>: ", f);  _print_base(f, in->base, true);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 4 <bool>:    NULL - <NON-UTILIZED-PARAMETER\n", f);

    for (int i = 0; i < tab_depth; ++i) fputs(TAB, f); 
    fputs("\n>", f);
}
stinl void fmt_in_to_strf(FILE *f, cvoid *vin, int tab_depth) {
    const bitos_conv_in *in = (bitos_conv_in*)vin;
    fputs("< -- BITOS_CONV --\n", f);

    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fprintf(f, "+) Input 1 <size_t>:  %zu\n", in->len);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 2 <bigInt>:", f); _print_bigint(f, &in->x, tab_depth); fputc('\n', f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 3 <uint8_t>: ", f);  _print_base(f, in->base, true);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fprintf(f, "+) Input 4 <bool>: %s\n", (in->uppercase) ? 
        "true - (Uppercase)" : 
        "false - (Undercase)"
    );

    for (int i = 0; i < tab_depth; ++i) fputs(TAB, f); 
    fputs("\n>", f);
}
// Inverse Formatter - BITOS_PRINT
stinl void fmt_in_fput(FILE *f, cvoid *vin, int tab_depth) {
    const bitos_print_in *in = (bitos_print_in*)vin;
    fputs("< -- BITOS_PRINT --\n", f);

    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 1 <bigInt>:", f); _print_bigint(f, &in->x, tab_depth); fputc('\n', f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 2 <uint8_t>: NULL - <NON-UTILIZED-PARAMETER\n", f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 3 <bool>:    NULL - <NON-UTILIZED-PARAMETER\n", f);

    for (int i = 0; i < tab_depth; ++i) fputs(TAB, f); 
    fputs("\n>", f);
}
stinl void fmt_in_fputb(FILE *f, cvoid *vin, int tab_depth) {
    const bitos_print_in *in = (bitos_print_in*)vin;
    fputs("< -- BITOS_PRINT --\n", f);

    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 1 <bigInt>:", f); _print_bigint(f, &in->x, tab_depth); fputc('\n', f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 2 <uint8_t>: ", f);  _print_base(f, in->base, true);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 3 <bool>:    NULL - <NON-UTILIZED-PARAMETER\n", f);

    for (int i = 0; i < tab_depth; ++i) fputs(TAB, f); 
    fputs("\n>", f);
}
stinl void fmt_in_fputf(FILE *f, cvoid *vin, int tab_depth) {
    const bitos_print_in *in = (bitos_print_in*)vin;
    fputs("< -- BITOS_PRINT --\n", f);

    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 1 <bigInt>:", f); _print_bigint(f, &in->x, tab_depth); fputc('\n', f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 2 <uint8_t>: ", f);  _print_base(f, in->base, true);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fprintf(f, "+) Input 3 <bool>: %s\n", (in->uppercase) ? 
        "true - (Uppercase)" : 
        "false - (Undercase)"
    );

    for (int i = 0; i < tab_depth; ++i) fputs(TAB, f); 
    fputs("\n>", f);
}
// Inverse Formatter - BITOS_FWRITE & BITOS_SERIALIZE
stinl void fmt_in_fwrite(FILE *f, cvoid *vin, int tab_depth) { DNML_UNFINISHED(); }
stinl void fmt_in_serial(FILE *f, cvoid *vin, int tab_depth) { DNML_UNFINISHED(); }


//* ========================= INVERSE STOBI FORMATTERS ======================= *//
/* Example Format - STOBI:
        - Input: < -- STOBI_ASSIGN --
            +) Input 1 - <char*>: <
                --- Low segment:  "111222333444555666777888999000AB..."
                --- High segment: "...111222333444555666777888999000AB"
                --- Length: 2048
            >
            +) Input 2 - <size_t>: 4096
            +) Input 3 - <uint8_t>: 16 - HEXADECIMAL
        >
*/
stinl void fmt_recon_stobi(FILE *f, cvoid *vrecon, int tab_depth) {
    char **precon = (char**)vrecon;
    const char *recon = *precon;
    _print_str(f, recon, strlen(recon), tab_depth);
}
// Inverse Formatter - STOBI_INIT
stinl void fmt_in_strinit(FILE *f, cvoid *vin, int tab_depth) {
    const stobi_init_in *in = (stobi_init_in*)vin;
    fputs("< -- STOBI_INIT --\n", f);

    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 1 <char*>: ", f); _print_str(f, in->str, strlen(in->str), tab_depth);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 2 <size_t>: NULL - <NON-UTILIZED-PARAMETER\n", f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 3 <uint8_t>: NULL - <NON-UTILIZED-PARAMETER\n", f);

    for (int i = 0; i < tab_depth; ++i) fputs(TAB, f); 
    fputs("\n>", f);
}
stinl void fmt_in_strbinit(FILE *f, cvoid *vin, int tab_depth) {
    const stobi_init_in *in = (stobi_init_in*)vin;
    fputs("< -- STOBI_INIT --\n", f);

    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 1 <char*>: ", f); _print_str(f, in->str, strlen(in->str), tab_depth);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 2 <size_t>: NULL - <NON-UTILIZED-PARAMETER\n", f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 3 <uint8_t>: ", f);  _print_base(f, in->base, true);

    for (int i = 0; i < tab_depth; ++i) fputs(TAB, f); 
    fputs("\n>", f);
}
stinl void fmt_in_strninit(FILE *f, cvoid *vin, int tab_depth) {
    const stobi_init_in *in = (stobi_init_in*)vin;
    fputs("< -- STOBI_INIT --\n", f);

    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 1 <char*>: ", f); _print_str(f, in->str, strlen(in->str), tab_depth);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fprintf(f, "+) Input 2 <size_t>:  %zu\n", in->len);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 3 <uint8_t>: NULL - <NON-UTILIZED-PARAMETER\n", f);

    for (int i = 0; i < tab_depth; ++i) fputs(TAB, f); 
    fputs("\n>", f);
}
stinl void fmt_in_strnbinit(FILE *f, cvoid *vin, int tab_depth) {
    const stobi_init_in *in = (stobi_init_in*)vin;
    fputs("< -- STOBI_INIT --\n", f);

    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 1 <char*>: ", f); _print_str(f, in->str, strlen(in->str), tab_depth);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fprintf(f, "+) Input 2 <size_t>:  %zu\n", in->len);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 3 <uint8_t>: ", f);  _print_base(f, in->base, true);

    for (int i = 0; i < tab_depth; ++i) fputs(TAB, f); 
    fputs("\n>", f);
}
// Inverse Formatter - STOBI_CONV
stinl void fmt_in_from_str(FILE *f, cvoid *vin, int tab_depth) {
    const stobi_conv_in *in = (stobi_conv_in*)vin;
    fputs("< -- STOBI_CONV --\n", f);

    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 1 <char*>: ", f); _print_str(f, in->str, strlen(in->str), tab_depth);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 2 <size_t>: NULL - <NON-UTILIZED-PARAMETER\n", f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 3 <uint8_t>: NULL - <NON-UTILIZED-PARAMETER\n", f);

    for (int i = 0; i < tab_depth; ++i) fputs(TAB, f); 
    fputs("\n>", f);
}
stinl void fmt_in_from_strb(FILE *f, cvoid *vin, int tab_depth) {
    const stobi_conv_in *in = (stobi_conv_in*)vin;
    fputs("< -- STOBI_CONV --\n", f);

    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 1 <char*>: ", f); _print_str(f, in->str, strlen(in->str), tab_depth);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 2 <size_t>: NULL - <NON-UTILIZED-PARAMETER\n", f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 3 <uint8_t>: ", f);  _print_base(f, in->base, true);

    for (int i = 0; i < tab_depth; ++i) fputs(TAB, f); 
    fputs("\n>", f);
}
stinl void fmt_in_from_strn(FILE *f, cvoid *vin, int tab_depth) {
    const stobi_conv_in *in = (stobi_conv_in*)vin;
    fputs("< -- STOBI_CONV --\n", f);

    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 1 <char*>: ", f); _print_str(f, in->str, strlen(in->str), tab_depth);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fprintf(f, "+) Input 2 <size_t>:  %zu\n", in->len);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 3 <uint8_t>: NULL - <NON-UTILIZED-PARAMETER\n", f);

    for (int i = 0; i < tab_depth; ++i) fputs(TAB, f); 
    fputs("\n>", f);
}
stinl void fmt_in_from_strnb(FILE *f, cvoid *vin, int tab_depth) {
    const stobi_conv_in *in = (stobi_conv_in*)vin;
    fputs("< -- STOBI_CONV --\n", f);

    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 1 <char*>: ", f); _print_str(f, in->str, strlen(in->str), tab_depth);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fprintf(f, "+) Input 2 <size_t>:  %zu\n", in->len);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 3 <uint8_t>: ", f);  _print_base(f, in->base, true);

    for (int i = 0; i < tab_depth; ++i) fputs(TAB, f); 
    fputs("\n>", f);
}
// Inverse Formatter - STOBI_ASSIGN
stinl void fmt_in_get_str(FILE *f, cvoid *vin, int tab_depth) {
    const stobi_assign_in *in = (stobi_assign_in*)vin;
    fputs("< -- STOBI_ASSIGN --\n", f);

    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 1 <char*>: ", f); _print_str(f, in->str, strlen(in->str), tab_depth);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 2 <size_t>: NULL - <NON-UTILIZED-PARAMETER\n", f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 3 <uint8_t>: NULL - <NON-UTILIZED-PARAMETER\n", f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 4 <size_t>:  NULL - <NON-UTILIZED-PARAMETER\n", f);


    for (int i = 0; i < tab_depth; ++i) fputs(TAB, f); 
    fputs("\n>", f);
}
stinl void fmt_in_get_sstr(FILE *f, cvoid *vin, int tab_depth) {
    const stobi_assign_in *in = (stobi_assign_in*)vin;
    fputs("< -- STOBI_ASSIGN --\n", f);

    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 1 <char*>: ", f); _print_str(f, in->str, strlen(in->str), tab_depth);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 2 <size_t>: NULL - <NON-UTILIZED-PARAMETER\n", f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 3 <uint8_t>: NULL - <NON-UTILIZED-PARAMETER\n", f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fprintf(f, "+) Input 4 <size_t>:  %zu (limbs)\n", in->bi_size);


    for (int i = 0; i < tab_depth; ++i) fputs(TAB, f); 
    fputs("\n>", f);
}
stinl void fmt_in_get_strb(FILE *f, cvoid *vin, int tab_depth) {
    const stobi_assign_in *in = (stobi_assign_in*)vin;
    fputs("< -- STOBI_ASSIGN --\n", f);

    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 1 <char*>: ", f); _print_str(f, in->str, strlen(in->str), tab_depth);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 2 <size_t>: NULL - <NON-UTILIZED-PARAMETER\n", f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 3 <uint8_t>: ", f);  _print_base(f, in->base, true);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 4 <size_t>:  NULL - <NON-UTILIZED-PARAMETER\n", f);

    for (int i = 0; i < tab_depth; ++i) fputs(TAB, f); 
    fputs("\n>", f);
}
stinl void fmt_in_get_sstrb(FILE *f, cvoid *vin, int tab_depth) {
    const stobi_assign_in *in = (stobi_assign_in*)vin;
    fputs("< -- STOBI_ASSIGN --\n", f);

    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 1 <char*>: ", f); _print_str(f, in->str, strlen(in->str), tab_depth);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 2 <size_t>: NULL - <NON-UTILIZED-PARAMETER\n", f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 3 <uint8_t>: ", f);  _print_base(f, in->base, true);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fprintf(f, "+) Input 4 <size_t>:  %zu (limbs)\n", in->bi_size);

    for (int i = 0; i < tab_depth; ++i) fputs(TAB, f); 
    fputs("\n>", f);
}
stinl void fmt_in_get_strn(FILE *f, cvoid *vin, int tab_depth) {
    const stobi_assign_in *in = (stobi_assign_in*)vin;
    fputs("< -- STOBI_ASSIGN --\n", f);

    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 1 <char*>: ", f); _print_str(f, in->str, strlen(in->str), tab_depth);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fprintf(f, "+) Input 2 <size_t>:  %zu\n", in->len);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 3 <uint8_t>: NULL - <NON-UTILIZED-PARAMETER\n", f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 4 <size_t>:  NULL - <NON-UTILIZED-PARAMETER\n", f);

    for (int i = 0; i < tab_depth; ++i) fputs(TAB, f); 
    fputs("\n>", f);
}
stinl void fmt_in_get_sstrn(FILE *f, cvoid *vin, int tab_depth) {
    const stobi_assign_in *in = (stobi_assign_in*)vin;
    fputs("< -- STOBI_ASSIGN --\n", f);

    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 1 <char*>: ", f); _print_str(f, in->str, strlen(in->str), tab_depth);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fprintf(f, "+) Input 2 <size_t>:  %zu\n", in->len);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 3 <uint8_t>: NULL - <NON-UTILIZED-PARAMETER\n", f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fprintf(f, "+) Input 4 <size_t>:  %zu (limbs)\n", in->bi_size);

    for (int i = 0; i < tab_depth; ++i) fputs(TAB, f); 
    fputs("\n>", f);
}
stinl void fmt_in_get_strnb(FILE *f, cvoid *vin, int tab_depth) {
    const stobi_assign_in *in = (stobi_assign_in*)vin;
    fputs("< -- STOBI_ASSIGN --\n", f);

    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 1 <char*>: ", f); _print_str(f, in->str, strlen(in->str), tab_depth);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fprintf(f, "+) Input 2 <size_t>:  %zu\n", in->len);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 3 <uint8_t>: ", f);  _print_base(f, in->base, true);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 4 <size_t>:  NULL - <NON-UTILIZED-PARAMETER\n", f);

    for (int i = 0; i < tab_depth; ++i) fputs(TAB, f); 
    fputs("\n>", f);
}
stinl void fmt_in_get_sstrnb(FILE *f, cvoid *vin, int tab_depth) {
    const stobi_assign_in *in = (stobi_assign_in*)vin;
    fputs("< -- STOBI_ASSIGN --\n", f);

    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 1 <char*>: ", f); _print_str(f, in->str, strlen(in->str), tab_depth);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fprintf(f, "+) Input 2 <size_t>:  %zu\n", in->len);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 3 <uint8_t>: ", f);  _print_base(f, in->base, true);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fprintf(f, "+) Input 4 <size_t>:  %zu (limbs)\n", in->bi_size);

    for (int i = 0; i < tab_depth; ++i) fputs(TAB, f); 
    fputs("\n>", f);
}
// Inverse Formatter - STOBI_SCAN
stinl void fmt_in_fscan(FILE *f, cvoid *vin, int tab_depth) {
    const stobi_scan_in *in = (stobi_scan_in*)vin;
    fputs("< -- STOBI_SCAN --\n", f);

    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fprintf(f, "+) Input 1 <FILE*>: %p\n", in->stream);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 2 <uint8_t>: NULL - <NON-UTILIZED-PARAMETER\n", f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 3 <size_t>:  NULL - <NON-UTILIZED-PARAMETER\n", f);

    for (int i = 0; i < tab_depth; ++i) fputs(TAB, f); 
    fputs("\n>", f);
}
stinl void fmt_in_fsscan(FILE *f, cvoid *vin, int tab_depth) {
    const stobi_scan_in *in = (stobi_scan_in*)vin;
    fputs("< -- STOBI_SCAN --\n", f);

    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fprintf(f, "+) Input 1 <FILE*>: %p\n", in->stream);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 2 <uint8_t>: NULL - <NON-UTILIZED-PARAMETER\n", f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fprintf(f, "+) Input 3 <size_t>: %zu (limbs)\n", in->bi_size);

    for (int i = 0; i < tab_depth; ++i) fputs(TAB, f); 
    fputs("\n>", f);
}
stinl void fmt_in_fscanb(FILE *f, cvoid *vin, int tab_depth) {
    const stobi_scan_in *in = (stobi_scan_in*)vin;
    fputs("< -- STOBI_SCAN --\n", f);

    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fprintf(f, "+) Input 1 <FILE*>: %p\n", in->stream);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 2 <uint8_t>: ", f);  _print_base(f, in->base, true);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 3 <size_t>:  NULL - <NON-UTILIZED-PARAMETER\n", f);


    for (int i = 0; i < tab_depth; ++i) fputs(TAB, f); 
    fputs("\n>", f);
}
stinl void fmt_in_fsscanb(FILE *f, cvoid *vin, int tab_depth) {
    const stobi_scan_in *in = (stobi_scan_in*)vin;
    fputs("< -- STOBI_SCAN --\n", f);

    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fprintf(f, "+) Input 1 <FILE*>: %p\n", in->stream);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Input 2 <uint8_t>: ", f);  _print_base(f, in->base, true);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fprintf(f, "+) Input 3 <size_t>: %zu (limbs)\n", in->bi_size);

    for (int i = 0; i < tab_depth; ++i) fputs(TAB, f); 
    fputs("\n>", f);
}
// Inverse Formatter - STOBI_FREAD && STOBI_DESERIALIZE
stinl void fmt_in_fread(FILE *f, cvoid *vin, int tab_depth) { DNML_UNFINISHED(); }
stinl void fmt_in_deserial(FILE *f, cvoid *vin, int tab_depth) { DNML_UNFINISHED(); }



#endif