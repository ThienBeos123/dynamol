#ifndef DNML_CRYPT_INT_ALGO_CORE
#define DNML_CRYPT_INT_ALGO_CORE


#include <libdnml_types.h>
#include <include.h>
#include <char_tables.h>
#include <dnml_sys/sys.h>
#include <debug_util.h>
#include <_libdnml_config/settings.h>
#include <_libdnml_config/numeric_config.h>
#include <_libdnml_mem/_ctx.h>
#include <dnml_status.h>



//* ========================================= MAGNITUDE MATHEMATICA ========================================== *//
/* ------------------------ MAGNITUDED ARITHMETIC ------------------------- */
void __CRINT_MAGADD__(crint *res, crint *a, crint *b);
void __CRINT_MAGSUB__(crint *res, crint *a, crint *b);
void __CRINT_MAGMUL___(crint *res, crint *a, crint *b);
void __CRINT_MAGDIVMOD__(crint *quot, crint *rem, crint *a, crint *b);
void __CRINT_MAGMUL_U64__(crint *res, crint *x, uint64_t val);
void __CRINT_MAGDIVMOD_U64__(crint *quot, crint *rem, crint *a, uint64_t val);
/* ------------------- MAGNITUDED CORE NUMBER-THEORETIC ------------------- */
/* --------------------- MAGNITUDED MODULAR-ARITHMETIC -------------------- */
/* -------------------- MAGNITUDED ALGEBRAIC OPERATIONS ------------------- */



#endif