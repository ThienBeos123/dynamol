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



#ifndef DNML_POW_ROOT_H
#define DNML_POW_ROOT_H

#include <dnml_status.h>
#include <debug_util.h>
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


#ifdef __cplusplus
extern "C" {
#endif
/* ----------------- WORKSPACE ----------------- */
size_t __BIGINT_BINEXP_WS__(size_t base_size, uint64_t pow);
size_t __BIGINT_FIXED_WS__(size_t base_size, uint64_t pow, uint8_t ksize);
size_t __BIGINT_SLIDIN_WS__(size_t base_size, uint64_t pow, uint8_t ksize);
size_t __BIGINT_HERON_WS__(size_t a_size);
size_t __BIGINT_NEWTON_CBRT_WS__(size_t a_size);
size_t __BIGINT_NEWTON_2NROOT_WS__(size_t a_size, uint64_t root);
size_t __BIGINT_NEWTON_NROOT_WS__(size_t a_size, uint64_t root);



/* ----------------- ALGORITHMS ----------------- */
void __BIGINT_BINARY_EXP__(bigInt *const res, const bigInt *const base, uint64_t exp, calc_ctx binexp_ctx, dnml_status *err);
void __BIGINT_FIXED__(bigInt *const res, const bigInt *const base, uint64_t exp, uint8_t k, calc_ctx fix_ctx, dnml_status *err);
void __BIGINT_SLIDING__(bigInt *const res, const bigInt *const base, uint64_t exp, uint8_t k, calc_ctx slide_ctx, dnml_status *err);
void __BIGINT_HERON__(bigInt *const res, const bigInt *const a, calc_ctx heron_ctx, dnml_status *err);
void __BIGINT_NEWTON_CBRT__(bigInt *const res, const bigInt *const a, calc_ctx cbrt_ctx, dnml_status *err);
uint64_t __UI64_NROOT__(uint64_t a, uint64_t root);
void __BIGINT_NEWTON_2NRT__(bigInt *const res, const bigInt *const a, uint64_t root, calc_ctx _2nroot_ctx, dnml_status *err);
void __BIGINT_NEWTON_NRT__(bigInt *const res, const bigInt *const a, uint64_t root, calc_ctx nroot_ctx, dnml_status *err);



/* ----------------- DISPATCHES ----------------- */
size_t __BIGINT_EXP_WS__(size_t base_size, uint64_t pow);
size_t __BIGINT_SQRT_WS__(size_t a_size);
size_t __BIGINT_CBRT_WS__(size_t a_size);
size_t __BIGINT_NRT_WS__(size_t a_size, uint64_t root);
void __BIGINT_EXP_DISP__(bigInt *const res, const bigInt *const base, uint64_t exp, calc_ctx exp_ctx, dnml_status *err);
void __BIGINT_SQRT_DISP__(bigInt *const res, const bigInt *const a, calc_ctx sqrt_ctx, dnml_status *err);
void __BIGINT_CBRT_DISP__(bigInt *const res, const bigInt *const a, calc_ctx cbrt_ctx, dnml_status *err);
void __BIGINT_NRT_DISP__(bigInt *const res, const bigInt *const a, uint64_t root, calc_ctx nroot_ctx, dnml_status *err);
#ifdef __cplusplus
}
#endif


#endif
