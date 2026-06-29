#include "_mv_matmul_.h"
#include <debug_util.h>
#include "../../util/aconv_macros.h"
/** ------------ INTRODUCTION ------------
 * This files contains the following function:
 *
 *  + __MV_ASYM_MATMUL_21__() - Asymmetrical Matrix Multiplication Size Calculation
 *  + __MV_MATMUL_21__() - General Linear Combination Matrix Multiplication Size Calculation
 *
 * These functions dispatches algorithms in "_hgcd_matmul_fft.c" AND "_hgcd_matmul_toom.c".
 * These function solely serves the purpose of multiplying one of the two linear combination
 * of a 2x2 * 2x1 Matrix-Vector Multiplication of a 2x2 Matrix and 2x1 Column Vector with
 * the following form:
 *
 *          X * Z + Y * W
 */




dnml_status __MV_ASYM_MATMUL_21__(
    P_BIGINT x, P_BIGINT z, /**/ P_BIGINT y, P_BIGINT w,
    P_BIGINT xz_res, P_BIGINT yw_res, calc_ctx mul_ctx
) { return BIGINT_SUCCESS; }





dnml_status __MV_MATMUL_21__(
    P_BIGINT x, P_BIGINT z, /**/ P_BIGINT y, P_BIGINT w,
    P_BIGINT xz_res, P_BIGINT yw_res, calc_ctx mul_ctx
) { return BIGINT_SUCCESS; }