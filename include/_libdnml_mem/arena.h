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



#ifndef __DNML_ARENA_H__
#define __DNML_ARENA_H__

#include "../include.h" // In /include, relative path for easier pathfind
#include "../dnml_sys/sys.h" // In /include, relative path for easier pathfind
#include "../dnml_status.h" // In /include, relative path for easier pathfind
#include <stdalign.h>


#ifdef __cplusplus
extern "C" {
#endif


//* ============= Declarations =============
typedef struct dnml_arena {
    uint64_t *base; size_t cap;
    size_t offset; bool poisoined;
} dnml_arena;

//* ============= FUNCTIONALITIES ============= *//
static inline dnml_status init_arena(dnml_arena *a, size_t init_cap) {
    if (a->base != NULL) return DARENA_SUCCESS;
    uint64_t* __BUFFER_P = (uint64_t*)malloc(init_cap);
    if (__BUFFER_P == NULL) { a->poisoined = true; return DNML_ALLOC_OOM; }
    a->base = __BUFFER_P; a->cap = init_cap;
    a->offset = 0; a->poisoined = false;
    return DARENA_SUCCESS;
}
static inline void arena_destruct(dnml_arena *a) {
    if (a->base != NULL) free(a->base);
    a->offset = 0; a->base = NULL;
    a->cap = 0; a->poisoined = 0;
}
static inline dnml_status arena_grow(dnml_arena *a, size_t min_cap) {
    if (a->poisoined) return DARENA_POISON;
    if (a->cap >= min_cap) return DARENA_SUCCESS;
    size_t new_cap = (a->cap) ? a->cap : 1;
    while (new_cap < min_cap) new_cap *= 2;
    uint64_t* __BUFFER_P = (uint64_t*)realloc(a->base, new_cap);
    if (__BUFFER_P == NULL) {  a->poisoined = true; return DNML_ALLOC_OOM; } 
    a->base = __BUFFER_P; a->cap = new_cap;
    return DARENA_SUCCESS;
}
static inline void* arena_alloc(dnml_arena *a, size_t space, dnml_status *err) {
    if (a->poisoined) { *err = DARENA_POISON; return NULL; }
    size_t new_offset = a->offset + space;
    if (new_offset > a->cap) { 
        if (err!= NULL) *err = DARENA_OVERFLOW; 
        return NULL; 
    }
    void *ptr = a->base + new_offset;
    a->offset = new_offset;
    if (err != NULL) *err = DARENA_SUCCESS; return ptr;
}
static inline void* arena_galloc(dnml_arena *a, size_t space, dnml_status *err) {
    if (a->poisoined) { *err = DARENA_POISON; return NULL; }
    size_t new_offset = a->offset + space;
    if (new_offset > a->cap) {
        if (arena_grow(a, new_offset) == DNML_ALLOC_OOM) {
            if (err != NULL) *err = DNML_ALLOC_OOM; 
            return NULL;
        }
    } void *ptr = a->base + new_offset;
    a->offset = new_offset;
    if (err != NULL) *err = DARENA_SUCCESS; return ptr;
}
static inline void arena_clear(dnml_arena *a) { a->offset = 0; }
static inline size_t arena_mark(dnml_arena *a) { return a->offset; }
static inline void arena_reset(dnml_arena *a, size_t mark) {
    if (mark <= a->offset)  a->offset = mark;
}

//* ================== ADAPTERS ===================== *//
static inline void *arena_alloc_adapter(void *state, size_t n, dnml_status *err) {
    return arena_alloc((dnml_arena*)state, n, err);
}
static inline size_t arena_mark_adapter(void *state) {
    return arena_mark((dnml_arena*)state);
}
static inline void arena_reset_adapter(void *state, size_t n) {
    arena_reset((dnml_arena*)state, n);
}
static inline void arena_clear_adapter(void *state) {
    arena_clear((dnml_arena*)state);
}
static inline void arena_destruct_adapter(void *state) {
    arena_destruct((dnml_arena*)state);
}

#ifdef __cplusplus
}
#endif


#endif
