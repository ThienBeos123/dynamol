#ifndef ___LIBDNML_LTEST_UI
#define ___LIBDNML_LTEST_UI


#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <time.h>

#include "_test_base.h"

//* =========== TYPE DEFINITIONS =========== *//
// Helper Types
typedef struct { uint64_t first; uint64_t second; } _dnml_pair;

typedef struct _libdnml_case {
    _dnml_pair exp;
    uint8_t inc;
    uint64_t *in;
} _libdnml_case;

typedef struct _libdnml_lsuite {
    const char *suite_name;
    void *fn_test;  void *fn_ref;
    _dnml_call_style call_style;
    const char *log_path;

    _libdnml_case *edge_cases;
    uint8_t edge_cases_count;   uint8_t edge_cases_correct;
    _dnml_pair *fail_edge_res;  _dnml_pair *fail_edge_exp;

    _libdnml_case *rand_cases;
    uint16_t rand_cases_count;  uint16_t rand_cases_correct;    uint8_t rand_nin;
    uint64_t **fail_rand_in;    _dnml_pair *fail_rand_res;      _dnml_pair *fail_rand_exp;

    int fail_enums[];
} _libdnml_lsuite;



//* =================== TEST CREATION FUNFCTIONS =================== *//
static inline size_t resbuf_size(uint8_t ecount, uint16_t rcount) { return (ecount + rcount) << 1; }
static inline void create_lsuite(
    _libdnml_lsuite *curr_suite, const char *suite_name,
    uint8_t edge_count, uint16_t rand_count, uint8_t rand_nin, 
    _libdnml_case *edge_bank, _libdnml_case *rand_bank,
    uint64_t **fail_randin, _dnml_pair *res_storage, 
    const char *log_path
) {
    curr_suite->suite_name = suite_name;
    curr_suite->edge_cases_count = edge_count;
    curr_suite->rand_cases_count = rand_count;
    curr_suite->rand_nin = rand_nin;
    curr_suite->log_path = log_path;
    // Filling in the banks
    curr_suite->edge_cases = edge_bank;
    curr_suite->rand_cases = rand_bank;
    // Result storage requires at least 2ecount + 4rcount
    curr_suite->fail_edge_res = &res_storage[0];
    curr_suite->fail_edge_exp = &res_storage[edge_count];
    curr_suite->fail_rand_in  = fail_randin;
    curr_suite->fail_rand_exp = &res_storage[2 * edge_count];
    curr_suite->fail_rand_res = &res_storage[2 * edge_count + rand_count];
}
static inline void create_lsession(
    _libdnml_session *curr_session,
    const char *session_name, uint8_t cli_delay,
    uint8_t suite_count, _libdnml_lsuite *suite_list,
    _dnml_output_mode output_mode
) {
    curr_session->session_name = session_name;
    curr_session->cli_delay = cli_delay;
    curr_session->suite_count = suite_count;
    curr_session->suites = suite_list;
    curr_session->output_mode = output_mode;
    curr_session->box_width = _dnml_box_width();
}



//* =================== FUNCTION-GENERALIZATION MACROS =================== *//
static inline int _comp_pair(const _dnml_pair a, const _dnml_pair b) {
    return (a.first == b.first && a.second == b.second);
}
typedef uint64_t (*_fn1_t)(uint64_t);
typedef uint64_t (*_fn2_t)(uint64_t, uint64_t);
typedef uint64_t (*_fn3_t)(uint64_t, uint64_t, uint64_t);
typedef uint64_t (*_fn4_t)(uint64_t, uint64_t, uint64_t, uint64_t);
typedef uint64_t (*_fn1o_t)(uint64_t, uint64_t*);
typedef uint64_t (*_fn2o_t)(uint64_t, uint64_t, uint64_t*);
typedef uint64_t (*_fn3o_t)(uint64_t, uint64_t, uint64_t, uint64_t*);
#define DNML_FCALL_(fn_ptr, case_ptr, out) \
    do { \
        switch ((case_ptr)->inc) { \
            case 1: (out).first = ((_fn1_t)(fn_ptr))((case_ptr)->in[0]); break;    \
            case 2: (out).first = ((_fn2_t)(fn_ptr))(  \
                (case_ptr)->in[0],                  \
                (case_ptr)->in[1]                   \
            ); break; \
            case 3: (out).first = ((_fn3_t)(fn_ptr))(  \
                (case_ptr)->in[0],                  \
                (case_ptr)->in[1],                  \
                (case_ptr)->in[2]                   \
            ); break; \
            case 4: (out).first = ((_fn4_t)(fn_ptr))(  \
                (case_ptr)->in[0],                  \
                (case_ptr)->in[1],                  \
                (case_ptr)->in[2],                  \
                (case_ptr)->in[3]                   \
            ); break; \
        } \
    } while(0)
#define DNML_OFCALL_(fn_ptr, case_ptr, out) \
    do { \
        switch ((case_ptr)->inc) { \
            case 1: { \
                (out).first = ((_fn1o_t)(fn_ptr))(                  \
                (case_ptr)->in[0],                                  \
               &(case_ptr)->in[1]                                   \
                ); (out).second = (case_ptr)->in[1]; break; }       \
            case 2: { \
                (out).first = ((_fn2o_t)(fn_ptr))(                  \
                (case_ptr)->in[0],                                  \
                (case_ptr)->in[1],                                  \
               &(case_ptr)->in[2]                                   \
            ); (out).second = (case_ptr)->in[2]; break; }           \
            case 3: { \
                (out).first = ((_fn3o_t)(fn_ptr))(                  \
                (case_ptr)->in[0],                                  \
                (case_ptr)->in[1],                                  \
                (case_ptr)->in[2],                                  \
               &(case_ptr)->in[3]                                   \
            ); (out).second = (case_ptr)->in[3]; break; } \
        } \
    } while(0)



//* ============== FULL SUITES/SESSIONS RENDER FUNCTIONS ============== *//
static inline void _dnml_run_suite(_libdnml_lsuite *s) {
    //* ======== 1. EDGE CASE TESTING ======== *//
    int enum_i = 0;
    for (uint8_t i = 0; i < s->edge_cases_count; ++i) {
        _dnml_pair got = {0}, exp = s->edge_cases->exp;
        if (s->call_style == DNML_CALL) DNML_FCALL_(s->fn_test, &s->edge_cases[i], got);
        else DNML_OFCALL_(s->fn_test,&s->edge_cases[i], got);
        if (_comp_pair(got, exp)) s->edge_cases_correct += 1;
        else {
            uint8_t findex = (i + 1) - s->edge_cases_correct;
            s->fail_edge_res[findex] = got;
            s->fail_edge_exp[findex] = exp;
            s->fail_enums[enum_i] = i + 1; ++enum_i;
        }
    }

    //* ======== 2. RAND CASE TESTING ======== *//
    for (uint16_t i = 0; i < s->rand_cases_count; ++i) {
        _dnml_pair got = {0}, exp = {0};
        if (s->call_style == DNML_CALL) {
            DNML_FCALL_(s->fn_test, &s->rand_cases[i], got);
            DNML_FCALL_(s->fn_ref,  &s->rand_cases[i], exp);
        } else {
            DNML_OFCALL_(s->fn_test, &s->rand_cases[i], got);
            DNML_OFCALL_(s->fn_ref,  &s->rand_cases[i], exp);
        } if (_comp_pair(got, exp)) s->rand_cases_correct += 1;
        else {
            uint16_t findex = (i + 1) - s->rand_cases_correct;
            s->fail_rand_res[findex] = got;
            s->fail_rand_exp[findex] = exp;
        }
    }
}
static inline void _dnml_log_suite(_libdnml_lsuite *s) {
    uint16_t fail_edge = s->edge_cases_count - s->edge_cases_correct;
    uint16_t fail_rand = s->rand_cases_count - s->rand_cases_correct;

    if ((fail_edge + fail_rand) == 0 || !s->log_path) return;
    FILE *f = fopen(s->log_path, "w"); if (!f) return;
    fprintf(f, "======== %s FAIL LOG ========", s->suite_name);
    //* PRINTS EDGE CASES *//
    for (uint8_t i = 0; i < fail_edge; ++i) {
        fprintf(f, "o) Edge case %d: \n", s->fail_enums[i]);
        for (uint8_t j = 0; j < s->edge_cases[i].inc; ++j) {
            fprintf(f, "     in[%" PRIu8 "]: 0x%016" PRIx64 "\n", j, s->edge_cases[i].in[j]);
        } fprintf(f, "     expected: <0x%016" PRIx64 ", 0x%016" PRIx64 ">\n", 
            s->fail_edge_exp[i].first, s->fail_edge_exp[i].second);
        fprintf(f, "     got:      <0x%016" PRIx64 ", 0x%016" PRIx64 ">\n", 
            s->fail_edge_res[i].first, s->fail_edge_res[i].second);
    }

    //* PRINTS RANDOM CASES *//
    for (uint16_t i = 0; i < fail_rand; ++i) {
        fprintf(f, "o) Rand case %" PRIu32 ":\n", i + 1);
        for (uint8_t j = 0; j < s->rand_nin; ++j) {
            fprintf(f, "     in[%" PRIu8 "]: 0x%016" PRIx64 "\n", j, s->fail_rand_in[i][j]);
        } fprintf(f, "     expected: <0x%016" PRIx64 ", 0x%016" PRIx64 ">\n", 
            s->fail_rand_exp[i].first, s->fail_rand_exp[i].second);
        fprintf(f, "     got: <0x%016" PRIx64 ", 0x%016" PRIx64 ">\n", 
            s->fail_rand_res[i].first, s->fail_rand_res[i].second);
    } fclose(f);
}
static inline void _dnml_render_csuite(_libdnml_lsuite *s) { // Render a "COMPACT" Suite
    uint8_t fail_edge = s->edge_cases_count - s->edge_cases_correct;
    uint8_t fail_rand = s->rand_cases_count - s->rand_cases_correct;
    char status = (fail_edge + fail_rand == 0) ? '+' : '-';
    printf("  [%c] %-20s     %2" PRIu8 "/%-2" PRIu8 " edge   %2" PRIu8 "/%-2" PRIu8 " random",
        status, s->suite_name,
        s->edge_cases_correct, s->edge_cases_count,
        s->rand_cases_correct, s->rand_cases_count
    ); if (fail_edge + fail_rand > 0) printf("  -> %s_fails.log", s->suite_name);
    putchar('\n'); fflush(stdout);
}
static inline void _dnml_render_esuite(_libdnml_lsuite *s, uint8_t suite_num, uint32_t delay_ms, int bw) {
    _dnml_box_top(s->suite_name, bw); _dnml_box_divider(bw); _dnml_delay_ms(delay_ms);
    // ------ edge cases line ------
    char edge_line[BOX_WIDTH]; snprintf(
        edge_line, sizeof(edge_line), "Edge case: %d/%d",
        s->edge_cases_correct, s->edge_cases_count
    ); _dnml_box_line(edge_line, bw);
    _dnml_delay_ms(delay_ms);

    // print failed edge cases
    int fail_edge = s->edge_cases_count - s->edge_cases_correct;
    char curr_index[10], fail_line[BOX_WIDTH];
    for (int i = 0; i < fail_edge; ++i) { fail_line[BOX_WIDTH];
        int ilen = (s->fail_enums[i], curr_index, sizeof(curr_index));
        snprintf(fail_line, sizeof(fail_line),
            "o) Case %.*s: Expected: <0x%016" PRIx64 ", 0x%016" PRIx64 "> | Got: <0x%016" PRIx64 ", 0x%016 " PRIx64 ">",
            ilen, curr_index,
            s->fail_edge_exp[i].first,
            s->fail_edge_exp[i].second,
            s->fail_edge_res[i].first,
            s->fail_edge_res[i].second
        ); _dnml_box_line(fail_line, bw);
        _dnml_delay_ms(delay_ms);
    } fflush(stdout);
}
static inline void _dnml_render_rsuite(_libdnml_lsuite *s, uint8_t suite_num, uint32_t delay_ms, int bw) {
    _dnml_box_divider(bw);
    // ------ random cases line ------
    char rand_line[BOX_WIDTH];
    snprintf(rand_line, sizeof(rand_line), "Random case: %d/%d",
             s->rand_cases_correct, s->rand_cases_count);
    _dnml_box_line(rand_line, bw);
    _dnml_delay_ms(delay_ms);

    // print failed random cases
    int fail_rand = s->rand_cases_count - s->rand_cases_correct;
    char curr_index[10], fail_line[BOX_WIDTH];
    for (int i = 0; i < fail_rand; i++) {
        int ilen = _itosn(i, curr_index, sizeof(curr_index));
        snprintf(
            fail_line, sizeof(fail_line),
            "o Case %.*s: Expected: <0x%016" PRIx64 ", 0x%016" PRIx64 "> | Got: <0x%016" PRIx64 ", 0x%016" PRIx64 ">",
            ilen, curr_index,
            s->fail_rand_exp[i].first,
            s->fail_rand_exp[i].second,
            s->fail_rand_res[i].first,
            s->fail_rand_res[i].second
        ); _dnml_box_line(fail_line, bw);
        _dnml_delay_ms(delay_ms);
    } _dnml_box_bottom(bw);
    putchar('\n');
    fflush(stdout);
}
static inline void start_low_session(const _libdnml_session *session) {
    int bw = session->box_width;
    _libdnml_lsuite *session_suites = (_libdnml_lsuite*)(session->suites);
    //* ---- COMPACT MODE ---- *//
    if (session->output_mode >= DNML_COUT) {
        printf("\n  -- %s ", session->session_name);
        int pad = 44 - (int)(strlen)(session->session_name);
        for (int i = 0; i < (pad - (pad & 1)) >> 1; ++i) fputs("--", stdout);
        putchar('\n');
        for (uint8_t i = 0; i < session->suite_count; ++i) {
            _dnml_run_suite(&session_suites[i]);
            _dnml_render_csuite(&session_suites[i]);
        } return;
    } 
    //* ---- VERBSOE/FULL MODE ---- *//
    // loading animation before session starts
    _dnml_loading("Loading session...", session->cli_delay, 12);
    for (uint8_t i = 0; i < session->suite_count; ++i) {
        // update progress bar before each suite
        _dnml_session_progress(i, session->suite_count, session->session_name);
        // loading animation between suites
        if (i > 0) _dnml_loading("Running suite...", session->cli_delay, 8);
        _dnml_run_suite(&session_suites[i]);
        _dnml_log_suite(&session_suites[i]);
        // Render the "edge case" & "rand case" part of suite[i]
        _dnml_render_esuite(&session_suites[i], i + 1, session->cli_delay, bw);
        _dnml_render_rsuite(&session_suites[i], i + 1, session->cli_delay, bw);
    }
    // final progress bar at 100%
    _dnml_session_progress(session->suite_count, session->suite_count, session->session_name);
}




#endif