#ifndef ___LIBDNML_TEST_BASE
#define ___LIBDNML_TEST_BASE

#include <include.h>
#include <system/sys.h>
#include "../adynamol/big_numbers/bigNums.h"
#include "../sconfigs/dnml_status.h"
#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#elif defined(__unix__) || defined(__APPLE__)
    #include <sys/ioctl.h>
        #include <unistd.h>
#endif

typedef enum { DNML_CALL, DNML_OCALL} _dnml_call_style;
typedef enum { DNML_VOUT = 4, DNML_COUT } _dnml_output_mode;
typedef enum { LOW_SUITE, IO_SUITE, BIGSUITE } suite_type;
typedef struct _libdnml_session {
    const char *session_name;
    uint8_t suite_count;
    void* *suites;
    _dnml_output_mode output_mode;
    uint32_t cli_delay; // in ms
    int box_width;
} _libdnml_session;

// Type-specific Limitations - INPUT
#define BIGINT_CAP 48
#define STR_CAP 518
// Type-specific Limitations - PRINTING
#define STR_PREVIEW 64
#define BIGINT_PREVIEW 4
// Type-specific Limitations - Output
#define STR_OUT_CAP 3078
#define STR_MIN_BASE 2
#define STR_MAX_BASE 64


//* =================== INTERFACE/UI TEXT =================== *//
#define TAB         "    "
#define BOX_TL      "┌"
#define BOX_TR      "┐"
#define BOX_BL      "└"
#define BOX_BR      "┘"
#define BOX_H       "─"
#define BOX_V       "│"
#define BOX_DIV_L   "├"
#define BOX_DIV_R   "┤"
#define BOX_WIDTH   80
//* ============== SUITE BOX FUNCTIONS ============== *//
static inline void _dnml_box_divider(int bw) {
    puts(BOX_DIV_L);
    for (int i = 0; i < bw; i++) puts(BOX_H);
    puts(BOX_DIV_R "\n");
}
static inline void _dnml_box_top(const char* suite_name, int bw) {
    printf(BOX_TL " %s ", suite_name); 
    size_t namelen = strlen(suite_name);
    for (int i = 0; i < bw - (int)namelen - 2; i++) puts(BOX_H);
    puts(BOX_TR "\n");
}
static inline void _dnml_box_bottom(int bw) {
    puts(BOX_BL);
    for (int i = 0; i < bw; i++) puts(BOX_H);
    puts(BOX_BR "\n");
}
static inline void _dnml_box_line(const char *text, int bw) {
    int len = (int)strlen(text);
    int pad = bw - len;
    if (pad < 0) pad = 0;
    printf(BOX_V " %.*s%*s" BOX_V "\n", bw - 1, text, pad - 1, "");
}
static inline void _dnml_box_multiline(const char *text, int bw) {
    char line[bw];
    while (*text != '\0') {
        const char *endl_char = strchr(text, '\n');
        size_t len = (endl_char) ? (size_t)(endl_char - text) : strlen(text);
        if (len >= bw) len = bw - 1;

        strncpy(line, text, len);
        line[len] = '\0';
        _dnml_box_line(line, bw);
        if (endl_char) text = endl_char + 1;
        else break;
    }
}
static inline void _dnml_box_fmultiline(FILE *f, int bw) {
    rewind(f);
    char line[BOX_WIDTH];
    while (fgets(line, sizeof(line), f)) {
        size_t len = strlen(line);
        if (len && line[len - 1] == '\n') line[len - 1] = '\0';
        _dnml_box_line(line, bw);
    }
}
//* ============== SESSION PROGRESS/FEATURES FUNCTIONS ============== *//
static inline void _dnml_delay_ms(uint32_t ms) {
    struct timespec ts = { 
        .tv_sec = ms / 1000,
        .tv_nsec = (ms % 1000) * 1000000L 
    }; nanosleep(&ts, NULL);
}
static inline void _dnml_loading(const char *label, uint32_t delay, uint32_t ticks) {
    const char *frames[] = { "|", "/", "-", "\\" };
    for (uint32_t i = 0; i < ticks; i++) {
        printf("\r  %s  %s ", frames[i % 4], label);
        fflush(stdout);
        _dnml_delay_ms(delay);
    }
    printf("\r%*s\r", BOX_WIDTH + 4, "");   // clear line
    fflush(stdout);
}
static inline void _dnml_session_progress(uint8_t done, uint8_t total, const char *session_name) {
    int barw = 40;
    int filled = (total) ? (uint8_t)(done * barw / total) : 0;

    printf("  %s\n  Session progression: [", session_name);
    for (int i = 0; i < barw; i++)
        printf((i < filled) ? "#" : " ");
    printf("] %" PRIu8 "\n\n", (total) ? (uint8_t)(done * 100 / total) : 0);
    fflush(stdout);
}
static inline int _dnml_twidth(void) {
    #if defined(_WIN32) || defined(_WIN64)
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
            return (int)(csbi.srWindow.Right - csbi.srWindow.Left + 1);
        } return 80;
    #elif defined(__unix__) || defined(__APPLE__)
        struct winsize w;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 
        && w.ws_col > 0) return (int)w.ws_col;
        return 80;
    #else
        return 80;
    #endif
}
static inline int _dnml_box_width(void) {
    int tw = _dnml_twidth();
    if (tw < 60) tw = 60;
    if (tw > 120) tw = 120;
    return tw - 4;
}



#endif