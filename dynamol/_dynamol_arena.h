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



#ifndef dynamol_arena_h
#define dynamol_arena_h


#include <dnml_status.h>
#include <debug_util.h>
#include <dnml_sys/sys.h>
#include <_libdnml_mem/arena.h>

/* Arena & Functions Declarations */
extern local_thread dnml_arena ___DASI_NUMERIC_ARENA_;
extern local_thread dnml_arena ___DASI_IO_ARENA_;
dnml_arena* _USE_ARENA(void);
dnml_arena* _USE_IO_ARENA(void);
dnml_status _init_dynamol_bigint(void);
void _cleanup_dynamol(void);


#endif
