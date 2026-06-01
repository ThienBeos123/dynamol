#ifndef DNML_MOD_OP_H
#define DNML_MOD_OP_H



#include <debug_util.h>
#include <dnml_status.h>
#include <libdnml_types.h>
#include <include.h>
#include <dnml_sys/sys.h>
#include <_libdnml_config/numeric_config.h>
#include <_libdnml_mem/_ctx.h>
#include "../../intrinsics/intrinsics.h"
#include "../../util/util.h"
#include "../../util/aconv_macros.h"

#include "../algo_base/add_sub.h"
#include "div.h"
#include "mul.h"
#include "mod.h"

#ifdef __cplusplus
extern "C" {
#endif
size_t __BIGINT_CMODMUL_WS__(size_t a_size, size_t b_size, size_t mod_size);
size_t __BIGINT_MONTMUL_WS__(size_t a_size, size_t b_size, mont_ctx ctx);
size_t __BIGINT_BIN_MODEXP_WS__(size_t base_size, size_t mod_size, size_t pow_size);
size_t __BIGINT_MBIN_MODEXP_WS__(size_t base_size, size_t mod_size, size_t pow_size);
size_t __BIGINT_MODMUL_WS__(size_t a_size, size_t b_size, size_t mod_size);
size_t __BIGINT_MODEXP_WS__(size_t base_size, size_t mod_size, size_t pow_size);


void __BIGINT_MONTMUL__(const bigInt *a, const bigInt *b, mont_ctx ctx, bigInt *res, calc_ctx montmul_ctx);
void __BIGINT_CMODMUL__(const bigInt *a, const bigInt *b, const bigInt *modulus, bigInt *res, calc_ctx modmul_ctx);
void __BIGINT_BIN_MODEXP__(
    const bigInt *base, const bigInt *power, 
    const bigInt *modulus, bigInt *res, calc_ctx binexp_ctx
);
void __BIGINT_MBIN_MODEXP__(
    const bigInt *base, const bigInt *power, 
    const bigInt *modulus, bigInt *res, calc_ctx binexp_ctx
);
void __BIGINT_MODMUL_DISPATCH__(
    const bigInt *a, const bigInt *b, 
    const bigInt *modulus, bigInt *res, calc_ctx modmul_ctx
);
void __BIGINT_MODEXP_DISPATCH__(
    const bigInt *base, const bigInt *power, 
    const bigInt *modulus, bigInt *res, calc_ctx binexp_ctx
);
#ifdef __cplusplus
}
#endif





#endif