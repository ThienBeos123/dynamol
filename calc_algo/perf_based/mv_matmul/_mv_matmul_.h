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



#ifndef _DNML_BI_HGCD_MATMUL_H_
#define _DNML_BI_HGCD_MATMUL_H_





#include <dnml_status.h>
#include <libdnml_types.h>
#include <include.h>
#include <dnml_sys/sys.h>
#include <_libdnml_config/numeric_config.h>
#include <_libdnml_mem/_ctx.h>
#include "../../../intrinsics/intrinsics.h"
#include "../../../util/util.h"


#ifdef __cplusplus
extern "C" {
#endif
/* ---------- Sizing Functions - Symmetrical ---------- */
size_t __BIGINT_MAT_TOOM3_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size);
size_t __BIGINT_MAT_TOOM4_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size);
size_t __BIGINT_MAT_TOOM5_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size);
size_t __BIGINT_MAT_SSA_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size);
size_t __BIGINT_MAT_TOOM6p5_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size);
size_t __BIGINT_MAT_TOOM7p5_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size);
size_t __BIGINT_MAT_TOOM8p5_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size);
/* ---------- Sizing Functions - XZ Asymmetrical ---------- */
size_t __ASYMXZ_MAT_TOOM3_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size);
size_t __ASYMXZ_MAT_TOOM4_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size);
size_t __ASYMXZ_MAT_TOOM5_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size);
size_t __ASYMXZ_MAT_SSA_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size);
size_t __ASYMXZ_MAT_TOOM6p5_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size);
size_t __ASYMXZ_MAT_TOOM7p5_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size);
size_t __ASYMXZ_MAT_TOOM8p5_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size);
/* ---------- Sizing Functions - YW Asymmetrical ---------- */
size_t __ASYMYW_MAT_TOOM3_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size);
size_t __ASYMYW_MAT_TOOM4_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size);
size_t __ASYMYW_MAT_TOOM5_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size);
size_t __ASYMYW_MAT_SSA_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size);
size_t __ASYMYW_MAT_TOOM6p5_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size);
size_t __ASYMYW_MAT_TOOM7p5_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size);
size_t __ASYMYW_MAT_TOOM8p5_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size);
/* ---------- Sizing Functions - Symmetrical Asymmetrical ---------- */
size_t __SYM_MATMUL_TOOM3_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size);
size_t __SYM_MATMUL_TOOM4_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size);
size_t __SYM_MATMUL_TOOM5_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size);
size_t __SYM_MATMUL_SSA_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size);
size_t __SYM_MATMUL_TOOM6p5_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size);
size_t __SYM_MATMUL_TOOM7p5_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size);
size_t __SYM_MATMUL_TOOM8p5_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size);


/* --------------- Algorithm Functions - Symmetrical  --------------- */
dnml_status __BIGINT_MATMUL_TOOM3__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res, calc_ctx toom_ctx);
dnml_status __BIGINT_MATMUL_TOOM4__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res, calc_ctx toom_ctx);
dnml_status __BIGINT_MATMUL_TOOM5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res, calc_ctx toom_ctx);
dnml_status __BIGINT_MATMUL_TOOM6p5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res, calc_ctx toom_ctx);
dnml_status __BIGINT_MATMUL_TOOM7p5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res, calc_ctx toom_ctx);
dnml_status __BIGINT_MATMUL_TOOM8p5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res, calc_ctx toom_ctx);
dnml_status __BIGINT_MATMUL_SSA__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res, calc_ctx fft_ctx);
/* --------------- Algorithm Functions - XZ Asymmetrical  --------------- */
dnml_status __ASYMXZ_MATMUL_TOOM3__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res, calc_ctx toom_ctx);
dnml_status __ASYMXZ_MATMUL_TOOM4__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res, calc_ctx toom_ctx);
dnml_status __ASYMXZ_MATMUL_TOOM5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res, calc_ctx toom_ctx);
dnml_status __ASYMXZ_MATMUL_TOOM6p5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res, calc_ctx toom_ctx);
dnml_status __ASYMXZ_MATMUL_TOOM7p5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res, calc_ctx toom_ctx);
dnml_status __ASYMXZ_MATMUL_TOOM8p5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res, calc_ctx toom_ctx);
dnml_status __ASYMXZ_MATMUL_SSA__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res, calc_ctx fft_ctx);
/* --------------- Algorithm Functions - YW Asymmetrical  --------------- */
dnml_status __ASYMYW_MATMUL_TOOM3__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res, calc_ctx toom_ctx);
dnml_status __ASYMYW_MATMUL_TOOM4__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res, calc_ctx toom_ctx);
dnml_status __ASYMYW_MATMUL_TOOM5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res, calc_ctx toom_ctx);
dnml_status __ASYMYW_MATMUL_TOOM6p5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res, calc_ctx toom_ctx);
dnml_status __ASYMYW_MATMUL_TOOM7p5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res, calc_ctx toom_ctx);
dnml_status __ASYMYW_MATMUL_TOOM8p5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res, calc_ctx toom_ctx);
dnml_status __ASYMYW_MATMUL_SSA__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res, calc_ctx fft_ctx);
/* ------------- Algorithm Functions - Symmetrical Asymmetrical ------------- */
dnml_status __SYM_MATMUL_TOOM3__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res, calc_ctx toom_ctx);
dnml_status __SYM_MATMUL_TOOM4__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res, calc_ctx toom_ctx);
dnml_status __SYM_MATMUL_TOOM5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res, calc_ctx toom_ctx);
dnml_status __SYM_MATMUL_TOOM6p5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res, calc_ctx toom_ctx);
dnml_status __SYM_MATMUL_TOOM7p5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res, calc_ctx toom_ctx);
dnml_status __SYM_MATMUL_TOOM8p5__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res, calc_ctx toom_ctx);
dnml_status __SYM_MATMUL_SSA__(bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w, bigInt *const xz_res, bigInt *const yw_res, calc_ctx fft_ctx);




/* ---------------- Dispatchers ---------------- */
size_t __MV_ASYM_MATMUL_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size);
size_t __MV_MATMUL_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size);
dnml_status __MV_ASYM_MATMUL_21__(
    bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w,
    bigInt *const xz_res, bigInt *const yw_res, calc_ctx mul_ctx
);
dnml_status __MV_MATMUL_21__(
    bigInt *const x, bigInt *const z, /**/ bigInt *const y, bigInt *const w,
    bigInt *const xz_res, bigInt *const yw_res, calc_ctx mul_ctx
);
#ifdef __cplusplus
}
#endif


#endif
