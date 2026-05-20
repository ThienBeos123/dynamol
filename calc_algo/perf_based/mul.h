#ifndef DNML_MUL_H
#define DNML_MUL_H



#ifdef __cplusplus
extern "C" {
#endif

#include <libdnml_types.h>
#include <include.h>
#include <dnml_sys/sys.h>
#include <_libdnml_config/numeric_config.h>
#include <_libdnml_mem/_ctx.h>
#include "../../intrinsics/intrinsics.h"
#include "../../util/util.h"
#include "../algo_base/add_sub.h"

/* BIGINT WORKSPACE SIZE */
size_t __BIGINT_KARATSUBA_WS__(size_t x_size, size_t y_size);
size_t __BIGINT_TOOM_3_WS__(size_t m_size, size_t n_size);
size_t __BIGINT_TOOM_4_WS__(size_t m_size, size_t n_size);
size_t __BIGINT_TOOM_5_WS__(size_t m_size, size_t n_size);
size_t __BIGINT_TOOM_6p5_WS__(size_t m_size, size_t n_size);
size_t __BIGINT_TOOM_7p5_WS__(size_t m_size, size_t n_size);
size_t __BIGINT_TOOM_8p5_WS__(size_t m_size, size_t n_size);
size_t __BIGINT_SSA_WS__(size_t a_size, size_t b_size);
size_t __BIGINT_MUL_WS__(size_t a_size, size_t b_size);

/* BIGINT ALGORITHMS */
void __BIGINT_SCHOOLBOOK__(const bigInt *a, const bigInt *b, bigInt *res);
void __BIGINT_KARATSUBA__(const bigInt *x, const bigInt *y, bigInt *res, calc_ctx karat_ctx);
void __BIGINT_TOOM_3__(const bigInt *m, const bigInt *n, bigInt *res, calc_ctx toom_ctx);
void __BIGINT_TOOM_4__(const bigInt *m, const bigInt *n, bigInt *res, calc_ctx toom_ctx);
void __BIGINT_TOOM_5__(const bigInt *m, const bigInt *n, bigInt *res, calc_ctx toom_ctx);
void __BIGINT_TOOM_6p5__(const bigInt *m, const bigInt *n, bigInt *res, calc_ctx toom_ctx);
void __BIGINT_TOOM_7p5__(const bigInt *m, const bigInt *n, bigInt *res, calc_ctx toom_ctx);
void __BIGINT_TOOM_8p5__(const bigInt *m, const bigInt *n, bigInt *res, calc_ctx toom_ctx);
void __BIGINT_SSA__(const bigInt *a, const bigInt *b, bigInt *res, calc_ctx ssa_ctx);
void __BIGINT_MUL_DISPATCH__(const bigInt *a, const bigInt *b, bigInt *res, calc_ctx mul_ctx);


#ifdef __cplusplus
}
#endif



#endif
