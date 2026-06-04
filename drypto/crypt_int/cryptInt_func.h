/*
Copyright (C) 2026 @ThienBeos123/@Poly-glon

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

  http://apache.org

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/



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
#include <dnml_status.h>

#include "../../intrinsics/intrinsics.h"
#include "../../calc_algo/crt_calc.h"
#include "../../util/crt_util.h"
#include "crint_algo_core.h"


#define FAKE_BUF_CAP 128 /* Constant tweaked to specific use case */
//! ANY MANUL TWEAKS TO THIS CONSTANT MACRO MUST SATISFY THE FOLLOWING:
//  1. THE CONSTANT VALUE IS A POWER OF 2 (TO PREVENT DIVISION INACURRACY IN PARTS OF THE CODE)
//  2. THE CONSTANT VALUE MUST BE ABLE TO AT LEAST BE DIVISBLE BY 4 INTO A Z+ INTEGER


//? ============================= COMMON !TEST! ASSERT ERRORS CATALOG ============================= ?//
#define pointer_null "Invalid Input: Pointer input passed as NULL (-Einput_null_pointer)"
#define calloc_null "Allocation Failure: calloc() returned NULL (-Ealloc_calloc_fail)" /* CALLOC returns NULL */
#define realloc_null "Allocation Failure: realloc() returned NULL (-Ealloc_realloc_fail)" /* REALLOC returns NULL */
#define full_contract "Contract Violation: Invalid CryptInt (-Ecrypt_int_invalid)" /* FULL Contract Violation */
#define store_inval "\
Partial Contract Violation: CryptInt invalid for storage (-Ecrypt_int_sinvalid)" /* Partial Contract Violation - Storage */
#define crint_poisoned "Mathematical Error: CryptInt Poisoned (-Ecrypt_int_invalid)" /* CryptInt Poisoined - Testing */
#define null_err "Parameter Error: Status/Error parameter-based returns is null (-Enull_err_param)" /* err == NULL - testing */




//? ================================= TEST ASSERT CONVENIENT MACROS ================================= ?//
// Functional Macros
#define pbv_crint_clear(x) do { x.limbs = 0; x.cap = 0; x.n = 0; x.sign = 0; x.poisoned = 0; } while(0);
#define preop_err(cond, err, err_code, cleanup) do { \
    if (cond) { \
        if (_lib_crt_neq((ptr_t)err, (ptr_t)NULL)) *err = err_code; \
        cleanup; return __CRINT_ERRVAL__(); \
    } \
} while(0);
#define tmp_cleanup(tmp, ret_stat, chosen_freed) do { \
    chosen_freed = (_lib_crt_eq((ret_stat), CRINT_SUCCESS)) ? NULL : (tmp).limbs; \
    free(chosen_freed); /* Safe nop even on chosen_freed = NULL since ANSI-C */ \
    (tmp).limbs = (_lib_crt_eq((ret_stat), CRINT_SUCCESS)) ? (tmp).limbs : 0; \
    CHOOSE_OPTION(((tmp).n), (_lib_crt_eq((ret_stat), CRINT_SUCCESS)), ((tmp).n), (0)); \
    CHOOSE_OPTION(((tmp).cap), (_lib_crt_eq((ret_stat), CRINT_SUCCESS)), ((tmp).cap), (0)); \
    CHOOSE_OPTION(((tmp).sign), (_lib_crt_eq((ret_stat), CRINT_SUCCESS)), ((tmp).sign), (0)); \
    CHOOSE_OPTION(((tmp).poisoned), (_lib_crt_eq((ret_stat), CRINT_SUCCESS)), ((tmp).poisoned), (0)); \
    chosen_freed = 0; \
} while(0);
#define mask_ret(ret, mask, ret_stat, chosen_freed) do { \
    chosen_freed = (_lib_crt_eq((ret_stat), CRINT_SUCCESS)) ? NULL : (ret)->limbs; \
    free(chosen_freed); /* Safe nop even on chosen_freed = NULL since ANSI-C */ \
    (ret)->limbs = (_lib_crt_eq((ret_stat), CRINT_SUCCESS)) ? (ret)->limbs : 0; \
    (ret)->n &= (mask); (ret)->cap &= (mask); (ret)->sign &= (mask); \
    (ret)->poisoned = (_lib_crt_eq((ret_stat), CRINT_POISON) | _lib_crt_eq((ret_stat), CRINT_ERR_DOMAIN)); \
} while(0);


// Mutative Macros
#define _pre_assert(x, cleanup) do { \
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)(x), (ptr_t)(NULL))), pointer_null, {}); \
    DNML_TEST_ASSERT((!(x)->poisoned), crint_poisoned, {cleanup}); \
    DNML_TEST_ASSERT((crint_pvalidate((x))), full_contract, {cleanup}); \
} while(0);
#define _mheap_alloc_oom(err) do { DNML_TEST_ASSERT((err != DNML_ALLOC_OOM), realloc_null, __crint_exit()); } while(0);




//* ===================================== TYPE SETUP FUNCTION ===================================== *//
dnml_status crint_free(crint *x);
dnml_status crint_new(crint *x);
dnml_status crint_snew(crint *x, size_t n);
dnml_status crint_cinew(crint *x, crint *y);
dnml_status crint_new_u64(crint *x, uint64_t in);
dnml_status crint_new_i64(crint *x, int64_t in);
dnml_status crint_new_f128(crint *x, long double in);




//* =============================================== ASSIGNMENTS ============================================== */
dnml_status crint_set(crint x, crint *dst);
dnml_status crint_set_safe(crint x, crint *dst);
/* --------- CryptInt --> Primitive Types --------- */
dnml_status crint_setu64(uint64_t* dst, crint x);
dnml_status crint_seti64(int64_t* dst, crint x);
dnml_status crint_setf128(long double* dst, crint x);
dnml_status crint_setu64_safe(uint64_t* dst, crint x);
dnml_status crint_seti64_safe(int64_t* dst, crint x);
dnml_status crint_setf128_safe(long double* dst, crint x);
/* --------- Primitive Types --> CryptInt --------- */
dnml_status crint_getu64(crint *dst, uint64_t val);
dnml_status crint_geti64(crint *dst, int64_t val);
dnml_status crint_getf128(crint *dst, long double val);
dnml_status crint_getf128_safe(crint *dst, long double val);




//* =============================================== CONVERSIONS ============================================== */
/* --------- CryptInt --> Primitive Types --------- */
uint64_t crint_tou64(crint x, dnml_status *err);
int64_t crint_toi64(crint x, dnml_status *err);
long double crint_tof128(crint x, dnml_status *err);
uint64_t crint_tou64_safe(crint x, dnml_status *err);
int64_t crint_toi64_safe(crint x, dnml_status *err);
long double crint_tof128_safe(crint x, dnml_status *err);
/* --------- Primitive Types --> CryptInt --------- */
crint crint_fromu64(uint64_t x, dnml_status *err);
crint crint_fromi64(int64_t x, dnml_status *err);
crint crint_fromf128(long double x, dnml_status *err);
crint crint_fromf128_safe(long double x, dnml_status *err);




//* =========================================== BITWISE OPERATIONS =========================================== */
crint crint_not(crint x, dnml_status *err);
crint crint_rshift(crint x, size_t k, dnml_status *err);
crint crint_lshift(crint x, size_t k, dnml_status *err);
crint crint_lsfhitg(crint x, size_t k, dnml_status *err);
dnml_status crint_mut_not(crint *x);
dnml_status crint_mut_rshift(crint *x, size_t k);
dnml_status crint_mut_lshift(crint *x, size_t k);
dnml_status crint_mut_lshiftg(crint *x, size_t k);
/* ----------------------- Mutative, Fixed Width ----------------------- */
dnml_status crint_mut_andu64(crint *x, uint64_t val);
dnml_status crint_mut_nandu64(crint *x, uint64_t val);
dnml_status crint_mut_oru64(crint *x, uint64_t val);
dnml_status crint_mut_noru64(crint *x, uint64_t val);
dnml_status crint_mut_xoru64(crint *x, uint64_t val);
dnml_status crint_mut_xnoru64(crint *x, uint64_t val);
dnml_status crint_mut_and(crint *x, crint y);
dnml_status crint_mut_nand(crint *x, crint y);
dnml_status crint_mut_or(crint *x, crint y);
dnml_status crint_mut_nor(crint *x, crint y);
dnml_status crint_mut_xor(crint *x, crint y);
dnml_status crint_mut_xnor(crint *x, crint y);
/* ----------------------- Mutative, Explicit Width ----------------------- */
dnml_status crint_mutex_andu64(crint *x, uint64_t val, size_t range);
dnml_status crint_mutex_nandu64(crint *x, uint64_t val, size_t range);
dnml_status crint_mutex_oru64(crint *x, uint64_t val, size_t range);
dnml_status crint_mutex_noru64(crint *x, uint64_t val, size_t range);
dnml_status crint_mutex_xoru64(crint *x, uint64_t val, size_t range);
dnml_status crint_mutex_xnoru64(crint *x, uint64_t val, size_t range);
dnml_status crint_mutex_andi64(crint *x, int64_t val, size_t range);
dnml_status crint_mutex_nandi64(crint *x, int64_t val, size_t range);
dnml_status crint_mutex_ori64(crint *x, int64_t val, size_t range);
dnml_status crint_mutex_nori64(crint *x, int64_t val, size_t range);
dnml_status crint_mutex_xori64(crint *x, int64_t val, size_t range);
dnml_status crint_mutex_xnori64(crint *x, int64_t val, size_t range);
dnml_status crint_mutex_and(crint *x, crint val, size_t range);
dnml_status crint_mutex_nand(crint *x, crint val, size_t range);
dnml_status crint_mutex_or(crint *x, crint val, size_t range);
dnml_status crint_mutex_nor(crint *x, crint val, size_t range);
dnml_status crint_mutex_xor(crint *x, crint val, size_t range);
dnml_status crint_mutex_xnor(crint *x, crint val, size_t range);
/* ----------------------- Functional, Fixed Width ----------------------- */
crint crint_andu64(crint x, uint64_t val, dnml_status *err);
crint crint_nandu64(crint x, uint64_t val, dnml_status *err);
crint crint_oru64(crint x, uint64_t val, dnml_status *err);
crint crint_noru64(crint x, uint64_t val, dnml_status *err);
crint crint_xoru64(crint x, uint64_t val, dnml_status *err);
crint crint_xnoru64(crint x, uint64_t val, dnml_status *err);
crint crint_and(crint x, crint y, dnml_status *err);
crint crint_nand(crint x, crint y, dnml_status *err);
crint crint_or(crint x, crint y, dnml_status *err);
crint crint_nor(crint x, crint y, dnml_status *err);
crint crint_xor(crint x, crint y, dnml_status *err);
crint crint_xnor(crint x, crint y, dnml_status *err);
/* ----------------------- Functional, Explicit Width ----------------------- */
crint crint_ex_andu64(crint x, uint64_t val, size_t range, dnml_status *err);
crint crint_ex_nandu64(crint x, uint64_t val, size_t range, dnml_status *err);
crint crint_ex_oru64(crint x, uint64_t val, size_t range, dnml_status *err);
crint crint_ex_noru64(crint x, uint64_t val, size_t range, dnml_status *err);
crint crint_ex_xoru64(crint x, uint64_t val, size_t range, dnml_status *err);
crint crint_ex_xnoru64(crint x, uint64_t val, size_t range, dnml_status *err);
crint crint_ex_andi64(crint x, int64_t val, size_t range, dnml_status *err);
crint crint_ex_nandi64(crint x, int64_t val, size_t range, dnml_status *err);
crint crint_ex_ori64(crint x, int64_t val, size_t range, dnml_status *err);
crint crint_ex_nori64(crint x, int64_t val, size_t range, dnml_status *err);
crint crint_ex_xori64(crint x, int64_t val, size_t range, dnml_status *err);
crint crint_ex_xnori64(crint x, int64_t val, size_t range, dnml_status *err);
crint crint_ex_and(crint x, crint val, size_t range, dnml_status *err);
crint crint_ex_nand(crint x, crint val, size_t range, dnml_status *err);
crint crint_ex_or(crint x, crint val, size_t range, dnml_status *err);
crint crint_ex_nor(crint x, crint val, size_t range, dnml_status *err);
crint crint_ex_xor(crint x, crint val, size_t range, dnml_status *err);
crint crint_ex_xnor(crint x, crint val, size_t range, dnml_status *err);




//* =============================================== COMPARISONS ============================================== */
/* ---------------- Integer - I64 ---------------- */
bool crint_equal_i64(crint x, int64_t val, dnml_status *err);
bool crint_less_i64(crint x, int64_t val, dnml_status *err);
bool crint_more_i64(crint x, int64_t val, dnml_status *err);
bool crint_lequal_i64(crint x, int64_t val, dnml_status *err);
bool crint_mequal_i64(crint x, int64_t val, dnml_status *err);
/* ----------- Unsigned Integer - UI64 ----------- */
bool crint_equal_u64(crint x, uint64_t val, dnml_status *err);
bool crint_less_u64(crint x, uint64_t val, dnml_status *err);
bool crint_more_u64(crint x, uint64_t val, dnml_status *err);
bool crint_lequal_u64(crint x, uint64_t val, dnml_status *err);
bool crint_mequal_u64(crint x, uint64_t val, dnml_status *err);
/* ------------------- Cryptint ------------------ */
bool crint_equal(crint x, crint y, dnml_status *err);
bool crint_less(crint x, crint y, dnml_status *err);
bool crint_more(crint x, crint y, dnml_status *err);
bool crint_lequal(crint x, crint y, dnml_status *err);
bool crint_mequal(crint x, crint y, dnml_status *err);




//* ============================================ SIGNED ARITHMETIC ========================================== */
/* ------------------- MUTATIVE ARITHMETIC -------------------- */
dnml_status crint_mut_mulu64(crint *x, uint64_t val);
dnml_status crint_mut_divu64(crint *x, uint64_t val);
dnml_status crint_mut_modu64(crint *x, uint64_t val);
dnml_status crint_mut_muli64(crint *x, int64_t val);
dnml_status crint_mut_divi64(crint *x, int64_t val);
dnml_status crint_mut_modi64(crint *x, int64_t val);
dnml_status crint_mut_add(crint *x, crint y);
dnml_status crint_mut_sub(crint *x, crint y);
dnml_status crint_mut_mul(crint *x, crint y);
dnml_status crint_mut_div(crint *x, crint y);
dnml_status crint_mut_mod(crint *x, crint y);
/* ------------------ FUNCTIONAL ARITHMETIC ------------------- */
crint crint_mulu64(crint x, uint64_t val, dnml_status *err);
crint crint_divu64(crint x, uint64_t val, dnml_status *err);
crint crint_modu64(crint x, uint64_t val, dnml_status *err);
crint crint_muli64(crint x, int64_t val, dnml_status *err);
crint crint_divi64(crint x, int64_t val, dnml_status *err);
crint crint_modi64(crint x, int64_t val, dnml_status *err);
crint crint_add(crint x, crint y, dnml_status *err);
crint crint_sub(crint x, crint y, dnml_status *err);
crint crint_mul(crint x, crint y, dnml_status *err);
crint crint_div(crint x, crint y, dnml_status *err);
crint crint_mod(crint x, crint y, dnml_status *err);




//* ================================================= COPIES ================================================= */
/* -------------  Mutative SMALL Copies ------------- */
dnml_status crint_mut_copyu64(crint *dst, uint64_t src);
dnml_status crint_mut_dcopyu64(crint *dst, uint64_t src);
dnml_status crint_mut_copyi64(crint *dst, int64_t src);
dnml_status crint_mut_dcopyi64(crint *dst, int64_t src);
/* -------------  Mutative LARGE Copies ------------- */
dnml_status crint_mut_copyf128(crint *dst, long double src);
dnml_status crint_mut_dcopyf128(crint *dst, long double src);
dnml_status crint_mut_ocopyf128(crint *dst, long double src);
dnml_status crint_mut_tover_copyf128(crint *dst, long double src);
dnml_status crint_mut_copy(crint *dst, crint src);
dnml_status crint_mut_dcopy(crint *dst, crint src);
dnml_status crint_mut_ocopy(crint *dst, crint src);
dnml_status crint_mut_tover_copy(crint *dst, crint src);
/* -------------  Functional SMALL Copies ------------- */
crint crint_copyu64(uint64_t src, dnml_status *err);
crint crint_copyi64(int64_t src, dnml_status *err);
/* -------------  Functional LARGE Copies ------------- */
crint crint_copyf128(long double src, dnml_status *err);
crint crint_ocopyf128(long double src, size_t output_cap, dnml_status *err);
crint crint_tover_copyf128(long double src, size_t output_cap, dnml_status *err);
crint crint_copy(crint src, dnml_status *err);
crint crint_ocopy(crint src, size_t output_cap, dnml_status *err);
crint crint_tover_copy(crint src, size_t output_cap, dnml_status *err);




//* ===================================== STATE ALTERATION FUNCTIONS ===================================== *//
dnml_status crint_canonicalize(crint *x);
dnml_status crint_normalize(crint *x);
dnml_status crint_transfer(crint *dst, crint *src);
dnml_status crint_resize(crint *x, size_t k);
dnml_status crint_reserve(crint *x, size_t k);
dnml_status crint_shrink(crint *x, size_t k);
dnml_status crint_reset(crint *x);
uint8_t __STORAGE_VAL__(crint *x);
bool crint_validate(crint x);
bool crint_pvalidate(crint *x);






#endif