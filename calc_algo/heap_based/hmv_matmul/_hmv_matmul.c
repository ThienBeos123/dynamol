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
#define THRESHOLD(a_size, b_size, threshold) (a_size <= threshold && b_size <= threshold)
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



/* --------------- Asymmetrical Dispatcher --------------- */
dnml_status __HMV_ASYM_MATMUL_21__(
    P_BIGINT x, P_BIGINT z, /**/ P_BIGINT y, P_BIGINT w,
    P_BIGINT xz_res, P_BIGINT yw_res
) {
    // Symmetrical Asymmetry
    if ((min(x->n, z->n) * 2 <= max(x->n, z->n)) && (min(y->n, w->n) * 2 <= max(y->n, w->n))) {
        size_t xz_Bsize = min(x->n, z->n); // Beta size lol
        size_t yw_Bsize = min(y->n, w->n); // Beta size lol
        size_t xz_Asize = max(x->n, z->n); // Alpha chad size lol
        size_t yw_Asize = max(y->n, w->n); // Alpha chad size lol
        size_t xz_slice = (xz_Asize / ((size_t)(xz_Asize / xz_Bsize) + 1));
        size_t yw_slice = (yw_Asize / ((size_t)(yw_Asize / yw_Bsize) + 1));
        if (THRESHOLD(xz_Bsize, xz_slice, BIGINT_TOOM_3) || THRESHOLD(yw_Asize, yw_slice, BIGINT_TOOM_3)) return __HSYM_MATMUL_TOOM3__(x, z, y, w, xz_res, yw_res);
        // else if (THRESHOLD(xz_Bsize, xz_slice, BIGINT_TOOM_4) || THRESHOLD(yw_Asize, yw_slice, BIGINT_TOOM_4)) return __HSYM_MATMUL_TOOM4__(x, z, y, w, xz_res, yw_res);
        // else if (THRESHOLD(xz_Bsize, xz_slice, BIGINT_TOOM_5) || THRESHOLD(yw_Asize, yw_slice, BIGINT_TOOM_5)) return __HSYM_MATMUL_TOOM5__(x, z, y, w, xz_res, yw_res);
        // else if (THRESHOLD(xz_Bsize, xz_slice, BIGINT_TOOM_6p5) || THRESHOLD(yw_Asize, yw_slice, BIGINT_TOOM_6p5)) return __HSYM_MATMUL_TOOM6p5__(x, z, y, w, xz_res, yw_res);
        // else if (THRESHOLD(xz_Bsize, xz_slice, BIGINT_TOOM_7p5) || THRESHOLD(yw_Asize, yw_slice, BIGINT_TOOM_7p5)) return __HSYM_MATMUL_TOOM7p5__(x, z, y, w, xz_res, yw_res);
        // else if (THRESHOLD(xz_Bsize, xz_slice, BIGINT_TOOM_8p5) || THRESHOLD(yw_Asize, yw_slice, BIGINT_TOOM_8p5)) return __HSYM_MATMUL_TOOM8p5__(x, z, y, w, xz_res, yw_res);
        else return __HSYM_MATMUL_SSA__(x, z, y, w, xz_res, yw_res);
    }
    // XZ Asymmetry
    else if (min(x->n, z->n) * 2 <= max(x->n, z->n)) {
        size_t Bsize = min(x->n, z->n); // Beta size lol
        size_t Asize = max(x->n, z->n); // Alpha chad size lol
        size_t slice = (Asize / ((size_t)(Asize / Bsize) + 1));
        if (THRESHOLD(Bsize, slice, BIGINT_TOOM_3) || THRESHOLD(y->n, w->n, BIGINT_TOOM_3)) return __HASYMXZ_MATMUL_TOOM3__(x, z, y, w, xz_res, yw_res);
        // else if (THRESHOLD(Bsize, slice, BIGINT_TOOM_4) || THRESHOLD(y->n, w->n, BIGINT_TOOM_4)) return __HASYMXZ_MATMUL_TOOM4__(x, z, y, w, xz_res, yw_res);
        // else if (THRESHOLD(Bsize, slice, BIGINT_TOOM_5) || THRESHOLD(y->n, w->n, BIGINT_TOOM_5)) return __HASYMXZ_MATMUL_TOOM5__(x, z, y, w, xz_res, yw_res);
        // else if (THRESHOLD(Bsize, slice, BIGINT_TOOM_6p5) || THRESHOLD(y->n, w->n, BIGINT_TOOM_6p5)) return __HASYMXZ_MATMUL_TOOM6p5__(x, z, y, w, xz_res, yw_res);
        // else if (THRESHOLD(Bsize, slice, BIGINT_TOOM_7p5) || THRESHOLD(y->n, w->n, BIGINT_TOOM_7p5)) return __HASYMXZ_MATMUL_TOOM7p5__(x, z, y, w, xz_res, yw_res);
        // else if (THRESHOLD(Bsize, slice, BIGINT_TOOM_8p5) || THRESHOLD(y->n, w->n, BIGINT_TOOM_8p5)) return __HASYMXZ_MATMUL_TOOM8p5__(x, z, y, w, xz_res, yw_res);
        else return __HASYMXZ_MATMUL_SSA__(x, z, y, w, xz_res, yw_res);
    }
    // YZ Asymmetry
    else {
        size_t Bsize = min(y->n, w->n); // Beta size lol
        size_t Asize = max(y->n, w->n); // Alpha chad size lol
        size_t slice = (Asize / ((size_t)(Asize / Bsize) + 1));
        if (THRESHOLD(x->n, z->n, BIGINT_TOOM_3) || THRESHOLD(Bsize, slice, BIGINT_TOOM_3)) return __HASYMYW_MATMUL_TOOM3__(x, z, y, w, xz_res, yw_res);
        // else if (THRESHOLD(x->n, z->n, BIGINT_TOOM_4) || THRESHOLD(Bsize, slice, BIGINT_TOOM_4)) return __HASYMYW_MATMUL_TOOM4__(x, z, y, w, xz_res, yw_res);
        // else if (THRESHOLD(x->n, z->n, BIGINT_TOOM_5) || THRESHOLD(Bsize, slice, BIGINT_TOOM_5)) return __HASYMYW_MATMUL_TOOM5__(x, z, y, w, xz_res, yw_res);
        // else if (THRESHOLD(x->n, z->n, BIGINT_TOOM_6p5) || THRESHOLD(Bsize, slice, BIGINT_TOOM_6p5)) return __HASYMYW_MATMUL_TOOM6p5__(x, z, y, w, xz_res, yw_res);
        // else if (THRESHOLD(x->n, z->n, BIGINT_TOOM_7p5) || THRESHOLD(Bsize, slice, BIGINT_TOOM_7p5)) return __HASYMYW_MATMUL_TOOM7p5__(x, z, y, w, xz_res, yw_res);
        // else if (THRESHOLD(x->n, z->n, BIGINT_TOOM_8p5) || THRESHOLD(Bsize, slice, BIGINT_TOOM_8p5)) return __HASYMYW_MATMUL_TOOM8p5__(x, z, y, w, xz_res, yw_res);
        else return __HASYMYW_MATMUL_SSA__(x, z, y, w, xz_res, yw_res);
    }
}





/* -------------- Main Function -------------- */
dnml_status __HMV_MATMUL_21__(
    P_BIGINT x, P_BIGINT z, /**/ P_BIGINT y, P_BIGINT w,
    P_BIGINT xz_res, P_BIGINT yw_res
) {
    if ((min(x->n, z->n) * 2 <= max(x->n, z->n)) || (min(y->n, w->n) * 2 <= max(y->n, w->n))) return __HMV_ASYM_MATMUL_21__(x, z, y, w, xz_res, yw_res);
    else if (THRESHOLD(x->n, z->n, BIGINT_TOOM_3) || THRESHOLD(y->n, w->n, BIGINT_TOOM_3)) return __BIHEAP_MATMUL_TOOM3__(x, z, y, w, xz_res, yw_res);
    // else if (THRESHOLD(x->n, z->n, BIGINT_TOOM_4) || THRESHOLD(y->n, w->n, BIGINT_TOOM_4)) return __BIHEAP_MATMUL_TOOM4__(x, z, y, w, xz_res, yw_res);
    // else if (THRESHOLD(x->n, z->n, BIGINT_TOOM_5) || THRESHOLD(y->n, w->n, BIGINT_TOOM_5)) return __BIHEAP_MATMUL_TOOM5__(x, z, y, w, xz_res, yw_res);
    // else if (THRESHOLD(x->n, z->n, BIGINT_TOOM_6p5) || THRESHOLD(y->n, w->n, BIGINT_TOOM_6p5)) return __BIHEAP_MATMUL_TOOM6p5__(x, z, y, w, xz_res, yw_res);
    // else if (THRESHOLD(x->n, z->n, BIGINT_TOOM_7p5) || THRESHOLD(y->n, w->n, BIGINT_TOOM_7p5)) return __BIHEAP_MATMUL_TOOM7p5__(x, z, y, w, xz_res, yw_res);
    // else if (THRESHOLD(x->n, z->n, BIGINT_TOOM_8p5) || THRESHOLD(y->n, w->n, BIGINT_TOOM_8p5)) return __BIHEAP_MATMUL_TOOM8p5__(x, z, y, w, xz_res, yw_res);
    else return __BIHEAP_MATMUL_SSA__(x, z, y, w, xz_res, yw_res);
}
