#ifndef DNML_CRT_MODOP_H
#define DNML_CRT_MODOP_H



#include <libdnml_types.h>
#include <include.h>
#include <dnml_sys/sys.h>
#include <_libdnml_config/numeric_config.h>
#include <_libdnml_mem/_ctx.h>
#include "../../intrinsics/intrinsics.h"
#include "../../util/crt_util.h"
#include "../algo_base/add_sub.h"
#include "crt_mul.h"
#include "crt_div.h"
#include "crt_mod.h"

#ifdef __cplusplus
extern "C" {
#endif
/* ------ Main Standalone Algorithms ------ */
void __CRINT_MONTMUL__(crint *res, const crint *a, const crint *b, mont_ctx ctx);
void __CRINT_CMODMUL__(crint *res, const crint *a, const crint *b, const crint *mod);
void __CRINT_BIN_MODEXP__(crint *res, const crint *base, const crint *exp, const crint *mod);
void __CRINT_MBIN_MODEXP__(crint *res, const bigInt *base, const bigInt *exp, const crint *mod);
/* ------ Algorithm Dispatchers ------ */
void __CRINT_MODMUL_DISP__(crint *res, const crint *a, const crint *b, const crint *mod);
void __CRINT_MODEXP_DISP__(crint *res, const crint *base, const crint *exp, const crint *mod);
#ifdef __cplusplus
}
#endif


#endif