#ifndef DNML_BI_SETUP_H
#define DNML_BI_SETUP_H


#include "../../../adynamol/big_numbers/bigNums.h"
#include "../../../test_ui/_strui.h"
#include "../../../intrinsics/intrinsics.h"
#include "../../case_gen/case_gen.h"
#include "../_ioconv.h"
#include "bi_indef.h"
#include <include.h>
#include <system/sys.h>

typedef rcap_mode cmode;

//* ===================== AUXILLARY SETUP FUNCTIONS ===================== *//
// Auxillary 1 Size returner
size_t _stobi_recon_size(void) { return sizeof(unsigned char*); }
size_t _bitos_recon_size(void) { return sizeof(bigInt); }
// Auxilary 1 Linker
void _stobi_recon_linker(void *in, rctx_input_t *rctx) {
    unsigned char *cin = (unsigned char*)in;
    cin = rctx->aux1_buf;
}
void _bitos_recon_linker(void *in, rctx_input_t *rctx) {
    bigInt *bin = (bigInt*)in;
    bin->limbs = (limb_t*)rctx->aux1_buf;
}


//* ===================== INPUT SETUP FUNCTIONS ===================== *//
// Input Buffer Size returner -- BITOS
size_t _bitos_conv_insize(void) { return sizeof(bitos_conv_in); }
size_t _bitos_print_insize(void) { return sizeof(bitos_print_in); }
size_t _bitos_fwrite_insize(void) { return sizeof(bitos_fwrite_in); }
size_t _bitos_serialize_insize(void) { return sizeof(bitos_serialize_in); }
size_t _bitos_util_insize(void) { return sizeof(bitos_util_in); }
// Input Buffer Size returner -- STOBI
size_t _stobi_init_insize(void) { return sizeof(stobi_init_in); }
size_t _stobi_conv_insize(void) { return sizeof(stobi_conv_in); }
size_t _stobi_assign_insize(void) { return sizeof(stobi_assign_in); }
size_t _stobi_scan_insize(void) { return sizeof(stobi_scan_in); }
size_t _stobi_fread_insize(void) { return sizeof(stobi_fread_in); }
size_t _stobi_deserialize_insize(void) { return sizeof(stobi_deserialize_in); }
// Input Buffer Linker - BITOS
void _bitos_conv_inlink(void *in, rand_container *incon) {
    bitos_conv_in* vin = (bitos_conv_in*)in;
    vin->x.limbs = (limb_t*)incon->in_cont.rctx->in_buf;
}
void _bitos_print_inlink(void *in, rand_container *incon) {
    bitos_print_in* vin = (bitos_print_in*)in;
    vin->x.limbs = (limb_t*)incon->in_cont.rctx->in_buf;
}
void _bitos_fwrite_inlink(void *in, rand_container *incon) {
    bitos_fwrite_in* vin = (bitos_fwrite_in*)in;
    vin->x.limbs = (limb_t*)incon->in_cont.rctx->in_buf;
}
void _bitos_serialize_inlink(void *in, rand_container *incon) {
    bitos_serialize_in* vin = (bitos_serialize_in*)in;
    vin->x.limbs = (limb_t*)incon->in_cont.rctx->in_buf;
}
void _bitos_util_inlink(void *in, rand_container *incon) {
    bitos_util_in* vin = (bitos_util_in*)in;
    vin->x.limbs = (limb_t*)incon->in_cont.rctx->in_buf;
}
// Input Buffer Linker - STOBI
void _stobi_init_inlink(void *in, rand_container *incon) {
    stobi_init_in* vin = (stobi_init_in*)in;
    vin->str = (char*)incon->in_cont.rctx->in_buf;
}
void _stobi_conv_inlink(void *in, rand_container *incon) {
    stobi_conv_in* vin = (stobi_conv_in*)in;
    vin->str = (char*)incon->in_cont.rctx->in_buf;
}
void _stobi_assign_inlink(void *in, rand_container *incon) {
    stobi_assign_in* vin = (stobi_assign_in*)in;
    vin->str = (char*)incon->in_cont.rctx->in_buf;
}
void _stobi_scan_inlink(void *in, rand_container *incon) {
    stobi_scan_in* vin = (stobi_scan_in*)in;
    vin->stream = incon->in_cont.stream;
}
void _stobi_fread_inlink(void *in, rand_container *incon) { 
    stobi_fread_in* vin = (stobi_fread_in*)in;
    vin->stream = incon->in_cont.stream;
}
void _stobi_deserialize_inlink(void *in, rand_container *incon) {
    stobi_deserialize_in* vin = (stobi_deserialize_in*)in;
    vin->str = (char*)incon->in_cont.rctx->in_buf;
}


//* ===================== OUTPUT SETUP FUNCTIONS ===================== *//
// Output Buffer Linker - BITOS
void _bitos_outlink(str_res *out, rand_container *rcon) {
    out = (str_res*)(rcon->res_cont->res_buf);
    out->type = STRING; out->cap = STR_CAP;
    out->data.len = 0;
}
void _bitos_aux2link(str_res *out, rand_container *rcon) {
    out = (str_res*)(rcon->res_cont->aux2_buf);
    out->type = STRING; out->cap = STR_CAP;
    out->data.len = 0;
}
// Output Buffer Linker - STOBI
void _stobi_outlink(str_res *out, rand_container *rcon) {
    out = (str_res*)(rcon->res_cont->res_buf);
    limb_t *limb = (limb_t*)(
        rcon->res_cont->res_buf 
      + ALIGN_UP(sizeof(str_res), _Alignof(max_align_t))
    );
    out->type = BIGINT; out->cap = 0; // cap refers to the FAM string buffer
    out->data.bi.limbs = limb;
    out->data.bi.n = 0;
    out->data.bi.cap = BIGINT_CAP;
    out->data.bi.sign = 1;
}
void _stobi_aux2link(str_res *aux2, rand_container *rcon) {
    aux2 = (str_res*)(rcon->res_cont->aux2_buf);
    limb_t *limb = (limb_t*)(
        rcon->res_cont->aux2_buf 
      + ALIGN_UP(sizeof(str_res), _Alignof(max_align_t))
    );
    aux2->type = BIGINT; aux2->cap = 0; // cap refers to the FAM string buffer
    aux2->data.bi.limbs = limb;
    aux2->data.bi.n = 0;
    aux2->data.bi.cap = BIGINT_CAP;
    aux2->data.bi.sign = 1;
}



//* ===================== INPUT GENERATION FUNCTIONS ===================== *//
size_t bisize_to_rcap_dist(size_t len, uint8_t base, cmode mode, rstate *state) {
    switch (mode) {
        case SATISFACTORY: {
            size_t bits = __BITCOUNT___(len, base);
            return __BIGINT_LIMBS_NEEDED__(bits); break;
        }
        case NEAR_SATISFACTORY: {
            size_t inlen = (size_t)(__rng_frange(state, 0.7, 0.9) * len);
            size_t bits = __BITCOUNT___(inlen, base);
            return __BIGINT_LIMBS_NEEDED__(bits); break;
        }
        case UNSATISFACTORY: {
            size_t inlen = (size_t)(__rng_frange(state, 0.3, 0.6) * len);
            size_t bits = __BITCOUNT___(inlen, base);
            return __BIGINT_LIMBS_NEEDED__(bits); break;
        }
        case FAULTY: {
            size_t inlen = (size_t)(__rng_frange(state, 0.0, 0.25) * len);
            size_t bits = __BITCOUNT___(inlen, base);
            return __BIGINT_LIMBS_NEEDED__(bits); break;
        }
    }
}
size_t strsize_to_rcap_dist(const bigInt *x, 
    uint8_t base, bool bprefix,
    cmode mode, rstate *state
) {
    switch (mode) {
        case SATISFACTORY: {
            size_t raw_bytes = __BIGINT_COUNTDB__(x, base);
            raw_bytes += (x->sign) ? 1 : 0;
            raw_bytes += (bprefix && base != 10) ? (
                (base == 2 || base == 8 || base == 16) ? 2 : (
                    (base < 10) ? 4 : ((base < 100) ? 5 : 6)
                )
            ) : 0;
            return raw_bytes; break;
        }
        case NEAR_SATISFACTORY: {
            size_t raw_bytes = __BIGINT_COUNTDB__(x, base);
            raw_bytes += (x->sign) ? 1 : 0;
            raw_bytes += (bprefix && base != 10) ? (
                (base == 2 || base == 8 || base == 16) ? 2 : (
                    (base < 10) ? 4 : ((base < 100) ? 5 : 6)
                )
            ) : 0;
            raw_bytes *= __rng_frange(state, 0.7f, 0.9f);
            return raw_bytes; break;
        }
        case UNSATISFACTORY: {
            size_t raw_bytes = __BIGINT_COUNTDB__(x, base);
            raw_bytes += (x->sign) ? 1 : 0;
            raw_bytes += (bprefix && base != 10) ? (
                (base == 2 || base == 8 || base == 16) ? 2 : (
                    (base < 10) ? 4 : ((base < 100) ? 5 : 6)
                )
            ) : 0;
            raw_bytes *= __rng_frange(state, 0.3f, 0.6f);
            return raw_bytes; break;
        }
        case FAULTY: {
            size_t raw_bytes = __BIGINT_COUNTDB__(x, base);
            raw_bytes += (x->sign) ? 1 : 0;
            raw_bytes += (bprefix && base != 10) ? (
                (base == 2 || base == 8 || base == 16) ? 2 : (
                    (base < 10) ? 4 : ((base < 100) ? 5 : 6)
                )
            ) : 0;
            raw_bytes *= __rng_frange(state, 0.0f, 0.25f);
            return raw_bytes; break;
        }
    }
}
// Input Random Generatioon - BITOS
void _bitos_conv_ingen(void *in, void *rconfig, rstate *state, cmode incap, rand_container *rcont) {
    bitos_conv_in *vin = (bitos_conv_in*)in;
    bi_rand_mod *config = (bi_rand_mod*) rconfig;
    // --- BigInt Generation Setup
    bigen_write(&vin->x, config);
    vin->base = __rng_range(state, STR_MIN_BASE, STR_MAX_BASE); vin->uppercase = false; 
    vin->len = strsize_to_rcap_dist(&vin->x, vin->base, false, incap, state);
}
void _bitos_convf_ingen(void *in, void *rconfig, rstate *state, cmode incap, rand_container *rcont) {
    bitos_conv_in *vin = (bitos_conv_in*)in;
    bi_rand_mod *config = (bi_rand_mod*) rconfig;
    // --- BigInt Generation Setup
    bigen_write(&vin->x, config);
    vin->base = __rng_range(state, STR_MIN_BASE, STR_MAX_BASE);
    vin->uppercase = __rng_range(state, 0, 1);
    vin->len = strsize_to_rcap_dist(&vin->x, vin->base, true, incap, state);
}
void _bitos_print_ingen(void *in, void *rconfig, rstate *state, cmode incap, rand_container *rcont) {
    bitos_print_in *vin = (bitos_print_in*)in;
    bi_rand_mod *config = (bi_rand_mod*) rconfig;
    // --- BigInt Generation Setup
    bigen_write(&vin->x, config);
    vin->base = __rng_range(state, STR_MIN_BASE, STR_MAX_BASE); 
    vin->uppercase = false;
}
void _bitos_fwrite_ingen(void *in, void *rconfig, rstate *state, cmode incap, rand_container *rcont) { DNML_UNFINISHED(); }
void _bitos_serialize_ingen(void *in, void *rconfig, rstate *state, cmode incap, rand_container *rcont) { DNML_UNFINISHED(); }
void _bitos_util_ingen(void *in, void *rconfig, rstate *state, cmode incap, rand_container *rcont) { DNML_UNFINISHED(); }
// Input Random Generatioon - STOBI
void _stobi_init_ingen_nob(void *in, void *rconfig, rstate *state, cmode incap, rand_container *rcont) {
    stobi_init_in *vin = (stobi_init_in*)in;
    str_rand_mod *config = (str_rand_mod*)rconfig;
    // --- String Generation setup
    strgen_write(vin->str, STR_CAP, &config, true);
    vin->len = config->str_len; vin->base = config->base; // Base is set for safety
}
void _stobi_init_ingen_b(void *in, void *rconfig, rstate *state, cmode incap, rand_container *rcont) {
    stobi_init_in *vin = (stobi_init_in*)in;
    str_rand_mod *config = (str_rand_mod*)rconfig;
    // --- Random Generation
    uint64_t scramble_eggs; // haha very funny 😂😂😂😂
    __GET_ENTROPY_FAST(&scramble_eggs, sizeof(uint64_t));
    __GET_ENTROPY_FAST(state->s, sizeof(uint64_t) << 2);
    seed_xoshiro256(state, scramble_eggs);
    // --- String Generation setup
    uint8_t rand_gmode = __rng_range(state, 0, 2);
    strgen_write(vin->str, STR_CAP, &config, false);
    vin->len = config->str_len; vin->base = config->base;
}
void _stobi_conv_ingen_nob(void *in, void *rconfig, rstate *state, cmode incap, rand_container *rcont) {
    stobi_conv_in *vin = (stobi_conv_in*)in;
    str_rand_mod *config = (str_rand_mod*)rconfig;
    // --- Random Generation
    uint64_t scramble_eggs; // haha very funny 😂😂😂😂
    __GET_ENTROPY_FAST(&scramble_eggs, sizeof(uint64_t));
    __GET_ENTROPY_FAST(state->s, sizeof(uint64_t) << 2);
    seed_xoshiro256(state, scramble_eggs);
    // --- String Generation setup
    uint8_t rand_gmode = __rng_range(state, 0, 2);
    strgen_write(vin->str, STR_CAP, &config, true);
    vin->len = config->str_len; vin->base = config->base; // Base is set for safety
}
void _stobi_conv_ingen_b(void *in, void *rconfig, rstate *state, cmode incap, rand_container *rcont) {
    stobi_conv_in *vin = (stobi_conv_in*)in;
    str_rand_mod *config = (str_rand_mod*)rconfig;
    // --- Random Generation
    uint64_t scramble_eggs; // haha very funny 😂😂😂😂
    __GET_ENTROPY_FAST(&scramble_eggs, sizeof(uint64_t));
    __GET_ENTROPY_FAST(state->s, sizeof(uint64_t) << 2);
    seed_xoshiro256(state, scramble_eggs);
    // --- String Generation setup
    strgen_write(vin->str, STR_CAP, &config, false);
    vin->len = config->str_len; vin->base = config->base;
}
void _stobi_assign_ingen_nob(void *in, void *rconfig, rstate *state, cmode incap, rand_container *rcont) {
    stobi_assign_in *vin = (stobi_assign_in*)in;
    str_rand_mod *config = (str_rand_mod*)rconfig;
    // --- Random Generation
    uint64_t scramble_eggs; // haha very funny 😂😂😂😂
    __GET_ENTROPY_FAST(&scramble_eggs, sizeof(uint64_t));
    __GET_ENTROPY_FAST(state->s, sizeof(uint64_t) << 2);
    seed_xoshiro256(state, scramble_eggs);
    // --- String Generation setup
    strgen_write(vin->str, STR_CAP, &config, true);
    vin->len = config->str_len; vin->base = config->base; // Base is set for safety
    vin->bi_size = bisize_to_rcap_dist(config->str_len, config->base, incap, state);
}
void _stobi_assign_ingen_b(void *in, void *rconfig, rstate *state, cmode incap, rand_container *rcont) {
    stobi_assign_in *vin = (stobi_assign_in*)in;
    str_rand_mod *config = (str_rand_mod*)rconfig;
    // --- Random Generation
    uint64_t scramble_eggs; // haha very funny 😂😂😂😂
    __GET_ENTROPY_FAST(&scramble_eggs, sizeof(uint64_t));
    __GET_ENTROPY_FAST(state->s, sizeof(uint64_t) << 2);
    seed_xoshiro256(state, scramble_eggs);
    // --- String Generation setup
    strgen_write(vin->str, STR_CAP, &config, false);
    vin->len = config->str_len; vin->base = config->base;
    vin->bi_size = bisize_to_rcap_dist(config->str_len, config->base, incap, state);
}
void _stobi_scan_ingen_nob(void *in, void *rconfig, rstate *state, cmode incap, rand_container *rcont) {
    stobi_scan_in *vin = (stobi_scan_in*)in;
    str_rand_mod *config = (str_rand_mod*)rconfig;
    // --- Random Generation
    uint64_t scramble_eggs; // haha very funny 😂😂😂😂
    __GET_ENTROPY_FAST(&scramble_eggs, sizeof(uint64_t));
    __GET_ENTROPY_FAST(state->s, sizeof(uint64_t) << 2);
    seed_xoshiro256(state, scramble_eggs);
    // --- String Generation setup
    strgen_write((char*)(rcont->in_cont.rctx->in_buf), STR_CAP, &config, true);
    fwrite(rcont->in_cont.rctx->in_buf, sizeof(char), config->str_len, vin->stream);
    memset(rcont->in_cont.rctx->in_buf, 0, STR_CAP); vin->base = config->base;
    vin->bi_size = bisize_to_rcap_dist(config->str_len, config->base, incap, state);
} 
void _stobi_scan_ingen_b(void *in, void *rconfig, rstate *state, cmode incap, rand_container *rcont) {
    stobi_scan_in *vin = (stobi_scan_in*)in;
    str_rand_mod *config = (str_rand_mod*)rconfig;
    // --- Random Generation
    uint64_t scramble_eggs; // haha very funny 😂😂😂😂
    __GET_ENTROPY_FAST(&scramble_eggs, sizeof(uint64_t));
    __GET_ENTROPY_FAST(state->s, sizeof(uint64_t) << 2);
    seed_xoshiro256(state, scramble_eggs);
    // --- String Generation setup
    strgen_write((char*)(rcont->in_cont.rctx->in_buf), STR_CAP, &config, false);
    fwrite(rcont->in_cont.rctx->in_buf, sizeof(char), config->str_len, vin->stream);
    memset(rcont->in_cont.rctx->in_buf, 0, STR_CAP); vin->base = config->base;
    vin->bi_size = bisize_to_rcap_dist(config->str_len, config->base, incap, state);
}
void _stobi_fread_ingen(void *in, void *rconfig, rstate *state, cmode incap, rand_container *rcont) { DNML_UNFINISHED(); }
void _stobi_deserialize_ingen(void *in, void *rconfig, rstate *state, cmode incap, rand_container *rcont) { DNML_UNFINISHED(); }





#endif