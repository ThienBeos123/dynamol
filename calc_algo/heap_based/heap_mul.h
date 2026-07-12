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



#ifndef DNML_HEAP_MUL_H
#define DNML_HEAP_MUL_H


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
/* BIGINT ALGORITHMS - BALANCED */
void __BIHEAP_SCHOOLBOOK__(const bigInt *const a, const bigInt *const b, bigInt *const res);
void __BIHEAP_KARATSUBA__(const bigInt *const x, const bigInt *const y, bigInt *const res, dnml_status *err);
void __BIHEAP_TOOM_3__(const bigInt *const m, const bigInt *const n, bigInt *const res, dnml_status *err);
void __BIHEAP_TOOM_4__(const bigInt *const m, const bigInt *const n, bigInt *const res, dnml_status *err);
void __BIHEAP_TOOM_5__(const bigInt *const m, const bigInt *const n, bigInt *const res, dnml_status *err);
void __BIHEAP_TOOM_6p5__(const bigInt *const m, const bigInt *const n, bigInt *const res, dnml_status *err);
void __BIHEAP_TOOM_7p5__(const bigInt *const m, const bigInt *const n, bigInt *const res, dnml_status *err);
void __BIHEAP_TOOM_8p5__(const bigInt *const m, const bigInt *const n, bigInt *const res, dnml_status *err);
void __BIHEAP_FFT__(const bigInt *const a, const bigInt *const b, bigInt *const res, dnml_status *err);
/* BIGINT ALGORITHMS DISPATCHER */
void __BIHEAP_MUL_DISP__(const bigInt *const a, const bigInt *const b, bigInt *const res, dnml_status *err);
#ifdef __cplusplus
}
#endif



#endif
