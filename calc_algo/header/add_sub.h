#ifndef DNML_ADD_SUB
#define DNML_ADD_SUB


#include <include.h>
#include <system/sys.h>
#include "../../sconfigs/memory/arena.h"
#include "../../sconfigs/settings/numeric_config.h"
#include "../../sconfigs/memory/_ctx.h"

#include "../../intrinsics/intrinsics.h"
#include "../../util/util.h"
#include "../../adynamol/big_numbers/bigNums.h"



void __BIGINT_ADD_WC__(bigInt *res, const bigInt *a, const bigInt *b);
void __BIGINT_ADD_SAW__(bigInt *res, const bigInt *x, const bigInt *y);
void __BIGINT_SUB_WB__(bigInt *res, const bigInt *a, const bigInt *b);
void __BIGINT_SUB_SAW__(bigInt *res, const bigInt *x, const bigInt *y);


#endif