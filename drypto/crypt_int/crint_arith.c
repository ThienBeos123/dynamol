#include "cryptInt_func.h"
#include "crint_algo_core.h"


//* ========================================= MAGNITUDE MATHEMATICA ========================================== *//
/* ------------------- MAGNITUDED ARITHMETIC ------------------- */
void __CRINT_MAGADD__(crint *res, const crint *a, const crint *b, dnml_status *err) {
    _magcrint_poison(res, { crint_free(res); crint_free(a); crint_free(b); __crint_exit(); });
    _magcrint_poison(a, { crint_free(res); crint_free(a); crint_free(b); __crint_exit(); });
    _magcrint_poison(b, { crint_free(res); crint_free(a); crint_free(b); __crint_exit(); });
    dnml_status err_check = crint_reserve(res, crtmax(a->n, b->n) + 1);
    mheap_alloc_oom(err_check, err); __CRINT_ADD_WC__(res, a, b);
    *err = CRINT_SUCCESS;
}
void __CRINT_MAGSUB__(crint *res, const crint *a, const crint *b, dnml_status *err) {
    _magcrint_poison(res, { crint_free(res); crint_free(a); crint_free(b); __crint_exit(); });
    _magcrint_poison(a, { crint_free(res); crint_free(a); crint_free(b); __crint_exit(); });
    _magcrint_poison(b, { crint_free(res); crint_free(a); crint_free(b); __crint_exit(); });
    DNML_TEST_ASSERT((__BIGINT_INTERNAL_COMP__(a, b) != -1),
        "Subtraction Underflow: Subtrahend's magnitude is too large for Minuend"
        " (-Esub_underflow)", { crint_free(res); crint_free(a); crint_free(b); __crint_exit(); }
    );
    dnml_status err_check = crint_reserve(res, a->n);
    _mheap_alloc_oom(err_check, err); __CRINT_SUB_WC__(res, a, b);
    *err = CRINT_SUCCESS;
}
void __CRINT_MAGMUL___(crint *res, const crint *a, const crint *b, dnml_status *err) {
    dnml_arena *_DCSI_MAGMUL_ARENA = _USE_CARENA();
    test_assert_mut(
        /* Static Analysis - Assert Parameters */
        (!(_DCSI_MAGMUL_ARENA->poisoined)), realloc_null,
        { crint_free(res); crint_free(a); crint_free(b); __crint_exit(); },
        err, DNML_ALLOC_OOM, ; /* Error Returns Parameters */
    )
    size_t needed_size = __CRINT_MUL_WS__(a->n, b->n);
}