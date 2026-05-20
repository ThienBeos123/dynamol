#include "../util.h"

#define PRECHECK_NLEN(str, curr_pos, off_plus, checked, len) do { \
    if ((*curr_pos) + off_plus == len \
    || str[(*curr_pos) + off_plus] == '\0') return 3; \
    if (str[(*curr_pos) + off_plus] != checked) return 2; \
} while (0)

size_t _actual_len(const char *str, size_t buflen, size_t *actual_len) {
    *actual_len = 0;
    for (; *actual_len < buflen; *actual_len++) {
        if (str[*actual_len]) break;
    } return *actual_len;
}
uint16_t _fskip_whitespace__(FILE *stream) {
    uint16_t c;
    while ((c = fgetc(stream)) != EOF && isspace(c));
    return c;
}
size_t _skip_whitespace(const char *str, size_t len, size_t *pos) {
    size_t total_whitespace = 0;
    while ((*pos < len || str[*pos] != '\0') && isspace(str[*pos])) { 
        (*pos)++; ++total_whitespace;
    } return total_whitespace;
}
size_t _skip_leading_zeros(const char *str, size_t len, size_t *pos) {
    size_t lzeros = 0;
    while ( str[*pos] == '0' 
        && (pos < len || str[*pos] != '\0')
    ) { (*pos)++; ++lzeros; } return lzeros;
}
uint8_t _is_valid_digit__(uint16_t *curr_char) { 
    return (*curr_char != EOF && !isspace(*curr_char)); 
}

/* ----------------------- */
/* --- Normal variants --- */
/* ----------------------- */
/* _arbit_bprefix ret: 
* 0 (SUCESS)
! 1 ---> OVERFLOW
! 2 ---> END OF STRING
! 3 ---> INVALID CHARACTER
*/
uint8_t _arbit_bprefix(const char *str, size_t *curr_pos, uint8_t *base) {
    uint16_t tmp_base = 0; (*curr_pos) += 3;
    tmp_base += (uint16_t)('0' + str[*curr_pos]);
    tmp_base *= 10; (*curr_pos)++;
    if (str[*curr_pos] == '\0') return 2;
    else if (str[*curr_pos] != '}' || !isdigit(str[*curr_pos])) return 3;
    else if (str[*curr_pos] == '}') { *base = (uint8_t)(tmp_base); return 0; }

    tmp_base += (uint16_t)('0' + str[*curr_pos]);
    tmp_base *= 10; (*curr_pos)++;
    if (str[*curr_pos] == '\0') return 2;
    else if (str[*curr_pos] != '}' || !isdigit(str[*curr_pos])) return 3;
    else if (str[*curr_pos] == '}') { *base = (uint8_t)(tmp_base); return 0; }
    tmp_base += (uint16_t)('0' + str[*curr_pos]); (*curr_pos)++;
    
    if (str[*curr_pos] != '}') return 3;
    if (tmp_base > UINT8_MAX) return 1;
    *base = (uint8_t)(tmp_base); return 0;
}
uint8_t _sign_handle_(const char *str, size_t *curr_pos, uint8_t *sign) {
    *sign = 1;
    if (str[*curr_pos] == '-') { 
        *sign = -1; (*curr_pos)++; 
        // In this case, the string is "-\null"
        if (str[*curr_pos] == '\0') return 3;
    }
    else if (str[*curr_pos] == '+') {
        (*curr_pos)++;
        // In this case, the string is "+\null"
        if (str[*curr_pos] == '\0') return 3;
    }
    // This case forces the next character to be 0->9 for the prefix/a decimal
    else if (str[*curr_pos] && !isdigit(str[*curr_pos])) return 4;
    return 0;
}
/* _prefix_handle_ ret:
* 0: Short End (Valid)
* 1: Clean End (Valid)
! 2: INVALID DIGIT (INVALID)
! 3: INCOMPLETE (INVALID)
! 4: OVERFLOW (INVALID)
*/
uint8_t _prefix_handle_(const char *str, size_t *curr_pos, uint8_t *base) {
    *base = 10; if (str[*curr_pos] == '\0') return 3; // Ended ("\0")
    if (isdigit(str[*curr_pos]) && str[*curr_pos] != '0') return 1; // A decimal (eg: 9...)
    else { (*curr_pos)++; // The string is currently "0..."
        if (str[*curr_pos] == '\0') return 0; // The string currently is "0\null"
        else if (isdigit(str[*curr_pos])) { // The string currently is "0(numerical)" (eg: 0942)
            (*curr_pos)++; // A leading zero --> Decimal
            return 1;
        } else {
            switch (str[*curr_pos]) {
                // Hexadecimal
                case 'x':   *base = 16; (*curr_pos)++; break;
                case 'X':   *base = 16; (*curr_pos)++; break;
                // Binary
                case 'b':   *base = 2; (*curr_pos)++; break;
                case 'B':   *base = 2; (*curr_pos)++; break;
                // Octal
                case 'o':   *base = 8; (*curr_pos)++; break;
                case 'O':   *base = 8; (*curr_pos)++; break;
                // Base-64
                case ',':   *base = 64; (*curr_pos)++; break;
                // Arbitrary-base:
                case '{': {
                    if (!isdigit(str[(*curr_pos) + 1])) { return 2; break; }
                    (*curr_pos)++;
                    if (str[(*curr_pos) + 1] != '}') return 2;
                    if (str[(*curr_pos) + 2] != '}') return 2;
                    if (str[(*curr_pos) + 3] != '}') return 2;
                    uint8_t res = _arbit_bprefix(str, curr_pos, base);
                    switch (res) {
                        case 0: return 1; break; // Clean End
                        case 1: return 4; break; // Overflow
                        case 2: return 3; break; // Incomplete
                        case 3: return 2; break; // Invalid Character
                    }
                } break;
                //! INVALID BASE PREFIX
                default:    return 2; break;
            }
        }
    } return 1;
}

/* --------------------- */
/* --- Nlen variants --- */
/* --------------------- */
/* _arbit_bprefix_nlen ret: 
* 0 (SUCESS)
! 1 ---> OVERFLOW
! 2 ---> END OF STRING
! 3 ---> INVALID CHARACTER
*/
uint8_t _arbit_bprefix_nlen(const char *str, size_t *curr_pos, uint8_t *base, size_t len) {
    uint16_t tmp_base = 0; (*curr_pos) += 3;
    tmp_base += (uint16_t)('0' + str[*curr_pos]);
    tmp_base *= 10; (*curr_pos)++;
    if (*curr_pos == len || str[*curr_pos] == '\0') return 2;
    else if (str[*curr_pos] != '}' || !isdigit(str[*curr_pos])) return 3;
    else if (str[*curr_pos] == '}') { *base = (uint8_t)(tmp_base); return 0; }

    tmp_base += (uint16_t)('0' + str[*curr_pos]);
    tmp_base *= 10; (*curr_pos)++;
    if (*curr_pos == len || str[*curr_pos] == '\0') return 2;
    else if (str[*curr_pos] != '}' || !isdigit(str[*curr_pos])) return 3;
    else if (str[*curr_pos] == '}') { *base = (uint8_t)(tmp_base); return 0; }

    tmp_base += (uint16_t)('0' + str[*curr_pos]); (*curr_pos)++;
    if (str[*curr_pos] != '}') return 3;
    if (tmp_base > UINT8_MAX) return 1;
    *base = (uint8_t)(tmp_base); return 0;
    
}
uint8_t _sign_handle_nlen_(const char *str, size_t *curr_pos, uint8_t *sign, size_t len) {
    *sign = 1;
    if (str[*curr_pos] == '-') { 
        *sign = -1; (*curr_pos)++; 
        // In this case, the string is "-\null" or ended as "-"
        if (*curr_pos == len || str[*curr_pos] == '\0') return 3;
    }
    else if (str[*curr_pos] == '+') {
        (*curr_pos)++;
        // In this case, the string is "+\null" or ended as "+"
        if (*curr_pos == len || str[*curr_pos] == '\0') return 3;
    }
    // This case forces the next character to be 0->9 for the prefix/a decimal
    else if (str[*curr_pos] && !isdigit(str[*curr_pos])) return 4;
}
/* _prefix_handle_nlen_ ret:
* 0: Short End (Valid)
* 1: Clean End (Valid)
! 2: INVALID DIGIT (INVALID)
! 3: INCOMPLETE (INVALID)
! 4: OVERFLOW (INVALID)
*/
uint8_t _prefix_handle_nlen_(const char *str, size_t *curr_pos, uint8_t *base, size_t len) {
    *base = 10; if (*curr_pos == len || str[*curr_pos] == '\0') return 3; // Ended ("\0")
    if (isdigit(str[*curr_pos]) && str[*curr_pos] != '0') return 1; // A decimal (eg: 9...)
    // The string is currently "0..."
    else if (str[*curr_pos] == '0') { (*curr_pos)++;
        if (*curr_pos == len || str[*curr_pos] == '\0') return 0; // The string ended as "0"
        else if (isdigit(str[*curr_pos])) { // The string currently is "0(numerical)" (eg: 0942)
            (*curr_pos)++; // A leading zero --> Decimal
            return 1;
        } else {
            switch (str[*curr_pos]) {
                // Hexadecimal
                case 'x':   *base = 16; (*curr_pos)++; break;
                case 'X':   *base = 16; (*curr_pos)++; break;
                // Binary
                case 'b':   *base = 2; (*curr_pos)++; break;
                case 'B':   *base = 2; (*curr_pos)++; break;
                // Octal
                case 'o':   *base = 8; (*curr_pos)++; break;
                case 'O':   *base = 8; (*curr_pos)++; break;
                // Base-64
                case ',':   *base = 64; (*curr_pos)++; break;
                // Arbitrary-base:
                case '{': {
                    if ((*curr_pos) + 1 == len) return 3;
                    if (!isdigit(str[(*curr_pos) + 1])) { return 2; break; }
                    (*curr_pos)++;
                    PRECHECK_NLEN(str, curr_pos, 1, '}', len);
                    PRECHECK_NLEN(str, curr_pos, 2, '}', len);
                    PRECHECK_NLEN(str, curr_pos, 3, '}', len);
                    uint8_t res = _arbit_bprefix_nlen(str, curr_pos, base, len);
                    switch (res) {
                        case 0: return 1; break; // Clean End
                        case 1: return 4; break; // Overflow
                        case 2: return 3; break; // Incomplete
                        case 3: return 2; break; // Invalid Character
                    }
                } break;
                //! INVALID BASE PREFIX
                default:    return 2; break;
            }
        }
    } else return 1;
}

/* ----------------------------- */
/* --- Stream parse variants --- */
/* ----------------------------- */
/* _prefix_handle_stream__ ret:
* 0: Short-end (VALID)
* 1: Clean-end (VALID)
! 2: Invalid Digit (INVALID)
! 3: Incomplete (INVALID)
! 4: File Error (Invalid)
! 5: Overflow
*/
uint8_t _prefix_handle_stream__(FILE* stream, uint8_t *base, uint16_t *curr_char) {
    *base = 10;
    if (isdigit(*curr_char) && *curr_char != '0') return 1;  // A decimal (eg: 9...)
    else { // The string is currently "0..."
        (*curr_char) = fgetc(stream);
        // The string currently is either "0\null or ERROR"
        if (*curr_char == EOF) return (ferror(stream) ? 4 : 0);
        else if (isdigit(*curr_char)) { // The string currently is "0(numerical)" (eg: 0942)
            (*curr_char) = fgetc(stream); // A leading zero --> Decimal
            return 1;
        } else {
            switch (*curr_char) {
                // Hexadecimal
                case 'x':   *base = 16; (*curr_char) = fgetc(stream); break;
                case 'X':   *base = 16; (*curr_char) = fgetc(stream); break;
                // Binary
                case 'b':   *base = 2; (*curr_char) = fgetc(stream); break;
                case 'B':   *base = 2; (*curr_char) = fgetc(stream); break;
                // Octal
                case 'o':   *base = 8; (*curr_char) = fgetc(stream); break;
                case 'O':   *base = 8; (*curr_char) = fgetc(stream); break;
                // Base-64:
                case ',':   *base = 64; (*curr_char) = fgetc(stream); break;
                // Arbitrary Base:
                case '{': { uint16_t tmp = 0;
                    for (uint8_t i = 0; i < 3 && tmp; i++) {
                        (*curr_char) = fgetc(stream);
                        if (*curr_char == EOF) return (ferror(stream) ? 4 : 3);
                        // The numerical segment handling
                        if (!i) { if (!isdigit(*curr_char)) return 2; }
                        else { 
                            if (*curr_char == '}') return 1;
                            else if (!isdigit(*curr_char)) return 2;
                        } tmp *= 10; tmp += (uint16_t)(*curr_char - '0');
                    }
                    // End of arbitrary-base / Closage
                    if (*curr_char != '}') return 2;
                    return ((tmp <= UINT8_MAX) ? 1 : 5);
                } break;
                //! INVALID DIGIT
                default:    return 2; break;
            }
        }
    }
}