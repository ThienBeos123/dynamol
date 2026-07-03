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
#include <debug_util.h>
#include "../../util/aconv_macros.h"


/* ------ Main Standalone Algorithms ------ */
dnml_status __CRINT_MONTMUL__(crint *res, crint *a, crint *b, mont_ctx ctx) { return CRINT_SUCCESS; }
dnml_status __CRINT_CMODMUL__(crint *res, crint *a, crint *b, crint *mod) { return CRINT_SUCCESS; }
dnml_status __CRINT_BIN_MODEXP__(crint *res, crint *base, crint *exp, crint *mod) { return CRINT_SUCCESS; }
dnml_status __CRINT_MBIN_MODEXP__(crint *res, bigInt *base, bigInt *exp, crint *mod) { return CRINT_SUCCESS; }



/* ------ Algorithm Dispatchers ------ */
dnml_status __CRINT_MODMUL_DISP__(crint *res, crint *a, crint *b, crint *mod) { return CRINT_SUCCESS; }
dnml_status __CRINT_MODEXP_DISP__(crint *res, crint *base, crint *exp, crint *mod) { return CRINT_SUCCESS; }
