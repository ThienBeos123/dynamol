#ifndef DNML_IO_CTX_H
#define DNML_IO_CTX_H

#include <stdint.h>
#include <stdlib.h>
#include "_test_base.h"
#include "_strui.h"

typedef struct {
    uint8_t res_buf[sizeof(str_res) + STR_CAP]; // FAM-based
    uint8_t aux2_buf[sizeof(str_res) + STR_CAP]; // FAM-based
} rctx_res_t;

typedef struct {
    uint8_t in_buf[STR_CAP];
    uint8_t aux1_buf[STR_CAP];
} rctx_input_t;


#endif