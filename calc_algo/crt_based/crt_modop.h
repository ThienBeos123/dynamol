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



#ifndef DNML_CRT_MODOP_H
#define DNML_CRT_MODOP_H



#include <libdnml_types.h>
#include <include.h>
#include <dnml_sys/sys.h>
#include <_libdnml_config/numeric_config.h>
#include <_libdnml_mem/_ctx.h>
#include "../../intrinsics/intrinsics.h"
#include "../../util/crt_util.h"
#include "../algo_base/add_sub.h"
#include "crt_mul.h"
#include "crt_div.h"
#include "crt_mod.h"

#ifdef __cplusplus
extern "C" {
#endif
/* ------ Main Standalone Algorithms ------ */
dnml_status __CRINT_MONTMUL__(crint *res, crint *a, crint *b, mont_ctx ctx);
dnml_status __CRINT_CMODMUL__(crint *res, crint *a, crint *b, crint *mod);
dnml_status __CRINT_BIN_MODEXP__(crint *res, crint *base, crint *exp, crint *mod);
dnml_status __CRINT_MBIN_MODEXP__(crint *res, bigInt *base, bigInt *exp, crint *mod);
/* ------ Algorithm Dispatchers ------ */
dnml_status __CRINT_MODMUL_DISP__(crint *res, crint *a, crint *b, crint *mod);
dnml_status __CRINT_MODEXP_DISP__(crint *res, crint *base, crint *exp, crint *mod);
#ifdef __cplusplus
}
#endif


#endif
