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



#ifndef DNML_HEAP_MODOP_H
#define DNML_HEAP_MODOP_H



#include <dnml_status.h>
#include <libdnml_types.h>
#include <include.h>
#include <dnml_sys/sys.h>
#include <_libdnml_config/numeric_config.h>
#include <_libdnml_mem/_ctx.h>
#include "../../intrinsics/intrinsics.h"
#include "../../util/util.h"
#include "../algo_base/add_sub.h"
#include "heap_div.h"
#include "heap_mul.h"
#include "heap_mod.h"

#ifdef __cplusplus
extern "C" {
#endif
void __BIHEAP_MONTMUL__(const bigInt *const a, const bigInt *const b, mont_ctx ctx, bigInt *const res, dnml_status *err);
void __BIHEAP_CMODMUL__(const bigInt *const a, const bigInt *const b, const bigInt *const mod, bigInt *const res, dnml_status *err);
void __BIHEAP_BIN_MODEXP__(const bigInt *const base, const bigInt *const exp, const bigInt *const mod, bigInt *const res, dnml_status *err);
void __BIHEAP_MBIN_MODEXP__(const bigInt *const base, const bigInt *const exp, const bigInt *const mod, bigInt *const res, dnml_status *err);
void __BIHEAP_FIX_MODEXP__(const bigInt *const base, const bigInt *const exp, const bigInt *const mod, bigInt *const res, dnml_status *err);
void __BIHEAP_SLIDE_MODEXP__(const bigInt *const base, const bigInt *const exp, const bigInt *const mod, bigInt *const res, dnml_status *err);
void __BIHEAP_MODMUL_DISP__(const bigInt *const a, const bigInt *const b, const bigInt *const mod, bigInt *const res, dnml_status *err);
void __BIHEAP_MODEXP_DISP__(const bigInt *const base, const bigInt *const exp, const bigInt *const mod, bigInt *const res, dnml_status *err);
#ifdef __cplusplus
}
#endif

#endif
