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
#include "../../calc_algo/perf_calc.h"
#include "../../util/util.h"

#ifdef __cplusplus
extern "C" {
#endif

#define io_cleanup { arena_clear(&___DASI_IO_ARENA_); arena_destruct(&___DASI_IO_ARENA_); }

//? ======================= COMMON !TEST! ASSERT ERRORS CATALOG ====================== ?//
// Invariant Enforcement
#define input_null "INput Error: Input BigInt is impossible to access due to being a NULL pointer (-Ebigint_null)"
#define full_contract "Validation Error: Input BigInt violates the full invariant contract (-Ebigint_inval)"
#define state_contract "Validation Error: Input BigInt violates contract's state invariants (-Ebigint_sinval)"
#define storage_inval "Validation Error: Input BigInt violates contract's storage invariants (-EbigInt_err_store_in)"
#define aliased_limbs "Validation Error: Input BigInts contain aliased limb pointers (-Ebigint_alias_limb)"
// Unexpected/UB Handling
#define arena_oom "CRITICAL ERROR: Arena initliaizationf failed due to OOM (-Ealloc_arena_oom)"
#define alloc_oom "CRITICAL ERROR: Heap-Allocation Failure - OOM (-Ealloc_oom)"
#define inval_cap "Reserve Capacity Calculation/Assumptions incorrect (-Ereserve_incorrect)"
#define arena_poison_oom "Arena Poisoned: Arena Re-allocation witnessed an OOM error (-Earena_poison)"
// String Format Handling
#define str_null "String Format Error: Input string pointer is null (-Estr_null)"
#define str_empty "String Format Error: Input string is empty (-Estr_empty)"
#define str_inval_base "String Format Error: Input base-parameter is invalid (-Estr_inval_base)"
#define stream_err "File Error: Input stream witnessed an inexplicable error (-Efile_ferror)"



//? ======================= OUT-OF-MEMORY HANDLING MACROS ====================== ?//
/* Functional Macros */
#define heap_alloc_oom(err_check) do { \
    test_assert((((err_check) != DNML_ALLOC_OOM)), alloc_oom, { \
            arena_clear(&___DASI_NUMERIC_ARENA_); arena_clear(&___DASI_LOWLVL_ARENA_); \
            arena_destruct(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_LOWLVL_ARENA_); \
        }, DNML_ALLOC_OOM \
    ); \
} while(0);
#define arena_poisoined(arena_name) do { \
    test_assert( \
        /* Static Analysis - Assert Parameters */ \
        (!((arena_name)->poisoined)), alloc_oom, { \
            arena_clear((arena_name)); arena_destruct((arena_name)); \
            arena_clear(&___DASI_LOWLVL_ARENA_); arena_destruct(&___DASI_LOWLVL_ARENA_); \
        }, DNML_ALLOC_OOM /* Error Returns Parameters */ \
    ) \
} while(0);
#define arena_alloc_oom(err_check, arena_name) do { \
    test_assert( \
        /* Static Analysis - Assert Parameters */ \
        (((err_check) != DNML_ALLOC_OOM)), alloc_oom, { \
            arena_clear((arena_name)); arena_destruct((arena_name)); \
            arena_clear(&___DASI_LOWLVL_ARENA_); arena_destruct(&___DASI_LOWLVL_ARENA_); \
        }, DNML_ALLOC_OOM /* Error Returns Parameters */ \
    ); \
} while(0);


/* Mutative Macros */
#define heap_alloc_oom_void(err_check, err) do { \
    test_assert_mut((((err_check) != DNML_ALLOC_OOM)), alloc_oom, { \
            arena_clear(&___DASI_NUMERIC_ARENA_); arena_clear(&___DASI_LOWLVL_ARENA_); \
            arena_destruct(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_LOWLVL_ARENA_); \
        }, (err), DNML_ALLOC_OOM, ; \
    ); \
} while(0);
#define heap_alloc_oom_mut(err_check, err) do { \
    test_assert_mut((((err_check) != DNML_ALLOC_OOM)), alloc_oom, { \
            arena_clear(&___DASI_NUMERIC_ARENA_); arena_clear(&___DASI_LOWLVL_ARENA_); \
            arena_destruct(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_LOWLVL_ARENA_); \
        }, (err), DNML_ALLOC_OOM, __BIGINT_ERROR_VALUE__() \
    ); \
} while(0);
#define arena_poison_mut(arena_name, err) do { \
    test_assert_mut( \
        /* Static Analysis - Assert Parameters */ \
        (!((arena_name)->poisoined)), alloc_oom, { \
            arena_clear((arena_name)); arena_destruct((arena_name)); \
            arena_clear(&___DASI_LOWLVL_ARENA_); arena_destruct(&___DASI_LOWLVL_ARENA_); \
        }, (err), DNML_ALLOC_OOM, __BIGINT_ERROR_VALUE__() /* Error Returns Parameters */ \
    ) \
} while(0);
#define arena_alloc_oom_mut(err_check, arena_name, err) do { \
    test_assert_mut( \
        /* Static Analysis - Assert Parameters */ \
        (((err_check) != DNML_ALLOC_OOM)), alloc_oom, { \
            arena_clear((arena_name)); arena_destruct((arena_name)); \
            arena_clear(&___DASI_LOWLVL_ARENA_); arena_destruct(&___DASI_LOWLVL_ARENA_); \
        }, (err), DNML_ALLOC_OOM, __BIGINT_ERROR_VALUE__() /* Error Returns Parameters */ \
    ); \
} while(0);
#define func_ret_oom(err) { *(err) = DNML_ALLOC_OOM; return __BIGINT_ERROR_VALUE__(); }
#define ocopy_check(err_check, arena_name) do { \
    DNML_TEST_ASSERT( \
        /* Static Analysis - Assert Parameters */ \
        (((err_check) != BIGINT_ERR_RANGE)), inval_cap, { \
            arena_clear((arena_name)); arena_destruct((arena_name)); \
            arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_); \
        } \
    ); \
} while(0); 

/* General Macros */
#define clear_arena do { \
    arena_clear(&___DASI_NUMERIC_ARENA_); arena_clear(&___DASI_LOWLVL_ARENA_); \
    arena_destruct(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_LOWLVL_ARENA_); \
} while (0);
#define clear_arena_io do { arena_clear(&___DASI_IO_ARENA_); arena_destruct(&___DASI_IO_ARENA_); } while(0);
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
void bigInt_free(bigInt *x); // Destructor
dnml_status bigInt_new(bigInt *x); // Default Constructor
dnml_status bigInt_snew(bigInt *x, size_t n);
dnml_status bigInt_binew(bigInt *x, const bigInt *y);
dnml_status bigInt_new_u64(bigInt *x, const uint64_t in);
dnml_status bigInt_new_i64(bigInt *x, const int64_t in);
dnml_status bigInt_new_f128(bigInt *x, long double in);



//* ------------------------ ASSIGNMENTS ------------------------ */
dnml_status bigInt_set(const bigInt x, bigInt *receiver);
dnml_status bigInt_set_safe(const bigInt x, bigInt *receiver);
/* --------- BigInt --> Primitive Types --------- */
dnml_status bigInt_setu64(const bigInt x, uint64_t* receiver);
dnml_status bigInt_seti64(const bigInt x, int64_t* receiver);
dnml_status bigInt_setf128(const bigInt x, long double* receiver);
dnml_status bigInt_setu64_safe(const bigInt x, uint64_t* receiver);
dnml_status bigInt_seti64_safe(const bigInt x, int64_t* receiver);
dnml_status bigInt_setf128_safe(const bigInt x, long double* receiver);
/* --------- Primitive Types --> BigInt --------- */
dnml_status bigInt_getu64(uint64_t x, bigInt *receiver);
dnml_status bigInt_geti64(int64_t x, bigInt *receiver);
dnml_status bigInt_getf128(long double x, bigInt *receiver);
dnml_status bigInt_getf128_safe(long double x, bigInt *receiver);



//* ------------------------ CONVERSIONS ------------------------ */
/* --------- BigInt --> Primitive Types --------- */
uint64_t bigInt_tou64(const bigInt x, dnml_status *err);
int64_t bigInt_toi64(const bigInt x, dnml_status *err);
long double bigInt_tof128(const bigInt x, dnml_status *err);
uint64_t bigInt_tou64_safe(const bigInt x, dnml_status *err);
int64_t bigInt_toi64_safe(const bigInt x, dnml_status *err);
long double bigInt_tof128_safe(const bigInt x, dnml_status *err);
/* --------- Primitive Types --> BigInt --------- */
bigInt bigInt_fromu64(uint64_t x, dnml_status *err);
bigInt bigInt_fromi64(int64_t x, dnml_status *err);
bigInt bigInt_fromf128(long double x, dnml_status *err);
bigInt bigInt_fromf128_safe(long double x, dnml_status *err);



//* -------------------- BITWISE OPERATIONS --------------------- */
bigInt bigInt_not(const bigInt x, dnml_status *err);
bigInt bigInt_rshift(const bigInt x, size_t k, dnml_status *err);
bigInt bigInt_lshift(const bigInt x, size_t k, dnml_status *err);
bigInt bigInt_lshiftg(const bigInt x, size_t k, dnml_status *err);
dnml_status bigInt_mut_not(bigInt *x);
dnml_status bigInt_mut_rshift(bigInt *x, size_t k);
dnml_status bigInt_mut_lshift(bigInt *x, size_t k);
dnml_status bigInt_mut_lshiftg(bigInt *x, size_t k);
/* ------------- Mutative, Fixed-width ------------- */
dnml_status bigInt_mut_andu64  (bigInt *x, const uint64_t y);
dnml_status bigInt_mut_nandu64 (bigInt *x, const uint64_t y);
dnml_status bigInt_mut_oru64   (bigInt *x, const uint64_t y);
dnml_status bigInt_mut_noru64  (bigInt *x, const uint64_t y);
dnml_status bigInt_mut_xoru64  (bigInt *x, const uint64_t y);
dnml_status bigInt_mut_xnoru64 (bigInt *x, const uint64_t y);
dnml_status bigInt_mut_and  (bigInt *x, const bigInt y);
dnml_status bigInt_mut_nand (bigInt *x, const bigInt y);
dnml_status bigInt_mut_or   (bigInt *x, const bigInt y);
dnml_status bigInt_mut_nor  (bigInt *x, const bigInt y);
dnml_status bigInt_mut_xor  (bigInt *x, const bigInt y);
dnml_status bigInt_mut_xnor (bigInt *x, const bigInt y);
/* ------------- Mutative, Explicit-width ------------- */
dnml_status bigInt_mutex_andu64  (bigInt *x, const uint64_t val, size_t limb_width);
dnml_status bigInt_mutex_nandu64 (bigInt *x, const uint64_t val, size_t limb_width);
dnml_status bigInt_mutex_oru64   (bigInt *x, const uint64_t val, size_t limb_width);
dnml_status bigint_mutex_noru64  (bigInt *x, const uint64_t val, size_t limb_width);
dnml_status bigInt_mutex_xoru64  (bigInt *x, const uint64_t val, size_t limb_width);
dnml_status bigInt_mutex_xnoru64 (bigInt *x, const uint64_t val, size_t limb_width);
dnml_status bigInt_mutex_andi64  (bigInt *x, const int64_t val, size_t limb_width);
dnml_status bigInt_mutex_nandi64 (bigInt *x, const int64_t val, size_t limb_width);
dnml_status bigInt_mutex_ori64   (bigInt *x, const int64_t val, size_t limb_width);
dnml_status bigInt_mutex_nori64  (bigInt *x, const int64_t val, size_t limb_width);
dnml_status bigInt_mutex_xori64  (bigInt *x, const int64_t val, size_t limb_width);
dnml_status bigInt_mutex_xnori64 (bigInt *x, const int64_t val, size_t limb_width);
dnml_status bigInt_mutex_and   (bigInt *x, const bigInt y, size_t limb_width);
dnml_status bigInt_mutex_nand  (bigInt *x, const bigInt y, size_t limb_width);
dnml_status bigInt_mutex_or    (bigInt *x, const bigInt y, size_t limb_width);
dnml_status bigInt_mutex_nor   (bigInt *x, const bigInt y, size_t limb_width);
dnml_status bigInt_mutex_xor   (bigInt *x, const bigInt y, size_t limb_width);
dnml_status bigInt_mutex_xnor  (bigInt *x, const bigInt y, size_t limb_width);
/* ------------- Functional, Fixed-width ------------- */
bigInt bigInt_andu64  (const bigInt x, const uint64_t val, dnml_status *err);
bigInt bigInt_nandu64 (const bigInt x, const uint64_t val, dnml_status *err);
bigInt bigInt_oru64   (const bigInt x, const uint64_t val, dnml_status *err);
bigInt bigInt_noru64  (const bigInt x, const uint64_t val, dnml_status *err);
bigInt bigInt_xoru64  (const bigInt x, const uint64_t val, dnml_status *err);
bigInt bigInt_xnoru64 (const bigInt x, const uint64_t val, dnml_status *err);
bigInt bigInt_and   (const bigInt x, const bigInt y, dnml_status *err);
bigInt bigInt_nand  (const bigInt x, const bigInt y, dnml_status *err);
bigInt bigInt_or    (const bigInt x, const bigInt y, dnml_status *err);
bigInt bigInt_nor   (const bigInt x, const bigInt y, dnml_status *err);
bigInt bigInt_xor   (const bigInt x, const bigInt y, dnml_status *err);
bigInt bigInt_xnor  (const bigInt x, const bigInt y, dnml_status *err);
/* ------------- Functional, Explicit-width ------------- */
bigInt bigInt_ex_andu64  (const bigInt x, const uint64_t val, size_t op_range, dnml_status *err);
bigInt bigInt_ex_nandu64 (const bigInt x, const uint64_t val, size_t op_range, dnml_status *err);
bigInt bigInt_ex_oru64   (const bigInt x, const uint64_t val, size_t op_range, dnml_status *err);
bigInt bigInt_ex_noru64  (const bigInt x, const uint64_t val, size_t op_range, dnml_status *err);
bigInt bigInt_ex_xoru64  (const bigInt x, const uint64_t val, size_t op_range, dnml_status *err);
bigInt bigInt_ex_xnoru64 (const bigInt x, const uint64_t val, size_t op_range, dnml_status *err);
bigInt bigInt_ex_andi64  (const bigInt x, const int64_t val, size_t op_range, dnml_status *err);
bigInt bigInt_ex_nandi64 (const bigInt x, const int64_t val, size_t op_range, dnml_status *err);
bigInt bigInt_ex_ori64   (const bigInt x, const int64_t val, size_t op_range, dnml_status *err);
bigInt bigInt_ex_nori64  (const bigInt x, const int64_t val, size_t op_range, dnml_status *err);
bigInt bigInt_ex_xori64  (const bigInt x, const int64_t val, size_t op_range, dnml_status *err);
bigInt bigInt_ex_xnori64 (const bigInt x, const int64_t val, size_t op_range, dnml_status *err);
bigInt bigInt_ex_and   (const bigInt x, const bigInt y, size_t op_range, dnml_status *err);
bigInt bigInt_ex_nand  (const bigInt x, const bigInt y, size_t op_range, dnml_status *err);
bigInt bigInt_ex_or    (const bigInt x, const bigInt y, size_t op_range, dnml_status *err);
bigInt bigInt_ex_nor   (const bigInt x, const bigInt y, size_t op_range, dnml_status *err);
bigInt bigInt_ex_xor   (const bigInt x, const bigInt y, size_t op_range, dnml_status *err);
bigInt bigInt_ex_xnor  (const bigInt x, const bigInt y, size_t op_range, dnml_status *err);



//* ------------------------ COMPARISONS ------------------------ */
/* --------------- Integer - I64 --------------- */
bool bigInt_equal_i64(const bigInt x, const int64_t val, dnml_status *err);
bool bigInt_less_i64(const bigInt x, const int64_t val, dnml_status *err);
bool bigInt_more_i64(const bigInt x, const int64_t val, dnml_status *err);
bool bigInt_lequal_i64(const bigInt x, const int64_t val, dnml_status *err);
bool bigInt_mequal_i64(const bigInt x, const int64_t val, dnml_status *err);
/* ---------- Unsigned Integer - UI64 ---------- */
bool bigInt_equal_u64(const bigInt x, const uint64_t val, dnml_status *err);
bool bigInt_less_u64(const bigInt x, const uint64_t val, dnml_status *err);
bool bigInt_more_u64(const bigInt x, const uint64_t val, dnml_status *err);
bool bigInt_lequal_u64(const bigInt x, const uint64_t val, dnml_status *err);
bool bigInt_mequal_u64(const bigInt x, const uint64_t val, dnml_status *err);
/* ------------------- BigInt ------------------ */
bool bigInt_equal(const bigInt a, const bigInt b, dnml_status *err);
bool bigInt_less(const bigInt a, const bigInt b, dnml_status *err);
bool bigInt_more(const bigInt a, const bigInt b, dnml_status *err);
bool bigInt_lequal(const bigInt a, const bigInt b, dnml_status *err);
bool bigInt_mequal(const bigInt a, const bigInt b, dnml_status *err);


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
dnml_status bigInt_mut_mulu64(bigInt *x, const uint64_t val);
dnml_status bigInt_mut_divu64(bigInt *x, const uint64_t val);
dnml_status bigInt_mut_modu64(bigInt *x, const uint64_t val);
dnml_status bigInt_mut_muli64(bigInt *x, const int64_t val);
dnml_status bigInt_mut_divi64(bigInt *x, const int64_t val);
dnml_status bigInt_mut_modi64(bigInt *x, const int64_t val);
dnml_status bigInt_mut_add(bigInt *x, const bigInt y);
dnml_status bigInt_mut_sub(bigInt *x, const bigInt y);
dnml_status bigInt_mut_mul(bigInt *x, const bigInt y);
dnml_status bigInt_mut_div(bigInt *x, const bigInt y);
dnml_status bigInt_mut_mod(bigInt *x, const bigInt y);
/* ------------------ Functional Arithmetic ------------------- */
bigInt bigInt_mulu64(const bigInt x, const uint64_t val, dnml_status *err);
bigInt bigInt_divu64(const bigInt x, const uint64_t val, dnml_status *err);
bigInt bigInt_modu64(const bigInt x, const uint64_t val, dnml_status *err);
bigInt bigInt_muli64(const bigInt x, const int64_t val, dnml_status *err);
bigInt bigInt_divi64(const bigInt x, const int64_t val, dnml_status *err);
bigInt bigInt_modi64(const bigInt x, const int64_t val, dnml_status *err);
bigInt bigInt_add(const bigInt x, const bigInt y, dnml_status *err);
bigInt bigInt_sub(const bigInt x, const bigInt y, dnml_status *err);
bigInt bigInt_mul(const bigInt x, const bigInt y, dnml_status *err);
bigInt bigInt_div(const bigInt x, const bigInt y, dnml_status *err);
bigInt bigInt_mod(const bigInt x, const bigInt y, dnml_status *err);



//* -------------------- SIGNED NUMBER-THEORETIC --------------------- */
/* -------------- Pure Number Theoretic -------------- */
bigInt bigInt_gcdu64(const bigInt x, const uint64_t val, dnml_status *err);
bigInt bigInt_gcdi64(const bigInt x, const int64_t val, dnml_status *err);
bigInt bigInt_gcd(const bigInt x, const bigInt y, dnml_status *err);
bigInt bigInt_lcmu64(const bigInt x, const uint64_t val, dnml_status *err);
bigInt bigInt_lcmi64(const bigInt x, const int64_t val, dnml_status *err);
bigInt bigInt_lcm(const bigInt x, const bigInt y, dnml_status *err);
bool bigInt_is_prime(const bigInt x, dnml_status *err);
/* ---------------- Modular Reduction ---------------- */
dnml_status bigInt_mut_emodu64(bigInt *x, const uint64_t mod);
dnml_status bigInt_mut_emodi64(bigInt *x, const int64_t mod);
dnml_status bigInt_mut_emod(bigInt *x, const bigInt mod);
uint64_t bigInt_emodu64(const bigInt x, const uint64_t mod, dnml_status *err);
uint64_t bigInt_emodi64(const bigInt x, const int64_t mod, dnml_status *err);
bigInt bigInt_emod(const bigInt x, const bigInt mod, dnml_status *err);
/* ---------------- SMALL Modular Arithmetic --------------- */
dnml_status bigInt_mut_modadd_u64(bigInt *x, const bigInt y, const uint64_t mod);
dnml_status bigInt_mut_modsub_u64(bigInt *x, const bigInt y, const uint64_t mod);
dnml_status bigInt_mut_modadd(bigInt *x, const bigInt y, const bigInt mod);
dnml_status bigInt_mut_modsub(bigInt *x, const bigInt y, const bigInt mod);
uint64_t bigInt_modadd_u64(const bigInt x, const bigInt y, const uint64_t mod);
uint64_t bigInt_modsub_u64(const bigInt x, const bigInt y, const uint64_t mod);
bigInt bigInt_modadd(const bigInt x, const bigInt y, const bigInt mod);
bigInt bigInt_modsub(const bigInt x, const bigInt y, const bigInt mod);
/* ---------------- LARGE Modular Arithmetic --------------- */
dnml_status bigInt_mut_modmul_u64(bigInt *x, const bigInt y, const uint64_t mod);
dnml_status bigInt_mut_moddiv_u64(bigInt *x, const bigInt y, const uint64_t mod);
dnml_status bigInt_mut_modmul(bigInt *x, const bigInt y, const bigInt mod);
dnml_status bigInt_mut_moddiv(bigInt *x, const bigInt y, const bigInt mod);
uint64_t bigInt_modmul_u64(const bigInt x, const bigInt y, const uint64_t mod);
uint64_t bigInt_moddiv_u64(const bigInt x, const bigInt y, const uint64_t mod);
bigInt bigInt_modmul(const bigInt x, const bigInt y, const bigInt mod);
bigInt bigInt_moddiv(const bigInt x, const bigInt y, const bigInt mod);
/* ---------------------- Modular Algebraic ------------------ */
dnml_status bigInt_mut_modexp_u64(bigInt *x, const bigInt y, const uint64_t mod);
dnml_status bigInt_mut_modsqr_u64(bigInt *x, const uint64_t mod);
dnml_status bigInt_mut_modinv_u64(bigInt *x, const uint64_t mod);
dnml_status bigInt_mut_modexp(bigInt *x, const bigInt y, const bigInt mod);
dnml_status bigInt_mut_modsqr(bigInt *x, const bigInt mod);
dnml_status bigInt_mut_modinv(bigInt *x, const bigInt mod);
uint64_t bigInt_modexp_u64(const bigInt x, const bigInt y, const uint64_t mod);
uint64_t bigInt_modsqr_u64(const bigInt x, const uint64_t mod);
uint64_t bigInt_modinv_u64(const bigInt x, const uint64_t mod);
bigInt bigInt_modexp(const bigInt x, const bigInt y, const bigInt mod);
bigInt bigInt_modsqr(const bigInt x, const bigInt mod);
bigInt bigInt_modinv(const bigInt x, const bigInt mod);



//* -------------------- SIGNED NUMBER-THEORETIC --------------------- */
/* -------------- MUTATIVE ALGEBRAIC -------------- */
dnml_status bigInt_mut_sqr(bigInt *x);
dnml_status bigInt_mut_pow(bigInt *x, const uint64_t exp);
dnml_status bigInt_mut_sqrt(bigInt *x);
dnml_status bigInt_mut_cbrt(bigInt *x);
dnml_status bigInt_mut_nrt(bigInt *x, const uint64_t root);
/* -------------- FUNCTIONAL ALGEBRAIC -------------- */
bigInt bigInt_sqr(const bigInt x, dnml_status *err);
bigInt bigInt_pow(const bigInt x, const uint64_t exp, dnml_status *err);
bigInt bigInt_sqrt(const bigInt x, dnml_status *err);
bigInt bigInt_cbrt(const bigInt x, dnml_status *err);
bigInt bigInt_nrt(const bigInt x, const uint64_t root, dnml_status *err);


//* ------------------------- COPIES --------------------------- */
/* -------------  Mutative SMALL Copies ------------- */
dnml_status bigInt_mut_copyu64(bigInt *dst, const uint64_t src);
dnml_status bigInt_mut_dcopyu64(bigInt *dst, const uint64_t src);
dnml_status bigInt_mut_copyi64(bigInt *dst, const int64_t src);
dnml_status bigInt_mut_dcopyi64(bigInt *dst, const int64_t src);
/* -------------  Mutative LARGE Copies ------------- */
dnml_status bigInt_mut_copyf128(bigInt *dst, long double src);
dnml_status bigInt_mut_dcopyf128(bigInt *dst, long double src);
dnml_status bigInt_mut_ocopyf128(bigInt *dst, long double src);
dnml_status bigInt_mut_tover_copyf128(bigInt *dst, long double src);
dnml_status bigInt_mut_copy(bigInt *dst, const bigInt src);
dnml_status bigInt_mut_dcopy(bigInt *dst, const bigInt src);
dnml_status bigInt_mut_ocopy(bigInt *dst, const bigInt src);
dnml_status bigInt_mut_tover_copy(bigInt *dst, const bigInt src);
/* -------------  Functional SMALL Copies ------------- */
bigInt bigInt_copyu64(const uint64_t src, dnml_status *err);
bigInt bigInt_copyi64(const int64_t src, dnml_status *err);
/* -------------  Functional LARGE Copies ------------- */
bigInt bigInt_copyf128(long double source_, dnml_status *err);
bigInt bigInt_ocopyf128(long double src, size_t output_cap, dnml_status *err);
bigInt bigInt_tover_copyf128(long double src, size_t output_cap, dnml_status *err);
bigInt bigInt_copy(const bigInt src, dnml_status *err);
bigInt bigInt_ocopy(const bigInt src, size_t output_cap, dnml_status *err);
bigInt bigInt_tover_copy(const bigInt src, size_t output_cap, dnml_status *err);



//* -------------------- GENERAL UTILITIES --------------------- */
dnml_status bigInt_canonicalize(bigInt *x);
dnml_status bigInt_normalize(bigInt *x);
dnml_status bigInt_resize(bigInt *x, size_t k);
dnml_status bigInt_reserve(bigInt *x, size_t k);
dnml_status bigInt_shrink(bigInt *x, size_t k);
dnml_status bigInt_reset(bigInt *x);
bool bigInt_validate(bigInt x);
bool bigInt_pvalidate(bigInt *x);





//todo ======================================= I/O FUNCTIONALITIES ======================================= todo//
size_t _finval_char(const char *str, size_t len, uint8_t *base_out);
size_t _finval_charb(const char *str, size_t len, uint8_t base);
//* -------------------- CONSTRUCTORS --------------------- */
dnml_status bigInt_strinit(bigInt *x, const char* str);
dnml_status bigInt_strbinit(bigInt *x, const char* str, uint8_t base);
dnml_status bigInt_strninit(bigInt *x, const char* str, size_t len);
dnml_status bigInt_strnbinit(bigInt *x, const char* str, size_t len, uint8_t base);


//* ------------------------------ ASSIGNMENTS ------------------------------- */
/* Truncative BigInt --> String */
dnml_status bigInt_tto_str(char* str, const bigInt x, size_t *written);
dnml_status bigInt_tto_strb(char* str, const bigInt x, uint8_t base, size_t *written);
dnml_status bigInt_tto_strn(char* str, size_t len, const bigInt x, size_t *written);
dnml_status bigInt_tto_strnb(char* str, size_t len, const bigInt x, uint8_t base, size_t *written);
dnml_status bigInt_tto_strf(
    char* str, size_t len, 
    const bigInt x, uint8_t base, 
    bool uppercase, size_t *written
);
/* Safe BigInt --> String */
dnml_status bigInt_to_str(char* str, const bigInt x, size_t *written);
dnml_status bigInt_to_strb(char* str, const bigInt x, uint8_t base, size_t *written);
dnml_status bigInt_to_strn(char* str, size_t len, const bigInt x, size_t *written);
dnml_status bigInt_to_strnb(char* str, size_t len, const bigInt x, uint8_t base, size_t *written);
dnml_status bigInt_to_strf(
    char* str, size_t len, 
    const bigInt x, uint8_t base, 
    bool uppercase, size_t *written
);
//* -------------------------- BigInt Conversions -------------------------- *//
bigInt bigInt_from_str(const char* str, dnml_status *err);
bigInt bigInt_from_strb(const char* str, uint8_t base, dnml_status *err);
bigInt bigInt_from_strn(const char* str, size_t len, dnml_status *err);
bigInt bigInt_from_strnb(const char* str, size_t len, uint8_t base, dnml_status *err);
//* -------------------------- BigInt Assignments -------------------------- *//
size_t bigInt_get_size(const char *str, size_t len, uint8_t *baseout, dnml_status *err);
size_t bigInt_get_sizeb(const char *str, size_t len, uint8_t base, dnml_status *err);
size_t bigInt_get_sizesa(
    const char *str, size_t len, 
    uint8_t *baseout, size_t bisize, 
    dnml_status *err
);
size_t bigInt_get_sizebsa(
    const char *str, size_t len, 
    uint8_t base, size_t bisize, 
    dnml_status *err
);
/* Default String --> BigInt */
dnml_status bigInt_get_str(bigInt *x, const char *str);
dnml_status bigInt_get_strb(bigInt *x, const char *str, uint8_t base);
dnml_status bigInt_get_strn(bigInt *x, const char *str, size_t len);
dnml_status bigInt_get_strnb(bigInt *x, const char *str, size_t len, uint8_t base);
/* Truncative String --> BigInt */
dnml_status bigInt_tget_str(bigInt *x, const char *str);
dnml_status bigInt_tget_strb(bigInt *x, const char *str, uint8_t base);
dnml_status bigInt_tget_strn(bigInt *x, const char *str, size_t len);
dnml_status bigInt_tget_strnb(bigInt *x, const char *str, size_t len, uint8_t base);
/* Safe String --> BigInt */
dnml_status bigInt_sget_str(bigInt *x, const char *str);
dnml_status bigInt_sget_strb(bigInt *x, const char *str, uint8_t base);
dnml_status bigInt_sget_strn(bigInt *x, const char *str, size_t len);
dnml_status bigInt_sget_strnb(bigInt *x, const char *str, size_t len, uint8_t base);



//* -------------------- DECIMAL INPUTS/OUTPUTS --------------------- */
size_t bigInt_fscan_size(FILE *stream, uint8_t *baseout, dnml_status *err);
size_t bigInt_fscanb_size(FILE *stream, uint8_t base, dnml_status *err);
size_t bigInt_fscansa_size(FILE *stream, uint8_t *baseout, size_t bi_size, dnml_status *err);
size_t bigInt_fscanbsa_size(FILE *stream, uint8_t base, size_t bi_size, dnml_status *err);
/* --------- Decimal Instant OUTPUT ---------  */
dnml_status bigInt_put(const bigInt x);
dnml_status bigInt_putb(const bigInt x, uint8_t base);
dnml_status bigInt_putf(const bigInt x, uint8_t base, bool uppercase);
dnml_status bigInt_fput(FILE *stream, const bigInt x);
dnml_status bigInt_fputb(FILE *stream, const bigInt x, uint8_t base);
dnml_status bigInt_fputf(FILE *stream, const bigInt x, uint8_t base, bool uppercase);
/* --------- Decimal Buffered OUTPUT ---------  */
dnml_status bigInt_sput(const bigInt x);
dnml_status bigInt_sputb(const bigInt x, uint8_t base);
dnml_status bigInt_sputf(const bigInt x, uint8_t base, bool uppercase);
dnml_status bigInt_sfput(FILE *stream, const bigInt x);
dnml_status bigInt_sfputb(FILE *stream, const bigInt x, uint8_t base);
dnml_status bigInt_sfputf(FILE *stream, const bigInt x, uint8_t base, bool uppercase);
/* --------- Standard Stream (stdin) INPUT ---------  */
dnml_status bigInt_scan(bigInt *x);
dnml_status bigInt_scanb(bigInt *x, uint8_t base);
dnml_status bigInt_sscan(bigInt *x);
dnml_status bigInt_sscanb(bigInt *x, uint8_t base);
dnml_status bigInt_tscan(bigInt *x);
dnml_status bigInt_tscanb(bigInt *x, uint8_t base);
/* --------- Custom Stream INPUT ---------  */
dnml_status bigInt_fscan(FILE *stream, bigInt *x);
dnml_status bigInt_fscanb(FILE *stream, bigInt *x, uint8_t base);
dnml_status bigInt_fsscan(FILE *stream, bigInt *x);
dnml_status bigInt_fsscanb(FILE *stream, bigInt *x, uint8_t base);
dnml_status bigInt_ftscan(FILE *stream, bigInt *x);
dnml_status bigInt_ftscanb(FILE *stream, bigInt *x, uint8_t base);



//* -------------------- BINARY INPUTS/OUTPUTS --------------------- */
/* --------- Binary INPUT/OUTPUT ---------  */
dnml_status bigInt_fwrite(FILE *stream, const bigInt x);
size_t bigInt_fread_size(FILE *stream);
dnml_status bigInt_fread(FILE *stream, bigInt *x);
dnml_status bigInt_fsread(FILE *stream, bigInt *x);
dnml_status bigInt_ftread(FILE *stream, bigInt *x);
/* --------- SERIALIZATION / DESERIALIZATION ---------  */
size_t bigInt_serial_size(const bigInt x);
dnml_status bigInt_serialize(char *buf, size_t len, const bigInt x, size_t *written);
dnml_status bigInt_tserialize(char *buf, size_t len, const bigInt x, size_t *written);
dnml_status bigInt_sserialize(char *buf, size_t len, const bigInt x, size_t *written);
size_t bigInt_deserial_size(const char *str, size_t len);
dnml_status bigInt_deserialize(bigInt *x, const char* str, size_t len);



//* -------------------- GENERAL UTILITIES --------------------- */
dnml_status bigInt_limb_dump(FILE *stream, const bigInt x);
dnml_status bigInt_hexdump(FILE *stream, const bigInt x, bool uppercase);
dnml_status bigInt_bindump(FILE *stream, const bigInt x); 
dnml_status bigInt_info(FILE *stream, const bigInt x);


#ifdef __cplusplus
}
#endif


#endif