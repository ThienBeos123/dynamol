#ifndef ___LIBDNML_IOTEST_UI
#define ___LIBDNML_IOTEST_UI



#include <include.h>
#include "../adynamol/big_numbers/bigNums.h"
#include "../sconfigs/dnml_status.h"
#include "../sconfigs/memory/_scratch.h"
#include "../util/util.h"
#include "_test_base.h"
#include "str_ctx.h"


//* =========== TYPE DEFINITIONS =========== *//
// Small, supporting types
typedef struct strbump_t { void *ctx; size_t off; size_t size } strbump_t;
typedef enum rcap_mode { 
    SATISFACTORY, NEAR_SATISFACTORY, 
    UNSATISFACTORY, FAULTY 
} rcap_mode;

// Cases & Suites
typedef struct _libdnml_scase {
    void* in;
    str_res exp;
    str_res res;
    void* recons;
} _libdnml_scase;

//* =========== TYPE-SPECIFIC UTILITIES =========== *//
static inline int move_forward(strbump_t *ctx, str_res *out) {
    assert(out->type); size_t amount;
    switch (out->type) {
        case BIGINT: amount = out->data.bi.n; break;
        case STRING: amount =  out->data.len; break;
    }
    if (amount > ctx->size) return 1;
    if (ctx->off + amount > ctx->size) return 1;
    ctx += amount; return 0;
}
static inline rcap_mode _rand_rcap(xoshiro256_state *state) {
    float roll = fmodf(__seed_to_float(state), 100.0f);
    if (roll < 25.0f) return FAULTY;
    else if (roll >= 25.0f && roll < 50.0f) return UNSATISFACTORY;
    else if (roll >= 50.0f && roll < 75.0f) return NEAR_SATISFACTORY;
    else return SATISFACTORY;
}
static inline bool _comp_str_res(const str_res *a, const str_res *b) {
    if (a->status != b->status) return false;
    if (a->status != BIGINT_SUCCESS || a->status != STR_SUCCESS) {
        return (a->status == b->status);
    } if (a->type != b->type) return false;
    switch (a->type) {
        case STRING: return strcmp(a->str, b->str) == 0; break;
        case BIGINT: return (__BIGINT_INTERNAL_COMP__(&a->data.bi, &b->data.bi) == 0); break;
        default: return true; break;
    };
}
static inline void _print_str(FILE *f, const char *s, size_t len, int tab_depth) {
    /* Example:
        - Input: <
            +) Input 1 - <char*>: "123456789abcdef...123456789abcdef" (Length = 2048)
            +) Random Text ....
            +) Random Text ....
        >
    */
    if (len <= STR_PREVIEW) {
        fprintf(f, "< \"%.*s\">" , len, s);
    } else {
        fputs("<\n", f); char buf[STR_PREVIEW];
        for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
        memcpy(buf, s, STR_PREVIEW); 
        fprintf(f, "--- Low Segment:  \"%.*s\"...\n", STR_PREVIEW, buf);
        for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
        memcpy(buf, s[len - STR_PREVIEW - 1], STR_PREVIEW);
        fprintf(f, "--- High Segment: \"%.*s\"...\n", STR_PREVIEW, buf);
        for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
        fprintf(f, "--- Length: %zu\n", len);
        for (int i = 0; i < tab_depth; ++i) fputs(TAB, f);
        fputc('>', f);
    }
}
static inline void _print_bigint(FILE *f, const bigInt *x, int tab_depth) {
    /* Example:
        +) .... I love femboy text ...: <
            --- Limb Count: 1024 (limbs)
            --- Sign: 1 (Positive +)
            --- Low Limbs: [12345678901234567890, ...]
            --- High Limbs: [..., 12345678901234567890]
        >
    */
    fputs("<\n", f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fprintf(f, "--- Limb Count: %zu\n", x->n);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fprintf(f, "--- Sign: %" PRIu8 " (%s %c)\n", x->sign, 
        (x->sign == 1) ? "Positive" : "Negative", 
        (x->sign == 1) ? '+' : '-'
    );
    if (x->n <= BIGINT_PREVIEW) {
        // Full Limbs
        for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
        fputs("--- Limbs: [", f); 
        for (size_t i = 0; i < x->n; ++i) {
            fprintf(f, "%016" PRIx64 ", ", x->limbs[i]);
        } fputs("]\n", f);
    } else {
        // Low Limbs
        for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
        fputs("--- Low Limbs: [", f); 
        for (size_t i = 0; i < BIGINT_PREVIEW/2; ++i) {
            fprintf(f, "%016" PRIx64 " ", x->limbs[i]);
        } fputs("]\n", f);

        // High Limbs
        for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
        fputs("--- High Limbs: [", f);
        for (size_t i = x->n - BIGINT_PREVIEW/2; i < x->n; ++i) {
            fprintf(f, "%016" PRIx64 " ", x->limbs[i]);
        } fputs(']', f);
    }
    // The final close-bracket ">"
    for (int i = 0; i < tab_depth; ++i) fputs(TAB, f);
    fputc('>', f);
}
static inline void _print_str_res(const str_res *a, FILE *f, int tab_depth) {
    fputs("< -- STR-RES --\n", f);
    for (int i = 0; i <= tab_depth; ++i) fputs(TAB, f);
    fputs("+) Status: ", f); _print_dnml_status(a->status, f); fputc('\n', f);
    fputs("+) Data:   ", f); switch (a->type) {
        case BIGINT: _print_bigint(f, &a->data.bi, tab_depth); break;
        case STRING: _print_str(f, a->str, a->data.len, tab_depth); break;
    } for (int i = 0; i < tab_depth; ++i) fputs(TAB, f); 
    fputs("\n>", f);
}


//* =================== FUNCTION-GENERALIZATION DISPATCHER =================== *//
typedef void (*dnml_gen_fn)(void *in, xoshiro256_state *state, rcap_mode cap_mode, rctx_t *rctx);
typedef void (*dnml_exec_fn)(const void *in, str_res *out, void *ctx);
typedef bool (*dnml_prop_fn)(const void *in, str_res *out);
// Evaluators & Inverses
typedef void (*dnml_eval_fn)(const void *in, str_res *exp, void *ctx);
typedef void (*dnml_inv_fn)(const void *in, const str_res *out, void *reconstructed, void *ctx);
// Comparisons
typedef bool (*dnml_stat_fn)(const void *in, dnml_status res_stat, str_res *out);
typedef bool (*dnml_cmp_inv_fn)(const void *original, const str_res *out, const void *recon, void *ctx);
typedef bool (*dnml_cmp_eval_fn)(const str_res *exp, const str_res *out);
// Printing & Formatting & Utilites
typedef void (*dnml_fmt_in_fn)(FILE *f, const void *in, int tab_depth);
typedef void (*dnml_fmt_recon_fn)(FILE *f, const void* recon, int tab_depth);
typedef size_t (*dnml_voidp_size)(void);
typedef void (*dnml_voidp_fill)(void *in, rctx_t *rctx);

static inline void *run_ecase(_libdnml_scase *c, dnml_exec_fn *fn, void *ctx) {
    (*fn)(c->in, &c->res, ctx);
}
static inline void *run_case(_libdnml_str_suite *s, dnml_exec_fn *fn, void *in, str_res *out) {
    (*fn)(in, out, s->rctx->res_buf);
}
static inline void *run_eval(_libdnml_str_suite *s, dnml_eval_fn *fn,  void *in, str_res *aux2) {
    (*fn)(in, aux2, s->rctx->aux2_buf);
}
static inline void *run_inverse(
    _libdnml_str_suite *s, dnml_inv_fn *fn,
    void *in, str_res *out, void *aux1
) {
    (*fn)(in, out, aux1, s->rctx->aux1_buf);
}



//* =================== TEST CREATION FUNFCTIONS =================== *//
typedef struct _libdnml_str_suite {
    const char *suite_name; const char *log_path;
    xoshiro256_state *state;

    dnml_gen_fn *gen_case;
    dnml_exec_fn *fn_test; 
    // Random Case Oracle Functions
    dnml_voidp_size *fn_insize; dnml_voidp_size *fn_aux1size;
    dnml_inv_fn *fn_inv; dnml_eval_fn *fn_eval;
    dnml_cmp_inv_fn *inv_cmp; dnml_cmp_eval_fn *eval_cmp;     
    dnml_fmt_in_fn *fmtin_fn; dnml_stat_fn *fn_stat;
    dnml_fmt_recon_fn *fmtrecon_fn; 
    dnml_voidp_fill *fn_infill; dnml_voidp_fill *fn_aux1fill;
    // Property Case Functions
    dnml_prop_fn *fn_prop;

    // Edge cases storage
    _libdnml_scase *edge; strbump_t ectx;
    uint8_t ecount; uint8_t ecorrect;
    str_res *fail_eres; str_res *fail_eexp;

    // Random cases Handling
    rcheck_mode check_mode;
    uint16_t rcount; uint16_t rcorrect;
    rctx_t *rctx; int fail_enums[];
} _libdnml_str_suite;


// Main Suite Setup
static inline void create_str_suite(
    _libdnml_str_suite *curr_suite, const char *name,
    uint8_t ecount, uint16_t rcount, _libdnml_scase *ebank,
    rcheck_mode mode, str_res *fail_ebuf, const char *log_path,
    strbump_t ectx, rctx_t *rctx
) {
    curr_suite->suite_name = name;
    curr_suite->ecount = ecount;
    curr_suite->rcount = rcount;
    curr_suite->log_path = log_path;
    // Filling in the banks
    curr_suite->edge = ebank;
    curr_suite->ectx = ectx;
    curr_suite->fail_eres = fail_ebuf;
    curr_suite->fail_eexp = &fail_ebuf[ecount];
    // Assigning random-case failure fail_ebuf
    curr_suite->rctx = rctx;
    curr_suite->check_mode = mode;
}


// Suite-specific setup (function evaluation-based)
// fill_suite_prop is unfinished
static inline void fill_suite_prop(_libdnml_str_suite *curr_suite, bool *prop_fn) {
    curr_suite->fn_prop = prop_fn;
}
static inline void fill_suite_rinv(
    _libdnml_str_suite *curr_suite, 
    void *case_gen, void *fn_test,
    void *fn_inv, bool *fn_stat,
    bool *cmp_inv, void *fmtin_fn, void *fmtrecon_fn
) {
    curr_suite->gen_case = (dnml_gen_fn*)(case_gen);
    curr_suite->fn_test = (dnml_exec_fn*)(fn_test);
    // Evaluators
    curr_suite->fn_inv = (dnml_inv_fn*)(fn_inv);
    curr_suite->fn_stat = (dnml_stat_fn*)(fn_stat);
    curr_suite->inv_cmp = (dnml_cmp_inv_fn*)(cmp_inv);
    // Priting & Formatting
    curr_suite->fmtin_fn = (dnml_fmt_in_fn*)(fmtin_fn);
    curr_suite->fmtrecon_fn = (dnml_fmt_recon_fn*)(fmtrecon_fn);
}
static inline void fill_suite_reval(
    _libdnml_str_suite *curr_suite, 
    void *case_gen, void *fn_test, void *fn_eval,
    bool *fn_stat, bool *cmp_eval
) {
    curr_suite->gen_case = (dnml_gen_fn*)(case_gen);
    curr_suite->fn_test = (dnml_exec_fn*)(fn_test);
    // Evaluators
    curr_suite->fn_eval = (dnml_eval_fn*)(fn_eval);
    curr_suite->fn_stat = (dnml_stat_fn*)(fn_stat);
    curr_suite->eval_cmp = (dnml_cmp_inv_fn*)(cmp_eval);
}


// Session Setup
static inline void create_str_session(
    _libdnml_session *curr_session,
    const char *session_name, uint8_t cli_delay,
    uint8_t suite_count, _libdnml_str_suite *suite_list,
    _dnml_output_mode output_mode
) {
    curr_session->session_name = session_name;
    curr_session->cli_delay = cli_delay;
    curr_session->suite_count = suite_count;
    curr_session->suites = suite_list;
    curr_session->output_mode = output_mode;
    curr_session->box_width = _dnml_box_width();
}



//* ================= SUITE HELPER FUNCTIONS ================= *//
#define RAND_PARAM_CONV _libdnml_str_suite *s, uint16_t i, FILE *f, void *in, void *aux_1, str_res *aux_2, str_res *out
static inline void __dnml_log_stcase(RAND_PARAM_CONV) {
    if (s->log_path || f == NULL) return;
    /* Example Result:
        o) Rand case 32:
            - Input: <...depends_on_format...>
            - Expected: STR_SUCCESS
            - Got: STR_NULL
    */
    fprintf(f,  "o) Rand case %" PRIu16 ":\n", i);
    fputs(      "    - Input: ", f);
    (*s->fmtin_fn)(f, in, 1); fputc('\n', f);
    fputs(      "    - Expected: ", f);
    _print_str_res(aux_2, f, 1); fputc('\n', f);
    fputs(      "    - Got:      ", f);
    _print_str_res(out, f, 1); fputc('\n', f);
}
static inline void __dnml_print_stcase(RAND_PARAM_CONV, int bw, uint32_t delay_ms) {
    // Printing the Case Index
    int fail_edge = s->ecount - s->ecorrect;
    char curr_index[10], fail_line[bw]; FILE *tmp = tmpfile(); 
    if (tmp == NULL) { 
        fprintf(stderr, "Failed to open a tmpfile(), Terminating..."); abort(); 
    } memset(fail_line, (char)(1), bw);
    int curri_len = _itosn(i, curr_index, sizeof(curr_index));
    snprintf(fail_line, sizeof(fail_line), "Case %" PRIu16 ": \n", i);
    _dnml_box_line(fail_line, bw);

    // Printing out the input
    freopen(NULL, "w", tmp);
    fputs("    - Input: ", tmp);
    (*s->fmtin_fn)(tmp, in, 1);
    _dnml_box_fmultiline(tmp, bw); putchar('\n');

    // Printing out the Expected Status
    freopen(NULL, "w", tmp);
    fputs("    - Expected: ", tmp);
    _print_dnml_status(aux_2->status, tmp);
    _dnml_box_line(tmp, bw); putchar('\n');

    // Printing out the Result Status
    freopen(NULL, "w", tmp);
    fputs("    - Got: ", tmp);
    _print_dnml_status(out->status, tmp);
    _dnml_box_line(tmp, bw); putchar('\n');

    _dnml_delay_ms(delay_ms);
}
static inline void __dnml_log_invc(RAND_PARAM_CONV) {
    if (s->log_path || f == NULL) return;
    /* Example Result:
        o) Rand case 32:
            - Input: <...depends_on_format...>
            - Output: < -- STR-RES --
                +) Status: STR_SUCCESS
                +) Data: "123456789abcdef...123456789abcdef" (Length = 1024)
            >
            - Reconstruction: <...depends_on_format...>
    */
    fprintf(f,  "o) Rand case %" PRIu16 ":\n", i);
    fputs(      "    - Input: ", f);
    (*s->fmtin_fn)(f, in, 1); fputc('\n', f);
    fputs(      "    - Output: ", f);
    _print_str_res(out, f, 1); fputc('\n', f);
    fputs(      "    - Reconstruction: ", f);\
    (*s->fmtrecon_fn)(f, aux_1, 1); fputc('\n', f);
}
static inline void __dnml_print_invc(RAND_PARAM_CONV, int bw, uint32_t delay_ms) {
    // Printing the Case Index
    int fail_edge = s->ecount - s->ecorrect;
    char curr_index[10], fail_line[bw]; FILE *tmp = tmpfile(); 
    if (tmp == NULL) { 
        fprintf(stderr, "Failed to open a tmpfile(), Terminating..."); abort(); 
    } memset(fail_line, (char)(1), bw);
    int curri_len = _itosn(i, curr_index, sizeof(curr_index));
    snprintf(fail_line, sizeof(fail_line), "Case %" PRIu16 ": \n", i);
    _dnml_box_line(fail_line, bw);

    // Printing out the input
    freopen(NULL, "w", tmp);
    fputs("    - Input: ", tmp);
    (*s->fmtin_fn)(tmp, in, 1);
    _dnml_box_fmultiline(tmp, bw); putchar('\n');

    // Printing out the Intermediate Output/Result
    freopen(NULL, "w", tmp);
    fputs("    - Result: ", tmp);
    _print_str_res(out, tmp, 1);
    _dnml_box_fmultiline(tmp, bw); putchar('\n');

    // Printing out the Reconstruction
    freopen(NULL, "w", tmp);
    fputs("    - Reconstruction: ", tmp);
    (*s->fmtrecon_fn)(tmp, aux_1, 1);
    _dnml_box_fmultiline(tmp, bw); putchar('\n');

    _dnml_delay_ms(delay_ms);
}
static inline void __dnml_log_evalc(RAND_PARAM_CONV) {
    if (s->log_path || f == NULL) return;
    /* Example Result:
        o) Rand case 32:
            - Output: < -- STR-RES --
                +) Status: STR_SUCCESS
                +) Data: "123456789abcdef...123456789abcdef" (Length = 1024)
            >
            - Reconstruction: <...depends_on_format...>
    */
    fprintf(f,  "o) Rand case %" PRIu16 ":\n", i);
    fputs(      "    - Input: ", f);
    (*s->fmtin_fn)(f, in, 1); fputc('\n', f);
    fputs(      "    - Expected: ", f);
    _print_str_res(aux_2, f, 1); fputc('\n', f);
    fputs(      "    - Got:      ", f);
    _print_str_res(out, f, 1); fputc('\n', f);
}
static inline void __dnml_print_evalc(RAND_PARAM_CONV, int bw, uint32_t delay_ms) {
    // Printing the Case Index
    int fail_edge = s->ecount - s->ecorrect;
    char curr_index[10], fail_line[bw]; FILE *tmp = tmpfile(); 
    if (tmp == NULL) { 
        fprintf(stderr, "Failed to open a tmpfile(), Terminating..."); abort(); 
    } memset(fail_line, (char)(1), bw);
    int curri_len = _itosn(i, curr_index, sizeof(curr_index));
    snprintf(fail_line, sizeof(fail_line), "Case %" PRIu16 ": \n", i);
    _dnml_box_line(fail_line, bw);

    // Printing out the input
    freopen(NULL, "w", tmp);
    fputs("    - Input: ", tmp);
    (*s->fmtin_fn)(tmp, in, 1);
    _dnml_box_fmultiline(tmp, bw); putchar('\n');

    // Printing out the Expected Status
    freopen(NULL, "w", tmp);
    fputs("    - Expected: ", tmp);
    _print_str_res(aux_2, tmp, 1);
    _dnml_box_line(tmp, bw); putchar('\n');

    // Printing out the Result Status
    freopen(NULL, "w", tmp);
    fputs("    - Got: ", tmp);
    _print_str_res(out, tmp, 1);
    _dnml_box_line(tmp, bw); putchar('\n');

    _dnml_delay_ms(delay_ms);
}
//* ================= SUITE RUNNER & RENDER FUNCTIONS ================= *//
static inline void _dnml_run_edge(_libdnml_str_suite *s) {
    int enum_i = 0;
    for (uint8_t i = 0; i < s->ecount; ++i) {
        _libdnml_scase *c = &s->edge[i];
        run_ecase(c, s->fn_test, s->ectx.ctx);
        assert(!move_forward(&s->ectx, &c->res));
        if (_comp_str_res(&c->res, &c->exp)) s->ecorrect++;
        else { uint8_t findex = (i + 1) - s->ecorrect;
            s->fail_eres[findex] = c->res;
            s->fail_eexp[findex] = c->exp;
            s->fail_enums[enum_i] = i + 1; ++enum_i;
        }
    }
}
static inline void _dnml_run_rand(_libdnml_str_suite *s, int bw, uint32_t delay_ms) {
    fprintf(logf, "======== %s RNG-CASES FAIL LOG ========", s->suite_name);
    void *in, *aux1; str_res *out, *aux2;
    for (uint16_t i = 0; i < s->rcount; ++i) {
        // Setting Up Input
        uint8_t voidp_in_buf[(*s->fn_insize)()];
        in = voidp_in_buf; (*s->fn_infill)(in, s->rctx);
        rcap_mode incap = _rand_rcap(s->state);
        (*s->gen_case)(in, &s->state, incap, s->rctx);
        
        // Setting up Auxillary 1 (Reconstruction) buffers
        uint8_t voidp_in_buf[(*s->fn_aux1size)()];
        aux1 = voidp_in_buf; (*s->fn_aux1fill)(in, s->rctx);
        // Setting up Evaluation Output buffers
        out = s->rctx->res_buf;
        aux2 = s->rctx->aux2_buf;

        // ---------------- MAIN EXECUTION PART ----------------
        run_case(s, s->fn_test, in, out);
        dnml_status stat = out->status;
        // Hard Barrier ---> STATUS_MODE
        if (stat != STR_SUCCESS || stat != STR_TRUNC_SUCCESS) {
            if (!(*s->fn_stat)(in, out->status, aux2)) {
                __dnml_log_stcase(s, i, logf, in, aux1, aux2, out);
            } else s->rcorrect++;
        }
        // INVERSE MODE
        else if (s->check_mode == INVERSE) { run_inverse(s, s->fn_inv, in, out, aux1);
            if (!(*s->inv_cmp)(in, out, aux1, s->rctx))  {
                __dnml_log_invc(s, i, logf, in, aux1, aux2, out);
            } else s->rcorrect++;
        }
        // EVALUATOR MODE
        // else {} works as well, but this is preferred for explicitcity
        else if (s->check_mode == EVAL) { run_eval(s, s->fn_eval, in, aux2);
            if (!(*s->eval_cmp)(aux2, out)) {
                __dnml_log_evalc(s, i, logf, in, aux1, aux2, out);
            } else s->rcorrect++;
        }
    }
}
static inline void _dnml_run_randp(_libdnml_str_suite *s, int bw, uint32_t delay_ms) {
    /* Example:
        o) Case 5:
            - Input: <...> (whatever the top-layer format is)
            - Output: < -- STR_RES --
                +) Status ...
                +) Data: ...
            >
            - Reconstruction: <...> (whatever the top-layer format is)
    */
    FILE *logf = fopen(s->log_path, "w");
    fprintf(logf, "======== %s RNG-CASES FAIL LOG ========", s->suite_name);
    void *in, *aux1; str_res *out, *aux2;
    for (uint16_t i = 0; i < s->rcount; ++i) {
        // Setting Up Input
        uint8_t voidp_in_buf[(*s->fn_insize)()];
        in = voidp_in_buf; (*s->fn_infill)(in, s->rctx);
        rcap_mode incap = _rand_rcap(s->state);
        (*s->gen_case)(in, &s->state, incap, s->rctx);

        // Setting up Auxillary 1 (Reconstruction) buffers
        uint8_t voidp_in_buf[(*s->fn_aux1size)()];
        aux1 = voidp_in_buf; (*s->fn_aux1fill)(in, s->rctx);
        // Setting up Evaluation Output buffers
        out = s->rctx->res_buf;
        aux2 = s->rctx->aux2_buf;

        // ---------------- MAIN EXECUTION PART ----------------
        run_case(s, s->fn_test, in, out);
        dnml_status stat = out->status;
        // Hard Barrier ---> STATUS_MODE
        if (stat != STR_SUCCESS || stat != STR_TRUNC_SUCCESS) {
            if (!(*s->fn_stat)(in, out->status, aux2)) {
                __dnml_log_stcase(s, i, logf, in, aux1, aux2, out);
                __dnml_print_stcase(s, i, logf, in, aux1, aux2, out, bw, delay_ms);
            } else s->rcorrect++;
        }
        // INVERSE MODE
        else if (s->check_mode == INVERSE) { run_inverse(s, s->fn_inv, in, out, aux1);
            if (!(*s->inv_cmp)(in, out, aux1, s->rctx))  {
                __dnml_log_invc(s, i, logf, in, aux1, aux2, out);
                __dnml_print_invc(s, i, logf, in, aux1, aux2, out, bw, delay_ms);
            } else s->rcorrect++;
        }
        // else {} works as well, but this is preferred for explicitcity
        else if (s->check_mode == EVAL) { run_eval(s, s->fn_eval, in, aux2);
            if (!(*s->eval_cmp)(aux2, out)) {
                __dnml_log_evalc(s, i, logf, in, aux1, aux2, out);
                __dnml_print_evalc(s, i, logf, in, aux1, aux2, out, bw, delay_ms);
            } else s->rcorrect++;
        }
    }
}
static inline void _dnml_render_csuite(_libdnml_str_suite *s) { // Render a "COMPACT" Suite
    uint8_t fail_edge = s->ecount - s->ecorrect;
    uint8_t fail_rand = s->rcount - s->rcorrect;
    char status = (fail_edge + fail_rand == 0) ? '+' : '-';
    printf("  [%c] %-20s     %2" PRIu8 "/%-2" PRIu8 " edge   %2" PRIu8 "/%-2" PRIu8 " random",
        status, s->suite_name,
        s->ecorrect, s->ecount,
        s->rcorrect, s->rcount
    ); if (fail_edge + fail_rand > 0) printf("  -> %s_fails.log", s->suite_name);
    putchar('\n'); fflush(stdout);
}
static inline void _dnml_log_esuite(_libdnml_str_suite *s) {
    uint16_t fail_edge = s->ecount - s->ecorrect;
    uint16_t fail_rand = s->rcorrect - s->rcorrect;

    if ((fail_edge + fail_rand) == 0 || !s->log_path) return;
    FILE *f = fopen(s->log_path, "w"); if (!f) return;
    fprintf(f, "======== %s FAIL LOG ========", s->suite_name);
    //* PRINTS EDGE CASES *//
    for (uint8_t i = 0; i < fail_edge; ++i) {
        /* Example Result:
            o) Edge case 32:
               - Expected: < -- STR-RES --
                    +) Status: STRING_NULL
                    +) Data: "\0";
                >
               - Got:      < -- STR-RES --
                    +) Status: STR_SUCCESS
                    +) Data: "123456789abcdef...123456789abcdef" (Length = 1024)
                >
        */
        fprintf(f,  "o) Edge case %d:\n", s->fail_enums[i]);
        fputs(      "    - Expected: ", f);
        _print_str_res(&s->fail_eexp[i], f, 1); fputc('\n', f);
        fputs(      "    - Got:      ", f);
        _print_str_res(&s->fail_eres[i], f, 1); fputc('\n', f);
    } fclose(f);
}
static inline void _dnml_render_esuite(_libdnml_str_suite *s, uint32_t delay_ms, int bw) {
    _dnml_box_top(s->suite_name, bw); _dnml_delay_ms(delay_ms);
    // ------ edge cases line ------
    char edge_line[bw]; snprintf(
        edge_line, sizeof(edge_line), "Edge case: %d/%d",
        s->ecorrect, s->ecount
    ); _dnml_box_line(edge_line, bw);
    _dnml_delay_ms(delay_ms);

    // print failed edge cases
    /* Example:
            o) Case 5:
                - Expected: < -- STR_RES -- 
                    +) Status: ...
                    +) Data: ...
                >
                - Got: < -- STR_RES --
                    +) Status ...
                    +) Data: ...
                >
    */
    int fail_edge = s->ecount - s->ecorrect;
    char curr_index[10], fail_line[bw]; FILE *tmp = tmpfile(); 
    if (tmp == NULL) { fprintf(stderr, "Failed to open a tmpfile(), Terminating..."); abort();  }
    for (uint8_t i = 0; i < fail_edge; ++i) { 
        memset(fail_line, (char)(1), bw);
        int curri_len = _itosn(i, curr_index, sizeof(curr_index));
        snprintf(fail_line, sizeof(fail_line), "Case %" PRIu16 ": \n", s->fail_enums[i]);
        _dnml_box_line(fail_line, bw);

        freopen(NULL, "w", tmp);
        fputs("    - Expected: ", tmp);
        _print_str_res(&s->fail_eexp[i], tmp, 1);
        _dnml_box_fmultiline(tmp, bw); putchar('\n');

        freopen(NULL, "w", tmp);
        fputs("    - Got: ", tmp);
        _print_str_res(&s->fail_eres[i], tmp, 1);
        _dnml_box_fmultiline(tmp, bw); putchar('\n');

        _dnml_delay_ms(delay_ms);
    } fflush(stdout);
}
//* ========= FULL SUITES/SESSIONS RENDER FUNCTIONS ========== *//
static inline void _dnml_full_suite(_libdnml_str_suite *s, uint32_t delay_ms, int bw) {
    _dnml_run_edge(s); _dnml_log_esuite(s); 
    _dnml_render_esuite(s, delay_ms, bw);
    if (s->ecorrect < s->ecount || s->check_mode == NONE) { 
        _dnml_box_bottom(bw); return; 
    }
    // Running Random Cases
    _dnml_box_divider(bw);
    char rand_line[bw]; snprintf(
        rand_line, sizeof(rand_line),
        "Rand case: %" PRIu16 "/%" PRIu16 "",
        s->rcorrect, s->rcount
    ); _dnml_box_line(rand_line, bw);
    _dnml_delay_ms(delay_ms);
    _dnml_run_randp(s, bw, delay_ms);
    _dnml_box_bottom(bw);
}
static inline void _dnml_compact_suite(_libdnml_str_suite *s, uint32_t delay_ms, int bw) {
    _dnml_run_edge(s); _dnml_log_esuite(s);
    if (s->ecorrect < s->ecount || s->check_mode == NONE) {
        uint8_t fail_edge = s->ecount - s->ecorrect;
        char status = (!fail_edge) ? '+' : '-';
        printf("  [%c] %-20s     %2" PRIu8 "/%-2" PRIu8 " edge",
            status, s->suite_name,
            s->ecorrect, s->ecount,
            s->rcorrect, s->rcount
        ); if (fail_edge) printf("  -> %s_fails.log", s->suite_name);
        putchar('\n'); fflush(stdout); return;
    }
    // Random Case Proceedings
    _dnml_run_rand(s, bw, delay_ms);
    _dnml_render_csuite(s);



}
static inline void start_str_session(const _libdnml_session *session) {
    int bw = session->box_width;
    _libdnml_str_suite **session_suites = (_libdnml_str_suite**)(session->suites);
    //* ---- COMPACT MODE ---- *//
    if (session->output_mode >= DNML_COUT) {
        printf("\n  -- %s ", session->session_name);
        int pad = 44 - (int)(strlen)(session->session_name);
        for (int i = 0; i < (pad - (pad & 1)) >> 1; ++i) fputs("--", stdout);
        putchar('\n');
        for (uint8_t i = 0; i < session->suite_count; ++i){
            _dnml_compact_suite(session_suites[i], session->cli_delay, session->box_width);
        } return;
    } 
    //* ---- VERBSOE/FULL MODE ---- *//
    // Loading animation before session starts
    _dnml_loading("Loading session...", session->cli_delay, 12);
    for (uint8_t i = 0; i < session->suite_count; ++i) {
        // Update progress bar before each suite
        _dnml_session_progress(i, session->suite_count, session->session_name);
        // loading animation between suites
        if (i > 0) _dnml_loading("Running suite...", session->cli_delay, 8);
        _dnml_full_suite(session_suites[i], session->cli_delay, session->box_width);
    }
    // Final progress bar at 100%
    _dnml_session_progress(session->suite_count, session->suite_count, session->session_name);
}

#endif