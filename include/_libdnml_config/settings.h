#ifndef ___DNML_SETTINGS_H___
#define ___DNML_SETTINGS_H___

#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

#define _DNML_DEBUG_MODE 1

#if _DNML_DEBUG_MODE
    #undef NDEBUG
#endif


#ifdef __cplusplus
}
#endif


#endif