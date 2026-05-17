#ifndef __DNML_SCRATCH__
#define __DNML_SCRATCH__


#include <system/sys.h>
#include <stdint.h>
#include <stddef.h>
#include <stdalign.h>
#include <stdlib.h>

#define MAX_ALIGN alignof(max_align_t)
typedef struct {
    uint8_t *base;
    size_t cap;
    size_t offset;
} dnml_dratch;

static inline void init_dratch(dnml_dratch *a, size_t init_cap) {
    if (a->base) return;
    a->base = (uint8_t*)malloc(init_cap);
    if (!a->base) abort();
    a->cap      = init_cap;
    a->offset   = 0;
}
static inline void dratch_destruct(dnml_dratch *a) {
    if (!a->base) return;
    a->offset = 0;
    free(a->base);
    a->base     = NULL;
    a->cap      = 0;
    a->offset   = 0;
}
static inline size_t dratch_grow(dnml_dratch *a, size_t min_cap) {
    if (a->cap >= min_cap) return a->cap;
    size_t new_cap = (a->cap) ? a->cap : 1;
    while (new_cap < min_cap) new_cap *= 2;
    uint8_t *buf = (uint8_t*)realloc(a->base, new_cap);
    if (!buf) abort();
    a->base     = buf;
    a->cap      = new_cap;
    return new_cap;
}
static inline void *dratch_alloc(dnml_dratch *s, size_t size) {
    size_t ptr = (size_t)(s->base + s->offset);
    size_t aligned = (ptr + (MAX_ALIGN - 1)) & ~(MAX_ALIGN - 1);
    size_t new_off = (aligned - (size_t)s->base) + size;

    if (new_off > s->cap) return NULL;
    void *res = (void*)aligned;
    s->offset = new_off;
    return res;
}
static inline size_t dratch_mark(dnml_dratch *a) { return a->offset; }
static inline void dratch_reset(dnml_dratch *a, size_t mark) { if (mark <= a->offset) a->offset = mark; }



#endif