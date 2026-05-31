#include "crint_algo_core.h"


local_thread dnml_crypt_arena ___DASI_ALGO_ARENA_;
dnml_crypt_arena* _USE_CARENA(void) {
    if (___DASI_ALGO_ARENA_.base == NULL) init_carena(&___DASI_ALGO_ARENA_, 32);
    return &___DASI_ALGO_ARENA_;
}
dnml_status _init_drypto_crint(void) { 
    test_assert(
        init_carena(&___DASI_ALGO_ARENA_, 32) != DNML_ALLOC_OOM, arena_oom,
        { carena_clear(&___DASI_ALGO_ARENA_); carena_destruct(&___DASI_ALGO_ARENA_); },
        DNML_ALLOC_OOM
    );
}