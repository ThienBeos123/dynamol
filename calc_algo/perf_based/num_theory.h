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



#ifndef DNML_NUM_THEORY_H
#define DNML_NUM_THEORY_H


#include <debug_util.h>
#include <dnml_status.h>
#include <libdnml_types.h>
#include <include.h>
#include <dnml_sys/sys.h>
#include <_libdnml_config/numeric_config.h>
#include <_libdnml_mem/_ctx.h>
#include "../../intrinsics/intrinsics.h"

#include "../algo_base/add_sub.h"
#include "div.h"
#include "mul.h"
#include "mod.h"
#include "mod_op.h"

#ifdef __cplusplus
extern "C" {
#endif
//* ======== GCD - WORKSPACE RETURNER ======== */
size_t __BIGINT_STEIN_WS__(size_t u_size, size_t v_size);
size_t __BIGINT_LEHMER_WS__(size_t u_size, size_t v_size);
size_t __BIGINT_HALF_WS__(size_t u_size, size_t v_size);
size_t __BIGINT_GCD_WS__(size_t u_size, size_t v_size);
/* ======== GCD - ALGORITHMS ======== */
uint64_t __BIGINT_EUCLID__(uint64_t u, uint64_t v);
void __BIGINT_STEIN__(bigInt *res, const bigInt *u, const bigInt *v, calc_ctx stein_ctx);
void __BIGINT_LEHMER__(bigInt *res, const bigInt *u, const bigInt *v, calc_ctx lehmer_ctx);
void __BIGINT_HALF__(bigInt *res, const bigInt *u, const bigInt *v, calc_ctx half_ctx);
void __BIGINT_GCD_DISPATCH__(bigInt *res, const bigInt *u, const bigInt *v, calc_ctx gcd_ctx);


//* ======== Primality Testing - WORKSPACE RETURNER ======== */
size_t __BIGINT_MRABIN_WS__(size_t n_size, size_t base_size);
size_t __BIGINT_BPSW_WS__(size_t n_size);
size_t __BIGINT_ECPP_WS__(size_t n_size);
size_t __BIGINT_PTEST_WS__(size_t x_size);
/* ======== Primality Testing - ALGORITHMS ======== */
uint8_t __BIGINT_TRIAL_DIV__(uint64_t x);
uint8_t __BIGINT_SMALL_MRABIN__(uint64_t n);
uint8_t __BIGINT_MILLER_RABIN__(const bigInt *n, const bigInt* base, calc_ctx mrabin_ctx);
uint8_t __BIGINT_BPSW__(const bigInt *n, calc_ctx mrabin_ctx);
uint8_t __BIGINT_ECPP__(const bigInt *n, calc_ctx mrabin_ctx);
uint8_t __BIGINT_PTEST_DISPATCH__(const bigInt *x, calc_ctx ptest_ctx);
#ifdef __cplusplus
}
#endif




#endif
