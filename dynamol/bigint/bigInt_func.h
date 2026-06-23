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



#ifndef dnml_bi_func
#define dnml_bi_func


#include <dnml_status.h>
#include <debug_util.h>
#include <libdnml_types.h>
#include <include.h>
#include <char_tables.h>
#include <dnml_sys/sys.h>
#include <_libdnml_config/settings.h>
#include <_libdnml_mem/arena.h>
#include "../../intrinsics/intrinsics.h"
#include "../../util/util.h"
#include "../../calc_algo/perf_calc.h"
#include "../_dynamol_arena.h"

#ifdef __cplusplus
extern "C" {
#endif
//? ======================= COMMON !TEST! ASSERT ERRORS CATALOG ====================== ?//
// Invariant Enforcement
#define bi_full_contract "BigInt violates the full invariant contract (-Ebigint_inval)"
#define bi_state_contract "BigInt violates contract's state invariants (-Ebigint_state)"
#define bi_storage_inval "BigInt violates contract's storage invariants (-EbigInt_store)"
#define bi_aliased_limbs "BigInts contain aliased limb pointers (-Ebigint_alias)"



//? ======================= OUT-OF-MEMORY HANDLING MACROS ====================== ?//
/* ---------- Functional Macros ---------- */
#define clear_arena do { _cleanup_dynamol(); } while (0);
#define heap_alloc_oom(echeck, free_list, free_cnt) do { \
    test_assert( \
        echeck != DNML_ALLOC_OOM, "Heap allocation failed due to OOM (-Ealloc_oom)", \
        { clear_arena; _FREE_ALL_BI__(free_list, free_cnt); }, \
        { _FREE_RET_BI__(free_list, free_cnt); }, DNML_ALLOC_OOM \
    ) \
} while(0);
#define darena_assert(echeck, free_list, free_cnt) do { \
    test_assert( \
        echeck != DARENA_POISON, "Arena posioned due to OOM upon growth (-Earena_poison)", \
        { clear_arena; _FREE_ALL_BI__(free_list, free_cnt); }, \
        { _FREE_RET_BI__(free_list, free_cnt); }, DARENA_OVERFLOW \
    ); \
    test_assert( \
        echeck != DARENA_OVERFLOW, "Workspace Size Estimation Incorrect (-Earena_overflow)", \
        { clear_arena; _FREE_ALL_BI__(free_list, free_cnt); }, \
        { _FREE_RET_BI__(free_list, free_cnt); }, DARENA_OVERFLOW \
    ); \
} while(0);
#define mag_ovf(echeck, arena_p, mark) if (echeck == DARENA_OVERFLOW) { arena_rewind(arena_p, mark); return DARENA_OVERFLOW; }
#define mag_oom(echeck, arena_p, mark) if (echeck == DNML_ALLOC_OOM) { arena_rewind(arena_p, mark); return DNML_ALLOC_OOM; }
#define mag_heap_oom(echeck, free_list, free_cnt) if (echeck == DNML_ALLOC_OOM) { _free_alloc_list(free_list, free_cnt); return DNML_ALLOC_OOM; }



/* -------------- Mutative Macros -------------- */
#define heap_alloc_oom_bi(echeck, err, free_list, free_cnt) do { \
    test_assert_mut( \
        echeck != DNML_ALLOC_OOM, "Heap allocation failed due to OOM (-Ealloc_oom)", \
        { clear_arena; _FREE_ALL_BI__(free_list, free_cnt); }, \
        { _FREE_RET_BI__(free_list, free_cnt); }, err, echeck, __BIGINT_ERROR_VALUE__() \
    ); \
} while(0);
#define heap_alloc_oom_mut(echeck, err, free_list, free_cnt, retval) do { \
    test_assert_mut( \
        echeck != DNML_ALLOC_OOM, "Heap allocation failed due to OOM (-Ealloc_oom)", \
        { clear_arena; _FREE_ALL_BI__(free_list, free_cnt); }, \
        { _FREE_RET_BI__(free_list, free_cnt); }, err, echeck, retval \
    ); \
} while(0);
#define darena_massert(echeck, err, free_list, free_cnt, retval) do { \
    test_assert_mut( \
        echeck == BIGINT_SUCCESS, "Workspace Size Estimation Incorrect (-Earena_overflow)", \
        { clear_arena; _FREE_ALL_BI__(free_list, free_cnt); }, \
        { _FREE_RET_BI__(free_list, free_cnt) }, err, DARENA_OVERFLOW, retval \
    ); \
} while(0);
#define darena_biassert(echeck, err, free_list, free_cnt) do { \
    test_assert_mut( \
        echeck == BIGINT_SUCCESS, "Workspace Size Estimation Incorrect (-Earena_overflow)", \
        { clear_arena; _FREE_ALL_BI__(free_list, free_cnt); }, \
        { _FREE_RET_BI__(free_list, free_cnt) }, err, DARENA_OVERFLOW, __BIGINT_ERROR_VALUE__(); \
    ); \
} while(0);
#define darena_vassert(echeck, err, free_list, free_cnt) do { \
    test_assert_mut( \
        echeck == BIGINT_SUCCESS, "Workspace Size Estimation Incorrect (-Earena_overflow)", \
        { clear_arena; _FREE_ALL_BI__(free_list, free_cnt); }, \
        { _FREE_RET_BI__(free_list, free_cnt) }, err, DARENA_OVERFLOW, \
    ); \
} while(0);



/* -------- General Macros -------- */
#define mut_gret(err, err_code, ret) do { \
    if ((err) != NULL) *(err) = err_code; return ret; \
} while(0)
#define mut_err(err, err_code) do { \
    if ((err) != NULL) *(err) = err_code; \
    return __BIGINT_ERROR_VALUE__(); \
} while(0)



//todo ===================================== NUMERIC FUNCTIONALITIES ===================================== todo//
dnml_status _init_dynamol_bigint(void);
//* ------------- CONSTRUCTORS & DESCTRUCTORS -------------- */
dnml_status bigInt_free(bigInt *const x); // Destructor
dnml_status bigInt_new(bigInt *const x); // Default Constructor
dnml_status bigInt_snew(bigInt *const x, size_t n);
dnml_status bigInt_binew(bigInt *const x, bigInt *const y);
dnml_status bigInt_newu64(bigInt *const x, const uint64_t in);
dnml_status bigInt_newi64(bigInt *const x, const int64_t in);
dnml_status bigInt_newf128(bigInt *const x, long double in);



//* ------------------------ ASSIGNMENTS ------------------------ */
dnml_status bigInt_set(bigInt x, bigInt *const receiver);
dnml_status bigInt_set_safe(bigInt x, bigInt *const receiver);
/* --------- BigInt --> Primitive Types --------- */
dnml_status bigInt_setu64(bigInt x, uint64_t* receiver);
dnml_status bigInt_seti64(bigInt x, int64_t* receiver);
dnml_status bigInt_setf128(bigInt x, long double* receiver);
dnml_status bigInt_setu64_safe(bigInt x, uint64_t* receiver);
dnml_status bigInt_seti64_safe(bigInt x, int64_t* receiver);
dnml_status bigInt_setf128_safe(bigInt x, long double* receiver);
/* --------- Primitive Types --> BigInt --------- */
dnml_status bigInt_getu64(uint64_t x, bigInt *const receiver);
dnml_status bigInt_geti64(int64_t x, bigInt *const receiver);
dnml_status bigInt_getf128(long double x, bigInt *const receiver);
dnml_status bigInt_getf128_safe(long double x, bigInt *const receiver);



//* ------------------------ CONVERSIONS ------------------------ */
/* --------- BigInt --> Primitive Types --------- */
uint64_t bigInt_tou64(bigInt x, dnml_status *err);
int64_t bigInt_toi64(bigInt x, dnml_status *err);
long double bigInt_tof128(bigInt x, dnml_status *err);
uint64_t bigInt_tou64_safe(bigInt x, dnml_status *err);
int64_t bigInt_toi64_safe(bigInt x, dnml_status *err);
long double bigInt_tof128_safe(bigInt x, dnml_status *err);
/* --------- Primitive Types --> BigInt --------- */
bigInt bigInt_fromu64(uint64_t x, dnml_status *err);
bigInt bigInt_fromi64(int64_t x, dnml_status *err);
bigInt bigInt_fromf128(long double x, dnml_status *err);
bigInt bigInt_fromf128_safe(long double x, dnml_status *err);



//* -------------------- BITWISE OPERATIONS --------------------- */
bigInt bigInt_not(bigInt x, dnml_status *err);
bigInt bigInt_rshift(bigInt x, size_t k, dnml_status *err);
bigInt bigInt_lshift(bigInt x, size_t k, dnml_status *err);
bigInt bigInt_lshiftg(bigInt x, size_t k, dnml_status *err);
dnml_status bigInt_mut_not(bigInt *const x);
dnml_status bigInt_mut_rshift(bigInt *const x, size_t k);
dnml_status bigInt_mut_lshift(bigInt *const x, size_t k);
dnml_status bigInt_mut_lshiftg(bigInt *const x, size_t k);
/* ------------- Mutative, Fixed-width ------------- */
dnml_status bigInt_mut_andu64  (bigInt *const x, const uint64_t y);
dnml_status bigInt_mut_nandu64 (bigInt *const x, const uint64_t y);
dnml_status bigInt_mut_oru64   (bigInt *const x, const uint64_t y);
dnml_status bigInt_mut_noru64  (bigInt *const x, const uint64_t y);
dnml_status bigInt_mut_xoru64  (bigInt *const x, const uint64_t y);
dnml_status bigInt_mut_xnoru64 (bigInt *const x, const uint64_t y);
dnml_status bigInt_mut_and  (bigInt *const x, bigInt y);
dnml_status bigInt_mut_nand (bigInt *const x, bigInt y);
dnml_status bigInt_mut_or   (bigInt *const x, bigInt y);
dnml_status bigInt_mut_nor  (bigInt *const x, bigInt y);
dnml_status bigInt_mut_xor  (bigInt *const x, bigInt y);
dnml_status bigInt_mut_xnor (bigInt *const x, bigInt y);
/* ------------- Mutative, Explicit-width ------------- */
dnml_status bigInt_mutex_andu64  (bigInt *const x, const uint64_t val, size_t limb_width);
dnml_status bigInt_mutex_nandu64 (bigInt *const x, const uint64_t val, size_t limb_width);
dnml_status bigInt_mutex_oru64   (bigInt *const x, const uint64_t val, size_t limb_width);
dnml_status bigint_mutex_noru64  (bigInt *const x, const uint64_t val, size_t limb_width);
dnml_status bigInt_mutex_xoru64  (bigInt *const x, const uint64_t val, size_t limb_width);
dnml_status bigInt_mutex_xnoru64 (bigInt *const x, const uint64_t val, size_t limb_width);
dnml_status bigInt_mutex_andi64  (bigInt *const x, const int64_t val, size_t limb_width);
dnml_status bigInt_mutex_nandi64 (bigInt *const x, const int64_t val, size_t limb_width);
dnml_status bigInt_mutex_ori64   (bigInt *const x, const int64_t val, size_t limb_width);
dnml_status bigInt_mutex_nori64  (bigInt *const x, const int64_t val, size_t limb_width);
dnml_status bigInt_mutex_xori64  (bigInt *const x, const int64_t val, size_t limb_width);
dnml_status bigInt_mutex_xnori64 (bigInt *const x, const int64_t val, size_t limb_width);
dnml_status bigInt_mutex_and   (bigInt *const x, bigInt y, size_t limb_width);
dnml_status bigInt_mutex_nand  (bigInt *const x, bigInt y, size_t limb_width);
dnml_status bigInt_mutex_or    (bigInt *const x, bigInt y, size_t limb_width);
dnml_status bigInt_mutex_nor   (bigInt *const x, bigInt y, size_t limb_width);
dnml_status bigInt_mutex_xor   (bigInt *const x, bigInt y, size_t limb_width);
dnml_status bigInt_mutex_xnor  (bigInt *const x, bigInt y, size_t limb_width);
/* ------------- Functional, Fixed-width ------------- */
bigInt bigInt_andu64  (bigInt x, const uint64_t val, dnml_status *err);
bigInt bigInt_nandu64 (bigInt x, const uint64_t val, dnml_status *err);
bigInt bigInt_oru64   (bigInt x, const uint64_t val, dnml_status *err);
bigInt bigInt_noru64  (bigInt x, const uint64_t val, dnml_status *err);
bigInt bigInt_xoru64  (bigInt x, const uint64_t val, dnml_status *err);
bigInt bigInt_xnoru64 (bigInt x, const uint64_t val, dnml_status *err);
bigInt bigInt_and   (bigInt x, bigInt y, dnml_status *err);
bigInt bigInt_nand  (bigInt x, bigInt y, dnml_status *err);
bigInt bigInt_or    (bigInt x, bigInt y, dnml_status *err);
bigInt bigInt_nor   (bigInt x, bigInt y, dnml_status *err);
bigInt bigInt_xor   (bigInt x, bigInt y, dnml_status *err);
bigInt bigInt_xnor  (bigInt x, bigInt y, dnml_status *err);
/* ------------- Functional, Explicit-width ------------- */
bigInt bigInt_ex_andu64  (bigInt x, const uint64_t val, size_t op_range, dnml_status *err);
bigInt bigInt_ex_nandu64 (bigInt x, const uint64_t val, size_t op_range, dnml_status *err);
bigInt bigInt_ex_oru64   (bigInt x, const uint64_t val, size_t op_range, dnml_status *err);
bigInt bigInt_ex_noru64  (bigInt x, const uint64_t val, size_t op_range, dnml_status *err);
bigInt bigInt_ex_xoru64  (bigInt x, const uint64_t val, size_t op_range, dnml_status *err);
bigInt bigInt_ex_xnoru64 (bigInt x, const uint64_t val, size_t op_range, dnml_status *err);
bigInt bigInt_ex_andi64  (bigInt x, const int64_t val, size_t op_range, dnml_status *err);
bigInt bigInt_ex_nandi64 (bigInt x, const int64_t val, size_t op_range, dnml_status *err);
bigInt bigInt_ex_ori64   (bigInt x, const int64_t val, size_t op_range, dnml_status *err);
bigInt bigInt_ex_nori64  (bigInt x, const int64_t val, size_t op_range, dnml_status *err);
bigInt bigInt_ex_xori64  (bigInt x, const int64_t val, size_t op_range, dnml_status *err);
bigInt bigInt_ex_xnori64 (bigInt x, const int64_t val, size_t op_range, dnml_status *err);
bigInt bigInt_ex_and   (bigInt x, bigInt y, size_t op_range, dnml_status *err);
bigInt bigInt_ex_nand  (bigInt x, bigInt y, size_t op_range, dnml_status *err);
bigInt bigInt_ex_or    (bigInt x, bigInt y, size_t op_range, dnml_status *err);
bigInt bigInt_ex_nor   (bigInt x, bigInt y, size_t op_range, dnml_status *err);
bigInt bigInt_ex_xor   (bigInt x, bigInt y, size_t op_range, dnml_status *err);
bigInt bigInt_ex_xnor  (bigInt x, bigInt y, size_t op_range, dnml_status *err);



//* ------------------------ COMPARISONS ------------------------ */
/* --------------- Integer - I64 --------------- */
bool bigInt_equal_i64(bigInt x, const int64_t val, dnml_status *err);
bool bigInt_less_i64(bigInt x, const int64_t val, dnml_status *err);
bool bigInt_more_i64(bigInt x, const int64_t val, dnml_status *err);
bool bigInt_lequal_i64(bigInt x, const int64_t val, dnml_status *err);
bool bigInt_mequal_i64(bigInt x, const int64_t val, dnml_status *err);
/* ---------- Unsigned Integer - UI64 ---------- */
bool bigInt_equal_u64(bigInt x, const uint64_t val, dnml_status *err);
bool bigInt_less_u64(bigInt x, const uint64_t val, dnml_status *err);
bool bigInt_more_u64(bigInt x, const uint64_t val, dnml_status *err);
bool bigInt_lequal_u64(bigInt x, const uint64_t val, dnml_status *err);
bool bigInt_mequal_u64(bigInt x, const uint64_t val, dnml_status *err);
/* ------------------- BigInt ------------------ */
bool bigInt_equal(bigInt a, bigInt b, dnml_status *err);
bool bigInt_less(bigInt a, bigInt b, dnml_status *err);
bool bigInt_more(bigInt a, bigInt b, dnml_status *err);
bool bigInt_lequal(bigInt a, bigInt b, dnml_status *err);
bool bigInt_mequal(bigInt a, bigInt b, dnml_status *err);


//* -------------------- SIGNED ARITHMETIC --------------------- */
/*  - These arithmetic functions handles:
*       +) Special/Edge cases
*       +) Fast Paths
*       +) Normalization
*       +) Illegal Operation
*   - They are designed to improve performance by implementing fast paths,
*     decrease boilerplate, and provide safe, public, surface-level interface for bigInt operations
*   - These function are included in two different sections below:
*       +) MUTATIVE ARITHMETIC      ---> In-place mutation of a variable                (Eg: x += 1     )
*       +) FUNCTIONAL ARITHMETIC    ---> Return a new copy of a value to be asigned     (Eg: x  = 1 + 2;)
*/
/* ------------------- Mutative Arithmetic -------------------- */
dnml_status bigInt_mut_mulu64(bigInt *const x, const uint64_t val);
dnml_status bigInt_mut_divu64(bigInt *const x, const uint64_t val);
dnml_status bigInt_mut_modu64(bigInt *const x, const uint64_t val);
dnml_status bigInt_mut_muli64(bigInt *const x, const int64_t val);
dnml_status bigInt_mut_divi64(bigInt *const x, const int64_t val);
dnml_status bigInt_mut_modi64(bigInt *const x, const int64_t val);
dnml_status bigInt_mut_add(bigInt *const x, bigInt y);
dnml_status bigInt_mut_sub(bigInt *const x, bigInt y);
dnml_status bigInt_mut_mul(bigInt *const x, bigInt y);
dnml_status bigInt_mut_div(bigInt *const x, bigInt y);
dnml_status bigInt_mut_mod(bigInt *const x, bigInt y);
/* ------------------ Functional Arithmetic ------------------- */
bigInt bigInt_mulu64(bigInt x, const uint64_t val, dnml_status *err);
bigInt bigInt_divu64(bigInt x, const uint64_t val, dnml_status *err);
bigInt bigInt_modu64(bigInt x, const uint64_t val, dnml_status *err);
bigInt bigInt_muli64(bigInt x, const int64_t val, dnml_status *err);
bigInt bigInt_divi64(bigInt x, const int64_t val, dnml_status *err);
bigInt bigInt_modi64(bigInt x, const int64_t val, dnml_status *err);
bigInt bigInt_add(bigInt x, bigInt y, dnml_status *err);
bigInt bigInt_sub(bigInt x, bigInt y, dnml_status *err);
bigInt bigInt_mul(bigInt x, bigInt y, dnml_status *err);
bigInt bigInt_div(bigInt x, bigInt y, dnml_status *err);
bigInt bigInt_mod(bigInt x, bigInt y, dnml_status *err);



//* -------------------- SIGNED NUMBER-THEORETIC --------------------- */
/* -------------- Pure Number Theoretic -------------- */
bigInt bigInt_gcdu64(bigInt x, const uint64_t val, dnml_status *err);
bigInt bigInt_gcdi64(bigInt x, const int64_t val, dnml_status *err);
bigInt bigInt_gcd(bigInt x, bigInt y, dnml_status *err);
bigInt bigInt_lcmu64(bigInt x, const uint64_t val, dnml_status *err);
bigInt bigInt_lcmi64(bigInt x, const int64_t val, dnml_status *err);
bigInt bigInt_lcm(bigInt x, bigInt y, dnml_status *err);
bool bigInt_is_prime(bigInt x, dnml_status *err);
/* ---------------- Modular Reduction ---------------- */
dnml_status bigInt_mut_emodu64(bigInt *const x, const uint64_t mod);
dnml_status bigInt_mut_emodi64(bigInt *const x, const int64_t mod);
dnml_status bigInt_mut_emod(bigInt *const x, bigInt mod);
uint64_t bigInt_emodu64(bigInt x, const uint64_t mod, dnml_status *err);
uint64_t bigInt_emodi64(bigInt x, const int64_t mod, dnml_status *err);
bigInt bigInt_emod(bigInt x, bigInt mod, dnml_status *err);
/* ---------------- SMALL Modular Arithmetic --------------- */
dnml_status bigInt_mut_modadd_u64(bigInt *const x, bigInt y, const uint64_t mod);
dnml_status bigInt_mut_modsub_u64(bigInt *const x, bigInt y, const uint64_t mod);
dnml_status bigInt_mut_modadd(bigInt *const x, bigInt y, bigInt mod);
dnml_status bigInt_mut_modsub(bigInt *const x, bigInt y, bigInt mod);
uint64_t bigInt_modadd_u64(bigInt x, bigInt y, const uint64_t mod);
uint64_t bigInt_modsub_u64(bigInt x, bigInt y, const uint64_t mod);
bigInt bigInt_modadd(bigInt x, bigInt y, bigInt mod);
bigInt bigInt_modsub(bigInt x, bigInt y, bigInt mod);
/* ---------------- LARGE Modular Arithmetic --------------- */
dnml_status bigInt_mut_modmul_u64(bigInt *const x, bigInt y, const uint64_t mod);
dnml_status bigInt_mut_moddiv_u64(bigInt *const x, bigInt y, const uint64_t mod);
dnml_status bigInt_mut_modmul(bigInt *const x, bigInt y, bigInt mod);
dnml_status bigInt_mut_moddiv(bigInt *const x, bigInt y, bigInt mod);
uint64_t bigInt_modmul_u64(bigInt x, bigInt y, const uint64_t mod);
uint64_t bigInt_moddiv_u64(bigInt x, bigInt y, const uint64_t mod);
bigInt bigInt_modmul(bigInt x, bigInt y, bigInt mod);
bigInt bigInt_moddiv(bigInt x, bigInt y, bigInt mod);
/* ---------------------- Modular Algebraic ------------------ */
dnml_status bigInt_mut_modexp_u64(bigInt *const x, bigInt y, const uint64_t mod);
dnml_status bigInt_mut_modsqr_u64(bigInt *const x, const uint64_t mod);
dnml_status bigInt_mut_modinv_u64(bigInt *const x, const uint64_t mod);
dnml_status bigInt_mut_modexp(bigInt *const x, bigInt y, bigInt mod);
dnml_status bigInt_mut_modsqr(bigInt *const x, bigInt mod);
dnml_status bigInt_mut_modinv(bigInt *const x, bigInt mod);
uint64_t bigInt_modexp_u64(bigInt x, bigInt y, const uint64_t mod);
uint64_t bigInt_modsqr_u64(bigInt x, const uint64_t mod);
uint64_t bigInt_modinv_u64(bigInt x, const uint64_t mod);
bigInt bigInt_modexp(bigInt x, bigInt y, bigInt mod);
bigInt bigInt_modsqr(bigInt x, bigInt mod);
bigInt bigInt_modinv(bigInt x, bigInt mod);



//* -------------------- SIGNED NUMBER-THEORETIC --------------------- */
/* -------------- MUTATIVE ALGEBRAIC -------------- */
dnml_status bigInt_mut_sqr(bigInt *const x);
dnml_status bigInt_mut_pow(bigInt *const x, const uint64_t exp);
dnml_status bigInt_mut_sqrt(bigInt *const x);
dnml_status bigInt_mut_cbrt(bigInt *const x);
dnml_status bigInt_mut_nrt(bigInt *const x, const uint64_t root);
/* -------------- FUNCTIONAL ALGEBRAIC -------------- */
bigInt bigInt_sqr(bigInt x, dnml_status *err);
bigInt bigInt_pow(bigInt x, const uint64_t exp, dnml_status *err);
bigInt bigInt_sqrt(bigInt x, dnml_status *err);
bigInt bigInt_cbrt(bigInt x, dnml_status *err);
bigInt bigInt_nrt(bigInt x, const uint64_t root, dnml_status *err);


//* ------------------------- COPIES --------------------------- */
/* -------------  Mutative SMALL Copies ------------- */
dnml_status bigInt_mut_copyu64(bigInt *const dst, const uint64_t src);
dnml_status bigInt_mut_dcopyu64(bigInt *const dst, const uint64_t src);
dnml_status bigInt_mut_copyi64(bigInt *const dst, const int64_t src);
dnml_status bigInt_mut_dcopyi64(bigInt *const dst, const int64_t src);
/* -------------  Mutative LARGE Copies ------------- */
dnml_status bigInt_mut_copyf128(bigInt *const dst, long double src);
dnml_status bigInt_mut_dcopyf128(bigInt *const dst, long double src);
dnml_status bigInt_mut_ocopyf128(bigInt *const dst, long double src);
dnml_status bigInt_mut_tover_copyf128(bigInt *const dst, long double src);
dnml_status bigInt_mut_copy(bigInt *const dst, bigInt src);
dnml_status bigInt_mut_dcopy(bigInt *const dst, bigInt src);
dnml_status bigInt_mut_ocopy(bigInt *const dst, bigInt src);
dnml_status bigInt_mut_tover_copy(bigInt *const dst, bigInt src);
/* -------------  Functional SMALL Copies ------------- */
bigInt bigInt_copyu64(const uint64_t src, dnml_status *err);
bigInt bigInt_copyi64(const int64_t src, dnml_status *err);
/* -------------  Functional LARGE Copies ------------- */
bigInt bigInt_copyf128(long double source_, dnml_status *err);
bigInt bigInt_ocopyf128(long double src, size_t output_cap, dnml_status *err);
bigInt bigInt_tover_copyf128(long double src, size_t output_cap, dnml_status *err);
bigInt bigInt_copy(bigInt src, dnml_status *err);
bigInt bigInt_ocopy(bigInt src, size_t output_cap, dnml_status *err);
bigInt bigInt_tover_copy(bigInt src, size_t output_cap, dnml_status *err);



//* -------------------- GENERAL UTILITIES --------------------- */
dnml_status bigInt_canonicalize(bigInt *const x);
dnml_status bigInt_normalize(bigInt *const x);
dnml_status bigInt_resize(bigInt *const x, size_t k);
dnml_status bigInt_reserve(bigInt *const x, size_t k);
dnml_status bigInt_shrink(bigInt *const x, size_t k);
dnml_status bigInt_reset(bigInt *const x);
bool bigInt_validate(bigInt x);
bool bigInt_pvalidate(bigInt *const x);





//todo ======================================= I/O FUNCTIONALITIES ======================================= todo//
size_t _finval_char(const char *str, size_t len, uint8_t *base_out);
size_t _finval_charb(const char *str, size_t len, uint8_t base);
//* -------------------- CONSTRUCTORS --------------------- */
dnml_status bigInt_strinit(bigInt *const x, const char* str);
dnml_status bigInt_strbinit(bigInt *const x, const char* str, uint8_t base);
dnml_status bigInt_strninit(bigInt *const x, const char* str, size_t len);
dnml_status bigInt_strnbinit(bigInt *const x, const char* str, size_t len, uint8_t base);


//* ------------------------------ ASSIGNMENTS ------------------------------- */
/* Truncative BigInt --> String */
dnml_status bigInt_tto_str(char* str, bigInt x, size_t *written);
dnml_status bigInt_tto_strb(char* str, bigInt x, uint8_t base, size_t *written);
dnml_status bigInt_tto_strn(char* str, size_t len, bigInt x, size_t *written);
dnml_status bigInt_tto_strnb(char* str, size_t len, bigInt x, uint8_t base, size_t *written);
dnml_status bigInt_tto_strf(
    char* str, size_t len,
    bigInt x, uint8_t base,
    bool uppercase, size_t *written
);
/* Safe BigInt --> String */
dnml_status bigInt_to_str(char* str, bigInt x, size_t *written);
dnml_status bigInt_to_strb(char* str, bigInt x, uint8_t base, size_t *written);
dnml_status bigInt_to_strn(char* str, size_t len, bigInt x, size_t *written);
dnml_status bigInt_to_strnb(char* str, size_t len, bigInt x, uint8_t base, size_t *written);
dnml_status bigInt_to_strf(
    char* str, size_t len,
    bigInt x, uint8_t base,
    bool uppercase, size_t *written
);
//* -------------------------- BigInt Conversions -------------------------- *//
bigInt bigInt_from_str(const char* str, dnml_status *err);
bigInt bigInt_from_strb(const char* str, uint8_t base, dnml_status *err);
bigInt bigInt_from_strn(const char* str, size_t len, dnml_status *err);
bigInt bigInt_from_strnb(const char* str, size_t len, uint8_t base, dnml_status *err);
//* -------------------------- BigInt Assignments -------------------------- *//
/* Default String --> BigInt */
dnml_status bigInt_get_str(bigInt *const x, const char *str);
dnml_status bigInt_get_strb(bigInt *const x, const char *str, uint8_t base);
dnml_status bigInt_get_strn(bigInt *const x, const char *str, size_t len);
dnml_status bigInt_get_strnb(bigInt *const x, const char *str, size_t len, uint8_t base);
/* Truncative String --> BigInt */
dnml_status bigInt_tget_str(bigInt *const x, const char *str);
dnml_status bigInt_tget_strb(bigInt *const x, const char *str, uint8_t base);
dnml_status bigInt_tget_strn(bigInt *const x, const char *str, size_t len);
dnml_status bigInt_tget_strnb(bigInt *const x, const char *str, size_t len, uint8_t base);
/* Safe String --> BigInt */
dnml_status bigInt_sget_str(bigInt *const x, const char *str);
dnml_status bigInt_sget_strb(bigInt *const x, const char *str, uint8_t base);
dnml_status bigInt_sget_strn(bigInt *const x, const char *str, size_t len);
dnml_status bigInt_sget_strnb(bigInt *const x, const char *str, size_t len, uint8_t base);



//* -------------------- DECIMAL INPUTS/OUTPUTS --------------------- */
/* --------- Decimal Instant OUTPUT ---------  */
dnml_status bigInt_put(bigInt x);
dnml_status bigInt_putb(bigInt x, uint8_t base);
dnml_status bigInt_putf(bigInt x, uint8_t base, bool uppercase);
dnml_status bigInt_fput(FILE *stream, bigInt x);
dnml_status bigInt_fputb(FILE *stream, bigInt x, uint8_t base);
dnml_status bigInt_fputf(FILE *stream, bigInt x, uint8_t base, bool uppercase);
/* --------- Decimal Buffered OUTPUT ---------  */
dnml_status bigInt_sput(bigInt x);
dnml_status bigInt_sputb(bigInt x, uint8_t base);
dnml_status bigInt_sputf(bigInt x, uint8_t base, bool uppercase);
dnml_status bigInt_sfput(FILE *stream, bigInt x);
dnml_status bigInt_sfputb(FILE *stream, bigInt x, uint8_t base);
dnml_status bigInt_sfputf(FILE *stream, bigInt x, uint8_t base, bool uppercase);
/* --------- Standard Stream (stdin) INPUT ---------  */
dnml_status bigInt_scan(bigInt *const x);
dnml_status bigInt_scanb(bigInt *const x, uint8_t base);
dnml_status bigInt_sscan(bigInt *const x);
dnml_status bigInt_sscanb(bigInt *const x, uint8_t base);
dnml_status bigInt_tscan(bigInt *const x);
dnml_status bigInt_tscanb(bigInt *const x, uint8_t base);
/* --------- Custom Stream INPUT ---------  */
dnml_status bigInt_fscan(FILE *stream, bigInt *const x);
dnml_status bigInt_fscanb(FILE *stream, bigInt *const x, uint8_t base);
dnml_status bigInt_fsscan(FILE *stream, bigInt *const x);
dnml_status bigInt_fsscanb(FILE *stream, bigInt *const x, uint8_t base);
dnml_status bigInt_ftscan(FILE *stream, bigInt *const x);
dnml_status bigInt_ftscanb(FILE *stream, bigInt *const x, uint8_t base);



//* -------------------- BINARY INPUTS/OUTPUTS --------------------- */
/* --------- Binary INPUT/OUTPUT ---------  */
dnml_status bigInt_fwrite(FILE *stream, bigInt x);
dnml_status bigInt_fread(FILE *stream, bigInt *const x);
dnml_status bigInt_fsread(FILE *stream, bigInt *const x);
dnml_status bigInt_ftread(FILE *stream, bigInt *const x);
/* --------- SERIALIZATION / DESERIALIZATION ---------  */
dnml_status bigInt_serialize(char *buf, size_t len, bigInt x, size_t *written);
dnml_status bigInt_tserialize(char *buf, size_t len, bigInt x, size_t *written);
dnml_status bigInt_sserialize(char *buf, size_t len, bigInt x, size_t *written);
dnml_status bigInt_deserialize(bigInt *const x, const char* str, size_t len);



//* -------------------- GENERAL UTILITIES --------------------- */
dnml_status bigInt_limb_dump(FILE *stream, bigInt x);
dnml_status bigInt_hexdump(FILE *stream, bigInt x, bool uppercase);
dnml_status bigInt_bindump(FILE *stream, bigInt x);
dnml_status bigInt_info(FILE *stream, bigInt x);


#ifdef __cplusplus
}
#endif


#endif
