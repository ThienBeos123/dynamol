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



#include "crt_modop.h"


/* ------ Main Standalone Algorithms ------ */
void __CRINT_MONTMUL__(crint *res, const crint *a, const crint *b, mont_ctx ctx) {}
void __CRINT_CMODMUL__(crint *res, const crint *a, const crint *b, const crint *mod) {}
void __CRINT_BIN_MODEXP__(crint *res, const crint *base, const crint *exp, const crint *mod) {}
void __CRINT_MBIN_MODEXP__(crint *res, const bigInt *base, const bigInt *exp, const crint *mod) {}



/* ------ Algorithm Dispatchers ------ */
void __CRINT_MODMUL_DISP__(crint *res, const crint *a, const crint *b, const crint *mod) {}
void __CRINT_MODEXP_DISP__(crint *res, const crint *base, const crint *exp, const crint *mod) {}


