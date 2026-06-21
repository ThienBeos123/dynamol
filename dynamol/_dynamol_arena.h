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



#ifndef dynamol_arena_h
#define dynamol_arena_h


#include <dnml_status.h>
#include <debug_util.h>
#include <dnml_sys/sys.h>
#include <_libdnml_mem/arena.h>

/* Static Analysis Messages */
#define arena_ovf "Arena workspace sizing estimation incorrect, open an issue on GitHub (-Earena_ovf)"
#define arena_oom "Arena initliaizationf failed due to OOM (-Ealloc_arena_oom)"
#define alloc_oom "Heap-Allocation Failure - OOM (-Ealloc_oom)"
#define arena_poison_oom "Arena Re-allocation witnessed an OOM error (-Earena_poison)"

/* Arena & Functions Declarations */
extern local_thread dnml_arena ___DASI_NUMERIC_ARENA_;
extern local_thread dnml_arena ___DASI_IO_ARENA_;
dnml_arena* _USE_ARENA(void);
dnml_arena* _USE_IO_ARENA(void);
dnml_status _init_dynamol_bigint(void);
void _cleanup_dynamol(void);

/* Functional Macros */
#define arena_poisoined(arena_name) do { \
    test_assert( \
        /* Static Analysis - Assert Parameters */ \
        (!((arena_name)->poisoined)), alloc_oom, { _cleanup_dynamol(); }, \
        DNML_ALLOC_OOM /* Error Returns Parameters */ \
    ) \
} while(0);
#define arena_alloc_oom(err_check, arena_name) do { \
    test_assert( \
        /* Static Analysis - Assert Parameters */ \
        (((err_check) != DNML_ALLOC_OOM)), alloc_oom, { _cleanup_dynamol(); }, \
        DNML_ALLOC_OOM /* Error Returns Parameters */ \
    ); \
} while(0);
#define arena_overflow(err_check, free_list, free_cnt) do { \
    test_assert( \
        /* Static Analysis - Assert Parameters */ \
        (((err_check) != DARENA_OVERFLOW)), arena_ovf, \
        { _cleanup_dynamol(); for (uint8_t i = 0; i < free_cnt; ++i) __BIGINT_INTERNAL_FREE__(free_list[i]); }, \
        DARENA_OVERFLOW \
    ) \
} while(0);

/* Mutative Macros */
#define arena_alloc_oom_mut(err_check, arena_name, err) do { \
    test_assert_mut( \
        (((err_check) != DNML_ALLOC_OOM)), alloc_oom, { _cleanup_dynamol(); }, \
        (err), DNML_ALLOC_OOM, __BIGINT_ERROR_VALUE__() /* Error Returns Parameters */ \
    ); \
} while(0);
#define arena_poison_mut(arena_name, err) do { \
    test_assert_mut( \
        /* Static Analysis - Assert Parameters */ \
        (!((arena_name)->poisoined)), alloc_oom, { _cleanup_dynamol(); }, \
        (err), DNML_ALLOC_OOM, __BIGINT_ERROR_VALUE__() /* Error Returns Parameters */ \
    ) \
} while(0);
#define arena_ovf_mut(err_check, err, free_list, free_cnt) do { \
    test_assert_mut( \
        /* Static Analysis - Assert Parameters */ \
        (((err_check) != DARENA_OVERFLOW)), arena_ovf, \
        { _cleanup_dynamol(); for (uint8_t i = 0; i < free_cnt; ++i) __BIGINT_INTERNAL_FREE__(free_list[i]); }, \
        (err), DARENA_OVERFLOW, __BIGINT_ERROR_VALUE__() /* Error Returns Parameters */ \
    ) \
} while(0);


#endif
