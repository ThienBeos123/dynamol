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



#include "../util.h"

size_t _actual_len(const char *str, size_t buflen) {
    size_t ret = 0;
    for (; ret < buflen; ++ret) {
        if (str[ret] == '\0') break;
    } return (ret < buflen) ? ret : buflen;
}
uint16_t _fskip_whitespace__(FILE *stream) {
    uint16_t c;
    while ((c = fgetc(stream)) != (uint16_t)EOF && c != '\n' && isspace(c));
    return c;
}
size_t _skip_whitespace(const char *str, size_t len, size_t *pos) {
    size_t total_whitespace = 0;
    while ((*pos < len && str[*pos] != '\0') && isspace(str[*pos])) {
        (*pos)++; ++total_whitespace;
    } return total_whitespace;
}
size_t _skip_leading_zeros(const char *str, size_t len, size_t *pos) {
    size_t lzeros = 0;
    while ( (*pos < len && str[*pos] != '\0' && str[*pos] == '0')
    ) { (*pos)++; ++lzeros; } return lzeros;
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
    uint16_t tmp_base = 0;
    for (int i = 0; i < 3; i++) {
        if (str[*curr_pos] == '\0') return 2; // Incomplete
        if (str[*curr_pos] == '}') {
            if (i == 0) return 3; // Invalid Character (empty brace)
            if (tmp_base > UINT8_MAX) return 1; // Overflow check before setting
            *base = (uint8_t)tmp_base;
            (*curr_pos)++; // Advance past '}'
            return 0; // Success
        }
        if (!isdigit(str[*curr_pos])) return 3; // Invalid Character
        tmp_base = tmp_base * 10 + (str[*curr_pos] - '0');
        if (tmp_base > UINT8_MAX) return 1; // Early overflow detection during accumulation
        (*curr_pos)++;
    }
    // After exactly 3 digits, the next char MUST be the closing brace
    if (str[*curr_pos] == '\0') return 2;
    if (str[*curr_pos] != '}') return 3;
    if (tmp_base > UINT8_MAX) return 1; // Final overflow check
    *base = (uint8_t)tmp_base;
    (*curr_pos)++; // Advance past '}'
    return 0;
}
uint8_t _sign_handle_(const char *str, size_t *curr_pos, int8_t *sign) {
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
    if (str[*curr_pos] && !isdigit(str[*curr_pos])) return 4;
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
    else if (!isdigit(str[*curr_pos])) return 2; // The current digit isn't a character
    else { (*curr_pos)++; // The string is currently "0..."
        if (str[*curr_pos] == '\0') return 0; // The string currently is "0\null"
        else if (isdigit(str[*curr_pos])) { // The string currently is "0(numerical)" (eg: 0942)
            (*curr_pos)++; // A leading zero --> Decimal
            return 1;
        } else {
            switch (str[*curr_pos]) {
                case 'x':   *base = 16; (*curr_pos)++; break;
                case 'X':   *base = 16; (*curr_pos)++; break;
                case 'b':   *base = 2; (*curr_pos)++; break;
                case 'B':   *base = 2; (*curr_pos)++; break;
                case 'o':   *base = 8; (*curr_pos)++; break;
                case 'O':   *base = 8; (*curr_pos)++; break;
                case ',':   *base = 64; (*curr_pos)++; break; // Base-64 prefix
                // Arbitrary-base:
                case '{': {
                    if (!isdigit(str[(*curr_pos) + 1])) { return 2; break; }
                    (*curr_pos)++; uint8_t res = _arbit_bprefix(str, curr_pos, base);
                    switch (res) {
                        case 0: return 1; break; // Clean End
                        case 1: return 4; break; // Overflow
                        case 2: return 3; break; // Incomplete
                        case 3: return 2; break; // Invalid Character
                    }
                } break;
                //! INVALID BASE PREFIX
                default: return 2; break;
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
    uint16_t tmp_base = 0;
    for (int i = 0; i < 3; i++) {
        if (*curr_pos == len || str[*curr_pos] == '\0') return 2; // Incomplete
        if (str[*curr_pos] == '}') {
            if (i == 0) return 3; // Invalid Character
            if (tmp_base > UINT8_MAX) return 1; // Overflow check before setting
            *base = (uint8_t)tmp_base;
            (*curr_pos)++; // Advance past '}'
            return 0; // Success
        }
        if (!isdigit(str[*curr_pos])) return 3; // Invalid Character
        tmp_base = tmp_base * 10 + (str[*curr_pos] - '0');
        if (tmp_base > UINT8_MAX) return 1; // Early overflow detection during accumulation
        (*curr_pos)++;
    }
    if (*curr_pos == len || str[*curr_pos] == '\0') return 2;
    if (str[*curr_pos] != '}') return 3;
    if (tmp_base > UINT8_MAX) return 1; // Final overflow check
    *base = (uint8_t)tmp_base;
    (*curr_pos)++; // Advance past '}'
    return 0;
}
uint8_t _sign_handle_nlen_(const char *str, size_t *curr_pos, int8_t *sign, size_t len) {
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
    if (str[*curr_pos] && !isdigit(str[*curr_pos])) return 4;
    return 0;
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
    else if (!isdigit(str[*curr_pos])) return 2; // The current digit isn't a character
    // The string is currently "0..."
    else { (*curr_pos)++;
        if (*curr_pos == len || str[*curr_pos] == '\0') return 0; // The string ended as "0"
        else if (isdigit(str[*curr_pos])) { // The string currently is "0(numerical)" (eg: 0942)
            (*curr_pos)++; // A leading zero --> Decimal
            return 1;
        } else {
            switch (str[*curr_pos]) {
                case 'x':   *base = 16; (*curr_pos)++; break;
                case 'X':   *base = 16; (*curr_pos)++; break;
                case 'b':   *base = 2; (*curr_pos)++; break;
                case 'B':   *base = 2; (*curr_pos)++; break;
                case 'o':   *base = 8; (*curr_pos)++; break;
                case 'O':   *base = 8; (*curr_pos)++; break;
                case ',':   *base = 64; (*curr_pos)++; break; // Base-64 prefix
                // Arbitrary-base:
                case '{': {
                    if ((*curr_pos) + 1 == len) return 3;
                    if (!isdigit(str[(*curr_pos) + 1])) { return 2; break; }
                    (*curr_pos)++; uint8_t res = _arbit_bprefix_nlen(str, curr_pos, base, len);
                    switch (res) {
                        case 0: return 1; break; // Clean End
                        case 1: return 4; break; // Overflow
                        case 2: return 3; break; // Incomplete
                        case 3: return 2; break; // Invalid Character
                    }
                } break;
                //! INVALID BASE PREFIX
                default: return 2; break;
            }
        }
    } return 1;
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
    *base = 10; if (isdigit(*curr_char) && *curr_char != '0') return 1;  // A decimal (eg: 9...)
    else if (*curr_char == (uint16_t)EOF) return (ferror(stream) ? 4 : 3);
    else if (*curr_char == '\n') return 3;
    else if (!isdigit((*curr_char))) return 2; // The current digit isn't a character
    else { // The string is currently "0..."
        (*curr_char) = fgetc(stream);
        // The string currently is either "0\null or ERROR"
        if (*curr_char == (uint16_t)EOF) return (ferror(stream) ? 4 : 0);
        else if (*curr_char == '\n') return 0;
        else if (isdigit(*curr_char)) { // The string currently is "0(numerical)" (eg: 0942)
            (*curr_char) = fgetc(stream); // A leading zero --> Decimal
            return 1;
        } else {
            switch (*curr_char) {
                case 'x':   *base = 16; (*curr_char) = fgetc(stream); break;
                case 'X':   *base = 16; (*curr_char) = fgetc(stream); break;
                case 'b':   *base = 2; (*curr_char) = fgetc(stream); break;
                case 'B':   *base = 2; (*curr_char) = fgetc(stream); break;
                case 'o':   *base = 8; (*curr_char) = fgetc(stream); break;
                case 'O':   *base = 8; (*curr_char) = fgetc(stream); break;
                case ',':   *base = 64; (*curr_char) = fgetc(stream); break; // Base-64 prefix
                // Arbitrary Base:
                case '{': { uint16_t tmp = 0;
                    for (uint8_t i = 0; i < 3; i++) {
                        (*curr_char) = fgetc(stream);
                        if (*curr_char == (uint16_t)EOF) return (ferror(stream) ? 4 : 3);
                        if (*curr_char == '\n') return 3;
                        if (*curr_char == '}') {
                            if (i == 0) return 2; // Empty brackets invalid
                            if (tmp > UINT8_MAX) return 5; // Overflow check before setting
                            *base = (uint8_t)tmp;
                            (*curr_char) = fgetc(stream); // Advance past '}'
                            return 1;
                        }
                        if (!isdigit(*curr_char)) return 2;
                        tmp = tmp * 10 + (uint16_t)(*curr_char - '0');
                        if (tmp > UINT8_MAX) return 5; // Early overflow detection during accumulation
                    }
                    // End of arbitrary-base bounds checking
                    (*curr_char) = fgetc(stream);
                    if (*curr_char == (uint16_t)EOF) return (ferror(stream) ? 4 : 3);
                    if (*curr_char == '\n') return 3;
                    if (*curr_char != '}') return 2;
                    if (tmp > UINT8_MAX) return 5; // Final overflow check
                    
                    *base = (uint8_t)tmp;
                    (*curr_char) = fgetc(stream); // Advance past '}'
                    return 1;
                } break;
                //! INVALID DIGIT
                default:    return 2; break;
            }
        }
    } return 1;
}
