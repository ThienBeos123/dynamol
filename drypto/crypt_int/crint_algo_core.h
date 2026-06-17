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
void __CRINT_MAGDIV__(crint *quot, crint *temp_rem, crint *a, crint *b);
void __CRINT_MAGMOD__(crint *temp_quot, crint *rem, crint *a, crint *b);
void __CRINT_MAGMUL_U64__(crint *res, crint *x, uint64_t val);
void __CRINT_MAGDIVMOD_U64__(crint *quot, uint64_t *rem, crint *a, uint64_t val);
/* ---------------------- MAGNITUDED NUMBER-THEORETIC --------------------- */
/* --------------------- MAGNITUDED MODULAR-ARITHMETIC -------------------- */
void __CRINT_MAGEMOD_U64__(uint64_t *rem, crint *tmp_quot, crint *dend, uint64_t mod);
void __CRINT_MAGEMOD__(crint *rem, crint *tmp_quot, crint *dend, crint *mod);
void __CRINT_MAGMADD__(crint *res, crint *a, crint *b, crint *mod);
void __CRINT_MAGMSUB__(crint *res, crint *a, crint *b, crint *mod);
void __CRINT_MAGMMUL__(crint *res, crint *a, crint *b, crint *mod);
void __CRINT_MAGMDIV__(crint *res, crint *a, crint *b, crint *mod);
void __CRINT_MAGMEXP__(crint *res, crint *a, crint *b, crint *mod);
void __CRINT_MAGMSQR__(crint *res, crint *a, crint *b, crint *mod);
void __CRINT_MAGMINV__(crint *res, crint *a, crint *b, crint *mod);
/* -------------------- MAGNITUDED ALGEBRAIC OPERATIONS ------------------- */



#endif
