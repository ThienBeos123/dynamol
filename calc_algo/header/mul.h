#ifndef DNML_MUL_H
#define DNML_MUL_H



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

/* WORKSPACE SIZE */
size_t __BIGINT_MUL_WS__(size_t a_size, size_t b_size);

/* ALGORITHMS */
void __BIGINT_MUL_DISPATCH__(const bigInt *a, const bigInt *b, bigInt *res, calc_ctx mul_ctx);


#ifdef __cplusplus
}
#endif



#endif
