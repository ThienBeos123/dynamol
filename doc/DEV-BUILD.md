# lib-dnml Development Build Guide
This document describes how to build `lib-dnml` for development and debugging, and how to connect the compiled static library to standalone test folders in `/test` like `test/low`.


---
<!-- ======================================================================== -->
## 1. Root development build
From the repository root, create a local build directory and configure the project:
```bash
mkdir build
cd build
cmake ..
```

Then build the library and any default targets:
```bash
cmake --build . -j
```

This compiles the `dnml_core` static library and any tests added by the root project `CMakeLists.txt`, in which dnml_core is installable as an exported package inside `/build/install`




---
<!-- ======================================================================== -->
## 2. Install the package locally for standalone test use
To let `test` test files (eg: `test/low/bitops_test.c`) or other standalone subprojects consume the library using `find_package`, install the package into a local tree under `build/install`:
```bash
cmake --install . --prefix install
```

This creates a self-contained install tree with:
- `build/install/lib` or `build/install/lib64`
- `build/install/include`
- `build/install/lib/cmake/dnml_core`




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

- `test/low/build/..` → `test/low`
- `test/low/../../build/install` → repository-root install tree

That means the standalone testing foler build can find the exported `dnml_core` package without requiring an absolute path, which is non-portable and is a major secuirty risk.




---
<!-- ======================================================================== -->
## 4. If the standalone test build cannot find the package
If your root install tree lives somewhere else, or if you are using a different build layout, pass a portable prefix path explicitly:
```bash
cmake .. -DCMAKE_PREFIX_PATH=../../build/install
```
This is still a relative path and does not depend on your absolute machine directory.




---
<!-- ======================================================================== -->
## 5. Quick workflow summary
- Build root library:
  - `mkdir build && cd build && cmake .. && cmake --build . -j`
- Install locally for package discovery:
  - `cmake --install . --prefix install`
- Build standalone test:
  - `cd path/to/dir && mkdir build && cd build && cmake .. && cmake --build . -j`
