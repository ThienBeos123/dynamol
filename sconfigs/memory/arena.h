#ifndef __DNML_ARENA_H__
#define __DNML_ARENA_H__

#include <include.h>
#include <system/sys.h>
#include <stdalign.h>

//* ============= Declarations =============
typedef struct dnml_arena {
  uint64_t *base;
  size_t cap;
  size_t offset;
} dnml_arena;

//* ============= FUNCTIONALITIES ============= *//
static inline void init_arena(dnml_arena *a, size_t init_cap) {
  if (a->base)
    return;
  a->base = malloc(init_cap);
  if (!a->base)
    abort();
  a->cap = init_cap;
  a->offset = 0;
}
static inline void arena_destruct(dnml_arena *a) {
  if (!a->base)
    return;
  a->offset = 0;
  free(a->base);
  a->base = NULL;
  a->cap = 0;
  a->offset = 0;
}
static inline size_t arena_grow(dnml_arena *a, size_t min_cap) {
  if (a->cap >= min_cap)
    return a->cap;
  size_t new_cap = (a->cap) ? a->cap : 1;
  while (new_cap < min_cap)
    new_cap *= 2;
  uint64_t *buf = realloc(a->base, new_cap);
  if (!buf)
    abort();
  a->base = buf;
  a->cap = new_cap;
  return new_cap;
}
static inline void *arena_alloc(dnml_arena *a, size_t space) {
  size_t new_offset = a->offset + space;
  if (new_offset > a->cap)
    return NULL; // Swiftly prevents further operation through segfault.
  void *ptr = a->base + new_offset;
  a->offset = new_offset;
  return ptr;
}
static inline void *arena_galloc(dnml_arena *a, size_t space) {
  size_t new_offset = a->offset + space;
  if (new_offset > a->cap)
    arena_grow(a, new_offset);
  void *ptr = a->base + new_offset;
  a->offset = new_offset;
  return ptr;
}
static inline size_t arena_mark(dnml_arena *a) { return a->offset; }
static inline void arena_reset(dnml_arena *a, size_t mark) {
  if (mark <= a->offset)
    a->offset = mark;
}

//* ================== ADAPTERS ===================== *//
static inline void *arena_alloc_adapter(void *state, size_t n) {
  return arena_alloc((dnml_arena *)state, n);
}
static inline size_t arena_mark_adapter(void *state) {
  return arena_mark((dnml_arena *)state);
}
static inline void arena_reset_adapter(void *state, size_t n) {
  arena_reset((dnml_arena *)state, n);
}

#endif
