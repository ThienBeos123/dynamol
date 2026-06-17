# lib-dnml Development Build Guide
This document describes how to build `lib-dnml` for development and debugging, and how to connect the compiled static library to standalone test folders in `/test` like `test/intrin_test`. This guide expects the developer to be on a fairly modern development environment that can leverage the advantages of CMake architecture, since the established testing structure heavily relies on CMake package exportation.


---
<!-- ======================================================================== -->
## 1. Root development build
From the repository root, we first identify our desired compilation/build preset for CMake to build:

- `cmake --list-presets`

Then, after identifying our desired preset, denoted as `chosen_preset`, we then move on to build configurations with CMake cache files and settings CMake.

- `cmake --preset chosen_preset`

This automatically create a `/build` directory (or `/build-crt` or `/build-perf`), containing a singular sub-directory named `/compiler-group`, in which compiler-group refers to either `unix` or `msvc` - i.e. between GCC + Clang and MSVC. Afterwards, we compile the entire library into a static library for later linking:

- `cmake --build --preset chosen_preset -j`

In which -j allows for optimize compile-time, especially on older hardware, by leveraging maximal amount of cores for compilation.




---
<!-- ======================================================================== -->
## 2. Install the package locally for standalone test use
To let `test` test files (eg: `test/intrin_test/bitops_test.c`) or other standalone subprojects consume the library using `find_package`, install the package into a local tree under `build-choice/compiler-group/install`:
```bash
cd build-choice/chosen
cmake --install . --prefix install
```

This creates a self-contained install tree with:
- `build-choice/compiler-group/install/lib` or `build-choice/compiler-group/install/lib64`
- `build-choice/compiler-group/install/include`
- `build-choice/compiler-group/install/lib/cmake/dnml_core`




---
<!-- ======================================================================== -->
## 3. Build a standalone test folder/directory
When testing or doing whatever outside of internal library development, we would firstly create the directory alongside a build directory for our CMake configurations, builds, and compilation to go into.
```bash
cd path/to/your/dir
mkdir build
cd build
cmake ..
```

Then build the test runner:
```bash
cmake --build . -j
```

### Why this works
`path/to/your/dir/CMakeLists.txt` is expected to automatically appends the expected local install path of the exported dnml_core package for your tests/whatever to use. Example:

- `test/intrin_test/build/..` → `test/low`
- `test/intr_test/../../build-choice/compiler-group/install` → repository-root install tree

That means the standalone testing foler build can find the exported `dnml_core` package without requiring an absolute path, which is non-portable and is a major secuirty risk.




---
<!-- ======================================================================== -->
## 4. If the standalone test build cannot find the package
If your root install tree lives somewhere else, or if you are using a different build layout, pass a portable prefix path explicitly:
```bash
cmake .. -DCMAKE_PREFIX_PATH=../../build-choice/compiler-group/install
```
This is still a relative path and does not depend on your absolute machine directory, in which the developer may need to identify the number of backward moves into a parent directory.




---
<!-- ======================================================================== -->
## 5. Quick workflow summary
- Build root library:
  - `cmake --preset preset-name && cmake --build --preset preset-name -j && cd build/unix && cmake --install . --prefix install`
- Install locally for package discovery:
  - `cd build/unix && cmake --install . --prefix install` (where build/unix can be replaced with whatever appropriate repository containing the selected build)
- Build standalone test:
  - `cd path/to/dir && mkdir build && cd build && cmake .. && cmake --build . -j`
