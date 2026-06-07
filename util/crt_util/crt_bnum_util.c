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



#include "../crt_util.h"

/* ---------- State Altering Utilities ---------- */
dnml_status __CRINT_IFREE__(crint *x) {
    crint_poison(x); free(x->limbs); // clang-format off
    x->limbs = NULL; x->cap = 0; x->n = 0;
    x->poisoned = 0; x->sign = 0; x = 0; return CRINT_SUCCESS; // clang-format on
}
dnml_status __CRINT_INEW__(crint *x) {
    /* Actual operation */
    uint64_t oom_mask = UINT64_MAX;  dnml_status ret_stat = CRINT_SUCCESS; limb_t *__BUFFER_P = calloc(1, U64_BYTES);
    DNML_TEST_ASSERT(_lib_crt_neq((ptr_t)__BUFFER_P, (ptr_t)(NULL)), 
        "Allocation Failure: realloc() returned NULL (-Ealloc_realloc_fail)", 
        { __CRINT_IFREE__(x); }
    );
    CHOOSE_OPTION((ret_stat), (_lib_crt_eq((ptr_t)__BUFFER_P, (ptr_t)(NULL)) & _lib_crt_eq((ptr_t)x->limbs, (ptr_t)(NULL))), DNML_ALLOC_OOM, ret_stat);
    CHOOSE_OPTION((oom_mask), (_lib_crt_eq((ptr_t)__BUFFER_P, (ptr_t)(NULL)) & _lib_crt_eq((ptr_t)x->limbs, (ptr_t)(NULL))), 0, oom_mask);

    uint8_t uninit = _lib_crt_eq((ptr_t)x->limbs, (ptr_t)(NULL));
    x->limbs = (uninit) ? __BUFFER_P : x->limbs;
    CHOOSE_OPTION((x->cap), (uninit), (1 & oom_mask), (x->cap));
    CHOOSE_OPTION((x->n), (uninit), (0 & oom_mask), (x->n));
    CHOOSE_OPTION((x->sign), (uninit), (1 & oom_mask), (x->sign));
    CHOOSE_OPTION((x->poisoned), (uninit), (0 & oom_mask), (x->poisoned));
    /* Post operation Aggrestive Clearance */ // clang-format off
    oom_mask = 0; __BUFFER_P = 0; uninit = 0; x = 0; return ret_stat; // clang-format on
}
dnml_status __CRINT_INEWS__(crint *x, size_t n) {
    /* Actual Operation */
    uint64_t oom_mask = UINT64_MAX; dnml_status ret_stat = CRINT_SUCCESS; size_t salloc = (n) | (!(n));
    limb_t *__BUFFER_P = calloc(salloc, U64_BYTES);
    DNML_TEST_ASSERT(_lib_crt_neq((ptr_t)__BUFFER_P, (ptr_t)(NULL)), 
        "Allocation Failure: realloc() returned NULL (-Ealloc_realloc_fail)", 
        { __CRINT_IFREE__(x); }
    );
    CHOOSE_OPTION((ret_stat), (_lib_crt_eq((ptr_t)__BUFFER_P, (ptr_t)(NULL)) & _lib_crt_eq((ptr_t)x->limbs, (ptr_t)(NULL))), DNML_ALLOC_OOM, ret_stat);
    CHOOSE_OPTION((oom_mask), (_lib_crt_eq((ptr_t)__BUFFER_P, (ptr_t)(NULL)) & _lib_crt_eq((ptr_t)x->limbs, (ptr_t)(NULL))), 0, oom_mask);

    uint8_t uninit = (_lib_crt_eq((ptr_t)(x->limbs), (ptr_t)(NULL)));
    x->limbs = (uninit) ? __BUFFER_P : x->limbs;
    CHOOSE_OPTION((x->cap), (uninit), (salloc & oom_mask), (x->cap));
    CHOOSE_OPTION((x->n), (uninit), (0 & oom_mask), (x->n));
    CHOOSE_OPTION((x->sign), (uninit), (1 & oom_mask), (x->sign));
    CHOOSE_OPTION((x->poisoned), (uninit), (0 & oom_mask), (x->poisoned));
    /* Post-operation Aggrestive Clearance */ // clang-format off
    oom_mask = 0; salloc = 0; __BUFFER_P = 0; uninit = 0; x = 0; n = 0; return ret_stat; // clang-format on
}
dnml_status __CRINT_TRIM_LZ__(crint *x) {
    crint_poison(x); uint8_t found_msl = 0;
    for (size_t i = x->cap - 1; i != (size_t)-1; --i) {
        uint8_t behind_n = (i < x->n);
        uint8_t zero = !(x->limbs[i]);
        // Checks if i < n AND i == 0 AND msl has not been set
        x->n -= ((behind_n & zero) & (~found_msl));
        // Checks if i < n AND i != 0 AND msl has not been set
        found_msl |= (behind_n & (~zero)); // clang-format off
        behind_n = 0; zero = 0;
    } found_msl = 0; x = 0; return CRINT_SUCCESS; // clang-format on
}
int8_t __CRINT_INTERNAL_CMP__(crint *x, crint *y) {
    /* Basically a more bareboned version of __CRINT_MAGCMP__ */
    crint_poison(x); int8_t ret = 2, curr = 0;
    // Check 1: if (x->n > y->n) return 1;
    CHOOSE_OPTION((curr), (_lib_crt_gt(x->n, y->n)), (1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 2: if (x->n < y->n) return -1;
    CHOOSE_OPTION((curr), (_lib_crt_lt(x->n, y->n)), (-1), (0));
    CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    // Check 3: Sequential checks
    size_t upperbound = crtmax(x->cap, y->cap);
    uint64_t x_curr, y_curr;
    for (size_t i = upperbound - 1; _lib_crt_neq(i, -1); --i) {
        /*
        * Guarantees that each iteration always access an index to normalize timing
        * We universally chosen the first limb of each crint as a placeholder
        * with not semantic meaning
        */
        CHOOSE_OPTION((x_curr), (_lib_crt_lt(i, x->cap)), (x->limbs[i]), (x->limbs[0]));
        CHOOSE_OPTION((x_curr), (_lib_crt_lt(i, x->cap)), (y->limbs[i]), (y->limbs[0]));
        // Actually Getting the right value
        CHOOSE_OPTION((x_curr), (_lib_crt_lt(i, x->n)), (x_curr), (0));
        CHOOSE_OPTION((y_curr), (_lib_crt_lt(i, y->n)), (y_curr), (0));
        // Comparing values
        /*
        * If our x_curr == x->limbs[i], in which i > y->n,
        * then such case was already covered above in our pre-checks
        * due to the fact that _lib_crt_gt(x->n, y->n). This also works inversely for y
        */
       CHOOSE_OPTION((curr), (_lib_crt_gt(x_curr, y_curr)), (1), (0));
       CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
       CHOOSE_OPTION((curr), (_lib_crt_lt(x_curr, y_curr)), (-1), (0));
       CHOOSE_OPTION((ret), (!!(curr) & (_lib_crt_eq(ret, 2))), (curr), (ret));
    }
    // ALL CHECKS WEREN'T SATIFIES --> EQUAL
    CHOOSE_OPTION((ret), (_lib_crt_eq(ret, 2)), (0), (ret));
    /* Aggresive Memory Sanitization */ // clang-format off
    curr = 0; upperbound = 0; x_curr = 0;
    y_curr = 0; x = 0; y = 0; return ret; // clang-format on
}
crint __CRINT_ERRVAL__(void) { return (crint){ .limbs = NULL, .n = 1, .cap = 0, .sign = 0, .poisoned = true }; }



/* ----- Mathematical Tools ----- */
dnml_status __CRINT_INTERNAL_RLSHIFT__(crint *x, size_t len, size_t limb_cnt) { return CRINT_SUCCESS; }
dnml_status __CRINT_INTERNAL_LLSHIFT__(crint *x, size_t len, size_t limb_cnt) { return CRINT_SUCCESS; }
