#ifndef _bi_macros_h
#define _bi_macros_h



#include <dnml_status.h>
#include <debug_util.h>
#include <libdnml_types.h>
#include <include.h>
#include <_libdnml_config/settings.h>
#include <_libdnml_mem/arena.h>
#include "../../util/util.h"
#include "../_dynamol_arena.h"
#include "../../util/aconv_macros.h"

#ifdef __cplusplus
extern "C" {
#endif


#define ___DASI_IO_BUFSIZE 512 // Internal stack-based I/O Buffer size


//? ======================= COMMON TEST ASSERT ERRORS CATALOG ====================== ?//
// Invariant Enforcement
#define bi_full_contract "BigInt violates the full invariant contract (-Ebigint_inval)"
#define bi_state_contract "BigInt violates contract's state invariants (-Ebigint_state)"
#define bi_storage_inval "BigInt violates contract's storage invariants (-EbigInt_store)"
#define bi_aliased_limbs "BigInts contain aliased limb pointers (-Ebigint_alias)"
// Arena error meesages
#define arena_ovf "Arena workspace sizing estimation incorrect, open an issue on GitHub (-Earena_ovf)"
#define arena_oom "Arena initliaizationf failed due to OOM (-Ealloc_arena_oom)"
#define alloc_oom "Heap-Allocation Failure - OOM (-Ealloc_oom)"
#define arena_poison_oom "Arena Re-allocation witnessed an OOM error (-Earena_poison)"




//? ======================= NORMAL ERROR HANDLING MACROS ====================== ?//
/* ---------------- Functional Macros ---------------- */
#define clear_arena do { _cleanup_dynamol(); } while (0);
#define heap_alloc_oom(echeck, free_list, free_cnt) do { \
    test_assert( \
        echeck != DNML_ALLOC_OOM, "Heap allocation failed due to OOM (-Ealloc_oom)", \
        { clear_arena; _FREE_ALL_BI__(free_list, free_cnt); }, \
        { _FREE_RET_BI__(free_list, free_cnt); }, DNML_ALLOC_OOM \
    ) \
} while(0);
#define darena_assert(echeck, free_list, free_cnt) do { \
    test_assert( \
        echeck != DARENA_POISON, "Arena posioned due to OOM upon growth (-Earena_poison)", \
        { clear_arena; _FREE_ALL_BI__(free_list, free_cnt); }, \
        { _FREE_RET_BI__(free_list, free_cnt); }, DARENA_OVERFLOW \
    ); \
    test_assert( \
        echeck != DARENA_OVERFLOW, "Workspace Size Estimation Incorrect (-Earena_overflow)", \
        { clear_arena; _FREE_ALL_BI__(free_list, free_cnt); }, \
        { _FREE_RET_BI__(free_list, free_cnt); }, DARENA_OVERFLOW \
    ); \
} while(0);
#define mag_ovf(echeck, arena_p, mark) if (echeck == DARENA_OVERFLOW) { arena_rewind(arena_p, mark); return DARENA_OVERFLOW; }
#define mag_oom(echeck, arena_p, mark) if (echeck == DNML_ALLOC_OOM) { arena_rewind(arena_p, mark); return DNML_ALLOC_OOM; }
#define mag_heap_oom(echeck, free_list, free_cnt) if (echeck == DNML_ALLOC_OOM) { _free_alloc_list(free_list, free_cnt); return DNML_ALLOC_OOM; }

/* ---------------- Mutative Macros ---------------- */
#define heap_alloc_oom_bi(echeck, err, free_list, free_cnt) do { \
    test_assert_mut( \
        echeck != DNML_ALLOC_OOM, "Heap allocation failed due to OOM (-Ealloc_oom)", \
        { clear_arena; _FREE_ALL_BI__(free_list, free_cnt); }, \
        { _FREE_RET_BI__(free_list, free_cnt); }, err, echeck, __BIGINT_ERROR_VALUE__() \
    ); \
} while(0);
#define heap_alloc_oom_mut(echeck, err, free_list, free_cnt, retval) do { \
    test_assert_mut( \
        echeck != DNML_ALLOC_OOM, "Heap allocation failed due to OOM (-Ealloc_oom)", \
        { clear_arena; _FREE_ALL_BI__(free_list, free_cnt); }, \
        { _FREE_RET_BI__(free_list, free_cnt); }, err, echeck, retval \
    ); \
} while(0);
#define darena_massert(echeck, err, free_list, free_cnt, retval) do { \
    test_assert_mut( \
        echeck == BIGINT_SUCCESS, "Workspace Size Estimation Incorrect (-Earena_overflow)", \
        { clear_arena; _FREE_ALL_BI__(free_list, free_cnt); }, \
        { _FREE_RET_BI__(free_list, free_cnt) }, err, DARENA_OVERFLOW, retval \
    ); \
} while(0);
#define darena_biassert(echeck, err, free_list, free_cnt) do { \
    test_assert_mut( \
        echeck == BIGINT_SUCCESS, "Workspace Size Estimation Incorrect (-Earena_overflow)", \
        { clear_arena; _FREE_ALL_BI__(free_list, free_cnt); }, \
        { _FREE_RET_BI__(free_list, free_cnt) }, err, DARENA_OVERFLOW, __BIGINT_ERROR_VALUE__(); \
    ); \
} while(0);
#define darena_vassert(echeck, err, free_list, free_cnt) do { \
    test_assert_mut( \
        echeck == BIGINT_SUCCESS, "Workspace Size Estimation Incorrect (-Earena_overflow)", \
        { clear_arena; _FREE_ALL_BI__(free_list, free_cnt); }, \
        { _FREE_RET_BI__(free_list, free_cnt) }, err, DARENA_OVERFLOW, \
    ); \
} while(0);

/* -------------------- General Macros -------------------- */
#define _tmp_heap(name, size, echeck, free_list, free_cnt) echeck = __BIGINT_INTERNAL_LINIT__(&(name), size); heap_alloc_oom(echeck, free_list, free_cnt); free_list[(free_cnt)++] = (list_bi){&(name),1};
#define _tmp_heap_mut(name, size, echeck, err, free_list, free_cnt) echeck = __BIGINT_INTERNAL_LINIT__(&(name), size); heap_alloc_oom_bi(echeck, err, free_list, free_cnt); free_list[(free_cnt)++] = (list_bi){&(name),1};
#define mut_gret(err, err_code, ret) do { \
    if ((err) != NULL) *(err) = err_code; return ret; \
} while(0)
#define mut_err(err, err_code) do { \
    if ((err) != NULL) *(err) = err_code; \
    return __BIGINT_ERROR_VALUE__(); \
} while(0)




//? ======================= ARENA ERROR HANDLING MACROS ====================== ?//
/* ----------- Functional Macros ----------- */
#define arena_overflow(echeck, free_list, free_cnt, arena_name, mark) do { \
    test_assert( \
        /* Static Analysis - Assert Parameters */ \
        (((echeck) != DARENA_OVERFLOW)), arena_ovf, \
        { _cleanup_dynamol(); _FREE_ALL_BI__(free_list, free_cnt); }, \
        { arena_rewind(arena_name, mark); _FREE_RET_BI__(free_list, free_cnt); }, DARENA_OVERFLOW \
    ) \
} while(0);

/* ------------- Mutative Macros ------------- */
#define arena_poison_mut(arena_name, err, free_list, free_cnt, retval) do { \
    test_assert_mut( \
        /* Static Analysis - Assert Parameters */ \
        (!((arena_name)->poisoined)), alloc_oom, \
        { _cleanup_dynamol(); _FREE_ALL_BI__(free_list, free_cnt); }, \
        { _FREE_RET_BI__(free_list, free_cnt); }, (err), DNML_ALLOC_OOM, retval \
    ) \
} while(0);
#define arena_poison_bi(arena_name, err, free_list, free_cnt) do { \
    test_assert_mut( \
        /* Static Analysis - Assert Parameters */ \
        (!((arena_name)->poisoined)), alloc_oom, \
        { _cleanup_dynamol(); _FREE_ALL_BI__(free_list, free_cnt); }, \
        { _FREE_RET_BI__(free_list, free_cnt); }, (err), DNML_ALLOC_OOM, __BIGINT_ERROR_VALUE__() \
    ) \
} while(0);
#define arena_ovf_bi(echeck, err, free_list, free_cnt, arena_name, mark) do { \
    test_assert_mut( \
        /* Static Analysis - Assert Parameters */ \
        (((echeck) != DARENA_OVERFLOW)), arena_ovf, \
        { _cleanup_dynamol(); _FREE_ALL_BI__(free_list, free_cnt); }, \
        { arena_rewind(arena_name, mark); _FREE_RET_BI__(free_list, free_cnt); }, \
        (err), DARENA_OVERFLOW, __BIGINT_ERROR_VALUE__() \
    ) \
} while(0);
#define arena_ovf_mut(echeck, err, free_list, free_cnt, retval) do { \
    test_assert_mut( \
        /* Static Analysis - Assert Parameters */ \
        (((echeck) != DARENA_OVERFLOW)), arena_ovf, \
        { _cleanup_dynamol(); _FREE_ALL_BI__(free_list, free_cnt); }, \
        { arena_rewind(arena_name, mark); _FREE_RET_BI__(free_list, free_cnt); }, \
        (err), DARENA_OVERFLOW, retval \
    ) \
} while(0);



#ifdef __cplusplus
}
#endif


#endif