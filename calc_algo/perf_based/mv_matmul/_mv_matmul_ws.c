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
/**  ----------- Matrix-Vector Multiplication Linear Combination -----------
 * This files contains the following function:
 *
 *  + __MV_ASYM_MATMUL_WS__() - Asymmetrical Matrix Multiplication Size Calculation
 *  + __MV_MATMUL_WS__() - General Linear Combination Matrix Multiplication Size Calculation
 *
 * These functions calculates the workspace scratch size requirement
 * of functions in "_hgcd_matmul_fft.c", "_hgcd_matmul_toom.c", and "_hgcd_matmul_disp.c".
 * These function solely serves the purpose of multiplying one of the two linear combination
 * of a 2x2 * 2x1 Matrix-Vector Multiplication of a 2x2 Matrix and 2x1 Column Vector with
 * the following form:
 *
 *          X * Z + Y * W
 */



size_t __MV_ASYM_MATMUL_WS__(struct rt_matrix *T, size_t a_size, size_t b_size) { return 0; } // For future-use





// TODO: ADD ASYMMETRY HANDLING
size_t __MV_MATMUL_WS__(struct rt_matrix *T, size_t a_size, size_t b_size) {
    return 0;
}
