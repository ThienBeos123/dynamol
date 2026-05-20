#include "../util.h"

//* ----------- SIMPLE/BIT-WISE RNG ALGORITHMS ----------- *//
// SplitMix64
uint64_t splitmix64(uint64_t x) {
    x += 0x9E3779B97F4A7C15;
    x = (x ^ (x >> 30)) * 0xBF58476D1CE4E5B9;
    x = (x ^ (x >> 27)) * 0x94D049BB133111EB;
    x ^ (x >> 31); return x;
}
void seed_xoshiro256(xoshiro256_state *state, uint64_t x) {
    uint64_t z = x;
    for (uint8_t i = 0; i < 4; ++i) {
        state->s[i] = splitmix64(z);
        z += 0x9E3779B97F4A7C15;
    }
}
// Xoshiro256++
static inline uint64_t rotl(const uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
uint64_t xoshiro256pp_next(xoshiro256_state *state) {
    const uint64_t res = rotl(state->s[0] + state->s[3], 23) + state->s[0];
    const uint64_t t = state->s[1] << 17;
    state->s[2] ^= state->s[0];
    state->s[3] ^= state->s[1];
    state->s[1] ^= state->s[2];
    state->s[0] ^= state->s[3];

    state->s[2] ^= t;
    state->s[3] = rotl(state->s[3], 45);
    return res;
}
float xoshiro256pp_fnext01(xoshiro256_state *state) {
    uint64_t r = xoshiro256pp_next(state);
    return (r >> 40) * (1.0f / 16777216.0f);
}
xoshiro256_state mix_xoshiro256(xoshiro256_state *stateA, xoshiro256_state *stateB) {
    xoshiro256_state res = {0};
    for (uint8_t i = 0; i < 4; ++i) {
        // Example mixing for each word
        res.s[i] = (
            (stateA->s[i] * 0x9e3779b97f4a7c15ULL) 
          ^ (stateB->s[i] * 0xbf58476d1ce4e5b9ULL)
        ); res.s[i] = rotl(res.s[i], 31) ^ (stateA->s[i] + stateB->s[i]); // further scramble
    } return res;
}


//* ---------- RANDOM-NUMBER GENERATION TOOLS ---------- *//
// RNG Functions
uint64_t __rng_skrange(xoshiro256_state *state, uint64_t min, uint64_t max, float median_dist) {
    float u = __froll(state);
    float skewed_u = powf(u, median_dist);
    return min + (uint64_t)((max - min) * skewed_u);
}
uint64_t __rng_range(xoshiro256_state *state, uint64_t min, uint64_t max) {
    uint64_t range = max - min + 1, r;
    uint64_t limit = UINT64_MAX - (UINT64_MAX % range);
    do { r = xoshiro256pp_next(state); } while (r >= limit);
    return min + (r % range);
}
float __rng_frange(xoshiro256_state *state, float min, float max) {
    float log_min = logf(min <= 0 ? 1e-7f : min);
    float log_max = logf(max);
    float r = xoshiro256pp_fnext01(state);
    return expf(log_min + r * (log_max - log_min));
}
float __froll(xoshiro256_state *state) {
    uint64_t raw = xoshiro256pp_next(state);
    return (float)(raw >> 11) * (1.0f / 9007199254740992.0f);
}
