#ifndef DNML_POW_ROOT_H
#define DNML_POW_ROOT_H


#ifdef __cplusplus
extern "C" {
#endif



#include <include.h>
#include <system/sys.h>
#include "../../sconfigs/settings/numeric_config.h"
#include "../../sconfigs/memory/_ctx.h"

#include "../../intrinsics/intrinsics.h"
#include "../../util/util.h"
#include "../../adynamol/big_numbers/bigNums.h"

#include "add_sub.h"
#include "div.h"
#include "mul.h"


/* ----------------- WORKSPACE ----------------- */
size_t __BIGINT_EXP_WS__(size_t base_size, uint64_t pow);
size_t __BIGINT_SQRT_WS__(size_t a_size);
size_t __BIGINT_CBRT_WS__(size_t a_size);
size_t __BIGINT_NROOT_WS__(size_t a_size, uint64_t root);



/* ----------------- ALGORITHMS ----------------- */
void __BIGINT_EXP_DISPATCH__(bigInt *res, const bigInt *base, uint64_t power, calc_ctx exp_ctx);
void __BIGINT_SQRT_DISPATCH__(bigInt *res, const bigInt *a, calc_ctx sqrt_ctx);
void __BIGINT_CBRT_DISPATCH__(bigInt *res, const bigInt *a, calc_ctx cbrt_ctx);
void __BIGINT_NROOT_DISPATCH__(bigInt *res, const bigInt *a, uint64_t root, calc_ctx nroot_ctx);



#ifdef __cplusplus
}
#endif


#endif
