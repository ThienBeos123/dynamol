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

#include "arm64/_arm64_conn.h"
#include "x86_64/_x86_conn.h"
#include "risc-v64/_rv64_conn.h"
#include "zvanillc/_vanillc_conn.h"

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

// //* ---------------------------------------------------------------------------- *//
// //*                              IFUNC FUNCTION TABLES                           *//
// //* ---------------------------------------------------------------------------- *//
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
    uint64_t (*wdiv128)(
        uint64_t lo, uint64_t hi, uint64_t div, 
        uint64_t *rhat
    );
} _ARITH_FTABLE;
typedef struct {
    uint64_t (*modinv64)(uint64_t x);
} _MARITH_FTABLE;
typedef struct {
} _ALG_FTABLE;
typedef struct {
    uint64_t (*hw_drbg)(int *err); // Uses fast, hardware CSPRNG seeded by hardware entropy
    uint64_t (*hw_trng)(int *err); // Bypasses PRNG and retrieves raw entropy from the hardware
    void (*hw_halt)(void); // Nanoseconds CPU Execution delay
    void (*hw_shalt)(void); // Shallow NOP Halts
} _HW_FTABLE;

extern _BITOPS_FTABLE _libdnml_gbitops_ftable;
extern _ARITH_FTABLE _libdnml_garith_ftable;
extern _MARITH_FTABLE _libdnml_gmarith_ftable;
extern _ALG_FTABLE _libdnml_galg_ftable;
extern _HW_FTABLE _libdnml_ghw_ftable;

void _libdnml_fill_gbitops(void);
void _libdnml_fill_garith(void);
void _libdnml_fill_gmarith(void);
void _libdnml_fill_galg(void);
void _libdnml_fill_ghw(void);


//* --------------------------------------------------------------------------------------- *//
//*                                    SINGLE-LIMB ARITHMETIC                               *//
//* --------------------------------------------------------------------------------------- *//
uint64_t _cintrin_divwrap(uint64_t lo, uint64_t hi, uint64_t div, uint64_t *rhat);
uint64_t __ADD_UI64__(uint64_t a, uint64_t b, uint8_t *carry);
uint64_t __SUB_UI64__(uint64_t a, uint64_t b, uint8_t *borrow);
uint64_t __MUL_UI64__(uint64_t a, uint64_t b, uint64_t *hi);
uint64_t __DIV_HELPER_UI64__(
    uint64_t lo, uint64_t hi, uint64_t div, 
    uint64_t *rhat
);
uint64_t __MODINV_UI64__(uint64_t x);
uint64_t __MODMUL_UI64__(uint64_t a, uint64_t b, uint64_t mod);
uint64_t __MODEXP_UI64__(uint64_t base, uint64_t exp, uint64_t mod);


//* --------------------------------------------------------------------------------------- *//
//*                                GENERAL MATHEMATICAL UTILITIES                           *//
//* --------------------------------------------------------------------------------------- *//
uint8_t __SAFE_EXP__(uint64_t base, uint64_t exp);
uint8_t __IS_2POW__(uint64_t x);
uint8_t __CLZ_UI64__(uint64_t x);
uint8_t __CTZ_UI64__(uint64_t x);
uint64_t __BSWAP_UI64__(uint64_t x);
uint8_t __PCNT_UI64__(uint64_t x);


//* --------------------------------------------------------------------------------------- *//
//*                                     FAST MEMORY UTILITIES                               *//
//* --------------------------------------------------------------------------------------- *//
// FAST MEMORY UTILITIES (UTILIZE SIMD EXTENSIVELY)
void __libdnml_MEMSET_FAST__(void *buf, uint64_t val, size_t len);
void __libdnml_MEMCPY_FAST__(void *buf, const void *src, size_t len);
void __libdnml_MEMWIPE__(void *buf, size_t len);


//* --------------------------------------------------------------------------------------- *//
//*                                HARDWARE INTERACTION UTILITIES                           *//
//* --------------------------------------------------------------------------------------- *//
/* Security-Extension Hardware Functionalities */
int __CPU_DBRG_SEED__(void *buf, size_t len, int retry_max, bool crypt, size_t *written);
int __CPU_TRNG_SEED__(void *buf, size_t len, int retry_max, bool crypt, size_t *written);
/* Hardware-Interactive Functionalities */
void __CPU_FULL_HALT__(void);
void __CPU_SHALLOW_HALT__(void);



//* --------------------------------------------------------------------------------------- *//
//*                                  CRYPTOGRAPHICAL OPERATIONS                             *//
//* --------------------------------------------------------------------------------------- *//
void __GET_ENTROPY_FAST(void* buf, size_t len);
void __GET_ENTROPY_STD(void *buf, size_t len);
int __GET_ENTROPY_PQC(void *buf, size_t len);


#ifdef __cplusplus
}
#endif

#endif
