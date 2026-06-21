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
 *       - Always arena-allocating, and no heap-fallbacks, ever
 *       - Pure Miller-Rabin + Baillie-PSW dispatched (1 round)
 *       - Use only if you fully trust arena workspace pre-calculations
 *       - Suitable for: HPC benchmarks, controlled environments
 *
 *   1 = BALANCE:
 *       - Arena-allocating ---> Heap-fallback upon release-build incidence
 *       - Always Baillie-PSW + Miller-Rabin + low MR rounds (5)
 *       - Still extremely fast, but will spike performance upon incidence for stability
 *       - Used for release builds for user wanting a balance between performance and stability
 *       - Also used in development builds for API testing and error handling
 *
 *   2 = STABLE:
 *       - Always heap-allocating, even in release builds
 *       - Always Baillie-PSW + Miller-Rabin + high MR Rounds (15-20)
 *       - Much, much slower, trading performance for absolute stability
 *       - Cryptographical Builds + Debugging and Development of Algorithms and Public-facing functions
 *
 * ============================================================================ */
#ifndef _DNML_PERF_PROFILE
    #define _DNML_PERF_PROFILE 1  /* 0=MAX, 1=BALANCE, 2=STABLE */
#endif

/* ===================================================================================================
 * ALLOCATION STTRATEGIES: How lib-dnnl handles algorithm's temporary allocations & allocation errors
 * ===================================================================================================
  * _DNML_ALLOC_STRAT values:
 *
 *   0 = ARENA-ALLOC: Algorithms always allocate on the arena (Dynamol)
 *
 *   1 = BALANCE: 
 *       - Algorithms initially allocate on the arena
 *       - Fallback to the heap on algorithm workspace estimation incorrectness,
 *         and algorithms always allocate from the heap from then on
 *
 *   2 = STABLE: Algorithms are always heap-allocating (Dynamol)
 *
 * ============================================================================ */
#ifndef _DNML_ALLOC_STRAT
    // The value of _DNML_ALLOC_STRAT corresponds to _DNML_PERF_PROFILE
    #define _DNML_ALLOC_STRAT _DNML_PERF_PROFILE
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
    #if _DNML_PERF_PROFILE == 1
        #define _DNML_MR_ROUNDS_DYNAMOL  5 /* Scientific: 2^(-10) error */
    #elif _DNML_PERF_PROFILE == 2
        #define _DNML_MR_ROUNDS_DYNAMOL 15 /* Scientific: 2^(-30) error */
    #endif
#endif
#ifndef _DNML_MR_ROUNDS_DBEDDED
    #define _DNML_MR_ROUNDS_DBEDDED 5 /* Embedded: 2^(-10) error */
#endif
#ifndef _DNML_MR_ROUNDS_DRYPTO
    #if _DNML_PERF_PROFILE == 1
        #define _DNML_MR_ROUNDS_DRYPTO 20 /* Cryptography: 2^(-40) error */
    #elif _DNML_PERF_PROFILE == 2
        #define _DNML_MR_ROUNDS_DRYPTO 30 /* Cryptography: 2^(-60) error */
    #endif
#endif

#ifdef __cplusplus
}
#endif

#endif
