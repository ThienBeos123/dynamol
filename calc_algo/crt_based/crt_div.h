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
/* CRYPTINT WORKSPACE SIZE */
size_t __CRINT_NEWTON_WS__(size_t dend_size, size_t div_size);
size_t __CRINT_DIV_WS__(size_t dend_size, size_t div_size);


/* CRYPTINT ALGORITHMS */
drypto_stat __CRINT_SHORT_DIVISION__(const cryptint *a, uint64_t b, cryptint *quot, cryptint *rem);
drypto_stat __CRINT_NEWTON_RECP__(
    const cryptint *a, const cryptint *b,
     cryptint *quot, cryptint *rem, 
     calc_ctx newton_ctx
);
drypto_stat __CRINT_DIVMOD_DISP__(
    const cryptint *a, const cryptint *b,
    cryptint *quot, cryptint *rem,
    calc_ctx dvmod_ctx
);




#endif