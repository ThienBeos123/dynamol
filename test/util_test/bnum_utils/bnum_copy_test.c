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



// Core Types & Utilities
#include <include.h>
#include <dnml_status.h>
#include <dnml_sys/sys.h>
#include <libdnml_types.h>
// Memory Management and Algorithmic core
#include <_libdnml_config/numeric_config.h>
#include "../../../util/util.h"
#include "../../../libdnml_base.h"
/* Static limbs for test cases */
static limb_t c01[1] = { UINT64_C(0x0000000000000001) };
static limb_t c02[1] = { UINT64_C(0xFFFFFFFFFFFFFFFF) };
static limb_t c03[2] = { UINT64_C(0x0000000000000000), UINT64_C(0x0000000000000001) };
static limb_t c04[2] = { UINT64_C(0xAAAAAAAAAAAAAAA), UINT64_C(0x5555555555555555) };
static limb_t c05[3] = { UINT64_C(0x0000000000000001), UINT64_C(0x0000000000000002), UINT64_C(0x0000000000000003) };
static limb_t c06[4] = { UINT64_C(0xDEADC0DECAFEBABE), UINT64_C(0xFEEDFACEDEADBEEF), UINT64_C(0x0123456789ABCDEF), UINT64_C(0xFEDCBA9876543210) };

static const bigInt global_cases[] = {
    { 0, 1, NULL, 1 },  /* 0 */ { 1, 1, c01, 1 },  /* 1 */ { 1, 1, c01, -1 }, /* 2 */
    { 1, 1, c02, 1 },   /* 3 */ { 1, 1, c02, -1 }, /* 4 */ { 2, 2, c03, 1 },  /* 5 */
    { 2, 2, c03, -1 },  /* 6 */ { 2, 2, c04, 1 },  /* 7 */ { 3, 4, c05, 1 },  /* 8 */
    { 4, 4, c06, -1 },  /* 9 */ { 0, 10, NULL, 1 }, /* 10 */ { 1, 1, c01, 1 }, /* 11 */
    { 1, 1, c01, -1 },  /* 12 */ { 1, 1, c02, 1 },   /* 13 */ { 2, 2, c03, 1 },   /* 14 */
    { 2, 2, c04, -1 },  /* 15 */ { 3, 3, c05, -1 },  /* 16 */ { 4, 4, c06, 1 },   /* 17 */
    { 0, 5, NULL, 1 }, /* 18 */ { 1, 1, c01, 1 },   /* 19 */ { 2, 2, c03, 1 },   /* 20 */
    { 1, 1, c02, 1 },   /* 21 */ { 4, 4, c06, -1 },  /* 22 */ { 0, 1, NULL, 1 },  /* 23 */
    { 1, 1, c01, -1 },  /* 24 */ { 2, 2, c04, 1 },   /* 25 */ { 3, 3, c05, 1 },   /* 26 */
    { 0, 0, NULL, 1 },  /* 27 */ { 1, 2, c02, -1 },  /* 28 */ { 2, 2, c03, 1 },   /* 29 */
    { 4, 10, c06, 1 },  /* 30 */ { 1, 1, c01, 1 },   /* 31 */ { 1, 1, c02, 1 },   /* 32 */
    { 0, 1, NULL, 1 }, /* 33 */ { 2, 2, c04, 1 },   /* 34 */ { 3, 5, c05, -1 },  /* 35 */
    { 4, 4, c06, 1 },   /* 36 */ { 1, 1, c01, -1 },  /* 37 */ { 0, 2, NULL, 1 },  /* 38 */
    { 2, 2, c03, -1 },  /* 39 */ { 1, 1, c02, 1 },   /* 40 */ { 4, 4, c06, 1 },   /* 41 */
    { 0, 1, NULL, 1 },  /* 42 */ { 3, 3, c05, 1 },   /* 43 */ { 1, 1, c01, 1 },   /* 44 */
    { 2, 2, c04, -1 },  /* 45 */ { 0, 8, NULL, 1 },  /* 46 */ { 1, 1, c02, -1 },  /* 47 */
    { 4, 4, c06, 1 },   /* 48 */ { 2, 2, c03, 1 }    /* 49 */
};


int main(void) {
    _libdnml_init();
    int num_cases = sizeof(global_cases) / sizeof(bigInt);
    int total_tests = 0, passed_tests = 0;
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    /* Setup heap-allocated buffer for destination */
    const size_t max_limbs = 4;
    limb_t *dst_buffer = (limb_t *)malloc(4 * sizeof(limb_t));
    if (!dst_buffer) return 1;
    printf("===================================================================\n");
    printf("     RUNNING INTEGRATED UNIT TESTS - BIGNUM COPY UTILITIES         \n");
    printf("===================================================================\n");
    
    printf("---- __BIGINT_INTERNAL_COPY__ -----\n");
    for (int i = 0; i < num_cases; i++) { total_tests++;
        const bigInt *src = &global_cases[i];
        bigInt dst = { .n = 0, .cap = max_limbs, .limbs = dst_buffer, .sign = 1 };
        memset(dst_buffer, 0, max_limbs * sizeof(limb_t));
        __BIGINT_INTERNAL_COPY__(&dst, src);
        /* Verification Logic */
        int match = 1;
        if (dst.n != src->n || dst.sign != src->sign) match = 0;
        else if (src->n > 0 && memcmp(dst.limbs, src->limbs, src->n * sizeof(limb_t)) != 0) match = 0;
        if (match) passed_tests++;
        else printf(
            "Case: %2d "
            "| Input = { .limbs = SEE case_%02d, .n = %zu, .sign = %d } "
            "| Output = { .limbs = ..., .n = %zu, .sign = %d }\n", 
            i, i, src->n, src->sign, dst.n, dst.sign
        );
    }

    /* Summary output bock */
    clock_gettime(CLOCK_MONOTONIC, &end); free(dst_buffer);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("=========================================================\n");
    printf("TEST SUMMARY:\n");
    printf("+) Passed %-4d out of %-4d total compiled checks.\n", passed_tests, total_tests);
    printf("+) Success rate: %.2f%%\n", (passed_tests * 100.0) / total_tests);
    printf("+) Total Runtime: %lf ms\n", elapsed_time * 1000.0);
    printf("=========================================================\n");
    _libdnml_cleanup(); return (passed_tests == total_tests) ? 0 : 1;
}
