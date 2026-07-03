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



#ifndef __DNML_CTX__
#define __DNML_CTX__

#include "../dnml_sys/sys.h" // In /include, relative path for easier pathfind
#include "../libdnml_types.h" // In /include, relative path for easier pathfind
#include "../include.h" // In /include, relative path for easier pathfind
#include "../dnml_status.h" // In /include, relative path for easier pathfind
#include <stdalign.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef struct mont_ctx {
const bigInt *n;
    limb_t nprime;
    const bigInt *r2;
    size_t k;
} mont_ctx;

typedef struct calc_ctx {
    void *(*alloc)(void *state, size_t size, dnml_status *err);
    size_t (*mark)(void *state);
    void (*rewind)(void *state, size_t mark);
    void (*clear)(void *state); /* Mostly for debugging */
    void (*destruct)(void *state); /* Mostly for debugging */
    void *state;
} calc_ctx;
static inline void *scratch_alloc(calc_ctx *ctx, size_t n, dnml_status *err) {
    return ctx->alloc(ctx->state, n, err);
}
static inline size_t scratch_mark(calc_ctx *ctx) {
    return ctx->mark(ctx->state);
}
static inline void scratch_rewind(calc_ctx *ctx, size_t mark) {
    ctx->rewind(ctx->state, mark);
}

static inline void scratch_clear(calc_ctx *ctx) {
    ctx->clear(ctx->state);
}

static inline void scratch_destruct(calc_ctx *ctx) {
    ctx->destruct(ctx->state);
}


#ifdef __cplusplus
}
#endif

#endif
