#ifndef DNML_CRYPT_INT_ALGO_CORE
#define DNML_CRYPT_INT_ALGO_CORE


#include <libdnml_types.h>
#include <include.h>
#include <char_tables.h>
#include <dnml_sys/sys.h>
#include <debug_util.h>
#include <_libdnml_config/settings.h>
#include <_libdnml_config/numeric_config.h>
#include <_libdnml_mem/_ctx.h>
#include <_libdnml_mem/_cr_arena.h>
#include <dnml_status.h>


// ARENA - ALGORITHMICLY ESSENTIAL
extern local_thread dnml_crypt_arena ___DASI_ALGO_ARENA_;
dnml_crypt_arena* _USE_CARENA(void);
dnml_status _init_drypto_crint(void);
#define arena_oom "CRITICAL ERROR: Arena initliaizationf failed due to OOM (-Ealloc_arena_oom)"


// General Arena-related Macros
#define __crint_exit() do { \
    if (!___DASI_ALGO_ARENA_.poisoined && ___DASI_ALGO_ARENA_.base != NULL) { \
        __libdnml_smemwipe_u64( \
            ___DASI_ALGO_ARENA_.base, ___DASI_ALGO_ARENA_.cap, \
            0, ___DASI_ALGO_ARENA_.cap - 1, false \
        ); \
    } arena_clear(&___DASI_ALGO_ARENA_); arena_destruct(&___DASI_ALGO_ARENA_);  \
} while(0);



//* ========================================= MAGNITUDE MATHEMATICA ========================================== *//
/* ------------------- MAGNITUDED ARITHMETIC ------------------- */
void __CRINT_MAGADD__(crint *res, const crint *a, const crint *b, dnml_status *err);
void __CRINT_MAGSUB__(crint *res, const crint *a, const crint *b, dnml_status *err);
void __CRINT_MAGMUL___(crint *res, const crint *a, const crint *b, dnml_status *err);
/* MAGNITUDED CORE NUMBER-THEORETIC */
/* MAGNITUDED MODULAR-ARITHMETIC */
/* MAGNITUDED ALGEBRAIC OPERATIONS */



#endif