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



#ifndef DNML_HEAP_DIV_H
#define DNML_HEAP_DIV_H


#include <dnml_status.h>
#include <debug_util.h>
#include <libdnml_types.h>
#include <include.h>
#include <dnml_sys/sys.h>
#include <_libdnml_config/numeric_config.h>
#include "../../intrinsics/intrinsics.h"
#include "../../util/util.h"
#include "../algo_base/add_sub.h"
#include "heap_mul.h"



#ifdef __cplusplus
extern "C" {
#endif
void __BIHEAP_SHORT_DIVISION__(const bigInt *const a, uint64_t b, bigInt *const quot, bigInt *const rem);
void __BIHEAP_KNUTH_D__(const bigInt *const a, const bigInt *const b, bigInt *const quot, bigInt *const rem, dnml_status *err);
void __BIHEAP_BURNIKEL__(
    const bigInt *const AH, const bigInt *const AL,
    const bigInt *const b, bigInt *const quot, bigInt *const rem, dnml_status *err
);
void __BIHEAP_NEWTON__(const bigInt *const a, const bigInt *const b, bigInt *const quot, bigInt *const rem, dnml_status *err);
void __RBIHEAP_SHORT_DIVISION__(const bigInt *const a, uint64_t b, bigInt *const quot, bigInt *const rem);
void __RBIHEAP_KNUTH_D__(const bigInt *const a, const bigInt *const b, bigInt *const quot, bigInt *const rem, dnml_status *err);
void __RBIHEAP_NEWTON__(const bigInt *const a, const bigInt *const b, bigInt *const quot, bigInt *const rem, dnml_status *err);
void __BIHEAP_DIV_DISP__(const bigInt *const a, const bigInt *const b, bigInt *const quot, bigInt *const rem, dnml_status *err);
#ifdef __cplusplus
}
#endif



#endif
