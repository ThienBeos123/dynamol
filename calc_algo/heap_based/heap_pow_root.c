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



#include "heap_pow_root.h"


/* ----------------- ALGORITHMS ----------------- */
void __BIHEAP_BINARY_EXP__(bigInt *const res, const bigInt *const base, uint64_t exp, dnml_status *err) {
    // SETUP
    dnml_status echeck = BIGINT_SUCCESS; bigInt *alloc_list[1], *early_free[2]; uint8_t alloc_cnt = 0, early_cnt = 0;
    BIHEAP_RET(tmp_res, base->n * exp, echeck, err, early_free, early_cnt,);
    BIHEAP_TEMP(tmp_base, base->n * exp, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    tmp_res.limbs[0] = 1; tmp_res.n = 1; /**/ memcpy(tmp_base.limbs, base->limbs, base->n * U64_BYTES);
    /* ------- MAIN OPERATION ------- */ tmp_base.n = base->n;
    while (exp) {
        if (exp & 1) {
            __BIHEAP_MUL_DISP__(&tmp_res, &tmp_base, &tmp_res, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
        } __BIHEAP_MUL_DISP__(&tmp_base, &tmp_base, &tmp_base, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,); 
        exp >>= 1; // Moving on to the next bit t evaluate
    } __BIGINT_INTERNAL_MOVE__(res, &tmp_res); _free_alloc_list(alloc_list, alloc_cnt); *err = BIGINT_SUCCESS;
}
void __BIHEAP_FIXED__(bigInt *const res, const bigInt *const base, uint64_t exp, uint8_t k, dnml_status *err) {
    /* --- 1. SETUP ---- */ dnml_status echeck = BIGINT_SUCCESS; 
    bigInt *alloc_list[1], *early_free[2]; uint8_t alloc_cnt = 0, early_cnt = 0;
    size_t table_size = 1 << (k - 1); bigInt table[table_size]; table[0] = *base;
    BIHEAP_TEMP(x2, (base->n << 1), echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    __BIHEAP_MUL_DISP__(base, base, &x2, &echeck);  HEAP_OOM(echeck, err, early_free, early_cnt,);

    //* ---- 2. TABLE PRECOMPUTATION ----- *//
    for (size_t i = 1; i < table_size; ++i) {
        __BIGINT_INTERNAL_LINIT__(&table[i], base->n * ((i << 1) + 1)); HEAP_OOM(echeck, err, early_free, early_cnt,);
        __BIHEAP_MUL_DISP__(&table[i-1], &x2, &table[i], &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    }

    //* ----- 3. MAIN LOOP ------ *//
    uint8_t chunk_count = (uint8_t)((U64_BITS - __CLZ_UI64__(exp)) >> 2) + 1;
    BIHEAP_RET(tmp_res, (base->n * exp), echeck, err, early_free, early_cnt,);
    tmp_res.limbs[0] = 1; tmp_res.n = 1; tmp_res.sign = 1;
    uint64_t mask = (1ULL << k) - 1; uint8_t curr_shift = 0;
    for (uint8_t i = chunk_count - 1; i != (uint8_t)-1; --i) {
        curr_shift = U64_BITS - k * (chunk_count - i - 1);
        uint64_t curr_chunk = exp & (mask << curr_shift);
        curr_chunk >>= curr_shift; curr_chunk = (uint8_t)(curr_chunk);
        uint8_t s = __CTZ_UI64__(curr_chunk); curr_chunk >>= s;

        for (uint8_t j = 1; j <= chunk_count - s; ++j) {
            __BIHEAP_MUL_DISP__(&tmp_res, &tmp_res, &tmp_res, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
        } __BIHEAP_MUL_DISP__(&tmp_res, &table[(curr_chunk - 1) >> 1], &tmp_res, &echeck);
        HEAP_OOM(echeck, err, early_free, early_cnt,);
        for (uint8_t j = 1; j <= s; ++j) {
            __BIHEAP_MUL_DISP__(&tmp_res, &tmp_res, &tmp_res, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
        }
    } __BIGINT_INTERNAL_MOVE__(res, &tmp_res); _free_alloc_list(alloc_list, alloc_cnt); *err = BIGINT_SUCCESS;
}
void __BIHEAP_SLIDING__(bigInt *const res, const bigInt *const base, uint64_t exp, uint8_t k, dnml_status *err) {
    /* --- 1. SETUP ---- */ dnml_status echeck = BIGINT_SUCCESS; 
    bigInt *alloc_list[1], *early_free[2]; uint8_t alloc_cnt = 0, early_cnt = 0;
    size_t table_size = 1 << (k - 1); bigInt table[table_size]; table[0] = *base;
    BIHEAP_TEMP(x2, (base->n << 1), echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    __BIHEAP_MUL_DISP__(base, base, &x2, &echeck);  HEAP_OOM(echeck, err, early_free, early_cnt,);

    //* ---- 2. TABLE PRECOMPUTATION ----- *//
    for (size_t i = 1; i < table_size; ++i) {
        __BIGINT_INTERNAL_LINIT__(&table[i], base->n * ((i << 1) + 1)); HEAP_OOM(echeck, err, early_free, early_cnt,);
        __BIHEAP_MUL_DISP__(&table[i-1], &x2, &table[i], &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    }

    //* ------------ 3. MAIN LOOP ------------ *//
    BIHEAP_RET(tmp_res, base->n * exp, echeck, err, early_free, early_cnt,);
    tmp_res.limbs[0] = 1; tmp_res.n = 1; tmp_res.sign = 1;
    uint8_t curr_pos = U64_BITS - (__CLZ_UI64__(exp)) - 1;
    while (curr_pos != (uint8_t)-1) {
        uint8_t curr_bit = exp & (1ULL << curr_pos);
        if (!curr_bit) { 
            __BIHEAP_MUL_DISP__(&tmp_res, &tmp_res, &tmp_res, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
        } else {
            int8_t s = max(((int8_t)curr_pos - (int8_t)k + 1), 0);
            uint64_t mask = (1ULL << (curr_pos - k + 1)) - 1;
            uint64_t curr_chunk = (exp >> s) & mask;
            curr_chunk >>= curr_pos - k + 1; curr_chunk = (uint8_t)(curr_chunk);
            uint8_t tz = __CTZ_UI64__(curr_chunk); s -= tz; curr_chunk >>= tz;
            for (uint8_t i = 0; i <= curr_bit - s; ++i) {
                __BIHEAP_MUL_DISP__(&tmp_res, &tmp_res, &tmp_res, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
            } __BIHEAP_MUL_DISP__(&tmp_res, &table[(curr_chunk - 1) >> 1], &tmp_res, &echeck); 
            HEAP_OOM(echeck, err, early_free, early_cnt,); curr_pos = s - 1;
        }
    } __BIGINT_INTERNAL_MOVE__(res, &tmp_res); _free_alloc_list(alloc_list, alloc_cnt); *err = BIGINT_SUCCESS;
}
void __BIHEAP_HERON__(bigInt *const res, const bigInt *const a, dnml_status *err) {
    dnml_status echeck = BIGINT_SUCCESS; bigInt *alloc_list[2], *early_free[3]; uint8_t alloc_cnt = 0, early_cnt = 0;
    uint64_t guess_bits = (__BIGINT_COUNTDB__(a, 2) + 1) >> 1;
    BIHEAP_RET(guess, a->n, echeck, err, early_free, early_cnt,);
    BIHEAP_TEMP(ratio, a->n, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    BIHEAP_TEMP(next, a->n + 1, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    guess.limbs[(guess_bits << 6)] = 1ULL << (guess_bits % 64); /**/ guess.n = (guess_bits << 6) + 1;
    for (;;) {
        // next in DIVMOD_DISPATCH acts as a temporary buffer
        __BIHEAP_DIV_DISP__(a, &guess, &ratio, &next, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
        __BIGINT_ADD_WC__(&next, &guess, &ratio); __BIGINT_INTERNAL_RSHIFT__(&next, 1);
        int8_t comp_res = __BIGINT_INTERNAL_COMP__(&next, &guess);
        if (!comp_res || comp_res == 1) break;
        __BIGINT_INTERNAL_SWAP__(&guess, &next);
    } __BIGINT_INTERNAL_MOVE__(res, &guess); _free_alloc_list(alloc_list, alloc_cnt); *err = BIGINT_SUCCESS;
}
void __BIHEAP_NEWTON_CBRT__(bigInt *const res, const bigInt *const a, dnml_status *err) {
    dnml_status echeck = BIGINT_SUCCESS; bigInt *alloc_list[3], *early_free[4]; uint8_t alloc_cnt = 0, early_cnt = 0;
    uint64_t guess_bits = (__BIGINT_COUNTDB__(a, 2) + 2) / 3;
    BIHEAP_RET(guess,   (a->n + 1) << 1, echeck, err, early_free, early_cnt,);
    BIHEAP_TEMP(ratio,   a->n + 1,       echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    BIHEAP_TEMP(next,   (a->n + 1) << 1, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    BIHEAP_TEMP(tmp,    (a->n + 1) << 1, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    guess.limbs[(guess_bits << 6)] = 1ULL << (guess_bits % 64); guess.n = (guess_bits << 6) + 1;
    for (;;) {
        __BIHEAP_MUL_DISP__(&guess, &guess, &next, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
        __BIHEAP_DIV_DISP__(a, &next, &ratio, &tmp, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
        // Can't use move semantics since we would LOSE next's buffers, leaking memory,
        // even though we want to do an assignment of next = guess
        __BIGINT_INTERNAL_COPY__(&next, &guess); __BIGINT_INTERNAL_LSHIFT__(&next, 1);
        __BIGINT_ADD_WC__(&next, &next, &ratio); __BIGINT_DIV3__(&next);
        int8_t comp_res = __BIGINT_INTERNAL_COMP__(&next, &guess);
        if (!comp_res || comp_res == 1) break; /**/ __BIGINT_INTERNAL_SWAP__(&guess, &next);
    } __BIGINT_INTERNAL_MOVE__(res, &guess); _free_alloc_list(alloc_list, alloc_cnt); *err = BIGINT_SUCCESS;
}
uint64_t __UI64_NROOT__(uint64_t a, uint64_t root) {
    if (__IS_2POW__(root)) {
        uint8_t shift = __CTZ_UI64__(root);
        uint64_t guess = ((U64_BITS - __CLZ_UI64__(a)) + root - 1) >> shift;
        guess = 1ULL << guess; uint64_t xpow = pow(guess, (root - 1)),
        ratio = 0, next = 0;
        for (;;) {
            ratio = a / xpow; /**/ next = guess * (root - 1);
            next += ratio; /**/ next >>= shift;
            if (next >= guess) break;
            guess = next; /**/ xpow = pow(guess, (root - 1));
        } return guess;
    } else {
        uint64_t guess = ((U64_BITS - __CLZ_UI64__(a)) + root - 1) / root;
        guess = 1ULL << guess; uint64_t xpow = pow(guess, (root - 1)),
        ratio = 0, next = 0;
        for (;;) { ratio = a / xpow;
            if (__IS_2POW__(root - 1)) next = guess >> __CTZ_UI64__(root - 1);
            else next = guess * (root - 1);
            next = ratio; next /= root;
            if (next >= guess) break;
            guess = next; /**/ xpow = pow(guess, (root - 1));
        } return guess;
    }
}
void __BIHEAP_NEWTON_2NRT__(bigInt *const res, const bigInt *const a, uint64_t root, dnml_status *err) {
    // ! WARNING ! WARNING ! WARNING ! WARNING !
    //   THIS FUNCTION EXPECTS THE ROOT TO A POWER OF 2
    // ! WARNING ! WARNING ! WARNING ! WARNING !
    dnml_status echeck = BIGINT_SUCCESS; bigInt *alloc_list[3], *early_free[4]; uint8_t alloc_cnt = 0, early_cnt = 0;
    uint8_t shift = __CTZ_UI64__(root); uint64_t guess_bits = (__BIGINT_COUNTDB__(a, 2) + root - 1) >> shift;
    BIHEAP_RET(guess, a->n * (root - 1), echeck, err, early_free, early_cnt,);
    BIHEAP_TEMP(ratio, a->n, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    guess.limbs[(guess_bits << 6)] = 1ULL << (guess_bits % 64);
    guess.n = (guess_bits << 6) + 1;
    BIHEAP_TEMP(next, a->n * (root - 1), echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    BIHEAP_TEMP(xpow, a->n * (root - 1), echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    __BIHEAP_EXP_DISPATCH__(&xpow, &guess, (root - 1), &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    for (;;) {
        // next in DIVMOD_DISPATCH acts as a temporary buffer;
        __BIHEAP_DIV_DISP__(a, &xpow, &ratio, &next, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
        // Can't use move semantics since we would LOSE next's buffers, leaking memory,
        // even though we want to do an assignment of next = guess
        __BIGINT_INTERNAL_COPY__(&next, &guess); __BIGINT_INTERNAL_MUL_UI64__(&next, (root - 1));
        __BIGINT_ADD_WC__(&next, &next, &ratio); __BIGINT_INTERNAL_RSHIFT__(&next, shift);
        int8_t comp_res = __BIGINT_INTERNAL_COMP__(&next, &guess);
        if (!comp_res || comp_res == 1) break; /**/  __BIGINT_INTERNAL_SWAP__(&guess, &next);
        __BIHEAP_EXP_DISPATCH__(&xpow, &guess, (root - 1), &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    } __BIGINT_INTERNAL_MOVE__(res, &guess); _free_alloc_list(alloc_list, alloc_cnt); *err = BIGINT_SUCCESS;
}
void __BIHEAP_NEWTON_NRT__(bigInt *const res, const bigInt *const a, uint64_t root, dnml_status *err) {
    dnml_status echeck = BIGINT_SUCCESS; bigInt *alloc_list[3], *early_free[4]; uint8_t alloc_cnt = 0, early_cnt = 0;
    uint64_t guess_bits = (__BIGINT_COUNTDB__(a, 2) + root - 1) / root;
    BIHEAP_RET(guess, a->n * (root - 1), echeck, err, early_free, early_cnt,);
    BIHEAP_TEMP(ratio, a->n, echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    guess.limbs[(guess_bits << 6)] = 1ULL << (guess_bits % 64); guess.n = (guess_bits << 6) + 1;
    BIHEAP_TEMP(next, a->n * (root - 1), echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    BIHEAP_TEMP(xpow, a->n * (root - 1), echeck, err, early_free, early_cnt, alloc_list, alloc_cnt,);
    __BIHEAP_EXP_DISPATCH__(&xpow, &guess, (root - 1), &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    for (;;) {
        // next in DIVMOD_DISPATCH acts as a temporary buffer;
        __BIHEAP_DIV_DISP__(a, &xpow, &ratio, &next, &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
        // Can't use move semantics since we would LOSE next's buffers, leaking memory,
        // even though we want to do an assignment of next = guess
        __BIGINT_INTERNAL_COPY__(&next, &guess);
        if (__IS_2POW__(root - 1)) __BIGINT_INTERNAL_LSHIFT__(&next, __CTZ_UI64__(root - 1));
        else __BIGINT_INTERNAL_MUL_UI64__(&next, (root - 1));
        __BIGINT_ADD_WC__(&next, &next, &ratio);
        // AT ANY POINT IN TIME, The actual size of "next" when used and calculate normally,
        // disregarding its usage as a temporary buffer, its maximum size is always a->n
        // -------> Uses "ratio" as a temporary buffer for the remainder
        __BIHEAP_SHORT_DIVISION__(&next, root, &next, &ratio);
        int8_t comp_res = __BIGINT_INTERNAL_COMP__(&next, &guess);
        if (!comp_res || comp_res == 1) break; /**/ __BIGINT_INTERNAL_SWAP__(&guess, &next);
        __BIHEAP_EXP_DISPATCH__(&xpow, &guess, (root - 1), &echeck); HEAP_OOM(echeck, err, early_free, early_cnt,);
    } __BIGINT_INTERNAL_MOVE__(res, &guess); _free_alloc_list(alloc_list, alloc_cnt); *err = BIGINT_SUCCESS;
}



/* ----------------- DISPATCHES ----------------- */
void __BIHEAP_EXP_DISPATCH__(bigInt *const res, const bigInt *const base, uint64_t exp, dnml_status *err) {
    uint8_t pow_bits = U64_BITS - __CLZ_UI64__(exp);
    if (pow_bits <= BIGINT_BINARY) __BIHEAP_BINARY_EXP__(res, base, exp, err);
    else if (pow_bits <= BIGINT_FIXED) __BIHEAP_FIXED__(res, base, exp, 4, err);
    else __BIHEAP_SLIDING__(res, base, exp, 3, err);
}
void __BIHEAP_SQRT_DISPATCH__(bigInt *const res, const bigInt *const a, dnml_status *err) {
    if (a->n <= BIGINT_NAIVE) { res->limbs[0] = (uint64_t)(sqrtl((long double)a->limbs[0])); res->n = 1; }
    else __BIHEAP_HERON__(res, a, err);
}
void __BIHEAP_CBRT_DISPATCH__(bigInt *const res, const bigInt *const a, dnml_status *err) {
    if (a->n <= BIGINT_NAIVE) { res->limbs[0] = (uint64_t)(cbrtl((long double)a->limbs[0])); res->n = 1; }
    else __BIHEAP_NEWTON_CBRT__(res, a, err);
}
void __BIHEAP_NRT_DISPATCH__(bigInt *const res, const bigInt *const a, uint64_t root, dnml_status *err) {
    if (a->n <= BIGINT_NAIVE) {
        if (root == 2) res->limbs[0] = (uint64_t)(sqrtl((long double)a->limbs[0]));
        else if (root == 3) res->limbs[0] = (uint64_t)(cbrtl((long double)a->limbs[0]));
        else res->limbs[0] = __UI64_NROOT__(a->limbs[0], root);
        res->n = 1;
    } else {
        if (root == 2) __BIHEAP_HERON__(res, a, err);
        else if (root == 3) __BIHEAP_NEWTON_CBRT__(res, a, err);
        else if (__IS_2POW__(root)) __BIHEAP_NEWTON_2NRT__(res, a, root, err);
        else __BIHEAP_NEWTON_NRT__(res, a, root, err);
    }
}
