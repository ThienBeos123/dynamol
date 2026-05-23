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

/* CRYPTINT WORKSPACE SIZE */
size_t __CRINT_BARETT_WS__(size_t dend_size, size_t mod_size);
size_t __CRINT_MOD_WS__(size_t dend_size, size_t mod_size);

/* CRYPTINT ALGORITHMS */
dnml_status __CRINT_BARETT_REDC__(const cryptint *dend, const cryptint *mod, cryptint *rem, calc_ctx barett_ctx);
dnml_status __CRINT_MONT_REDC__(cryptint *t, mont_ctx mredc_ctx, cryptint *rem, calc_ctx redc_ctx);
dnml_status __CRINT_MOD_DISP__(const cryptint *dend, const cryptint *mod, cryptint *rem, calc_ctx redc_ctx);


#endif