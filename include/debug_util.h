#ifndef ___DNML_DEBUG_UTIL_H
#define ___DNML_DEBUG_UTIL_H


#include "_libdnml_config/settings.h"



// CUSTOM DNML_ASSERT
#if _DNML_DEBUG_MODE 
#define DNML_ASSERT(condition, message) do { \
    if (!(condition)) { \
        fprintf(stderr, "[FATAL] Assertion Failed: %s\n", message); \
        fprintf(stderr, "File: %s | Line: %d\n", __FILE__, __LINE__); \
        /* NASA Rule: Take explicit recovery action instead of just aborting */ \
        exit(EXIT_FAILURE); \
    } \
} while (0)
#else
#define DNML_ASSERT(condition, message)
#endif


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
        assert(!(cond)); \
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




#endif