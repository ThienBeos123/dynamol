#include "_dynamol_arena.h"

local_thread dnml_arena ___DASI_NUMERIC_ARENA_;
dnml_arena* _USE_ARENA(void) {
    if (___DASI_NUMERIC_ARENA_.base == NULL && !___DASI_NUMERIC_ARENA_.poisoined) 
    init_arena(&___DASI_NUMERIC_ARENA_, 16); // Safety check for non-initialization cases
    return &___DASI_NUMERIC_ARENA_;
}

local_thread dnml_arena ___DASI_LOWLVL_ARENA_;
dnml_arena* _USE_LOW_ARENA(void) {
    if (___DASI_LOWLVL_ARENA_.base == NULL && !___DASI_LOWLVL_ARENA_.poisoined)
    init_arena(&___DASI_LOWLVL_ARENA_, 16); // Safety check for non-initialization cases
    return &___DASI_LOWLVL_ARENA_;
}

local_thread dnml_arena ___DASI_IO_ARENA_;
dnml_arena* _USE_IO_ARENA(void) {
    if (___DASI_IO_ARENA_.base == NULL && !___DASI_IO_ARENA_.poisoined)
    init_arena(&___DASI_IO_ARENA_, 16); // Safety check for non-initialization cases
    return &___DASI_IO_ARENA_;
}


dnml_status _init_dynamol_bigint(void) {
    test_assert( // Initializing Arena for Numerical Computations
        init_arena(&___DASI_NUMERIC_ARENA_, 16) != DNML_ALLOC_OOM, arena_oom, {
        arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_);
    }, DNML_ALLOC_OOM);
    test_assert( // Initializing Low-level Arena for Algorithms
        init_arena(&___DASI_LOWLVL_ARENA_, 16) != DNML_ALLOC_OOM, arena_oom, {
        arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_);
        arena_clear(&___DASI_LOWLVL_ARENA_); arena_destruct(&___DASI_LOWLVL_ARENA_);
    }, DNML_ALLOC_OOM);
    test_assert( // Initializing Arena for I/O use cases
        init_arena(&___DASI_IO_ARENA_, 16) != DNML_ALLOC_OOM, arena_oom, {
        arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_);                                           
        arena_clear(&___DASI_LOWLVL_ARENA_); arena_destruct(&___DASI_LOWLVL_ARENA_);
        arena_clear(&___DASI_IO_ARENA_); arena_destruct(&___DASI_IO_ARENA_);
    }, DNML_ALLOC_OOM);
    return DARENA_SUCCESS;
}
void _cleanup_dynamol(void) {
    arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_);                                           
    arena_clear(&___DASI_LOWLVL_ARENA_); arena_destruct(&___DASI_LOWLVL_ARENA_);
    arena_clear(&___DASI_IO_ARENA_); arena_destruct(&___DASI_IO_ARENA_);
}
