#ifndef DNML_CRT_DIV_H
#define DNML_CRT_DIV_H


#include <libdnml_types.h>
#include <include.h>
#include <dnml_sys/sys.h>
#include <_libdnml_config/numeric_config.h>
#include <_libdnml_mem/_ctx.h>
#include "../../intrinsics/intrinsics.h"
#include "../../util/crt_util.h"
#include "../algo_base/add_sub.h"
#include "crt_mul.h"


#ifdef __cplusplus
extern "C" {
#endif


/* CRYPTINT ALGORITHMS */
dnml_status __CRINT_SHORT_DIVISION__(const crint *a, uint64_t b, crint *quot, crint *rem);
dnml_status __CRINT_NEWTON_RECP__(const crint *a, const crint *b, crint *quot, crint *rem);
dnml_status __CRINT_DIV_DISP__(const crint *a, const crint *b, crint *quot, crint *rem);
#ifdef __cplusplus
}
#endif




#endif