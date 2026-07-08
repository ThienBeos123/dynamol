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



#ifndef DNML_PRIME_TEST_H
#define DNML_PRIME_TEST_H

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
#include "mod_mulexp.h"
#include "pow_root.h"

#ifdef __cplusplus
extern "C" {
#endif
/* ======== Primality Testing - WORKSPACE RETURNER ======== */
size_t __BIGINT_MRABIN_WS__(size_t n_size, size_t base_size);
size_t __BIGINT_BPSW_WS__(size_t n_size);
size_t __BIGINT_ECPP_WS__(size_t n_size);
size_t __BIGINT_PTEST_WS__(size_t x_size);
/* ======== Primality Testing - ALGORITHMS ======== */
uint8_t __BIGINT_TRIAL_DIV__(uint64_t x);
uint8_t __BIGINT_SMALL_MRABIN__(uint64_t n);
uint8_t __BIGINT_MILLER_RABIN__(const bigInt *const n, const bigInt *const base, calc_ctx *rabin_ctx, dnml_status *err);
uint8_t __BIGINT_BPSW__(const bigInt *const n, calc_ctx *bpsw_ctx, dnml_status *err);
uint8_t __BIGINT_ECPP__(const bigInt *const n, calc_ctx *ecpp_ctx, dnml_status *err);
uint8_t __BIGINT_PTEST_DISP__(const bigInt *const x, calc_ctx *ptest_ctx, dnml_status *err);
#ifdef __cplusplus
}
#endif




#endif
