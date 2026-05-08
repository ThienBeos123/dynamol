#include "util.h"

#define PRECHECK_NLEN(str, curr_pos, off_plus, checked, len) do { \
    if (*curr_pos + off_plus == len - 1) return 3; \
    if (str[*curr_pos + off_plus] != checked) return 2; \
} while (0)


uint16_t _fskip_whitespace__(FILE *stream) {
    uint16_t c;
    while ((c = fgetc(stream)) != EOF && isspace(c));
    return c;
}
size_t _skip_whitespace(const char *str, size_t len, size_t *pos) {
    size_t total_whitespace = 0;
    while (*pos < len && str[*pos] != '\0' && isspace(str[*pos])) { 
        *pos++; ++total_whitespace; 
    } return total_whitespace;
}
size_t _skip_leading_zeros(const char *str, size_t len, size_t *pos) {
    size_t lzeros = 0;
    while ( str[*pos] == '0' 
        && (str[*pos] != '\0' || pos < len)
    ) { *pos++; ++lzeros; } return lzeros;
}
uint8_t _is_valid_digit__(uint16_t *curr_char) { 
    return (*curr_char != EOF && !isspace(*curr_char)); 
}

/* ----------------------- */
/* --- Normal variants --- */
/* ----------------------- */
// _arbit_bprefix returns 
// either 1 (overflowed) or 0 (didn't overflowed) 
// and also 2 ---> END OF STRING
// and also 3 ---> INVALID CHARACTER
// for the uint8_t base limit
uint8_t _arbit_bprefix(const char *str, size_t *curr_pos, uint8_t *base) {
    uint16_t tmp_base = 0; *curr_pos += 3;
    tmp_base += (uint16_t)('0' + str[*curr_pos]);
    tmp_base *= 10; *curr_pos++;
    if (str[*curr_pos] == '\0') return 2;
    else if (str[*curr_pos] != '}' || !isdigit(str[*curr_pos])) return 3;
    else if (str[*curr_pos] == '}') { *base = (uint8_t)(tmp_base); return 0; }
    tmp_base += (uint16_t)('0' + str[*curr_pos]);
    tmp_base *= 10; *curr_pos++;
    if (str[*curr_pos] == '\0') return 2;
    else if (str[*curr_pos] != '}' || !isdigit(str[*curr_pos])) return 3;
    else if (str[*curr_pos] == '}') { *base = (uint8_t)(tmp_base); return 0; }
    tmp_base += (uint16_t)('0' + str[*curr_pos]); *curr_pos++;
    if (str[*curr_pos] != '}') return 3;
    if (tmp_base > UINT8_MAX) return 1;
    *base = (uint8_t)(tmp_base); return 0;
}
uint8_t _sign_handle_(const char *str, size_t *curr_pos, uint8_t *sign) {
    *sign = 1;
    if (str[*curr_pos] == '-') { 
        *sign = -1; *curr_pos += 1; 
        // In this case, the string is "-\null"
        if (str[*curr_pos] == '\0') return 3;
    }
    else if (str[*curr_pos] == '+') {
        *curr_pos += 1;
        // In this case, the string is "+\null"
        if (str[*curr_pos] == '\0') return 3;
    }
    // This case forces the next character to be 0->9 for the prefix/a decimal
    else if (str[*curr_pos] && !isdigit(str[*curr_pos])) return 4;
    return 0;
}
uint8_t _prefix_handle_(const char *str, size_t *curr_pos, uint8_t *base) {
    *base = 10; if (str[*curr_pos] == '\0') return 3; // Ended ("\0")
    if (isdigit(str[*curr_pos]) && str[*curr_pos] != '0') return 1; // A decimal (eg: 9...)
    else { *curr_pos++; // The string is currently "0..."
        if (str[*curr_pos] == '\0') return 0; // The string currently is "0\null"
        else if (isdigit(str[*curr_pos])) { // The string currently is "0(numerical)" (eg: 0942)
            *curr_pos++; // A leading zero --> Decimal
            return 1;
        } else {
            switch (str[*curr_pos]) {
                // Hexadecimal
                case 'x':   *base = 16; *curr_pos++; break;
                case 'X':   *base = 16; *curr_pos++; break;
                // Binary
                case 'b':   *base = 2; *curr_pos++; break;
                case 'B':   *base = 2; *curr_pos++; break;
                // Octal
                case 'o':   *base = 8; *curr_pos++; break;
                case 'O':   *base = 8; *curr_pos++; break;
                // Arbitrary-base:
                case '{': {
                    if (!isdigit(str[*curr_pos + 1])) { return 2; break; }
                    *curr_pos++;
                    if (str[*curr_pos + 1] != '}') return 2; \
                    if (str[*curr_pos + 2] != '}') return 2; \
                    if (str[*curr_pos + 3] != '}') return 2; \
                    _arbit_bprefix(str, curr_pos, base);
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
// _arbit_bprefix_nlen returns 
// either 1 (overflowed) or 0 (didn't overflowed)
// and also 2 ---> END OF STRING
// and also 3 ---> INVALID CHARACTER
// for the uint8_t base limt
uint8_t _arbit_bprefix_nlen(const char *str, size_t *curr_pos, uint8_t *base, size_t len) {
    uint16_t tmp_base = 0; *curr_pos += 3;
    tmp_base += (uint16_t)('0' + str[*curr_pos]);
    tmp_base *= 10; *curr_pos++;
    if (str[*curr_pos] == '\0' || *curr_pos == len - 1) return 2;
    else if (str[*curr_pos] != '}' || !isdigit(str[*curr_pos])) return 3;
    else if (str[*curr_pos] == '}') { *base = (uint8_t)(tmp_base); return 0; }
    tmp_base += (uint16_t)('0' + str[*curr_pos]);
    tmp_base *= 10; *curr_pos++;
    if (str[*curr_pos] == '\0' || *curr_pos == len - 1) return 2;
    else if (str[*curr_pos] != '}' || !isdigit(str[*curr_pos])) return 3;
    else if (str[*curr_pos] == '}') { *base = (uint8_t)(tmp_base); return 0; }
    tmp_base += (uint16_t)('0' + str[*curr_pos]); *curr_pos++;
    if (str[*curr_pos] != '}') return 3;
    if (tmp_base > UINT8_MAX) return 1;
    *base = (uint8_t)(tmp_base); return 0;
    
}
uint8_t _sign_handle_nlen_(const char *str, size_t *curr_pos, uint8_t *sign, size_t len) {
    *sign = 1;
    if (str[*curr_pos] == '-') { 
        *sign = -1; *curr_pos += 1; 
        // In this case, the string is "-\null" or ended as "-"
        if (*curr_pos == len - 1 || str[*curr_pos] == '\0') return 3;
    }
    else if (str[*curr_pos] == '+') {
        *curr_pos += 1;
        // In this case, the string is "+\null" or ended as "+"
        if (*curr_pos == len - 1 || str[*curr_pos] == '\0') return 3;
    }
    // This case forces the next character to be 0->9 for the prefix/a decimal
    else if (str[*curr_pos] && !isdigit(str[*curr_pos])) return 4;
}
uint8_t _prefix_handle_nlen_(const char *str, size_t *curr_pos, uint8_t *base, size_t len) {
    *base = 10; if (*curr_pos == len - 1 || str[*curr_pos] == '\0') return 3; // Ended ("\0")
    if (isdigit(str[*curr_pos]) && str[*curr_pos] != '0') return 1; // A decimal (eg: 9...)
    // The string is currently "0..."
    else if (str[*curr_pos] == '0') { *curr_pos++;
        if (*curr_pos == len - 1) return 0; // The string ended as "0"
        else if (str[*curr_pos] == '\0') return 0; // The string currently is "0\null"
        else if (isdigit(str[*curr_pos])) { // The string currently is "0(numerical)" (eg: 0942)
            *curr_pos += 1; // A leading zero --> Decimal
            return 1;
        } else {
            switch (str[*curr_pos]) {
                // Hexadecimal
                case 'x':   *base = 16; *curr_pos++; break;
                case 'X':   *base = 16; *curr_pos++; break;
                // Binary
                case 'b':   *base = 2; *curr_pos++; break;
                case 'B':   *base = 2; *curr_pos++; break;
                // Octal
                case 'o':   *base = 8; *curr_pos++; break;
                case 'O':   *base = 8; *curr_pos++; break;
                // Arbitrary-base:
                case '{': {
                    if (*curr_pos + 1 == len - 1) return 3;
                    if (!isdigit(str[*curr_pos + 1])) { return 2; break; }
                    *curr_pos++;
                    PRECHECK_NLEN(str, curr_pos, 1, '}', len);
                    PRECHECK_NLEN(str, curr_pos, 2, '}', len);
                    PRECHECK_NLEN(str, curr_pos, 3, '}', len);
                    _arbit_bprefix_nlen(str, curr_pos, base, len);
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
uint8_t _prefix_handle_stream__(FILE* stream, uint8_t *base, uint16_t *curr_char) {
    *base = 10;
    if (isdigit(*curr_char) && *curr_char != 0) return 1;  // A decimal (eg: 9...)
    else { // The string is currently "0..."
        *curr_char = fgetc(stream);
        if (*curr_char == EOF || isspace(*curr_char)) return 0; // The string currently is "0\null"
        else if (isdigit(*curr_char)) { // The string currently is "0(numerical)" (eg: 0942)
            *curr_char = fgetc(stream); // A leading zero --> Decimal
            return 1;
        } else {
            switch (*curr_char) {
                // Hexadecimal
                case 'x':   *base = 16; *curr_char = fgetc(stream); break;
                case 'X':   *base = 16; *curr_char = fgetc(stream); break;
                // Binary
                case 'b':   *base = 2; *curr_char = fgetc(stream); break;
                case 'B':   *base = 2; *curr_char = fgetc(stream); break;
                // Octal
                case 'o':   *base = 8; *curr_char = fgetc(stream); break;
                case 'O':   *base = 8; *curr_char = fgetc(stream); break;
                //! INVALID BASE PREFIX
                default:    return 2; break;
            }
        }
    }
}