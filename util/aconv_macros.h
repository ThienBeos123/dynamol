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



#ifndef DNML_CONV_MACROS
#define DNML_CONV_MACROS



#include <debug_util.h>
#include <dnml_status.h>
#include <libdnml_types.h>
#include <dnml_sys/sys.h>
#include <_libdnml_mem/_ctx.h>
#include <_libdnml_mem/arena.h>
#include <_libdnml_config/numeric_config.h>

#ifdef __cplusplus
extern "C" {
#endif


#define BIGINT_TEMP(name, limb_count, ctx, ctx_mark, err_check, err, ret_val) \
    limb_t *name##_limbs = scratch_alloc(&(ctx), (limb_count), (&(err_check))); \
    if ((err_check) != (DARENA_SUCCESS)) { scratch_rewind(&(ctx), (ctx_mark)); *err = DARENA_OVERFLOW; return ret_val; } \
    bigInt name = {.limbs = name##_limbs, .sign = 1, .n = 0, .cap = (limb_count)};

#define SCRATCH_OVF(err_check, ctx, mark, err, ret_val) if ((err_check) == DARENA_OVERFLOW) { scratch_rewind(&(ctx), (mark)); (*(err)) = DARENA_OVERFLOW; return ret_val;  }

#ifdef __cplusplus
}
#endif


#endif
