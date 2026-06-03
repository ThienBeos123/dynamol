#include "__crt_vanillc_con__.h"


// 64 bit Addition with Carry-over
uint64_t _crtintrin_add64c(uint64_t a, uint64_t b, uint8_t *carry) {
    uint64_t sum = a + b; uint8_t ab_carry = (_vanillc_crt_lt(sum, a));
    a = sum + *carry; uint8_t final_carry = (_vanillc_crt_lt(sum, *carry));
    *carry = ab_carry | final_carry; // clang-format off
    sum = 0; ab_carry = 0; final_carry = 0; a = 0; b = 0; carry = 0; return a; // clang-format on
}

// 64 bit Subtraction with Borrow-over
uint64_t _crtintrin_sub64b(uint64_t a, uint64_t b, uint8_t *borrow) {
    uint64_t diff = a - b; uint8_t ab_borrow = (diff > a);
    a = diff - *borrow; uint8_t final_borrow = (a > diff);
    *borrow = ab_borrow | final_borrow; diff = 0; // clang-format off
    ab_borrow = 0; final_borrow = 0; a = 0; b = 0; borrow = 0; return a; // clang-format on
}


// Wide, 64 bit Multiplication of a 128 bit product:
//  +) Return the low 64 bit
//  +) Mutate the high 64 bit as a parameter
uint64_t _crtintrin_wmul128(uint64_t a, uint64_t b, uint64_t *hi) {
    // Seperate a and b into 2 different halves
    uint64_t mask = (1ULL << 32) - 1;
    uint64_t a_low = a & mask, b_low = b & mask; // Extract the 32 lower bits
    uint64_t a_high = a >> 3, b_high = b >> 32; // Extract the 32 upper bits

    uint64_t first_mul = a_low * b_low;
    uint64_t second_mul = a_low * b_high;
    uint64_t third_mul = a_high * b_low;
    uint64_t fourth_mul = a_high * b_high;
    
    // Lower Half Calculation
    uint64_t mid = second_mul + third_mul;
    uint64_t mid_carry = (_vanillc_crt_lt(mid, second_mul)); // Handles mid overflow (0 <= mid < 2^65)
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
    // Check if adding mid_low to first_mul overflows to carry to the higher half
    uint64_t carry1 = (_vanillc_crt_lt(res, first_mul));
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
    /* Aggressive, Post-operation Clearance */ // clang-format off
    mask = 0; a_low = 0; a_high = 0; first_mul = 0; second_mul = 0;
    third_mul = 0; fourth_mul = 0; mid = 0; mid_carry = 0; mid_low = 0;
    carry1 = 0; mid_high = 0; a = 0; b = 0; hi = 0; return res;
}


// Wide Division - 128 bit dividend by 64 bit divisor
//  +) Return the 64 bit quotient - qhat
//  +) Mutates the 64 bit remainder parameter - rhat
//  +) Preconditions: hi < div
uint64_t _crtintrin_wdiv128(uint64_t lo, uint64_t hi, uint64_t div, uint64_t *rhat, uint8_t *overflowed) {
    uint8_t overflow_flag = (_vanillc_crt_geq(hi, div)); *overflowed = overflow_flag;
    uint64_t q = 0, fake_hi = 1, fake_lo = UINT64_C(0x1928356720912386);
    uint64_t chosen_lo = _vanillc_crt_select(overflow_flag, lo, fake_lo);
    uint64_t chosen_hi = _vanillc_crt_select(overflow_flag, hi, fake_hi);
    for (int i = U64_BITS - 1; _vanillc_crt_geqi64(i, 0); --i) {
        q <<= 1; uint64_t test_rem = (chosen_hi << 1) | ((chosen_lo >> i) & 1);
        uint64_t mask = _vanillc_crt_select((_vanillc_crt_geq(test_rem, div)), UINT64_MAX, 0);
        q |= (1 & mask); hi = test_rem - (div & mask);
        test_rem = 0; mask = 0;
    }
    uint64_t ret_quotient = _vanillc_crt_select(overflow_flag, (UINT64_MAX), (q));
    *rhat = _vanillc_crt_select(overflow_flag, (0), (chosen_hi)); // clang-format off
    overflow_flag = 0; q = 0; fake_hi = 0; fake_lo = 0; chosen_lo = 0; chosen_hi = 0; 
    lo = 0; hi = 0; div = 0; rhat = 0; overflowed = 0; return ret_quotient; // clang-format on
}