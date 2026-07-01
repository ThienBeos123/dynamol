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
#include "div.h"
#include "mul.h"
#include "mod.h"
struct rt_matrix {
    bigInt A; bigInt B;
    bigInt C; bigInt D;
};

#ifdef __cplusplus
extern "C" {
#endif
//* ======== GCD - WORKSPACE RETURNER ======== */
size_t __BIGINT_STEIN_WS__(size_t u_size, size_t v_size);
size_t __BIGINT_LEHMER_WS__(size_t u_size, size_t v_size);
size_t __BIGINT_SUBQ_WS__(size_t u_size, size_t v_size);
size_t __BIGINT_GCD_WS__(size_t u_size, size_t v_size);
/* ======== GCD - ALGORITHMS ======== */
uint64_t __BINARY_GCDU64__(uint64_t u, uint64_t v);
void __BIGINT_STEIN__(bigInt *const res, const bigInt *const u, const bigInt *const v, calc_ctx stein_ctx, dnml_status *err);
void __BIGINT_LEHMER__(bigInt *const res, const bigInt *const u, const bigInt *const v, calc_ctx lehmer_ctx, dnml_status *err);
void __BIGINT_SUBQ__(bigInt *const res, const bigInt *const u, const bigInt *const v, calc_ctx half_ctx, dnml_status *err);
void __BIGINT_GCD_DISP__(bigInt *const res, const bigInt *const u, const bigInt *const v, calc_ctx gcd_ctx, dnml_status *err);
/* ========= Extra algorithms and functions ========= */
size_t __hgcd_mat_compose_ws(size_t Asize, size_t Bsize, size_t Csize, size_t Dsize, size_t Esize, size_t Fsize, size_t Gsize, size_t Hsize);
size_t __hgcd_matmul_ws(size_t a_size, size_t b_size, size_t Asize, size_t Bsize, size_t Csize, size_t Dsize);
size_t _hgcd_ws(size_t a_size, size_t b_size);
dnml_status __hgcd_mat_compose(struct rt_matrix *T1, struct rt_matrix *T2, struct rt_matrix *T, calc_ctx hgcd_ctx);
dnml_status __hgcd_matmul(bigInt *const a, bigInt *const b, struct rt_matrix *T, calc_ctx hgcd_ctx);
dnml_status _hgcd_reduct(struct rt_matrix *T, bigInt *const a, bigInt *const b, calc_ctx hgcd_ctx); // For XGCD
#ifdef __cplusplus
}
#endif




#endif
