# lib-dnml TESTING

## Table of Contents
- [Overview](#overview)
- [Testing Methodology](#testing-methodology)
- [QOL Scripts](#quality-of-life-scripts)


<!-- ------------------------------ -------------------------------------------------------------->
---
## Overview
This directory, and all of its subdirectories (excluding automatically-generated outliers such as `/build` from CMake), for all intend and purposes, is resided exclusively for correctness testing. For any other purposes of testing further than quickly checking an algorithm/function/feature's algorithmic validity, it is mandated that it must be resided in the following

- `/benchmark`: Directory resided specifically for benchmarking algorithms for fine tuning algorithmic dispatching for maximal library performance, or proving algorithmic efficiency of lib-dnml mathematical functions and algorithms. (For more details, please have a look at the [README.md](/benchmark/README.md) located at [`/benchmark`](/benchmark/README.md))

- `/crt_test`: Directory created specifically for the imperative proof of Drypto's constant-time algorithms and functions via a variety of exploit simulations, including, but not limited to, timing attacks, cache-blowfish attacks, BPA (branch-predicition analysis), and a multitude of other side-channel attacks.

For any other testing purposes not fittingly appropriate in either of the 3 established directory, it is necessary that the contributor MUST create a pull request/issue, detailing the need for a new testing suite/directory, given enough community validation/agreement and a well-formulated argument for so.



<!-- ------------------------------ -------------------------------------------------------------->
---
## Testing Methodology
For lib-dnml `/test` and `/crt_test`, the tests must always follow the following criterias:

1. The test MUST be independent of any testing system, given said system is a generalized testing framework that has high complexity and maintenance in an effort to genearlize the execution of multiple distinct features

2. The test MUST be deterministic and reproducible, meaning the test cases are visual edge cases that can be hand-tested with an outside truth source.

3. The test MUST be cross-platform compatible, in which any contributor from nearly any platforms can execute the test for feedback.

In essence, these 3 criterias formulates a unit-testing environment for lib-dnml's features and functions that keeps development quick and inter-independent while keeping collaborative peer review of tests and features.



<!-- ------------------------------ -------------------------------------------------------------->
---
## Quality-of-life Scripts
In the development of testing runners and their edge cases, you might find it useful to use the Python and Lua scripts to help you quickly and efficiently convert certain objects (Strings, Numbers, etc) into a different form with different characteristics. It is highly advised for such scripts to be used, as they greatly increase development velocity, but the usage and development of such scripts have some rules. Here are some notable rules applicable to testing, found in [SCRIPTS.md](/scripts/SCRIPTS.md):

- For testing, please run the Bash script [dev_setup.sh](/scripts/dev_setup.sh) to download both necessary dependencies (CMake, Ninja, Rust + Cargo) and also Quality of Life ones (RISC-V64 Toolchains, Python, Lua, etc) for the best and msot integrated development experience if you're on a typical, modern desktop environment

- However, for people who are on a more restricted development environment, or just wants a minimalsitic development stack, please execute the Bash script [need_dependency.sh](/scripts/need_dependecy.sh) to only install CMake, Ninja, and Rust's Cargo.

- Quality of Life scripts must be seperated into different directories with different suffixes representing the sole language that the scripts inside will be written in (eg: bigInt.py ---> written in Python, side_script.lua ----> written in Lua) for compatibility with the root directory program script_launcher.lua. It is also highly advised that scripts are organized based on functionality and operation type

- The main scripting language for lib-dnml is primarily Python (for its robust integrated library, high scripting velocity, and integrated bigInt and bigFloat arithmetic) and Lua (light, robust, highly compatible with C). However, other scripting languages might be permitted as well depending on the user's second-hand expertise (Ruby, Go, JavaScript with Node.JS, etc).