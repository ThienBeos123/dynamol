#ifndef DNML_CRYPT_INT_FUNC
#define DNML_CRYPT_INT_FUNC




#include <libdnml_types.h>
#include <include.h>
#include <char_tables.h>
#include <dnml_sys/sys.h>
#include <_libdnml_config/settings.h>
#include <_libdnml_mem/_ctx.h>
#include <dnml_status.h>

#include "../../intrinsics/intrinsics.h"
#include "../../calc_algo/crt_calc.h"
#include "../../util/crt_util.h"


#define CT_COND_ASSIGN(x, cond, new_val) do { \
    size_t mask = -(size_t)(cond);  /* All 1s if true, all 0s if false */ \
    (x) = ((x) & ~mask) | ((new_val) & mask); \
} while(0);



//* ===================================== TYPE SETUP FUNCTION ===================================== *//
void crint_free(cryptInt *x);
void crint_new(cryptInt *x);
void crint_snew(cryptInt *x, const size_t n);
drypto_stat crint_cinew(cryptInt *x, cryptInt *y);
drypto_stat crint_new_u64(cryptInt *x, const uint64_t in);
drypto_stat crint_new_i64(cryptInt *x, const int64_t in);
drypto_stat crint_new_f128(cryptInt *x, long double in);




//* ===================================== STATE ALTERATION FUNCTIONS ===================================== *//
void crint_canonicalize(cryptInt *x);
void crint_normalize(cryptInt *X);
drypto_stat crint_resize(cryptInt *x, size_t k);
drypto_stat crint_reserve(cryptInt *x, size_t k);
drypto_stat crint_shrink(cryptInt *x, size_t k);
drypto_stat crint_reset(cryptInt *x);
bool crint_validate(cryptInt x);
bool crint_pvalidate(cryptInt *x);






#endif