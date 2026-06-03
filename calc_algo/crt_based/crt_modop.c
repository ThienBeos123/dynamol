#include "crt_modop.h"


/* ------ Main Standalone Algorithms ------ */
void __CRINT_MONTMUL__(crint *res, const crint *a, const crint *b, mont_ctx ctx) {}
void __CRINT_CMODMUL__(crint *res, const crint *a, const crint *b, const crint *mod) {}
void __CRINT_BIN_MODEXP__(crint *res, const crint *base, const crint *exp, const crint *mod) {}
void __CRINT_MBIN_MODEXP__(crint *res, const bigInt *base, const bigInt *exp, const crint *mod) {}



/* ------ Algorithm Dispatchers ------ */
void __CRINT_MODMUL_DISP__(crint *res, const crint *a, const crint *b, const crint *mod) {}
void __CRINT_MODEXP_DISP__(crint *res, const crint *base, const crint *exp, const crint *mod) {}


