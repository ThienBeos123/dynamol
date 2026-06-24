#ifndef _crint_macros_h
#define _crint_macros_h

#include <libdnml_types.h>
#include <include.h>
#include <debug_util.h>
#include <_libdnml_config/settings.h>
#include <dnml_status.h>
#include "../../util/crt_util.h"
#include "crint_algo_core.h"


#ifdef __cplusplus
extern "C" {
#endif



#define FAKE_BUF_CAP 128 /* Constant tweaked to specific use case */
//! ANY MANUL TWEAKS TO THIS CONSTANT MACRO MUST SATISFY THE FOLLOWING:
//  1. THE CONSTANT VALUE IS A POWER OF 2 (TO PREVENT DIVISION INACURRACY IN PARTS OF THE CODE)
//  2. THE CONSTANT VALUE MUST BE ABLE TO AT LEAST BE DIVISBLE BY 4 INTO A Z+ INTEGER


//? ============================= COMMON !TEST! ASSERT ERRORS CATALOG ============================= ?//
#define ci_full_contract "Invalid CryptInt (-Ecrypt_int_invalid)" /* FULL Contract Violation */
#define ci_store_inval "CryptInt invalid for storage (-Ecrypt_int_sinvalid)" /* Partial Contract Violation - Storage */
#define crint_poisoned "CryptInt Poisoned (-Ecrypt_int_invalid)" /* CryptInt Poisoined - Testing */

//? ================================= TEST ASSERT CONVENIENT MACROS ================================= ?//
// Functional Macros
#define pbv_crint_clear(x) do { x.limbs = 0; x.cap = 0; x.n = 0; x.sign = 0; x.poisoned = 0; } while(0);
#define preop_err(cond, err, err_code, cleanup) do { \
    if (cond) { \
        if (_lib_crt_neq((ptr_t)err, (ptr_t)NULL)) *err = err_code; \
        cleanup; return __CRINT_ERRVAL__(); \
    } \
} while(0);
#define tmp_cleanup(tmp, ret_stat, chosen_freed) do { \
    chosen_freed = (_lib_crt_eq((ret_stat), CRINT_SUCCESS)) ? NULL : (tmp).limbs; \
    free(chosen_freed); /* Safe nop even on chosen_freed = NULL since ANSI-C */ \
    (tmp).limbs = (_lib_crt_eq((ret_stat), CRINT_SUCCESS)) ? (tmp).limbs : 0; \
    CHOOSE_OPTION(((tmp).n), (_lib_crt_eq((ret_stat), CRINT_SUCCESS)), ((tmp).n), (0)); \
    CHOOSE_OPTION(((tmp).cap), (_lib_crt_eq((ret_stat), CRINT_SUCCESS)), ((tmp).cap), (0)); \
    CHOOSE_OPTION(((tmp).sign), (_lib_crt_eq((ret_stat), CRINT_SUCCESS)), ((tmp).sign), (0)); \
    CHOOSE_OPTION(((tmp).poisoned), (_lib_crt_eq((ret_stat), CRINT_SUCCESS)), ((tmp).poisoned), (0)); \
    chosen_freed = 0; \
} while(0);
#define mask_ret(ret, mask, ret_stat, chosen_freed) do { \
    chosen_freed = (_lib_crt_eq((ret_stat), CRINT_SUCCESS)) ? NULL : (ret)->limbs; \
    free(chosen_freed); /* Safe nop even on chosen_freed = NULL since ANSI-C */ \
    (ret)->limbs = (_lib_crt_eq((ret_stat), CRINT_SUCCESS)) ? (ret)->limbs : 0; \
    (ret)->n &= (mask); (ret)->cap &= (mask); (ret)->sign &= (mask); \
    (ret)->poisoned = (_lib_crt_eq((ret_stat), CRINT_POISON) | _lib_crt_eq((ret_stat), CRINT_ERR_DOMAIN)); \
} while(0);


// Mutative Macros
#define _pre_assert(x, cleanup) do { \
    DNML_TEST_ASSERT((_lib_crt_neq((ptr_t)(x), (ptr_t)(NULL))), input_null, {}); \
    DNML_TEST_ASSERT((!(x)->poisoned), crint_poisoned, {cleanup}); \
    DNML_TEST_ASSERT((crint_pvalidate((x))), ci_full_contract, {cleanup}); \
} while(0);
#define _mheap_alloc_oom(err) do { DNML_TEST_ASSERT((err != DNML_ALLOC_OOM), realloc_null, __crint_exit()); } while(0);



#ifdef __cplusplus
}
#endif


#endif