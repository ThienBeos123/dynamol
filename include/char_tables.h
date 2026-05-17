#ifndef ___DNMLin_CHAR_TABLES_H____
#define ___DNMLin_CHAR_TABLES_H____


#include <stdint.h>
// Case-insensitive - base-16: 0123456789 + ABCDEF/abcdef
// Case-sensitive - base-64: 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz./
extern const uint8_t _VALUE_LOOKUP_INSEN_[256];
extern const uint8_t _VALUE_LOOKUP_SEN_[256];
extern const char _DIGIT_INSEN_[32];
extern const char _DIGIT_SEN_[64];
extern const uint8_t _ASCII_INVAL_RANGE64[4][2];

#endif