#ifndef _BIGINT_INPUT_DEFINITION_H
#define _BIGINT_INPUT_DEFINITION_H



#include <stdint.h>
#include <stdio.h>
#include <dnml_sys/sys.h>
#include <libdnml_types.h>
#include "../../../test_ui/_strui.h"

#define DNML_UNFINISHED() do { \
    fputs("\n\n !CRITICAL FAILURE!: This feature is either: ", stderr); \
    fputs("    1) Unfinished / Unimplemented feature\n", stderr); \
    fputs("    2) Unimplemented testing abstractions\n", stderr); \
    fputs("TERMINATING SESSION...", stderr); abort(); \
} while(0)

#define DNML_FOPEN_ERR(fp, name, path) do { \
    if (likely(fp != NULL)) break; \
    fprintf(stderr, "CRITICAL ERROR: Unable to open %s at %s\n", name, path); \
    fputs("Aborting the program immediately...", stderr); abort(); \
} while(0)

//* ===================== INPUT STRUCT DEFINITION ===================== *//
// BITOS Input Structs
typedef struct { size_t len; bigInt x; uint8_t base; bool uppercase; } bitos_conv_in;
typedef struct { bigInt x; uint8_t base; bool uppercase; } bitos_print_in;
typedef struct { bigInt x; } bitos_fwrite_in;
typedef struct { size_t len; bigInt x; } bitos_serialize_in;
typedef struct { bigInt x; bool uppercase; } bitos_util_in;
// STOBI Input Structs
typedef struct { char *str; size_t len; uint8_t base; } stobi_init_in;
typedef struct { char *str; size_t len; uint8_t base; } stobi_conv_in;
typedef struct { char *str; size_t len; uint8_t base; size_t bi_size; } stobi_assign_in;
typedef struct { FILE *stream; uint8_t base; size_t bi_size; } stobi_scan_in;
typedef struct { FILE *stream; size_t bi_size; } stobi_fread_in;
typedef struct { char* str; size_t len; } stobi_deserialize_in;


//* ===================== RECONSTRUCTION STRUCT DEFINITION ===================== *//
typedef struct { const bigInt x; } bitos_recon;
typedef struct { const char *str; } stobi_recon;




#endif