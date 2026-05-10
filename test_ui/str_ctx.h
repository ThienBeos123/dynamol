#ifndef DNML_IO_CTX_H
#define DNML_IO_CTX_H

#include <stdint.h>
#include <stdlib.h>
#include <stdalign.h>
#include "_test_base.h"
#include "_strui.h"

#define ALIGN_UP(x, a) (((x) + ((a) - 1)) & ~((a) - 1))
#define RES_BUF_SIZE ((ALIGN_UP(sizeof(str_res), alignof(limb_t))) + STR_CAP)

typedef struct {
    uint8_t res_buf[RES_BUF_SIZE]; // FAM-based
    uint8_t aux2_buf[RES_BUF_SIZE]; // FAM-based
} rctx_res_t;

typedef struct {
    uint8_t in_buf[STR_CAP];
    uint8_t aux1_buf[STR_CAP];
} rctx_input_t;


#endif