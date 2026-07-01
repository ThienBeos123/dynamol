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



#ifndef DNML_GCD_H
#define DNML_GCD_H


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
struct rt_matrix {
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
size_t __hgcd_mat_compose_ws(size_t Asize, size_t Bsize, size_t Csize, size_t Dsize, size_t Esize, size_t Fsize, size_t Gsize, size_t Hsize);
size_t __hgcd_matmul_ws(size_t a_size, size_t b_size, size_t Asize, size_t Bsize, size_t Csize, size_t Dsize);
size_t _hgcd_ws(size_t a_size, size_t b_size);
dnml_status __hgcd_mat_compose(struct rt_matrix *T1, struct rt_matrix *T2, struct rt_matrix *T);
dnml_status __hgcd_matmul(bigInt *const a, bigInt *const b, struct rt_matrix *T);
dnml_status _hgcd_reduct(struct rt_matrix *T, bigInt *const a, bigInt *const b); // For XGCD
#ifdef __cplusplus
}
#endif




#endif