#ifndef __DNML_STAT_H__
#define __DNML_STAT_H__


#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BIGINT_SUCCESS,             // 0
    BIGINT_TRUNC_SUCCESS,       // 1
    BIGINT_NULL,                // 2
    BIGINT_ERR_INVAL,           // 3
    BIGINT_ERR_SINVAL,          // 4
    BIGINT_ERR_STORE_IN,        // 5
    BIGINT_ERR_RANGE,           // 6
    BIGINT_ERR_DOMAIN,          // 7
    BIGINT_ERR_ALIASED,         // 8

    STR_SUCCESS = 100,          // 100
    STR_TRUNC_SUCCESS,          // 101
    STR_NULL,                   // 102
    STR_EMPTY,                  // 103
    STR_INVALID_CAP,            // 104
    STR_INCOMPLETE,             // 105
    STR_INVALID_SIGN,           // 106
    STR_INVALID_BASE_PREFIX,    // 107
    STR_INVALID_BASE,           // 108
    STR_INVALID_DIGIT,          // 109

    CRINT_SUCCESS = 200,     // 200
    CRINT_POISON,            // 201
    CRINT_ERR_RANGE,         // 202
    CRINT_ERR_DOMAIN,        // 203
    CRINT_NULL,              // 204
    CRINT_ERR_INVAL,         // 205
    CRINT_ERR_SINVAL,        // 206

    DARENA_OVERFLOW = 300,  // 301
    DARENA_SUCCESS,         // 302
    DARENA_POISON,          // 303

    FILE_ERR_PARSE = 400,       // 400
    FILE_INVAL,                 // 401
    FILE_ILLEGAL,               // 402
    DNML_ALLOC_OOM,             // 403
    DNML_NULL_EPARAM,           // 404
    DNML_INVAL_CAP_REQUEST,     // 405
    DNML_LIB_INISUCCESS,        // 406
} dnml_status;

static inline void _print_dnml_status(dnml_status a, FILE *f) {
    switch (a) {
        case BIGINT_SUCCESS:        fputs("BIGINT_SUCCESS", f);     break;
        case BIGINT_TRUNC_SUCCESS:  fputs("BIGINT_TRUNC_SUCCESS", f); break;
        case BIGINT_NULL:           fputs("BIGINT_NULL", f);        break;
        case BIGINT_ERR_INVAL:      fputs("BIGINT_ERR_INVAL", f);   break;
        case BIGINT_ERR_SINVAL:     fputs("BIGINT_ERR_SINVAL", f);  break;
        case BIGINT_ERR_STORE_IN:   fputs("BIGINT_ERR_STORE_IN", f);break;
        case BIGINT_ERR_RANGE:      fputs("BIGINT_ERR_RANGE", f);   break;
        case BIGINT_ERR_DOMAIN:     fputs("BIGINT_ERR_DOMAIN", f);  break;
        case BIGINT_ERR_ALIASED:    fputs("BIGINT_ERR_ALIASED", f); break;

        case STR_SUCCESS:               fputs("STR_SUCCESS", f);            break;
        case STR_TRUNC_SUCCESS:         fputs("STR_TRUNC_SUCCESS", f);      break;
        case STR_NULL:                  fputs("STR_NULL", f);               break;
        case STR_EMPTY:                 fputs("STR_EMPTY", f);              break;
        case STR_INVALID_CAP:           fputs("STR_INVALID_CAP", f);        break;
        case STR_INCOMPLETE:            fputs("STR_INCOMPLETE", f);         break;
        case STR_INVALID_SIGN:          fputs("STR_INVALID_SIGN", f);       break;
        case STR_INVALID_BASE_PREFIX:   fputs("STR_INVALID_BASE_PREFIX", f); break;
        case STR_INVALID_BASE:          fputs("STR_INVALID_BASE", f);       break;
        case STR_INVALID_DIGIT:         fputs("STR_INVALID_DIGIT", f);      break;

        case CRINT_SUCCESS:     fputs("CRINT_SUCCESS", f); break;
        case CRINT_POISON:      fputs("CRINT_POISON", f); break;
        case CRINT_ERR_RANGE:   fputs("CRINT_ERR_RANGE", f); break;
        case CRINT_ERR_DOMAIN:  fputs("CRINT_ERR_DOMAIN", f); break;
        case CRINT_NULL:        fputs("CRINT_NULL", f); break;
        case CRINT_ERR_INVAL:   fputs("CRINT_ERR_INVAL", f); break;
        case CRINT_ERR_SINVAL:  fputs("CRINT_ERR_SINVAL", f); break;

        case FILE_ERR_PARSE:            fputs("FILE_ERR_PARSE", f); break;
        case FILE_INVAL:                fputs("FILE_INVAL", f); break;
        case FILE_ILLEGAL:              fputs("FILE_ILLEGAL", f); break;
        case DNML_ALLOC_OOM:            fputs("DNML_ALLOC_OOM", f); break;
        case DNML_NULL_EPARAM:          fputs("DNML_NULL_EPARAM", f); break;
        case DNML_INVAL_CAP_REQUEST:    fputs("DNML_INVAL_CAP_REQUEST", f); break;
        case DARENA_OVERFLOW:           fputs("DARENA_OVERFLOW", f); break;
        case DARENA_SUCCESS:            fputs("DARENA_SUCCESS", f); break;
        case DNML_LIB_INISUCCESS:       fputs("DNML_LIB_INISUCCESS", f); break;
    }
}


#ifdef __cplusplus
}
#endif


#endif