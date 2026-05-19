#ifndef dnml_bi_func
#define dnml_bi_func

#include <include.h>
#include <char_tables.h>
#include <system/sys.h>
#include "../../sconfigs/settings/settings.h"
#include "../../sconfigs/memory/arena.h"
#include "../../sconfigs/dnml_status.h"

#include "../../intrinsics/intrinsics.h"
#include "../../calc_algo/perf_calc.h"
#include "../../util/util.h"
#include "bigNums.h"


// Pre-opreration evaluation asserts
#define test_assert(cond, err_code) do { \
    if (_DNML_DEBUG_MODE) { \
        assert(!(cond)); \
    } else { \
        if (cond) return err_code; \
    } \
} while (0);
#define test_assert_mut(cond, err, err_code, retval) do { \
    if (_DNML_DEBUG_MODE) { \
        assert(!(cond)); \
    } else { \
        if (cond) { (*err) = (err_code); return retval; } \
    } \
} while (0);
#define test_assert_pre(cond, err, err_code, baseout, base, retsize ) do { \
    if (_DNML_DEBUG_MODE) { \
        assert(cond); \
    } else { \
        if (cond) { (*err) = (err_code); (*baseout) = (base); return retsize; } \
    } \
} while (0);

// Stream-based Scanning EOF Handling
#define scan_eof(curr_char, stream, ret) do { \
    if (curr_char == EOF) { \
        if (ferror(stream)) return FILE_ERR_PARSE; \
        else return ret; \
    } \
} while (0);


//todo ===================================== NUMERIC FUNCTIONALITIES ===================================== todo//
//* ------------- CONSTRUCTORS & DESCTRUCTORS -------------- */
inline void __BIGINT_FREE__(bigInt *x); // Destructor
void __BIGINT_EMPTY_INIT__(bigInt *__bigInteger); // Default Constructor
void bigInt_linit(bigInt *__bigInteger, size_t __fixed_size);
void __BIGINT_STANDARD_INIT__(bigInt *__bigInteger, const bigInt __preBigInt);
void __BIGINT_UI64_INIT__(bigInt *__bigInteger, uint64_t __unsigned_int);
void __BIGINT_I64_INIT__(bigInt *__bigInteger, int64_t __signed_int);
void __BIGINT_LD_INIT__(bigInt *__bigInteger, long double __float );

#define bigInt_free __BIGINT_FREE__
#define bigInt_init(initializing, initializer) \
    _Generic((initializer)                                                  \
        /* Signed, INTEGER Initialization */                                \
        char:                       __BIGINT_I64_INIT__,                    \
        int:                        __BIGINT_I64_INIT__,                    \
        long:                       __BIGINT_I64_INIT__,                    \
        long long:                  __BIGINT_I64_INIT__,                    \
        int8_t:                     __BIGINT_I64_INIT__,                    \
        int16_t:                    __BIGINT_I64_INIT__,                    \
        int32_t:                    __BIGINT_I64_INIT__,                    \
        int64_t:                    __BIGINT_I64_INIT__,                    \
                                                                            \
        /* Unsigned, INTEGER Initialization */                              \
        unsigned char:              __BIGINT_UI64_INIT__,                   \
        unsigned int:               __BIGINT_UI64_INIT__,                   \
        unsigned long:              __BIGINT_UI64_INIT__,                   \
        unsigned long long:         __BIGINT_UI64_INIT__,                   \
        uint8_t:                    __BIGINT_UI64_INIT__,                   \
        uint16_t:                   __BIGINT_UI64_INIT__,                   \
        uint32_t:                   __BIGINT_UI64_INIT__,                   \
        uint64_t:                   __BIGINT_UI64_INIT__,                   \
                                                                            \
        /* Float Initialization */                                          \
        float:                      __BIGINT_LD_INIT__,                     \
        double:                     __BIGINT_LD_INIT__,                     \
        long double:                __BIGINT_LD_INIT__,                     \
                                                                            \
        /* BigInt Initialization */                                         \
        bigInt                     __BIGINT_STANDARD_INIT__,                \
        size_t                     bigInt_linit,                            \
                                                                            \
        /* Empty Initialization */                                          \
        default:                    __BIGINT_EMPTY_INIT__                   \
)(initializing, initializer);



//* ------------------------ ASSIGNMENTS ------------------------ */
void __BIGINT_SET_BIGINT__(const bigInt x, bigInt *receiver);
dnml_status __BIGINT_SET_BIGINT_SAFE__(const bigInt x, bigInt *receiver);
/* --------- BigInt --> Primitive Types --------- */
void __BIGINT_SET_UI64__(const bigInt x, uint64_t *receiver);
void __BIGINT_SET_I64__(const bigInt x, int64_t *receiver);
void __BIGINT_SET_LD__(const bigInt x, long double *receiver);
dnml_status __BIGINT_SET_UI64_SAFE__(const bigInt x, uint64_t *receiver);
dnml_status __BIGINT_SET_I64_SAFE__(const bigInt x, int64_t *receiver);
dnml_status __BIGINT_SET_LD_SAFE__(const bigInt x, long double *receiver);
/* --------- Primitive Types --> BigInt --------- */
void __BIGINT_GET_UI64__(uint64_t x, bigInt *receiver);
void __BIGINT_GET_I64__(int64_t x, bigInt *receiver);
void __BIGINT_GET_LD__(long double x, bigInt *receiver);
dnml_status __BIGINT_GET_LD_SAFE__(long double x, bigInt *receiver);

#define bigInt_set(receiver, giver) \
    _Generic((receiver)                                         \
        /* Signed, INTEGER Variants */                          \
        char*:                       __BIGINT_SET_I64__,        \
        int*:                        __BIGINT_SET_I64__,        \
        long*:                       __BIGINT_SET_I64__,        \
        long long*:                  __BIGINT_SET_I64__,        \
        int8_t*:                     __BIGINT_SET_I64__,        \
        int16_t*:                    __BIGINT_SET_I64__,        \
        int32_t*:                    __BIGINT_SET_I64__,        \
        int64_t*:                    __BIGINT_SET_I64__,        \
                                                                \
        /* Unsigned, INTEGER Variants */                        \
        unsigned char*:              __BIGINT_SET_UI64__,       \
        unsigned int*:               __BIGINT_SET_UI64__,       \
        unsigned long*:              __BIGINT_SET_UI64__,       \
        unsigned long long*:         __BIGINT_SET_UI64__,       \
        uint8_t*:                    __BIGINT_SET_UI64__,       \
        uint16_t*:                   __BIGINT_SET_UI64__,       \
        uint32_t*:                   __BIGINT_SET_UI64__,       \
        uint64_t*:                   __BIGINT_SET_UI64__,       \
                                                                \
        /* Floating Point Variants */                           \
        float*:                      __BIGINT_SET_LD__,         \
        double*:                     __BIGINT_SET_LD__,         \
        long double*:                __BIGINT_SET_LD__,         \
                                                                \
        /* Normal/BigInt Variant */                             \
        bigInt*                     __BIGINT_SET_BIGINT__       \
)(receiver, giver) /* Don't accept no input */



//* ------------------------ CONVERSIONS ------------------------ */
/* --------- BigInt --> Primitive Types --------- */
uint64_t __BIGINT_TO_UI64__(const bigInt x);
int64_t __BIGINT_TO_I64__(const bigInt x);
long double __BIGINT_TO_LD_(const bigInt x);
uint64_t __BIGINT_TO_UI64_SAFE__(const bigInt x, dnml_status *err);
int64_t __BIGINT_TO_I64_SAFE__(const bigInt x, dnml_status *err);
long double __BIGINT_TO_LD_SAFE_(const bigInt x, dnml_status *err);
/* --------- Primitive Types --> BigInt --------- */
bigInt __BIGINT_FROM_UI64__(uint64_t x);
bigInt __BIGINT_FROM_I64__(int64_t x);
bigInt __BIGINT_FROM_LD_(long double x);
bigInt __BIGINT_FROM_LD_SAFE__(long double x);



//* -------------------- BITWISE OPERATIONS --------------------- */
bigInt __BIGINT_NOT__(const bigInt x);
bigInt __BIGINT_RSHIFT__(const bigInt x, size_t k);
bigInt __BIGINT_LSHIFT__(const bigInt x, size_t k);
void __BIGINT_MUT_RSHIFT__(bigInt *x, size_t k);
void __BIGINT_MUT_LSHIFT__(bigInt *x, size_t k);
/* ------------- Mutative, Fixed-width ------------- */
void __BIGINT_MUT_AND_UI64__  (bigInt *x, uint64_t y);
void __BIGINT_MUT_NAND_UI64__ (bigInt *x, uint64_t y);
void __BIGINT_MUT_OR_UI64__   (bigInt *x, uint64_t y);
void __BIGINT_MUT_NOR_UI64__  (bigInt *x, uint64_t y);
void __BIGINT_MUT_XOR_UI64__  (bigInt *x, uint64_t y);
void __BIGINT_MUT_XNOR_UI64__ (bigInt *x, uint64_t y);
void __BIGINT_MUT_AND__  (bigInt *x, const bigInt y);
void __BIGINT_MUT_NAND__ (bigInt *x, const bigInt y);
void __BIGINT_MUT_OR__   (bigInt *x, const bigInt y);
void __BIGINT_MUT_NOR__  (bigInt *x, const bigInt y);
void __BIGINT_MUT_XOR__  (bigInt *x, const bigInt y);
void __BIGINT_MUT_XNOR__ (bigInt *x, const bigInt y);
/* ------------- Mutative, Explicit-width ------------- */
void __BIGINT_MUT_EX_AND_UI64__  (bigInt *x, uint64_t val, size_t range);
void __BIGINT_MUT_EX_NAND_UI64__ (bigInt *x, uint64_t val, size_t range);
void __BIGINT_MUT_EX_OR_UI64__   (bigInt *x, uint64_t val, size_t range);
void __BIGINT_MUT_EX_NOR_UI64__  (bigInt *x, uint64_t val, size_t range);
void __BIGINT_MUT_EX_XOR_UI64__  (bigInt *x, uint64_t val, size_t range);
void __BIGINT_MUT_EX_XNOR_UI64__ (bigInt *x, uint64_t val, size_t range);
void __BIGINT_MUT_EX_AND_I64__  (bigInt *x, int64_t val, size_t range);
void __BIGINT_MUT_EX_NAND_I64__ (bigInt *x, int64_t val, size_t range);
void __BIGINT_MUT_EX_OR_I64__   (bigInt *x, int64_t val, size_t range);
void __BIGINT_MUT_EX_NOR_I64__  (bigInt *x, int64_t val, size_t range);
void __BIGINT_MUT_EX_XOR_I64__  (bigInt *x, int64_t val, size_t range);
void __BIGINT_MUT_EX_XNOR_I64__ (bigInt *x, int64_t val, size_t range);
void __BIGINT_MUT_EX_AND__   (bigInt *x, const bigInt y, size_t range);
void __BIGINT_MUT_EX_NAND__  (bigInt *x, const bigInt y, size_t range);
void __BIGINT_MUT_EX_OR__    (bigInt *x, const bigInt y, size_t range);
void __BIGINT_MUT_EX_NOR__   (bigInt *x, const bigInt y, size_t range);
void __BIGINT_MUT_EX_XOR__   (bigInt *x, const bigInt y, size_t range);
void __BIGINT_MUT_EX_XNOR__  (bigInt *x, const bigInt y, size_t range);
/* ------------- Functional, Fixed-width ------------- */
bigInt __BIGINT_AND_UI64__  (const bigInt x, uint64_t val);
bigInt __BIGINT_NAND_UI64__ (const bigInt x, uint64_t val);
bigInt __BIGINT_OR_UI64__   (const bigInt x, uint64_t val);
bigInt __BIGINT_NOR_UI64__  (const bigInt x, uint64_t val);
bigInt __BIGINT_XOR_UI64__  (const bigInt x, uint64_t val);
bigInt __BIGINT_XNOR_UI64__ (const bigInt x, uint64_t val);
bigInt __BIGINT_AND__   (const bigInt x, const bigInt y);
bigInt __BIGINT_NAND__  (const bigInt x, const bigInt y);
bigInt __BIGINT_OR__    (const bigInt x, const bigInt y);
bigInt __BIGINT_NOR__   (const bigInt x, const bigInt y);
bigInt __BIGINT_XOR__   (const bigInt x, const bigInt y);
bigInt __BIGINT_XNOR__  (const bigInt x, const bigInt y);
/* ------------- Functional, Explicit-width ------------- */
bigInt __BIGINT_EX_AND_UI64__  (const bigInt x, uint64_t val, size_t width);
bigInt __BIGINT_EX_NAND_UI64__ (const bigInt x, uint64_t val, size_t width);
bigInt __BIGINT_EX_OR_UI64__   (const bigInt x, uint64_t val, size_t width);
bigInt __BIGINT_EX_NOR_UI64__  (const bigInt x, uint64_t val, size_t width);
bigInt __BIGINT_EX_XOR_UI64__  (const bigInt x, uint64_t val, size_t width);
bigInt __BIGINT_EX_XNOR_UI64__ (const bigInt x, uint64_t val, size_t width);
bigInt __BIGINT_EX_AND_I64__  (const bigInt x, int64_t val, size_t width);
bigInt __BIGINT_EX_NAND_I64__ (const bigInt x, int64_t val, size_t width);
bigInt __BIGINT_EX_OR_I64__   (const bigInt x, int64_t val, size_t width);
bigInt __BIGINT_EX_NOR_I64__  (const bigInt x, int64_t val, size_t width);
bigInt __BIGINT_EX_XOR_I64__  (const bigInt x, int64_t val, size_t width);
bigInt __BIGINT_EX_XNOR_I64__ (const bigInt x, int64_t val, size_t width);
bigInt __BIGINT_EX_AND__   (const bigInt x, const bigInt y, size_t width);
bigInt __BIGINT_EX_NAND__  (const bigInt x, const bigInt y, size_t width);
bigInt __BIGINT_EX_OR__    (const bigInt x, const bigInt y, size_t width);
bigInt __BIGINT_EX_NOR__   (const bigInt x, const bigInt y, size_t width);
bigInt __BIGINT_EX_XOR__   (const bigInt x, const bigInt y, size_t width);
bigInt __BIGINT_EX_XNOR__  (const bigInt x, const bigInt y, size_t width);



//* ------------------------ COMPARISONS ------------------------ */
int8_t __BIGINT_COMPARE_MAGNITUDE_UI64__(const bigInt *x, const uint64_t val);
int8_t __BIGINT_COMPARE_MAGNITUDE__(const bigInt *a, const bigInt *b);
/* --------------- Integer - I64 --------------- */
uint8_t __BIGINT_EQUAL_I64__(const bigInt x, const int64_t val);
uint8_t __BIGINT_LESS_I64__(const bigInt x, const int64_t val);
uint8_t __BIGINT_MORE_I64__(const bigInt x, const int64_t val);
uint8_t __BIGINT_LESS_OR_EQUAL_I64__(const bigInt x, const int64_t val);
uint8_t __BIGINT_MORE_OR_EQUALL_I64__(const bigInt x, const int64_t val);
/* ---------- Unsigned Integer - UI64 ---------- */
uint8_t __BIGINT_EQUAL_UI64__(const bigInt x, const uint64_t val);
uint8_t __BIGINT_LESS_UI64__(const bigInt x, const uint64_t val);
uint8_t __BIGINT_MORE_UI64__(const bigInt x, const uint64_t val);
uint8_t __BIGINT_LESS_OR_EQUAL_UI64__(const bigInt x, const uint64_t val);
uint8_t __BIGINT_MORE_OR_EQUALL_UI64__(const bigInt x, const uint64_t val);
/* ------------------- BigInt ------------------ */
uint8_t __BIGINT_EQUAL__(const bigInt a, const bigInt b);
uint8_t __BIGINT_LESS__(const bigInt a, const bigInt b);
uint8_t __BIGINT_MORE__(const bigInt a, const bigInt b);
uint8_t __BIGINT_LESS_OR_EQUAL__(const bigInt a, const bigInt b);
uint8_t __BIGINT_MORE_OR_EQUAL__(const bigInt a, const bigInt b);


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
void __BIGINT_MUT_MUL_UI64__(bigInt *x, uint64_t val);
dnml_status __BIGINT_MUT_DIV_UI64__(bigInt *x, uint64_t val);
dnml_status __BIGINT_MUT_MOD_UI64__(bigInt *x, uint64_t val);
void __BIGINT_MUT_MUL_I64__(bigInt *x, int64_t val);
dnml_status __BIGINT_MUT_DIV_I64__(bigInt *x, int64_t val);
dnml_status __BIGINT_MUT_MOD_I64__(bigInt *x, int64_t val);
void __BIGINT_MUT_ADD__(bigInt *x, const bigInt y);
void __BIGINT_MUT_SUB__(bigInt *x, const bigInt y);
void __BIGINT_MUT_MUL__(bigInt *x, const bigInt y);
dnml_status __BIGINT_MUT_DIV__(bigInt *x, const bigInt y);
dnml_status __BIGINT_MUT_MOD__(bigInt *x, const bigInt y);
/* ------------------ Functional Arithmetic ------------------- */
bigInt __BIGINT_MUL_UI64__(const bigInt x, uint64_t val);
bigInt __BIGINT_DIV_UI64__(const bigInt x, uint64_t val, dnml_status *err);
bigInt __BIGINT_MOD_UI64__(const bigInt x, uint64_t val, dnml_status *err);
bigInt __BIGINT_MUL_I64__(const bigInt x, int64_t val);
bigInt __BIGINT_DIV_I64__(const bigInt x, int64_t val, dnml_status *err);
bigInt __BIGINT_MOD_I64__(const bigInt x, int64_t val, dnml_status *err);
bigInt __BIGINT_ADD__(const bigInt x, const bigInt y);
bigInt __BIGINT_SUB__(const bigInt x, const bigInt y);
bigInt __BIGINT_MUL__(const bigInt x, const bigInt y);
bigInt __BIGINT_DIV__(const bigInt x, const bigInt y, dnml_status *err);
bigInt __BIGINT_MOD__(const bigInt x, const bigInt y, dnml_status *err);



//* -------------------- SIGNED NUMBER-THEORETIC --------------------- */
/* -------------- Pure Number Theoretic -------------- */
bigInt __BIGINT_GCD_UI64__(const bigInt x, uint64_t val);
bigInt __BIGINT_GCD_I64__(const bigInt x, int64_t val);
bigInt __BIGINT_GCD__(const bigInt x, const bigInt y);
bigInt __BIGINT_LCM_UI64__(const bigInt x, uint64_t val);
bigInt __BIGINT_LCM_I64__(const bigInt x, int64_t val);
bigInt __BIGINT_LCM__(const bigInt x, const bigInt y);
bool __BIGINT_IS_PRIME__(const bigInt x);
/* ---------------- Modular Reduction ---------------- */
dnml_status __BIGINT_MUT_MODULO_UI64__(bigInt *x, uint64_t modulus);
dnml_status __BIGINT_MUT_MODULO_I64__(bigInt *x, int64_t modulus);
dnml_status __BIGINT_MUT_MODULO__(bigInt *x, const bigInt modulus);
uint64_t __BIGINT_MODULO_UI64__(const bigInt x, uint64_t modulus, dnml_status *err);
uint64_t __BIGINT_MODULO_I64__(const bigInt x, int64_t modulus, dnml_status *err);
bigInt __BIGINT_MODULO__(const bigInt x, const bigInt modulus, dnml_status *err);
/* ---------------- SMALL Modular Arithmetic --------------- */
dnml_status __BIGINT_MUT_MODADD_UI64__(bigInt *x, const bigInt y, uint64_t modulus);
dnml_status __BIGINT_MUT_MODSUB_UI64__(bigInt *x, const bigInt y, uint64_t modulus);
dnml_status __BIGINT_MUT_MODADD__(bigInt *x, const bigInt y, const bigInt modulus);
dnml_status __BIGINT_MUT_MODSUB__(bigInt *x, const bigInt y, const bigInt modulus);
uint64_t __BIGINT_MODADD_UI64__(const bigInt x, const bigInt y, uint64_t modulus);
uint64_t __BIGINT_MODSUB_UI64__(const bigInt x, const bigInt y, uint64_t modulus);
bigInt __BIGINT_MODADD__(const bigInt x, const bigInt y, const bigInt modulus);
bigInt __BIGINT_MODSUB__(const bigInt x, const bigInt y, const bigInt modulus);
/* ---------------- LARGE Modular Arithmetic --------------- */
dnml_status __BIGINT_MUT_MODMUL_UI64_UI64__(bigInt *x, uint64_t y, uint64_t modulus);
dnml_status __BIGINT_MUT_MODDIV_UI64_UI64__(bigInt *x, uint64_t y, uint64_t modulus);
dnml_status __BIGINT_MUT_MODMUL_BI_UI64__(bigInt *x, const bigInt y, uint64_t modulus);
dnml_status __BIGINT_MUT_MODDIV_BI_UI64__(bigInt *x, const bigInt y, uint64_t modulus);
dnml_status __BIGINT_MUT_MODMUL_UI64_BI__(bigInt *x, uint64_t y, const bigInt modulus);
dnml_status __BIGINT_MUT_MODDIV_UI64_BI__(bigInt *x, uint64_t y, const bigInt modulus);
dnml_status __BIGINT_MUT_MODMUL__(bigInt *x, const bigInt y, const bigInt modulus);
dnml_status __BIGINT_MUT_MODDIV__(bigInt *x, const bigInt y, const bigInt modulus);
uint64_t __BIGINT_MODMUL_UI64_UI64__(const bigInt x, uint64_t y, uint64_t modulus);
uint64_t __BIGINT_MODDIV_UI64_UI64__(const bigInt x, uint64_t y, uint64_t modulus);
uint64_t __BIGINT_MODMUL_BI_UI64__(const bigInt x, const bigInt y, uint64_t modulus);
uint64_t __BIGINT_MODDIV_BI_UI64__(const bigInt x, const bigInt y, uint64_t modulus);
bigInt __BIGINT_MODMUL_UI64_BI__(const bigInt x, uint64_t y, const bigInt modulus);
bigInt __BIGINT_MODDIV_UI64_BI__(const bigInt x, uint64_t y, const bigInt modulus);
bigInt __BIGINT_MODMUL__(const bigInt x, const bigInt y, const bigInt modulus);
bigInt __BIGINT_MODDIV__(const bigInt x, const bigInt y, const bigInt modulus);
/* ---------------------- Modular Algebraic ------------------ */
dnml_status __BIGINT_MUT_MODEXP_UI64__(bigInt *x, const bigInt y, uint64_t modulus);
dnml_status __BIGINT_MUT_MODSQR_UI64__(bigInt *x, uint64_t modulus);
dnml_status __BIGINT_MUT_MODINV_UI64__(bigInt *x, uint64_t modulus);
dnml_status __BIGINT_MUT_MODEXP__(bigInt *x, const bigInt y, const bigInt modulus);
dnml_status __BIGINT_MUT_MODSQR__(bigInt *x, const bigInt modulus);
dnml_status __BIGINT_MUT_MODINV__(bigInt *x, const bigInt modulus);
uint64_t __BIGINT_MODEXP_UI64__(const bigInt x, const bigInt y, uint64_t modulus);
uint64_t __BIGINT_MODSQR_UI64__(const bigInt x, uint64_t modulus);
uint64_t __BIGINT_MODINV_UI64__(const bigInt x, uint64_t modulus);
bigInt __BIGINT_MODEXP__(const bigInt x, const bigInt y, const bigInt modulus);
bigInt __BIGINT_MODSQR__(const bigInt x, const bigInt modulus);
bigInt __BIGINT_MODINV__(const bigInt x, const bigInt modulus);



//* ------------------------- COPIES --------------------------- */
/* -------------  Mutative SMALL Copies ------------- */
void __BIGINT_MUT_COPY_UI64__(bigInt *dst__, uint64_t source__);
void __BIGINT_MUT_COPY_DEEP_UI64__(bigInt *dst__, uint64_t source__);
void __BIGINT_MUT_COPY_I64__(bigInt *dst__, int64_t source__);
void __BIGINT_MUT_COPY_DEEP_I64__(bigInt *dst__, int64_t source__);
/* -------------  Mutative LARGE Copies ------------- */
void __BIGINT_MUT_COPY_LD__(bigInt *dst__, long double source__);
void __BIGINT_MUT_COPY_DEEP_LD__(bigInt *dst__, long double source__);
void __BIGINT_MUT_COPY_OVER_LD__(bigInt *dst__, long double source__);
void __BIGINT_MUT_COPY_TRUNCOVER_LD__(bigInt *dst__, long double source__);
void __BIGINT_MUT_COPY__(bigInt *dst__, const bigInt source__);
void __BIGINT_MUT_COPY_DEEP__(bigInt *dst__, const bigInt source__);
void __BIGINT_MUT_COPY_OVER__(bigInt *dst__, const bigInt source__);
void __BIGINT_MUT_COPY_TRUNCOVER__(bigInt *dst__, const bigInt source__);
/* -------------  Functional SMALL Copies ------------- */
bigInt __BIGINT_COPY_UI64__(uint64_t source__);
bigInt __BIGINT_COPY_I64__(int64_t source__);
/* -------------  Functional LARGE Copies ------------- */
bigInt __BIGINT_COPY_LD__(long double source__);
bigInt __BIGINT_COPY_OVER_LD__(long double source__, size_t output_cap);
bigInt __BIGINT_COPY_TRUNCOVER_LD__(long double source__, size_t output_cap);
bigInt __BIGINT_COPY__(const bigInt source__);
bigInt __BIGINT_COPY_DEEP__(const bigInt source__);
bigInt __BIGINT_COPY_OVER__(const bigInt source__, size_t output_cap, dnml_status *err);
bigInt __BIGINT_COPY_TRUNCOVER__(const bigInt source__, size_t output_cap);



//* -------------------- GENERAL UTILITIES --------------------- */
inline void __BIGINT_CANONICALIZE__(bigInt *x);
inline void __BIGINT_NORMALIZE__(bigInt *x);
void __BIGINT_RESIZE__(bigInt *x, size_t k);
void __BIGINT_RESERVE__(bigInt *x, size_t k);
void __BIGINT_SHRINK__(bigInt *x, size_t k);
inline void __BIGINT_RESET__(bigInt *x);
static inline uint8_t __BIGINT_MUTATIVE_SUBJECT_VALIDATE__(bigInt *x);
static inline uint8_t __BIGINT_STATE_VALIDATE__(bigInt x);
inline uint8_t __BIGINT_VALIDATE__(bigInt x);
inline uint8_t __BIGINT_PVALIDATE__(bigInt *x);

#define bigInt_canonicalize     __BIGINT_CANONICALIZE__
#define bigInt_normalize        __BIGINT_NORMALIZE__
#define bigInt_resize           __BIGINT_RESIZE__
#define bigInt_reserve          __BIGINT_RESERVE__
#define bigInt_shrink           __BIGINT_SHRINK__
#define bigInt_reset            __BIGINT_RESET__





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
void bigInt_fwrite(FILE *stream, const bigInt x);
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
void bigInt_limb_dump(FILE *stream, const bigInt x);
void bigInt_hexdump(FILE *stream, const bigInt x, bool uppercase);
void bigInt_bindump(FILE *stream, const bigInt x); 
void bigInt_info(FILE *stream, const bigInt x);



#endif