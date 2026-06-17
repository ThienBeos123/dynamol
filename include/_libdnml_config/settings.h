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



#ifndef ___DNML_SETTINGS_H___
#define ___DNML_SETTINGS_H___

#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _DNML_DEBUG_MODE
  /* Default debug mode value; can be overridden at compile time with -D_DNML_DEBUG_MODE=1 or 0 */
  #define _DNML_DEBUG_MODE 0
#endif

#if _DNML_DEBUG_MODE
  #undef NDEBUG
#endif


#ifdef __cplusplus
}
#endif


#endif
