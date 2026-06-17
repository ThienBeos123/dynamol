# Building and Installing `lib-dnml`
This document is the user-facing build guide for `lib-dnml`.
It explains how to build using modern CMake presets, how to map those presets manually on older CMake 3.x engines, and provides legacy fallback alternatives (Makefiles and direct compiler scripting) when modern build tools are entirely absent.

---
## Method 1: CMake Build Pipeline

The project target structure requires a minimum of **CMake 3.1**. 

### 1.1 Modern Path: Using CMake Presets (CMake 3.21+)
If you are running a modern version of CMake, configuration presets are the cleanest entry point. They handle target folders, sanitizer choices, and compilation flags automatically.

To see all available configuration profiles, run
```sh
cmake --list-preset
```

Choose a preset matching your library use-case and target platform:
```sh
# Dynamol Variants (Performance-Optimized)
cmake --preset dynamol-release-unix
cmake --preset dynamol-release-msvc

# Drypto Variants (Hardened Cryptography)
cmake --preset drypto-release-unix
cmake --preset drypto-release-msvc
```

Once configured, compile using the corresponding build preset:
```sh
cmake --build --preset dynamol-release-unix -j
```

---

### 1.2 Legacy Path: Manual Command Line Mapping (CMake 3.1 to 3.20)

If you are on an older system (such as legacy enterprise servers or older stable Linux distributions), CMake will ignore the presets file. You must fall back to a manual build directory allocation and pass the compiler configurations as flags explicitly.

Create a build directory and supply the corresponding flags manually:

#### A. Dynamol Performance Profile (Unix: GCC/Clang)
```sh
mkdir build && cd build
cmake .. -DCMAKE_C_FLAGS="-w -O3 -ffast-math -flto -march=native -mtune=native"
cmake --build . -j
```

#### B. Drypto Cryptographic Profile (Unix: GCC/Clang)
```sh
mkdir build && cd build
cmake .. -DCMAKE_C_FLAGS="-w -O2 -fno-strict-aliasing -fstack-protector-strong -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3 -fPIE"
cmake --build . -j
```

#### C. Dynamol Performance Profile (Windows: MSVC via Developer Prompt)
```cmd
mkdir build && cd build
cmake .. -G "Ninja" -DCMAKE_C_FLAGS="/W0 /O2 /Oi /Ot /fp:fast"
cmake --build . -j
```

#### D. Drypto Cryptographic Profile (Windows: MSVC via Developer Prompt)
```cmd
mkdir build && cd build
cmake .. -G "Ninja" -DCMAKE_C_FLAGS="/W0 /O2 /GS /guard:cf"
cmake --build . -j
```

---

### 1.3 Target Installation & Downstream Consuming

Once compiled via either the modern preset path or manual 3.x configuration, map your artifacts out to a standalone portable distribution tree:
```sh
# Unix Target Tree Export
cmake --install build/unix --prefix ./dist

# Windows MSVC Target Tree Export
cmake --install build/msvc --prefix ./dist
```

The resulting `./dist` directory is completely isolated and safe to move anywhere on your filesystem. To consume this compiled distribution inside a downstream CMake app:
```cmake
find_package(dnml_core REQUIRED CONFIG)
add_executable(my_application main.c)
target_link_libraries(my_application PRIVATE dnml::dnml_core)
```

Point your consumer build step at your isolated directory path:
```sh
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/dist
cmake --build build -j
```

---

## Method 2: Standard Build Automation (Upcoming)

For environments completely lacking a functional CMake setup but maintaining a local build utility engine, standard Makefile pipelines are supported as fallback compilation methods.

### 2.1 GNU Make (Linux, macOS, and BSD systems)

*Development Status: Blueprint Phase.* An upcoming native POSIX `Makefile` will support standalone execution matching our C99 baseline capabilities. It will read local toolchain variables directly:
```sh
make CC=gcc ARCH=x86_64 Variant=Drypto
```

### 2.2 NMake (Windows Command Architecture Fallback)

*Development Status: Blueprint Phase.* For bare Windows environments operating without CMake but carrying access to the C++ Build Tools suite, an MSVC-compatible `Makefile.msc` configuration framework is being established for execution through the native Visual Studio `nmake` command processor.

---

## Method 3: Direct Terminal Pipelines & Scripting (Legacy Fallback)

> **WARNING**: The internal file matrix of `lib-dnml` updates rapidly during pre-release velocity cycles. Manual multi-line compilation strings can quickly become obsolete.

When both CMake and Make infrastructures are missing entirely, compilation can be offloaded to automated automation wrapper scripts or processed as a raw manual stream.

### 3.1 Automated Script Infrastructure (Recommended Legacy Path)

Rather than writing massive multi-line commands by hand, use the root utility shell wrappers. These scripts internally handle platform detection, target architecture splitting, and safely pass the appropriate compiler flag sets:

* **Unix-like environments:** Execute `./legacy-build.sh` (dispatches compiler optimization levels and binds localized assembly sequences).
* **Windows Command Environments:** Execute `legacy-build.bat` via an active Visual Studio Developer Command Prompt instance.

### 3.2 Manual Single-Line Pipeline Reference Base

Below is the raw terminal configuration sequence for a **Unix Intel x86_64 host utilizing GCC/Clang**. You can use this as a structural foundation to customize parameters or diagnose edge-case environment compilation constraints manually:
```sh
# Step 1: Compile source files and positional assembly elements to standalone objects
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

# Step 2: Archive object files to generate your library file
ar rcs libdnml_core.a *.o
```