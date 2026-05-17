#ifndef DNML_MOD_H
#define DNML_MOD_H



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



size_t __BIGINT_MOD_WS__(size_t a_size, size_t n_size);
void __BIGINT_MONT_REDC__(bigInt *t, mont_ctx mredc_ctx, bigInt *rem, calc_ctx redc_ctx);
void __BIGINT_MOD_DISPATCH__(
    const bigInt *a, const bigInt *n, 
    bigInt *rem, bigInt *tmp_quot, calc_ctx mod_ctx
);


#ifdef __cplusplus
}
#endif


#endif