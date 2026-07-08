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



#ifndef DNML_MOD_MULEXP_H
#define DNML_MOD_MULEXP_H



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

#ifdef __cplusplus
extern "C" {
#endif
size_t __BIGINT_CMODMUL_WS__(size_t a_size, size_t b_size, size_t mod_size);
size_t __BIGINT_MONTMUL_WS__(size_t a_size, size_t b_size, mont_ctx ctx);
size_t __BIGINT_BIN_MODEXP_WS__(size_t base_size, size_t mod_size, size_t pow_size);
size_t __BIGINT_MBIN_MODEXP_WS__(size_t base_size, size_t mod_size, size_t pow_size);
size_t __BIGINT_FIX_MODEXP_WS__(size_t base_size, size_t mod_size, size_t pow_size, uint8_t k);
size_t __BIGINT_SLIDE_MODEXP_WS__(size_t base_size, size_t mod_size, size_t pow_size, uint8_t k);
size_t __BIGINT_MODMUL_WS__(size_t a_size, size_t b_size, size_t mod_size);
size_t __BIGINT_MODEXP_WS__(size_t base_size, size_t mod_size, size_t pow_size);


void __BIGINT_MONTMUL__(const bigInt *const a, const bigInt *const b, mont_ctx ctx, bigInt *const res, calc_ctx *montmul_ctx, dnml_status *err);
void __BIGINT_CMODMUL__(const bigInt *const a, const bigInt *const b, const bigInt *const mod, bigInt *const res, calc_ctx *modmul_ctx, dnml_status *err);
void __BIGINT_BIN_MODEXP__(const bigInt *const base, const bigInt *const exp, const bigInt *const mod, bigInt *const res, calc_ctx *binexp_ctx, dnml_status *err);
void __BIGINT_MBIN_MODEXP__(const bigInt *const base, const bigInt *const exp, const bigInt *const mod, bigInt *const res, calc_ctx *binexp_ctx, dnml_status *err);
void __BIGINT_FIX_MODEXP__(const bigInt *const base, const bigInt *const exp, const bigInt *const mod, bigInt *const res, uint8_t k, calc_ctx *fix_ctx, dnml_status *err);
void __BIGINT_SLIDE_MODEXP__(const bigInt *const base, const bigInt *const exp, const bigInt *const mod, bigInt *const res, uint8_t k, calc_ctx *slide_ctx, dnml_status *err);
void __BIGINT_MODMUL_DISP__(const bigInt *const a, const bigInt *const b, const bigInt *const mod, bigInt *const res, calc_ctx *modmul_ctx, dnml_status *err);
void __BIGINT_MODEXP_DISP__(const bigInt *const base, const bigInt *const exp, const bigInt *const mod, bigInt *const res, calc_ctx *binexp_ctx, dnml_status *err);
#ifdef __cplusplus
}
#endif





#endif
