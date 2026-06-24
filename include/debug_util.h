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



#ifndef ___DNML_DEBUG_UTIL_H
#define ___DNML_DEBUG_UTIL_H


#include "_libdnml_config/settings.h"

#ifdef __cplusplus
extern "C" {
#endif


// CUSTOM DNML_ASSERT
#if _DNML_DEBUG_MODE
/* This version is design for quick debugging,
and is generally unsafe for production-use for the user */
#define DNML_TEST_ASSERT(condition, message, cleanup_code) do { \
    if (!(condition)) { \
        cleanup_code; \
        fprintf(stderr, "\033[0;31m- [FATAL] Assertion Failed: (%s)\033[0m\n", #condition); \
        fprintf(stderr, "\033[0;31m  Function %s(): %s\033[0m\n", __func__, message); \
        fprintf(stderr, "\033[0;31m  File: %s | Line: %d\033[0m\n", __FILE__, __LINE__); \
        exit(EXIT_FAILURE); \
    } \
} while (0)
#else
#define DNML_TEST_ASSERT(condition, message, cleanup_code)
#endif


// Pre-opreration evaluation asserts
#define test_assert(cond, err_msg, cleanup, ret_cleanup, err_code) do { \
    if (_DNML_DEBUG_MODE) { \
        DNML_TEST_ASSERT(cond, err_msg, cleanup); \
    } else { \
        if (!(cond)) { ret_cleanup; return err_code; } \
    } \
} while (0);
#define test_assert_mut(cond, err_msg, cleanup, ret_cleanup, err, err_code, retval) do { \
    if (_DNML_DEBUG_MODE) { \
        DNML_TEST_ASSERT(cond, err_msg, cleanup); \
    } else { \
        if (!(cond)) { \
            if (err != NULL) *err = err_code; \
            return retval; \
        } \
    } \
} while (0);
#define test_assert_pre(cond, err_msg, cleanup, ret_cleanup, err, err_code, baseout, base, retsize) do { \
    if (_DNML_DEBUG_MODE) { \
        DNML_TEST_ASSERT(cond, err_msg, cleanup); \
    } else { \
        if (!(cond)) { \
            if (err != NULL) *err = err_code; \
            (*baseout) = (base); \
            return retsize; \
        } \
    } \
} while (0);

// CONVENIENT MACROS
#define scan_eos(curr_char, stream, ret) do { \
    if (curr_char == (uint16_t)(EOF) || curr_char == '\n') { \
        if (ferror(stream)) return FILE_ERR_PARSE; \
        else return ret; \
    } \
} while (0);
#define crint_poison(x) do { \
    DNML_TEST_ASSERT((!x->poisoned), "cryptInt x is invalid/poisoined (-Ecryptin_poisoined)", {}); \
} while(0);


//? ======================= COMMON !TEST! ASSERT ERRORS CATALOG ====================== ?//
// Invariant Enforcement & UB Handling
#define calloc_null "Allocation Failure: calloc() returned NULL (-Ealloc_calloc_fail)" /* CALLOC returns NULL */
#define realloc_null "Allocation Failure: realloc() returned NULL (-Ealloc_realloc_fail)" /* REALLOC returns NULL */
#define input_null "Input Error: Mutated nput is impossible to access due to being a NULL pointer (-Einput_null)"
#define null_err "Parameter Error: Status/Error parameter-based returns is null (-Enull_err_param)"
#define inval_cap "Reserve Capacity Calculation/Assumptions incorrect (-Ereserve_incorrect)"
// String Format Handling
#define str_null "String Format Error: Input string pointer is null (-Estr_null)"
#define str_empty "String Format Error: Input string is empty (-Estr_empty)"
#define str_inval_base "String Format Error: Input base-parameter is invalid (-Estr_inval_base)"
#define stream_err "File Error: Input stream witnessed an inexplicable error (-Efile_ferror)"


#ifdef __cplusplus
}
#endif


#endif
