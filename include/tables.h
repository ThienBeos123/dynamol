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



#ifndef ___DNML_CHAR_TABLES_H____
#define ___DNML_CHAR_TABLES_H____

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
// Case-insensitive - base-16: 0123456789 + ABCDEF/abcdef
// Case-sensitive - base-64: 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz./
extern const uint8_t _VALUE_LOOKUP_INSEN_[256];
extern const uint8_t _VALUE_LOOKUP_SEN_[256];
extern const char _DIGIT_INSEN_[32];
extern const char _DIGIT_SEN_[64];
extern const uint8_t _ASCII_INVAL_RANGE64[4][2];

#define _PFSQR_MOD64_CNT 9
#define _PFSQR_MOD256_CNT 40
extern const uint32_t pfsqr_filter_mod64[_PFSQR_MOD64_CNT];
extern const uint32_t pfsqr_filter_mod256[_PFSQR_MOD256_CNT];


#ifdef __cplusplus
}
#endif

#endif
