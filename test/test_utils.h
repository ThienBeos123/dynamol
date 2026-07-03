#ifndef dnml_test_utils_h
#define dnml_test_utils_h



#include <stdio.h>
#include <stdint.h>
#include <libdnml_types.h>


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


static inline void print_ci_limbs(const char *xname, const crint *x, FILE* f, size_t max_cap) {
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



#endif