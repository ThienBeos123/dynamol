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



#ifndef DNML_HEAP_POW_ROOT_H
#define DNML_HEAP_POW_ROOT_H

#include <dnml_status.h>
#include <debug_util.h>
#include <libdnml_types.h>
#include <include.h>
#include <dnml_sys/sys.h>
#include <_libdnml_config/numeric_config.h>
#include "../../intrinsics/intrinsics.h"
#include "../../util/util.h"
#include "../algo_base/add_sub.h"
#include "heap_div.h"
#include "heap_mul.h"


#ifdef __cplusplus
extern "C" {
#endif
/* ----------------- ALGORITHMS ----------------- */
void __BIHEAP_BINARY_EXP__(bigInt *const res, const bigInt *const base, uint64_t exp, dnml_status *err);
void __BIHEAP_FIXED__(bigInt *const res, const bigInt *const base, uint64_t exp, uint8_t k, dnml_status *err);
void __BIHEAP_SLIDING__(bigInt *const res, const bigInt *const base, uint64_t exp, uint8_t k, dnml_status *err);
void __BIHEAP_HERON__(bigInt *const res, const bigInt *const a, dnml_status *err);
void __BIHEAP_NEWTON_CBRT__(bigInt *const res, const bigInt *const a, dnml_status *err);
uint64_t __UI64_NROOT__(uint64_t a, uint64_t root);
void __BIHEAP_NEWTON_2NRT__(bigInt *const res, const bigInt *const a, uint64_t root, dnml_status *err);
void __BIHEAP_NEWTON_NRT__(bigInt *const res, const bigInt *const a, uint64_t root, dnml_status *err);



/* ----------------- DISPATCHES ----------------- */
void __BIHEAP_EXP_DISPATCH__(bigInt *const res, const bigInt *const base, uint64_t exp, dnml_status *err);
void __BIHEAP_SQRT_DISPATCH__(bigInt *const res, const bigInt *const a, dnml_status *err);
void __BIHEAP_CBRT_DISPATCH__(bigInt *const res, const bigInt *const a, dnml_status *err);
void __BIHEAP_NRT_DISPATCH__(bigInt *const res, const bigInt *const a, uint64_t root, dnml_status *err);
#ifdef __cplusplus
}
#endif


#endif
