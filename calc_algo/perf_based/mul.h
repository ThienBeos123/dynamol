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



#ifndef DNML_MUL_H
#define DNML_MUL_H


#include <debug_util.h>
#include <dnml_status.h>
#include <libdnml_types.h>
#include <include.h>
#include <dnml_sys/sys.h>
#include <_libdnml_config/numeric_config.h>
#include <_libdnml_mem/_ctx.h>
#include "../../intrinsics/intrinsics.h"
#include "../../util/util.h"
#include "../algo_base/add_sub.h"


#ifdef __cplusplus
extern "C" {
#endif
/* BIGINT WORKSPACE SIZE */
size_t __BIGINT_KARATSUBA_WS__(size_t x_size, size_t y_size);
size_t __BIGINT_TOOM_3_WS__(size_t m_size, size_t n_size);
size_t __BIGINT_TOOM_4_WS__(size_t m_size, size_t n_size);
size_t __BIGINT_TOOM_5_WS__(size_t m_size, size_t n_size);
size_t __BIGINT_TOOM_6p5_WS__(size_t m_size, size_t n_size);
size_t __BIGINT_TOOM_7p5_WS__(size_t m_size, size_t n_size);
size_t __BIGINT_TOOM_8p5_WS__(size_t m_size, size_t n_size);
size_t __BIGINT_SSA_WS__(size_t a_size, size_t b_size);
size_t __BIGINT_MUL_WS__(size_t a_size, size_t b_size);

/* BIGINT ALGORITHMS */
void __BIGINT_SCHOOLBOOK__(const bigInt *a, const bigInt *b, bigInt *res);
void __BIGINT_KARATSUBA__(const bigInt *x, const bigInt *y, bigInt *res, calc_ctx karat_ctx);
void __BIGINT_TOOM_3__(const bigInt *m, const bigInt *n, bigInt *res, calc_ctx toom_ctx);
void __BIGINT_TOOM_4__(const bigInt *m, const bigInt *n, bigInt *res, calc_ctx toom_ctx);
void __BIGINT_TOOM_5__(const bigInt *m, const bigInt *n, bigInt *res, calc_ctx toom_ctx);
void __BIGINT_TOOM_6p5__(const bigInt *m, const bigInt *n, bigInt *res, calc_ctx toom_ctx);
void __BIGINT_TOOM_7p5__(const bigInt *m, const bigInt *n, bigInt *res, calc_ctx toom_ctx);
void __BIGINT_TOOM_8p5__(const bigInt *m, const bigInt *n, bigInt *res, calc_ctx toom_ctx);
void __BIGINT_NTT__(const bigInt *a, const bigInt *b, bigInt *res, calc_ctx ssa_ctx);
void __BIGINT_MUL_DISPATCH__(const bigInt *a, const bigInt *b, bigInt *res, calc_ctx mul_ctx);
#ifdef __cplusplus
}
#endif



#endif
