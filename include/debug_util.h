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
        fprintf(stderr, "  - [FATAL] Assertion Failed: (%s)", #condition); \
        fprintf(stderr, "    Function %s(): %s\n", __func__, message); \
        fprintf(stderr, "     File: %s | Line: %d\n", __FILE__, __LINE__); \
        exit(EXIT_FAILURE); \
    } \
} while (0)
#else
#define DNML_TEST_ASSERT(condition, message)
#endif

/* 
This version is targeted as the production-ready assertion, 
allowing for detailed fail reason to the user + 
cryptographically-ready cleanup code insertions 

! PLEASE ALSO CLEARLY DOCUMENT THIS FUNCTION MACRO, AS IT
! USES POTENTIALLY DANGEROUS AND BUG-RIDDEN COD INSERTION
*/
#define DNML_ASSERT(condition, message, cleanup_code) do { \
    if (!(condition)) { \
        do { cleanup_code } while (0); \
        fprintf(stderr, "  - [FATAL] Assertion Failed: (%s)", #condition); \
        fprintf(stderr, "    Function %s(): %s\n", __func__, message); \
        fprintf(stderr, "     File: %s | Line: %d\n", __FILE__, __LINE__); \
        exit(EXIT_FAILURE); \
    } \
} while (0)


// Pre-opreration evaluation asserts
#define test_assert(cond, err_msg, cleanup, err_code) do { \
    if (_DNML_DEBUG_MODE) { \
        DNML_TEST_ASSERT(cond, err_msg, cleanup); \
    } else { \
        if (cond) return err_code; \
    } \
} while (0);
#define test_assert_mut(cond, err_msg, cleanup, err, err_code, retval) do { \
    if (_DNML_DEBUG_MODE) { \
        DNML_TEST_ASSERT(cond, err_msg, cleanup); \
    } else { \
        if (cond) { \
            if (err != NULL) *err = err_code; \
            return retval; \
        } \
    } \
} while (0);
#define test_assert_pre(cond, err_msg, cleanup, err, err_code, baseout, base, retsize ) do { \
    if (_DNML_DEBUG_MODE) { \
        DNML_TEST_ASSERT(cond, err_msg, cleanup); \
    } else { \
        if (cond) { \
            if (err != NULL) *err = err_code; \
            (*baseout) = (base); \
            return retsize; \
        } \
    } \
} while (0);

// CONVENIENT MACROS
#define scan_eof(curr_char, stream, ret) do { \
    if (curr_char == EOF) { \
        if (ferror(stream)) return FILE_ERR_PARSE; \
        else return ret; \
    } \
} while (0);
#define crint_poison(x) do { \
    DNML_TEST_ASSERT((!x->poisoned), "cryptInt x is invalid/poisoined (-Ecryptin_poisoined)", {}); \
} while(0);


#ifdef __cplusplus
}
#endif


#endif