#include "crt_num_theory.h"

/* ------------- GCD ALGORITHMS ------------- */
uint64_t __CRINT_EUCLID__(uint64_t u, uint64_t v) {}
void __CRINT_STEIN__(crint *res, const crint *u, const crint *v) {}
void __CRINT_LEHMER__(crint *res, const crint *u, const crint *v) {}
void __CRINT_HALF__(crint *res, const crint *i, const crint *v) {}
void __CRINT_GCD_DISP__(crint *res, const crint *u, const crint *v) {}




/* ------------- PRIMALITY TESTING ALGORITHMS -------------  */
uint8_t __CRINT_TRIAL_DIV__(uint64_t n) {}
uint8_t __CRINT_SMALL_MRABIN__(uint64_t n) {}
uint8_t __CRINT_MILLER_RABIN__(const crint *n, const crint *base) {}
uint8_t __CRINT_BPSW__(const crint *n) {}
uint8_t __CRINT_ECPP__(const crint *n) {}
uint8_t __CRINT_PTEST_DISP__(const crint *n) {}
