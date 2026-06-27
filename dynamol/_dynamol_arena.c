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



#include "_dynamol_arena.h"
#define arena_oom "Arena initialization occured an Out-of-Memory error (-Ealloc_oom)"

local_thread dnml_arena ___DASI_NUMERIC_ARENA_;
dnml_arena* _USE_ARENA(void) {
    if (___DASI_NUMERIC_ARENA_.base == NULL && !___DASI_NUMERIC_ARENA_.poisoined) 
    init_arena(&___DASI_NUMERIC_ARENA_, 128); // Safety check for non-initialization cases
    return &___DASI_NUMERIC_ARENA_;
}

local_thread dnml_arena ___DASI_IO_ARENA_;
dnml_arena* _USE_IO_ARENA(void) {
    if (___DASI_IO_ARENA_.base == NULL && !___DASI_IO_ARENA_.poisoined)
    init_arena(&___DASI_IO_ARENA_, 128); // Safety check for non-initialization cases
    return &___DASI_IO_ARENA_;
}


dnml_status _init_dynamol_bigint(void) {
    test_assert( // Initializing Arena for Numerical Computations
        init_arena(&___DASI_NUMERIC_ARENA_, 128) != DNML_ALLOC_OOM, arena_oom, 
        { arena_clear(&___DASI_NUMERIC_ARENA_); }, { arena_clear(&___DASI_NUMERIC_ARENA_); }, DNML_ALLOC_OOM
    );
    test_assert( // Initializing Arena for I/O use cases
        init_arena(&___DASI_IO_ARENA_, 128) != DNML_ALLOC_OOM, arena_oom,
        { arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_); arena_clear(&___DASI_IO_ARENA_); },
        { arena_clear(&___DASI_IO_ARENA_); }, DNML_ALLOC_OOM
    );
    return DARENA_SUCCESS;
}
void _cleanup_dynamol(void) {
    arena_clear(&___DASI_NUMERIC_ARENA_); arena_destruct(&___DASI_NUMERIC_ARENA_);
    arena_clear(&___DASI_IO_ARENA_); arena_destruct(&___DASI_IO_ARENA_);
}
