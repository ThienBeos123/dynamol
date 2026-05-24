#ifndef DNML_CONV_MACROS
#define DNML_CONV_MACROS



#include <debug_util.h>
#include <dnml_status.h>
#include <libdnml_types.h>
#include <dnml_sys/sys.h>
#include <_libdnml_mem/_ctx.h>
#include <_libdnml_mem/arena.h>
#include <_libdnml_config/numeric_config.h>


#define BIGINT_TEMP(name, limb_count, ctx, err_check, end_stat) \
    limb_t *name##_limbs = scratch_alloc(&(ctx), (limb_count) * U64_BYTES, (&(err_check))); \
    mod_endstat((end_stat), (err_check)); \
    DNML_TEST_ASSERT( \
        !((end_stat) == DNML_ARENA_ALLOC_OVERFLOW),  \
        "Insufficient Scratch Allocation Capaicty (-Earena_cap_overflow)", \
        { scratch_clear(&(ctx)); scratch_destruct(&(ctx)); } \
    ); \
    bigInt name = {.limbs = name##_limbs, .sign = 1, .n = 0, .cap = (limb_count)};




#endif