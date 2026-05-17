#ifndef DNML_NUM_THEORY_H
#define DNML_NUM_THEORY_H



#ifdef __cplusplus
extern "C" {
#endif

#include <include.h>
#include <system/sys.h>
#include "../../sconfigs/settings/numeric_config.h"
#include "../../sconfigs/memory/_ctx.h"

#include "../../intrinsics/intrinsics.h"
#include "../../adynamol/big_numbers/bigNums.h"

#include "add_sub.h"
#include "div.h"
#include "mul.h"
#include "mod.h"
#include "mod_op.h"

//todo ============================== GREATEST COMMON DIVISORS ============================== todo//
/* GCD - WORKSPACE RETURNER */
size_t __BIGINT_GCD_WS__(size_t u_size, size_t v_size);
/* GCD - ALGORITHMS */
uint64_t __BIGINT_EUCLID__(uint64_t u, uint64_t v);
void __BIGINT_GCD_DISPATCH__(bigInt *res, const bigInt *u, const bigInt *v, calc_ctx gcd_ctx);




//todo ================================== PRIMALITY TESTING ================================== todo//
/* Primality Testing - WORKSPACE RETURNER */
size_t __BIGINT_ECPP_WS__(size_t n_size);
size_t __BIGINT_PTEST_WS__(size_t x_size);
/* Primality Testing - ALGORITHMS */
uint8_t __BIGINT_ECPP__(const bigInt *n, calc_ctx mrabin_ctx);
uint8_t __BIGINT_PTEST_DISPATCH__(const bigInt *x, calc_ctx ptest_ctx);


#ifdef __cplusplus
}
#endif




#endif