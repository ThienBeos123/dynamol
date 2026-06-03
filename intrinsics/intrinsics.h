#ifndef __DNML_INTRINSICS_H
#define __DNML_INTRINSICS_H



#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <dnml_sys/sys.h>
#include <_libdnml_config/numeric_config.h>
#include <_libdnml_config/settings.h>

#include "arm64/__arm64_conn__.h"
#include "x86_64/__x86_conn__.h"
#include "risc-v64/__rv64_conn__.h"
#include "zvanillc/__vanillc_conn__.h"
#include "crt_vanillc/__crt_vanillc_con__.h"

#if __ARCH_X86_64__
    #include <x86intrin.h>
#endif

#if __OS_LINUX__
    #include <sys/random.h>      /* getrandom() */
    #include <unistd.h>          /* read() */
    #include <fcntl.h>           /* open() */
#elif __OS_MACOS__ || __OS_IOS__ || __OS_BSD__
    #include <sys/random.h>
    #include <unistd.h>          /* getentropy(), read() */
    #include <fcntl.h>           /* open() */
#elif defined(__OS_WIN64__)
    #include <intrin.h>
    #include <windows.h>
    #include <bcrypt.h>
    #pragma comment(lib, "bcrypt.lib")
#endif


#ifdef __cplusplus
extern "C" {
#endif

//* ---------------------------------------------------------------------------- *//
//*                        IFUNC FUNCTION TABLES - PERF-BASE                     *//
//* ---------------------------------------------------------------------------- *//
typedef struct {
    uint8_t     (*clz64)(uint64_t x);
    uint8_t     (*ctz64)(uint64_t x);
    uint64_t    (*bswap64)(uint64_t x);
    uint8_t     (*pcnt64)(uint64_t x);
} _BITOPS_FTABLE;
typedef struct {
    uint64_t (*add64c)(uint64_t a, uint64_t b, uint8_t *carry);
    uint64_t (*sub64b)(uint64_t a, uint64_t b, uint8_t *borrow);
    uint64_t (*wmul128)(uint64_t a, uint64_t b, uint64_t *hi);
    uint64_t (*wdiv128)(uint64_t lo, uint64_t hi, uint64_t div, uint64_t *rhat, uint8_t *overflowed);
} _ARITH_FTABLE;
typedef struct {
    uint64_t (*modinv64)(uint64_t x);
} _MARITH_FTABLE;
typedef struct {
    uint64_t (*hw_drbg)(int *err); // Uses fast, hardware CSPRNG seeded by hardware entropy
    uint64_t (*hw_trng)(int *err); // Bypasses PRNG and retrieves raw entropy from the hardware
    void (*hw_halt)(void); // Nanoseconds CPU Execution delay
    void (*hw_shalt)(void); // Shallow NOP Halts
} _HW_FTABLE;

extern _BITOPS_FTABLE _libdnml_gbitops_ftable;
extern _ARITH_FTABLE _libdnml_garith_ftable;
extern _MARITH_FTABLE _libdnml_gmarith_ftable;
extern _HW_FTABLE _libdnml_ghw_ftable;

void _libdnml_fill_gbitops(void);
void _libdnml_fill_garith(void);
void _libdnml_fill_gmarith(void);
void _libdnml_fill_ghw(void);




//* ---------------------------------------------------------------------------- *//
//*                        IFUNC FUNCTION TABLES - CRT-BASE                      *//
//* ---------------------------------------------------------------------------- *//
typedef struct {
    uint8_t     (*clz64)(uint64_t x);
    uint8_t     (*ctz64)(uint64_t x);
    uint64_t    (*bswap64)(uint64_t x);
    uint8_t     (*pcnt64)(uint64_t x);
} _CRT_BITOPS_FTABLE;
typedef struct {
    uint64_t (*add64c)(uint64_t a, uint64_t b, uint8_t *carry);
    uint64_t (*sub64b)(uint64_t a, uint64_t b, uint8_t *borrow);
    uint64_t (*wmul128)(uint64_t a, uint64_t b, uint64_t *hi);
    uint64_t (*wdiv128)(uint64_t lo, uint64_t hi, uint64_t div, uint64_t *rhat, uint8_t *overflowed);
} _CRT_ARITH_FTABLE;
typedef struct {} _CRT_ALG_FTABLE;
typedef struct {
    /* Standard U64 Comparisons */
    uint8_t (*lt_func)(uint64_t x, uint64_t y);
    uint8_t (*gt_func)(uint64_t x, uint64_t y);
    uint8_t (*leq_func)(uint64_t x, uint64_t y);
    uint8_t (*geq_func)(uint64_t x, uint64_t y);
    /* Standard I64 Comparisons */
    uint8_t (*lti64_func)(int64_t x, int64_t y);
    uint8_t (*gti64_func)(int64_t x, int64_t y);
    uint8_t (*leqi64_func)(int64_t x, int64_t y);
    uint8_t (*geqi64_func)(int64_t x, int64_t y);
    /* Equality Comparisons */
    uint8_t (*is_neg)(int64_t x);
    uint8_t (*is_pos)(int64_t x);
    uint8_t (*eq_func)(uint64_t x, uint64_t y);
    uint8_t (*neq_func)(uint64_t x, uint64_t y);
    uint64_t (*select_fn)(uint8_t cond, uint64_t a, uint64_t b);
} _CRT_CMP_FTABLE;
typedef struct {
    uint64_t (*hw_drbg)(int *err); // Uses fast, hardware CSPRNG seeded by hardware entropy
    uint64_t (*hw_trng)(int *err); // Bypasses PRNG and retrieves raw entropy from the hardware
    void (*hw_halt)(void); // Nanoseconds CPU Execution delay
    void (*hw_shalt)(void); // Shallow NOP Halts
} _CRT_SEC_FTABLE;

extern _CRT_BITOPS_FTABLE _libdnml_crt_gbitops_ftable;
extern _CRT_ARITH_FTABLE _libdnml_crt_garith_ftable;
extern _CRT_ALG_FTABLE _libdnml_crt_galg_ftable;
extern _CRT_CMP_FTABLE _libdnml_crt_cmp_ftable;
extern _CRT_SEC_FTABLE _libdnml_crt_sec_ftable;

void _libdnml_fill_crt_gbitops(void);
void _libdnml_fill_crt_garith(void);
void _libdnml_fill_crt_galg(void);
void _libdnml_fill_crt_gcmp(void);
void _libdnml_fill_crt_gsec(void);



//* ------------------------------------------------------------------------------------------------------------- *//
//*                                            PERFORMANCE-BASED INTRINSICS                                       *//
//* ------------------------------------------------------------------------------------------------------------- *//
/* ----------------- Normal Arithmetic ----------------- */
uint64_t __ADD_UI64__(uint64_t a, uint64_t b, uint8_t *carry);
uint64_t __SUB_UI64__(uint64_t a, uint64_t b, uint8_t *borrow);
uint64_t __MUL_UI64__(uint64_t a, uint64_t b, uint64_t *hi);
uint64_t __DIV_HELPER_UI64__(uint64_t lo, uint64_t hi, uint64_t div, uint64_t *rhat, uint8_t *overflowed);
/* ----------------- Modular Arithmetic ----------------- */
uint64_t __MODINV_UI64__(uint64_t x);
uint64_t __MODMUL_UI64__(uint64_t a, uint64_t b, uint64_t mod);
uint64_t __MODEXP_UI64__(uint64_t base, uint64_t exp, uint64_t mod);
/* ----------------- Bitwise Operation Intrinsics ----------------- */
uint8_t __CLZ_UI64__(uint64_t x);
uint8_t __CTZ_UI64__(uint64_t x);
uint64_t __BSWAP_UI64__(uint64_t x);
uint8_t __PCNT_UI64__(uint64_t x);
/* ----------------- General Utilities ----------------- */
uint8_t __SAFE_EXP__(uint64_t base, uint64_t exp);
uint8_t __IS_2POW__(uint64_t x);






//* ----------------------------------------------------------------------------------------------------------------- *//
//*                                            CRYPTOGRAPHICAL-BASED INTRINSICS                                       *//
//* ----------------------------------------------------------------------------------------------------------------- *//
/* ---------------- Arithmetic Intrinsics ---------------- */
uint64_t __CRT_ADD_U64__(uint64_t a, uint64_t b, uint8_t *carry);
uint64_t __CRT_SUB_U64__(uint64_t a, uint64_t b, uint8_t *borrow);
uint64_t __CRT_MUL_U64__(uint64_t a, uint64_t b, uint64_t *hi);
uint64_t __CRT_DIV_U128__(uint64_t lo, uint64_t hi, uint64_t div, uint64_t *rhat, uint8_t *overflowed);
/* ------------------ Bitwise Intrinsics ----------------- */
uint8_t __CRT_CLZ_UI64__(uint64_t x);
uint8_t __CRT_CTZ_UI64__(uint64_t x);
uint64_t __CRT_BSWAP_UI64__(uint64_t x);
uint8_t __CRT_PCNT_UI64__(uint64_t x);
/* ----------------- Algebraic Intrinsics ---------------- */
/* ----------------- Comparison Intrinsics --------------- */
uint8_t _lib_crt_lt(uint64_t x, uint64_t y);
uint8_t _lib_crt_gt(uint64_t x, uint64_t y);
uint8_t _lib_crt_leq(uint64_t x, uint64_t y);
uint8_t _lib_crt_geq(uint64_t x, uint64_t y);
uint8_t _lib_crt_lti64(int64_t x, int64_t y);
uint8_t _lib_crt_gti64(int64_t x, int64_t y);
uint8_t _lib_crt_leqi64(int64_t x, int64_t y);
uint8_t _lib_crt_geqi64(int64_t x, int64_t y);
uint8_t _lib_crt_ispos(int64_t x);
uint8_t _lib_crt_isneg(int64_t x);
uint8_t _lib_crt_neq(uint64_t x, uint64_t y);
uint8_t _lib_crt_eq(uint64_t x, uint64_t y);
uint64_t _lib_crt_select(uint8_t cond, uint64_t a, uint64_t b);





//* ------------------------------------------------------------------------------------------------------------------ *//
//*                                            GENERAL, MODULE-SHARED INTRINSICS                                       *//
//* ------------------------------------------------------------------------------------------------------------------ *//
/* ----------------- Independent, Hardware Interactive Intrinsics ----------------- */
// Security-Extension Hardware Functionalities
int __CPU_DBRG_SEED__(void *buf, size_t len, int retry_max, bool crypt, size_t *written);
int __CPU_TRNG_SEED__(void *buf, size_t len, int retry_max, bool crypt, size_t *written);
int __CPU_CSDBRG_SEED__(void *buf, size_t len, int retry_max, size_t *written);
int __CPU_CSTRNG_SEED__(void *buf, size_t len, int retry_max, size_t *written);
// Hardware-Interactive Functionalities
void __CPU_FULL_HALT__(void);
void __CPU_SHALLOW_HALT__(void);


/* ----------------- Cryptographical Operations / Intrinscs ----------------- */
void __GET_ENTROPY_FAST(void* buf, size_t len);
void __GET_ENTROPY_STD(void *buf, size_t len);
int __GET_ENTROPY_PQC(void *buf, size_t len);


#ifdef __cplusplus
}
#endif

#endif
