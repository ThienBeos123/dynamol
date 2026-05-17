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
After the acquisition of the source code, have a look at INSTALLATION.md for detailed instructions at building, compiling, and integrating lib-dnml into your projects. For more platform-specific instructions:

* [Instruction for UNIX-like Platforms](/doc/UNIX-BUILD.md)
* [Instruction for Windows](/doc/WIN64-BUILD.md)
* [Instruction for Androids](/doc/ANDROID-BUILD.md)
* [Instruction for iOS](/doc/IOS-BUILD.md)
* [Instruction for BSD-based platforms](/doc/BSD-BUILD.md)


## Documentation
If you are a developer who wants to either contribute to our project, or just wants better integration quality by reading ducomentation, here are some valuable documentation for notable types:

* [BigNumbers Utilization Details](/adynamol/BIGNUM_CONTRACT.txt)
* Future documentation incoming..


## Project Status
~~~md
## Status
- Version: v0.2.2 (pre-release)
- API is subject to change

- Mathematical Module is maturing               [#####     ] 50%
- Testing & benchmarking in progress            [#####     ] 50%
- Cryptography features in progress             [#         ] 10%
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