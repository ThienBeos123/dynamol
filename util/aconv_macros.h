#ifndef DNML_CONV_MACROS
#define DNML_CONV_MACROS



#include <libdnml_types.h>
#include <dnml_sys/sys.h>
#include <_libdnml_mem/_ctx.h>
#include <_libdnml_mem/arena.h>
#include <_libdnml_config/numeric_config.h>


#define BIGINT_TEMP(name, limb_count, ctx) \
    limb_t *name##_limbs = scratch_alloc(&(ctx), (limb_count)); \
    bigInt name = {.limbs = name##_limbs, .sign = 1, .n = 0, .cap = (limb_count)};




#endif