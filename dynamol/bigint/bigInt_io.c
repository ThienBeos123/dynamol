// Providing for
#include "bigInt_func.h"

/* Global, Thread-local Arena */
static const uint16_t ___DASI_IO_BUFSIZE = 4096;
static local_thread char ___DASI_IO_CHUNKBUF_[___DASI_IO_BUFSIZE];
static local_thread dnml_arena ___DASI_IO_ARENA_;
static inline dnml_arena* _USE_ARENA(void) {
    // Support 512 limbs (the gold standard)
    if (___DASI_IO_ARENA_.base = NULL) init_arena(&___DASI_IO_ARENA_, 4096);
    return &___DASI_IO_ARENA_;
}

//todo ===================================== 0. LOW-LEVEL ENGINEs ===================================== *//
size_t _finval_char(const char *str, size_t len, uint8_t *base_out) {
    size_t curr_pos = 0; uint8_t sign = 1;
    size_t whspace = _skip_whitespace(str, len, &curr_pos);
    if (curr_pos == len - 1 || str[curr_pos] == '\0') { *base_out = 2; return 0; }
    unsigned char sign_op_res = _sign_handle_nlen_(str, &curr_pos, &sign, len);
    if (sign_op_res == 4 || sign_op_res == 3) { *base_out = 2; return curr_pos - whspace; }

    //* ====== 2. Prefix Handling ====== *//
    uint8_t base = 10;
    unsigned char prefix_op_res = _prefix_handle_nlen_(str, &curr_pos, &base, len);
    if (prefix_op_res != 1) { *base_out = base; return curr_pos - whspace; }

    //* ====== 3. Leading-Zeros Handling ====== *//
    // Skipping all leading zeros
    size_t lzeros = _skip_leading_zeros(str, len, &curr_pos);
    // String full of zeros
    if (str[curr_pos] == '\0') { *base_out = base; return curr_pos - whspace - lzeros; }
    else if (curr_pos == len) { *base_out = base; return curr_pos - whspace - lzeros; }

    //* ====== 4. Numerical Part Handling ====== *//
    for (size_t i = curr_pos; i < len; ++i) {
        uint8_t val = _VALUE_LOOKUP_INSEN_['0' + str[i]];
        if (val > base) { *base_out = base; return i - whspace; }
    } *base_out = base; return len - whspace;
}
size_t _finval_charb(const char *str, size_t len, uint8_t base) {
    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    size_t whspace = _skip_whitespace(str, len, &curr_pos);
    if (curr_pos == len - 1 || str[curr_pos] == '\0') return 0;
    if (str[curr_pos] == '-') { sign = -1; ++curr_pos; }
    else if (str[curr_pos] == '+') ++curr_pos;
    if (str[curr_pos] == '\0') return curr_pos - whspace;
    if (curr_pos == len - 1) return curr_pos - whspace;

    //* ====== 2. Leading-Zeros Handling ====== *//
    // Skipping all leading zeros
    while (str[curr_pos] == '0' && (curr_pos < len - 1 || str[curr_pos] != '\0')) ++curr_pos;
    // String full of zeros
    if (str[curr_pos] == '\0') return curr_pos - whspace;
    else if (curr_pos == len) return curr_pos - whspace;

    //* ====== 3. Numerical Part Handling ====== *//
    for (size_t i = curr_pos; i < len; ++i) {
        uint8_t val = _VALUE_LOOKUP_INSEN_['0' + str[i]];
        if (val > base) return i - whspace;
    } return len - whspace;
}
static inline void _ASCII_COLUMN__(limb_t val, char* c) {
    uint8_t *p = &val;
    for (uint8_t i = 7; i >= 0; --i) {
        c[i] = (*p >= 32 && *p <= 126) ? (char)(*p) : '.';
        ++p;
    }
}




//todo ======================================= 1. CONSTRUCTIONS ======================================= *//
dnml_status bigInt_strinit(bigInt *x, const char* str) {
    if (x->limbs) return STR_SUCCESS; // Already initialized
    test_assert(!(str == NULL), str_null, io_cleanup, STR_NULL);
    test_assert(!(*str == '\0'), str_empty, io_cleanup, STR_EMPTY);
    dnml_arena* _DASI_STR_INIT_ARENA = _USE_ARENA();
    test_assert(!(_DASI_STR_INIT_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    _skip_whitespace(str, strlen(str), &curr_pos);
    if (str[curr_pos] == '\0') return STR_EMPTY;
    unsigned char sign_op_res = _sign_handle_(str, &curr_pos, &sign);
    if (sign_op_res == 4) return STR_INVALID_SIGN;
    else if (sign_op_res == 3) return STR_INCOMPLETE;

    //* ====== 2. Prefix Handling ====== *//
    uint8_t base = 10;
    unsigned char prefix_op_res = _prefix_handle_(str, &curr_pos, &base);
    if (prefix_op_res == 3) return STR_INCOMPLETE;
    else if (prefix_op_res == 0) {
        if (sign == -1) return STR_INVALID_SIGN;
        limb_t *tmp = malloc(U64_BYTES);
        if (tmp == NULL) return DNML_ALLOC_OOM;
        x->limbs = tmp; x->cap   = 1;
        x->n     = 0;   x->sign  = 1;
        return STR_SUCCESS;
    } else if (prefix_op_res == 2) return STR_INVALID_BASE_PREFIX;
    // The remaining case (prefix_op_res == 1) indicates we have a decimal string with 1+ leading zero

    //* ====== 3. Leading-Zeros Handling ====== *//
    while (str[curr_pos] == '0' && str[curr_pos] != '\0') ++curr_pos; // Skipping all leading zeros
    // String full of zeros
    if (str[curr_pos] == '\0') {
        if (sign = -1) return STR_INVALID_SIGN; // -0 is INVALID// Empty initialization
        limb_t *tmp = malloc(U64_BYTES);
        if (tmp == NULL) return DNML_ALLOC_OOM;
        x->limbs = tmp; x->cap = 1;
        x->n = 0; x->sign = 1;
        return STR_SUCCESS;
    }

    //* ======= 4. Instatiating Temporary BigInt ======= *//
    dnml_status err_check; size_t d = strlen(&str[curr_pos]);
    size_t bits = __BITCOUNT___(d, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
    size_t tmp_mark = arena_mark(_DASI_STR_INIT_ARENA);
    limb_t *tmp_limbs = arena_galloc(_DASI_STR_INIT_ARENA, cap * U64_BYTES, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    bigInt __TEMPHOLDER__ = { .limbs = tmp_limbs, .cap = cap, .n = 0, .sign = sign };

    //* ============= 5. Parsing and Initiating Value ================ *//
    for (; curr_pos < d; ++curr_pos) {
        uint8_t lookup_index = (uint8_t)(str[curr_pos] - '\0');
        uint8_t num_val = (base <= 16) ? 
            _VALUE_LOOKUP_INSEN_[lookup_index] 
          : _VALUE_LOOKUP_SEN_[lookup_index];
        if (num_val > base) {
            arena_reset(_DASI_STR_INIT_ARENA, tmp_mark);
            return STR_INVALID_DIGIT;
        }
        __BIGINT_INTERNAL_MUL_UI64__(&__TEMPHOLDER__, base);
        __BIGINT_INTERNAL_ADD_UI64__(&__TEMPHOLDER__, num_val);
    }

    //* =========== 6. FULLY Initializing and Copy-over ============== *//
    x->limbs = malloc(cap * U64_BYTES);
    if (!x->limbs) { arena_reset(_DASI_STR_INIT_ARENA, tmp_mark); abort(); }
    memcpy(x->limbs, tmp_limbs, cap * U64_BYTES);
    x->cap  = cap;
    x->n    = __TEMPHOLDER__.n;
    x->sign = sign;
    arena_reset(_DASI_STR_INIT_ARENA, tmp_mark);
    return STR_SUCCESS;
}
dnml_status bigInt_strbinit(bigInt *x, const char* str, const uint8_t base) {
    if (x->limbs) return STR_SUCCESS; // Already initialized
    test_assert(!(str == NULL), str_null, io_cleanup, STR_NULL);
    test_assert(base > 1, str_inval_base, io_cleanup, STR_INVALID_BASE);
    test_assert(!(*str == '\0'), str_empty, io_cleanup, STR_EMPTY);
    dnml_arena* _DASI_BASE_INIT_ARENA = _USE_ARENA();
    test_assert(!(_DASI_BASE_INIT_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    _skip_whitespace(str, strlen(str), &curr_pos);
    if (str[curr_pos] == '-') { sign = -1; ++curr_pos; }
    else if (str[curr_pos] == '+') ++curr_pos;
    if (str[curr_pos] == '\0') return STR_INCOMPLETE;


    //* ====== 2. Leading-Zeros Handling ====== *//
    while (str[curr_pos] == '0' && str[curr_pos] != '\0') ++curr_pos; // Skipping all leading zeros
    // String full of zeros
    if (str[curr_pos] == '\0') {
        if (sign = -1) return STR_INVALID_SIGN; // -0 is INVALID// Empty initialization
        limb_t *tmp = malloc(sizeof(uint64_t));
        if (tmp == NULL) return DNML_ALLOC_OOM;
        x->limbs = tmp; x->cap = 1;
        x->n = 0; x->sign = 1;
        return STR_SUCCESS;
    }

    //* ======= 3. Instatiating Temporary BigInt ======= *//
    dnml_status *err_check; size_t d = strlen(&str[curr_pos]);
    size_t bits = __BITCOUNT___(d, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
    size_t tmp_mark = arena_mark(_DASI_BASE_INIT_ARENA);
    limb_t *tmp_limbs = arena_galloc(_DASI_BASE_INIT_ARENA, cap * U64_BYTES, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    bigInt __TEMPHOLDER__ = { .limbs = tmp_limbs, .cap = cap, .n = 0, .sign = sign };

    //* ============= 4. Parsing and Initiating Value ================ *//
    for (; curr_pos < d; ++curr_pos) {
        uint8_t lookup_index = (uint8_t)(str[curr_pos] - '\0');
        uint8_t num_val = (base <= 16) ? 
            _VALUE_LOOKUP_INSEN_[lookup_index] 
          : _VALUE_LOOKUP_SEN_[lookup_index];
        if (num_val > base) {
            arena_reset(_DASI_BASE_INIT_ARENA, tmp_mark);
            return STR_INVALID_DIGIT;
        }
        __BIGINT_INTERNAL_MUL_UI64__(&__TEMPHOLDER__, base);
        __BIGINT_INTERNAL_ADD_UI64__(&__TEMPHOLDER__, num_val);
    }


    //* =========== 5. FULLY Initializing and Copy-over ============== *//
    limb_t *tmp = malloc(cap * U64_BYTES);
    if (!tmp) { arena_reset(_DASI_BASE_INIT_ARENA, tmp_mark); abort(); }
    x->limbs = tmp;
    memcpy(x->limbs, tmp_limbs, cap * U64_BYTES);
    x->cap  = cap;
    x->n    = cap;
    x->sign = sign;
    arena_reset(_DASI_BASE_INIT_ARENA, tmp_mark);
    return STR_SUCCESS;
}
dnml_status bigInt_strninit(bigInt *x, const char* str, const size_t len) {
    if (x->limbs) return STR_SUCCESS; // Already initialized
    test_assert(!(str == NULL), str_null, io_cleanup, STR_NULL);
    test_assert(!(!len || *str == '\0'), str_empty, io_cleanup, STR_EMPTY);
    dnml_arena* _DASI_STRNLEN_INIT_ARENA = _USE_ARENA();
    test_assert(!(_DASI_STRNLEN_INIT_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    _skip_whitespace(str, len, &curr_pos);
    if (curr_pos == len || str[curr_pos] == '\0') return STR_EMPTY;
    unsigned char sign_op_res = _sign_handle_nlen_(str, &curr_pos, &sign, len);
    if (sign_op_res == 4) return STR_INVALID_SIGN;
    else if (sign_op_res == 3) return STR_INCOMPLETE;

    //* ====== 2. Prefix Handling ====== *//
    uint8_t base = 10;
    unsigned char prefix_op_res = _prefix_handle_nlen_(str, &curr_pos, &base, len);
    if (prefix_op_res == 3) return STR_INCOMPLETE; 
    else if (prefix_op_res == 0) {
        if (sign == -1) return STR_INVALID_SIGN;
        limb_t *tmp = malloc(U64_BYTES);
        if (tmp == NULL) return DNML_ALLOC_OOM;
        x->limbs = tmp; x->cap = 1;
        x->n = 0; x->sign = 1;
        return STR_SUCCESS;
    } else if (prefix_op_res == 2) return STR_INVALID_BASE_PREFIX;
    // The remaining case (prefix_op_res == 1) indicates we have a decimal string with 1+ leading zero


    //* ====== 3. Leading-Zeros Handling ====== *//
    // Skipping all leading zeros
    while (str[curr_pos] == '0' && (curr_pos < len || str[curr_pos] != '\0')) ++curr_pos;
    // String full of zeros
    if (curr_pos == len || str[curr_pos] == '\0') {
        if (sign = -1) return STR_INVALID_SIGN; // -0 is INVALID// Empty initialization
        limb_t *tmp = malloc(U64_BYTES);
        if (tmp == NULL) return DNML_ALLOC_OOM;
        x->limbs = tmp; x->cap = 1;
        x->n = 0; x->sign = 1;
        return STR_SUCCESS;
    }

    //* ======= 4. Instatiating Temporary BigInt ======= *//
    dnml_status err_check; size_t bits = __BITCOUNT___(len - curr_pos + 1, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
    size_t tmp_mark = arena_mark(_DASI_STRNLEN_INIT_ARENA);
    limb_t *tmp_limbs = arena_galloc(_DASI_STRNLEN_INIT_ARENA, cap * U64_BYTES, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    bigInt __TEMPHOLDER__ = { .limbs = tmp_limbs, .cap = cap, .n = 0, .sign = sign };

    //* ============= 5. Parsing and Initiating Value ================ *//
    for (; curr_pos < len; ++curr_pos) {
        uint8_t lookup_index = (uint8_t)(str[curr_pos] - '\0');
        uint8_t num_val = (base <= 16) ? 
            _VALUE_LOOKUP_INSEN_[lookup_index] 
          : _VALUE_LOOKUP_SEN_[lookup_index];
        if (num_val > base) {
            arena_reset(_DASI_STRNLEN_INIT_ARENA, tmp_mark);
            return STR_INVALID_DIGIT;
        }
        __BIGINT_INTERNAL_MUL_UI64__(&__TEMPHOLDER__, base);
        __BIGINT_INTERNAL_ADD_UI64__(&__TEMPHOLDER__, num_val);
    }

    //* =========== 6. FULLY Initializing and Copy-over ============== *//
    x->limbs = malloc(cap * U64_BYTES);
    if (!x->limbs) { arena_reset(_DASI_STRNLEN_INIT_ARENA, tmp_mark); abort(); }
    memcpy(x->limbs, tmp_limbs, cap * U64_BYTES);
    x->cap  = cap;
    x->n    = __TEMPHOLDER__.n;
    x->sign = sign;
    arena_reset(_DASI_STRNLEN_INIT_ARENA, tmp_mark);
    return STR_SUCCESS;
}
dnml_status bigInt_strnbinit(bigInt *x, const char* str, size_t len, uint8_t base) {
    if (x->limbs) return STR_SUCCESS; // Already initialized
    test_assert(!(str == NULL), str_null, io_cleanup, STR_NULL);
    test_assert(base > 1, str_inval_base, io_cleanup, STR_INVALID_BASE);
    test_assert(!(!len || *str == '\0'), str_empty, io_cleanup, STR_EMPTY);
    dnml_arena* _DASI_BASENLEN_INIT_ARENA = _USE_ARENA();
    test_assert(!(_DASI_BASENLEN_INIT_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    _skip_whitespace(str, len, &curr_pos);
    if (curr_pos == len || str[curr_pos] == '\0') return STR_EMPTY;
    if (str[curr_pos] == '-') { sign = -1; ++curr_pos; }
    else if (str[curr_pos] == '+') ++curr_pos;
    if (curr_pos == len || str[curr_pos] == '\0') return STR_INCOMPLETE;

    //* ====== 2. Leading-Zeros Handling ====== *//
    // Skipping all leading zeros
    while (str[curr_pos] == '0' && (curr_pos < len || str[curr_pos] != '\0')) ++curr_pos;
    // String full of zeros
    if (curr_pos == len || str[curr_pos] == '\0') {
        if (sign = -1) return STR_INVALID_SIGN; // -0 is INVALID// Empty initialization
        limb_t *tmp = malloc(sizeof(uint64_t));
        if (tmp == NULL) return DNML_ALLOC_OOM; // OOM
        x->limbs = tmp; x->cap = 1;
        x->n = 0; x->sign = 1;
        return STR_SUCCESS;
    }

    //* ======= 3. Instatiating Temporary BigInt ======= *//
    dnml_status err_check; size_t bits = __BITCOUNT___(len - curr_pos + 1, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
    size_t tmp_mark = arena_mark(_DASI_BASENLEN_INIT_ARENA);
    limb_t *tmp_limbs = arena_galloc(_DASI_BASENLEN_INIT_ARENA, cap * U64_BYTES, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    bigInt __TEMPHOLDER__ = { .limbs = tmp_limbs, .cap = cap, .n = 0, .sign = sign };

    //* ============= 4. Parsing and Initiating Value ================ *//
    for (; curr_pos < len; ++curr_pos) {
        uint8_t lookup_index = (uint8_t)(str[curr_pos] - '\0');
        uint8_t num_val = (base <= 16) ? 
            _VALUE_LOOKUP_INSEN_[lookup_index] 
          : _VALUE_LOOKUP_SEN_[lookup_index];
        if (num_val > base) {
            arena_reset(_DASI_BASENLEN_INIT_ARENA, tmp_mark);
            return STR_INVALID_DIGIT;
        }
        __BIGINT_INTERNAL_MUL_UI64__(&__TEMPHOLDER__, base);
        __BIGINT_INTERNAL_ADD_UI64__(&__TEMPHOLDER__, num_val);
    }


    //* =========== 5. FULLY Initializing and Copy-over ============== *//
    x->limbs = malloc(cap * U64_BYTES);
    if (!x->limbs) { arena_reset(_DASI_BASENLEN_INIT_ARENA, tmp_mark); abort(); }
    memcpy(x->limbs, tmp_limbs, cap * U64_BYTES);
    x->cap = cap; x->n = __TEMPHOLDER__.n; x->sign = sign;
    arena_reset(_DASI_BASENLEN_INIT_ARENA, tmp_mark);
    return STR_SUCCESS;
}




//todo ================================= 2. CONVERSIONS & ASSIGNMENTS ================================= *//
//* -------------------------- String Conversions -------------------------- *//
/* Truncative BigInt --> String */
dnml_status bigInt_tto_str(char* str, const bigInt x, size_t *written) {
    assert(__BIGINT_INTERNAL_VALID__(&x));
    test_assert(!(str == NULL), str_null, io_cleanup, STR_NULL);
    size_t str_length = strlen(str); // Early segfault if no NULL-Terminator found
    /* Special Case - Singular Character */
    if (x.n == 1 && x.limbs[0] <= 9) {
        if (str_length < 1 + ((x.sign == -1) ? 1 : 0)) return STR_INVALID_CAP;  // Need "-0"
        if (x.sign == -1) str[0] = '-';
        str[0 + ((x.sign == -1) ? 1 : 0)] = (char)(x.limbs[0] + '0');
        return STR_SUCCESS;
    }
    /* Normal Case */
    dnml_arena *_DASI_TSET_STRING_ARENA = _USE_ARENA();
    test_assert(!(_DASI_TSET_STRING_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    dnml_status err_check; uint8_t sign_space = (x.sign == -1) ? 1 : 0;
    
    if (str_length <= sign_space) return STR_INVALID_CAP;
    if (sign_space) { str[0] = '-'; (*written)++; }
    size_t digit_needed = __BIGINT_COUNTDB__(&x, 10) + sign_space;
    size_t end = (digit_needed < str_length) ? digit_needed : str_length;
    size_t tmp_mark = arena_mark(_DASI_TSET_STRING_ARENA);
    limb_t *tmp_limbs = arena_galloc(_DASI_TSET_STRING_ARENA, x.n * U64_BYTES, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    memcpy(tmp_limbs, x.limbs, x.n * U64_BYTES);
    bigInt tmp_buf = { .limbs = tmp_limbs, .sign = x.sign, .cap = x.n, .n = x.n, };

    /* Numerical Accumulation Loop */
    for (size_t i = end - 1; i >= sign_space; --i) {
        if (!tmp_buf.n) break;
        uint8_t numeric_value = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_buf, 10);
        str[i] = _DIGIT_INSEN_[numeric_value]; (*written)++;
    }
    arena_reset(_DASI_TSET_STRING_ARENA, tmp_mark);
    if (digit_needed > str_length) return STR_TRUNC_SUCCESS;
    else return STR_SUCCESS;
}
dnml_status bigInt_tto_strb(char* str, const bigInt x, uint8_t base, size_t *written) {
    assert(__BIGINT_INTERNAL_VALID__(&x));
    test_assert(!(str == NULL), str_null, io_cleanup, STR_NULL);
    test_assert(base > 1, str_inval_base, io_cleanup, STR_INVALID_BASE);
    size_t str_length = strlen(str); // Early segfauly if no NULL-Terminator found
    // Special Case - Singular Character
    if (x.n == 1 && x.limbs[0] <= base - 1) {
        if (str_length < 1 + ((x.sign == -1) ? 1 : 0)) {
            return STR_INVALID_CAP;  // Need "-0"
        }
        if (x.sign == -1) str[0] = '-';
        str[0 + ((x.sign == -1) ? 1 : 0)] = _DIGIT_SEN_[x.limbs[0]];
        return STR_SUCCESS;
    }
    // Normal Case
    dnml_arena *_DASI_TSET_BASE_ARENA = _USE_ARENA();
    test_assert(!(_DASI_TSET_BASE_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    dnml_status err_check; uint8_t sign_space = (x.sign == -1) ? 1 : 0;
    
    if (str_length <= sign_space) return STR_INVALID_CAP;
    if (sign_space) { str[0] = '-'; (*written)++; }
    size_t digit_needed = __BIGINT_COUNTDB__(&x, base) + sign_space;
    size_t end = (digit_needed < str_length) ? digit_needed : str_length;
    size_t tmp_mark = arena_mark(_DASI_TSET_BASE_ARENA);
    limb_t *tmp_limbs = arena_galloc(_DASI_TSET_BASE_ARENA, x.n * U64_BYTES, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    memcpy(tmp_limbs, x.limbs, x.n * U64_BYTES);
    bigInt tmp_buf = { .limbs = tmp_limbs, .sign = x.sign, .cap = x.n, .n = x.n, };

    /* Numerical Accumulation Loop */
    for (size_t i = end - 1; i >= sign_space; --i) {
        uint8_t numeric_value = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_buf, base);
        str[i] = (base <= 16) ? _DIGIT_INSEN_[numeric_value] : 
        _DIGIT_SEN_[numeric_value]; (*written)++;
    }
    arena_reset(_DASI_TSET_BASE_ARENA, tmp_mark);
    if (digit_needed > str_length) return STR_TRUNC_SUCCESS;
    else return STR_SUCCESS;
}
dnml_status bigInt_tto_strn(char* str, size_t len, const bigInt x, size_t *written) {
    assert(__BIGINT_INTERNAL_VALID__(&x));
    test_assert(!(str == NULL), str_null, io_cleanup, STR_NULL);
    /* Special Case - Singular Character */
    if (x.n == 1 && x.limbs[0] <= 9) {
        if (len < 1 + ((x.sign == -1) ? 1 : 0)) return STR_INVALID_CAP;  // Need "-0"
        if (x.sign == -1) str[0] = '-';
        str[0 + ((x.sign == -1) ? 1 : 0)] = (char)(x.limbs[0] + '0');
        return STR_SUCCESS;
    }
    /* Normal Case */
    dnml_arena *_DASI_TSET_STRNLEN_ARENA = _USE_ARENA();
    test_assert(!(_DASI_TSET_STRNLEN_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    dnml_status err_check; uint8_t sign_space = (x.sign == -1) ? 1 : 0;

    if (len <= sign_space) return STR_INVALID_CAP;
    if (sign_space) { str[0] = '-'; (*written)++; }
    size_t digit_needed = __BIGINT_COUNTDB__(&x, 10) + sign_space;
    size_t end = (digit_needed < len) ? digit_needed : len;
    size_t tmp_mark = arena_mark(_DASI_TSET_STRNLEN_ARENA);
    limb_t *tmp_limbs = arena_galloc(_DASI_TSET_STRNLEN_ARENA, x.n * U64_BYTES, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    memcpy(tmp_limbs, x.limbs, x.n * U64_BYTES);
    bigInt tmp_buf = { .limbs = tmp_limbs, .sign = x.sign, .cap = x.n, .n = x.n };

    /* Main Accumulation Loop */
    for (size_t i = end - 1; i >= sign_space; --i) {
        if (!tmp_buf.n) break;
        uint8_t numeric_value = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_buf, 10);
        str[i] = _DIGIT_INSEN_[numeric_value]; (*written)++;
    }
    arena_reset(_DASI_TSET_STRNLEN_ARENA, tmp_mark);
    if (digit_needed > len) return STR_TRUNC_SUCCESS;
    else return STR_SUCCESS;
}
dnml_status bigInt_tto_strnb(char* str, size_t len, const bigInt x, uint8_t base, size_t *written) {
    assert(__BIGINT_INTERNAL_VALID__(&x));
    test_assert(!(str == NULL), str_null, io_cleanup, STR_NULL);
    test_assert(base > 1, str_inval_base, io_cleanup, STR_INVALID_BASE);
    /* Special Case - Singular Character */
    if (x.n == 1 && x.limbs[0] <= base - 1) {
        if (len < 1 + ((x.sign == -1) ? 1 : 0)) return STR_INVALID_CAP;  // Need "-0"
        if (x.sign == -1) str[0] = '-';
        str[0 + ((x.sign == -1) ? 1 : 0)] = _DIGIT_SEN_[x.limbs[0]];
        return STR_SUCCESS;
    }
    /* Normal Case */
    dnml_arena *_DASI_TSET_BASENLEN_ARENA = _USE_ARENA();
    test_assert(!(_DASI_TSET_BASENLEN_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    dnml_status err_check; uint8_t sign_space = (x.sign == -1) ? 1 : 0;

    if (len <= sign_space) return STR_INVALID_CAP;
    if (sign_space) { str[0] = '-'; (*written)++; }
    size_t digit_needed = __BIGINT_COUNTDB__(&x, base) + sign_space;
    size_t end = (digit_needed < len) ? digit_needed : len;
    size_t tmp_mark = arena_mark(_DASI_TSET_BASENLEN_ARENA);
    limb_t *tmp_limbs = arena_galloc(_DASI_TSET_BASENLEN_ARENA, x.n * U64_BYTES, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    memcpy(tmp_limbs, x.limbs, x.n * U64_BYTES);
    bigInt tmp_buf = {
        .limbs = tmp_limbs, .sign = x.sign,
        .cap   = x.n,       .n    = x.n,
    };

    /* Numerical Accumulation Loop */
    for (size_t i = end - 1; i >= sign_space; --i) {
        if (!tmp_buf.n) break;
        uint8_t numeric_value = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_buf, base);
        str[i] = (base <= 16) ? _DIGIT_INSEN_[numeric_value] : 
        _DIGIT_SEN_[numeric_value]; (*written)++;
    }
    arena_reset(_DASI_TSET_BASENLEN_ARENA, tmp_mark);
    if (digit_needed > len) return STR_TRUNC_SUCCESS;
    else return STR_SUCCESS;
}
dnml_status bigInt_tto_strf(
    char* str, size_t len, 
    const bigInt x, uint8_t base, 
    bool uppercase, size_t *written
) {
    /* Pre-operation validation & Static Analysis */
    assert(__BIGINT_INTERNAL_VALID__(&x));
    test_assert(!(str == NULL), str_null, io_cleanup, STR_NULL);
    test_assert(base > 1, str_inval_base, io_cleanup, STR_INVALID_BASE);

    /* Main Operation */
    dnml_arena *_DASI_TSET_BASENLEN_ARENA = _USE_ARENA();
    test_assert(!(_DASI_TSET_BASENLEN_ARENA), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    uint8_t sign_space = (x.sign == -1) ? 1 : 0,
    prefix_space = (base == 2 || base == 8 || base == 16) ? 2 : (
        (base < 10) ? 4 : ((base < 100) ? 5 : 6)
    ), prefix_add = (uppercase) ? 32 : 0, char_add = (uppercase) ? 16 : 0;
    if (len <= sign_space + prefix_space) return STR_INVALID_CAP;
    if (sign_space) { str[0] = '-'; }
    if (prefix_space) { str[0 + sign_space] = '0';
        switch (base) {
            case 2:  str[1 + sign_space] = 'b' + prefix_add; break;
            case 8:  str[1 + sign_space] = 'o' + prefix_add; break;
            case 16: str[1 + sign_space] = 'x' + prefix_add; break;
            case 64: str[1 + sign_space] = ','; break;
            default: { uint8_t tmp = base, i = 1;
                str[i + sign_space] = '{'; ++i;
                for (; i < 5 && tmp; i++) {
                    str[i + sign_space] = (char)((tmp % 10) - '0'); 
                    tmp /= 10;
                } str[i + sign_space] = '}';
            } break;
        }
    } (*written) += sign_space + prefix_space;
    size_t digit_needed = __BIGINT_COUNTDB__(&x, base) + sign_space + prefix_space;
    size_t end = (digit_needed < len) ? digit_needed : len;
    size_t tmp_mark = arena_mark(_DASI_TSET_BASENLEN_ARENA); dnml_status err_check;
    limb_t *tmp_limbs = arena_galloc(_DASI_TSET_BASENLEN_ARENA, x.n * U64_BYTES, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    memcpy(tmp_limbs, x.limbs, x.n * U64_BYTES);
    bigInt tmp_buf = { .limbs = tmp_limbs, .sign = x.sign, .cap = x.n, .n = x.n };

    /* Main Accumulation Loop */
    for (size_t i = end - 1; i >= sign_space; --i) {
        if (!tmp_buf.n) break;
        uint8_t numeric_value = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_buf, base);
        str[i] = (base <= 16) ? _DIGIT_INSEN_[numeric_value + char_add] : 
        _DIGIT_SEN_[numeric_value]; (*written)++;
    }
    arena_reset(_DASI_TSET_BASENLEN_ARENA, tmp_mark);
    if (digit_needed > len) return STR_TRUNC_SUCCESS;
    else return STR_SUCCESS;
}
/* Safe BigInt --> String */
dnml_status bigInt_to_str(char* str, const bigInt x, size_t *written) {
    /* Pre-operation Valdation & Static Analysis */
    assert(__BIGINT_INTERNAL_VALID__(&x));
    test_assert(!(str == NULL), str_null, io_cleanup, STR_NULL);
    size_t str_length = strlen(str);
    /* Special Case - Singular Character */
    if (x.n == 1 && x.limbs[0] <= 9) {
        if (str_length < 1 + ((x.sign == -1) ? 1 : 0)) return STR_INVALID_CAP;  // Need "-0"
        if (x.sign == -1) str[0] = '-';
        str[0 + ((x.sign == -1) ? 1 : 0)] = (char)(x.limbs[0] + '0');
        return STR_SUCCESS;
    }
    /* Normal Case */
    dnml_arena *_DASI_SET_STRING_ARENA = _USE_ARENA();
    test_assert(!(_DASI_SET_STRING_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    dnml_status err_check; uint8_t sign_space = (x.sign == -1) ? 1 : 0;

    if (str_length <= sign_space) return STR_INVALID_CAP;
    size_t digit_needed = __BIGINT_COUNTDB__(&x, 10);
    size_t total_needed = digit_needed + sign_space;
    if (str_length < total_needed) return STR_INVALID_CAP;
    if (sign_space) { str[0] = '-'; (*written)++; }
    size_t tmp_mark = arena_mark(&_DASI_SET_STRING_ARENA);
    limb_t *tmp_limbs = arena_galloc(&_DASI_SET_STRING_ARENA, x.n * U64_BYTES, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    memcpy(tmp_limbs, x.limbs, x.n * U64_BYTES);
    bigInt tmp_buf = { .limbs = tmp_limbs, .sign = x.sign, .cap = x.n, .n = x.n };

    /* Numerical Accumulation Loop */
    for (size_t i = total_needed - 1; i >= sign_space; --i) {
        uint8_t numeric_value = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_buf, 10);
        str[i] = _DIGIT_INSEN_[numeric_value]; (*written)++;
    }  arena_reset(&_DASI_SET_STRING_ARENA, tmp_mark); return STR_SUCCESS;
}
dnml_status bigInt_to_strb(char* str, const bigInt x, uint8_t base, size_t *written) {
    /* Pre-operation Validation & Static Analysis */
    assert(__BIGINT_INTERNAL_VALID__(&x));
    test_assert(!(str == NULL), str_null, io_cleanup, STR_NULL);
    test_assert(base > 1, str_inval_base, io_cleanup, STR_INVALID_BASE);
    size_t str_length = strlen(str);
    /* Special Case - Singular Character */
    if (x.n == 1 && x.limbs[0] <= base - 1) {
        if (str_length < 1 + ((x.sign == -1) ? 1 : 0)) return STR_INVALID_CAP;  // Need "-0"
        if (x.sign == -1) str[0] = '-';
        str[0 + ((x.sign == -1) ? 1 : 0)] = _DIGIT_SEN_[x.limbs[0]];
        return STR_SUCCESS;
    }
    /* Normal Case */
    dnml_arena *_DASI_SET_BASE_ARENA = _USE_ARENA();
    test_assert(!(_DASI_SET_BASE_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    dnml_status err_check; uint8_t sign_space = (x.sign == -1) ? 1 : 0;

    if (str_length <= sign_space) return STR_INVALID_CAP;
    size_t digit_needed = __BIGINT_COUNTDB__(&x, base);
    size_t total_needed = digit_needed + sign_space;
    if (str_length < total_needed) return STR_INVALID_CAP;
    if (sign_space) { str[0] = '-'; (*written)++; }
    size_t tmp_mark = arena_mark(_DASI_SET_BASE_ARENA);
    limb_t *tmp_limbs = arena_galloc(_DASI_SET_BASE_ARENA, x.n * U64_BYTES, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    memcpy(tmp_limbs, x.limbs, x.n * U64_BYTES);
    bigInt tmp_buf = { .limbs = tmp_limbs, .sign = x.sign, .cap = x.n, .n = x.n, };

    /* Numerical Accumulation Loop */
    for (size_t i = total_needed - 1; i >= sign_space; --i) {
        uint8_t numeric_value = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_buf, base);
        str[i] = (base <= 16) ? _DIGIT_INSEN_[numeric_value] : 
        _DIGIT_SEN_[numeric_value]; (*written)++;
    } arena_reset(_DASI_SET_BASE_ARENA, tmp_mark); return STR_SUCCESS;
}
dnml_status bigInt_to_strn(char* str, size_t len, const bigInt x, size_t *written) {
    /* Pre-operation Validation & Static Analysis */
    assert(__BIGINT_INTERNAL_VALID__(&x));
    test_assert(!(str == NULL), str_null, io_cleanup, STR_NULL);
    /* Special Case - Singular Character */
    if (x.n == 1 && x.limbs[0] <= 9) {
        if (len < 1 + ((x.sign == -1) ? 1 : 0)) return STR_INVALID_CAP;  // Need "-0\0"
        if (x.sign == -1) str[0] = '-';
        str[0 + ((x.sign == -1) ? 1 : 0)] = (char)(x.limbs[0] + '0');
        return STR_SUCCESS;
    }
    /* Normal Case */
    dnml_arena *_DASI_SET_STRNLEN_ARENA = _USE_ARENA();
    test_assert(!(_DASI_SET_STRNLEN_ARENA), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    dnml_status err_check; uint8_t sign_space = (x.sign == -1) ? 1 : 0;

    if (len <= sign_space) return STR_INVALID_CAP;
    size_t digit_needed = __BIGINT_COUNTDB__(&x, 10);
    size_t total_needed = digit_needed + sign_space;
    if (len < total_needed) return STR_INVALID_CAP;
    if (sign_space) { str[0] = '-'; (*written)++; }
    size_t tmp_mark = arena_mark(_DASI_SET_STRNLEN_ARENA);
    limb_t *tmp_limbs = arena_galloc(_DASI_SET_STRNLEN_ARENA, x.n * U64_BYTES, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    memcpy(tmp_limbs, x.limbs, x.n * U64_BYTES);
    bigInt tmp_buf = { .limbs = tmp_limbs, .sign = x.sign, .cap = x.n, .n = x.n, };

    /* Numerical Accumulation Loop */
    for (size_t i = total_needed - 1; i >= sign_space; --i) {
        uint8_t numeric_value = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_buf, 10);
        str[i] = _DIGIT_INSEN_[numeric_value]; (*written)++;
    }
    arena_reset(_DASI_SET_STRNLEN_ARENA, tmp_mark); return STR_SUCCESS;
}
dnml_status bigInt_to_strnb(char* str, size_t len, const bigInt x, uint8_t base, size_t *written) {
    assert(__BIGINT_INTERNAL_VALID__(&x));
    test_assert(!(str == NULL), str_null, io_cleanup, STR_NULL);
    test_assert(base > 1, str_inval_base, io_cleanup, STR_INVALID_BASE);
    // Special Case - Singular Character
    if (x.n == 1 && x.limbs[0] <= base - 1) {
        if (len < 1 + ((x.sign == -1) ? 1 : 0)) {
            return STR_INVALID_CAP;  // Need "-0\0"
        } 
        if (x.sign == -1) str[0] = '-';
        str[0 + ((x.sign == -1) ? 1 : 0)] = _DIGIT_SEN_[x.limbs[0]];
        return STR_SUCCESS;
    }
    /* Normal Case */
    dnml_arena *_DASI_SET_BASENLEN_ARENA = _USE_ARENA();
    test_assert(!(_DASI_SET_BASENLEN_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    dnml_status err_check; uint8_t sign_space = (x.sign == -1) ? 1 : 0;

    if (len <= sign_space) return STR_INVALID_CAP;
    size_t digit_needed = __BIGINT_COUNTDB__(&x, base);
    size_t total_needed = digit_needed + sign_space;
    if (len < total_needed) return STR_INVALID_CAP;
    if (sign_space) { str[0] = '-'; (*written)++; }
    size_t tmp_mark = arena_mark(_DASI_SET_BASENLEN_ARENA);
    limb_t *tmp_limbs = arena_galloc(_DASI_SET_BASENLEN_ARENA, x.n * U64_BYTES, &err_check);
    memcpy(tmp_limbs, x.limbs, x.n * U64_BYTES);
    bigInt tmp_buf = { .limbs = tmp_limbs, .sign = x.sign, .cap = x.n, .n = x.n, };

    /* Numerical Accumulation Loop */
    for (size_t i = total_needed - 1; i >= sign_space; --i) {
        uint8_t numeric_value = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_buf, base);
        str[i] = (base <= 16) ? _DIGIT_INSEN_[numeric_value] : 
        _DIGIT_SEN_[numeric_value]; (*written)++;
    }
    arena_reset(_DASI_SET_BASENLEN_ARENA, tmp_mark); return STR_SUCCESS;
}
dnml_status bigInt_to_strf(
    char* str, size_t len, 
    const bigInt x, uint8_t base, 
    bool uppercase, size_t *written
) {
    assert(__BIGINT_INTERNAL_VALID__(&x));
    test_assert(!(str == NULL), str_null, io_cleanup, STR_NULL);
    test_assert(base > 1, str_inval_base, io_cleanup, STR_INVALID_BASE);

    /* Normal Case */
    dnml_arena *_DASI_SET_BASENLEN_ARENA = _USE_ARENA();
    test_assert(!(_DASI_SET_BASENLEN_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    uint8_t sign_space = (x.sign == -1) ? 1 : 0,
    prefix_space = (base == 2 || base == 8 || base == 16) ? 2 : (
        (base < 10) ? 4 : ((base < 100) ? 5 : 6)
    ), prefix_add = (uppercase) ? 32 : 0, char_add = (uppercase) ? 16 : 0;
    if (len <= sign_space + prefix_space) return STR_INVALID_CAP;
    size_t digit_needed = __BIGINT_COUNTDB__(&x, base);
    size_t total_needed = digit_needed + sign_space + prefix_space;
    if (len < total_needed) return STR_INVALID_CAP;
    if (sign_space) str[0] = '-';
    if (prefix_space) { str[0 + sign_space] = '0';
        switch (base) {
            case 2:  str[1 + sign_space] = 'b' + prefix_add; break;
            case 8:  str[1 + sign_space] = 'o' + prefix_add; break;
            case 16: str[1 + sign_space] = 'x' + prefix_add; break;
            case 64: str[1 + sign_space] = ','; break;
            default: { uint8_t tmp = base, i = 1;
                str[i + sign_space] = '{'; ++i;
                for (; i < 5 && tmp; i++) {
                    str[i + sign_space] = (char)((tmp % 10) - '0'); 
                    tmp /= 10;
                } str[i + sign_space] = '}';
            } break;
        }
    } (*written) += sign_space + prefix_space;
    size_t tmp_mark = arena_mark(_DASI_SET_BASENLEN_ARENA); dnml_status err_check;
    limb_t *tmp_limbs = arena_galloc(_DASI_SET_BASENLEN_ARENA, x.n * U64_BYTES, &err_check);
    memcpy(tmp_limbs, x.limbs, x.n * U64_BYTES);
    bigInt tmp_buf = { .limbs = tmp_limbs, .sign = x.sign, .cap = x.n, .n = x.n};
    
    /* Numerical Accumulation Loop */
    for (size_t i = total_needed - 1; i >= sign_space + prefix_space; --i) {
        uint8_t numeric_value = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_buf, base);
        str[i] = (base <= 16) ? _DIGIT_INSEN_[numeric_value + char_add] : 
        _DIGIT_SEN_[numeric_value]; (*written)++;
    }
    arena_reset(_DASI_SET_BASENLEN_ARENA, tmp_mark); return STR_SUCCESS;
}
//* -------------------------- BigInt Conversions -------------------------- *//
bigInt bigInt_from_str(const char* str, dnml_status *err) {
    assert(err); 
    test_assert_mut((str == NULL), str_null, io_cleanup, err, STR_NULL, (__BIGINT_ERROR_VALUE__()));
    test_assert_mut((*str == '\0'), str_empty, io_cleanup, err, STR_EMPTY, (__BIGINT_ERROR_VALUE__()));
    bigInt res;
    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    _skip_whitespace(str, strlen(str), &curr_pos);
    if (str[curr_pos] == '\0') { *err = STR_EMPTY; return __BIGINT_ERROR_VALUE__(); }
    unsigned char sign_op_res = _sign_handle_(str, &curr_pos, &sign);
    if (sign_op_res == 4) { *err = STR_INVALID_DIGIT; return __BIGINT_ERROR_VALUE__(); }
    else if (sign_op_res == 3) { *err = STR_INCOMPLETE; return __BIGINT_ERROR_VALUE__(); }

    //* ====== 2. Prefix Handling ====== *//
    uint8_t base = 10;
    unsigned char prefix_op_res = _prefix_handle_(str, &curr_pos, &base);
    if (prefix_op_res == 3) { *err = STR_INCOMPLETE; return __BIGINT_ERROR_VALUE__(); }
    else if (prefix_op_res == 0) {
        if (sign == -1) { *err = STR_INVALID_SIGN; return __BIGINT_ERROR_VALUE__(); }
        limb_t *tmp = malloc(U64_BYTES);
        if (tmp == NULL) { *err = DNML_ALLOC_OOM; return __BIGINT_ERROR_VALUE__(); }
        res.limbs = tmp; res.cap  = 1; 
        res.n = 0; res.sign = 1;
        *err = STR_SUCCESS; return res;
    } else if (prefix_op_res == 2) { *err = STR_INVALID_BASE_PREFIX; return __BIGINT_ERROR_VALUE__(); }
    else if (prefix_op_res == 4) { *err = STR_INVALID_BASE; return __BIGINT_ERROR_VALUE__(); }
    // The remaining case (prefix_op_res == 1) indicates we have a decimal string with 1+ leading zero

    //* ====== 3. Leading-Zeros Handling ====== *//
    while (str[curr_pos] == '0' && str[curr_pos] != '\0') ++curr_pos; // Skipping all leading zeros
    // String full of zeros
    if (str[curr_pos] == '\0') {
        // -0 is INVALID
        if (sign = -1) { *err = STR_INVALID_SIGN; return __BIGINT_ERROR_VALUE__(); };
        limb_t *tmp = malloc(U64_BYTES);
        if (tmp == NULL) { *err = DNML_ALLOC_OOM; return __BIGINT_ERROR_VALUE__(); }
        res.limbs = tmp; res.cap = 1; 
        res.n = 0; res.sign  = 1;
        *err = STR_SUCCESS; return res;
    }

    //* ======= 4. Initiating Resulting BigInt ======= *//
    size_t d = strlen(&str[curr_pos]);
    size_t bits = __BITCOUNT___(d, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
    limb_t *tmp = malloc(U64_BYTES * cap);
    if (tmp == NULL) { *err = DNML_ALLOC_OOM; return __BIGINT_ERROR_VALUE__(); }
    res.limbs = tmp; res.sign = sign;
    res.cap   = cap; res.n = cap;

    //* ============= 5. Parsing and Initiating Value ================ *//
    for (size_t i = d - 1; i >= curr_pos; --i) {
        uint8_t lookup_index = (uint8_t)(str[i] - '\0');
        uint8_t num_val = (base <= 16) ? 
            _VALUE_LOOKUP_INSEN_[lookup_index] 
          : _VALUE_LOOKUP_SEN_[lookup_index];
        if (num_val > base) {
            *err = STR_INVALID_DIGIT;
            free(res.limbs); return __BIGINT_ERROR_VALUE__();
        }
        __BIGINT_INTERNAL_MUL_UI64__(&res, base);
        __BIGINT_INTERNAL_ADD_UI64__(&res, num_val);
    } return res;
}
bigInt bigInt_from_strb(const char* str, uint8_t base, dnml_status *err) {
    assert(err); 
    test_assert_mut((str == NULL), str_null, io_cleanup, err, STR_NULL, (__BIGINT_ERROR_VALUE__()));
    test_assert_mut((!base), str_inval_base, io_cleanup, err, STR_INVALID_BASE, (__BIGINT_ERROR_VALUE__()));
    test_assert_mut((*str == '\0'), str_empty, io_cleanup, err, STR_EMPTY, (__BIGINT_ERROR_VALUE__()));
    bigInt res;
    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    _skip_whitespace(str, strlen(str), &curr_pos);
    if (str[curr_pos] == '\0') { *err = STR_EMPTY; return __BIGINT_ERROR_VALUE__(); }
    if (str[curr_pos] == '-') { sign = -1; ++curr_pos; }
    else if (str[curr_pos] == '+') ++curr_pos;
    if (str[curr_pos] == '\0') { *err = STR_INCOMPLETE; return __BIGINT_ERROR_VALUE__(); }

    //* ====== 2. Leading-Zeros Handling ====== *//
    while (str[curr_pos] == '0' && str[curr_pos] != '\0') ++curr_pos; // Skipping all leading zeros
    // String full of zeros
    if (str[curr_pos] == '\0') {
        // -0 is INVALID
        if (sign = -1) { *err = STR_INVALID_SIGN; return __BIGINT_ERROR_VALUE__(); };
        limb_t *tmp = malloc(U64_BYTES);
        if (tmp == NULL) { *err = DNML_ALLOC_OOM; return __BIGINT_ERROR_VALUE__(); }
        res.limbs = tmp; res.cap = 1;
        res.n = 0; res.sign = 1;
        *err = STR_SUCCESS; return res;
    }

    //* ======= 3. Initiating Resulting BigInt ======= *//
    size_t d = strlen(&str[curr_pos]);
    size_t bits = __BITCOUNT___(d, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
    limb_t *tmp = malloc(U64_BYTES * cap);
    if (tmp == NULL) { *err = DNML_ALLOC_OOM; return __BIGINT_ERROR_VALUE__(); }
    res.limbs = tmp; res.sign = sign;
    res.cap = cap; res.n = cap;

    //* ============= 4. Parsing and Initiating Value ================ *//
    for (size_t i = d - 1; i >= curr_pos; --i) {
        uint8_t lookup_index = (uint8_t)(str[curr_pos] - '\0');
        uint8_t num_val = (base <= 16) ? 
            _VALUE_LOOKUP_INSEN_[lookup_index] 
          : _VALUE_LOOKUP_SEN_[lookup_index];
        if (num_val > base) {
            *err = STR_INVALID_DIGIT;
            free(res.limbs); return __BIGINT_ERROR_VALUE__();
        }
        __BIGINT_INTERNAL_MUL_UI64__(&res, base);
        __BIGINT_INTERNAL_ADD_UI64__(&res, num_val);
    } return res;
}
bigInt bigInt_from_strn(const char* str, size_t len, dnml_status *err) {
    assert(err); test_assert_mut((str == NULL), str_null, io_cleanup, err, STR_NULL, (__BIGINT_ERROR_VALUE__()));
    test_assert_mut((!len || *str == '\0'), str_empty, io_cleanup, err, STR_EMPTY, (__BIGINT_ERROR_VALUE__()));
    bigInt res;
    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    _skip_whitespace(str, len, &curr_pos);
    if (curr_pos == len || str[curr_pos] == '\0') { 
        *err = STR_EMPTY;
        return __BIGINT_ERROR_VALUE__(); 
    } unsigned char sign_op_res = _sign_handle_nlen_(str, &curr_pos, &sign, len);
    if (sign_op_res == 4) { *err = STR_INVALID_DIGIT; return __BIGINT_ERROR_VALUE__(); }
    else if (sign_op_res == 3) { *err = STR_INCOMPLETE; return __BIGINT_ERROR_VALUE__(); }

    //* ====== 2. Prefix Handling ====== *//
    uint8_t base = 10;
    unsigned char prefix_op_res = _prefix_handle_nlen_(str, &curr_pos, &base, len);
    if (prefix_op_res == 3) { *err = STR_INCOMPLETE; return __BIGINT_ERROR_VALUE__(); }
    else if (prefix_op_res == 0) {
        if (sign == -1) { *err = STR_INVALID_SIGN; return __BIGINT_ERROR_VALUE__(); }
        limb_t *tmp = malloc(U64_BYTES);
        if (tmp == NULL) { *err = DNML_ALLOC_OOM; return __BIGINT_ERROR_VALUE__(); }
        res.limbs = tmp; res.cap = 1;
        res.n = 0; res.sign = 1;
        *err = STR_SUCCESS; return res; 
    } else if (prefix_op_res == 2) { *err = STR_INVALID_BASE_PREFIX; return __BIGINT_ERROR_VALUE__(); }
    else if (prefix_op_res == 4) { *err = STR_INVALID_BASE; return __BIGINT_ERROR_VALUE__(); }
    // The remaining case (prefix_op_res == 1) indicates we have a decimal string with 1+ leading zero

    //* ====== 3. Leading-Zeros Handling ====== *//
    // Skipping all leading zeros
    while (str[curr_pos] == '0' && (curr_pos < len || str[curr_pos] != '\0')) ++curr_pos;
    // String full of zeros
    if (curr_pos == len || str[curr_pos] == '\0') {
        // -0 is INVALID
        if (sign = -1) { *err = STR_INVALID_SIGN; return __BIGINT_ERROR_VALUE__(); }
        // Empty initialization
        limb_t *tmp = malloc(U64_BYTES);
        if (tmp == NULL) { *err = DNML_ALLOC_OOM; return __BIGINT_ERROR_VALUE__(); }
        res.limbs = tmp; res.cap = 1;
        res.n = 0; res.sign = 1;
        *err = STR_SUCCESS; return res;
    }

    //* ======= 4. Initiating Resulting BigInt ======= *//
    size_t true_len = 0; _actual_len(str, len, &true_len);
    size_t bits = __BITCOUNT___(true_len, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
    limb_t *tmp = malloc(U64_BYTES * cap);
    if (tmp == NULL) { *err = DNML_ALLOC_OOM; return __BIGINT_ERROR_VALUE__(); }
    res.limbs = tmp; res.sign = sign;
    res.cap = cap; res.n = cap;

    //* ============= 5. Parsing and Initiating Value ================ *//
    for (size_t i = true_len - 1; i >= curr_pos; --i) {
        uint8_t lookup_index = (uint8_t)(str[curr_pos] - '\0');
        uint8_t num_val = (base <= 16) ? 
            _VALUE_LOOKUP_INSEN_[lookup_index] 
          : _VALUE_LOOKUP_SEN_[lookup_index];
        if (num_val > base) {
            *err = STR_INVALID_DIGIT;
            free(res.limbs); return __BIGINT_ERROR_VALUE__();
        }
        __BIGINT_INTERNAL_MUL_UI64__(&res, base);
        __BIGINT_INTERNAL_ADD_UI64__(&res, num_val);
    } return res;
}
bigInt bigInt_from_strnb(const char* str, size_t len, uint8_t base, dnml_status *err) {
    assert(err); 
    test_assert_mut((str == NULL), str_null, io_cleanup, err, STR_NULL, (__BIGINT_ERROR_VALUE__()));
    test_assert_mut((!base), str_inval_base, io_cleanup, err, STR_INVALID_BASE, (__BIGINT_ERROR_VALUE__()));
    test_assert_mut((!len || *str == '\0'), str_empty, io_cleanup, err, STR_EMPTY, (__BIGINT_ERROR_VALUE__()));
    bigInt res;
    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    _skip_whitespace(str, len, &curr_pos);
    if (curr_pos == len || str[curr_pos] == '\0') { 
        *err = STR_EMPTY;
        return __BIGINT_ERROR_VALUE__(); 
    } if (str[curr_pos] == '-') { sign = -1; ++curr_pos; }
    else if (str[curr_pos] == '+') ++curr_pos;
    if (curr_pos == len - 1 || str[curr_pos] == '\0') { 
        *err = STR_INCOMPLETE; 
        return __BIGINT_ERROR_VALUE__(); 
    }

    //* ====== 2. Leading-Zeros Handling ====== *//
    // Skipping all leading zeros
    while (str[curr_pos] == '0' && (curr_pos < len || str[curr_pos] != '\0')) ++curr_pos;
    // String full of zeros
    if (curr_pos == len || str[curr_pos] == '\0') {
        // -0 is INVALID
        if (sign = -1) { *err = STR_INVALID_SIGN; return __BIGINT_ERROR_VALUE__(); }
        // Empty initialization
        limb_t *tmp = malloc(U64_BYTES);
        if (tmp == NULL) { *err = DNML_ALLOC_OOM; return __BIGINT_ERROR_VALUE__(); }
        res.limbs = tmp; res.cap = 1;
        res.n = 0; res.sign = 1;
        *err = STR_SUCCESS; return res;
    }

    //* ======= 3. Initiating Resulting BigInt ======= *//
    size_t true_len = 0; _actual_len(str, len, &true_len);
    size_t bits = __BITCOUNT___(true_len, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
    limb_t *tmp = malloc(U64_BYTES * cap);
    if (tmp == NULL) { *err = DNML_ALLOC_OOM; return __BIGINT_ERROR_VALUE__(); }
    res.limbs = tmp; res.sign = sign;
    res.cap   = cap; res.n = cap;

    //* ============= 4. Parsing and Initiating Value ================ *//
    for (size_t i = true_len - 1; i >= curr_pos; --i) {
        uint8_t lookup_index = (uint8_t)(str[curr_pos] - '\0');
        uint8_t num_val = (base <= 16) ? 
            _VALUE_LOOKUP_INSEN_[lookup_index] 
          : _VALUE_LOOKUP_SEN_[lookup_index];
        if (num_val > base) {
            *err = STR_INVALID_DIGIT;
            free(res.limbs); return __BIGINT_ERROR_VALUE__();
        }
        __BIGINT_INTERNAL_MUL_UI64__(&res, base);
        __BIGINT_INTERNAL_ADD_UI64__(&res, num_val);
    } return res;
}
//* -------------------------- BigInt Assignments -------------------------- *//
size_t bigInt_get_size(const char *str, size_t len, uint8_t *baseout, dnml_status *err) {
    test_assert_pre((str == NULL), str_null, io_cleanup, err, STR_NULL, baseout, 10, 0);
    test_assert_pre((!len || *str == '\0'), str_empty, io_cleanup, err, STR_EMPTY, baseout, 10, 0);
    size_t curr_pos = 0, res = 0; uint8_t sign = 1;
    _skip_whitespace(str, len, &curr_pos);
    if (curr_pos == len || str[curr_pos] == '\0') { *err = STR_EMPTY; *baseout = 10; return 0; }
    unsigned char sign_op_res = _sign_handle_nlen_(str, &curr_pos, &sign, len);
    if (sign_op_res == 4) { *err = STR_INVALID_SIGN; *baseout = 10; return res; }
    else if (sign_op_res == 3) { *err = STR_INCOMPLETE; *baseout = 10; return res; }

    //* ====== 2. Prefix Handling ====== *//
    uint8_t base = 10;
    unsigned char prefix_op_res = _prefix_handle_nlen_(str, &curr_pos, &base, len);
    if (prefix_op_res == 3) { *err = STR_INCOMPLETE; *baseout = base; return res; }
    else if (prefix_op_res == 0) {
        if (sign == -1) { *err = STR_INVALID_SIGN; *baseout = base; return res; }
        *err = STR_SUCCESS; *baseout = base; return res;
    } else if (prefix_op_res == 2) { *err = STR_INVALID_BASE_PREFIX; *baseout = base; return res; }
    else if (prefix_op_res == 4) { *err = STR_INVALID_BASE; *baseout = base; return res; }
    // The remaining case (prefix_op_res == 1) indicates we have a decimal string with 1+ leading zero

    //* ====== 3. Leading-Zeros Handling ====== *//
    // Skipping all leading zeros
    while (str[curr_pos] == '0' && (curr_pos < len || str[curr_pos] != '\0')) ++curr_pos;
    // String full of zeros
    if (curr_pos == len || str[curr_pos] == '\0') {
        // -0 is INVALID
        if (sign == -1) { *err = STR_INVALID_SIGN; *baseout = base; return res; }
        { *err = STR_SUCCESS; *baseout = base; return res; }
    }

    //* ====== 4. Numerical Digit Segment Handling ====== *//
    for (size_t i = len - 1; i >= curr_pos; --i) {
        uint8_t lookup_index = (uint8_t)(str[i] - '\0');
        uint8_t num_val = (base <= 16) ? 
            _VALUE_LOOKUP_INSEN_[lookup_index] 
          : _VALUE_LOOKUP_SEN_[lookup_index];
        if (num_val > base) { 
            *err = STR_INVALID_DIGIT; *baseout = base; return res; 
        } ++res;
    } *err = STR_SUCCESS; *baseout = base; return res;
} 
size_t bigInt_get_sizeb(const char *str, size_t len, uint8_t base, dnml_status *err) {
    test_assert_mut((str == NULL), str_null, io_cleanup, err, STR_NULL, 0);
    test_assert_mut((!base), str_inval_base, io_cleanup, err, STR_INVALID_BASE, 0);
    test_assert_mut((!len || *str == '\0'), str_empty, io_cleanup, err, STR_EMPTY, 0);
    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0, res = 0; uint8_t sign = 1;
    _skip_whitespace(str, len, &curr_pos);
    if (curr_pos == len || str[curr_pos] == '\0') { *err = STR_EMPTY; return 0; }
    if (str[curr_pos] == '-') { sign = -1; ++curr_pos; }
    else if (str[curr_pos] == '+') ++curr_pos;
    if (curr_pos == len || str[curr_pos] == '\0') { *err = STR_INCOMPLETE; return res; }

    //* ====== 2. Leading-Zeros Handling ====== *//
    // Skipping all leading zeros
    while (str[curr_pos] == '0' && (curr_pos < len || str[curr_pos] != '\0')) ++curr_pos;
    // String full of zeros
    if (curr_pos == len || str[curr_pos] == '\0') {
        // -0 is INVALID
        if (sign = -1) { *err = STR_INVALID_SIGN; return res; }
        *err = STR_SUCCESS; return res;
    }

    //* ====== 3. Numerical Digit Segment Handling ====== *//
    for (size_t i = len - 1; i >= curr_pos; --i) {
        uint8_t lookup_index = (uint8_t)(str[i] - '\0');
        uint8_t num_val = (base <= 16) ? 
            _VALUE_LOOKUP_INSEN_[lookup_index] 
          : _VALUE_LOOKUP_SEN_[lookup_index];
        if (num_val > base) { 
            *err = STR_INVALID_DIGIT; return res; 
        } ++res;
    } return res;
}
size_t bigInt_get_sizesa(
    const char *str, size_t len, 
    uint8_t *baseout, size_t bisize, 
    dnml_status *err
) {
    test_assert_pre((str == NULL), str_null, io_cleanup, err, STR_NULL, baseout, 10, 0);
    test_assert_pre((!len || *str == '\0'), str_empty, io_cleanup, err, STR_EMPTY, baseout, 10, 0);
    size_t curr_pos = 0, res = 0; uint8_t sign = 1;
    _skip_whitespace(str, len, &curr_pos);
    if (curr_pos == len || str[curr_pos] == '\0') { *err = STR_EMPTY; *baseout = 10; return 0; }
    unsigned char sign_op_res = _sign_handle_nlen_(str, &curr_pos, &sign, len);
    if (sign_op_res == 4) { *err = STR_INVALID_SIGN; *baseout = 10; return res; }
    else if (sign_op_res == 3) { *err = STR_INCOMPLETE; *baseout = 10; return res; }

    //* ====== 2. Prefix Handling ====== *//
    uint8_t base = 10;
    unsigned char prefix_op_res = _prefix_handle_nlen_(str, &curr_pos, &base, len);
    if (prefix_op_res == 3) { *err = STR_INCOMPLETE; *baseout = base; return res; }
    else if (prefix_op_res == 0) {
        if (sign == -1) { *err = STR_INVALID_SIGN; *baseout = base; return res; }
        { *err = STR_SUCCESS; return 0; }
    } else if (prefix_op_res == 2) { *err = STR_INVALID_BASE_PREFIX; *baseout = base; return res; }
    else if (prefix_op_res == 4) { *err = STR_INVALID_BASE; *baseout = base; return res; }
    // The remaining case (prefix_op_res == 1) indicates we have a decimal string with 1+ leading zero

    //* ====== 3. Leading-Zeros Handling ====== *//
    // Skipping all leading zeros
    while (str[curr_pos] == '0' && (curr_pos < len || str[curr_pos] != '\0')) ++curr_pos;
    // String full of zeros
    if (curr_pos == len || str[curr_pos] == '\0') {
        // -0 is INVALID
        if (sign == -1) { *err = STR_INVALID_SIGN; *baseout = base; return 0; }
        *err = STR_SUCCESS; *baseout = base; return res;
    }

    //* ======= 4. BigInt Size Validity Checking ======= *//
    size_t true_len = 0; _actual_len(str, len, &true_len);
    size_t bits = __BITCOUNT___(true_len - curr_pos, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
    if (bisize < cap) { *err = BIGINT_ERR_RANGE; *baseout = base; return res;}

    //* ====== 5. Numerical Digit Segment Handling ====== *//
    for (size_t i = true_len - 1; i >= curr_pos; --i) {
        uint8_t lookup_index = (uint8_t)(str[i] - '\0');
        uint8_t num_val = (base <= 16) ? 
            _VALUE_LOOKUP_INSEN_[lookup_index] 
          : _VALUE_LOOKUP_SEN_[lookup_index];
        if (num_val > base) { 
            *err = STR_INVALID_DIGIT; *baseout = base; return res; 
        } ++res;
    } *err = STR_SUCCESS; *baseout = base; return res;
}
size_t bigInt_get_sizebsa(
    const char *str, size_t len,
    uint8_t base, size_t bisize, 
    dnml_status *err
) {
    test_assert_mut((str == NULL), str_null, io_cleanup, err, STR_NULL, 0);
    test_assert_mut((!base), str_inval_base, io_cleanup, err, STR_INVALID_BASE, 0);
    test_assert_mut((!len || *str == '\0'), str_empty, io_cleanup, err, STR_EMPTY, 0);
    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0, res = 0; uint8_t sign = 1;
    _skip_whitespace(str, len, &curr_pos);
    if (curr_pos == len || str[curr_pos] == '\0') { *err = STR_EMPTY; return 0; }
    if (str[curr_pos] == '-') { sign = -1; ++curr_pos; }
    else if (str[curr_pos] == '+') ++curr_pos;
    if (curr_pos == len || str[curr_pos] == '\0') { *err = STR_INCOMPLETE; return res; }

    //* ====== 2. Leading-Zeros Handling ====== *//
    // Skipping all leading zeros
    while (str[curr_pos] == '0' && (curr_pos < len || str[curr_pos] != '\0')) ++curr_pos;
    // String full of zeros
    if (curr_pos == len || str[curr_pos] == '\0') {
        // -0 is INVALID
        if (sign = -1) { *err = STR_INVALID_SIGN; return res; }
        *err = STR_SUCCESS; return res;
    }

    //* ======= 3. Initiating Resulting BigInt ======= *//
    size_t true_len = 0; _actual_len(str, len, &true_len);
    size_t bits = __BITCOUNT___(true_len - curr_pos, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
    if (bisize < cap) { *err = BIGINT_ERR_RANGE; return res; }

    //* ====== 3. Numerical Digit Segment Handling ====== *//
    for (size_t i = true_len - 1; i >= curr_pos; --i) {
        uint8_t lookup_index = (uint8_t)(str[i] - '\0');
        uint8_t num_val = (base <= 16) ? 
            _VALUE_LOOKUP_INSEN_[lookup_index] 
          : _VALUE_LOOKUP_SEN_[lookup_index];
        if (num_val > base) { 
            *err = STR_INVALID_DIGIT; return res; 
        } ++res;
    } *err = STR_SUCCESS; return res;
}
/* Default String --> BigInt */ /* Grows */
dnml_status bigInt_get_str(bigInt *x, const char *str) {
    assert(__BIGINT_INTERNAL_PVALID__(x));
    test_assert(!(str == NULL), str_null, io_cleanup, STR_NULL);
    test_assert(!(*str == '\0'), str_empty, io_cleanup, STR_EMPTY);
    dnml_arena *_DASI_GET_STRING_ARENA = _USE_ARENA();
    test_assert(!(_DASI_GET_STRING_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    _skip_whitespace(str, strlen(str), &curr_pos);
    if (str[curr_pos] == '\0') return STR_EMPTY;
    unsigned char sign_op_res = _sign_handle_(str, &curr_pos, &sign);
    if (sign_op_res == 4) return STR_INVALID_DIGIT;
    else if (sign_op_res == 3) return STR_INCOMPLETE;

    //* ====== 2. Prefix Handling ====== *//
    uint8_t base = 10;
    unsigned char prefix_op_res = _prefix_handle_(str, &curr_pos, &base);
    if (prefix_op_res == 3) return STR_INCOMPLETE;
    else if (prefix_op_res == 0) {
        if (sign == -1) return STR_INVALID_SIGN;
        x->n = 0; x->sign = 1; return STR_SUCCESS;
    } else if (prefix_op_res == 2) return STR_INVALID_BASE_PREFIX;
    else if (prefix_op_res == 4) return STR_INVALID_BASE;
    // The remaining case (prefix_op_res == 1) indicates we have a decimal string with 1+ leading zero

    //* ====== 3. Leading-Zeros Handling ====== *//
    while (str[curr_pos] == '0' && str[curr_pos] != '\0') ++curr_pos; // Skipping all leading zeros
    // String full of zeros
    if (str[curr_pos] == '\0') {
        // -0 is INVALID
        if (sign = -1) return STR_INVALID_SIGN;
        x->n = 0; x->sign = 1;
        return STR_SUCCESS;
    }

    //* ======= 4. Initiating Resulting BigInt ======= *//
    dnml_status err_check; size_t d = strlen(&str[curr_pos]);
    size_t bits = __BITCOUNT___(d - curr_pos, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
    if (__BIGINT_INTERNAL_ENSCAP__(x, cap) == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    size_t tmp_mark = arena_mark(_DASI_GET_STRING_ARENA);
    limb_t *tmp_limbs = arena_galloc(_DASI_GET_STRING_ARENA, cap * U64_BYTES, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    bigInt tmp_buf = { .limbs = tmp_limbs, .sign = sign, .cap = cap, .n = 0 };

    //* ============= 5. Parsing and Initiating Value ================ *//
    for (size_t i = d - 1; i >= curr_pos; --i) {
        uint8_t lookup_index = (uint8_t)(str[i] - '\0');
        uint8_t num_val = (base <= 16) ? 
            _VALUE_LOOKUP_INSEN_[lookup_index] 
          : _VALUE_LOOKUP_SEN_[lookup_index];
        if (num_val > base) {
            arena_reset(_DASI_GET_STRING_ARENA, tmp_mark);
            return STR_INVALID_DIGIT;
        }
        __BIGINT_INTERNAL_MUL_UI64__(&tmp_buf, base);
        __BIGINT_INTERNAL_ADD_UI64__(&tmp_buf, num_val);
    }
    __BIGINT_INTERNAL_COPY__(x, &tmp_buf); x->n = cap; // For safety measures
    arena_reset(_DASI_GET_STRING_ARENA, tmp_mark);
    return STR_SUCCESS;
}
dnml_status bigInt_get_strb(bigInt *x, const char *str, uint8_t base) {
    assert(__BIGINT_INTERNAL_PVALID__(x));
    test_assert(!(str == NULL), str_null, io_cleanup, STR_NULL);
    test_assert(base > 1, str_inval_base, io_cleanup, STR_INVALID_BASE);
    test_assert(!(*str == '\0'), str_empty, io_cleanup, STR_EMPTY);
    dnml_arena *_DASI_GET_BASE_ARENA = _USE_ARENA();
    test_assert(!(_DASI_GET_BASE_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    _skip_whitespace(str, strlen(str), &curr_pos);
    if (str[curr_pos] == '\0') return STR_EMPTY;
    if (str[curr_pos] == '-') { sign = -1; ++curr_pos; }
    else if (str[curr_pos] == '+') ++curr_pos;
    if (str[curr_pos] == '\0') return STR_INCOMPLETE;

    //* ====== 2. Leading-Zeros Handling ====== *//
    while (str[curr_pos] == '0' && str[curr_pos] != '\0') ++curr_pos; // Skipping all leading zeros
    // String full of zeros
    if (str[curr_pos] == '\0') {
        // -0 is INVALID
        if (sign = -1) return STR_INVALID_SIGN;
        x->n = 0; x->sign = 1; return STR_SUCCESS;
    }

    //* ======= 3. Initiating Resulting BigInt ======= *//
    dnml_status err_check; size_t d = strlen(&str[curr_pos]);
    size_t bits = __BITCOUNT___(d - curr_pos, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
    if (__BIGINT_INTERNAL_ENSCAP__(x, cap) == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    size_t tmp_mark = arena_mark(_DASI_GET_BASE_ARENA);
    limb_t *tmp_limbs = arena_galloc(_DASI_GET_BASE_ARENA, cap * U64_BYTES, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    bigInt tmp_buf = { .limbs = tmp_limbs, .sign = sign, .cap = cap, .n = 0 };


    //* ============= 4. Parsing and Initiating Value ================ *//
    for (size_t i = d - 1; i >= curr_pos; --i) {
        uint8_t lookup_index = (uint8_t)(str[i] - '\0');
        uint8_t num_val = (base <= 16) ? 
            _VALUE_LOOKUP_INSEN_[lookup_index] 
          : _VALUE_LOOKUP_SEN_[lookup_index];
        if (num_val > base) {
            arena_reset(_DASI_GET_BASE_ARENA, tmp_mark);
            return STR_INVALID_DIGIT;
        }
        __BIGINT_INTERNAL_MUL_UI64__(&tmp_buf, base);
        __BIGINT_INTERNAL_ADD_UI64__(&tmp_buf, num_val);
    }
    __BIGINT_INTERNAL_COPY__(x, &tmp_buf); x->n = cap; // For safety measures
    arena_reset(_DASI_GET_BASE_ARENA, tmp_mark);
    return STR_SUCCESS;
}
dnml_status bigInt_get_strn(bigInt *x, const char *str, size_t len) {
    assert(__BIGINT_INTERNAL_PVALID__(x));
    test_assert(!(str == NULL), str_null, io_cleanup, STR_NULL);
    test_assert(!(!len || *str == '\0'), str_empty, io_cleanup, STR_EMPTY);
    dnml_arena *_DASI_GET_STRNLEN_ARENA = _USE_ARENA();
    test_assert(!(_DASI_GET_STRNLEN_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
     //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    _skip_whitespace(str, len, &curr_pos);
    if (curr_pos == len || str[curr_pos] == '\0') return STR_EMPTY;
    unsigned char sign_op_res = _sign_handle_nlen_(str, &curr_pos, &sign, len);
    if (sign_op_res == 4) return STR_INVALID_DIGIT;
    else if (sign_op_res == 3) return STR_INCOMPLETE;

    //* ====== 2. Prefix Handling ====== *//
    uint8_t base = 10;
    unsigned char prefix_op_res = _prefix_handle_nlen_(str, &curr_pos, &base, len);
    if (prefix_op_res == 3) return STR_INCOMPLETE;
    else if (prefix_op_res == 0) {
        if (sign == -1) return STR_INVALID_SIGN;
        x->n = 0; x->sign = 1; return STR_SUCCESS;
    } else if (prefix_op_res == 2) return STR_INVALID_BASE_PREFIX;
    else if (prefix_op_res == 4) return STR_INVALID_BASE;
    // The remaining case (prefix_op_res == 1) indicates we have a decimal string with 1+ leading zero

    //* ====== 3. Leading-Zeros Handling ====== *//
    // Skipping all leading zeros
    while (str[curr_pos] == '0' && (curr_pos < len || str[curr_pos] != '\0')) ++curr_pos;
    // String full of zeros
    if (curr_pos == len || str[curr_pos] == '\0') {
        // -0 is INVALID
        if (sign == -1) return STR_INVALID_SIGN;
        x->n = 0; x->sign = 1; return STR_SUCCESS;
    }

    //* ======= 4. Initiating Resulting BigInt ======= *//
    dnml_status err_check;
    size_t true_len = 0; _actual_len(str, len, &true_len);
    size_t bits = __BITCOUNT___(true_len - curr_pos, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
    if (__BIGINT_INTERNAL_ENSCAP__(x, cap) == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    size_t tmp_mark = arena_mark(_DASI_GET_STRNLEN_ARENA);
    limb_t *tmp_limbs = arena_galloc(_DASI_GET_STRNLEN_ARENA, cap * U64_BYTES, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    bigInt tmp_buf = { .limbs = tmp_limbs, .sign = sign, .cap = cap, .n = 0 };

    //* ============= 5. Parsing and Initiating Value ================ *//
    for (size_t i = true_len - 1; i >= curr_pos; --i) {
        uint8_t lookup_index = (uint8_t)(str[i] - '\0');
        uint8_t num_val = (base <= 16) ? 
            _VALUE_LOOKUP_INSEN_[lookup_index] 
          : _VALUE_LOOKUP_SEN_[lookup_index];
        if (num_val > base) {
            arena_reset(_DASI_GET_STRNLEN_ARENA, tmp_mark);
            return STR_INVALID_DIGIT;
        }
        __BIGINT_INTERNAL_MUL_UI64__(&tmp_buf, base);
        __BIGINT_INTERNAL_ADD_UI64__(&tmp_buf, num_val);
    }
    __BIGINT_INTERNAL_COPY__(x, &tmp_buf); x->n = cap; // For safety measures
    arena_reset(_DASI_GET_STRNLEN_ARENA, tmp_mark);
    return STR_SUCCESS;
}
dnml_status bigInt_get_strnb(bigInt *x, const char *str, size_t len, uint8_t base) {
    assert(__BIGINT_INTERNAL_PVALID__(x));
    test_assert(!(str == NULL), str_null, io_cleanup, STR_NULL);
    test_assert(base > 1, str_inval_base, io_cleanup, STR_INVALID_BASE);
    test_assert(!(!len || *str == '\0'), str_empty, io_cleanup, STR_EMPTY);
    dnml_arena *_DASI_GET_BASENLEN_ARENA = _USE_ARENA();
    test_assert(!(_DASI_GET_BASENLEN_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    _skip_whitespace(str, len, &curr_pos);
    if (curr_pos == len || str[curr_pos] == '\0') return STR_EMPTY;
    if (str[curr_pos] == '-') { sign = -1; ++curr_pos; }
    else if (str[curr_pos] == '+') ++curr_pos;
    if (curr_pos == len || str[curr_pos] == '\0') return STR_INVALID_DIGIT;

    //* ====== 2. Leading-Zeros Handling ====== *//
    // Skipping all leading zeros
    while (str[curr_pos] == '0' && (curr_pos < len || str[curr_pos] != '\0')) ++curr_pos;
    // String full of zeros
    if (curr_pos == len || str[curr_pos] == '\0') {
        // -0 is INVALID
        if (sign = -1) return STR_INVALID_SIGN;
        x->n = 0; x->sign = 1; return STR_SUCCESS;
    }

    //* ======= 3. Initiating Resulting BigInt ======= *//
    dnml_status err_check;
    size_t true_len = 0; _actual_len(str, len, &true_len);
    size_t bits = __BITCOUNT___(true_len - curr_pos, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
    if (__BIGINT_INTERNAL_ENSCAP__(x, cap) == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    size_t tmp_mark = arena_mark(_DASI_GET_BASENLEN_ARENA);
    limb_t *tmp_limbs = arena_galloc(_DASI_GET_BASENLEN_ARENA, cap * U64_BYTES, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    bigInt tmp_buf = { .limbs = tmp_limbs, .sign = sign, .cap = cap, .n = 0 };

    //* ============= 4. Parsing and Initiating Value ================ *//
    for (size_t i = true_len - 1; i >= curr_pos; --i) {
        uint8_t lookup_index = (uint8_t)(str[i] - '\0');
        uint8_t num_val = (base <= 16) ? 
            _VALUE_LOOKUP_INSEN_[lookup_index] 
          : _VALUE_LOOKUP_SEN_[lookup_index];
        if (num_val > base) {
            arena_reset(_DASI_GET_BASENLEN_ARENA, tmp_mark);
            return STR_INVALID_DIGIT;
        }
        __BIGINT_INTERNAL_MUL_UI64__(&tmp_buf, base);
        __BIGINT_INTERNAL_ADD_UI64__(&tmp_buf, num_val);
    }
    __BIGINT_INTERNAL_COPY__(x, &tmp_buf); x->n = cap; // For safety measures
    arena_reset(_DASI_GET_BASENLEN_ARENA, tmp_mark);
    return STR_SUCCESS;
}
/* Truncative String --> BigInt */ /* Truncates */
dnml_status bigInt_tget_str(bigInt *x, const char *str) {
    assert(__BIGINT_INTERNAL_PVALID__(x));
    test_assert(!(str == NULL), str_null, io_cleanup, STR_NULL);
    test_assert(!(*str == '\0'), str_empty, io_cleanup, STR_EMPTY);
    dnml_arena *_DASI_TGET_STRING_ARENA = _USE_ARENA();
    test_assert(!(_DASI_TGET_STRING_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    _skip_whitespace(str, strlen(str), &curr_pos);
    if (str[curr_pos] == '\0') return STR_EMPTY;
    unsigned char sign_op_res = _sign_handle_(str, &curr_pos, &sign);
    if (sign_op_res == 4) return STR_INVALID_DIGIT;
    else if (sign_op_res == 3) return STR_INCOMPLETE;

    //* ====== 2. Prefix Handling ====== *//
    uint8_t base = 10;
    unsigned char prefix_op_res = _prefix_handle_(str, &curr_pos, &base);
    if (prefix_op_res == 3) return STR_INCOMPLETE;
    else if (prefix_op_res == 0) {
        if (sign == -1) return STR_INVALID_SIGN;
        x->n = 0; x->sign = 1; return STR_SUCCESS;
    } else if (prefix_op_res == 2) return STR_INVALID_BASE_PREFIX;
    else if (prefix_op_res == 4) return STR_INVALID_BASE;
    // The remaining case (prefix_op_res == 1) indicates we have a decimal string with 1+ leading zero

    //* ====== 3. Leading-Zeros Handling ====== *//
    while (str[curr_pos] == '0' && str[curr_pos] != '\0') ++curr_pos; // Skipping all leading zeros
    // String full of zeros
    if (str[curr_pos] == '\0') {
        // -0 is INVALID
        if (sign = -1) return STR_INVALID_SIGN;
        x->n = 0; x->sign = 1; return STR_SUCCESS;
    }

    //* ======= 4. Initiating Resulting BigInt ======= *//
    dnml_status err_check; size_t d = strlen(&str[curr_pos]);
    size_t bits = __BITCOUNT___(d - curr_pos, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
    size_t limit = (cap > x->cap) ? d - __BIGINT_COUNTDB__(x, 10): curr_pos;
    size_t ranged_cap = (cap > x->cap) ? x->cap : cap;
    // Initializing BigInt buffer
    size_t tmp_mark = arena_mark(_DASI_TGET_STRING_ARENA);
    limb_t *tmp_limbs = arena_galloc(_DASI_TGET_STRING_ARENA, ranged_cap * U64_BYTES, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    bigInt tmp_buf = { .limbs = tmp_limbs, .sign = sign, .cap = ranged_cap, .n = 0 };

    //* ============= 5. Parsing and Initiating Value ================ *//
    for (size_t i = d - 1; i >= limit; --i) {
        uint8_t lookup_index = (uint8_t)(str[curr_pos] - '\0');
        uint8_t num_val = (base <= 16) ? 
            _VALUE_LOOKUP_INSEN_[lookup_index] 
          : _VALUE_LOOKUP_SEN_[lookup_index];
        if (num_val > base) {
            arena_reset(_DASI_TGET_STRING_ARENA, tmp_mark);
            return STR_INVALID_DIGIT;
        }
        __BIGINT_INTERNAL_MUL_UI64__(&tmp_buf, base);
        __BIGINT_INTERNAL_ADD_UI64__(&tmp_buf, num_val);
    }
    __BIGINT_INTERNAL_COPY__(x, &tmp_buf); x->n = ranged_cap; // For safety measures
    arena_reset(_DASI_TGET_STRING_ARENA, tmp_mark);
    if (cap > x->cap) return STR_TRUNC_SUCCESS;
    else return STR_SUCCESS;
}
dnml_status bigInt_tget_strb(bigInt *x, const char *str, uint8_t base) {
    assert(__BIGINT_INTERNAL_PVALID__(x));
    test_assert(!(str == NULL), str_null, io_cleanup, STR_NULL);
    test_assert(base > 1, str_inval_base, io_cleanup, STR_INVALID_BASE);
    test_assert(!(*str == '\0'), str_empty, io_cleanup, STR_EMPTY);
    dnml_arena *_DASI_TGET_BASE_ARENA = _USE_ARENA();
    test_assert(!(_DASI_TGET_BASE_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    _skip_whitespace(str, strlen(str), &curr_pos);
    if (str[curr_pos] == '\0') return STR_EMPTY;
    if (str[curr_pos] == '-') { sign = -1; ++curr_pos; }
    else if (str[curr_pos] == '+') ++curr_pos;
    if (str[curr_pos] == '\0') return STR_INCOMPLETE;

    //* ====== 2. Leading-Zeros Handling ====== *//
    while (str[curr_pos] == '0' && str[curr_pos] != '\0') ++curr_pos; // Skipping all leading zeros
    // String full of zeros
    if (str[curr_pos] == '\0') {
        // -0 is INVALID
        if (sign = -1) return STR_INVALID_SIGN;
        x->n = 0; x->sign = 1; return STR_SUCCESS;
    }

    //* ======= 3. Initiating Resulting BigInt ======= *//
    dnml_status err_check; size_t d = strlen(&str[curr_pos]);
    size_t bits = __BITCOUNT___(d, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
    size_t limit = (cap > x->cap) ? d - __BIGINT_COUNTDB__(x, 10): curr_pos;
    size_t ranged_cap = (cap > x->cap) ? x->cap : cap;
    // Initializing BigInt buffer
    size_t tmp_mark = arena_mark(_DASI_TGET_BASE_ARENA);
    limb_t *tmp_limbs = arena_galloc(_DASI_TGET_BASE_ARENA, cap * U64_BYTES, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    bigInt tmp_buf = { .limbs = tmp_limbs, .sign = sign, .cap = ranged_cap, .n = 0 };

    //* ============= 4. Parsing and Initiating Value ================ *//
    for (size_t i = d - 1; i >= limit; --i) {
        uint8_t lookup_index = (uint8_t)(str[curr_pos] - '\0');
        uint8_t num_val = (base <= 16) ? 
            _VALUE_LOOKUP_INSEN_[lookup_index] 
          : _VALUE_LOOKUP_SEN_[lookup_index];
        if (num_val > base) {
            arena_reset(_DASI_TGET_BASE_ARENA, tmp_mark);
            return STR_INVALID_DIGIT;
        }
        __BIGINT_INTERNAL_MUL_UI64__(&tmp_buf, base);
        __BIGINT_INTERNAL_ADD_UI64__(&tmp_buf, num_val);
    }
    __BIGINT_INTERNAL_COPY__(x, &tmp_buf); x->n = ranged_cap; // For safety measures
    arena_reset(_DASI_TGET_BASE_ARENA, tmp_mark);
    if (cap > x->cap) return STR_TRUNC_SUCCESS;
    else return STR_SUCCESS;
}
dnml_status bigInt_tget_strn(bigInt *x, const char *str, size_t len) {
    assert(__BIGINT_INTERNAL_PVALID__(x));
    test_assert(!(str == NULL), str_null, io_cleanup, STR_NULL);
    test_assert(!(!len || *str == '\0'), str_empty, io_cleanup, STR_EMPTY);
    dnml_arena *_DASI_TGET_STRNLEN_ARENA = _USE_ARENA();
    test_assert(!(_DASI_TGET_STRNLEN_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    _skip_whitespace(str, len, &curr_pos);
    if (curr_pos == len || str[curr_pos] == '\0') return STR_EMPTY;
    unsigned char sign_op_res = _sign_handle_nlen_(str, &curr_pos, &sign, len);
    if (sign_op_res == 4) return STR_INVALID_DIGIT;
    else if (sign_op_res == 3) return STR_INCOMPLETE;

    //* ====== 2. Prefix Handling ====== *//
    uint8_t base = 10;
    unsigned char prefix_op_res = _prefix_handle_nlen_(str, &curr_pos, &base, len);
    if (prefix_op_res == 3) return STR_INCOMPLETE;
    else if (prefix_op_res == 0) {
        if (sign == -1) return STR_INVALID_SIGN;
        x->n = 0; x->sign = 1; return STR_SUCCESS;
    } else if (prefix_op_res == 2) return STR_INVALID_BASE_PREFIX;
    else if (prefix_op_res == 4) return STR_INVALID_BASE;
    // The remaining case (prefix_op_res == 1) indicates we have a decimal string with 1+ leading zero

    //* ====== 3. Leading-Zeros Handling ====== *//
    // Skipping all leading zeros
    while (str[curr_pos] == '0' && (curr_pos < len || str[curr_pos] != '\0')) ++curr_pos;
    // String full of zeros
    if (curr_pos == len || str[curr_pos] == '\0') {
        // -0 is INVALID
        if (sign == -1) return STR_INVALID_SIGN;
        x->n = 0; x->sign = 1; return STR_SUCCESS;
    }

    //* ======= 4. Initiating Resulting BigInt ======= *//
    dnml_status err_check;
    size_t true_len = 0; _actual_len(str, len, &true_len);
    size_t bits = __BITCOUNT___(true_len - curr_pos, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
    size_t limit = (cap > x->cap) ? true_len - __BIGINT_COUNTDB__(x, 10): curr_pos;
    size_t ranged_cap = (cap > x->cap) ? x->cap : cap;
    // Initializing BigInt buffer
    size_t tmp_mark = arena_mark(_DASI_TGET_STRNLEN_ARENA);
    limb_t *tmp_limbs = arena_galloc(_DASI_TGET_STRNLEN_ARENA, cap * U64_BYTES, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    bigInt tmp_buf = { .limbs = tmp_limbs, .sign = sign, .cap = ranged_cap, .n = 0 };

    //* ============= 5. Parsing and Initiating Value ================ *//
    for (size_t i = true_len - 1; i >= limit; --i) {
        uint8_t lookup_index = (uint8_t)(str[curr_pos] - '\0');
        uint8_t num_val = (base <= 16) ? 
            _VALUE_LOOKUP_INSEN_[lookup_index] 
          : _VALUE_LOOKUP_SEN_[lookup_index];
        if (num_val > base) {
            arena_reset(_DASI_TGET_STRNLEN_ARENA, tmp_mark);
            return STR_INVALID_DIGIT;
        }
        __BIGINT_INTERNAL_MUL_UI64__(&tmp_buf, base);
        __BIGINT_INTERNAL_ADD_UI64__(&tmp_buf, num_val);
    }
    __BIGINT_INTERNAL_COPY__(x, &tmp_buf); x->n = ranged_cap; // For safety measures
    arena_reset(_DASI_TGET_STRNLEN_ARENA, tmp_mark);
    if (cap > x->cap) return STR_TRUNC_SUCCESS;
    else return STR_SUCCESS;
}
dnml_status bigInt_tget_strnb(bigInt *x, const char *str, size_t len, uint8_t base) {
    assert(__BIGINT_INTERNAL_PVALID__(x));
    test_assert(!(str == NULL), str_null, io_cleanup, STR_NULL);
    test_assert(base > 1, str_inval_base, io_cleanup, STR_INVALID_BASE);
    test_assert(!(!len || *str == '\0'), str_empty, io_cleanup, STR_EMPTY);
    dnml_arena *_DASI_TGET_BASENLEN_ARENA = _USE_ARENA();
    test_assert(!(_DASI_TGET_BASENLEN_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    _skip_whitespace(str, len, &curr_pos);
    if (curr_pos == len || str[curr_pos] == '\0') return STR_EMPTY;
    if (str[curr_pos] == '-') { sign = -1; ++curr_pos; }
    else if (str[curr_pos] == '+') ++curr_pos;
    if (curr_pos == len || str[curr_pos] == '\0') return STR_INCOMPLETE;

    //* ====== 2. Leading-Zeros Handling ====== *//
    // Skipping all leading zeros
    while (str[curr_pos] == '0' && (curr_pos < len || str[curr_pos] != '\0')) ++curr_pos;
    // String full of zeros
    if (curr_pos == len || str[curr_pos] == '\0') {
        // -0 is INVALID
        if (sign = -1) return STR_INVALID_SIGN;
        x->n = 0; x->sign = 1;
        return STR_SUCCESS;
    }

    //* ======= 3. Initiating Resulting BigInt ======= *//
    dnml_status err_check;
    size_t true_len = 0; _actual_len(str, len, &true_len);
    size_t bits = __BITCOUNT___(true_len, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
    size_t limit = (cap > x->cap) ? true_len - __BIGINT_COUNTDB__(x, 10): curr_pos;
    size_t ranged_cap = (cap > x->cap) ? x->cap : cap;
    // Initializing BigInt buffer
    size_t tmp_mark = arena_mark(_DASI_TGET_BASENLEN_ARENA);
    limb_t *tmp_limbs = arena_galloc(_DASI_TGET_BASENLEN_ARENA, cap * U64_BYTES, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    bigInt tmp_buf = { .limbs = tmp_limbs, .sign = sign, .cap = ranged_cap, .n = 0 };

    //* ============= 4. Parsing and Initiating Value ================ *//
    for (size_t i = true_len - 1; i >= limit; --i) {
        uint8_t lookup_index = (uint8_t)(str[curr_pos] - '\0');
        uint8_t num_val = (base <= 16) ? 
            _VALUE_LOOKUP_INSEN_[lookup_index] 
          : _VALUE_LOOKUP_SEN_[lookup_index];
        if (num_val > base) {
            arena_reset(_DASI_TGET_BASENLEN_ARENA, tmp_mark);
            return STR_INVALID_DIGIT;
        }
        __BIGINT_INTERNAL_MUL_UI64__(&tmp_buf, base);
        __BIGINT_INTERNAL_ADD_UI64__(&tmp_buf, num_val);
    }
    __BIGINT_INTERNAL_COPY__(x, &tmp_buf); x->n = ranged_cap; // For safety measures
    arena_reset(_DASI_TGET_BASENLEN_ARENA, tmp_mark);
    if (cap > x->cap) return STR_TRUNC_SUCCESS;
    else return STR_SUCCESS;
}
/* Safe String --> BigInt */ /* Return an Error */
dnml_status bigInt_sget_str(bigInt *x, const char *str) {
    assert(__BIGINT_INTERNAL_PVALID__(x));
    test_assert(!(str == NULL), str_null, io_cleanup, STR_NULL);
    test_assert(!(*str == '\0'), str_empty, io_cleanup, STR_EMPTY);
    dnml_arena *_DASI_SGET_STRING_ARENA = _USE_ARENA();
    test_assert(!(_DASI_SGET_STRING_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    _skip_whitespace(str, strlen(str), &curr_pos);
    if (str[curr_pos] == '\0') return STR_EMPTY;
    unsigned char sign_op_res = _sign_handle_(str, &curr_pos, &sign);
    if (sign_op_res == 4) return STR_INVALID_DIGIT;
    else if (sign_op_res == 3) return STR_INCOMPLETE;

    //* ====== 2. Prefix Handling ====== *//
    uint8_t base = 10;
    unsigned char prefix_op_res = _prefix_handle_(str, &curr_pos, &base);
    if (prefix_op_res == 3) return STR_INCOMPLETE;
    else if (prefix_op_res == 0) {
        if (sign == -1) return STR_INVALID_SIGN;
        x->n = 0; x->sign = 1; return STR_SUCCESS;
    } else if (prefix_op_res == 2) return STR_INVALID_BASE_PREFIX;
    else if (prefix_op_res == 4) return STR_INVALID_BASE;
    // The remaining case (prefix_op_res == 1) indicates we have a decimal string with 1+ leading zero

    //* ====== 3. Leading-Zeros Handling ====== *//
    while (str[curr_pos] == '0' && str[curr_pos] != '\0') ++curr_pos; // Skipping all leading zeros
    // String full of zeros
    if (str[curr_pos] == '\0') {
        // -0 is INVALID
        if (sign = -1) return STR_INVALID_SIGN;
        x->n = 0; x->sign = 1; return STR_SUCCESS;
    }

    //* ======= 4. Initiating Resulting BigInt ======= *//
    dnml_status err_check; size_t d = strlen(&str[curr_pos]);
    size_t bits = __BITCOUNT___(d - curr_pos, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
    if (x->cap < cap) return BIGINT_ERR_RANGE;
    size_t tmp_mark = arena_mark(_DASI_SGET_STRING_ARENA);
    limb_t *tmp_limbs = arena_galloc(_DASI_SGET_STRING_ARENA, cap * U64_BYTES, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    bigInt tmp_buf = { .limbs = tmp_limbs, .sign = sign, .cap = cap, .n = cap };

    //* ============= 5. Parsing and Initiating Value ================ *//
    for (size_t i = d - 1; i >= curr_pos; --i) {
        uint8_t lookup_index = (uint8_t)(str[i] - '\0');
        uint8_t num_val = (base <= 16) ? 
            _VALUE_LOOKUP_INSEN_[lookup_index] 
          : _VALUE_LOOKUP_SEN_[lookup_index];
        if (num_val > base) {
            arena_reset(_DASI_SGET_STRING_ARENA, tmp_mark);
            return STR_INVALID_DIGIT;
        }
        __BIGINT_INTERNAL_MUL_UI64__(&tmp_buf, base);
        __BIGINT_INTERNAL_ADD_UI64__(&tmp_buf, num_val);
    }
    memcpy(x->limbs, tmp_limbs, cap * U64_BYTES);
    x->n = cap; x->sign = sign;
    arena_reset(_DASI_SGET_STRING_ARENA, tmp_mark);
    return STR_SUCCESS;
}
dnml_status bigInt_sget_strb(bigInt *x, const char *str, uint8_t base) {
    assert(__BIGINT_INTERNAL_PVALID__(x));
    test_assert(!(str == NULL), str_null, io_cleanup, STR_NULL);
    test_assert(base > 1, str_inval_base, io_cleanup, STR_INVALID_BASE);
    test_assert(!(*str == '\0'), str_empty, io_cleanup, STR_EMPTY);
    dnml_arena *_DASI_SGET_BASE_ARENA = _USE_ARENA();
    test_assert(!(_DASI_SGET_BASE_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    _skip_whitespace(str, strlen(str), &curr_pos);
    if (str[curr_pos] == '\0') return STR_EMPTY;
    if (str[curr_pos] == '-') { sign = -1; ++curr_pos; }
    else if (str[curr_pos] == '+') ++curr_pos;
    if (str[curr_pos] == '\0') return STR_INCOMPLETE;

    //* ====== 2. Leading-Zeros Handling ====== *//
    while (str[curr_pos] == '0' && str[curr_pos] != '\0') ++curr_pos; // Skipping all leading zeros
    // String full of zeros
    if (str[curr_pos] == '\0') {
        // -0 is INVALID
        if (sign = -1) return STR_INVALID_SIGN;
        x->n = 0; x->sign = 1; return STR_SUCCESS;
    }

    //* ======= 3. Initiating Resulting BigInt ======= *//
    dnml_status err_check; size_t d = strlen(&str[curr_pos]);
    size_t bits = __BITCOUNT___(d - curr_pos, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
    if (x->cap < cap) return BIGINT_ERR_RANGE;
    size_t tmp_mark = arena_mark(_DASI_SGET_BASE_ARENA);
    limb_t *tmp_limbs = arena_galloc(_DASI_SGET_BASE_ARENA, cap * U64_BYTES, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    bigInt tmp_buf = { .limbs = tmp_limbs, .sign = sign, .cap = cap, .n = cap };


    //* ============= 4. Parsing and Initiating Value ================ *//
    for (size_t i = d - 1; i >= curr_pos; --i) {
        uint8_t lookup_index = (uint8_t)(str[i] - '\0');
        uint8_t num_val = (base <= 16) ? 
            _VALUE_LOOKUP_INSEN_[lookup_index] 
          : _VALUE_LOOKUP_SEN_[lookup_index];
        if (num_val > base) {
            arena_reset(_DASI_SGET_BASE_ARENA, tmp_mark);
            return STR_INVALID_DIGIT;
        }
        __BIGINT_INTERNAL_MUL_UI64__(&tmp_buf, base);
        __BIGINT_INTERNAL_ADD_UI64__(&tmp_buf, num_val);
    }
    memcpy(x->limbs, tmp_limbs, cap * U64_BYTES);
    x->n = cap; x->sign = sign;
    arena_reset(_DASI_SGET_BASE_ARENA, tmp_mark);
    return STR_SUCCESS;
}
dnml_status bigInt_sget_strn(bigInt *x, const char *str, size_t len) { 
    assert(__BIGINT_INTERNAL_PVALID__(x));
    test_assert(!(str == NULL), str_null, io_cleanup, STR_NULL);
    test_assert(!(!len || *str == '\0'), str_empty, io_cleanup, STR_EMPTY);
    dnml_arena *_DASI_SGET_STRNLEN_ARENA = _USE_ARENA();
    test_assert(!(_DASI_SGET_STRNLEN_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
     //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    _skip_whitespace(str, len, &curr_pos);
    if (curr_pos == len || str[curr_pos] == '\0') return STR_EMPTY;
    unsigned char sign_op_res = _sign_handle_nlen_(str, &curr_pos, &sign, len);
    if (sign_op_res == 4) return STR_INVALID_DIGIT;
    else if (sign_op_res == 3) return STR_INCOMPLETE;

    //* ====== 2. Prefix Handling ====== *//
    uint8_t base = 10;
    unsigned char prefix_op_res = _prefix_handle_nlen_(str, &curr_pos, &base, len);
    if (prefix_op_res == 3) return STR_INCOMPLETE;
    else if (prefix_op_res == 0) {
        if (sign == -1) return STR_INVALID_SIGN;
        x->n = 0; x->sign = 1; return STR_SUCCESS;
    } else if (prefix_op_res == 2) return STR_INVALID_BASE_PREFIX;
    else if (prefix_op_res == 4) return STR_INVALID_BASE;
    // The remaining case (prefix_op_res == 1) indicates we have a decimal string with 1+ leading zero

    //* ====== 3. Leading-Zeros Handling ====== *//
    // Skipping all leading zeros
    while (str[curr_pos] == '0' && (curr_pos < len || str[curr_pos] != '\0')) ++curr_pos;
    // String full of zeros
    if (curr_pos == len || str[curr_pos] == '\0') {
        // -0 is INVALID
        if (sign == -1) return STR_INVALID_SIGN;
        x->n = 0; x->sign = 1; return STR_SUCCESS;
    }

    //* ======= 4. Initiating Resulting BigInt ======= *//
    dnml_status err_check;
    size_t true_len = 0; _actual_len(str, len, &true_len);
    size_t bits = __BITCOUNT___(true_len - curr_pos, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
    if (x->cap < cap) return BIGINT_ERR_RANGE;
    size_t tmp_mark = arena_mark(_DASI_SGET_STRNLEN_ARENA);
    limb_t *tmp_limbs = arena_galloc(_DASI_SGET_STRNLEN_ARENA, cap * U64_BYTES, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    bigInt tmp_buf = { .limbs = tmp_limbs, .sign = sign, .cap = cap, .n = cap };

    //* ============= 5. Parsing and Initiating Value ================ *//
    for (size_t i = true_len - 1; i >= curr_pos; --i) {
        uint8_t lookup_index = (uint8_t)(str[i] - '\0');
        uint8_t num_val = (base <= 16) ? 
            _VALUE_LOOKUP_INSEN_[lookup_index] 
          : _VALUE_LOOKUP_SEN_[lookup_index];
        if (num_val > base) {
            arena_reset(_DASI_SGET_STRNLEN_ARENA, tmp_mark);
            return STR_INVALID_DIGIT;
        }
        __BIGINT_INTERNAL_MUL_UI64__(&tmp_buf, base);
        __BIGINT_INTERNAL_ADD_UI64__(&tmp_buf, num_val);
    }
    memcpy(x->limbs, tmp_limbs, cap * U64_BYTES);
    x->n = cap; x->sign = sign;
    arena_reset(_DASI_SGET_STRNLEN_ARENA, tmp_mark);
    return STR_SUCCESS;
}
dnml_status bigInt_sget_strnb(bigInt *x, const char *str, size_t len, uint8_t base) {
    assert(__BIGINT_INTERNAL_PVALID__(x));
    test_assert(!(str == NULL), str_null, io_cleanup, STR_NULL);
    test_assert(base > 1, str_inval_base, io_cleanup, STR_INVALID_BASE);
    test_assert(!(!len || *str == '\0'), str_empty, io_cleanup, STR_EMPTY);
    dnml_arena *_DASI_SGET_BASENLEN_ARENA = _USE_ARENA();
    test_assert(!(_DASI_SGET_BASENLEN_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    //* ====== 1. Sign Handling ====== *//
    size_t curr_pos = 0; uint8_t sign = 1;
    _skip_whitespace(str, len, &curr_pos);
    if (curr_pos == len || str[curr_pos] == '\0') return STR_EMPTY;
    if (str[curr_pos] == '-') { sign = -1; ++curr_pos; }
    else if (str[curr_pos] == '+') ++curr_pos;
    if (curr_pos == len || str[curr_pos] == '\0') return STR_INCOMPLETE;

    //* ====== 2. Leading-Zeros Handling ====== *//
    // Skipping all leading zeros
    while (str[curr_pos] == '0' && (curr_pos < len || str[curr_pos] != '\0')) ++curr_pos;
    // String full of zeros
    if (curr_pos == len || str[curr_pos] == '\0') {
        // -0 is INVALID
        if (sign = -1) return STR_INVALID_SIGN;
        x->n = 0; x->sign = 1; return STR_SUCCESS;
    }

    //* ======= 3. Initiating Resulting BigInt ======= *//
    dnml_status err_check;
    size_t true_len = 0; _actual_len(str, len, &true_len);
    size_t bits = __BITCOUNT___(true_len - curr_pos, base);
    size_t cap = __BIGINT_LIMBS_NEEDED__(bits);
    if (x->cap < cap) return BIGINT_ERR_RANGE;
    size_t tmp_mark = arena_mark(_DASI_SGET_BASENLEN_ARENA);
    limb_t *tmp_limbs = arena_galloc(_DASI_SGET_BASENLEN_ARENA, cap * U64_BYTES, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    bigInt tmp_buf = { .limbs = tmp_limbs, .sign = sign, .cap = cap, .n = cap };

    //* ============= 4. Parsing and Initiating Value ================ *//
    for (size_t i = true_len - 1; i >= curr_pos; --i) {
        uint8_t lookup_index = (uint8_t)(str[i] - '\0');
        uint8_t num_val = (base <= 16) ? 
            _VALUE_LOOKUP_INSEN_[lookup_index] 
          : _VALUE_LOOKUP_SEN_[lookup_index];
        if (num_val > base) {
            arena_reset(_DASI_SGET_BASENLEN_ARENA, tmp_mark);
            return STR_INVALID_DIGIT;
        }
        __BIGINT_INTERNAL_MUL_UI64__(&tmp_buf, base);
        __BIGINT_INTERNAL_ADD_UI64__(&tmp_buf, num_val);
    }
    memcpy(x->limbs, tmp_limbs, cap * U64_BYTES);
    x->n = cap; x->sign = sign;
    arena_reset(_DASI_SGET_BASENLEN_ARENA, tmp_mark);
    return STR_SUCCESS;
}




//todo ======================================== 3. INPUT & OUTPUT ======================================= *//
/* Note - GROW API:
*   +) Functions:
*
*       - bigInt_scan()     - bigInt_fscan()
*       - bigInt_scanb()    - bigInt_fscanb()
*
*      Internally utilizes heap-allocation for its temporaries instead
*      of the traditionally "preferred" arena allocation from ___DASI_IO_ARENA_
*      due to its "growth" API. 
*
*      It contracts the growth of the mutated input "x" to hold the potentially 
*      larger string parsed from a STREAM (stdin or custom).
*      Since such API is stream-based, meaning we can't know in advance the true
*      size of the input string to precalculate the appropriate buffer size, we
*      resort to on-the-fly checks for imminent overflow.
*
*      Since our arena model DOES NOT safely enables reallocation for growth,
*      for either its capacity nor a pointer's scope, trying to grow using
*      the dnml_arena model is not reentrant-safe since reallocation renders
*      out-of-scoped pointers to point to invalid memory.
*
*      --------> Heap-allocation for size-owning buffers that can be easily grown.
*/
size_t bigInt_fscan_size(FILE *stream, uint8_t *baseout, dnml_status *err) {
    char lexical_comp[3]; size_t res = 0;
    while (isspace(fgetc(stream))) fseek(stream, 1, SEEK_CUR); // Whitespace
    size_t parse_res = fread(lexical_comp, sizeof(char), 3, stream);
    test_assert_pre(!(ferror(stream)), stream_err, io_cleanup, err, FILE_ERR_PARSE, baseout, 10, res);
    long offset_set = 0;
    uint8_t sign = 1, base = 10, curr_lexpos = 0;
    if (lexical_comp[curr_lexpos] == '-') { 
        sign = -1; ++curr_lexpos; 
        if (curr_lexpos >= parse_res) { *err =  STR_INCOMPLETE; *baseout = base; return res; }
    } else if (lexical_comp[curr_lexpos] == '+') { 
        ++curr_lexpos;
        if (curr_lexpos >= parse_res) { *err = STR_INCOMPLETE; *baseout = base; return res; }
    } else if (!isdigit(lexical_comp[curr_lexpos])) { *err = STR_INVALID_SIGN; *baseout = base; return res; }

    //* Prefix & leading zeros *//
    if (isdigit(lexical_comp[curr_lexpos]) // The string is currently "9.."
    && lexical_comp[curr_lexpos] != '0') offset_set = -(parse_res - curr_lexpos - 1);
    else {
        ++curr_lexpos;
        if (curr_lexpos >= parse_res) {
            // The string is just 1 singular 0 ('0' or '-0', etc)
            if (sign == -1) { *err = STR_INVALID_SIGN; *baseout = base; return res; }
            *err = STR_SUCCESS; *baseout = 10; return res;
        } else if (isdigit(lexical_comp[curr_lexpos])) {
            // The string is base-10 with leading zeros ('09' or '00', etc)
            offset_set = (-parse_res - curr_lexpos - 1);
        } else {
            switch (lexical_comp[curr_lexpos]) {
                // Hexadecimal (Base-16)
                case 'x':       base = 16; break;
                case 'X':       base = 16; break;
                // Binary (Base-2)
                case 'b':       base = 2; break;
                case 'B':       base = 2; break;
                // Octal (Base-8)
                case 'o':       base = 8; break;
                case 'O':       base = 8; break;
                //! INVALID BASE PREFIX
                default:        *err = STR_INVALID_BASE_PREFIX; *baseout = base; return res; break;
            } offset_set = (-parse_res - curr_lexpos - 1);
        }
    } fseek(stream, offset_set, SEEK_CUR);
    int curr_char = fgetc(stream);
    while (curr_char != EOF && curr_char == '0') curr_char = fgetc(stream);
    if (curr_char == EOF) {
        if (sign == -1) { *err = STR_INVALID_SIGN; *baseout = base; return res; }
        *err = STR_SUCCESS; *baseout = base; return res;
    }

    //* Main accumalator loop *//
    size_t i; uint8_t index_lookup, numerical_val;
    while (1) {
        parse_res = fread(___DASI_IO_CHUNKBUF_, sizeof(char), ___DASI_IO_BUFSIZE, stream);
        //* THE ACTUAL ACCUMALATION
        if (parse_res > 0) {
            for (i = 0; i < parse_res; ++i) {
                index_lookup = (uint8_t)(___DASI_IO_CHUNKBUF_[i] - '\0');
                numerical_val = (base <= 16) ? 
                    _VALUE_LOOKUP_INSEN_[index_lookup] : 
                    _VALUE_LOOKUP_SEN_[index_lookup];
                if (numerical_val >= base) { 
                    *err = STR_INVALID_DIGIT; 
                    *baseout = base;
                    return res; 
                } ++res;
            }
        }
        //* ENDING CONDITION
        if (parse_res < ___DASI_IO_BUFSIZE) {
            if (ferror(stream)) { *err = FILE_ERR_PARSE; return res; }
            else if (feof(stream)) break;
        }
    } *err = STR_SUCCESS; *baseout = base; return res;
}
size_t bigInt_fscanb_size(FILE *stream, uint8_t base, dnml_status *err) {
    test_assert(base > 1, str_inval_base, io_cleanup, STR_INVALID_BASE);
    //* Whitespace -> Signs -> Leading Zeros *//
    uint8_t sign = 1; int curr_char; size_t res = 0;
    while (isspace(fgetc(stream))) fseek(stream, 1, SEEK_CUR); // Whitespace
    curr_char = fgetc(stream);
    if (curr_char == '-' || curr_char == '+') {
        if (curr_char == '-') sign = -1;
        if (fgetc(stream) == EOF) { *err = STR_INCOMPLETE; return res; }
    }
    else if (!isdigit(curr_char)) { *err = STR_INVALID_DIGIT; return res; }
    else ungetc(curr_char, stream); // Rewind back if curr_char is numeric (0-9)
    // Skipping Leading Zeros
    do { curr_char = fgetc(stream); }
    while (curr_char != EOF && curr_char == '0');
    if (curr_char == EOF && sign == -1) { *err = STR_INVALID_SIGN; return res; }

    //* Main accumalator loop *//
    uint8_t index_lookup, numerical_val, i; size_t parse_res;
    while (1) {
        parse_res = fread(___DASI_IO_CHUNKBUF_, sizeof(char), ___DASI_IO_BUFSIZE, stream);
        //* THE ACTUAL ACCUMALATION
        if (parse_res > 0) {
            for (i = 0; i < parse_res; ++i) {
                index_lookup = (uint8_t)(___DASI_IO_CHUNKBUF_[i] - '\0');
                numerical_val = (base <= 16) ? 
                    _VALUE_LOOKUP_INSEN_[index_lookup] : 
                    _VALUE_LOOKUP_SEN_[index_lookup];
                if (numerical_val >= base) { 
                    *err = STR_INVALID_DIGIT; return res; 
                } ++res;
            }
        }
        //* ENDING CONDITION
        if (parse_res < ___DASI_IO_BUFSIZE) {
            if (ferror(stream)) {
                *err = FILE_ERR_PARSE;
                return res;
            } else if (feof(stream)) break;
        }
    } *err = STR_SUCCESS; return res;
}
size_t bigInt_fscansa_size(FILE *stream, uint8_t *baseout, size_t bi_size, dnml_status *err) {
    char lexical_comp[3]; size_t res = 0;
    while (isspace(fgetc(stream))) fseek(stream, 1, SEEK_CUR); // Whitespace
    size_t parse_res = fread(lexical_comp, sizeof(char), 3, stream);
    test_assert_pre(!(ferror(stream)), stream_err, io_cleanup, err, FILE_ERR_PARSE, baseout, 10, res);
    long offset_set = 0;
    uint8_t sign = 1, base = 10, curr_lexpos = 0;
    if (lexical_comp[curr_lexpos] == '-') { 
        sign = -1; ++curr_lexpos; 
        if (curr_lexpos >= parse_res) { *err =  STR_INCOMPLETE; *baseout = base; return res; }
    } else if (lexical_comp[curr_lexpos] == '+') { 
        ++curr_lexpos;
        if (curr_lexpos >= parse_res) { *err = STR_INCOMPLETE; *baseout = base; return res; }
    } else if (!isdigit(lexical_comp[curr_lexpos])) { *err = STR_INVALID_SIGN; *baseout = base; return res; }

    //* Prefix & leading zeros *//
    if (isdigit(lexical_comp[curr_lexpos]) // The string is currently "9.."
    && lexical_comp[curr_lexpos] != '0') offset_set = -(parse_res - curr_lexpos - 1);
    else {
        ++curr_lexpos;
        if (curr_lexpos >= parse_res) {
            // The string is just 1 singular 0 ('0' or '-0', etc)
            if (sign == -1) { *err = STR_INVALID_SIGN; *baseout = base; return res; }
            *err = STR_SUCCESS; *baseout = 10; return res;
        } else if (isdigit(lexical_comp[curr_lexpos])) {
            // The string is base-10 with leading zeros ('09' or '00', etc)
            offset_set = (-parse_res - curr_lexpos - 1);
        } else {
            switch (lexical_comp[curr_lexpos]) {
                // Hexadecimal (Base-16)
                case 'x':       base = 16; break;
                case 'X':       base = 16; break;
                // Binary (Base-2)
                case 'b':       base = 2; break;
                case 'B':       base = 2; break;
                // Octal (Base-8)
                case 'o':       base = 8; break;
                case 'O':       base = 8; break;
                //! INVALID BASE PREFIX
                default:        *err = STR_INVALID_BASE_PREFIX; *baseout = base; return res; break;
            } offset_set = (-parse_res - curr_lexpos - 1);
        }
    } fseek(stream, offset_set, SEEK_CUR);
    int curr_char = fgetc(stream);
    while (curr_char != EOF && curr_char == '0') curr_char = fgetc(stream);
    if (curr_char == EOF) {
        if (sign == -1) { *err = STR_INVALID_SIGN; *baseout = base; return res; }
        *err = STR_SUCCESS; *baseout = base; return res;
    }

    //* Main accumalator loop *//
    size_t i = 0, curr_n = 0; 
    uint8_t index_lookup, numerical_val;
    uint64_t last_limb = 0, hi = 0, lo = 0;
    while (1) {
        parse_res = fread(___DASI_IO_CHUNKBUF_, sizeof(char), ___DASI_IO_BUFSIZE, stream);
        //* THE ACTUAL ACCUMALATION
        if (parse_res > 0) {
            for (i = 0; i < parse_res; ++i) {
                index_lookup = (uint8_t)(___DASI_IO_CHUNKBUF_[i] - '\0');
                numerical_val = (base <= 16) ? 
                    _VALUE_LOOKUP_INSEN_[index_lookup] : 
                    _VALUE_LOOKUP_SEN_[index_lookup];
                if (numerical_val >= base) { 
                    *err = STR_INVALID_DIGIT; 
                    *baseout = base;
                    return res; 
                } ++res; uint8_t carry = 0;
                lo = __MUL_UI64__(last_limb, base, &hi);
                lo = __ADD_UI64__(lo, numerical_val, &carry);
                hi += carry; lo = last_limb;
                if (hi) {
                    if (curr_n == bi_size) {
                        *err = STR_INVALID_DIGIT; 
                        *baseout = base;
                        return res; 
                    } ++curr_n;
                }
            }
        }
        //* ENDING CONDITION
        if (parse_res < ___DASI_IO_BUFSIZE) {
            if (ferror(stream)) { *err = FILE_ERR_PARSE; return res; }
            else if (feof(stream)) break;
        }
    } *err = STR_SUCCESS; *baseout = base; return res;
}
size_t bigInt_fscanbsa_size(FILE *stream, uint8_t base, size_t bi_size, dnml_status *err) {
    test_assert(base > 1, str_inval_base, io_cleanup, STR_INVALID_BASE);
    //* Whitespace -> Signs -> Leading Zeros *//
    uint8_t sign = 1; int curr_char; size_t res = 0;
    while (isspace(fgetc(stream))) fseek(stream, 1, SEEK_CUR); // Whitespace
    curr_char = fgetc(stream);
    if (curr_char == '-' || curr_char == '+') {
        if (curr_char == '-') sign = -1;
        if (fgetc(stream) == EOF) { *err = STR_INCOMPLETE; return res; }
    }
    else if (!isdigit(curr_char)) { *err = STR_INVALID_DIGIT; return res; }
    else ungetc(curr_char, stream); // Rewind back if curr_char is numeric (0-9)
    // Skipping Leading Zeros
    do { curr_char = fgetc(stream); }
    while (curr_char != EOF && curr_char == '0');
    if (curr_char == EOF && sign == -1) { *err = STR_INVALID_SIGN; return res; }

    //* Main accumalator loop *//
    uint8_t index_lookup, numerical_val; 
    size_t parse_res, i = 0, curr_n = 0;
    uint64_t last_limb, lo = 0, hi = 0;
    while (1) {
        parse_res = fread(___DASI_IO_CHUNKBUF_, sizeof(char), ___DASI_IO_BUFSIZE, stream);
        //* THE ACTUAL ACCUMALATION
        if (parse_res > 0) {
            for (i = 0; i < parse_res; ++i) {
                index_lookup = (uint8_t)(___DASI_IO_CHUNKBUF_[i] - '\0');
                numerical_val = (base <= 16) ? 
                    _VALUE_LOOKUP_INSEN_[index_lookup] : 
                    _VALUE_LOOKUP_SEN_[index_lookup];
                if (numerical_val >= base) { 
                    *err = STR_INVALID_DIGIT; return res; 
                } ++res; uint8_t carry = 0;
                lo = __MUL_UI64__(last_limb, base, &hi);
                lo = __ADD_UI64__(lo, numerical_val, &carry);
                hi += carry; lo = last_limb;
                if (hi) {
                    if (curr_n == bi_size) {
                        *err = STR_INVALID_DIGIT; 
                        return res; 
                    } ++curr_n;
                }
            }
        }
        //* ENDING CONDITION
        if (parse_res < ___DASI_IO_BUFSIZE) {
            if (ferror(stream)) {
                *err = FILE_ERR_PARSE;
                return res;
            } else if (feof(stream)) break;
        }
    } *err = STR_SUCCESS; return res;
}
/* --------- Decimal Instant OUTPUT ---------  */
dnml_status bigInt_put(const bigInt x) {
    assert(__BIGINT_INTERNAL_VALID__(&x));
    if (x.n == 0) putchar('0');
    else if (x.n == 1) {
        if (x.sign == -1) putchar('-');
        printf("%" PRIu64, x.limbs[0]);
    } else {
        if (x.sign == -1) putchar('-');
        dnml_arena *_DASI_PUT_ARENA = _USE_ARENA();
        test_assert(!(_DASI_PUT_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
        size_t tmp_mark = arena_mark(_DASI_PUT_ARENA); dnml_status err_check;
        limb_t *tmp_limbs = arena_galloc(_DASI_PUT_ARENA, x.n * U64_BYTES, &err_check);
        if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;

        bigInt tmp_buf = { .limbs = tmp_limbs, .sign = x.sign, .cap = x.n, .n = x.n }; 
        memcpy(tmp_limbs, x.limbs, x.n * U64_BYTES);
        while (tmp_buf.n > 0) {
            uint8_t numeric_val = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_buf, 10);
            putchar((char)('0' + numeric_val));
        } 
        arena_reset(_DASI_PUT_ARENA, tmp_mark);
    } return STR_SUCCESS;
}
dnml_status bigInt_putb(const bigInt x, uint8_t base) {
    assert(__BIGINT_INTERNAL_VALID__(&x));
    test_assert(base > 1, str_inval_base, io_cleanup, STR_INVALID_BASE);
    if (x.n == 0) putchar('0');
    else if (x.n == 1) {
        if (x.sign == -1) putchar('-');
        if (base == 10)         printf("%" PRIu64, x.limbs[0]);
        else if (base == 8)     printf("%" PRIo64, x.limbs[0]);
        else if (base == 16)    printf("%" PRIX64, x.limbs[0]);
        else if (base == 2) {
            uint64_t tmp_copy = x.limbs[0];
            while (tmp_copy > 0) {
                char c = (tmp_copy & 1) ? '1' : '0';
                putchar(c); tmp_copy >>= 1;
            }
        } else {
            uint64_t tmp_copy = x.limbs[0];
            while (tmp_copy > 0) {
                char c = _DIGIT_INSEN_[tmp_copy % base];
                putchar(c); tmp_copy /= base;
            }
        }
    } else {
        if (x.sign == -1) putchar('-');
        dnml_arena *_DASI_PUTB_ARENA = _USE_ARENA();
        test_assert(!(_DASI_PUTB_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
        size_t tmp_mark = arena_mark(_DASI_PUTB_ARENA); dnml_status err_check;
        limb_t *tmp_limbs = arena_galloc(_DASI_PUTB_ARENA, x.n * U64_BYTES, &err_check);
        if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;

        bigInt tmp_buf = { .limbs = tmp_limbs, .sign = x.sign, .cap = x.n, .n = x.n }; 
        memcpy(tmp_limbs, x.limbs, x.n * U64_BYTES);
        while (tmp_buf.n > 0) {
            uint8_t numerical_val = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_buf, base);
            char c = (base <= 16) ? _DIGIT_INSEN_[numerical_val] : 
            _DIGIT_SEN_[numerical_val]; putchar(c);
        } 
        arena_reset(_DASI_PUTB_ARENA, tmp_mark);
    } return STR_SUCCESS;
}
dnml_status bigInt_putf(const bigInt x, uint8_t base, bool uppercase) {
    assert(__BIGINT_INTERNAL_VALID__(&x));
    test_assert(base > 1, str_inval_base, io_cleanup, STR_INVALID_BASE);
    if (x.n == 0) putchar('0');
    else if (x.n == 1) {
        if (x.sign == -1) putchar('-');
        if (base == 10)         printf("%" PRIu64, x.limbs[0]);
        else if (base == 8)     printf("%#%" PRIo64, x.limbs[0]);
        else if (base == 16)    printf("%#%" PRIX64, x.limbs[0]);
        else if (base == 2) {
            fputs("0b", stdout); uint64_t tmp_copy = x.limbs[0];
            while (tmp_copy > 0) {
                char c = (tmp_copy & 1) ? '1' : '0';
                putchar(c); tmp_copy >>= 1;
            }
        } else { if (base == 64) fputs("0,", stdout);
            else printf("0{%" PRIu8 "}", base); 
            uint64_t tmp_copy = x.limbs[0];
            while (tmp_copy > 0) {
                char c = (base <= 16) ? 
                    _DIGIT_INSEN_[tmp_copy % base] : 
                    _DIGIT_SEN_[tmp_copy % base];
                putchar(c); tmp_copy /= base;
            }
        }
    } else {
        uint8_t additional_val = (uppercase) ? 15 : 0;
        if (x.sign == -1) putchar('-');
        switch (base) {
            case 2: fputs("0b", stdout); break;
            case 8: fputs("0o", stdout); break;
            case 16: fputs("0x", stdout); break;
            case 64: fputs("0,", stdout); break;
            case 10: break;
            default: printf("0{%" PRIu8 "}", base); break;
        }
        dnml_arena *_DASI_PUTF_ARENA = _USE_ARENA();
        test_assert(!(_DASI_PUTF_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
        size_t tmp_mark = arena_mark(_DASI_PUTF_ARENA); dnml_status err_check;
        limb_t *tmp_limbs = arena_galloc(_DASI_PUTF_ARENA, x.n * U64_BYTES, &err_check);
        if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;

        bigInt tmp_buf = { .limbs = tmp_limbs, .sign = x.sign, .cap = x.n, .n = x.n }; 
        memcpy(tmp_limbs, x.limbs, x.n * U64_BYTES);
        while (tmp_buf.n > 0) {
            uint8_t numerical_val = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_buf, base);
            char c = (base <= 16) ? _DIGIT_INSEN_[numerical_val + additional_val] : 
            _DIGIT_SEN_[numerical_val]; putchar(c);
        } 
        arena_reset(_DASI_PUTF_ARENA, tmp_mark);
    } return STR_SUCCESS;
}
dnml_status bigInt_fput(FILE *stream, const bigInt x) {
    assert(__BIGINT_INTERNAL_VALID__(&x));
    if (x.n == 0) fputc('0', stream);
    else if (x.n == 1) {
        if (x.sign == -1) fputc('-', stream);
        fprintf(stream, "%" PRIu64, x.limbs[0]);
    } else {
        if (x.sign == -1) fputc('-', stream);
        dnml_arena *_DASI_FPUT_ARENA = _USE_ARENA();
        test_assert(!(_DASI_FPUT_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
        size_t tmp_mark = arena_mark(_DASI_FPUT_ARENA); dnml_status err_check;
        limb_t *tmp_limbs = arena_galloc(_DASI_FPUT_ARENA, x.n * U64_BYTES, &err_check);
        if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;

        bigInt tmp_buf = { .limbs = tmp_limbs, .sign = x.sign, .cap = x.n, .n = x.n }; 
        memcpy(tmp_limbs, x.limbs, x.n * U64_BYTES);
        while (tmp_buf.n > 0) {
            uint8_t numerical_val = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_buf, 10);
            fputc((char)('0' + numerical_val), stream);
        } 
        arena_reset(_DASI_FPUT_ARENA, tmp_mark);
    } return STR_SUCCESS;
}
dnml_status bigInt_fputb(FILE *stream, const bigInt x, uint8_t base) {
    assert(__BIGINT_INTERNAL_VALID__(&x));
    test_assert(base > 1, str_inval_base, io_cleanup, STR_INVALID_BASE);
    if (x.n == 0) putchar('0');
    else if (x.n == 1) {
        if (x.sign == -1) fputc('-', stream);
        if (base == 10)         fprintf(stream, "%" PRIu64, x.limbs[0]);
        else if (base == 8)     fprintf(stream, "%" PRIo64, x.limbs[0]);
        else if (base == 16)    fprintf(stream, "%" PRIX64, x.limbs[0]);
        else if (base == 2) {
            uint64_t tmp_copy = x.limbs[0];
            while (tmp_copy > 0) {
                char c = (tmp_copy & 1) ? '1' : '0';
                fputc(c, stream); tmp_copy >>= 1;
            }
        } else {
            uint64_t tmp_copy = x.limbs[0];
            while (tmp_copy > 0) {
                char c = _DIGIT_INSEN_[tmp_copy % base];
                fputc(c, stream); tmp_copy /= base;
            }
        }
    } else {
        if (x.sign == -1) fputc('-', stream);
        dnml_arena *_DASI_FPUTB_ARENA = _USE_ARENA();
        test_assert(!(_DASI_FPUTB_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
        size_t tmp_mark = arena_mark(_DASI_FPUTB_ARENA); dnml_status err_check;
        limb_t *tmp_limbs = arena_galloc(_DASI_FPUTB_ARENA, x.n * U64_BYTES, &err_check);
        if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;

        bigInt tmp_buf = { .limbs = tmp_limbs, .sign = x.sign, .cap = x.n, .n = x.n }; 
        memcpy(tmp_limbs, x.limbs, x.n * U64_BYTES);
        while (tmp_buf.n > 0) {
            uint8_t numerical_val = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_buf, base);
            char c = (base <= 16) ? _DIGIT_INSEN_[numerical_val] : 
            _DIGIT_SEN_[numerical_val]; fputc(c, stream);
        } 
        arena_reset(_DASI_FPUTB_ARENA, tmp_mark);
    } return STR_SUCCESS;
}
dnml_status bigInt_fputf(FILE *stream, const bigInt x, uint8_t base, bool uppercase) {
    assert(__BIGINT_INTERNAL_VALID__(&x));
    test_assert(base > 1, str_inval_base, io_cleanup, STR_INVALID_BASE);
    if (x.n == 0) putchar('0');
    else if (x.n == 1) {
        if (x.sign == -1) fputc('-', stream);
        if (base == 10)         fprintf(stream, "%" PRIu64, x.limbs[0]);
        else if (base == 8)     fprintf(stream, "%#%" PRIo64, x.limbs[0]);
        else if (base == 16)    fprintf(stream, "%#%" PRIx64, x.limbs[0]);
        else if (base == 16 && uppercase) fprintf(stream, "%#%" PRIX64, x.limbs[0]);
        else if (base == 2) {
            fputs("0b", stream); uint64_t tmp_copy = x.limbs[0];
            while (tmp_copy > 0) {
                char c = (tmp_copy & 1) ? '1' : '0';
                fputc(c, stream); tmp_copy >>= 1;
            }
        } else { if (base == 64) fputs("0,", stream);
            else fprintf(stream, "0{%" PRIu8 "}", base); 
            uint8_t add_val = (uppercase) ? 16 : 0;
            uint64_t tmp_copy = x.limbs[0];
            while (tmp_copy > 0) {
                char c = (base <= 16) ? 
                    _DIGIT_INSEN_[tmp_copy % base + add_val] : 
                    _DIGIT_SEN_[tmp_copy % base];
                fputc(c, stream); tmp_copy /= base;
            }
        }
    } else {
        uint8_t additional_val = (uppercase) ? 15 : 0;
        if (x.sign == -1) fputc('-', stream);
        switch (base) {
            case 2: fputs("0b", stream); break;
            case 8: fputs("0o", stream); break;
            case 16: fputs("0x", stream); break;
            case 64: fputs("0,", stream); break;
            case 10: break;
            default: fprintf(stream, "0{%" PRIu8 "}", base); break;
        }
        dnml_arena *_DASI_FPUTF_ARENA = _USE_ARENA();
        test_assert(!(_DASI_FPUTF_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
        size_t tmp_mark = arena_mark(_DASI_FPUTF_ARENA); dnml_status err_check;
        limb_t *tmp_limbs = arena_galloc(_DASI_FPUTF_ARENA, x.n * U64_BYTES, &err_check);
        if (err_check == DNML_ALLOC_OOM); return DNML_ALLOC_OOM;

        bigInt tmp_buf = { .limbs = tmp_limbs, .sign = x.sign, .cap = x.n, .n = x.n }; 
        memcpy(tmp_limbs, x.limbs, x.n * U64_BYTES);
        while (tmp_buf.n > 0) {
            uint8_t numerical_val = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_buf, base);
            char c = (base <= 16) ? _DIGIT_INSEN_[numerical_val + additional_val] : 
            _DIGIT_SEN_[numerical_val]; fputc(c, stream);
        } 
        arena_reset(_DASI_FPUTF_ARENA, tmp_mark);
    } return STR_SUCCESS;
}
/* --------- Decimal Buffered OUTPUT ---------  */
dnml_status bigInt_sput(const bigInt x) {
    assert(__BIGINT_INTERNAL_VALID__(&x));
    if (x.n == 0) putchar('0\n');
    else if (x.n == 1) printf("%s %" PRIu64 "\n", (x.sign == -1) ? "-" : "", x.limbs[0]);
    else {
        uint8_t sign_space = (x.sign == -1) ? 1 : 0, first_iter = 1;
        size_t str_len = __BIGINT_COUNTDB__(&x, 10) + sign_space;
        if (sign_space) ___DASI_IO_CHUNKBUF_[0] = '-';
        dnml_arena *_DASI_SPUT_ARENA = _USE_ARENA();
        test_assert(!(_DASI_SPUT_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
        size_t tmp_mark = arena_mark(_DASI_SPUT_ARENA); dnml_status err_check;
        limb_t *tmp_limbs = arena_galloc(_DASI_SPUT_ARENA, x.n * U64_BYTES, &err_check);
        if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;

        bigInt tmp_buf = { .limbs = tmp_limbs, .sign = x.sign, .cap = x.n, .n = x.n };
        memcpy(tmp_limbs, x.limbs, x.n * U64_BYTES);
        while (tmp_buf.n) {
            size_t begin = (first_iter) ? sign_space : 0,
            curr_size = (str_len <= ___DASI_IO_BUFSIZE) ? str_len : ___DASI_IO_BUFSIZE;
            if (first_iter) first_iter = 0;
            for (size_t i = curr_size - 1; i >= begin; --i) {
                uint8_t numerical_value = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_buf, 10);
                ___DASI_IO_CHUNKBUF_[i] = _DIGIT_INSEN_[numerical_value];
            } 
            printf("%.*s\n", curr_size, ___DASI_IO_CHUNKBUF_);
            if (str_len >= ___DASI_IO_BUFSIZE) str_len -= ___DASI_IO_BUFSIZE;
        } 
        arena_reset(_DASI_SPUT_ARENA, tmp_mark);
    } return STR_SUCCESS;
}
dnml_status bigInt_sputb(const bigInt x, uint8_t base) {
    assert(__BIGINT_INTERNAL_VALID__(&x));
    test_assert(base > 1, str_inval_base, io_cleanup, STR_INVALID_BASE);
    if (x.n == 0) putchar('0\n');
    else if (x.n == 1) {
        if (base == 10)         printf("%s %" PRIu64 "\n", (x.sign == -1) ? "-" : "", x.limbs[0]);
        else if (base == 8)     printf("%s %" PRIo64 "\n", (x.sign == -1) ? "-" : "", x.limbs[0]);
        else if (base == 16)    printf("%s %" PRIX64 "\n", (x.sign == -1) ? "-" : "", x.limbs[0]);
        else if (base == 2) {
            uint64_t tmp_copy = x.limbs[0];
            uint8_t sign_space = (x.sign == -1) ? 1 : 0;
            uint8_t len = __BASEN_DCOUNT__(tmp_copy, 2) + sign_space;
            char c[len]; if (sign_space) c[0] = '-';
            for (uint8_t i = len - 1; i >= sign_space && tmp_copy > 0; --i) {
                c[i] = (tmp_copy & 1) ? '1' : '0';
                tmp_copy >>= 1;
            } printf("%.*s", len, c);
        } else {
            uint64_t tmp_copy = x.limbs[0];
            uint8_t sign_space = (x.sign == -1) ? 1 : 0;
            uint8_t len = __BASEN_DCOUNT__(tmp_copy, base) + sign_space;
            char c[len]; if (sign_space) c[0] = '-';
            for (uint8_t i = len - 1; i >= sign_space && tmp_copy > 0; --i) {
                c[i] = _DIGIT_INSEN_[tmp_copy % base];
                tmp_copy /= base;
            } printf("%.*s", len, c);
        }
    } else {
        uint8_t sign_space = (x.sign == -1) ? 1 : 0, first_iter = 1;
        size_t str_len = __BIGINT_COUNTDB__(&x, base) + sign_space;
        if (sign_space) ___DASI_IO_CHUNKBUF_[0] = '-';
        dnml_arena *_DASI_PUT_ARENA = _USE_ARENA();
        test_assert(!(_DASI_PUT_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
        size_t tmp_mark = arena_mark(_DASI_PUT_ARENA); dnml_status err_check;
        limb_t *tmp_limbs = arena_galloc(_DASI_PUT_ARENA, x.n * U64_BYTES, &err_check);
        if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;

        bigInt tmp_buf = { .limbs = tmp_limbs, .sign = x.sign, .cap = x.n, .n = x.n }; 
        memcpy(tmp_limbs, x.limbs, x.n * U64_BYTES);
        while (tmp_buf.n) {
            size_t begin = (first_iter) ? sign_space : 0,
            curr_size = (str_len <= ___DASI_IO_BUFSIZE) ? str_len : ___DASI_IO_BUFSIZE;
            if (first_iter) first_iter = 0;

            for (size_t i = curr_size - 1; i >= begin; --i) {
                uint8_t numerical_value = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_buf, base);
                ___DASI_IO_CHUNKBUF_[i] = (base <= 16) ? 
                    _DIGIT_INSEN_[numerical_value] : 
                    _DIGIT_SEN_[numerical_value];
            } 
            printf("%.*s", curr_size, ___DASI_IO_CHUNKBUF_);
            if (str_len >= ___DASI_IO_BUFSIZE) str_len -= ___DASI_IO_BUFSIZE;
        }
        arena_reset(_DASI_PUT_ARENA, tmp_mark);
    } return STR_SUCCESS;
}
dnml_status bigInt_sputf(const bigInt x, uint8_t base, bool uppercase) {
    assert(__BIGINT_INTERNAL_VALID__(&x));
    test_assert(base > 1, str_inval_base, io_cleanup, STR_INVALID_BASE);
    if (x.n == 0) putchar('0\n');
    else if (x.n == 1) {
        if (base == 10)         printf("%s %" PRIu64 "\n", (x.sign == -1) ? "-" : "", x.limbs[0]);
        else if (base == 8)     printf("%s %#%" PRIo64 "\n", (x.sign == -1) ? "-" : "", x.limbs[0]);
        else if (base == 16)    printf("%s %#%" PRIX64 "\n", (x.sign == -1) ? "-" : "", x.limbs[0]);
        else if (base == 16 && uppercase) {
            printf("%s %#%" PRIX64 "\n", (x.sign == -1) ? "-" : "", x.limbs[0]);
        } else if (base == 2) {
            uint64_t tmp_copy = x.limbs[0];
            uint8_t len = __BASEN_DCOUNT__(tmp_copy, 2); char c[len];
            for (uint8_t i = len - 1; i >= 0 && tmp_copy > 0; --i) {
                c[i] = (tmp_copy & 1) ? '1' : '0';
                tmp_copy >>= 1;
            } printf("%s0b%.*s\n", (x.sign == -1) ? "-" : "", len, c);
        } else {
            uint64_t tmp_copy = x.limbs[0];
            uint8_t len = __BASEN_DCOUNT__(tmp_copy, base); char c[len];
            for (uint8_t i = len - 1; i >= 0 && tmp_copy > 0; --i) {
                c[i] = (base <= 16) ? 
                    _DIGIT_INSEN_[tmp_copy % base] : 
                    _DIGIT_SEN_[tmp_copy & base];
                tmp_copy /= base;
            }
            if (base == 64) printf("%s0,%.*s\n", (x.sign == -1) ? "-" : "", base, len, c);
            else printf("%s0{%" PRIu8 "}%.*s\n", (x.sign == -1) ? "-" : "", base, len, c);
        }
    } else {
        uint8_t sign_space = (x.sign == -1) ? 1 : 0, 
        addval = (uppercase) ? 16 : 0, bpsub = (uppercase) ? 32 : 0,
        prefix_space = (base == 10) ? 0 : ((base == 16 || base == 8 || base == 2) ? 2 : 
            ((base < 10) ? 4 : ((base < 100) ? 5 : 6))
        ); size_t str_len = __BIGINT_COUNTDB__(&x, base) + sign_space + prefix_space; 
        if (sign_space) ___DASI_IO_CHUNKBUF_[0] = '-';
        if (prefix_space) {
            ___DASI_IO_CHUNKBUF_[sign_space] = '0';
            switch (base) {
                case 64:    ___DASI_IO_CHUNKBUF_[sign_space + 1] = ','; break;
                case 16:    ___DASI_IO_CHUNKBUF_[sign_space + 1] = 'x' - bpsub; break;
                case 2:     ___DASI_IO_CHUNKBUF_[sign_space + 1] = 'b' - bpsub; break;
                case 8:     ___DASI_IO_CHUNKBUF_[sign_space + 1] = 'o' - bpsub; break;
                default: { uint8_t temp_base = base, i = 1;
                    ___DASI_IO_CHUNKBUF_[sign_space + i] = '{';
                    for (; i < 5 && temp_base; ++i) {
                        ___DASI_IO_CHUNKBUF_[sign_space + i] = (char)(temp_base % 10); 
                        base /= 10;
                    } ___DASI_IO_CHUNKBUF_[sign_space + i] = '}';
                } break;
            }
        } dnml_arena *_DASI_PUT_ARENA = _USE_ARENA();
        test_assert(!(_DASI_PUT_ARENA), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
        size_t tmp_mark = arena_mark(_DASI_PUT_ARENA); dnml_status err_check;
        limb_t *tmp_limbs = arena_galloc(_DASI_PUT_ARENA, x.n * U64_BYTES, &err_check);
        bigInt tmp_buf = { .limbs = tmp_limbs, .sign = x.sign, .cap = x.n, .n = x.n }; 
        memcpy(tmp_limbs, x.limbs, x.n * U64_BYTES);

        bpsub = 1; // bpsub is re-used to track if the outer while-loop is n its first iteration or not
        while (tmp_buf.n) {
            size_t begin = (bpsub) ? sign_space + prefix_space : 0,
            curr_size = (str_len <= ___DASI_IO_BUFSIZE) ? str_len : ___DASI_IO_BUFSIZE;
            if (bpsub) bpsub = 0; // Reset bpsub to FALSE

            for (size_t i = curr_size - 1; i >= begin; --i) {
                uint8_t numerical_value = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_buf, base);
                ___DASI_IO_CHUNKBUF_[i] = (base <= 16) ?
                    _DIGIT_INSEN_[numerical_value + addval] :
                    _DIGIT_SEN_[numerical_value];
            } 
            printf("%.*s\n", curr_size, ___DASI_IO_CHUNKBUF_);
            if (str_len >= ___DASI_IO_BUFSIZE) str_len -= ___DASI_IO_BUFSIZE;
        } 
        arena_reset(_DASI_PUT_ARENA, tmp_mark);
    } return STR_SUCCESS;
}
dnml_status bigInt_sfput(FILE *stream, const bigInt x) {
    assert(__BIGINT_INTERNAL_VALID__(&x));
    if (x.n == 0) fputc('0\n', stream);
    else if (x.n == 1) fprintf(stream, "%s %" PRIu64 "\n", (x.sign == -1) ? "-" : "", x.limbs[0]);
    else {
        uint8_t sign_space = (x.sign == -1) ? 1 : 0, first_iter = 1;
        size_t str_len = __BIGINT_COUNTDB__(&x, 10) + sign_space;
        if (sign_space) ___DASI_IO_CHUNKBUF_[0] = '-';
        dnml_arena *_DASI_SPUT_ARENA = _USE_ARENA();
        test_assert(!(_DASI_SPUT_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
        size_t tmp_mark = arena_mark(_DASI_SPUT_ARENA); dnml_status err_check;
        limb_t *tmp_limbs = arena_galloc(_DASI_SPUT_ARENA, x.n * U64_BYTES, &err_check);
        if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;

        bigInt tmp_buf = { .limbs = tmp_limbs, .sign = x.sign, .cap = x.n, .n = x.n }; 
        memcpy(tmp_limbs, x.limbs, x.n * U64_BYTES);
        while (tmp_buf.n) {
            size_t begin = (first_iter) ? sign_space : 0,
            curr_size = (str_len <= ___DASI_IO_BUFSIZE) ? str_len : ___DASI_IO_BUFSIZE;
            if (first_iter) first_iter = 0;
            for (size_t i = curr_size - 1; i >= begin; --i) {
                uint8_t numerical_value = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_buf, 10);
                ___DASI_IO_CHUNKBUF_[i] = _DIGIT_INSEN_[numerical_value];
            } fprintf(stream, "%.*s\n", curr_size, ___DASI_IO_CHUNKBUF_);
            if (str_len >= ___DASI_IO_BUFSIZE) str_len -= ___DASI_IO_BUFSIZE;
        }
        arena_reset(_DASI_SPUT_ARENA, tmp_mark);
    } return STR_SUCCESS;
}
dnml_status bigInt_sfputb(FILE *stream, const bigInt x, uint8_t base) {
    assert(__BIGINT_INTERNAL_VALID__(&x));
    test_assert(base > 1, str_inval_base, io_cleanup, STR_INVALID_BASE);
    if (x.n == 0) fputc('0\n', stream);
    else if (x.n == 1) {
        if (base == 10)         fprintf(stream, "%s %" PRIu64 "\n", (x.sign == -1) ? "-" : "", x.limbs[0]);
        else if (base == 8)     fprintf(stream, "%s %" PRIo64 "\n", (x.sign == -1) ? "-" : "", x.limbs[0]);
        else if (base == 16)    fprintf(stream, "%s %" PRIX64 "\n", (x.sign == -1) ? "-" : "", x.limbs[0]);
        else if (base == 2) {
            uint64_t tmp_copy = x.limbs[0];
            uint8_t sign_space = (x.sign == -1) ? 1 : 0;
            uint8_t len = __BASEN_DCOUNT__(tmp_copy, 2) + sign_space;
            char c[len]; if (sign_space) c[0] = '-';
            for (uint8_t i = len - 1; i >= sign_space && tmp_copy > 0; --i) {
                c[i] = (tmp_copy & 1) ? '1' : '0';
                tmp_copy >>= 1;
            } fprintf(stream, "%.*s\n", len, c);
        } else {
            uint64_t tmp_copy = x.limbs[0];
            uint8_t sign_space = (x.sign == -1) ? 1 : 0;
            uint8_t len = __BASEN_DCOUNT__(tmp_copy, base) + sign_space;
            char c[len]; if (sign_space) c[0] = '-';
            for (uint8_t i = len - 1; i >= sign_space && tmp_copy > 0; --i) {
                c[i] = _DIGIT_INSEN_[tmp_copy % base];
                tmp_copy /= base;
            } fprintf(stream, "%.*s\n", len, c);
        }
    } else {
        uint8_t sign_space = (x.sign == -1) ? 1 : 0, first_iter = 1;
        size_t str_len = __BIGINT_COUNTDB__(&x, base) + sign_space;
        if (sign_space) ___DASI_IO_CHUNKBUF_[0] = '-';
        dnml_arena *_DASI_PUT_ARENA = _USE_ARENA();
        test_assert(!(_DASI_PUT_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
        size_t tmp_mark = arena_mark(_DASI_PUT_ARENA); dnml_status err_check;
        limb_t *tmp_limbs = arena_galloc(_DASI_PUT_ARENA, x.n * U64_BYTES, &err_check);
        if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
        
        bigInt tmp_buf = { .limbs = tmp_limbs, .sign = x.sign, .cap = x.n, .n = x.n }; 
        memcpy(tmp_limbs, x.limbs, x.n * U64_BYTES);
        while (tmp_buf.n) {
            size_t begin = (first_iter) ? sign_space : 0,
            curr_size = (str_len <= ___DASI_IO_BUFSIZE) ? str_len : ___DASI_IO_BUFSIZE;
            if (first_iter) first_iter = 0;

            for (size_t i = str_len - 1; i >= sign_space; --i) {
                uint8_t numerical_value = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_buf, base);
                ___DASI_IO_CHUNKBUF_[i] = (base <= 16) ? 
                    _DIGIT_INSEN_[numerical_value] :
                     _DIGIT_SEN_[numerical_value];
            }
            fprintf(stream, "%.*s\n", str_len, ___DASI_IO_CHUNKBUF_); 
        }
        arena_reset(_DASI_PUT_ARENA, tmp_mark);
    } return STR_SUCCESS;
}
dnml_status bigInt_sfputf(FILE *stream, const bigInt x, uint8_t base, bool uppercase) {
    assert(__BIGINT_INTERNAL_VALID__(&x));
    test_assert(base > 1, str_inval_base, io_cleanup, STR_INVALID_BASE);
    if (x.n == 0) fputc('0\n', stream);
    else if (x.n == 1) {
        if (base == 10)         fprintf(stream, "%s %" PRIu64 "\n", (x.sign == -1) ? "-" : "", x.limbs[0]);
        else if (base == 8)     fprintf(stream, "%s %#%" PRIo64 "\n", (x.sign == -1) ? "-" : "", x.limbs[0]);
        else if (base == 16)    fprintf(stream, "%s %#%" PRIx64 "\n", (x.sign == -1) ? "-" : "", x.limbs[0]);
        else if (base == 16 && uppercase) {
            fprintf(stream, "%s %#%" PRIX64 "\n", (x.sign == -1) ? "-" : "", x.limbs[0]);
        } else if (base == 2) {
            uint64_t tmp_copy = x.limbs[0];
            uint8_t len = __BASEN_DCOUNT__(tmp_copy, 2); char c[len];
            for (uint8_t i = len - 1; i >= 0 && tmp_copy > 0; --i) {
                c[i] = (tmp_copy & 1) ? '1' : '0';
                tmp_copy >>= 1;
            } fprintf(stream, "%s0b%.*s\n", (x.sign == -1) ? "-" : "", len, c);
        } else {
            uint64_t tmp_copy = x.limbs[0]; uint8_t add_val = (uppercase) ? 16 : 0,
            len = __BASEN_DCOUNT__(tmp_copy, base); char c[len];
            for (uint8_t i = len - 1; i >= 0 && tmp_copy > 0; --i) {
                c[i] = _DIGIT_INSEN_[tmp_copy % base + add_val];
                tmp_copy /= base;
            }
            if (base == 64) fprintf(stream, "%s0,%.*s\n", (x.sign == -1) ? "-" : "", base, len, c);
            else fprintf(stream, "%s0{%" PRIu8 "}%.*s\n", (x.sign == -1) ? "-" : "", base, len, c);
        }
    } else {
        uint8_t sign_space = (x.sign == -1) ? 1 : 0,
        add_val = (uppercase) ? 16 : 0, psub = (uppercase) ? 32 : 0;
        uint8_t prefix_space = (base == 10) ? 0 : ((base == 16 || base == 8 || base == 2) ? 2 : 
            (base < 10) ? 4 : ((base < 100) ? 5 : 6)
        ); size_t str_len = __BIGINT_COUNTDB__(&x, base) + sign_space + prefix_space;
        if (sign_space) ___DASI_IO_CHUNKBUF_[0] = '-';
        if (prefix_space) { ___DASI_IO_CHUNKBUF_[sign_space] = '0';
            switch (base) {
                case 64:    ___DASI_IO_CHUNKBUF_[sign_space + 1] = ','; break;
                case 16:    ___DASI_IO_CHUNKBUF_[sign_space + 1] = (char)('x' - psub); break;
                case 2:     ___DASI_IO_CHUNKBUF_[sign_space + 1] = (char)('b' - psub); break;
                case 8:     ___DASI_IO_CHUNKBUF_[sign_space + 1] = (char)('o' - psub); break;
                default: { uint8_t temp_base = base, i = 1;
                    ___DASI_IO_CHUNKBUF_[sign_space + i] = '{';
                    for (; i < 5 && temp_base; ++i) {
                        ___DASI_IO_CHUNKBUF_[sign_space + i] = (char)(temp_base % 10);
                        base /= 10;
                    } ___DASI_IO_CHUNKBUF_[sign_space + i] = '}'; break;
                } break;
            }
        } dnml_arena *_DASI_PUT_ARENA = _USE_ARENA();
        test_assert(!(_DASI_PUT_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
        size_t tmp_mark = arena_mark(_DASI_PUT_ARENA); dnml_status err_check;
        limb_t *tmp_limbs = arena_galloc(_DASI_PUT_ARENA, x.n * U64_BYTES, &err_check);
        if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;

        bigInt tmp_buf = { .limbs = tmp_limbs, .sign = x.sign, .cap = x.n, .n = x.n }; 
        memcpy(tmp_limbs, x.limbs, x.n * U64_BYTES); 
        psub = 1; // Psub is re-used to track if the outer while-loop is on its first iteration or not
        while (tmp_buf.n) {
            size_t begin = (psub) ? sign_space + prefix_space : 0,
            curr_size = (str_len <= ___DASI_IO_BUFSIZE) ? str_len : ___DASI_IO_BUFSIZE;
            if (psub) psub = 0; // Reset psub to FALSE
            for (size_t i = curr_size - 1; i >= begin; --i) {
                uint8_t numerical_value = __BIGINT_INTERNAL_DIVMOD_UI64__(&tmp_buf, base);
                ___DASI_IO_CHUNKBUF_[i] = (base <= 16) ?
                    _DIGIT_INSEN_[numerical_value + add_val] : 
                    _DIGIT_SEN_[numerical_value];
            }
            fprintf(stream, "%.*s\n", curr_size, ___DASI_IO_CHUNKBUF_);
            if (str_len >= ___DASI_IO_BUFSIZE) str_len -= ___DASI_IO_BUFSIZE;
        } 
        arena_reset(_DASI_PUT_ARENA, tmp_mark);
    } return STR_SUCCESS;
}
/* --------- Standard Stream (stdin) INPUT ---------  */
dnml_status bigInt_scan(bigInt *x) {                            //* Heap-allocated Temporary
    assert(__BIGINT_INTERNAL_SVALID__(x));
    //* Whitespace & Sign *//
    uint16_t current_char = _fskip_whitespace__(stdin); uint8_t sign = 1;
    if (current_char == '-') { sign = -1; current_char = getchar(); }
    else if (current_char == '+') { current_char = getchar(); }
    else if (!isdigit(current_char)) return STR_INVALID_SIGN;
    // ---> Forcing it to be prefix/leading zero/decimal value valid

    //* Base-prefix & Leading Zeros *//
    uint8_t base = 10, prefix_res = _prefix_handle_stream__(stdin, &base, &current_char);
    if (prefix_res == 1);
    else if (prefix_res == 0) {
        if (sign == -1) return STR_INVALID_SIGN;
        else __BIGINT_INTERNAL_ZSET__(x);
    } else if (prefix_res == 2) return STR_INVALID_BASE_PREFIX;
    else if (prefix_res == 4) return STR_INVALID_BASE;
    // Leading Zeros
    while ((current_char = getchar()) != EOF && !isspace(current_char) && current_char == '0');
    if (current_char == EOF || isspace(current_char)) {
        if (sign == -1) return STR_INVALID_SIGN;
        else __BIGINT_INTERNAL_ZSET__(x);
    }

    //* Main accumulator loop *//
    uint64_t threshold; uint8_t index_lookup, numerical_val; bigInt tmp_buf; 
    if (__BIGINT_INTERNAL_LINIT__(&tmp_buf, x->cap) == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    while (_is_valid_digit__(&current_char)) {
        index_lookup = (uint8_t)(current_char - '\0');
        numerical_val = (base <= 16) ? 
            _VALUE_LOOKUP_INSEN_[index_lookup] : 
            _VALUE_LOOKUP_SEN_[index_lookup];
        threshold = (UINT64_MAX - numerical_val) / base;
        if (numerical_val >= base) { 
            __BIGINT_INTERNAL_FREE__(&tmp_buf); 
            return STR_INVALID_DIGIT; 
        } if (__BIGINT_WILL_OVERFLOW__(&tmp_buf, threshold)) {
            // Grows geometrically internally anyways
            if (__BIGINT_INTERNAL_ENSCAP__(&tmp_buf, tmp_buf.n + 1) == DNML_ALLOC_OOM) {
                return DNML_ALLOC_OOM; // No need to free any resources, no resources are even allocated
            }
        }
        __BIGINT_INTERNAL_MUL_UI64__(&tmp_buf, base);
        __BIGINT_INTERNAL_ADD_UI64__(&tmp_buf, numerical_val);
        current_char = getchar();
    }
    __BIGINT_INTERNAL_COPY__(x, &tmp_buf); x->sign = sign;
    __BIGINT_INTERNAL_FREE__(&tmp_buf);
    return STR_SUCCESS;
}
dnml_status bigInt_scanb(bigInt *x, uint8_t base) {             //* Heap-allocated Temporary
    assert(__BIGINT_INTERNAL_SVALID__(x));
    test_assert(base > 1, str_inval_base, io_cleanup, STR_INVALID_BASE);
    //* Whitespace, Sign, & Leading zeros *//
    uint16_t current_char = _fskip_whitespace__(stdin); uint8_t sign = 1;
    if (current_char == '-') { sign = -1; current_char = getchar(); }
    else if (current_char == '+') { current_char = getchar(); }
    else if (!isdigit(current_char)) return STR_INVALID_SIGN;
    // ---> Forcing it to be leading zero/decimal value valid
    while ((current_char = getchar()) != EOF && !isspace(current_char) && current_char == '0');
    if (current_char == EOF || isspace(current_char)) {
        if (sign == -1) return STR_INVALID_SIGN;
        else __BIGINT_INTERNAL_ZSET__(x);
    }

    //* Main accumulator loop *//
    uint64_t threshold; uint8_t index_lookup, numerical_val; bigInt tmp_buf; 
    if (__BIGINT_INTERNAL_LINIT__(&tmp_buf, x->cap) == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    while (_is_valid_digit__(&current_char)) {
        index_lookup = (uint8_t)(current_char - '\0');
        numerical_val = (base <= 16) ? 
            _VALUE_LOOKUP_INSEN_[index_lookup] : 
            _VALUE_LOOKUP_SEN_[index_lookup];
        threshold = (UINT64_MAX - numerical_val) / base;
        if (numerical_val >= base) { 
            __BIGINT_INTERNAL_FREE__(&tmp_buf); 
            return STR_INVALID_DIGIT; 
        } if (__BIGINT_WILL_OVERFLOW__(&tmp_buf, threshold)) {
            // Grows geometrically internally anyways
            if (__BIGINT_INTERNAL_ENSCAP__(&tmp_buf, tmp_buf.n + 1) == DNML_ALLOC_OOM) {
                return DNML_ALLOC_OOM; // No resources were allocated
            }
        }
        __BIGINT_INTERNAL_MUL_UI64__(&tmp_buf, base);
        __BIGINT_INTERNAL_ADD_UI64__(&tmp_buf, numerical_val);
        current_char = getchar();
    }
    __BIGINT_INTERNAL_COPY__(x, &tmp_buf); x->sign = sign;
    __BIGINT_INTERNAL_FREE__(&tmp_buf);
    return STR_SUCCESS;
}
dnml_status bigInt_sscan(bigInt *x) {
    assert(__BIGINT_INTERNAL_SVALID__(x));
    dnml_arena *_DASI_SGET_ARENA = _USE_ARENA();
    test_assert(!(_DASI_SGET_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    //* Whitespace & Sign *//
    uint16_t current_char = _fskip_whitespace__(stdin); uint8_t sign = 1;
    if (current_char == '-') { sign = -1; current_char = getchar(); }
    else if (current_char == '+') { current_char = getchar(); }
    else if (!isdigit(current_char)) return STR_INVALID_SIGN;
    // ---> Forcing it to be prefix/leading zero/decimal value valid

    //* Base-prefix & Leading Zeros *//
    uint8_t base = 10;
    uint8_t prefix_res = _prefix_handle_stream__(stdin, &base, &current_char);
    if (prefix_res == 1);
    else if (prefix_res == 0) {
        if (sign == -1) return STR_INVALID_SIGN;
        else __BIGINT_INTERNAL_ZSET__(x);
    } else if (prefix_res == 2) return STR_INVALID_BASE_PREFIX;
    else if (prefix_res == 4) return STR_INVALID_BASE;
    // Leading Zeros
    while ((current_char = getchar()) != EOF && !isspace(current_char) && current_char == '0');
    if (current_char == EOF || isspace(current_char)) {
        if (sign == -1) return STR_INVALID_SIGN;
        else __BIGINT_INTERNAL_ZSET__(x);
    }

    //* Main accumulator loop *//
    dnml_status err_check;
    uint64_t threshold; uint8_t index_lookup, numerical_val;
    size_t tmp_mark = arena_mark(_DASI_SGET_ARENA);
    limb_t *tmp_limbs = arena_galloc(_DASI_SGET_ARENA, x->cap, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;

    bigInt tmp_buf = { .limbs = tmp_limbs, .sign = 1, .n = 0, .cap = x->cap };
    while (_is_valid_digit__(&current_char)) {
        index_lookup = (uint8_t)(current_char - '\0');
        numerical_val = (base <= 16) ? 
            _VALUE_LOOKUP_INSEN_[index_lookup] : 
            _VALUE_LOOKUP_SEN_[index_lookup];
        threshold = (UINT64_MAX - numerical_val) / base;
        if (numerical_val >= base) { 
            // Invalid Digit from the user
            arena_reset(_DASI_SGET_ARENA, tmp_mark);
            tmp_limbs = NULL; return STR_INVALID_DIGIT; 
        } if (tmp_buf.n == tmp_buf.cap && tmp_buf.limbs[tmp_buf.n - 1] > threshold) {
            // Overflow/Too small
            arena_reset(_DASI_SGET_ARENA, tmp_mark);
            tmp_limbs = NULL; return BIGINT_ERR_RANGE;
        }
        __BIGINT_INTERNAL_MUL_UI64__(&tmp_buf, base);
        __BIGINT_INTERNAL_ADD_UI64__(&tmp_buf, numerical_val);
        current_char = getchar();
    }
    __BIGINT_INTERNAL_COPY__(x, &tmp_buf); x->sign = sign;
    arena_reset(_DASI_SGET_ARENA, tmp_mark);
    tmp_limbs = NULL; return STR_SUCCESS;
}
dnml_status bigInt_sscanb(bigInt *x, uint8_t base) {
    assert(__BIGINT_INTERNAL_SVALID__(x));
    test_assert(base > 1, str_inval_base, io_cleanup, STR_INVALID_BASE);
    dnml_arena *_DASI_SGETB_ARENA = _USE_ARENA();
    test_assert(!(_DASI_SGETB_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    //* Whitespace, Sign, & Leading Zeros *//
    uint16_t current_char = _fskip_whitespace__(stdin); uint8_t sign = 1;
    if (current_char == '-') { sign = -1; current_char = getchar(); }
    else if (current_char == '+') { current_char = getchar(); }
    else if (!isdigit(current_char)) return STR_INVALID_SIGN;
    // ---> Forcing it to be leading zero/decimal value valid
    while ((current_char = getchar()) != EOF && !isspace(current_char) && current_char == '0');
    if (current_char == EOF || isspace(current_char)) {
        if (sign == -1) return STR_INVALID_SIGN;
        else __BIGINT_INTERNAL_ZSET__(x);
    }

    //* Main accumulator loop *//
    dnml_status err_check;
    uint64_t threshold; uint8_t index_lookup, numerical_val;
    size_t tmp_mark = arena_mark(_DASI_SGETB_ARENA);
    limb_t *tmp_limbs = arena_galloc(_DASI_SGETB_ARENA, x->cap, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;

    bigInt tmp_buf = { .limbs = tmp_limbs, .sign = 1, .n = 0, .cap = x->cap };
    while (_is_valid_digit__(&current_char)) {
        index_lookup = (uint8_t)(current_char - '\0');
        numerical_val = (base <= 16) ? 
            _VALUE_LOOKUP_INSEN_[index_lookup] : 
            _VALUE_LOOKUP_SEN_[index_lookup];
        threshold = (UINT64_MAX - numerical_val) / base;
        if (numerical_val >= base) { 
            // Invalid Digit from the user
            arena_reset(_DASI_SGETB_ARENA, tmp_mark);
            tmp_limbs = NULL; return STR_INVALID_DIGIT; 
        } if (tmp_buf.n == tmp_buf.cap && tmp_buf.limbs[tmp_buf.n - 1] > threshold) {
            // Overflow/Too small
            arena_reset(_DASI_SGETB_ARENA, tmp_mark);
            tmp_limbs = NULL; return BIGINT_ERR_RANGE;
        }
        __BIGINT_INTERNAL_MUL_UI64__(&tmp_buf, base);
        __BIGINT_INTERNAL_ADD_UI64__(&tmp_buf, numerical_val);
        current_char = getchar();
    }
    __BIGINT_INTERNAL_COPY__(x, &tmp_buf); x->sign = sign;
    arena_reset(_DASI_SGETB_ARENA, tmp_mark);
    tmp_limbs = NULL; return STR_SUCCESS;
}
dnml_status bigInt_tscan(bigInt *x) {
    assert(__BIGINT_INTERNAL_SVALID__(x));
    dnml_arena *_DASI_TGET_ARENA = _USE_ARENA();
    test_assert(!(_DASI_TGET_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    //* Whitespace & Sign *//
    uint16_t current_char = _fskip_whitespace__(stdin); uint8_t sign = 1;
    if (current_char == '-') { sign = -1; current_char = getchar(); }
    else if (current_char == '+') { current_char = getchar(); }
    else if (!isdigit(current_char)) return STR_INVALID_SIGN;
    // ---> Forcing it to be prefix/leading zero/decimal value valid

    //* Base-prefix & Leading Zeros *//
    uint8_t base = 10;
    uint8_t prefix_res = _prefix_handle_stream__(stdin, &base, &current_char);
    if (prefix_res == 1);
    else if (prefix_res == 0) {
        if (sign == -1) return STR_INVALID_SIGN;
        else __BIGINT_INTERNAL_ZSET__(x);
    } else if (prefix_res == 2) return STR_INVALID_BASE_PREFIX;
    else if (prefix_res == 4) return STR_INVALID_BASE;
    // Leading Zeros
    while ((current_char = getchar()) != EOF && !isspace(current_char) && current_char == '0');
    if (current_char == EOF || isspace(current_char)) {
        if (sign == -1) return STR_INVALID_SIGN;
        else __BIGINT_INTERNAL_ZSET__(x);
    }

    //* Main accumulator loop *//
    dnml_status err_check;
    uint64_t threshold; uint8_t index_lookup, numerical_val;
    size_t tmp_mark = arena_mark(_DASI_TGET_ARENA);
    limb_t *tmp_limbs = arena_galloc(_DASI_TGET_ARENA, x->cap, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;

    bigInt tmp_buf = { .limbs = tmp_limbs, .sign = 1, .n = 0, .cap = x->cap};
    while (_is_valid_digit__(&current_char)) {
        index_lookup = (uint8_t)(current_char - '\0');
        numerical_val = (base <= 16) ? 
            _VALUE_LOOKUP_INSEN_[index_lookup] : 
            _VALUE_LOOKUP_SEN_[index_lookup];
        threshold = (UINT64_MAX - numerical_val) / base;
        if (numerical_val >= base) { 
            // Invalid Digit from the user
            arena_reset(_DASI_TGET_ARENA, tmp_mark);
            tmp_limbs = NULL; return STR_INVALID_DIGIT; 
        }
        // Not in the loop condition since
        // we have to update the threshold first. 
        if (__BIGINT_WILL_OVERFLOW__(&tmp_buf, threshold)) break;
        __BIGINT_INTERNAL_MUL_UI64__(&tmp_buf, base);
        __BIGINT_INTERNAL_ADD_UI64__(&tmp_buf, numerical_val);
        current_char = getchar();
    } dnml_status ret = STR_SUCCESS;
    if (__BIGINT_WILL_OVERFLOW__(&tmp_buf, threshold)) ret = STR_TRUNC_SUCCESS;
    __BIGINT_INTERNAL_COPY__(x, &tmp_buf); x->sign = sign;
    arena_reset(_DASI_TGET_ARENA, tmp_mark);
    tmp_limbs = NULL; return ret;
}
dnml_status bigInt_tscanb(bigInt *x, uint8_t base) {
    assert(__BIGINT_INTERNAL_SVALID__(x));
    test_assert(base > 1, str_inval_base, io_cleanup, STR_INVALID_BASE);
    dnml_arena *_DASI_TGETB_ARENA = _USE_ARENA();
    test_assert(!(_DASI_TGETB_ARENA->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    //* Whitespace, Sign, & Leading zeros *//
    uint16_t current_char = _fskip_whitespace__(stdin); uint8_t sign = 1;
    if (current_char == '-') { sign = -1; current_char = getchar(); }
    else if (current_char == '+') { current_char = getchar(); }
    else if (!isdigit(current_char)) return STR_INVALID_SIGN;
    // ---> Forcing it to be leading zero/decimal value valid
    while ((current_char = getchar()) != EOF && !isspace(current_char) && current_char == '0');
    if (current_char == EOF || isspace(current_char)) {
        if (sign == -1) return STR_INVALID_SIGN;
        else __BIGINT_INTERNAL_ZSET__(x);
    }

    //* Main accumulator loop *//
    dnml_status err_check;
    uint64_t threshold; uint8_t index_lookup, numerical_val;
    size_t tmp_mark = arena_mark(_DASI_TGETB_ARENA);
    limb_t *tmp_limbs = arena_galloc(_DASI_TGETB_ARENA, x->cap, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    
    bigInt tmp_buf = { .limbs = tmp_limbs, .sign = 1, .n = 0, .cap = x->cap };
    while (_is_valid_digit__(&current_char)) {
        index_lookup = (uint8_t)(current_char - '\0');
        numerical_val = (base <= 16) ? 
            _VALUE_LOOKUP_INSEN_[index_lookup] :
            _VALUE_LOOKUP_SEN_[index_lookup];
        threshold = (UINT64_MAX - numerical_val) / base;
        if (numerical_val >= base) { 
            // Invalid Digit from the user
            arena_reset(_DASI_TGETB_ARENA, tmp_mark);
            tmp_limbs = NULL; return STR_INVALID_DIGIT; 
        }
        // Not in the loop condition since
        // we have to update the threshold first. 
        if (__BIGINT_WILL_OVERFLOW__(&tmp_buf, threshold)) break;
        __BIGINT_INTERNAL_MUL_UI64__(&tmp_buf, base);
        __BIGINT_INTERNAL_ADD_UI64__(&tmp_buf, numerical_val);
        current_char = getchar();
    } dnml_status ret = STR_SUCCESS;
    if (__BIGINT_WILL_OVERFLOW__(&tmp_buf, threshold)) ret = STR_TRUNC_SUCCESS;
    __BIGINT_INTERNAL_COPY__(x, &tmp_buf); x->sign = sign;
    arena_reset(_DASI_TGETB_ARENA, tmp_mark);
    tmp_limbs = NULL; return ret;
}
/* --------- Custom Stream INPUT ---------  */
dnml_status bigInt_fscan(FILE *stream, bigInt *x) {                     //* Heap-allocated Temporary
    assert(__BIGINT_INTERNAL_SVALID__(x)); clearerr(stream);
    /* Whitespace */ int curr_char;
    while ((curr_char = fgetc(stream)) != EOF && isspace(curr_char)) 
    scan_eof(curr_char, stream, STR_EMPTY);
    
    //* Signs *//
    uint8_t sign = 1, base = 10;
    if (curr_char == '-') { sign = -1; curr_char = fgetc(stream); }
    else if (curr_char == '+') curr_char = fgetc(stream);
    else if (!isdigit(curr_char)) return STR_INVALID_SIGN;
    scan_eof(curr_char, stream, STR_INCOMPLETE);

    //* Prefix & leading zeros *//
    uint8_t bp_res = _prefix_handle_stream__(stream, &base, &curr_char);
    if (bp_res == 1);
    else if (bp_res == 3) return STR_INCOMPLETE;
    else if (!bp_res) {
        if (sign == -1) return STR_INVALID_SIGN;
        __BIGINT_INTERNAL_ZSET__(x); return STR_SUCCESS;
    } else if (bp_res == 2) return STR_INVALID_BASE_PREFIX;
    else if (bp_res == 4) return STR_INVALID_BASE; // Overflows
    // Leading Zeros handling
    while (curr_char != EOF && curr_char == '0') curr_char = fgetc(stream);
    if (curr_char == EOF) {
        if (ferror(stream)) return FILE_ERR_PARSE;
        if (sign == -1) return STR_INVALID_SIGN;
        __BIGINT_INTERNAL_ZSET__(x); return STR_SUCCESS;
    }

    //* Main accumalator loop *//
    uint64_t threshold; size_t i = 0, parse_res;
    uint8_t index_lookup, numerical_val, end = 0;
    bigInt tmp_buf; 
    if (__BIGINT_INTERNAL_LINIT__(&tmp_buf, x->cap) == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    while (!end) {
        parse_res = fread(___DASI_IO_CHUNKBUF_, sizeof(char), ___DASI_IO_BUFSIZE, stream);
        //* THE ACTUAL ACCUMALATION
        if (parse_res > 0) {
            for (i = 0; i < parse_res; ++i) {
                // Delimiter Handling
                if (___DASI_IO_CHUNKBUF_[i] == '\n') {
                    fseek(stream, -(long)(parse_res - i), SEEK_CUR);
                    end = 1; break;
                }
                index_lookup = (uint8_t)(___DASI_IO_CHUNKBUF_[i] - '\0');
                numerical_val = (base <= 16) ?
                    _VALUE_LOOKUP_INSEN_[index_lookup] :
                    _VALUE_LOOKUP_SEN_[index_lookup];
                threshold = (UINT64_MAX - numerical_val) / base;
                if (numerical_val >= base) { __BIGINT_INTERNAL_FREE__(&tmp_buf); return STR_INVALID_DIGIT; } 
                if (__BIGINT_WILL_OVERFLOW__(&tmp_buf, threshold)) {
                    if (__BIGINT_INTERNAL_ENSCAP__(&tmp_buf, tmp_buf.cap + 5) == DNML_ALLOC_OOM) {
                        return DNML_ALLOC_OOM; // No resources were allocated
                    }
                }
                __BIGINT_INTERNAL_MUL_UI64__(&tmp_buf, base);
                __BIGINT_INTERNAL_ADD_UI64__(&tmp_buf, numerical_val);
            }
        }
        //* ENDING CONDITION
        if (parse_res < ___DASI_IO_BUFSIZE) {
            if (ferror(stream)) {
                __BIGINT_INTERNAL_FREE__(&tmp_buf);
                return FILE_ERR_PARSE;
            } else if (feof(stream)) end = 1;
        }
    }
    __BIGINT_INTERNAL_COPY__(x, &tmp_buf); x->sign = sign;
    __BIGINT_INTERNAL_FREE__(&tmp_buf);
    return STR_SUCCESS;
}
dnml_status bigInt_fscanb(FILE *stream, bigInt *x, uint8_t base) {      //* Heap-allocated Temporary
    assert(__BIGINT_INTERNAL_SVALID__(x));
    test_assert(base > 1, str_inval_base, io_cleanup, STR_INVALID_BASE);
    clearerr(stream);

    //* Whitespace *//
    uint8_t sign = 1; int curr_char;
    while ((curr_char = fgetc(stream)) != EOF && isspace(curr_char))
    scan_eof(curr_char, stream, STR_EMPTY);

    //* Sign & Leading Zeros *//
    curr_char = fgetc(stream);
    if (curr_char == '-' || curr_char == '+') {
        if (curr_char == '-') sign = -1;
        curr_char = fgetc(stream);
        scan_eof(curr_char, stream, STR_INCOMPLETE);
        if (isspace(curr_char)) return STR_INVALID_DIGIT;
    }
    else if (!isdigit(curr_char)) return STR_INVALID_DIGIT;
    else ungetc(curr_char, stream); // Rewind back if curr_char is numeric (0-9)
    // Skipping Leading Zeros
    do { curr_char = fgetc(stream); }
    while (curr_char != EOF && curr_char == '0');
    if (curr_char == EOF) {
        if (ferror(stream)) return FILE_ERR_PARSE;
        if (sign == -1) return STR_INVALID_SIGN;
        __BIGINT_INTERNAL_ZSET__(x); return STR_SUCCESS;
    }

    //* Main accumalator loop *//
    uint8_t index_lookup, numerical_val, end = 0; 
    size_t parse_res, i = 0; uint64_t threshold;
    bigInt tmp_buf; 
    if (__BIGINT_INTERNAL_LINIT__(&tmp_buf, x->cap) == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;
    while (!end) {
        parse_res = fread(___DASI_IO_CHUNKBUF_, sizeof(char), ___DASI_IO_BUFSIZE, stream);
        //* THE ACTUAL ACCUMALATION
        if (parse_res > 0) {
            for (i = 0; i < parse_res; ++i) {
                // Delimiter Handling
                if (___DASI_IO_CHUNKBUF_[i] == '\n') {
                    fseek(stream, -(long)(parse_res - i), SEEK_CUR);
                    end = 1; break; // This disrupt the inner for loop
                }
                index_lookup = (uint8_t)(___DASI_IO_CHUNKBUF_[i] - '\0');
                numerical_val = (base <= 16) ?
                    _VALUE_LOOKUP_INSEN_[index_lookup] :
                    _VALUE_LOOKUP_SEN_[index_lookup];
                threshold = (UINT64_MAX - numerical_val) / base;
                if (numerical_val >= base) { __BIGINT_INTERNAL_FREE__(&tmp_buf); return STR_INVALID_DIGIT; } 
                if (__BIGINT_WILL_OVERFLOW__(&tmp_buf, threshold)) {
                    if (__BIGINT_INTERNAL_ENSCAP__(&tmp_buf, tmp_buf.cap + 5) == DNML_ALLOC_OOM) {
                        return DNML_ALLOC_OOM; // No resources were allocated
                    }
                }
                __BIGINT_INTERNAL_MUL_UI64__(&tmp_buf, base);
                __BIGINT_INTERNAL_ADD_UI64__(&tmp_buf, numerical_val);
            }
        }
        //* ENDING CONDITION
        if (parse_res < ___DASI_IO_BUFSIZE) {
            if (ferror(stream)) {
                __BIGINT_INTERNAL_FREE__(&tmp_buf);
                return FILE_ERR_PARSE;
            } else if (feof(stream)) end = 1;
        }
    }
    __BIGINT_INTERNAL_COPY__(x, &tmp_buf); x->sign = sign;
    __BIGINT_INTERNAL_FREE__(&tmp_buf);
    return STR_SUCCESS;
}
dnml_status bigInt_fsscan(FILE *stream, bigInt *x) {
    assert(__BIGINT_INTERNAL_SVALID__(x));
    dnml_arena *_DASI_FSGET = _USE_ARENA();
    test_assert(!(_DASI_FSGET->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    /* Whitespace */ int curr_char;
    while ((curr_char = fgetc(stream)) != EOF && isspace(curr_char)) 
    scan_eof(curr_char, stream, STR_EMPTY);
    
    //* Sign *//
    uint8_t sign = 1, base = 10, curr_lexpos = 0;
    if (curr_char == '-') { sign = -1; curr_char = fgetc(stream); }
    else if (curr_char == '+') curr_char = fgetc(stream);
    else if (!isdigit(curr_char)) return STR_INVALID_SIGN;
    scan_eof(curr_char, stream, STR_INCOMPLETE);

    //* Prefix & leading zeros *//
    uint8_t bp_res = _prefix_handle_stream__(stream, &base, &curr_char);
    if (bp_res == 1);
    else if (bp_res == 3) return STR_INCOMPLETE;
    else if (!bp_res) {
        if (sign == -1) return STR_INVALID_SIGN;
        __BIGINT_INTERNAL_ZSET__(x); return STR_SUCCESS;
    } else if (bp_res == 2) return STR_INVALID_BASE_PREFIX;
    else if (bp_res == 4) return STR_INVALID_BASE; // Overflow
    // Leading Zeros
    while (curr_char != EOF && curr_char == '0') curr_char = fgetc(stream);
    if (curr_char == EOF) {
        if (ferror(stream)) return FILE_ERR_PARSE;
        if (sign == -1) return STR_INVALID_SIGN;
        __BIGINT_INTERNAL_ZSET__(x); return STR_SUCCESS;
    }

    //* Main accumalator loop *//
    dnml_status err_check;
    uint64_t threshold; size_t i = 0, parse_res;
    uint8_t index_lookup, numerical_val, end = 0;
    size_t tmp_mark = arena_mark(_DASI_FSGET);
    limb_t *tmp_limbs = arena_galloc(_DASI_FSGET, x->cap, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;

    bigInt tmp_buf = { .limbs = tmp_limbs, /**/ .n = 0, /**/ .cap = x->cap };
    while (!end) {
        parse_res = fread(___DASI_IO_CHUNKBUF_, sizeof(char), ___DASI_IO_BUFSIZE, stream);
        //* THE ACTUAL ACCUMALATION
        if (parse_res > 0) {
            for (i = 0; i < parse_res; ++i) {
                // Delimiter Handling
                if (___DASI_IO_CHUNKBUF_[i] == '\n') {
                    fseek(stream, -(long)(parse_res - i), SEEK_CUR);
                    end = 1; break; // This disrupts the inner loop
                }
                index_lookup = (uint8_t)(___DASI_IO_CHUNKBUF_[i] - '\0');
                numerical_val = (base <= 16) ? 
                    _VALUE_LOOKUP_INSEN_[index_lookup] :
                    _VALUE_LOOKUP_SEN_[index_lookup];
                threshold = (UINT64_MAX - numerical_val) / base;
                if (numerical_val >= base) { 
                    arena_reset(_DASI_FSGET, tmp_mark); 
                    tmp_limbs = NULL; return STR_INVALID_DIGIT; 
                } if (__BIGINT_WILL_OVERFLOW__(&tmp_buf, threshold)) {
                    arena_reset(_DASI_FSGET, tmp_mark);
                    tmp_limbs = NULL; return BIGINT_ERR_RANGE;
                }
                __BIGINT_INTERNAL_MUL_UI64__(&tmp_buf, base);
                __BIGINT_INTERNAL_ADD_UI64__(&tmp_buf, numerical_val);
            }
        }
        //* ENDING CONDITION
        if (parse_res < ___DASI_IO_BUFSIZE) {
            if (ferror(stream)) {
                arena_reset(_DASI_FSGET, tmp_mark);
                tmp_limbs = NULL; return FILE_ERR_PARSE; 
            } else if (feof(stream)) end = 1;
        }
    }
    __BIGINT_INTERNAL_COPY__(x, &tmp_buf); x->sign = sign;
    arena_reset(_DASI_FSGET, tmp_mark);
    tmp_limbs = NULL; return STR_SUCCESS;
}
dnml_status bigInt_fsscanb(FILE *stream, bigInt *x, uint8_t base) {
    assert(__BIGINT_INTERNAL_SVALID__(x));
    test_assert(base > 1, str_inval_base, io_cleanup, STR_INVALID_BASE);
    clearerr(stream); dnml_arena *_DASI_FSGETB = _USE_ARENA();
    test_assert(!(_DASI_FSGETB->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);

    //* Whitespace *//
    uint8_t sign = 1; int curr_char;
    while ((curr_char = fgetc(stream)) != EOF && isspace(curr_char))
    scan_eof(curr_char, stream, STR_EMPTY);

    //* Sign & Leading Zeros *//
    curr_char = fgetc(stream);
    if (curr_char == '-' || curr_char == '+') {
        if (curr_char == '-') sign = -1;
        curr_char = fgetc(stream);
        scan_eof(curr_char, stream, STR_INCOMPLETE);
        if (isspace(curr_char)) return STR_INVALID_DIGIT;
    }
    else if (!isdigit(curr_char)) return STR_INVALID_DIGIT;
    else ungetc(curr_char, stream); // Rewind back if curr_char is numeric (0-9)
    // Skipping Leading Zeros
    do { curr_char = fgetc(stream); }
    while (curr_char != EOF && curr_char == '0');
    if (curr_char == EOF) {
        if (ferror(stream)) return FILE_ERR_PARSE;
        if (sign == -1) return STR_INVALID_SIGN;
        __BIGINT_INTERNAL_ZSET__(x); return STR_SUCCESS;
    }

    //* Main accumalator loop *//
    dnml_status err_check;
    uint8_t index_lookup, numerical_val, end = 0; 
    size_t parse_res, i = 0; uint64_t threshold;
    size_t tmp_mark = arena_mark(_DASI_FSGETB);
    limb_t *tmp_limbs = arena_galloc(_DASI_FSGETB, x->cap, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;

    bigInt tmp_buf = { .limbs = tmp_limbs, /**/ .n = 0, /**/ .cap = x->cap };
    while (!end) {
        parse_res = fread(___DASI_IO_CHUNKBUF_, sizeof(char), ___DASI_IO_BUFSIZE, stream);
        //* THE ACTUAL ACCUMALATION
        if (parse_res > 0) {
            for (i = 0; i < parse_res; ++i) {
                // Delimiter Handling
                if (___DASI_IO_CHUNKBUF_[i] == '\n') {
                    fseek(stream, -(long)(parse_res - i), SEEK_CUR);
                    end = 1; break; // This disrupt the inner for loop
                }
                index_lookup = (uint8_t)(___DASI_IO_CHUNKBUF_[i] - '\0');
                numerical_val = (base <= 16) ? 
                    _VALUE_LOOKUP_INSEN_[index_lookup] :
                    _VALUE_LOOKUP_SEN_[index_lookup];
                threshold = (UINT64_MAX - numerical_val) / base;
                if (numerical_val >= base) { 
                    arena_reset(_DASI_FSGETB, tmp_mark); 
                    tmp_limbs = NULL; return STR_INVALID_DIGIT; 
                } if (__BIGINT_WILL_OVERFLOW__(&tmp_buf, threshold)) {
                    arena_reset(_DASI_FSGETB, tmp_mark); 
                    tmp_limbs = NULL; return BIGINT_ERR_RANGE; 
                }
                __BIGINT_INTERNAL_MUL_UI64__(&tmp_buf, base);
                __BIGINT_INTERNAL_ADD_UI64__(&tmp_buf, numerical_val);
            }
        }
        //* ENDING CONDITION
        if (parse_res < ___DASI_IO_BUFSIZE) {
            if (ferror(stream)) {
                arena_reset(_DASI_FSGETB, tmp_mark); 
                tmp_limbs = NULL; return FILE_ERR_PARSE; 
            } else if (feof(stream)) end = 1;
        }
    }
    __BIGINT_INTERNAL_COPY__(x, &tmp_buf); x->sign = sign;
    arena_reset(_DASI_FSGETB, tmp_mark);
    tmp_limbs = NULL; return STR_SUCCESS;
}
dnml_status bigInt_ftscan(FILE *stream, bigInt *x) {
    assert(__BIGINT_INTERNAL_SVALID__(x)); clearerr(stream);
    dnml_arena *_DASI_FSGET = _USE_ARENA();
    test_assert(!(_DASI_FSGET->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    //* Whitespace & Signs *//
    int curr_char; uint8_t sign = 1, base = 10;
    while ((curr_char = fgetc(stream)) != EOF && isspace(curr_char)) 
    scan_eof(curr_char, stream, STR_EMPTY);
    if (curr_char == '-') { sign = -1; curr_char = fgetc(stream); }
    else if (curr_char == '+') curr_char = fgetc(stream);
    else if (!isdigit(curr_char)) return STR_INVALID_SIGN;
    scan_eof(curr_char, stream, STR_INCOMPLETE);

    //* Prefix & leading zeros *//
    uint8_t bp_res = _prefix_handle_stream__(stream, &base, &curr_char);
    if (bp_res == 1);
    else if (bp_res == 3) return STR_INCOMPLETE;
    else if (!bp_res) {
        if (sign == -1) return STR_INVALID_SIGN;
        __BIGINT_INTERNAL_ZSET__(x); return STR_SUCCESS;
    } else if (bp_res == 2) return STR_INVALID_BASE_PREFIX;
    else if (bp_res == 4) return STR_INVALID_BASE; // Overflow
    // Leading Zeros
    while (curr_char != EOF && curr_char == '0') curr_char = fgetc(stream);
    if (curr_char == EOF) {
        if (ferror(stream)) return FILE_ERR_PARSE;
        if (sign == -1) return STR_INVALID_SIGN;
        __BIGINT_INTERNAL_ZSET__(x); return STR_SUCCESS;
    }

    //* Main accumalator loop *//
    dnml_status err_check;
    uint64_t threshold; size_t i = 0, parse_res;
    uint8_t index_lookup, numerical_val, end = 0;
    size_t tmp_mark = arena_mark(_DASI_FSGET);
    limb_t *tmp_limbs = arena_galloc(_DASI_FSGET, x->cap, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;

    bigInt tmp_buf = { .limbs = tmp_limbs, /**/ .n = 0, /**/ .cap = x->cap };
    while (!end) {
        parse_res = fread(___DASI_IO_CHUNKBUF_, sizeof(char), ___DASI_IO_BUFSIZE, stream);
        //* THE ACTUAL ACCUMALATION
        if (parse_res > 0) {
            for (i = 0; i < parse_res; ++i) {
                // Delimiter Handling
                if (___DASI_IO_CHUNKBUF_[i] == '\n') {
                    fseek(stream, -(long)(parse_res - i), SEEK_CUR);
                    end = 1; break; // This disrupts the inner loop
                }
                index_lookup = (uint8_t)(___DASI_IO_CHUNKBUF_[i] - '\0');
                numerical_val = (base <= 16) ? 
                    _VALUE_LOOKUP_INSEN_[index_lookup] :
                    _VALUE_LOOKUP_SEN_[index_lookup];
                threshold = (UINT64_MAX - numerical_val) / base;
                if (numerical_val >= base) { 
                    arena_reset(_DASI_FSGET, tmp_mark); 
                    tmp_limbs = NULL; return STR_INVALID_DIGIT; 
                } if (__BIGINT_WILL_OVERFLOW__(&tmp_buf, threshold)) { 
                    // Ends the entire accumalation loop
                    // -----> Acquire a "truncative" effect
                    end = 1; break;
                }
                __BIGINT_INTERNAL_MUL_UI64__(&tmp_buf, base);
                __BIGINT_INTERNAL_ADD_UI64__(&tmp_buf, numerical_val);
            }
        }
        //* ENDING CONDITION
        if (parse_res < ___DASI_IO_BUFSIZE) {
            if (ferror(stream)) {
                arena_reset(_DASI_FSGET, tmp_mark);
                tmp_limbs = NULL; return FILE_ERR_PARSE; 
            } else if (feof(stream)) break;
        }
    } 
    dnml_status ret = STR_SUCCESS;
    if (__BIGINT_WILL_OVERFLOW__(&tmp_buf, threshold)) ret = STR_TRUNC_SUCCESS;
    __BIGINT_INTERNAL_COPY__(x, &tmp_buf); x->sign = sign;
    arena_reset(_DASI_FSGET, tmp_mark);
    tmp_limbs = NULL; return ret;
}
dnml_status bigInt_ftscanb(FILE *stream, bigInt *x, uint8_t base) {
    assert(__BIGINT_INTERNAL_SVALID__(x));
    test_assert(base > 1, str_inval_base, io_cleanup, STR_INVALID_BASE);
    clearerr(stream); dnml_arena *_DASI_FSGETB = _USE_ARENA();
    test_assert(!(_DASI_FSGETB->poisoined), arena_poison_oom, io_cleanup, DNML_ALLOC_OOM);
    //* Whitespace *//
    uint8_t sign = 1; int curr_char;
    while ((curr_char = fgetc(stream)) != EOF && isspace(curr_char))
    scan_eof(curr_char, stream, STR_EMPTY);

    //* Sign & Leading Zeros *//
    curr_char = fgetc(stream);
    if (curr_char == '-' || curr_char == '+') {
        if (curr_char == '-') sign = -1;
        curr_char = fgetc(stream);
        scan_eof(curr_char, stream, STR_INCOMPLETE);
        if (isspace(curr_char)) return STR_INVALID_DIGIT;
    }
    else if (!isdigit(curr_char)) return STR_INVALID_DIGIT;
    else ungetc(curr_char, stream); // Rewind back if curr_char is numeric (0-9)
    // Skipping Leading Zeros
    do { curr_char = fgetc(stream); }
    while (curr_char != EOF && curr_char == '0');
    if (curr_char == EOF) {
        if (ferror(stream)) return FILE_ERR_PARSE;
        if (sign == -1) return STR_INVALID_SIGN;
        __BIGINT_INTERNAL_ZSET__(x); return STR_SUCCESS;
    }

    //* Main accumalator loop *//
    dnml_status err_check;
    uint8_t index_lookup, numerical_val, end = 0;; 
    size_t parse_res, i = 0; uint64_t threshold;
    size_t tmp_mark = arena_mark(_DASI_FSGETB);
    limb_t *tmp_limbs = arena_galloc(_DASI_FSGETB, x->cap, &err_check);
    if (err_check == DNML_ALLOC_OOM) return DNML_ALLOC_OOM;

    bigInt tmp_buf = { .limbs = tmp_limbs, /**/ .n = 0, /**/ .cap = x->cap };
    while (!end) { 
        parse_res = fread(___DASI_IO_CHUNKBUF_, sizeof(char), ___DASI_IO_BUFSIZE, stream);
        //* THE ACTUAL ACCUMALATION
        if (parse_res > 0) {
            for (i = 0; i < parse_res; ++i) {
                // Delimiter Handling
                if (___DASI_IO_CHUNKBUF_[i] == '\n') {
                    fseek(stream, -(long)(parse_res - i), SEEK_CUR);
                    end = 1; break; // This disrupt the inner for loop
                }
                index_lookup = (uint8_t)(___DASI_IO_CHUNKBUF_[i] - '\0');
                numerical_val = (base <= 16) ? 
                    _VALUE_LOOKUP_INSEN_[index_lookup] :
                    _VALUE_LOOKUP_SEN_[index_lookup];
                threshold = (UINT64_MAX - numerical_val) / base;
                if (numerical_val >= base) { 
                    arena_reset(_DASI_FSGETB, tmp_mark); 
                    tmp_limbs = NULL; return STR_INVALID_DIGIT; 
                } if (__BIGINT_WILL_OVERFLOW__(&tmp_buf, threshold)) {
                    // Termiante the whole accumalation loop
                    // -----> Achieve the "truncative effect"
                    end = 1; break;
                }
                __BIGINT_INTERNAL_MUL_UI64__(&tmp_buf, base);
                __BIGINT_INTERNAL_ADD_UI64__(&tmp_buf, numerical_val);
            }
        }
        //* ENDING CONDITION
        if (parse_res < ___DASI_IO_BUFSIZE) {
            if (ferror(stream)) {
                arena_reset(_DASI_FSGETB, tmp_mark); 
                tmp_limbs = NULL; return FILE_ERR_PARSE; 
            } else if (feof(stream)) end = 1;
        }
    }
    dnml_status ret = STR_SUCCESS;
    if (__BIGINT_WILL_OVERFLOW__(&tmp_buf, threshold)) ret = STR_TRUNC_SUCCESS;
    __BIGINT_INTERNAL_COPY__(x, &tmp_buf); x->sign = sign;
    arena_reset(_DASI_FSGETB, tmp_mark);
    tmp_limbs = NULL; return ret;
}




//todo ================================= 4. SERIALIZATION & DESERIALIZATION ============================== *//
/* --------- Binary INPUT/OUTPUT ---------  */
void bigInt_fwrite(FILE *stream, const bigInt x) {}
size_t bigInt_fread_size(FILE *stream) {}
dnml_status bigInt_fread(FILE *stream, bigInt *x) {}
dnml_status bigInt_fsread(FILE *stream, bigInt *x) {}
dnml_status bigInt_ftread(FILE *stream, bigInt *x) {}
/* --------- SERIALIZATION ---------  */
size_t bigInt_serial_size(const bigInt x) {}
dnml_status bigInt_serialize(char *buf, size_t len, const bigInt x, size_t *written) {}
dnml_status bigInt_tserialize(char *buf, size_t len, const bigInt x, size_t *written) {}
dnml_status bigInt_sserialize(char *buf, size_t len, const bigInt x, size_t *written) {}
/* -------- DESERIALIZATION --------  */
size_t bigInt_deserial_size(const char *str, size_t len) {}
dnml_status bigInt_deserialize(bigInt *x, const char* str, size_t len) {}




//todo ====================================== 5. GENERAL UTILITIES ===================================== *//
void bigInt_limb_dump(FILE *stream, const bigInt x) {
    assert(__BIGINT_INTERNAL_PVALID__(&x));
    fputs  (        "--- DECIMAL LIMB DUMP --------------------------------------\n", stream);
    fprintf(stream, "Limbs' starting location: %p\n", (void*)(x.limbs));
    fputs  (        "------------------------------------------------------------\n", stream);
    fputs(          "memloc              offset     value                   ASCII\n", stream);
    char ascii[8];
    for (size_t i = 0; i < x.cap; ++i) {
        _ASCII_COLUMN__(x.limbs[i], &ascii);
        fprintf(stream, "%p %#9zx %20" PRIu64 "%.8s", &x.limbs[i], i, x.limbs[i], ascii);
    } fputc('\n', stream);
    fputs(          "--------------------------------------------------------\n", stream);
}
void bigInt_hexdump(FILE *stream, const bigInt x, bool uppercase) {
    assert(__BIGINT_INTERNAL_PVALID__(&x));
    fputs  (        "--- HEX LIMB DUMP --------------------------------------\n", stream);
    fprintf(stream, "Limbs' starting location: %p\n", (void*)(x.limbs));
    fputs  (        "--------------------------------------------------------\n", stream);
    fputs(          "memloc              offset     value               ASCII\n", stream);
    char ascii[8];
    for (size_t i = 0; i < x.cap; ++i) {
        _ASCII_COLUMN__(x.limbs[i], &ascii);
        fprintf(stream, "%p %#9zx %#16" PRIX64 "%.8s", &x.limbs[i], i, x.limbs[i], ascii);
    } fputc('\n', stream);
    fputs(          "--------------------------------------------------------\n", stream);
}
void bigInt_bindump(FILE *stream, const bigInt x) {
    assert(__BIGINT_INTERNAL_PVALID__(&x));
    fputs  (        "--- BINARY LIMB DUMP ----------------------------------------------------------------------------------------\n", stream);
    fprintf(stream, "Limbs' starting location: %p\n", (void*)(x.limbs));
    fputs  (        "-------------------------------------------------------------------------------------------------------------\n", stream);
    fputs(          "memloc              offset     value                                                                    ASCII\n", stream);
    limb_t temp_val; char d[64], ascii[8];
    for (size_t i = 0; i < x.cap; ++i) {
        temp_val = x.limbs[i];
        // Format the value to be fixed-width
        for (uint8_t i = 63; i >= 0; --i) {
            d[i] = (temp_val & 1) ? '1' : '0';
            temp_val >>= 1;
        }
        _ASCII_COLUMN__(x.limbs[i], &ascii);
        fprintf(stream, "%p %#9zx %.64s %.8s", &x.limbs[i], i, d, ascii);
    } fputc('\n', stream);
    fputs(          "-------------------------------------------------------------------------------------------------------------\n", stream);
} 
void bigInt_info(FILE *stream, const bigInt x) {
    assert(__BIGINT_INTERNAL_PVALID__(&x));
    fputs    (        "-------- [ BIGINT DEBUG INFO ] --------\n", stream);
    fprintf  (stream, "Address:  %p\n", x.limbs);
    fprintf  (stream, "Sign:         %" PRId8 " %s\n", x.sign, (x.sign == -1) ? "(Negative)" : "(Positive)");
    fprintf  (stream, "Size:         %zu limbs (Used)\n", x.n);
    fprintf  (stream, "Capacity:     %zu limbs (Total)\n\n", x.cap);

    fputs    (        "Limb Data (Little-Endian: LSL -> MSL)\n", stream);
    for (size_t i = 0; i < x.cap; ++i) fprintf(stream, "[%10zu]: %#16" PRIX64, x.limbs[i], i);
    fputs  (          "-----------------------------------------\n", stream);
}

