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



#ifndef DNML_ADD_SUB
#define DNML_ADD_SUB


#include <debug_util.h>
#include <libdnml_types.h>
#include <include.h>
#include <dnml_status.h>
#include <dnml_sys/sys.h>
#include "../../intrinsics/intrinsics.h"
#include "../../util/util.h"
#include "../../util/crt_util.h"


#ifdef __cplusplus
extern "C" {
#endif
void __BIGINT_ADD_WC__(bigInt *res, const bigInt *a, const bigInt *b);
void __BIGINT_ADD_SAW__(bigInt *res, const bigInt *x, const bigInt *y);
void __BIGINT_SUB_WB__(bigInt *res, const bigInt *a, const bigInt *b);
void __BIGINT_SUB_SAW__(bigInt *res, const bigInt *x, const bigInt *y);
dnml_status __CRINT_ADD_WC__(crint *res, crint *a, crint *b);
dnml_status __CRINT_SUB_WC__(crint *res, crint *a, crint *b);
#ifdef __cplusplus
}
#endif


#endif
