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
#include <_libdnml_mem/arena.h>
#include <_libdnml_mem/_ctx.h>
#include "../../../libdnml_base.h"
#include "../../../calc_algo/algo_base/add_sub.h"
#define CASE_CNT 50
#define MAX_SIZE 10
typedef struct { const bigInt a; const bigInt b; const bigInt exp; } bi_case;
//* ============== 




//* =================== GLOBAL ARRAY OF CASES =================== *//





int main(void) { _libdnml_init();
    int total_tests = 0, passed_tests = 0;
    struct timespec start, end; clock_gettime(CLOCK_MONOTONIC, &start);
    limb_t *ret_buf = (limb_t *)malloc(12 * U64_BYTES); assert(ret_buf != NULL);
    fputs("====================================================================\n", stdout);
    fputs("            LIB-DNML ALGORITHM TESTS - BIGINT SUBTRACTION           \n", stdout);
    fputs("====================================================================\n", stdout);
    fputs("----- __BIGINT_SUB_WB__ -----\n", stdout);
    for (int i = 0; i < CASE_CNT; ++i) { total_tests++;
        memset(ret_buf, 0, 12 * U64_BYTES);
    }
    fputs("----- __BIGINT_SUB_SAW__ -----\n", stdout);
    for (int i = 0; i < CASE_CNT; ++i) { total_tests++;
        memset(ret_buf, 0, 12 * U64_BYTES);
    }




    /* Summary output block */
    #undef CASE_CNT
    #undef MAX_SIZE
    clock_gettime(CLOCK_MONOTONIC, &end); free(ret_buf);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    fputs( "=========================================================\n", stdout);
    fputs( "TEST SUMMARY:\n", stdout);
    printf("+) Passed %-4d out of %-4d total compiled checks.\n", passed_tests, total_tests);
    printf("+) Success rate: %.2f%%\n", (total_tests > 0) ? ((passed_tests * 100.0) / total_tests) : 0.0);
    printf("+) Total Runtime: %lf ms\n", elapsed_time * 1000.0);
    fputs( "=========================================================\n", stdout);
    _libdnml_cleanup(); return 0;
}
