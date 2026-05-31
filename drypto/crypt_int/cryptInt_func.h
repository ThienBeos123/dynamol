#ifndef DNML_CRYPT_INT_FUNC
#define DNML_CRYPT_INT_FUNC




#include <libdnml_types.h>
#include <include.h>
#include <char_tables.h>
#include <dnml_sys/sys.h>
#include <debug_util.h>
#include <_libdnml_config/settings.h>
#include <_libdnml_config/numeric_config.h>
#include <_libdnml_mem/_ctx.h>
#include <_libdnml_mem/_cr_arena.h>
#include <dnml_status.h>

#include "../../intrinsics/intrinsics.h"
#include "../../calc_algo/crt_calc.h"
#include "../../util/crt_util.h"
#include "crint_algo_core.h"

#define FAKE_BUF_CAP 1024 /* Constant tweaked to specific use case */
#define CT_COND_ASSIGN(x, cond, new_val) do { \
    size_t mask = -(size_t)(cond);  /* All 1s if true, all 0s if false */ \
    (x) = ((x) & ~mask) | ((new_val) & mask); \
} while(0);
#define NORMALIZE_0_TO_1(dst, x) do { \
    (dst) = (x) | (!(x)); \
} while(0);

// ARENA - ALGORITHMICLY ESSENTIAL
extern local_thread dnml_arena ___DASI_ALGO_ARENA_;
dnml_arena* _USE_CARENA(void);




//* ========================== COMMON ASSERT ERRORS CATALOG ========================== *//
#define arena_poison_oom "Arena Poisoned: Arena Re-allocation witnessed an OOM error (-Earena_poison)"
#define calloc_null "Allocation Failure: calloc() returned NULL (-Ealloc_calloc_fail)" /* CALLOC returns NULL */
#define realloc_null "Allocation Failure: realloc() returned NULL (-Ealloc_realloc_fail)" /* REALLOC returns NULL */
#define full_contract "Contract Violation: Invalid CryptInt (-Ecrypt_int_invalid)" /* FULL Contract Violation */
#define store_inval "\
Partial Contract Violation: CryptInt invalid for storage (-Ecrypt_int_sinvalid)" /* Partial Contract Violation - Storage */


//* ========================== COMMON !TEST! ASSERT ERRORS CATALOG ========================== *//
#define crint_poisoned "Mathematical Error: CryptInt Poisoned (-Ecrypt_int_invalid)" /* CryptInt Poisoined - Testing */
#define null_err "Parameter Error: Status/Error parameter-based returns is null (-Enull_err_param)" /* err == NULL - testing */




//* ===================== TEST ASSERT CONVENIENT MACROS ===================== *//
// Functional Macros


// Mutative Macros
#define _magcrint_poison(x, cleanup) do { DNML_TEST_ASSERT((!x->poisoned), crint_poisoned, {cleanup}); } while(0);
#define _mheap_alloc_oom(err_check, err) do { \
    test_assert_mut((err_check != DNML_ALLOC_OOM), realloc_null, __crint_exit(), err, DNML_ALLOC_OOM, ;); \
} while(0);



//* ===================================== TYPE SETUP FUNCTION ===================================== *//
void crint_free(crint *x);
dnml_status crint_new(crint *x);
dnml_status crint_snew(crint *x, const size_t n);
dnml_status crint_cinew(crint *x, crint *y);
dnml_status crint_new_u64(crint *x, const uint64_t in);
dnml_status crint_new_i64(crint *x, const int64_t in);
dnml_status crint_new_f128(crint *x, long double in);




//* =============================================== ASSIGNMENTS ============================================== */
dnml_status crint_set(const crint x, crint *receiver);
dnml_status crint_set_safe(const crint x, crint *receiver);
/* --------- CryptInt --> Primitive Types --------- */
dnml_status crint_setu64(uint64_t* receiver, const crint x);
dnml_status crint_seti64(int64_t* receiver, const crint x);
dnml_status crint_setf128(long double* receiver, const crint x);
dnml_status crint_setu64_safe(uint64_t* receiver, const crint x);
dnml_status crint_seti64_safe(int64_t* receiver, const crint x);
dnml_status crint_setf128_safe(long double* receiver, const crint x);
/* --------- Primitive Types --> CryptInt --------- */
dnml_status crint_getu64(crint *receiver, const uint64_t val);
dnml_status crint_geti64(crint *receiver, const int64_t val);
dnml_status crint_getf128(crint *receiver, const long double val);
dnml_status crint_getf128_safe(crint *receiver, const long double val);




//* =============================================== CONVERSIONS ============================================== */
/* --------- CryptInt --> Primitive Types --------- */
uint64_t crint_tou64(const crint x, dnml_status *err);
int64_t crint_toi64(const crint x, dnml_status *err);
long double crint_tof128(const crint x, dnml_status *err);
uint64_t crint_tou64_safe(const crint x, dnml_status *err);
int64_t crint_toi64_safe(const crint x, dnml_status *err);
long double crint_tof128_safe(const crint x, dnml_status *err);
/* --------- Primitive Types --> CryptInt --------- */
crint crint_fromu64(const uint64_t x, dnml_status *err);
crint crint_fromi64(const int64_t x, dnml_status *err);
crint crint_fromf128(const long double x, dnml_status *err);
crint crint_fromf128_safe(const long double x, dnml_status *err);




//* =========================================== BITWISE OPERATIONS =========================================== */
crint crint_not(const crint x, dnml_status *err);
crint crint_rshift(crint x, size_t k, dnml_status *err);
crint crint_lshift(crint x, size_t k, dnml_status *err);
crint crint_lsfhitg(crint x, size_t k, dnml_status *err);
dnml_status crint_mut_not(crint *x);
dnml_status crint_mut_rshift(crint *x, size_t k);
dnml_status crint_mut_lshift(crint *x, size_t k);
dnml_status crint_mut_lshiftg(crint *x, size_t k);
/* ----------------------- Mutative, Fixed Width ----------------------- */
dnml_status crint_mut_andu64(crint *x, const uint64_t val);
dnml_status crint_mut_nandu64(crint *x, const uint64_t val);
dnml_status crint_mut_oru64(crint *x, const uint64_t val);
dnml_status crint_mut_noru64(crint *x, const uint64_t val);
dnml_status crint_mut_xoru64(crint *x, const uint64_t val);
dnml_status crint_mut_xnoru64(crint *x, const uint64_t val);
dnml_status crint_mut_and(crint *x, crint y);
dnml_status crint_mut_nand(crint *x, crint y);
dnml_status crint_mut_or(crint *x, crint y);
dnml_status crint_mut_nor(crint *x, crint y);
dnml_status crint_mut_xor(crint *x, crint y);
dnml_status crint_mut_xnor(crint *x, crint y);
/* ----------------------- Mutative, Explicit Width ----------------------- */
dnml_status crint_mutex_andu64(crint *x, const uint64_t val, size_t range);
dnml_status crint_mutex_nandu64(crint *x, const uint64_t val, size_t range);
dnml_status crint_mutex_oru64(crint *x, const uint64_t val, size_t range);
dnml_status crint_mutex_noru64(crint *x, const uint64_t val, size_t range);
dnml_status crint_mutex_xoru64(crint *x, const uint64_t val, size_t range);
dnml_status crint_mutex_xnoru64(crint *x, const uint64_t val, size_t range);
dnml_status crint_mutex_andi64(crint *x, const int64_t val, size_t range);
dnml_status crint_mutex_nandi64(crint *x, const int64_t val, size_t range);
dnml_status crint_mutex_ori64(crint *x, const int64_t val, size_t range);
dnml_status crint_mutex_nori64(crint *x, const int64_t val, size_t range);
dnml_status crint_mutex_xori64(crint *x, const int64_t val, size_t range);
dnml_status crint_mutex_xnori64(crint *x, const int64_t val, size_t range);
dnml_status crint_mutex_and(crint *x, crint val, size_t range);
dnml_status crint_mutex_nand(crint *x, crint val, size_t range);
dnml_status crint_mutex_or(crint *x, crint val, size_t range);
dnml_status crint_mutex_nor(crint *x, crint val, size_t range);
dnml_status crint_mutex_xor(crint *x, crint val, size_t range);
dnml_status crint_mutex_xnor(crint *x, crint val, size_t range);
/* ----------------------- Functional, Fixed Width ----------------------- */
crint crint_andu64(crint x, const uint64_t val, dnml_status *err);
crint crint_nandu64(crint x, const uint64_t val, dnml_status *err);
crint crint_oru64(crint x, const uint64_t val, dnml_status *err);
crint crint_noru64(crint x, const uint64_t val, dnml_status *err);
crint crint_xoru64(crint x, const uint64_t val, dnml_status *err);
crint crint_xnoru64(crint x, const uint64_t val, dnml_status *err);
crint crint_and(crint x, crint y, dnml_status *err);
crint crint_nand(crint x, crint y, dnml_status *err);
crint crint_or(crint x, crint y, dnml_status *err);
crint crint_nor(crint x, crint y, dnml_status *err);
crint crint_xor(crint x, crint y, dnml_status *err);
crint crint_xnor(crint x, crint y, dnml_status *err);
/* ----------------------- Functional, Explicit Width ----------------------- */
crint crint_ex_andu64(crint x, const uint64_t val, size_t range, dnml_status *err);
crint crint_ex_nandu64(crint x, const uint64_t val, size_t range, dnml_status *err);
crint crint_ex_oru64(crint x, const uint64_t val, size_t range, dnml_status *err);
crint crint_ex_noru64(crint x, const uint64_t val, size_t range, dnml_status *err);
crint crint_ex_xoru64(crint x, const uint64_t val, size_t range, dnml_status *err);
crint crint_ex_xnoru64(crint x, const uint64_t val, size_t range, dnml_status *err);
crint crint_ex_andi64(crint x, const int64_t val, size_t range, dnml_status *err);
crint crint_ex_nandi64(crint x, const int64_t val, size_t range, dnml_status *err);
crint crint_ex_ori64(crint x, const int64_t val, size_t range, dnml_status *err);
crint crint_ex_nori64(crint x, const int64_t val, size_t range, dnml_status *err);
crint crint_ex_xori64(crint x, const int64_t val, size_t range, dnml_status *err);
crint crint_ex_xnori64(crint x, const int64_t val, size_t range, dnml_status *err);
crint crint_ex_and(crint x, crint val, size_t range, dnml_status *err);
crint crint_ex_nand(crint x, crint val, size_t range, dnml_status *err);
crint crint_ex_or(crint x, crint val, size_t range, dnml_status *err);
crint crint_ex_nor(crint x, crint val, size_t range, dnml_status *err);
crint crint_ex_xor(crint x, crint val, size_t range, dnml_status *err);
crint crint_ex_xnor(crint x, crint val, size_t range, dnml_status *err);




//* =============================================== COMPARISONS ============================================== */
/* ---------------- Integer - I64 ---------------- */
bool crint_equal_i64(const crint x, const int64_t val, dnml_status *err);
bool crint_less_i64(const crint x, const int64_t val, dnml_status *err);
bool crint_more_i64(const crint x, const int64_t val, dnml_status *err);
bool crint_lequal_i64(const crint x, const int64_t val, dnml_status *err);
bool crint_mequal_i64(const crint x, const int64_t val, dnml_status *err);
/* ----------- Unsigned Integer - UI64 ----------- */
bool crint_equal_u64(const crint x, const uint64_t val, dnml_status *err);
bool crint_less_u64(const crint x, const uint64_t val, dnml_status *err);
bool crint_more_u64(const crint x, const uint64_t val, dnml_status *err);
bool crint_lequal_u64(const crint x, const uint64_t val, dnml_status *err);
bool crint_mequal_u64(const crint x, const uint64_t val, dnml_status *err);
/* ------------------- Cryptint ------------------ */
bool crint_equal(const crint x, crint y, dnml_status *err);
bool crint_less(const crint x, crint y, dnml_status *err);
bool crint_more(const crint x, crint y, dnml_status *err);
bool crint_lequal(const crint x, crint y, dnml_status *err);
bool crint_mequal(const crint x, crint y, dnml_status *err);




//* ================================================= COPIES ================================================= */
/* -------------  Mutative SMALL Copies ------------- */
dnml_status crint_mut_copyu64(crint *dst__, const uint64_t source__);
dnml_status crint_mut_dcopyu64(crint *dst__, const uint64_t source__);
dnml_status crint_mut_copyi64(crint *dst__, const int64_t source__);
dnml_status crint_mut_dcopyi64(crint *dst__, const int64_t source__);
/* -------------  Mutative LARGE Copies ------------- */
dnml_status crint_mut_copyf128(crint *dst__, long double source__);
dnml_status crint_mut_dcopyf128(crint *dst__, long double source__);
dnml_status crint_mut_ocopyf128(crint *dst__, long double source__);
dnml_status crint_mut_tover_copyf128(crint *dst__, long double source__);
dnml_status crint_mut_copy(crint *dst__, const crint source__);
dnml_status crint_mut_dcopy(crint *dst__, const crint source__);
dnml_status crint_mut_ocopy(crint *dst__, const crint source__);
dnml_status crint_mut_tover_copy(crint *dst__, const crint source__);
/* -------------  Functional SMALL Copies ------------- */
crint crint_copyu64(const uint64_t source__, dnml_status *__err__);
crint crint_copyi64(const int64_t source__, dnml_status *__err__);
/* -------------  Functional LARGE Copies ------------- */
crint crint_copyf128(long double source__, dnml_status *__err__);
crint crint_ocopyf128(long double source__, size_t output_cap, dnml_status *__err__);
crint crint_tover_copyf128(long double source__, size_t output_cap, dnml_status *__err__);
crint crint_copy(const crint source__, dnml_status *__err__);
crint crint_ocopy(const crint source__, size_t output_cap, dnml_status *__err__);
crint crint_tover_copy(const crint source__, size_t output_cap, dnml_status *__err__);




//* ===================================== STATE ALTERATION FUNCTIONS ===================================== *//
void crint_canonicalize(crint *x);
void crint_normalize(crint *X);
dnml_status crint_resize(crint *x, size_t k);
dnml_status crint_reserve(crint *x, size_t k);
dnml_status crint_shrink(crint *x, size_t k);
dnml_status crint_reset(crint *x);
bool crint_validate(crint x);
bool crint_pvalidate(crint *x);






#endif