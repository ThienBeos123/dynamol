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



#ifndef DNML_HEAP_NUM_THEORY_H
#define DNML_HEAP_NUM_THEORY_H


#include <dnml_status.h>
#include <libdnml_types.h>
#include <include.h>
#include <dnml_sys/sys.h>
#include <_libdnml_config/numeric_config.h>
#include "../../intrinsics/intrinsics.h"
#include "../../util/util.h"
#include "../algo_base/add_sub.h"
#include "heap_div.h"
#include "heap_mul.h"
#include "heap_mod.h"
#include "heap_modmulexp.h"

#ifdef __cplusplus
extern "C" {
#endif
//* ======== Primality Testing - ALGORITHMS ======== *//
uint8_t __BIHEAP_TRIAL_DIV__(uint64_t x);
uint8_t __BIHEAP_SMALL_MRABIN__(uint64_t n);
uint8_t __BIHEAP_MILLER_RABIN__(const bigInt *const n, const bigInt* base, dnml_status *err);
uint8_t __BIHEAP_BPSW__(const bigInt *const n, dnml_status *err);
uint8_t __BIHEAP_ECPP__(const bigInt *const n, dnml_status *err);
uint8_t __BIHEAP_PTEST_DISP__(const bigInt *const x, dnml_status *err);
#ifdef __cplusplus
}
#endif




#endif