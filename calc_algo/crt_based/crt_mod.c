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



#include "crt_mod.h"
#include <debug_util.h>
#include "../../util/aconv_macros.h"


/* CRYPTINT ALGORITHMS */
dnml_status __CRINT_BARETT_REDC__(crint *dend, crint *mod, crint *rem) { return CRINT_SUCCESS; } 
dnml_status __CRINT_MONT_REDC__(crint *t, mont_ctx mredc_ctx, crint *rem) { return CRINT_SUCCESS; }
dnml_status __CRINT_MOD_DISP__(crint *dend, crint *mod, crint *rem, crint *tmp_quot) {
    if (mod->n < BIGINT_SHORT) return __CRINT_SHORT_DIVISION__(dend, mod->limbs[0], tmp_quot, rem);
    return __CRINT_BARETT_REDC__(dend, mod, rem);
}
