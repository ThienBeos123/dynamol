#include "pow_root.h"





/* ----------------- WORKSPACE ----------------- */
size_t __BIGINT_BINEXP_WS__(size_t base_size, uint64_t pow) {
    size_t raw = base_size * pow;
    size_t fcall_size = __BIGINT_MUL_WS__((raw - base_size), (raw - base_size));
    return raw + fcall_size;
}
size_t __BIGINT_2K_ARY_WS__(size_t base_size, uint64_t pow, uint8_t ksize) {
    // RAW OBJECTS
    size_t table_size = 1 << (ksize - 1);
    size_t raw_outside = (base_size << 1) + (base_size * pow);
    size_t table_pows = (base_size * table_size) * (table_size + 1);
    // FUNCTION CALL WORKSPACE
    size_t outside_fcalls = __BIGINT_MUL_WS__(base_size, base_size);
    size_t table_loop_fcalls = __BIGINT_MUL_WS__((base_size * (((table_size - 1) << 1) + 1)), (base_size << 1));
    size_t main_loop_even = __BIGINT_MUL_WS__((base_size * pow - base_size), (base_size * pow - base_size));
    size_t main_loop_odd = __BIGINT_MUL_WS__(
        (base_size * pow - base_size), 
        (base_size * ((table_size << 1) + 1))
    ); size_t max_fcall = max(outside_fcalls,
                              max(table_loop_fcalls,
                                  max(main_loop_even, main_loop_odd)));
    return raw_outside + table_pows + max_fcall;
}
size_t __BIGINT_SLIDIN_WS__(size_t base_size, uint64_t pow, uint8_t ksize) {
    // RAW OBJECTS
    size_t table_size = 1 << (ksize - 1);
    size_t raw_outside = (base_size << 1) + (base_size * pow);
    size_t table_pows = (base_size * table_size) * (table_size + 1);
    // FUNCTION CALL WORKSPACE
    size_t outside_fcall = __BIGINT_MUL_WS__(base_size, base_size);
    size_t table_loop_fcalls = __BIGINT_MUL_WS__((base_size * (((table_size - 1) << 1) + 1)), (base_size << 1));
    size_t main_loop_even = __BIGINT_MUL_WS__((base_size * pow - base_size), (base_size * pow - base_size));
    size_t main_loop_odd = __BIGINT_MUL_WS__(
        (base_size * pow - base_size),
        (base_size * ((table_size << 1) + 1))
    ); size_t max_fcall = max(outside_fcall,
                              max(table_loop_fcalls,
                                  max(main_loop_even, main_loop_odd)));
    return raw_outside + table_pows + max_fcall;
}
size_t __BIGINT_HERON_WS__(size_t a_size) {
    size_t raw_size = 3 * a_size + 1;
    size_t fcall = __BIGINT_DIVMOD_WS__(a_size, a_size);
    return raw_size + fcall;
}
size_t __BIGINT_NEWTON_CBRT_WS__(size_t a_size) {
    size_t raw_size = 7 * (a_size + 1);
    size_t fcall = max(
        __BIGINT_MUL_WS__(a_size + 1, a_size + 1),
        __BIGINT_DIVMOD_WS__(a_size, (a_size + 1) << 1)
    ); return raw_size + fcall;
}
size_t __BIGINT_NEWTON_2NROOT_WS__(size_t a_size, uint64_t root) {
    size_t raw_size = 3 * (a_size * (root - 1));
    size_t fcall = max(
        __BIGINT_EXP_WS__(a_size, root - 1),
        __BIGINT_DIVMOD_WS__(a_size, a_size * (root - 1))
    ); return raw_size + a_size + fcall;
}
size_t __BIGINT_NEWTON_NROOT_WS__(size_t a_size, uint64_t root) {
    size_t raw_size = 3 * (a_size * (root - 1));
    size_t fcall = max(
        __BIGINT_EXP_WS__(a_size, root - 1),
        __BIGINT_DIVMOD_WS__(a_size, a_size * (root - 1))
    ); return raw_size + a_size + fcall;
}



/* ----------------- ALGORITHMS ----------------- */
void __BIGINT_BINARY_EXP__(bigInt *res, const bigInt *base, uint64_t power, calc_ctx binexp_ctx) {
    // SETUP
    dnml_status err_check, end_stat = 0;
    size_t binexp_mark = scratch_mark(&binexp_ctx);
    BIGINT_TEMP(tmp_res, base->n * power, binexp_ctx, err_check, end_stat);
    tmp_res.limbs[0] = 1; tmp_res.n = 1;
    BIGINT_TEMP(tmp_base, base->n * power, binexp_ctx, err_check, end_stat);
    memcpy(tmp_base.limbs, base->limbs, base->n * U64_BYTES);
    tmp_base.n = base->n;

    // MAIN OPERATION
    while (power) { 
        if (power & 1) { 
            __BIGINT_MUL_DISPATCH__(&tmp_res, &tmp_base, &tmp_res, binexp_ctx); 
        } __BIGINT_MUL_DISPATCH__(&tmp_base, &tmp_base, &tmp_base, binexp_ctx); 
        power >>= 1;
    } __BIGINT_INTERNAL_COPY__(res, &tmp_res); 
    scratch_reset(&binexp_ctx, binexp_mark);
}
void __BIGINT_2K_ARY__(bigInt *res, const bigInt *base, uint64_t power, uint8_t k, calc_ctx fixed_ctx) {
    //* --- 1. SETUP ---- *//
    dnml_status err_check, end_stat = 0;
    size_t table_size = 1 << (k - 1), _2kary_mark = scratch_mark(&fixed_ctx);
    bigInt table[table_size]; table[0] = *base;
    BIGINT_TEMP(x2, (base->n << 1), fixed_ctx, err_check, end_stat);
    __BIGINT_MUL_DISPATCH__(base, base, &x2, fixed_ctx);

    //* ---- 2. TABLE PRECOMPUTATION ----- *//
    for (size_t i = 1; i < table_size; ++i) {
        table[i].limbs = scratch_alloc(&fixed_ctx, base->n * ((i << 1) + 1) * U64_BYTES, &err_check);
        mod_endstat(end_stat, err_check);
        DNML_TEST_ASSERT(
            !(end_stat == DNML_ALLOC_OOM),
            "Insufficient Scratch Allocation Capaicty (-Earena_cap_overflow)",
            { scratch_clear(&fixed_ctx); scratch_destruct(&fixed_ctx); }
        );
        table[i].cap   = base->n * ((i << 1) + 1);
        table[i].sign  = 1;
        __BIGINT_MUL_DISPATCH__(&table[i-1], &x2, &table[i], fixed_ctx);
    }

    //* ----- 3. MAIN LOOP ------ *//
    uint8_t chunk_count = (uint8_t)((U64_BITS - __CLZ_UI64__(power)) >> 2) + 1;
    BIGINT_TEMP(tmp_res, base->n * power, fixed_ctx, err_check, end_stat);
    tmp_res.limbs[0] = 1; tmp_res.n = 1; tmp_res.sign = 1;
    uint64_t mask = (1ULL << k) - 1; uint8_t curr_shift = 0;
    for (uint8_t i = chunk_count - 1; i != -1; --i) {
        curr_shift = U64_BITS - k * (chunk_count - i - 1);
        uint64_t curr_chunk = power & (mask << curr_shift);
        curr_chunk = curr_chunk >>= curr_shift; curr_chunk = (uint8_t)(curr_chunk);
        uint8_t s = __CTZ_UI64__(curr_chunk); curr_chunk >>= s;

        for (uint8_t j = 1; j <= chunk_count - s; ++j) {
            __BIGINT_MUL_DISPATCH__(&tmp_res, &tmp_res, &tmp_res, fixed_ctx);
        } __BIGINT_MUL_DISPATCH__(&tmp_res, &table[(curr_chunk - 1) >> 1], &tmp_res, fixed_ctx);
        for (uint8_t j = 1; j <= s; ++j) __BIGINT_MUL_DISPATCH__(&tmp_res, &tmp_res, &tmp_res, fixed_ctx);
    } __BIGINT_INTERNAL_COPY__(res, &tmp_res); scratch_reset(&fixed_ctx, _2kary_mark);
}
void __BIGINT_SLIDING__(bigInt *res, const bigInt *base, uint64_t power, uint8_t k, calc_ctx slide_ctx) {
    //* --- 1. SETUP ---- *//
    dnml_status err_check, end_stat = 0;
    size_t table_size = 1 << (k - 1), slidin_mark = scratch_mark(&slide_ctx);
    bigInt table[table_size]; table[0] = *base;
    BIGINT_TEMP(x2, (base->n << 1), slide_ctx, err_check, end_stat);
    __BIGINT_MUL_DISPATCH__(base, base, &x2, slide_ctx);

    //* ---- 2. TABLE PRECOMPUTATION ----- *//
    for (size_t i = 1; i < table_size; ++i) {
        table[i].limbs = scratch_alloc(&slide_ctx, base->n * ((i << 1) + 1) * U64_BYTES, &err_check);
        mod_endstat(end_stat, err_check);
        DNML_TEST_ASSERT(
            !(end_stat == DNML_ALLOC_OOM),
            "Insufficient Scratch Allocation Capaicty (-Earena_cap_overflow)",
            { scratch_clear(&slide_ctx); scratch_destruct(&slide_ctx); }
        );
        table[i].cap   = base->n * ((i << 1) + 1); 
        table[i].sign  = 1;
        __BIGINT_MUL_DISPATCH__(&table[i-1], &x2, &table[i], slide_ctx);
    }

    //* ------------ 3. MAIN LOOP ------------ *//
    BIGINT_TEMP(tmp_res, base->n * power, slide_ctx, err_check, end_stat);
    tmp_res.limbs[0] = 1; tmp_res.n = 1; tmp_res.sign = 1;
    uint8_t curr_pos = U64_BITS - (__CLZ_UI64__(power)) - 1;
    uint64_t mask = (1ULL << k) - 1;
    while (curr_pos != -1) {
        uint8_t curr_bit = power & (1ULL << curr_pos);
        if (!curr_bit) { __BIGINT_MUL_DISPATCH__(&tmp_res, &tmp_res, &tmp_res, slide_ctx); --curr_pos; } 
        else {
            uint8_t s = max(curr_pos - k + 1, 0);
            uint64_t curr_chunk = power & (mask << curr_pos - (k - 1));
            curr_chunk >>= curr_pos - (k - 1); curr_chunk = (uint8_t)(curr_chunk);
            uint8_t tz = __CTZ_UI64__(curr_chunk); s -= tz; curr_chunk >>= tz;
            for (uint8_t i = 0; i <= curr_bit - s; ++i) {
                __BIGINT_MUL_DISPATCH__(&tmp_res, &tmp_res, &tmp_res, slide_ctx);
            } __BIGINT_MUL_DISPATCH__(&tmp_res, &table[(curr_chunk - 1) >> 1], &tmp_res, slide_ctx);
            curr_pos = s - 1;
        }
    } __BIGINT_INTERNAL_COPY__(res, &tmp_res); 
    scratch_reset(&slide_ctx, slidin_mark);

}
void __BIGINT_HERON__(bigInt *res, const bigInt *a, calc_ctx heron_ctx) {
    dnml_status err_check, end_stat = 0;
    uint64_t guess_bits = (__BIGINT_COUNTDB__(a, 2) + 1) >> 1;
    size_t heron_mark = scratch_mark(&heron_ctx);
    BIGINT_TEMP(guess, a->n, heron_ctx, err_check, end_stat); 
    BIGINT_TEMP(ratio, a->n, heron_ctx, err_check, end_stat);
    BIGINT_TEMP(next, a->n + 1, heron_ctx, err_check, end_stat);
    guess.limbs[(guess_bits << 6)] = 1ULL << (guess_bits % 64); 
    guess.n = (guess_bits << 6) + 1;
    while (true) {
        // next in DIVMOD_DISPATCH acts as a temporary buffer
        __BIGINT_DIVMOD_DISPATCH__(a, &guess, &ratio, &next, heron_ctx);
        __BIGINT_ADD_WC__(&next, &guess, &ratio);
        __BIGINT_INTERNAL_RSHIFT__(&next, 1);
        int8_t comp_res = __BIGINT_INTERNAL_COMP__(&next, &guess);
        if (!comp_res || comp_res == 1) break;
        __BIGINT_INTERNAL_SWAP__(&guess, &next);
    } __BIGINT_INTERNAL_COPY__(res, &guess); scratch_reset(&heron_ctx, heron_mark);
}
void __BIGINT_NEWTON_CBRT__(bigInt *res, const bigInt *a, calc_ctx cbrt_ctx) {
    dnml_status err_check, end_stat = 0;
    uint64_t guess_bits = (__BIGINT_COUNTDB__(a, 2) + 2) / 3;
    size_t cbrt_mark = scratch_mark(&cbrt_ctx);
    BIGINT_TEMP(guess,  (a->n + 1) << 1,  cbrt_ctx, err_check, end_stat); 
    BIGINT_TEMP(ratio,   a->n + 1,        cbrt_ctx, err_check, end_stat);
    BIGINT_TEMP(next,   (a->n + 1) << 1,  cbrt_ctx, err_check, end_stat);
    BIGINT_TEMP(tmp,    (a->n + 1) << 1,  cbrt_ctx, err_check, end_stat);
    guess.limbs[(guess_bits << 6)] = 1ULL << (guess_bits % 64); 
    guess.n = (guess_bits << 6) + 1;
    while (true) {
        __BIGINT_MUL_DISPATCH__(&guess, &guess, &next, cbrt_ctx);
        __BIGINT_DIVMOD_DISPATCH__(a, &next, &ratio, &tmp, cbrt_ctx);
        __BIGINT_INTERNAL_COPY__(&next, &guess);
        __BIGINT_INTERNAL_LSHIFT__(&next, 1);
        __BIGINT_ADD_WC__(&next, &next, &ratio);
        __BIGINT_DIV3__(&next);
        int8_t comp_res = __BIGINT_INTERNAL_COMP__(&next, &guess);
        if (!comp_res || comp_res == 1) break;
        __BIGINT_INTERNAL_SWAP__(&guess, &next);
    } __BIGINT_INTERNAL_COPY__(res, &guess); scratch_reset(&cbrt_ctx, cbrt_mark);
}
uint64_t __UI64_NROOT__(uint64_t a, uint64_t root) {
    if (__IS_2POW__(root)) {
        uint8_t shift = __CTZ_UI64__(root);
        uint64_t guess = ((U64_BITS - __CLZ_UI64__(a)) + root - 1) >> shift;
        guess = 1ULL << guess; uint64_t xpow = pow(guess, (root - 1)), 
        ratio = 0, next = 0;
        while (true) {
            ratio = a / xpow;
            next = guess * (root - 1);
            next += ratio; next >>= shift;
            if (next >= guess) break;
            guess = next;
            xpow = pow(guess, (root - 1));
        } return guess;
    } else {
        uint64_t guess = ((U64_BITS - __CLZ_UI64__(a)) + root - 1) / root;
        guess = 1ULL << guess; uint64_t xpow = pow(guess, (root - 1)),
        ratio = 0, next = 0;
        while (true) {
            ratio = a / xpow;
            if (__IS_2POW__(root - 1)) {
                next = guess >> __CTZ_UI64__(root - 1);
            } else next = guess * (root - 1);
            next = ratio; next /= root;
            if (next >= guess) break;
            guess = next;
            xpow = pow(guess, (root - 1));
        } return guess;
    }
}
void __BIGINT_NEWTON_2NROOT__(bigInt *res, const bigInt *a, uint64_t root, calc_ctx _2nroot_ctx) {
    // ! WARNING ! WARNING ! WARNING ! WARNING !
    //   THIS FUNCTION EXPECTS THE ROOT TO A POWER OF 2
    // ! WARNING ! WARNING ! WARNING ! WARNING !
    dnml_status err_check, end_stat = 0;
    uint8_t shift = __CTZ_UI64__(root);
    uint64_t guess_bits = (__BIGINT_COUNTDB__(a, 2) + root - 1) >> shift;
    size_t _2nroot_mark = scratch_mark(&_2nroot_ctx);
    BIGINT_TEMP(guess, a->n * (root - 1), _2nroot_ctx, err_check, end_stat); 
    BIGINT_TEMP(ratio, a->n, _2nroot_ctx, err_check, end_stat);
    guess.limbs[(guess_bits << 6)] = 1ULL << (guess_bits % 64); 
    guess.n = (guess_bits << 6) + 1;
    BIGINT_TEMP(next, a->n * (root - 1), _2nroot_ctx, err_check, end_stat); 
    BIGINT_TEMP(xpow, a->n * (root - 1), _2nroot_ctx, err_check, end_stat);
    __BIGINT_EXP_DISPATCH__(&xpow, &guess, (root - 1), _2nroot_ctx);
    while (true) {
        // next in DIVMOD_DISPATCH acts as a temporary buffer;
        __BIGINT_DIVMOD_DISPATCH__(a, &xpow, &ratio, &next, _2nroot_ctx);
        __BIGINT_INTERNAL_COPY__(&next, &guess);
        __BIGINT_INTERNAL_MUL_UI64__(&next, (root - 1));
        __BIGINT_ADD_WC__(&next, &next, &ratio);
        __BIGINT_INTERNAL_RSHIFT__(&next, shift);
        int8_t comp_res = __BIGINT_INTERNAL_COMP__(&next, &guess);
        if (!comp_res || comp_res == 1) break;
        __BIGINT_INTERNAL_SWAP__(&guess, &next);
        __BIGINT_EXP_DISPATCH__(&xpow, &guess, (root - 1), _2nroot_ctx);
    } __BIGINT_INTERNAL_COPY__(res, &guess); scratch_reset(&_2nroot_ctx, _2nroot_mark);
}
void __BIGINT_NEWTON_NROOT__(bigInt *res, const bigInt *a, uint64_t root, calc_ctx nroot_ctx) {
    dnml_status err_check, end_stat = 0;
    uint64_t guess_bits = (__BIGINT_COUNTDB__(a, 2) + root - 1) / root;
    size_t nroot_mark = scratch_mark(&nroot_ctx);
    BIGINT_TEMP(guess, a->n * (root - 1), nroot_ctx, err_check, end_stat); 
    BIGINT_TEMP(ratio, a->n, nroot_ctx, err_check, end_stat);
    guess.limbs[(guess_bits << 6)] = 1ULL << (guess_bits % 64); 
    guess.n = (guess_bits << 6) + 1;
    BIGINT_TEMP(next, a->n * (root - 1), nroot_ctx, err_check, end_stat); 
    BIGINT_TEMP(xpow, a->n * (root - 1), nroot_ctx, err_check, end_stat);
    __BIGINT_EXP_DISPATCH__(&xpow, &guess, (root - 1), nroot_ctx);
    while (true) {
        // next in DIVMOD_DISPATCH acts as a temporary buffer;
        __BIGINT_DIVMOD_DISPATCH__(a, &xpow, &ratio, &next, nroot_ctx);
        __BIGINT_INTERNAL_COPY__(&next, &guess);
        if (__IS_2POW__(root - 1)) {
            __BIGINT_INTERNAL_LSHIFT__(&next, __CTZ_UI64__(root - 1));
        } else __BIGINT_INTERNAL_MUL_UI64__(&next, (root - 1));
        __BIGINT_ADD_WC__(&next, &next, &ratio);
        // AT ANY POINT IN TIME, The actual size of "next" when used and calculate normally, 
        // disregarding its usage as a temporary buffer, its maximum size is always a->n
        // -------> Uses "ratio" as a temporary buffer for the remainder
        __BIGINT_SHORT_DIVISION__(&next, root, &next, &ratio);
        int8_t comp_res = __BIGINT_INTERNAL_COMP__(&next, &guess);
        if (!comp_res || comp_res == 1) break;
        __BIGINT_INTERNAL_SWAP__(&guess, &next);
        __BIGINT_EXP_DISPATCH__(&xpow, &guess, (root - 1), nroot_ctx);
    } __BIGINT_INTERNAL_COPY__(res, &guess); scratch_reset(&nroot_ctx, nroot_mark);
}



/* ----------------- DISPATCHES ----------------- */
size_t __BIGINT_EXP_WS__(size_t base_size, uint64_t pow) {
    uint8_t pow_bits = U64_BITS - __CLZ_UI64__(pow);
    if (pow_bits <= BIGINT_BINARY) return __BIGINT_BINEXP_WS__(base_size, pow);
    else if (pow_bits <= BIGINT_FIXED) return __BIGINT_2K_ARY_WS__(base_size, pow, 4);
    else return __BIGINT_SLIDIN_WS__(base_size, pow, 3);
}
size_t __BIGINT_SQRT_WS__(size_t a_size) {
    if (a_size <= BIGINT_NAIVE) return 0;
    else return __BIGINT_HERON_WS__(a_size);
}
size_t __BIGINT_CBRT_WS__(size_t a_size) {
    if (a_size <= BIGINT_NAIVE) return 0;
    else return __BIGINT_NEWTON_CBRT_WS__(a_size);
}
size_t __BIGINT_NROOT_WS__(size_t a_size, uint64_t root) {
    if (a_size <= BIGINT_NAIVE) return 0;
    else {
        if (root == 2) return __BIGINT_HERON_WS__(a_size);
        else if (root == 3) return __BIGINT_NEWTON_CBRT_WS__(a_size);
        else if (__IS_2POW__(root)) __BIGINT_NEWTON_2NROOT_WS__(a_size, root);
        else __BIGINT_NEWTON_NROOT_WS__(a_size, root);
    }
}
void __BIGINT_EXP_DISPATCH__(bigInt *res, const bigInt *base, uint64_t power, calc_ctx exp_ctx) {
    uint8_t pow_bits = U64_BITS - __CLZ_UI64__(power);
    if (pow_bits <= BIGINT_BINARY) __BIGINT_BINARY_EXP__(res, base, power, exp_ctx);
    else if (pow_bits <= BIGINT_FIXED) __BIGINT_2K_ARY__(res, base, power, 4, exp_ctx);
    else __BIGINT_SLIDING__(res, base, power, 3, exp_ctx);
}
void __BIGINT_SQRT_DISPATCH__(bigInt *res, const bigInt *a, calc_ctx sqrt_ctx) {
    if (a->n <= BIGINT_NAIVE) {
        res->limbs[0] = (uint64_t)(sqrt(a->limbs[0]));
        res->n = 1;
    } else __BIGINT_HERON__(res, a, sqrt_ctx);
}
void __BIGINT_CBRT_DISPATCH__(bigInt *res, const bigInt *a, calc_ctx cbrt_ctx) {
    if (a->n <= BIGINT_NAIVE) {
        res->limbs[0] = (uint64_t)(cbrt(a->limbs[0]));
        res->n = 1;
    } else __BIGINT_NEWTON_CBRT__(res, a, cbrt_ctx);
}
void __BIGINT_NROOT_DISPATCH__(bigInt *res, const bigInt *a, uint64_t root, calc_ctx nroot_ctx) {
    if (a->n <= BIGINT_NAIVE) {
        if (root == 2) res->limbs[0] = (uint64_t)(sqrt(a->limbs[0]));
        else if (root = 3) res->limbs[0] = (uint64_t)(cbrt(a->limbs[0]));
        else res->limbs[0] = __UI64_NROOT__(a->limbs[0], root);
        res->n = 1;
    } else {
        if (root == 2) __BIGINT_HERON__(res, a, nroot_ctx);
        else if (root == 3) __BIGINT_NEWTON_CBRT__(res, a, nroot_ctx);
        else if (__IS_2POW__(root)) __BIGINT_NEWTON_2NROOT__(res, a, root, nroot_ctx);
        else __BIGINT_NEWTON_NROOT__(res, a, root, nroot_ctx);
    }
}