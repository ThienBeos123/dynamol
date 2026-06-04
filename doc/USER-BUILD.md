# User Build Instructions

This document explains how a user can compile `lib-dnml` and link it into their own project.

## 1. Build the library from source

From the repository root:

```bash
mkdir -p build
cd build
cmake ..
cmake --build . -j 4
```

This compiles `libdnml_core.a` and the library's headers.

## 2. Install the library for package discovery

Install the built library and headers into a local prefix so consumer projects can find it:

```bash
cmake --install . --prefix /path/to/install
```

For a local install tree inside the repository, you can use:

```bash
cmake --install . --prefix install
```

## 3. Use `lib-dnml` in your own CMake project

In your project `CMakeLists.txt`:

```cmake
find_package(dnml_core REQUIRED CONFIG)

add_executable(myapp main.c)
target_link_libraries(myapp PRIVATE dnml::dnml_core)
```

Then configure your project with the install path:

```bash
mkdir -p build
cd build
cmake .. -DCMAKE_PREFIX_PATH=/path/to/install
cmake --build . -j 4
```

If the install tree is local and relative to your build, you can also pass a relative path, for example:

```bash
cmake .. -DCMAKE_PREFIX_PATH=../install
```

## 4. Platform notes

### Linux and macOS

- The installed package tree will be under `lib/cmake/dnml_core`.
- Use the same `CMAKE_PREFIX_PATH` pattern on both platforms.
- Example install prefix:
  - `/usr/local`
  - `~/local`
  - `./build/install`

### Windows

- CMake will still install the static library and package metadata correctly.
- Use `-DCMAKE_PREFIX_PATH=C:/path/to/install` when configuring your own project.
- Link against `dnml::dnml_core` the same way.

## 5. Static library behavior

`lib-dnml` installs a static archive and exports a CMake target, so users do not need to add include directories manually.

The `dnml::dnml_core` imported target already carries the correct include paths and link properties.

## 6. Example consumer workflow

```bash
git clone <repo>
cd <repo>
mkdir build && cd build
cmake ..
cmake --build . -j 4
cmake --install . --prefix install
cd ../my-user-project/build
cmake .. -DCMAKE_PREFIX_PATH=../repo/build/install
cmake --build . -j 4
```

This makes `lib-dnml` available as a CMake package and keeps the workflow portable across machines.
