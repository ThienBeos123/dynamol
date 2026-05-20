#ifndef DNML_MUL_H
#define DNML_MUL_H



#ifdef __cplusplus
extern "C" {
#endif

#include <libdnml_types.h>
#include <include.h>
#include <dnml_sys/sys.h>
#include <_libdnml_config/numeric_config.h>
#include <_libdnml_mem/_ctx.h>

#include "../../intrinsics/intrinsics.h"
#include "../../util/util.h"

#include "add_sub.h"

/* WORKSPACE SIZE */
size_t __BIGINT_MUL_WS__(size_t a_size, size_t b_size);

/* ALGORITHMS */
void __BIGINT_MUL_DISPATCH__(const bigInt *a, const bigInt *b, bigInt *res, calc_ctx mul_ctx);


#ifdef __cplusplus
}
#endif



#endif
