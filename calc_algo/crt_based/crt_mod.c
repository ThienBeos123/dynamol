#include "crt_mod.h"


/* CRYPTINT WORKSPACE SIZE */
size_t __CRINT_BARETT_WS__(size_t dend_size, size_t mod_size) {}
size_t __CRINT_MOD_WS__(size_t dend_size, size_t mod_size) {
    if (mod_size < BIGINT_SHORT) return 0;
    else if (mod_size < BIGINT_BARETT) return __CRINT_BARETT_WS__(dend_size, mod_size);
    else return __CRINT_NTT_WS__(dend_size, mod_size);
}


/* CRYPTINT ALGORITHMS */
drypto_stat __CRINT_BARETT_REDC__(const cryptInt *dend, const cryptInt *mod, cryptInt *rem, calc_ctx barett_ctx) {}
drypto_stat __CRINT_MONT_REDC__(cryptInt *t, mont_ctx mredc_ctx, cryptInt *rem, calc_ctx redc_ctx) {}
drypto_stat __CRINT_MOD_DISP__(
    const cryptInt *dend, const cryptInt *mod, 
    cryptInt *rem, cryptInt *tmp_quot, calc_ctx mod_ctx
) {
    if (mod->n < BIGINT_SHORT) return __CRINT_SHORT_DIVISION__(dend, mod->limbs[0], &tmp_quot, rem);
    else if (mod->n < BIGINT_BARETT) return __CRINT_BARETT_REDC__(dend, mod, rem, mod_ctx);
    else return __CRINT_NEWTON_RECP__(dend, mod, &tmp_quot, rem, mod_ctx);
}