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



#include "crt_div.h"
#include <debug_util.h>
#include "../../util/aconv_macros.h"

/* CRYPTINT ALGORITHMS */
dnml_status __CRINT_SHORT_DIVISION__(crint *a, uint64_t b, crint *quot, crint *rem) {
    // Static Analysis
    crint_poison(a); crint_poison(quot);
    crint_poison(rem); DNML_TEST_ASSERT((b),
        "Mathematical Undefinindness: Division by 0 (-Ediv_by_zero)",
        { __CRINT_IFREE__(rem); __CRINT_IFREE__(quot); __CRINT_IFREE__(a); }
    );
    DNML_TEST_ASSERT((quot->cap >= a->n),
        "Insufficient Quotient Capacity: Capacity unsatisfactory for a / b "
        "(-Eshort_div_insufficient_cap)", {}
    );
    // Main Algorithms
    uint64_t remainder = 0; uint8_t ovf_test;
    for (size_t i = a->n - 1; _lib_crt_neq(i, -1); --i) {
        quot->limbs[i] = __CRT_DIV_U128__(remainder, a->limbs[i], b, &remainder, &ovf_test);
        DNML_TEST_ASSERT(ovf_test, "CRITICIAL DEBUG ERROR: Division quotient's overflowed", {});
    }
    __CRINT_TRIM_LZ__(rem); CHOOSE_OPTION((quot->sign), (!(quot->n)), (1), (quot->sign));
    rem->limbs[0] = remainder; rem->n = !!(rem); rem->sign = 1; return CRINT_SUCCESS;
}
dnml_status __CRINT_NEWTON_RECP__(crint *dend, crint *div, crint *quot, crint *rem) { return CRINT_SUCCESS; }
dnml_status __CRINT_DIV_DISP__(crint *a, crint *b, crint *quot, crint *rem) {
    if (b->n < BIGINT_SHORT) return __CRINT_SHORT_DIVISION__(a, b->limbs[0], quot, rem);
    else return __CRINT_NEWTON_RECP__(a, b, quot, rem);
}
