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
#define THRESHOLD(a_size, b_size, threshold) (a_size <= threshold && b_size <= threshold)
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



size_t __MV_ASYM_MATMUL_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size) {
    // Symmetrical Asymmetry
    if ((min(x_size, z_size) * 2 <= max(x_size, z_size)) && (min(y_size, w_size) * 2 <= max(y_size, w_size))) {
        size_t xz_Bsize = min(x_size, z_size); // Beta size lol
        size_t yw_Bsize = min(y_size, w_size); // Beta size lol
        size_t xz_Asize = max(x_size, z_size); // Alpha chad size lol
        size_t yw_Asize = max(y_size, w_size); // Alpha chad size lol
        size_t xz_slice = (xz_Asize / ((size_t)(xz_Asize / xz_Bsize) + 1));
        size_t yw_slice = (yw_Asize / ((size_t)(yw_Asize / yw_Bsize) + 1));
        if (THRESHOLD(xz_Bsize, xz_slice, BIGINT_TOOM_3) || THRESHOLD(yw_Asize, yw_slice, BIGINT_TOOM_3)) return __SYM_MATMUL_TOOM3_WS__(x_size, z_size, y_size, w_size);
        // else if (THRESHOLD(xz_Bsize, xz_slice, BIGINT_TOOM_4) || THRESHOLD(yw_Asize, yw_slice, BIGINT_TOOM_4)) return __SYM_MATMUL_TOOM4_WS__(x_size, z_size, y_size, w_size);
        // else if (THRESHOLD(xz_Bsize, xz_slice, BIGINT_TOOM_5) || THRESHOLD(yw_Asize, yw_slice, BIGINT_TOOM_5)) return __SYM_MATMUL_TOOM5_WS__(x_size, z_size, y_size, w_size);
        // else if (THRESHOLD(xz_Bsize, xz_slice, BIGINT_TOOM_6p5) || THRESHOLD(yw_Asize, yw_slice, BIGINT_TOOM_6p5)) return __SYM_MATMUL_TOOM6p5_WS__(x_size, z_size, y_size, w_size);
        // else if (THRESHOLD(xz_Bsize, xz_slice, BIGINT_TOOM_7p5) || THRESHOLD(yw_Asize, yw_slice, BIGINT_TOOM_7p5)) return __SYM_MATMUL_TOOM7p5_WS__(x_size, z_size, y_size, w_size);
        // else if (THRESHOLD(xz_Bsize, xz_slice, BIGINT_TOOM_8p5) || THRESHOLD(yw_Asize, yw_slice, BIGINT_TOOM_8p5)) return __SYM_MATMUL_TOOM8p5_WS__(x_size, z_size, y_size, w_size);
        else return __SYM_MATMUL_SSA_WS__(x_size, z_size, y_size, w_size);
    }
    // XZ Asymmetry
    else if (min(x_size, z_size) * 2 <= max(x_size, z_size)) {
        size_t Bsize = min(x_size, z_size); // Beta size lol
        size_t Asize = max(x_size, z_size); // Alpha chad size lol
        size_t slice = (Asize / ((size_t)(Asize / Bsize) + 1));
        if (THRESHOLD(Bsize, slice, BIGINT_TOOM_3) || THRESHOLD(y_size, w_size, BIGINT_TOOM_3)) return __ASYMXZ_MAT_TOOM3_WS__(x_size, z_size, y_size, w_size);
        // else if (THRESHOLD(Bsize, slice, BIGINT_TOOM_4) || THRESHOLD(y_size, w_size, BIGINT_TOOM_4)) return __ASYMXZ_MAT_TOOM4_WS__(x_size, z_size, y_size, w_size);
        // else if (THRESHOLD(Bsize, slice, BIGINT_TOOM_5) || THRESHOLD(y_size, w_size, BIGINT_TOOM_5)) return __ASYMXZ_MAT_TOOM5_WS__(x_size, z_size, y_size, w_size);
        // else if (THRESHOLD(Bsize, slice, BIGINT_TOOM_6p5) || THRESHOLD(y_size, w_size, BIGINT_TOOM_6p5)) return __ASYMXZ_MAT_TOOM6p5_WS__(x_size, z_size, y_size, w_size);
        // else if (THRESHOLD(Bsize, slice, BIGINT_TOOM_7p5) || THRESHOLD(y_size, w_size, BIGINT_TOOM_7p5)) return __ASYMXZ_MAT_TOOM7p5_WS__(x_size, z_size, y_size, w_size);
        // else if (THRESHOLD(Bsize, slice, BIGINT_TOOM_8p5) || THRESHOLD(y_size, w_size, BIGINT_TOOM_8p5)) return __ASYMXZ_MAT_TOOM8p5_WS__(x_size, z_size, y_size, w_size);
        else return __ASYMXZ_MAT_SSA_WS__(x_size, z_size, y_size, w_size);
    }
    // YZ Asymmetry
    else {
        size_t Bsize = min(y_size, w_size); // Beta size lol
        size_t Asize = max(y_size, w_size); // Alpha chad size lol
        size_t slice = (Asize / ((size_t)(Asize / Bsize) + 1));
        if (THRESHOLD(x_size, z_size, BIGINT_TOOM_3) || THRESHOLD(Bsize, slice, BIGINT_TOOM_3)) return __ASYMYW_MAT_TOOM3_WS__(x_size, z_size, y_size, w_size);
        // else if (THRESHOLD(x_size, z_size, BIGINT_TOOM_4) || THRESHOLD(Bsize, slice, BIGINT_TOOM_4)) return __ASYMYW_MAT_TOOM4_WS__(x_size, z_size, y_size, w_size);
        // else if (THRESHOLD(x_size, z_size, BIGINT_TOOM_5) || THRESHOLD(Bsize, slice, BIGINT_TOOM_5)) return __ASYMYW_MAT_TOOM5_WS__(x_size, z_size, y_size, w_size);
        // else if (THRESHOLD(x_size, z_size, BIGINT_TOOM_6p5) || THRESHOLD(Bsize, slice, BIGINT_TOOM_6p5)) return __ASYMYW_MAT_TOOM6p5_WS__(x_size, z_size, y_size, w_size);
        // else if (THRESHOLD(x_size, z_size, BIGINT_TOOM_7p5) || THRESHOLD(Bsize, slice, BIGINT_TOOM_7p5)) return __ASYMYW_MAT_TOOM7p5_WS__(x_size, z_size, y_size, w_size);
        // else if (THRESHOLD(x_size, z_size, BIGINT_TOOM_8p5) || THRESHOLD(Bsize, slice, BIGINT_TOOM_8p5)) return __ASYMYW_MAT_TOOM8p5_WS__(x_size, z_size, y_size, w_size);
        else return __ASYMYW_MAT_SSA_WS__(x_size, z_size, y_size, w_size);
    }
}



size_t __MV_MATMUL_WS__(size_t x_size, size_t z_size, size_t y_size, size_t w_size) {
    if ((min(x_size, z_size) * 2 <= max(x_size, z_size)) || (min(y_size, w_size) * 2 <= max(y_size, w_size))) return __MV_ASYM_MATMUL_WS__(x_size, z_size, y_size, w_size);
    else if (THRESHOLD(x_size, z_size, BIGINT_TOOM_3) || THRESHOLD(y_size, w_size, BIGINT_TOOM_3)) return __BIGINT_MAT_TOOM3_WS__(x_size, z_size, y_size, w_size);
    // else if (THRESHOLD(x_size, z_size, BIGINT_TOOM_4) || THRESHOLD(y_size, w_size, BIGINT_TOOM_4)) return __BIGINT_MAT_TOOM4_WS__(x_size, z_size, y_size, w_size);
    // else if (THRESHOLD(x_size, z_size, BIGINT_TOOM_5) || THRESHOLD(y_size, w_size, BIGINT_TOOM_5)) return __BIGINT_MAT_TOOM5_WS__(x_size, z_size, y_size, w_size);
    // else if (THRESHOLD(x_size, z_size, BIGINT_TOOM_6p5) || THRESHOLD(y_size, w_size, BIGINT_TOOM_6p5)) return __BIGINT_MAT_TOOM6p5_WS__(x_size, z_size, y_size, w_size);
    // else if (THRESHOLD(x_size, z_size, BIGINT_TOOM_7p5) || THRESHOLD(y_size, w_size, BIGINT_TOOM_7p5)) return __BIGINT_MAT_TOOM7p5_WS__(x_size, z_size, y_size, w_size);
    // else if (THRESHOLD(x_size, z_size, BIGINT_TOOM_8p5) || THRESHOLD(y_size, w_size, BIGINT_TOOM_8p5)) return __BIGINT_MAT_TOOM8p5_WS__(x_size, z_size, y_size, w_size);
    else return __BIGINT_MAT_SSA_WS__(x_size, z_size, y_size, w_size);
}
