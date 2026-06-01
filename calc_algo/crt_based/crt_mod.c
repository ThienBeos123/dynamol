#include "crt_mod.h"


/* CRYPTINT ALGORITHMS */
dnml_status __CRINT_BARETT_REDC__(const crint *dend, const crint *mod, crint *rem) {}
dnml_status __CRINT_MONT_REDC__(crint *t, mont_ctx mredc_ctx, crint *rem) {}
dnml_status __CRINT_MOD_DISP__(
    const crint *dend, const crint *mod, 
    crint *rem, crint *tmp_quot
) {
    if (mod->n < BIGINT_SHORT) return __CRINT_SHORT_DIVISION__(dend, mod->limbs[0], &tmp_quot, rem);
    else if (mod->n < BIGINT_BARETT) return __CRINT_BARETT_REDC__(dend, mod, rem);
    else return __CRINT_NEWTON_RECP__(dend, mod, &tmp_quot, rem);
}