#ifndef dnml_test_utils_h
#define dnml_test_utils_h



#include <stdio.h>
#include <stdint.h>
#include <libdnml_types.h>
#include <_libdnml_mem/arena.h>
#define init_log(log_path, log_arr, log_cnt, pre_cleanup) do { \
    log_arr[log_cnt] = fopen(log_path, "w+"); \
    if (log_arr[log_cnt] == NULL) { pre_cleanup; close_logs(log_arr, log_cnt); fputs("Can't open log file", stderr); return 1; } \
    ++log_cnt; \
} while(0);


static inline void print_bi_limbs(const char *xname, const bigInt *x, FILE* f) {
    fprintf(f, "%s = {\n", xname);
    for (size_t i = 0; i < x->n; i += 8) {
        // Doing the loop like this makes it easier to maintain an 8-column row layout
        fputs("    ", f); 
        for (uint8_t j = i; j < i + 7; ++j) { 
            if (j >= x->n) break;
            fprintf(f, "%" PRIX64 ", ", x->limbs[j]); 
        } fputc('\n', f); // Yeah ig bro
    } fputs("}\n", f);
}


static inline void print_ci_limbs(const char *xname, const crint *x, FILE* f) {
    fprintf(f, "%s = {\n", xname);
    for (size_t i = 0; i < x->n; i += 8) {
        // Doing the loop like this makes it easier to maintain an 8-column row layout
        fputs("    ", f); 
        for (uint8_t j = i; j < i + 7; ++j) { 
            if (j >= x->n) break;
            fprintf(f, "%" PRIX64 ", ", x->limbs[j]); 
        } fputc('\n', f); // Yeah ig bro
    } fputs("}\n", f);
}


static inline void close_logs(FILE** log_arr, uint8_t log_cnt) {
    for (uint8_t i = 0; i < log_cnt; ++i) fclose(log_arr[i]);
}
static inline void test_cleanup(dnml_arena *arena, limb_t *ret_buf, FILE** log_arr, uint8_t log_cnt) {
    arena_clear(arena); arena_destruct(arena); free(ret_buf); close_logs(log_arr, log_cnt);
    fputs("Scratch workspace size is inadequate for operation\n", stderr); exit(SIGABRT);
}



#endif