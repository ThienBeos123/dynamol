#ifndef DNML_MOD_H
#define DNML_MOD_H


#include <debug_util.h>
#include <dnml_status.h>
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

#ifdef __cplusplus
extern "C" {
#endif
size_t __BIGINT_BARETT_WS__(size_t a_size, size_t n_size);
size_t __BIGINT_MOD_WS__(size_t a_size, size_t n_size);
void __BIGINT_BARETT__(const bigInt *a, const bigInt *n, bigInt *rem, calc_ctx barett_ctx);
void __BIGINT_MONT_REDC__(bigInt *t, mont_ctx mredc_ctx, bigInt *rem);
void __BIGINT_MOD_DISPATCH__(
    const bigInt *a, const bigInt *n, 
    bigInt *rem, bigInt *tmp_quot, calc_ctx mod_ctx
);
#ifdef __cplusplus
}
#endif


#endif