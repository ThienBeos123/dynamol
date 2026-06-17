# lib-dnml

> [!CAUTION]
> We do **NOT** have a website, The only official place of this project is in this repository

<div align="center">

[![lib-dnml banner](doc/libdnml_banner.svg)](doc/libdnml_banner.svg)

[![][latest-release-shield]][latest-release-link]
[![][downloads-shield]][downloads-link]
<br>
An open-source, high-performance, multi-purpose, arbitrary-precision library in C
</div>


## Table of Contents
- [Overview](#overview)
- [Key Features](#key-features)
- [Platform Compatibility](#platform-compatibiltiy)
- [Architecture Goals](#design--architecture-goals)
- [Example Usage](#example-usage)
- [Build & Installation](#build--installation)
- [Documentation](#documentation)
- [Project Status](#project-status)


## Overview
lib-dnml is precisely a collection of modules serving different purposes, but foundated on a shared backend basis:
- **Dynamol**: Efficient mathematical computation module, providing arbitrary-sized numerical types
- **Drypto**: Cryptographically-secure, constant-time mathematical operations and entropy-generation module
- **dBedded**: Light-weight, maximmally-flexible embedded development module
- **dIOstream**: Generic, powerful I/O handling of lib-dnml types above


## Key Features

### lib-dnml is, precisely, a collection of modules serving different purposes. 

- **Arbitrary-Precision**: Integers (bigInt) & Floating-point numbers (bigFloat) 
- **Cryptographically secure**: Constant-time operations against timed-based attacks
- **Cross-platform Compability**: Supported on nearly all 64 bit platform, (see more in [Platform Compatibily](#platform-compatibiltiy))
- **Integration Friendly**: Seamlessly integrate with other projects while maintaining performance and control


## Architecture Goals
- **Predictable** memory behavior
- Explicit control over **performance-critical** operations
- **Explicit memory-usage** for temporary objects (Crypto and Embedded)


## Platform Compatibiltiy
- **Supported Architectures**:
  - *x86_64* 
  - *AARCH64*
  - *RV64*
- **Supported Platform/OS**:
  - *Windows* (MinGW, MSVC - x64, ARM64)
  - *MacOS* (x64, ARM64)
  - *Linux* (GNU, non-GNU - x64, ARM64, RV64)
  - *Android* and *iOS* (ARM64)
  - *BSD-based* (OpenBSD, FreeBSD - x64, ARM64, RV64)


## Example Usage
~~~c
#include "dynamol.h"
// OR #include <dynamol.h>

int main(void) {
    bigInt a, b, res;

    bigInt_init(&res);
    bigInt_init(&a, "1234989689842");
    bigInt_init(&b, "1923895849860");

    bigInt_add(&a, &b, &res);
    bigInt_print(&res);
    return 0;
}
~~~


## Build & Installation
For the installation of lib-dnml, there are a variety of methods. The user can either download and maintain lib-dnml alongside other dependencies/packages through a package manager like vcpkg, Homebrew, pacman, apt, etc (W.I.P). The user can also install lib-dnml through downloading source code releases on GitHub and Gitlab as a .zip, and compiling natively to maximally optimize for their hardware. Here are some valuable documentation detailing such endeavor:

* [Instruction for developers](/doc/DEV-BUILD.md)
* [Instruction for downloader/users](/doc/USER-BUILD.md)

For convenience or potentially issues relating to the inability to build lib-dnml through build systems such as CMake, Ninja, or Makefiles and pure command-line compilation commands as provided per release in [USER-BUILD.md](/doc/USER-BUILD.md), the user may resort to pre-compiled distribution `.zip` files containing the public headers (`lib-dnml.h(pp)`),  linkable static library (`libdnml_core.a`), and a `README.md` containing the supported OS/Distributions for such pre-compiled release and criteria for such identification (libc implementation, glibc version, etc). Here are the current (and planned) supported platforms for pre-compiled releases:

* MacOS - ARM64 (W.I.P - No release versions yet)
* MacOS - x86_64 (Intel Mac) (W.I.P - No release versions yet)
* Windows - x86_64 (W.I.P - No release versions yet)
* Linux - x86_64 (W.I.P - No release versions yet)
* Linux - ARM64 (W.I.P - No release versions yet)
* Linux - RISC-V (W.I.P - No release versions yet)
* MORE INCOMING... (Potential future support for BSD Distributions and Mobile platforms such as Android or iOS)



## Documentation
If you are a developer who wants to either contribute to our project, or just wants better integration quality by reading ducomentation, here are some valuable documentation for notable types:

* [BigNumbers Utilization Details](/dynamol/BIGNUM_CONTRACT.txt)
* [CryptNumbers Utilization Details](/drypto/CRYPT_NUM_CONTRACT.txt)
* [Cryptographically-secure API design protocol](/drypto/API-DESIGN.md)
* [Formal, Standardized, Porject-wide Code standard](/CODE-STANDARDS.md)
* Future documentation incoming..


## Project Status
~~~md
## Status
- Version: v0.3.1 (pre-release)
- API is subject to change

- Mathematical Module is maturing               [#####     ] 50%
- Testing & benchmarking in progress            [##        ] 25%
- Cryptography features in progress             [####      ] 40%
- Embedded-development features in progress     [          ] 0%
- Generic I/O in progress                       [          ] 0%
~~~


## License
lib-dnml is licensed under the Apache License 2.0, enabling for free acquisition and utilization for both commercial and non-commercial purposes as long you meet the project's conditions


## Contributing
Please have a look at [CONTRIBUTING.md](CONTRIBUTING.md) if you're interest in developing, maintaining, or overall just contributing in our project.


[latest-release-shield]: https://img.shields.io/github/v/release/ThienBeos123/lib-dnml
[latest-release-link]: https://github.com/ThienBeos123/lib-dnml/releases

[downloads-shield]: https://img.shields.io/github/downloads/ThienBeos123/lib-dnml/total
[downloads-link]: https://github.com/ThienBeos123/lib-dnml/releases