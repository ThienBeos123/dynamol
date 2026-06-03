#ifndef DNML_CRT_MOD_H
#define DNML_CRT_MOD_H



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

#ifdef __cplusplus
extern "C" {
#endif
/* CRYPTINT ALGORITHMS */
dnml_status __CRINT_BARETT_REDC__(const crint *dend, const crint *mod, crint *rem);
dnml_status __CRINT_MONT_REDC__(crint *t, mont_ctx mredc_ctx, crint *rem);
dnml_status __CRINT_MOD_DISP__(const crint *dend, const crint *mod, crint *rem, crint *tmp_quot);
#ifdef __cplusplus
}
#endif


#endif