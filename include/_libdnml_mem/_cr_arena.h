#ifndef __DNML_CARENA_H__
#define __DNML_CARENA_H__

#include "../include.h" // In /include, relative path for easier pathfind
#include "../dnml_sys/sys.h" // In /include, relative path for easier pathfind
#include "../dnml_status.h" // In /include, relative path for easier pathfind
#include "../../intrinsics/intrinsics.h"
#include <stdalign.h>


#ifdef __cplusplus
extern "C" {
#endif


//* ============= Declarations =============
typedef struct dnml_crypt_arena {
    uint64_t *base; size_t cap;
    size_t offset; bool poisoined;
} dnml_crypt_arena;

//* ============= FUNCTIONALITIES ============= *//
static inline dnml_status init_carena(dnml_crypt_arena *a, size_t init_cap) {
    uint64_t oom_mask = UINT64_MAX;
    dnml_status ret_stat = DARENA_SUCCESS;
    size_t alloc_size = (init_cap) | !(init_cap);
    limb_t *__BUFFER_P = calloc(alloc_size, U64_BYTES);
    ret_stat = _lib_crt_select((_lib_crt_eq(__BUFFER_P, NULL) & _lib_crt_eq(a->base, NULL)), DNML_ALLOC_OOM, ret_stat);
    oom_mask = _lib_crt_select((_lib_crt_eq(__BUFFER_P, NULL) & _lib_crt_eq(a->base, NULL)), 0, oom_mask);

    uint8_t uninit = (_lib_crt_eq(a->base, NULL));
    a->base = _lib_crt_select(uninit, ((ptr_t)__BUFFER_P) & oom_mask, ((ptr_t)a->base));
    a->cap = _lib_crt_select(uninit, alloc_size & oom_mask, a->cap);
    a->offset = _lib_crt_select(uninit, 0, a->offset);
    a->poisoined = _lib_crt_select(uninit, false, a->poisoined);
    /* Post_operation Clearance */ // clang-format off
    oom_mask = 0; alloc_size = 0; __BUFFER_P = 0; uninit = 0; return ret_stat; // clang_format on
}
static inline void carena_destruct(dnml_crypt_arena *a) {
    if (a->base != NULL) free(a->base); // clang-format off
    a->offset = 0; a->base = NULL;
    a->cap = 0; a->poisoined = 0; // clang-format on
}
static inline dnml_status carena_grow(dnml_crypt_arena *a, size_t min_cap) {
    dnml_status ret_stat = DARENA_SUCCESS; uint64_t oom_mask = UINT64_MAX;
    ret_stat = _lib_crt_select(a->poisoined & (_lib_crt_eq(ret_stat, DARENA_SUCCESS)), (DARENA_POISON), (ret_stat));
    /* Main Resizing */ limb_t *operated; 
    size_t new_cap = !!(a->cap); while (new_cap < min_cap) new_cap *= 2;
    operated = _lib_crt_select(
        _lib_crt_eq(ret_stat, DARENA_POISON) | _lib_crt_eq(new_cap, a->cap), 
        ((ptr_t)(malloc(U64_BYTES))), ((ptr_t)a->base)
    );
    uint64_t* __BUFFER_P = (uint64_t*)realloc(operated, new_cap);
    ret_stat = _lib_crt_select(
        _lib_crt_eq(__BUFFER_P, NULL) & _lib_crt_neq(new_cap, a->cap) & 
        _lib_crt_eq(ret_stat, DARENA_SUCCESS), (DNML_ALLOC_OOM), (ret_stat)
    );
    oom_mask = (uint64_t)(-(int64_t)(!(
        _lib_crt_eq(__BUFFER_P, NULL) &
        _lib_crt_neq(new_cap, a->cap) & 
        _lib_crt_neq(ret_stat, DARENA_POISON)
    )));
    // DNML_ALLOC_OOM ---> Always assign new
    // DARENA_SUCCESS ---> ASSIGN NWE IF NEEDED (ACTUALLY NEW CAP)
    // DARENA_POISON ----> NEVER
    uint8_t change_cap = (
        (_lib_crt_eq(ret_stat, DNML_ALLOC_OOM) | 
        (_lib_crt_eq(ret_stat, DARENA_SUCCESS) &
         _lib_crt_eq(new_cap, a->cap)))
    );
    a->base = _lib_crt_select(change_cap, (ptr_t)(__BUFFER_P) & oom_mask, (ptr_t)a->base);
    a->cap = _lib_crt_select(change_cap, new_cap & oom_mask, a->cap);
    /* Post Operation */ // clang-format off
    if ((_lib_crt_eq(ret_stat, DARENA_POISON)) |
        _lib_crt_eq(new_cap, a->cap) &
        _lib_crt_neq((ptr_t)__BUFFER_P, NULL)
    ) free(__BUFFER_P); oom_mask = 0; operated = 0;
    new_cap = 0; __BUFFER_P = 0; return ret_stat; // clang_format on
}
static inline void* carena_alloc(dnml_crypt_arena *a, size_t space, dnml_status *err) {
    dnml_status ret_stat = DARENA_SUCCESS;
    size_t new_offset = a->offset + space;
    ret_stat = _lib_crt_select(new_offset > a->cap, DARENA_OVERFLOW, ret_stat);
    void *ptr = _lib_crt_select(_lib_crt_eq(ret_stat, DARENA_SUCCESS), (ptr_t)(a->base + new_offset), (ptr_t)NULL);
    a->offset = _lib_crt_select(_lib_crt_eq(ret_stat, DARENA_SUCCESS), new_offset, a->offset);
    if (_lib_crt_neq((ptr_t)err, NULL)) *err = ret_stat; return ptr;
}
static inline void* carena_galloc(dnml_crypt_arena *a, size_t space, dnml_status *err) {
    dnml_status ret_stat = DARENA_SUCCESS, reserve_stat;
    size_t new_offset = a->offset + space;
    reserve_stat = carena_grow(a, new_offset);
    ret_stat = _lib_crt_select(reserve_stat == DNML_ALLOC_OOM, DNML_ALLOC_OOM, ret_stat);
    void *ptr = _lib_crt_select(_lib_crt_eq(ret_stat, DARENA_SUCCESS), (ptr_t)(a->base + new_offset), (ptr_t)NULL);
    a->offset = _lib_crt_select(_lib_crt_eq(ret_stat, DARENA_SUCCESS), new_offset, a->offset);
    if (_lib_crt_neq((ptr_t)err, NULL)) *err = ret_stat; return ptr;
}
static inline void carena_clear(dnml_crypt_arena *a) { a->offset = 0; }
static inline size_t carena_mark(dnml_crypt_arena *a) { return a->offset; }
static inline void carena_reset(dnml_crypt_arena *a, size_t mark) {
    a->offset = _lib_crt_select(_lib_crt_leq(mark, a->offset), mark, a->offset);
}

//* ================== ADAPTERS ===================== *//
static inline void *carena_alloc_adapter(void *state, size_t n, dnml_status *err) {
    return carena_alloc((dnml_crypt_arena*)state, n, err);
}
static inline size_t carena_mark_adapter(void *state) {
    return carena_mark((dnml_crypt_arena*)state);
}
static inline void carena_reset_adapter(void *state, size_t n) {
    carena_reset((dnml_crypt_arena*)state, n);
}
static inline void carena_clear_adapter(void *state) {
    carena_clear((dnml_crypt_arena*)state);
}
static inline void arena_destruct_adapter(void *state) {
    carena_destruct((dnml_crypt_arena*)state);
}

#ifdef __cplusplus
}
#endif


#endif
