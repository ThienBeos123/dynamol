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
    crint_poison(x); int8_t ret = 2;
    // 1st, 2nd check, testing metadata
    ret = (int8_t)_lib_crt_select(_lib_crt_gt(x->n, y->n), 1, ret);
    ret = (int8_t)_lib_crt_select(_lib_crt_lt(x->n, y->n), -1, ret);
    size_t upperbound = crtmax(x->n, y->n); uint64_t x_curr, y_curr;
    for (size_t i = upperbound; _lib_crt_gt(i, 0); --i) {
        x_curr = _lib_crt_select(_lib_crt_lt(i - 1, x->cap), x->limbs[i - 1], x->limbs[0]);
        y_curr = _lib_crt_select(_lib_crt_lt(i - 1, y->cap), y->limbs[i - 1], y->limbs[0]);
        x_curr = _lib_crt_select(_lib_crt_lt(i - 1, x->n), x_curr, 0);
        y_curr = _lib_crt_select(_lib_crt_lt(i - 1, y->n), y_curr, 0);
        /* Evaluating if x_curr > y_curr AND vice versa */
        int8_t limb_diff = 0;
        limb_diff = (int8_t)_lib_crt_select(_lib_crt_gt(x_curr, y_curr), 1, limb_diff);
        limb_diff = (int8_t)_lib_crt_select(_lib_crt_lt(x_curr, y_curr), -1, limb_diff);
        /* Mutating and modifiying ret based on limb_diff */
        int8_t next_value = (int8_t)_lib_crt_select(_lib_crt_neq(limb_diff, 0), limb_diff, ret);
        ret = (int8_t)_lib_crt_select(_lib_crt_eq(ret, 2), next_value, ret); // clang-format off
        limb_diff = 0; next_value = 0; // clang-format on
    }
    ret = (int8_t)_lib_crt_select(_lib_crt_eq(ret, 2), 0, ret); // clang-format off
    upperbound = 0; x_curr = 0; y_curr = 0; x = NULL; y = NULL; return ret; // clang-format on
}
crint __CRINT_ERRVAL__(void) { return (crint){ .limbs = NULL, .n = 1, .cap = 0, .sign = 0, .poisoned = true }; }



/* ----- Mathematical Tools ----- */
dnml_status __CRINT_INTERNAL_RLSHIFT__(crint *x, size_t len, size_t limb_cnt) { return CRINT_SUCCESS; }
dnml_status __CRINT_INTERNAL_LLSHIFT__(crint *x, size_t len, size_t limb_cnt) { return CRINT_SUCCESS; }
