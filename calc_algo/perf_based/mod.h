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



#ifndef DNML_MOD_H
#define DNML_MOD_H


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

#ifdef __cplusplus
extern "C" {
#endif
size_t __BIGINT_BARETT_WS__(size_t a_size, size_t n_size);
size_t __BIGINT_MOD_WS__(size_t a_size, size_t n_size);
void __BIGINT_BARETT__(const bigInt *const a, const bigInt *const n, bigInt *const rem, calc_ctx barett_ctx, dnml_status *err);
void __BIGINT_MONT_REDC__(bigInt *const t, mont_ctx mredc_ctx, bigInt *const rem);
void __BIGINT_MOD_DISP__(
    const bigInt *const a, const bigInt *const n,
    bigInt *const rem, bigInt *const tmp_quot,
    calc_ctx mod_ctx, dnml_status *err
);
#ifdef __cplusplus
}
#endif


#endif
