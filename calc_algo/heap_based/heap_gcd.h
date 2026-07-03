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



#ifndef DNML_HEAP_GCD_H
#define DNML_HEAP_GCD_H


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
struct hrt_matrix {
    bigInt A; bigInt B;
    bigInt C; bigInt D;
};

#ifdef __cplusplus
extern "C" {
#endif
/* ======== GCD - ALGORITHMS ======== */
void __BIHEAP_STEIN__(bigInt *const res, const bigInt *const u, const bigInt *const v, dnml_status *err);
void __BIHEAP_LEHMER__(bigInt *const res, const bigInt *const u, const bigInt *const v, dnml_status *err);
void __BIHEAP_SUBQ__(bigInt *const res, const bigInt *const u, const bigInt *const v, dnml_status *err);
void __BIHEAP_GCD_DISP__(bigInt *const res, const bigInt *const u, const bigInt *const v, dnml_status *err);
/* ========= Extra algorithms and functions ========= */
dnml_status __hgcd_heap_matcomp(struct hrt_matrix *T1, struct hrt_matrix *T2, struct hrt_matrix *T);
dnml_status __hgcd_heap_matmul(bigInt *const a, bigInt *const b, struct hrt_matrix *T);
dnml_status _hgcd_heap_reduct(struct hrt_matrix *T, bigInt *const a, bigInt *const b); // For XGCD
#ifdef __cplusplus
}
#endif




#endif
