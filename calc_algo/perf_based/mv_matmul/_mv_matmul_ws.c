#include "_mv_matmul_.h"
#include <debug_util.h>
#include "../../util/aconv_macros.h"
/** ------------ INTRODUCTION ------------
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
    if ( // Toom-cook 3-way
        a_size <= BIGINT_TOOM_3 && b_size <= BIGINT_TOOM_3 && 
        ((T->A.n <= BIGINT_TOOM_3 && T->B.n <= BIGINT_TOOM_3) ||
         (T->C.n <= BIGINT_TOOM_3 && T->D.n <= BIGINT_TOOM_3))
    ) return max(
        __BIGINT_MAT_TOOM3_WS__(a_size, T->A.n, b_size, T->B.n), 
        __BIGINT_MAT_TOOM3_WS__(a_size, T->C.n, b_size, T->D.n)
    );
    // else if ( // Toom-cook 4-way
    //     a_size <= BIGINT_TOOM_4 && b_size <= BIGINT_TOOM_4 && 
    //     ((T->A.n <= BIGINT_TOOM_4 && T->B.n <= BIGINT_TOOM_4) ||
    //      (T->C.n <= BIGINT_TOOM_4 && T->D.n <= BIGINT_TOOM_4))
    // ) return max(
    //     __BIGINT_MAT_TOOM4_WS__(a_size, T->A.n, b_size, T->B.n), 
    //     __BIGINT_MAT_TOOM4_WS__(a_size, T->C.n, b_size, T->D.n)
    // );
    // else if ( // Toom-cook 5-way
    //     a_size <= BIGINT_TOOM_5 && b_size <= BIGINT_TOOM_5 && 
    //     ((T->A.n <= BIGINT_TOOM_5 && T->B.n <= BIGINT_TOOM_5) ||
    //      (T->C.n <= BIGINT_TOOM_5 && T->D.n <= BIGINT_TOOM_5))
    // ) return max(
    //     __BIGINT_MAT_TOOM5_WS__(a_size, T->A.n, b_size, T->B.n), 
    //     __BIGINT_MAT_TOOM5_WS__(a_size, T->C.n, b_size, T->D.n)
    // );
    // else if ( // Toom-cook 6.5-way
    //     a_size <= BIGINT_TOOM_6p5 && b_size <= BIGINT_TOOM_6p5 && 
    //     ((T->A.n <= BIGINT_TOOM_6p5 && T->B.n <= BIGINT_TOOM_6p5) ||
    //      (T->C.n <= BIGINT_TOOM_6p5 && T->D.n <= BIGINT_TOOM_6p5))
    // ) return max(
    //     __BIGINT_MAT_TOOM6p5_WS__(a_size, T->A.n, b_size, T->B.n), 
    //     __BIGINT_MAT_TOOM6p5_WS__(a_size, T->C.n, b_size, T->D.n)
    // );
    // else if ( // Toom-cook 7.5-way
    //     a_size <= BIGINT_TOOM_7p5 && b_size <= BIGINT_TOOM_7p5 && 
    //     ((T->A.n <= BIGINT_TOOM_7p5 && T->B.n <= BIGINT_TOOM_7p5) ||
    //      (T->C.n <= BIGINT_TOOM_7p5 && T->D.n <= BIGINT_TOOM_7p5))
    // ) return max(
    //     __BIGINT_MAT_TOOM7p5_WS__(a_size, T->A.n, b_size, T->B.n), 
    //     __BIGINT_MAT_TOOM7p5_WS__(a_size, T->C.n, b_size, T->D.n)
    // );
    // else if ( // Toom-cook 8.5-way
    //     a_size <= BIGINT_TOOM_8p5 && b_size <= BIGINT_TOOM_8p5 && 
    //     ((T->A.n <= BIGINT_TOOM_8p5 && T->B.n <= BIGINT_TOOM_8p5) ||
    //      (T->C.n <= BIGINT_TOOM_8p5 && T->D.n <= BIGINT_TOOM_8p5))
    // ) return max(
    //     __BIGINT_MAT_TOOM8p5_WS__(a_size, T->A.n, b_size, T->B.n), 
    //     __BIGINT_MAT_TOOM8p5_WS__(a_size, T->C.n, b_size, T->D.n)
    // );
    else return max(
        __BIGINT_MAT_SSA_WS__(a_size, T->A.n, b_size, T->B.n), 
        __BIGINT_MAT_SSA_WS__(a_size, T->C.n, b_size, T->D.n)
    );
}