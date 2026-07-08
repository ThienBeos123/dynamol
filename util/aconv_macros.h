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

/* Type Conveniences */
#define PCONST_BIGINT const bigInt *const
#define P_BIGINT bigInt *const


/* Heap Allocation Convenience - Mutative */
void _free_alloc_list(bigInt **alloc_list, uint8_t alloc_cnt);
#define BIHEAP_TEMP(name, size, echeck, err, early_list, early_cnt, alloc_list, alloc_cnt, ret_val) \
    bigInt name; echeck = __BIGINT_INTERNAL_LINIT__(&(name), (size)); \
    if ((echeck) != (DARENA_SUCCESS)) { \
        _free_alloc_list((early_list), (early_cnt)); \
        *(err) = DNML_ALLOC_OOM; return ret_val; \
    } \
    (early_list)[(early_cnt)++] = &(name); \
    (alloc_list)[(alloc_cnt)++] = &(name);
#define BIHEAP_RET(name, size, echeck, err, early_list, early_cnt, ret_val) \
    bigInt name; echeck = __BIGINT_INTERNAL_LINIT__(&(name), (size)); \
    if ((echeck) != (DARENA_SUCCESS)) { \
        _free_alloc_list((early_list), (early_cnt)); \
        *(err) = DNML_ALLOC_OOM; return ret_val; \
    } (early_list)[(early_cnt)++] = &(name);
#define HEAP_OOM(echeck, err, early_list, early_cnt, ret_val) if ((echeck) != (DARENA_SUCCESS)) { \
    _free_alloc_list((early_list), (early_cnt)); \
    *(err) = DNML_ALLOC_OOM; return ret_val; \
}


/* Heap Allocation Convenience - Functional */
#define BIHEAP_FTEMP(name, size, echeck, early_list, early_cnt, alloc_list, alloc_cnt) \
    bigInt name; echeck = __BIGINT_INTERNAL_LINIT__(&(name), (size)); \
    if ((echeck) != (DARENA_SUCCESS)) { _free_alloc_list((early_list), (early_cnt)); return DNML_ALLOC_OOM; } \
    (early_list)[(early_cnt)++] = &(name); \
    (alloc_list)[(alloc_cnt)++] = &(name);
#define BIHEAP_FRET(name, size, echeck, early_list, early_cnt) \
    bigInt name; echeck = __BIGINT_INTERNAL_LINIT__(&(name), (size)); \
    if ((echeck) != (DARENA_SUCCESS)) { _free_alloc_list((early_list), (early_cnt)); return DNML_ALLOC_OOM; } \
    (early_list)[(early_cnt)++] = &(name);
#define HEAP_FOOM(echeck, early_list, early_cnt) if ((echeck) != (DARENA_SUCCESS)) {  \
    _free_alloc_list((early_list), (early_cnt)); return DNML_ALLOC_OOM; \
}





/* Scratch/Arena Allocation Convenience - Mutative */
#define BIGINT_TEMP(name, limb_count, ctx, ctx_mark, err_check, err, ret_val) \
    limb_t *name##_limbs = scratch_alloc((ctx), (limb_count), (&(err_check))); \
    if ((err_check) != (DARENA_SUCCESS)) { scratch_rewind((ctx), (ctx_mark)); *err = DARENA_OVERFLOW; return ret_val; } \
    bigInt name = {.limbs = name##_limbs, .sign = 1, .n = 0, .cap = (limb_count)};
#define RAW_TEMP(name, limb_count, ctx, ctx_mark, echeck, err, retval) \
    limb_t *name = scratch_alloc((ctx), (limb_count), (&(echeck))); \
    if ((echeck) != (DARENA_SUCCESS)) { scratch_rewind((ctx), (ctx_mark)); *err = DARENA_OVERFLOW; return retval; }
#define SCRATCH_OVF(err_check, ctx, mark, err, ret_val) if ((err_check) == DARENA_OVERFLOW) { \
    scratch_rewind((ctx), (mark)); (*(err)) = DARENA_OVERFLOW; return ret_val; \
}


/* Scratch/Arena Allocation Convenience - Functional */
#define BIGINT_FTEMP(name, limb_count, ctx, ctx_mark, echeck) \
    limb_t *name##_limbs = scratch_alloc((ctx), (limb_count), (&(echeck))); \
    if ((echeck) != (DARENA_SUCCESS)) { scratch_rewind((ctx), (ctx_mark)); return echeck; } \
    bigInt name = {.limbs = name##_limbs, .sign = 1, .n = 0, .cap = (limb_count)};
#define RAW_FTEMP(name, limb_count, ctx, ctx_mark, echeck) \
    limb_t *name = scratch_alloc((ctx), (limb_count), (&(echeck))); \
    if ((echeck) != (DARENA_SUCCESS)) { scratch_rewind((ctx), (ctx_mark)); return echeck; }
#define SCRATCH_FOVF(echeck, ctx, mark) if ((echeck) == DARENA_OVERFLOW) { \
    scratch_rewind((ctx), (mark)); return echeck; \
}


#ifdef __cplusplus
}
#endif


#endif
