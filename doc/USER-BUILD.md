# Building and Installing `lib-dnml`

This document outlines the step-by-step instructions to compile `lib-dnml` into a standalone, architecture-optimized static library archive (`.a` / `.lib`) alongside its public include headers.

---
## Method 1: Standard Compilation with CMake (Recommended)

Using CMake is the highly recommended approach. It automatically handles localized host architecture identification, optimized compiler-specific optimization flags, and processes the correct platform assembly (`.S`) target sequences.

### 1. Compile the Static Library
Execute the following commands from the library's root repository directory:
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j 4
```

### 2. Export and Install to a Staging Location
To place the compiled static binary and unified headers into a clean, portable distribution folder (e.g., a directory named `dist`):

```bash
cmake --install build --prefix ./dist
```

This isolates the output assets into a standard package architecture layout:

* `./dist/lib/` → Contains `libdnml_core.a` (or `dnml_core.lib` on Windows MSVC)
* `./dist/include/` → Contains `libdnml.h` and all dependent calculation sub-headers.
* `./dist/lib/cmake/dnml_core/` → Contains the tracking target configurations for downstream integration.

### 3. Linking into a Downstream CMake Target
To use this compiled static library inside an external software project, add these lines to your consumer project's `CMakeLists.txt`:

```cmake
find_package(dnml_core REQUIRED CONFIG)
add_executable(my_application main.c)
target_link_libraries(my_application PRIVATE dnml::dnml_core)
```

Point your downstream build system to your localized installation tree folder during layout configuration:

```bash
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/dist
cmake --build build
```



---
## Method 2: Direct Compiler Terminal Invocations (Legacy Fallback)
If you are working inside restricted legacy systems or specialized environments that lack a functional installation of CMake, Make, or Ninja, you can compile the source code directly using a single command pipeline.

Select the precise terminal command block that corresponds to your specific host processor architecture and compilation toolchain.

### Option A: GCC or Clang (Linux & macOS Intel x86_64)

This terminal command compiles all core operational calculation modules alongside the System-V ABI x86_64 vector assembly paths into local target objects, then wraps them inside a static library archive:

```bash
# Step 1: Compile all sources and assembly sequences to individual object targets
gcc -O3 -c \
    -I./include -I./include/dnml_sys -I./include/dnml_sys/asm -I./include/_libdnml_config -I./include/_libdnml_mem -I./include/_libdnml_perf \
    -I./intrinsics -I./intrinsics/x86_64 -I./intrinsics/zvanillc -I./util -I./calc_algo -I./calc_algo/perf_based -I./calc_algo/crt_based -I./dynamol/bigint -I./dynamol/bigfloat -I./drypto/crypt_int -I./drypto/crypt_float -I./test_ui \
    include/dnml_sys/__hwcaps.c include/dnml_sys/asm/_dnml_cpuid.S include/char_table.c \
    intrinsics/intrin_base.c intrinsics/intrinsics.c intrinsics/crt_intrin.c \
    intrinsics/x86_64/_x86_sysv_arith.S intrinsics/x86_64/_x86_sysv_bitops.S intrinsics/x86_64/_x86_sysv_crt_alg.S intrinsics/x86_64/_x86_sysv_crt_equal.S intrinsics/x86_64/_x86_sysv_crt_i64cmp.S intrinsics/x86_64/_x86_sysv_crt_u64cmp.S intrinsics/x86_64/_x86_sysv_marith.S intrinsics/x86_64/_x86_sysv_sec.S \
    intrinsics/zvanillc/_vanillc_arith.c intrinsics/zvanillc/_vanillc_bitops.c intrinsics/zvanillc/_vanillc_marith.c intrinsics/zvanillc/_vanillc_misc.c \
    intrinsics/crt_vanillc/_crt_vanillc_alg.c intrinsics/crt_vanillc/_crt_vanillc_arith.c intrinsics/crt_vanillc/_crt_vanillc_bitops.c intrinsics/crt_vanillc/_crt_vanillc_cmp.c \
    util/perf_util/bigNum_utils.c util/perf_util/misc_utils.c util/perf_util/str_parse.c util/perf_util/rng_util.c \
    util/crt_util/crt_bnum_util.c util/crt_util/crt_misc_util.c calc_algo/algo_base/add_sub.c \
    calc_algo/perf_based/mul.c calc_algo/perf_based/div.c calc_algo/perf_based/mod.c calc_algo/perf_based/mod_op.c calc_algo/perf_based/num_theory.c calc_algo/perf_based/pow_root.c \
    calc_algo/crt_based/crt_mul.c calc_algo/crt_based/crt_div.c calc_algo/crt_based/crt_mod.c calc_algo/crt_based/crt_modop.c calc_algo/crt_based/crt_num_theory.c \
    dynamol/bigint/bigInt.c dynamol/bigint/bigInt_io.c drypto/crypt_int/crint_core.c drypto/crypt_int/crint_arith.c drypto/crypt_int/crint_bitops.c drypto/crypt_int/crint_num_theory.c

# Step 2: Archive all generated target object modules into a static library binary
ar rcs libdnml_core.a *.o

```

### Option B: Apple Clang / LLVM Clang (macOS Apple Silicon ARM64)

This terminal command handles compilation sequences targeting ARM64 processors, compiling all source engines and ARM64-specific computational assembly modules before packaging them together:

```bash
# Step 1: Compile ARM64 hardware assembly and source blocks to objects
clang -O3 -c \
    -I./include -I./include/dnml_sys -I./include/dnml_sys/asm -I./include/_libdnml_config -I./include/_libdnml_mem -I./include/_libdnml_perf \
    -I./intrinsics -I./intrinsics/arm64 -I./intrinsics/zvanillc -I./util -I./calc_algo -I./calc_algo/perf_based -I./calc_algo/crt_based -I./dynamol/bigint -I./dynamol/bigfloat -I./drypto/crypt_int -I./drypto/crypt_float -I./test_ui \
    include/dnml_sys/__hwcaps.c include/dnml_sys/asm/_dnml_cpuid.S include/char_table.c \
    intrinsics/intrin_base.c intrinsics/intrinsics.c intrinsics/crt_intrin.c \
    intrinsics/arm64/_arm64_arith.S intrinsics/arm64/_arm64_bitops.S intrinsics/arm64/_arm64_crt_alg.S intrinsics/arm64/_arm64_crt_equal.S intrinsics/arm64/_arm64_crt_i64cmp.S intrinsics/arm64/_arm64_crt_u64cmp.S intrinsics/arm64/_arm64_marith.S intrinsics/arm64/_arm64_sec.S \
    intrinsics/zvanillc/_vanillc_arith.c intrinsics/zvanillc/_vanillc_bitops.c intrinsics/zvanillc/_vanillc_marith.c intrinsics/zvanillc/_vanillc_misc.c \
    intrinsics/crt_vanillc/_crt_vanillc_alg.c intrinsics/crt_vanillc/_crt_vanillc_arith.c intrinsics/crt_vanillc/_crt_vanillc_bitops.c intrinsics/crt_vanillc/_crt_vanillc_cmp.c \
    util/perf_util/bigNum_utils.c util/perf_util/misc_utils.c util/perf_util/str_parse.c util/perf_util/rng_util.c \
    util/crt_util/crt_bnum_util.c util/crt_util/crt_misc_util.c calc_algo/algo_base/add_sub.c \
    calc_algo/perf_based/mul.c calc_algo/perf_based/div.c calc_algo/perf_based/mod.c calc_algo/perf_based/mod_op.c calc_algo/perf_based/num_theory.c calc_algo/perf_based/pow_root.c \
    calc_algo/crt_based/crt_mul.c calc_algo/crt_based/crt_div.c calc_algo/crt_based/crt_mod.c calc_algo/crt_based/crt_modop.c calc_algo/crt_based/crt_num_theory.c \
    dynamol/bigint/bigInt.c dynamol/bigint/bigInt_io.c drypto/crypt_int/crint_core.c drypto/crypt_int/crint_arith.c drypto/crypt_int/crint_bitops.c drypto/crypt_int/crint_num_theory.c

# Step 2: Build the static archive binary
ar rcs libdnml_core.a *.o

```

### Option C: Microsoft Visual C++ Compiler (Windows MSVC x86_64)

From an open instance of the **Developer Command Prompt for Visual Studio**, run these compilation strings to process using the Windows x64 ABI calling convention assembly targets:

```cmd
:: Step 1: Compile all source paths and specialized Windows x64 assembly files
cl /O2 /c /D__ABI_X64_WIN64__ ^
    /I.\include /I.\include\dnml_sys /I.\include\dnml_sys\asm /I.\include\_libdnml_config /I.\include\_libdnml_mem /I.\include\_libdnml_perf ^
    /I.\intrinsics /I.\intrinsics\x86_64 /I.\intrinsics\zvanillc /I.\util /I.\calc_algo /I.\calc_algo\perf_based /I.\calc_algo\crt_based /I.\dynamol\bigint /I.\dynamol\bigfloat /I.\drypto\crypt_int /I.\drypto\crypt_float /I.\test_ui ^
    include\dnml_sys\__hwcaps.c include\dnml_sys\asm\_dnml_cpuid.S include\char_table.c ^
    intrinsics\intrin_base.c intrinsics\intrinsics.c intrinsics\crt_intrin.c ^
    intrinsics\x86_64\_x86_win64_arith.S intrinsics\x86_64\_x86_win64_bitops.S intrinsics\x86_64\_x86_win64_crt_alg.S intrinsics\x86_64\_x86_win64_crt_equal.S intrinsics\x86_64\_x86_win64_crt_i64cmp.S intrinsics\x86_64\_x86_win64_crt_u64cmp.S intrinsics\x86_64\_x86_win64_marith.S intrinsics\x86_64\_x86_win64_sec.S ^
    intrinsics\zvanillc\_vanillc_arith.c intrinsics\zvanillc\_vanillc_bitops.c intrinsics\zvanillc\_vanillc_marith.c intrinsics\zvanillc\_vanillc_misc.c ^
    intrinsics\crt_vanillc\_crt_vanillc_alg.c intrinsics\crt_vanillc\_crt_vanillc_arith.c intrinsics\crt_vanillc\_crt_vanillc_bitops.c intrinsics\crt_vanillc\_crt_vanillc_cmp.c ^
    util\perf_util\bigNum_utils.c util\perf_util\misc_utils.c util\perf_util\str_parse.c util\perf_util\rng_util.c ^
    util\crt_util\crt_bnum_util.c util\crt_util\crt_misc_util.c calc_algo\algo_base\add_sub.c ^
    calc_algo\perf_based\mul.c calc_algo\perf_based\div.c calc_algo\perf_based\mod.c calc_algo\perf_based\mod_op.c calc_algo\perf_based\num_theory.c calc_algo\perf_based\pow_root.c ^
    calc_algo\crt_based\crt_mul.c calc_algo\crt_based\crt_div.c calc_algo\crt_based\crt_mod.c calc_algo\crt_based\crt_modop.c calc_algo\crt_based\crt_num_theory.c ^
    dynamol/bigint/bigInt.c dynamol/bigint/bigInt_io.c drypto/crypt_int/crint_core.c drypto/crypt_int/crint_arith.c drypto/crypt_int/crint_bitops.c drypto/crypt_int/crint_num_theory.c

:: Step 2: Merge the generated target object structures into a Windows Static Library
lib /OUT:dnml_core.lib *.obj

```

### Consuming Standalone Built Binaries

Once a direct manual compilation is completed, copy the generated static binary archive file (`libdnml_core.a` or `dnml_core.lib`) along with the parent interface header `include/libdnml.h` and its sub-headers straight into your downstream consumer development directory space. Link them manually during your application's compile step (e.g., passing toolchain reference flags like `-L. -ldnml_core`).