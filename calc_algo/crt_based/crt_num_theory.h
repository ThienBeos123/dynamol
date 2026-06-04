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



#ifndef DNML_CRT_MODOP_H
#define DNML_CRT_MODOP_H



#include <libdnml_types.h>
#include <include.h>
#include <dnml_sys/sys.h>
#include <_libdnml_config/numeric_config.h>
#include <_libdnml_mem/_ctx.h>
#include "../../intrinsics/intrinsics.h"
#include "../../util/crt_util.h"
#include "../algo_base/add_sub.h"
#include "crt_mul.h"
#include "crt_div.h"
#include "crt_mod.h"
#include "crt_modop.h"
#ifdef __cplusplus
extern "C" {
#endif
/* ------------- GCD ALGORITHMS ------------- */
uint64_t __CRINT_EUCLID__(uint64_t u, uint64_t v);
void __CRINT_STEIN__(crint *res, const crint *u, const crint *v);
void __CRINT_LEHMER__(crint *res, const crint *u, const crint *v);
void __CRINT_HALF__(crint *res, const crint *i, const crint *v);
void __CRINT_GCD_DISP__(crint *res, const crint *u, const crint *v);
/* ------------- PRIMALITY TESTING ALGORITHMS -------------  */
uint8_t __CRINT_TRIAL_DIV__(uint64_t n);
uint8_t __CRINT_SMALL_MRABIN__(uint64_t n);
uint8_t __CRINT_MILLER_RABIN__(const crint *n, const crint *base);
uint8_t __CRINT_BPSW__(const crint *n);
uint8_t __CRINT_ECPP__(const crint *n);
uint8_t __CRINT_PTEST_DISP__(const crint *n);
#ifdef __cplusplus
}
#endif


#endif