#ifndef __DNML_ARENA_H__
#define __DNML_ARENA_H__

#include "../include.h" // In /include, relative path for easier pathfind
#include "../dnml_sys/sys.h" // In /include, relative path for easier pathfind
#include "../dnml_status.h" // In /include, relative path for easier pathfind
#include <stdalign.h>

//* ============= Declarations =============
typedef struct dnml_arena {
    uint64_t *base; size_t cap;
    size_t offset; bool poisoined;
} dnml_arena;

//* ============= FUNCTIONALITIES ============= *//
static inline dnml_status init_arena(dnml_arena *a, size_t init_cap) {
    if (a->base != NULL) return;
    uint64_t* __BUFFER_P = malloc(init_cap);
    if (__BUFFER_P == NULL) {
        a->poisoined = true;
        return DNML_ALLOC_OOM;
    }
    a->base = __BUFFER_P;
    a->cap = init_cap;
    a->offset = 0;
    a->poisoined = false;
}
static inline void arena_destruct(dnml_arena *a) {
    if (!a->base) return;
    a->offset = 0; free(a->base);
    a->base = NULL; a->cap = 0; a->offset = 0;
}
static inline size_t arena_grow(dnml_arena *a, size_t min_cap, dnml_status *err) {
    if (a->cap >= min_cap) return a->cap;
    size_t new_cap = (a->cap) ? a->cap : 1;
    while (new_cap < min_cap) new_cap *= 2;
    uint64_t* __BUFFER_P = realloc(a->base, new_cap);
    if (__BUFFER_P == NULL) { 
        *err = DNML_ALLOC_OOM; 
        a->poisoined = true; 
        return 0;
    }
    a->base = __BUFFER_P; a->cap = new_cap;
    return new_cap;
}
static inline void* arena_alloc(dnml_arena *a, size_t space, dnml_status *err) {
    size_t new_offset = a->offset + space;
    if (new_offset > a->cap) { *err = DNML_ARENA_ALLOC_OVERFLOW; return NULL; }
    void *ptr = a->base + new_offset;
    a->offset = new_offset;
    return ptr;
}
static inline void* arena_galloc(dnml_arena *a, size_t space, dnml_status *err) {
    size_t new_offset = a->offset + space;
    if (new_offset > a->cap) arena_grow(a, new_offset, err);
    if (*err == DNML_ALLOC_OOM) return NULL;
    void *ptr = a->base + new_offset;
    a->offset = new_offset;
    return ptr;
}
static inline size_t arena_mark(dnml_arena *a) { return a->offset; }
static inline void arena_reset(dnml_arena *a, size_t mark) {
    if (mark <= a->offset)  a->offset = mark;
}

//* ================== ADAPTERS ===================== *//
static inline void *arena_alloc_adapter(void *state, size_t n, dnml_status *err) {
    return arena_alloc((dnml_arena *)state, n, err);
}
static inline size_t arena_mark_adapter(void *state) {
    return arena_mark((dnml_arena *)state);
}
static inline void arena_reset_adapter(void *state, size_t n) {
    arena_reset((dnml_arena *)state, n);
}

#endif
