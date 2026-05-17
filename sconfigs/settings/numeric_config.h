#ifndef ___DNML_NUM_CONFIG_H___
#define ___DNML_NUM_CONFIG_H___

#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#define BYTES_IN_UINT64_T sizeof(uint64_t)
#define BITS_IN_UINT64_T ((sizeof(uint64_t)) * CHAR_BIT)
#define BITS_IN_SIZE_T ((sizeof(size_t)) * CHAR_BIT)
#define I64_MAX_BIT_MASK 0x7FFFFFFFFFFFFFFFULL
#define I64_MIN_BIT_MASK 0x8000000000000000ULL

/* Precomputation */
#define log2_10 3.322
#define log2_16 4
#define log2_2  1
#define log2_8  3

//*========================================== Thresholds //*========================================== *//
/* String Parsing */
#define DASI_NAIVE_PARSE            750 // digits <= 750
#define DASI_DC_PARSE            750 // digits > 750

/* Multiplication */
typedef enum {
    BIGINT_SCHOOLBOOK       = 24,    BIGINT_KARATSUBA        = 72,
    BIGINT_TOOM_3           = 144,   BIGINT_TOOM_4           = 288,  BIGINT_TOOM_5           = 512,
    BIGINT_TOOM_6p5         = 1024,  BIGINT_TOOM_7p5         = 1536, BIGINT_TOOM_8p5         = 3072,
    BIGINT_SSA,
} MUL_THRESHOLDS;

/* Division + Euclidean Modulo */
typedef enum {
    BIGINT_SHORT                = 1,   // n < 1        DIV + MOD
    BIGINT_KNUTH                = 64,  // n < 64       DIV + MOD
    BIGINT_BURNIKEL             = 512, // n < 512      DIV
    BIGINT_BARETT               = 512, // n < 512            MOD
    BIGINT_NEWTON,                     // n >= 512     DIV + MOD
} DIV_THRESHOLDS;

/* Power, Roots, & Miscallenous Algebraic Operations */
typedef enum {
    /* Power / Exponentiation */
    BIGINT_BINARY       = 8,
    BIGINT_FIXED        = 32,
    BIGINT_SLIDING,
    /* Square root / Nth Root */
    BIGINT_NAIVE            = 1,
    BIGINT_NEWTON_RAPHSON,
};

typedef enum {
    /* GCD - Greatest Common Divisor */
    BIGINT_EUCLID           = 1,
    BIGINT_STEIN            = 512,
    BIGINT_LEHMER           = 4096,
    BIGINT_HALF_GCD,

    /* Primality Testing */ 
    TRIAL_DIVISION          = 207936, // Value, not limbs
    DETERMINISTIC_MR        = 207936, // Value, not limbs
    BPSW_ONLY               = 128, // Baillie-PSW ONLY
    MIXED_MAIN                   , // Baillie-PSW + 10-20 Miller-Rabin random-base rounds
    ECPP                    = 0, // PROOF OF PRIMALITY ONLY
} NUM_THEORY_THRESHOLDS;

/* Modular Arithmetic */
typedef enum {
    /* Modular Multiplication */
    BIGINT_CLASSICAL    = 8,
    BIGINT_MONTGOMERY   = 8,
    /* Modular Exponentiation */
    BIGINT_MOD_BINARY   = 8,
    BIGINT_MONT_BINARY  = 512,
    BIGINT_MOD_FIXED    = 1536,
    BIGINT_MOD_SLIDING,
    //todo MODULAR SQUARES HERE
    /* Modular Inverse */
    BIGINT_XEUCLID          = 256,
    BIGINT_BINARY_XGCD      = 2048,
    BIGINT_HALF_XGCD,
} MOD_ARITHMETIC_THRESHOLDS;

#define MRROUNDS_DNML               5 // Dynamol - Scientific Calculation
#define MRROUNDS_DBEDDED            5 // dBedded - Embedded Development
#define MRROUNDS_DRYPTO             20 // Drypto - Cryptography

#define __bienable_mont__       1   
#define __bienable_newton__     1
#define __bienable_fft__        1
#define __bienable_ecpp__       1

#define __bicrypto_offset__     0

#endif

