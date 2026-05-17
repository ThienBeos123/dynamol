#ifndef DNML_CONV_MACROS
#define DNML_CONV_MACROS




#include <system/sys.h>
#include "../../adynamol/big_numbers/bigNums.h"
#include "../sconfigs/memory/_ctx.h"
#include "../sconfigs/memory/arena.h"
#include "../sconfigs/settings/numeric_config.h"


#define BIGINT_TEMP(name, limb_count, ctx) \
    limb_t *name##_limbs = scratch_alloc(&(ctx), (limb_count)); \
    bigInt name = {.limbs = name##_limbs, .sign = 1, .n = 0, .cap = (limb_count)};




#endif