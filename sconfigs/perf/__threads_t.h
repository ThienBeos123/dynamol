#ifndef __DNML_THREADS_T__
#define __DNML_THREADS_T__


#include <system/sys.h>

//* ------ THREADS INTERFACE DETECTION ------ *//
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_THREADS__)
    #define DNML_THREADS_C11 1
    #define DNML_THREADS_PTHREAD 0
    #define DNML_THREADS_WIN32 0
    #define DNML_THREADS_NONE 0
#elif defined(__unix__) || defined(__APPLE__) || defined(__linux__)
    #define DNML_THREADS_C11 0
    #define DNML_THREADS_PTHREAD 1
    #define DNML_THREADS_WIN32 0
    #define DNML_THREADS_NONE 0
#elif defined(_WIN32) || defined(_WIN64)
    #define DNML_THREADS_C11 0
    #define DNML_THREADS_PTHREAD 0
    #define DNML_THREADS_WIN32 1
    #define DNML_THREADS_NONE 0
#else
    #define DNML_THREADS_C11 0
    #define DNML_THREADS_PTHREAD 0
    #define DNML_THREADS_WIN32 0
    #define DNML_THREADS_NONE 1
#endif



//* ------ THREADS BACKEND INCLUDES ------ *//
#if DNML_THREADS_C11
    #include <threads.h>
    #include <stdatomic.h>
#elif DNML_THREADS_PTHREAD
    #include <pthread.h>
    #include <stdatomic.h>
#elif DNML_THREADS_WIN32
    #include <windows.h>
#endif



//* ------ THREADS TYPE DECLARATION ------ *//
#if DNML_THREADS_C11
    typedef thrd_t              __thread_t;
    typedef mtx_t               __mutex_t;
    typedef _Atomic(uint64_t)   __itomic;
#elif DNML_THREADS_PTHREAD
    typedef pthread_t           __thread_t;
    typedef pthread_mutex_t     __mutex_t;
    typedef _Atomic(uint64_t)   __itomic;
#elif DNML_THREADS_WIN32
    typedef HANDLE              __thread_t;
    typedef CRITICAL_SECTION    __mutex_t;
    typedef volatile LONG       __itomic;  // use InterlockedIncrement
#else
    typedef int         __thread_t;   // dummy — never used
    typedef int         __mutex_t;
    typedef int         __itomic;
#endif

// Threads fucntion signatures
#if DNML_THREADS_C11
    typedef int     (*__thread_fn)(void*);
#else
    typedef void*   (*__thread_fn)(void*);
#endif



//* -------------------- THREADS FUNCTION -------------------- *//
static inline int has_dthread_t(void) { return !DNML_THREADS_NONE; }
static inline int create_dthread_t(__thread_t *t, __thread_fn fn, void *arg) {
    #if DNML_THREADS_C11
        return thrd_create(t, fn, arg) == thrd_success ? 0 : -1;
    #elif DNML_THREADS_PTHREAD
        return pthread_create(t, NULL, fn, arg);
    #elif DNML_THREADS_WIN32
        *t = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)fn, arg, 0, NULL);
        return (*t == NULL) ? -1 : 0;
    #else
        (void)t; (void)fn; (void)arg;
        return -1;
    #endif
}
static inline int join_dthread_t(__thread_t t) {
    #if DNML_THREADS_C11
        return thrd_join(t, NULL) = thrd_success ? 0 : -1;
    #elif DNML_THREADS_PTHREAD
        return pthread_join(t, NULL);
    #elif DNML_THREADS_WIN32
        WaitForSingleObject(t, INFINITE);
        CloseHandle(t); return 0;
    #else
        (void)t; return -1;
    #endif
}
//* -------------------- ATOMICS FUNCTION -------------------- *//
static inline void itomic_store(void *i, int v) {
    __itomic *a = (__itomic*)i;
    #if DNML_THREADS_C11 || DNML_THREADS_PTHREAD
        atomic_store(a, v);
    #elif DNML_THREADS_WIN32
        InterlockedExchange(a, (LONG)v);
    #else
        *a = v;
    #endif
}
static inline int itomic_load(void *i) {
    __itomic *a = (__itomic*)i;
    #if DNML_THREADS_C11 || DNML_THREADS_PTHREAD
        return atomic_load(a);
    #elif DNML_THREADS_WIN32
        return (int)InterlockedCompareExchange(a, 0, 0);
    #else
        return *a;
    #endif
}
static inline void itomic_increment(void *i) {
    __itomic *a = (__itomic*)i;
    #if DNML_THREADS_C11 || DNML_THREADS_PTHREAD
        atomic_fetch_add(a, 1);
    #elif DNML_THREADS_WIN32
        InterlockedIncrement(a);
    #else
        ++(*a);
    #endif
}
static inline void _dnml_sleep(unsigned int ms) {
    #if DNML_THREADS_WIN32
        Sleep(ms);
    #elif DNML_THREADS_C11
        struct timespec ts = { ms/1000, (ms%1000)*1000000L };
        thrd_sleep(&ts, NULL);
    #elif DNML_THREADS_PTHREAD
        struct timespec ts = { ms/1000, (ms%1000)*1000000L };
        nanosleep(&ts, NULL);
    #endif
}


#endif