#ifndef DNML_ADD_SUB
#define DNML_ADD_SUB


#include <debug_util.h>
#include <libdnml_types.h>
#include <include.h>
#include <dnml_status.h>
#include <dnml_sys/sys.h>
#include "../../intrinsics/intrinsics.h"
#include "../../util/util.h"
#include "../../util/crt_util.h"



void __BIGINT_ADD_WC__(bigInt *res, const bigInt *a, const bigInt *b);
void __BIGINT_ADD_SAW__(bigInt *res, const bigInt *x, const bigInt *y);
void __BIGINT_SUB_WB__(bigInt *res, const bigInt *a, const bigInt *b);
void __BIGINT_SUB_SAW__(bigInt *res, const bigInt *x, const bigInt *y);
drypto_stat __CRINT_ADD_WC__(cryptInt *res, const cryptInt *a, const cryptInt *b);
drypto_stat __CRINT_SUB_WC__(cryptInt *res, const cryptInt *a, const cryptInt *b);


#endif