#ifndef DNML_POW_ROOT_H
#define DNML_POW_ROOT_H


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
#include "div.h"
#include "mul.h"


/* ----------------- WORKSPACE ----------------- */
size_t __BIGINT_BINEXP_WS__(size_t base_size, uint64_t pow);
size_t __BIGINT_2K_ARY_WS__(size_t base_size, uint64_t pow, uint8_t ksize);
size_t __BIGINT_SLIDIN_WS__(size_t base_size, uint64_t pow, uint8_t ksize);
size_t __BIGINT_HERON_WS__(size_t a_size);
size_t __BIGINT_NEWTON_CBRT_WS__(size_t a_size);
size_t __BIGINT_NEWTON_2NROOT_WS__(size_t a_size, uint64_t root);
size_t __BIGINT_NEWTON_NROOT_WS__(size_t a_size, uint64_t root);



/* ----------------- ALGORITHMS ----------------- */
void __BIGINT_BINARY_EXP__(bigInt *res, const bigInt *base, uint64_t power, calc_ctx binexp_ctx);
void __BIGINT_2K_ARY__(bigInt *res, const bigInt *base, uint64_t power, uint8_t k, calc_ctx fixed_ctx);
void __BIGINT_SLIDING__(bigInt *res, const bigInt *base, uint64_t power, uint8_t k, calc_ctx slide_ctx);
void __BIGINT_HERON__(bigInt *res, const bigInt *a, calc_ctx heron_ctx);
void __BIGINT_NEWTON_CBRT__(bigInt *res, const bigInt *a, calc_ctx cbrt_ctx);
uint64_t __UI64_NROOT__(uint64_t a, uint64_t root);
void __BIGINT_NEWTON_2NROOT__(bigInt *res, const bigInt *a, uint64_t root, calc_ctx _2nroot_ctx);
void __BIGINT_NEWTON_NROOT__(bigInt *res, const bigInt *a, uint64_t root, calc_ctx nroot_ctx);



/* ----------------- DISPATCHES ----------------- */
size_t __BIGINT_EXP_WS__(size_t base_size, uint64_t pow);
size_t __BIGINT_SQRT_WS__(size_t a_size);
size_t __BIGINT_CBRT_WS__(size_t a_size);
size_t __BIGINT_NROOT_WS__(size_t a_size, uint64_t root);
void __BIGINT_EXP_DISPATCH__(bigInt *res, const bigInt *base, uint64_t power, calc_ctx exp_ctx);
void __BIGINT_SQRT_DISPATCH__(bigInt *res, const bigInt *a, calc_ctx sqrt_ctx);
void __BIGINT_CBRT_DISPATCH__(bigInt *res, const bigInt *a, calc_ctx cbrt_ctx);
void __BIGINT_NROOT_DISPATCH__(bigInt *res, const bigInt *a, uint64_t root, calc_ctx nroot_ctx);



#ifdef __cplusplus
}
#endif


#endif
