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

#ifndef ___DNML_SETTINGS_H___
#define ___DNML_SETTINGS_H___

#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * DEBUG MODE: Enable assertions and detailed error reporting
 * ============================================================================ */
#ifndef _DNML_DEBUG_MODE
    #define _DNML_DEBUG_MODE 1
#endif

#if _DNML_DEBUG_MODE
    #undef NDEBUG
#else
    #define NDEBUG
#endif

/* ============================================================================
 * PERFORMANCE PROFILE: Sets overall behavior (overrides individual settings)
 * ============================================================================
 * _DNML_PERF_PROFILE values:
 *
 *   0 = MAX PERFORMANCE:
 *       - No heap fallback on DARENA_OVERFLOW (returns error instead)
 *       - Pure Miller-Rabin for primality (fastest)
 *       - Minimum MR rounds per profile
 *       - Use only if you fully trust arena workspace pre-calculations
 *       - Suitable for: HPC benchmarks, controlled environments
 *
 *   1 = SECURE:
 *       - Heap fallback enabled
 *       - Always Baillie-PSW + high MR rounds
 *       - Slowest but highest certainty
 *       - Use for cryptography, formal verification, or when absolutely sure
 * ============================================================================ */
#ifndef _DNML_PERF_PROFILE
    #define _DNML_PERF_PROFILE 1  /* 0=MAX, 1=SECURE */
#endif

/* ============================================================================
 * HEAP FALLBACK: Allow heap allocation when arena pre-calc is insufficient
 * ============================================================================
 * If enabled and arena allocation fails (DARENA_OVERFLOW), the library
 * allocates temporaries on the heap instead, guaranteeing correctness at
 * the cost of performance. Release-build safety net against insufficient
 * workspace pre-calculations.
 *
 * Automatically disabled for _DNML_PERF_PROFILE = 0 (MAX PERFORMANCE).
 * Can be manually overridden here.
 * ============================================================================ */
#ifndef _DNML_HEAP_FALLBACK_ENABLED
    #if _DNML_PERF_PROFILE == 0
        #define _DNML_HEAP_FALLBACK_ENABLED 0
    #else
        #define _DNML_HEAP_FALLBACK_ENABLED 1
    #endif
#endif

/* ============================================================================
 * PRIMALITY TESTING: Strategy for composite/prime discrimination
 * ============================================================================
 * _DNML_PRIMALITY_STRATEGY values:
 *
 *   0 = ADAPTIVE (MAX default):
 *       - Inputs < 1000 bits: Baillie-PSW (deterministic-ish, very strong)
 *       - Inputs >= 1000 bits: Baillie-PSW + adaptive Miller-Rabin rounds
 *       - Good balance of speed and certainty across all input sizes
 *
 *   1 = BAILLIE_PSW_PLUS_MR (SECURE default):
 *       - Always Baillie-PSW + profile-dependent MR rounds
 *       - Higher certainty but slower
 * ============================================================================ */
#ifndef _DNML_PRIMALITY_STRATEGY
    #if _DNML_PERF_PROFILE == 0
        #define _DNML_PRIMALITY_STRATEGY 0  /* Pure MR */
    #elif _DNML_PERF_PROFILE == 1
        #define _DNML_PRIMALITY_STRATEGY 1  /* Adaptive */
    #endif
#endif

/* ============================================================================
 * MILLER-RABIN ROUNDS: Accuracy/speed tradeoff per application domain
 * ============================================================================
 * Number of Miller-Rabin rounds for primality testing.
 * Error probability after k rounds: 2^(-2k) in worst case.
 *
 * Domain profiles:
 *   - DYNAMOL (Scientific): Moderate confidence, speed-friendly
 *   - DBEDDED (Embedded): Resource-constrained, lower rounds
 *   - DRYPTO (Cryptography): High confidence required, more rounds
 *
 * Can be individually overridden at compile time.
 * ============================================================================ */
#ifndef _DNML_MR_ROUNDS_DYNAMOL
    #define _DNML_MR_ROUNDS_DYNAMOL  5   /* Scientific: 2^(-10) error */
#endif
#ifndef _DNML_MR_ROUNDS_DBEDDED
    #define _DNML_MR_ROUNDS_DBEDDED  5   /* Embedded: 2^(-10) error */
#endif
#ifndef _DNML_MR_ROUNDS_DRYPTO
    #define _DNML_MR_ROUNDS_DRYPTO   20  /* Cryptography: 2^(-40) error */
#endif

#ifdef __cplusplus
}
#endif

#endif