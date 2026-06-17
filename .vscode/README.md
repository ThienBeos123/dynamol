# LSP Configuration
### This document details the configuration of files dictating LSP (Language Server Protocols) behavior for development

---
## 1. IntelliSense LSP
IntelliSense LSP is dictated by the file [`c_cpp_properties.json`](/.vscode/c_cpp_properties.json). This is a sample of the file in lib-dnml v0.3.1:

```json
{
    "configurations": [
        {
            "name": "Mac",
            "includePath": [
                "${workspaceFolder}/include",
                "${workspaceFolder}/include/dnml_sys",
                "${workspaceFolder}/include/dnml_sys/asm",
                "${workspaceFolder}/include/_libdnml_mem",
                "${workspaceFolder}/include/_libdnml_perf",
                "${workspaceFolder}/include/_libdnml_config",

                "${workspaceFolder}/intrinsics",
                "${workspaceFolder}/intrinsics/x86_64",
                "${workspaceFolder}/intrinsics/arm64",
                "${workspaceFolder}/intrinsics/risc-v64",
                "${workspaceFolder}/intrinsics/zvanillc",
                "${workspaceFolder}/intrinsics/crt_vanillc",

                "${workspaceFolder}/util",
                "${workspaceFolder}/calc_algo/algo_base",
                "${workspaceFolder}/calc_algo/perf_based",
                "${workspaceFolder}/calc_algo/crt_based",

                "${workspaceFolder}/dynamol",
                "${workspaceFolder}/dynamol/bigint",
                "${workspaceFolder}/dynamol/bigfloat",
                "${workspaceFolder}/drypto/crypt_int",
                "${workspaceFolder}/drypto/crypt_float"
            ],
            "defines": [],
            "compilerPath": "/opt/homebrew/opt/llvm/bin/clang", 
            "cStandard": "c23",
            "cppStandard": "c++26",
            "intelliSenseMode": "macos-clang-arm64"
        }
    ],
    "version": 4
}
```

Now, specifically, we should focus on the `compilerPath` and `intelliSenseMode` attribute. The `compilerPath` and `intelliSenseMode` is currently of exemplary demonstration, and should be replaced with whatever that is the path to the user's compiler (Clang, GCC, MSVC, etc) and the user's current `os-compiler-arch`. Additionally, for any developers on older systems, they may modify `cStandard` and `cppStandard` to be within their version support, as lib-dnml supports a standard of C99, but the Author utilizes C23 and. C++26 standards for more modern code specifications.

Knowing that, one developer should create there own local "c_cpp_properties.json" from [`c_cpp_properties_template.json`](/.vscode/c_cpp_properties_template.json) with the latest include paths for the LSP. Afterwards, they should fill in the cStandard, cppStandard, compilerPath, intelliSenseMode, and name attributes according to their machine.

> ***IMPORTANT***: As lib-dnml is an ever-changing library, with nearly daily active-development, it is imperical that there is a consistent effort in maintaining and updating the `includePath` list of directories containing `.h` files to maintain the best development experience. However, such things is tricky and largely periphery in the grand scheme of software development, and therefore is expected to be flawed and outdated.


---
## 2. ClangD LSP
ClangD LSP by LLVM is a little bit more tricky to work with, since it relies on compilation instructions, and is overall stricter due to using, fully, a parsing engine. It is dictated by the more general file [`settings.json`](/.vscode/settings.json) as an extension, but also the file [`.clangd`](/.clangd), used mostly for warning suppressions. Here's a snippet from [`settings.json`](/.vscode/settings.json) detailing the configuration of ClangD:

```json
{
    // ... Whatever settings above here ...
    "clangd.arguments": [
        // ClangD Core Configs
        "--compile-commands-dir=${workspaceFolder}/build/dev-unix",
        "--background-index",
        "--clang-tidy",
        // Completion and QOLs
        "--function-arg-placeholders=0",
        "--header-insertion=never",
        "--completion-style=bundled",
    ],
    // ... Whatever settings down here ...
}
```

The key point here is the `"--compile-commands-dir"` attribute. This attribute points to the source of compilation commands ClangD uses for Syntax Highlighting and Code Inspection. Therefore, it is impirical for the user to follow the building and compilation procedure, detailed in [`DEV-BUILD.md`](/doc/DEV-BUILD.md) to generate the necessary include paths and compilation modes (-Wall, -Werror, etc) for ClangD to work. Afterwards, we replace `"--compile-commands-dir=${workspaceFolder}/build/dev-unix"` with `"--compile-commands-dir=${workspaceFolder}/your-build/your-preset"`, with `your-build/your-preset` being whatever build you're working on. However, in cases such compilation preset (eg: dev-unix) might obscurely fail, it might be helpful to follow the following procedure/commands:

```sh
cd path/back/to/root
# 1. Delete the build directory containing incomplete build comfigurations
rm -rf build # It can be
rm -rf build-crt # either of these
rm -rf build-perf # or all of these
# 2. Build and Compile with the absolute default settings with no flags
mkdir build
cd build
cmake ..
cmake --build . -j
cmake --install . --prefix install
```

And then changing `"clangd.arguments"` as below inside [`settings.json`](/.vscode/settings.json):

```json
{
    // ... Whatever settings above here ...
    "clangd.arguments": [
        // ClangD Core Configs
        "--compile-commands-dir=${workspaceFolder}/build",
        "--background-index",
        "--clang-tidy",
        // Completion and QOLs
        "--function-arg-placeholders=0",
        "--header-insertion=never",
        "--completion-style=bundled",
    ],
    // ... Whatever settings down here ...
}
```

This ensures continuation of development and debugging of compilation failures caused by using build presets in [`CMakePresets.json`](/CMakePresets.json), where the build is absolutely stripped of any optimizations (crytographically, performance-wised, debugging-wise, etc).

Knowing this, one developer should fill in the exact build they are working on for ClangD to track into, and add any additional settings when copied into settings.json from [`settings_template.json`](/.vscode/settings_template.json) like function and comments folding, LSP discrepancies handling, etc.