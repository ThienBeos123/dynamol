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
#define NORMALIZE_0_TO_1(dst, x) do { \
    (dst) = (x) | (!(x)); \
} while(0);



//* ===================================== TYPE SETUP FUNCTION ===================================== *//
void crint_free(cryptint *x);
dnml_status crint_new(cryptint *x);
dnml_status crint_snew(cryptint *x, const size_t n);
dnml_status crint_cinew(cryptint *x, cryptint *y);
dnml_status crint_new_u64(cryptint *x, const uint64_t in);
dnml_status crint_new_i64(cryptint *x, const int64_t in);
dnml_status crint_new_f128(cryptint *x, long double in);




//* =============================================== COMPARISONS ============================================== */
/* ---------------- Integer - I64 ---------------- */
bool crint_equal_i64(const cryptint x, const int64_t val, dnml_status *err);
bool crint_less_i64(const cryptint x, const int64_t val, dnml_status *err);
bool crint_more_i64(const cryptint x, const int64_t val, dnml_status *err);
bool crint_lequal_i64(const cryptint x, const int64_t val, dnml_status *err);
bool crint_mequal_i64(const cryptint x, const int64_t val, dnml_status *err);
/* ----------- Unsigned Integer - UI64 ----------- */
bool crint_equal_u64(const cryptint x, const uint64_t val, dnml_status *err);
bool crint_less_u64(const cryptint x, const uint64_t val, dnml_status *err);
bool crint_more_u64(const cryptint x, const uint64_t val, dnml_status *err);
bool crint_lequal_u64(const cryptint x, const uint64_t val, dnml_status *err);
bool crint_mequal_u64(const cryptint x, const uint64_t val, dnml_status *err);
/* ------------------- Cryptint ------------------ */
bool crint_equal(const cryptint x, cryptint y, dnml_status *err);
bool crint_less(const cryptint x, cryptint y, dnml_status *err);
bool crint_more(const cryptint x, cryptint y, dnml_status *err);
bool crint_lequal(const cryptint x, cryptint y, dnml_status *err);
bool crint_mequal(const cryptint x, cryptint y, dnml_status *err);





//* ================================================= COPIES ================================================= */
/* -------------  Mutative SMALL Copies ------------- */
dnml_status crint_mut_copyu64(cryptint *dst__, const uint64_t source__);
dnml_status crint_mut_dcopyu64(cryptint *dst__, const uint64_t source__);
dnml_status crint_mut_copyi64(cryptint *dst__, const int64_t source__);
dnml_status crint_mut_dcopyi64(cryptint *dst__, const int64_t source__);
/* -------------  Mutative LARGE Copies ------------- */
dnml_status crint_mut_copyf128(cryptint *dst__, long double source__);
dnml_status crint_mut_dcopyf128(cryptint *dst__, long double source__);
dnml_status crint_mut_ocopyf128(cryptint *dst__, long double source__);
dnml_status crint_mut_tover_copyf128(cryptint *dst__, long double source__);
dnml_status crint_mut_copy(cryptint *dst__, const cryptint source__);
dnml_status crint_mut_dcopy(cryptint *dst__, const cryptint source__);
dnml_status crint_mut_ocopy(cryptint *dst__, const cryptint source__);
dnml_status crint_mut_tover_copy(cryptint *dst__, const cryptint source__);
/* -------------  Functional SMALL Copies ------------- */
cryptint crint_copyu64(const uint64_t source__, dnml_status *__err__);
cryptint crint_copyi64(const int64_t source__, dnml_status *__err__);
/* -------------  Functional LARGE Copies ------------- */
cryptint crint_copyf128(long double source__, dnml_status *__err__);
cryptint crint_ocopyf128(long double source__, size_t output_cap, dnml_status *__err__);
cryptint crint_tover_copyf128(long double source__, size_t output_cap, dnml_status *__err__);
cryptint crint_copy(const cryptint source__, dnml_status *__err__);
cryptint crint_ocopy(const cryptint source__, size_t output_cap, dnml_status *__err__);
cryptint crint_tover_copy(const cryptint source__, size_t output_cap, dnml_status *__err__);





//* ===================================== STATE ALTERATION FUNCTIONS ===================================== *//
void crint_canonicalize(cryptint *x);
void crint_normalize(cryptint *X);
dnml_status crint_resize(cryptint *x, size_t k);
dnml_status crint_reserve(cryptint *x, size_t k);
dnml_status crint_shrink(cryptint *x, size_t k);
dnml_status crint_reset(cryptint *x);
bool crint_validate(cryptint x);
bool crint_pvalidate(cryptint *x);






#endif