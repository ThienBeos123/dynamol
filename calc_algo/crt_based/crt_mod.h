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



#ifndef DNML_CRT_MOD_H
#define DNML_CRT_MOD_H



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

#ifdef __cplusplus
extern "C" {
#endif
/* CRYPTINT ALGORITHMS */
dnml_status __CRINT_BARETT_REDC__(const crint *dend, const crint *mod, crint *rem);
dnml_status __CRINT_MONT_REDC__(crint *t, mont_ctx mredc_ctx, crint *rem);
dnml_status __CRINT_MOD_DISP__(const crint *dend, const crint *mod, crint *rem, crint *tmp_quot);
#ifdef __cplusplus
}
#endif


#endif