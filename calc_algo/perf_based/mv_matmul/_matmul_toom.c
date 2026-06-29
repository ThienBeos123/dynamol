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



#include "_mv_matmul_.h"
#include <debug_util.h>
#include "../../util/aconv_macros.h"
/**
 * THIS FILE CONTAINS THE FOLLOWING ALGORITHMS FOR MATRIX MULTIPLICATION:
 *
 *      - Toom-cook 6.5-way
 *      - Toom-cook 7.6-way
 *      - Toom-cook 8.5-way
 *
 * In which they would be structurally modify to accompany the multiplications
 * of a single linear-combination pair of the form xz + yw (as utilized by hgcd)
 */
/* ---------- Function Workspace Size Returners ---------- */
size_t __BIGINT_MAT_TOOM6p5_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size) { return 0; }
size_t __BIGINT_MAT_TOOM7p5_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size) { return 0; }
size_t __BIGINT_MAT_TOOM8p5_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size) { return 0; }








/* ----------- BigInt Matrix Multiplication Toom-cook 6.5-way ----------- */
dnml_status __BIGINT_MATMUL_TOOM6p5__(
    P_BIGINT x, P_BIGINT z, /**/ P_BIGINT y, P_BIGINT w, 
    P_BIGINT xz_res, P_BIGINT yw_res, calc_ctx toom_ctx
) { return BIGINT_SUCCESS; }






/* ----------- BigInt Matrix Multiplication Toom-cook 6.5-way ----------- */
dnml_status __BIGINT_MATMUL_TOOM7p5__(
    P_BIGINT x, P_BIGINT z, /**/ P_BIGINT y, P_BIGINT w, 
    P_BIGINT xz_res, P_BIGINT yw_res, calc_ctx toom_ctx
) { return BIGINT_SUCCESS; }






/* ----------- BigInt Matrix Multiplication Toom-cook 6.5-way ----------- */
dnml_status __BIGINT_MATMUL_TOOM8p5__(
    P_BIGINT x, P_BIGINT z, /**/ P_BIGINT y, P_BIGINT w, 
    P_BIGINT xz_res, P_BIGINT yw_res, calc_ctx toom_ctx
) { return BIGINT_SUCCESS; }

