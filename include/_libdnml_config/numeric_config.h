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



#ifndef ___DNML_NUM_CONFIG_H___
#define ___DNML_NUM_CONFIG_H___

#include <stdint.h>
#include <stddef.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif


#define U64_BYTES 8
#define U64_BITS 64
#define SIZE_T_BITS ((sizeof(size_t)) * CHAR_BIT)
#define I64_MAX_BIT_MASK UINT64_C(0x7FFFFFFFFFFFFFFF)
#define I64_MIN_BIT_MASK UINT64_C(0x8000000000000000)
#define SIGN_BIT_MASK UINT64_C(0x8000000000000000)

/* Precomputation */
#define log2_10 3.32192809489L
#define log2_16 4.00000000000L
#define log2_2  1.00000000000L
#define log2_8  3.00000000000L

//*========================================== Thresholds //*========================================== *//
/* Multiplication */
typedef enum MUL_THRESHOLDS {
    BIGINT_SCHOOLBOOK = 16, BIGINT_KARATSUBA = 96, BIGINT_TOOM_3 = 256,
    BIGINT_TOOM_4 = 512, BIGINT_TOOM_5 = 1024, BIGINT_TOOM_6p5 = 1536,
    BIGINT_TOOM_7p5 = 2048, BIGINT_TOOM_8p5 = 4096, BIGINT_SSA = 4096,
} MUL_THRESHOLDS;

/* Division + Euclidean Modulo */
typedef enum DIV_THRESHOLDS {
    BIGINT_SHORT = 1,           // n < 1        DIV + MOD
    BIGINT_KNUTH = 32,          // n < 32       DIV + MOD
    BIGINT_BURNIKEL = 128,      // n < 128      DIV
    BIGINT_BARETT = 128,        // n < 128            MOD
    BIGINT_NEWTON,              // n >= 128     DIV + MOD
} DIV_THRESHOLDS;

/* Power, Roots, & Miscallenous Algebraic Operations */
typedef enum ALG_THRESHOLDS {
    /* Power / Exponentiation */
    BIGINT_BINARY = 8,
    BIGINT_FIXED = 32,
    BIGINT_SLIDING,
    /* Square root / Nth Root */
    BIGINT_NAIVE = 1,
    BIGINT_NEWTON_RAPHSON
} ALG_THRESHOLDS ;

typedef enum {
    /* GCD - Greatest Common Divisor */
    BIGINT_EUCLID = 1,
    BIGINT_STEIN = 64,
    BIGINT_LEHMER = 256,
    BIGINT_SUBQ_GCD,

    /* Primality Testing */
    TRIAL_DIVISION = 207936, // Value, not limbs
    DETERMINISTIC_MR = 207936, // Value, not limbs
    MRABIN_ONLY = 256, // Miller-Rabin ONLY
    BPSW_ONLY = 257, // Baillie-PSW ONLY (This value is a placeholder, and is typically dispatched as an else)
    MIXED_MAIN = 1, // Baillie-PSW + 10-20 Miller-Rabin random-base rounds
} NUM_THEORY_THRESHOLDS;

/* Modular Arithmetic */
typedef enum {
    /* Modular Multiplication */
    BIGINT_CLASSICAL = 64,
    BIGINT_MONTGOMERY,
    /* Modular Exponentiation */
    BIGINT_MOD_BINARY = 1,
    BIGINT_MONT_BINARY = 2,
    BIGINT_MOD_FIXED = 8,
    BIGINT_MOD_SLIDING,
    /* Modular Inverse */
    BIGINT_XEUCLID = 1,
    BIGINT_BINARY_XGCD = 64,
    BIGINT_HALF_XGCD
} MOD_ARITHMETIC_THRESHOLDS;


#ifdef __cplusplus
}
#endif

#endif
