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



#include "__vanillc_conn__.h"


// 64 bit Addition with Carry-over
uint64_t _cintrin_add64c(uint64_t a, uint64_t b, uint8_t *carry) {
    uint64_t sum = a + b; uint8_t ab_carry = (sum < a);
    a = sum + *carry; uint8_t final_carry = (a < sum);
    *carry = ab_carry | final_carry; return a;
}

// 64 bit Subtraction with Borrow-over
uint64_t _cintrin_sub64b(uint64_t a, uint64_t b, uint8_t *borrow) {
    uint64_t diff = a - b; uint8_t ab_borrow = (diff > a);
    a = diff - *borrow; uint8_t final_borrow = (a > diff);
    *borrow = ab_borrow | final_borrow; return a;
}

// Wide, 64 bit Multiplication of a 128 bit product:
//  +) Return the low 64 bit
//  +) Mutate the high 64 bit as a parameter
uint64_t _cintrin_wmul128(uint64_t a, uint64_t b, uint64_t *hi) {
    // Seperate a and b into 2 different halves
    uint64_t mask = (1ULL << 32) - 1;
    uint64_t a_low = a & mask;        uint64_t b_low = b & mask; // Extract the 32 lower bits
    uint64_t a_high = a >> 32;        uint64_t b_high = b >> 32; // Extract the 32 upper bits

    uint64_t first_mul = a_low * b_low;
    uint64_t second_mul = a_low * b_high;
    uint64_t third_mul = a_high * b_low;
    uint64_t fourth_mul = a_high * b_high;
    
    // Lower Half Calculation
    uint64_t mid = second_mul + third_mul;
    uint64_t mid_carry = (mid < second_mul); // Handles mid overflow (0 <= mid < 2^65)
    uint64_t mid_low = (mid & mask) << 32; // Extract and Isolate the lower 32 bit of mid
    uint64_t res = first_mul + mid_low; // Return the lower 64 bits
    /* Lower half of a bit is attained by the formula:  Res % 2^64 = low
    *   +) (a x b % 2^64) = (first_mul + mid * 2^32 + fourth_mul * 2^64) % 2^64
    *                     = (first_mul + mid * 2^32) % 2^64 (1)
    * 
    *   +) mid                      = mid_low + mid_high * 2^32
    *      mid * 2^32               = mid_low * 2^32 + mid_high * 2^64
    *      (mid * 2^32) % 2^64      = (mid_low * 2^32 + mid_high * 2^64) % 2^64
    *      (mid * 2^32) % 2^64      = mid_low * 2^32                                
    *                               = mid_low << 32 (2)
    * 
    * ----> (a x b % 2^64)      = first_mul + mid_low * 2^32
    * ----> (a x b) lower bits  = first_mul + mid_low * 2^32
    */

    // Upper Half Calculation
    uint64_t carry1 = (res < first_mul); // Check if adding mid_low to first_mul overflows to carry to the higher half
    uint64_t mid_high = mid >> 32; // The upper 32 bits of mid
    /* Higher half of a bit is attained by the formula:  floor(Res / 2^64) = high
    *   +) floor(a x b / 2^64) = floor((first_mul + mid * 2^32 + fourth_mul * 2^64) / 2^64)
    *                          = floor((first_mul / 2^64) + (mid * 2^-32) + fourth_mul)
    * 
    *   +) mid                  = mid_low + mid_high * 2^32
    *      mid * 2^-32          = (mid_low + mid_high * 2^32) / 2^32
    *      mid * 2^-32          = (mid_low / 2^32) + mid_high
    *      floor(mid * 2^-32)   = floor((mid_low / 2^32) + mid_high)
    *      floor(mid * 2^-32)   = floor({0 <= mid_low / 2^32 < 1} + mid_high)  (0 <= mid_low < 2^32)
    *      floor(mid * 2^-32)   = mid_high
    * 
    *   -----> floor(a x b / 2^64) = floor(first_mul / 2^64) + mid_high + fourth_mul
    *                              = mid_high + fourth_mul
    */

    // Takes the carry from lower half + the overflowed mid bit
    *hi = fourth_mul + carry1 + mid_high + (mid_carry << 32);
    return res;
}


// Wide Division - 128 bit dividend by 64 bit divisor
//  +) Return the 64 bit quotient - qhat
//  +) Mutates the 64 bit remainder parameter - rhat
//  +) Preconditions: hi < div
uint64_t _cintrin_wdiv128(uint64_t lo, uint64_t hi, uint64_t div, uint64_t *rhat, uint8_t *overflowed) {
    if (hi >= div) {
        if (overflowed) *overflowed = 1;
        *rhat = hi % div; return UINT64_MAX;
    }
    /* Normal Operations here */
    uint64_t q = 0;
    for (int i = U64_BITS - 1; i >= 0; --i) {
        q <<= 1;
        uint64_t test_rem = (hi << 1) | ((lo >> i) & 1);
        if (test_rem >= div) { q |= 1; hi = test_rem - div; }
        else hi = test_rem;
    }
    *rhat = hi; *overflowed = 0; return q;
}