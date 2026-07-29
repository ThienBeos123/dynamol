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



#ifndef DNML_DIV_H
#define DNML_DIV_H


#include <dnml_status.h>
#include <libdnml_types.h>
#include <include.h>
#include <dnml_sys/sys.h>
#include <_libdnml_config/numeric_config.h>
#include <_libdnml_mem/_ctx.h>

#include "../../intrinsics/intrinsics.h"
#include "../../util/util.h"

#include "../algo_base/add_sub.h"
#include "mul.h"



#ifdef __cplusplus
extern "C" {
#endif
size_t __BIGINT_SHORTDIV_WS__(size_t a_size, size_t b_size);
size_t __BIGINT_BURNIKEL_WS__(size_t a_size, size_t b_size, bool normalize);
size_t __BIGINT_NEWTON_WS__(size_t a_size, size_t b_size);
size_t __BIGINT_DIV_WS__(size_t a_size, size_t b_size, bool normalize);


void __BIGINT_SHORT_DIVISION__(const bigInt *const a, uint64_t b, bigInt *const quot, bigInt *const rem);
void __RBIGINT_SHORT_DIVISION__(const bigInt *const a, uint64_t b, bigInt *const rem);
void __BIGINT_BURK__(
    const bigInt *const AH, const bigInt *const AL,
    const bigInt *const b, bigInt *const quot, bigInt *const rem,
    calc_ctx *burk_ctx, dnml_status *err, FILE *f
);
void __BIGINT_BURNIKEL__(const bigInt *const a, const bigInt *const b, bigInt *const quot, calc_ctx *burk_ctx, dnml_status *err, FILE *f);
void __BIGINT_NEWTON__(const bigInt *const n, const bigInt *const d, bigInt *const quot, bigInt *const rem, calc_ctx *newton_ctx, dnml_status *err);
void __BIGINT_DIV_DISP__(const bigInt *const n, const bigInt *const d, bigInt *const quot, calc_ctx *div_ctx, dnml_status *err);
#ifdef __cplusplus
}
#endif



#endif
