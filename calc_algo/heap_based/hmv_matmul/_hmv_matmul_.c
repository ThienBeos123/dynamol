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



#include "_hmv_matmul_.h"
#include <debug_util.h>
#include "../../util/aconv_macros.h"
/** ----------- Matrix-Vector Multiplication Linear Combination -----------
 * This files contains the following function:
 *
 *  + __HMV_ASYM_MATMUL_21__() - Asymmetrical Matrix Multiplication Size Calculation
 *  + __HMV_MATMUL_21__() - General Linear Combination Matrix Multiplication Size Calculation
 *
 * These functions dispatches algorithms in "_hgcd_matmul_fft.c" AND "_hgcd_matmul_toom.c".
 * These function solely serves the purpose of multiplying one of the two linear combination
 * of a 2x2 * 2x1 Matrix-Vector Multiplication of a 2x2 Matrix and 2x1 Column Vector with
 * the following form:
 *
 *          X * Z + Y * W
 */
dnml_status __HMV_ASYM_MATMUL_21__(
    P_BIGINT x, P_BIGINT z, /**/ P_BIGINT y, P_BIGINT w,
    P_BIGINT xz_res, P_BIGINT yw_res, calc_ctx mul_ctx
) { return BIGINT_SUCCESS; }





dnml_status __HMV_MATMUL_21__(
    P_BIGINT x, P_BIGINT z, /**/ P_BIGINT y, P_BIGINT w,
    P_BIGINT xz_res, P_BIGINT yw_res, calc_ctx mul_ctx
) { return BIGINT_SUCCESS; }
