#ifndef DNML_CRT_MUL_H
#define DNML_CRT_MUL_H


#include <libdnml_types.h>
#include <include.h>
#include <dnml_sys/sys.h>
#include <_libdnml_config/numeric_config.h>
#include <_libdnml_mem/_ctx.h>
#include "../../intrinsics/intrinsics.h"
#include "../../util/crt_util.h"
#include "../algo_base/add_sub.h"


/* CRYPTINT WORKSPACE SIZE */




/* CRYPTINT ALGORITHMS */
drypto_stat __CRINT_SCHOOLBOOK__(const cryptInt *a, const cryptInt *b, cryptInt *res);





#endif