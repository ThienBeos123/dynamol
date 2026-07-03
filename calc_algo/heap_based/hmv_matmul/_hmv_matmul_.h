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



#ifndef _DNML_BIHEAP_HGCD_MATMUL_H_
#define _DNML_BIHEAP_HGCD_MATMUL_H_





#include <dnml_status.h>
#include <libdnml_types.h>
#include <include.h>
#include <dnml_sys/sys.h>
#include <_libdnml_config/numeric_config.h>
#include <_libdnml_mem/_ctx.h>
#include "../../../intrinsics/intrinsics.h"
#include "../../../util/util.h"
#include "heap_mul.h"


#ifdef __cplusplus
extern "C" {
#endif

/* --------------- Algorithm Functions - Symmetrical  --------------- */
dnml_status __BIHEAP_MATMUL_TOOM3__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res);
dnml_status __BIHEAP_MATMUL_TOOM4__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res);
dnml_status __BIHEAP_MATMUL_TOOM5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res);
dnml_status __BIHEAP_MATMUL_TOOM6p5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res);
dnml_status __BIHEAP_MATMUL_TOOM7p5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res);
dnml_status __BIHEAP_MATMUL_TOOM8p5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res);
dnml_status __BIHEAP_MATMUL_SSA__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res);
/* --------------- Algorithm Functions - XZ Asymmetrical  --------------- */
dnml_status __HASYMXZ_MATMUL_TOOM3__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res);
dnml_status __HASYMXZ_MATMUL_TOOM4__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res);
dnml_status __HASYMXZ_MATMUL_TOOM5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res);
dnml_status __HASYMXZ_MATMUL_TOOM6p5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res);
dnml_status __HASYMXZ_MATMUL_TOOM7p5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res);
dnml_status __HASYMXZ_MATMUL_TOOM8p5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res);
dnml_status __HASYMXZ_MATMUL_SSA__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res);
/* --------------- Algorithm Functions - YW Asymmetrical  --------------- */
dnml_status __HASYMYW_MATMUL_TOOM3__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res);
dnml_status __HASYMYW_MATMUL_TOOM4__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res);
dnml_status __HASYMYW_MATMUL_TOOM5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res);
dnml_status __HASYMYW_MATMUL_TOOM6p5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res);
dnml_status __HASYMYW_MATMUL_TOOM7p5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res);
dnml_status __HASYMYW_MATMUL_TOOM8p5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res);
dnml_status __HASYMYW_MATMUL_SSA__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res);
/* ------------- Algorithm Functions - Symmetrical Asymmetrical ------------- */
dnml_status __HSYM_MATMUL_TOOM3__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res);
dnml_status __HSYM_MATMUL_TOOM4__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res);
dnml_status __HSYM_MATMUL_TOOM5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res);
dnml_status __HSYM_MATMUL_TOOM6p5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res);
dnml_status __HSYM_MATMUL_TOOM7p5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res);
dnml_status __HSYM_MATMUL_TOOM8p5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res);
dnml_status __HSYM_MATMUL_SSA__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res);




/* ---------------- Dispatchers ---------------- */
dnml_status __HMV_ASYM_MATMUL_21__(
    bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w,
    bigInt *const xz_res, bigInt *const yw_res
);
dnml_status __HMV_MATMUL_21__(
    bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w,
    bigInt *const xz_res, bigInt *const yw_res
);
#ifdef __cplusplus
}
#endif


#endif
