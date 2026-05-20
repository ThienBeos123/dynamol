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


/* CRYPTINT WORKSPACE SIZE */



/* CRYPTINT ALGORITHMS */
drypto_stat __BIGINT_SHORT_DIVISION__(const cryptInt *a, uint64_t b, cryptInt *quot, cryptInt *rem);
drypto_stat __BIGINT_KNUTH_D__(const cryptInt *a, const cryptInt *b, cryptInt *quot, cryptInt *rem, calc_ctx knuth_ctx);




#endif