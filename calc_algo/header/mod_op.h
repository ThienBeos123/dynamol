#ifndef DNML_MOD_OP_H
#define DNML_MOD_OP_H




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
#include "mod.h"

size_t __BIGINT_CMODMUL_WS__(size_t a_size, size_t b_size, size_t mod_size);
size_t __BIGINT_MONTMUL_WS__(size_t a_size, size_t b_size, mont_ctx ctx);
size_t __BIGINT_MODMUL_WS__(size_t a_size, size_t b_size, size_t mod_size);
size_t __BIGINT_MODEXP_WS__(size_t base_size, size_t mod_size, size_t pow_size);


void __BIGINT_MONTMUL__(
    const bigInt *a, const bigInt *b, 
    mont_ctx ctx, bigInt *res, calc_ctx montmul_ctx
);
void __BIGINT_CLASSICAL_MODMUL__(
    const bigInt *a, const bigInt *b, 
    const bigInt *modulus, bigInt *res, calc_ctx modmul_ctx
);
void __BIGINT_MODMUL_DISPATCH__(
    const bigInt *a, const bigInt *b, 
    const bigInt *modulus, bigInt *res, calc_ctx modmul_ctx
);
void __BIGINT_MODEXP_DISPATCH__(
    const bigInt *base, const bigInt *power, 
    const bigInt *modulus, bigInt *res, calc_ctx binexp_ctx
);






#endif