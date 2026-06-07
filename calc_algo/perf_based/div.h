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
#include <debug_util.h>
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
size_t __BIGINT_KNUTH_WS__(size_t a_size, size_t b_size);
size_t __BIGINT_NEWTON_WS__(size_t a_size, size_t b_size);
size_t __BIGINT_DIV_WS__(size_t a_size, size_t b_size);


void __BIGINT_SHORT_DIVISION__(const bigInt *a, uint64_t b, bigInt *quot, bigInt *rem);
void __BIGINT_KNUTH_D__(const bigInt *a, const bigInt *b, bigInt *quot, bigInt *rem, calc_ctx knuth_ctx);
void __BIGINT_BURNIKEL__(
    const bigInt *AH, const bigInt *AL,
    const bigInt *b, bigInt *quot, bigInt *rem, calc_ctx burk_ctx
);
void __BIGINT_NEWTON__(const bigInt *a, const bigInt *b, bigInt *quot, bigInt *rem, calc_ctx newton_ctx);
void __BIGINT_DIV_DISPATCH__(const bigInt *a, const bigInt *b, bigInt *quot, bigInt *rem, calc_ctx div_ctx);
#ifdef __cplusplus
}
#endif



#endif
