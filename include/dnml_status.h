#ifndef __DNML_STAT_H__
#define __DNML_STAT_H__

#include <stdio.h>

typedef enum {
    BIGINT_SUCCESS,             // 0
    BIGINT_TRUNC_SUCCESS,       // 1
    BIGINT_NULL,                // 2
    BIGINT_ERR_INVAL,           // 3
    BIGINT_ERR_RANGE,           // 4
    BIGINT_ERR_DOMAIN,          // 5

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

    CRYPTINT_SUCCESS = 200,     // 200
    CRYPTINT_POISOINED,         // 201
    CRYPTINT_ERR_RANGE,         // 202

    FILE_ERR_PARSE = 300,       // 300
    FILE_INVAL,                 // 301
    FILE_ILLEGAL,               // 302
    DNML_ALLOC_OOM,             // 303
    DNML_NULL_EPARAM,           // 304
    DNML_INVAL_CAP_REQUEST,     // 305
    DNML_ARENA_ALLOC_OVERFLOW,  // 306
} dnml_status;

static inline void _print_dnml_status(dnml_status a, FILE *f) {
    switch (a) {
        case BIGINT_SUCCESS:        fputs("BIGINT_SUCCESS", f);     break;
        case BIGINT_TRUNC_SUCCESS:  fputs("BIGINT_TRUNC_SUCCESS", f); break;
        case BIGINT_NULL:           fputs("BIGINT_NULL", f);        break;
        case BIGINT_ERR_INVAL:      fputs("BIGINT_ERR_INVAL", f);   break;
        case BIGINT_ERR_RANGE:      fputs("BIGINT_ERR_RANGE", f);   break;
        case BIGINT_ERR_DOMAIN:     fputs("BIGINT_ERR_DOMAIN", f);  break;

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

        case CRYPTINT_SUCCESS:      fputs("CRYPTINT_SUCCESS", f); break;
        case CRYPTINT_POISOINED:    fputs("CRYPTINT_POISOINED", f); break;
        case CRYPTINT_ERR_RANGE:    fputs("CRYPTINT_ERR_RANGE", f); break;

        case FILE_ERR_PARSE:            fputs("FILE_ERR_PARSE", f); break;
        case FILE_INVAL:                fputs("FILE_INVAL", f); break;
        case FILE_ILLEGAL:              fputs("FILE_ILLEGAL", f); break;
        case DNML_ALLOC_OOM:            fputs("DNML_ALLOC_OOM", f); break;
        case DNML_NULL_EPARAM:          fputs("DNML_NULL_EPARAM", f); break;
        case DNML_INVAL_CAP_REQUEST:    fputs("DNML_INVAL_CAP_REQUEST", f); break;
    }
}


#endif