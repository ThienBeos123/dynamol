# HOW TO PROPERLY IMPLEMENT TEST FOR lib-dnml

## Table of Contents
- [Testing Methodology](#testing-methodology)
- [Framework Setup](#framework-setup)
- [Random Case Generation](#random-case-generation)
- [Runners and their setup](#runners-and-their-setup)
- [QOL Scripts](#quality-of-life-scripts)
- [Execution Scripts](#execution-scripts)


<!-- ------------------------------ -------------------------------------------------------------->
## Testing Methodology
 In lib-dnml, some already established test (made and maintained by ThienBeos123) utilizes fuzzy-testing, or, more precisely, controlled-entropy, randomly-generated tests, as supported by generators in [/case_gen](/test/case_gen/).

 However, moving forward, it is not needed, and strictly advised not so, to only include unit tests, in which features are tested solely on established, deterministic, and well-known edge cases. This is to not stunt development velocity and overcomplicate testing facilities, as fuzzy-testing is a rather complex endeavor.

 If fuzzy-testing if of interest for any contributor, please develop an established, deterministic, and breadth-ful framework in [/test_ui](/test_ui/) to generalize the randomly-generated testing of multiple features of the same genre. Please have a look at the [Framework Setup](#framework-setup) section for more details on this matter. If not, please skip the next two sections - [Framework Setup](#framework-setup) and [Random Case Generation](#random-case-generation).



<!-- ------------------------------ -------------------------------------------------------------->
## Framework setup
### - Framework Definition 
The most important part of any generalized testing framework that supports fuzzy-testing is the execution, containment, and evaluation of randomly generated cases. These features are stored in suites, objects that manages and runs both edge and randomly-generated cases as structs, in which it is managed by a session.

-  A session is a unit of testing, in which each runners (a .c file or whatever) only have 1 of. It contains multiple suites and running them sequentially. Each suites may share the same context and buffers and resources in the same session, as the execution of suites by their session is sequential. However, it is still highly-advised for verification of such shared resources, or else each suite should have their own resource pool. For further details, suite yourself in the [Runners and their setup](#runners-and-their-setup) section

Next, our generic (and often type-washed for many reasons) is stored and defined as a header file in [/test_ui](/test_ui/), with printing, formatting, and logging support in [_test_base.h](/test_ui/_test_base.h), and each framework is a module, responsible for utilizing such interface support for execution, evaluation, suite setup, and resources management generically. These framework is primarily written as ``.h`` file for C-based test runners, but certain framework might entail the encapsulation inside a ``.rs`` file for Rust-based test runners

### - Agents of Execution
As detailed in the previous subsection, ***execution***, ***failure containment***, and ***evaluation*** are the cornerstones of our library's fuzzy-testing:

- Execution are basically the method in which our group of functions/features can be generically executed by our framework. It can be either a simple function pointer, in which it is simply dispatched by a few input cases and executed like so in our testing framework (like in [lowui.h](/test_ui/lowui.h)), or generically inputted as a generalized wrapper, in which the wrapper is stored inside the feature's specific testing directory (like in [_strui.h](/test_ui/_strui.h) with bigInt execution wrappers in [bi_exec_func.h](/test/io/bigint/bi_exec_func.h))

- Failure containment varies a lot depending on the complexity and scale of the tested feature. Failure containment can be storing cases in which failed inside a pre-allocated array on the runner's surface, stored as a pointer inside our suite's struct (preferrably stack-allocated in such cases), and then sequentially printed out (this is referred to as ***Batch-based Containment***). On the other hand, we have stream-lined failure containment through generating, executing, evaluating, and printing per case, achieving O(1) memory usage in largely-scaled features test (like [IO Testing](/test/io)) (called as ***Stream-based Containment***), with a tradeoff of much more abstractions and overhead

- Evaluation is a pretty tricky topic, considering our library operates at a pretty low-level. However, here are some notorious examples of evaluation methodologies that we can have:

    - **Outside Source**: This is probably one of the most common one, and most preferred one when fuzzy-testing. It utilizes established, production-grade, combat-ready outside sources such as ***Python***, ***GMP***, or ***OpenSSL*** as truth-source, especially when universal, numerical calculations. However, for domain/library-specific features such as I/O, this is not preferred as no truth source fully aligns with our library's behavior.

    - **Inverses**: This is one of the most common, typically found in the established testing for [bigInt IO](/test/io/bigint/) for normal function APIs (apart from truncative ones). It establish a function and its respective inverses. This is largely effective, as it hit two birds with one stone. However it is also largely risky due to the unpredictable nature of it, where it is entirely possible for The execution and its inverse to both be flawed, but nullify each other's flaws out to still get the original answer. Therefore, it is recommended to set the amount of random case for every feature to 0 to test holistically their edge-case potency before jumping in into inverse-based testing

    - **Evaluators**: This is pretty much the last-resort, least effective one by far. It is a way to evaluate truncative APIs in IO testing, where it is nigh-impossible to uses an inverse due to information-loss from truncation. Therefore, we basically simulates a "more-deterministic version" of the function's execution, in which we only target signal return areas. This is really risky and inefficient in most cases, due to the method's dependcy on the correctness of the evaluator

    - **Variant-dependency**: This method is commonly found inside our [/test/low](/test/low) testing module, where we compare each function to its more deterministic variant, like the Vanilla-C variants. However, this method is also risky due to its dependency on yet-to-be-proven functions, in which it can simulate falsehoods.

### - Sideline Agents
Other important parts of fuzzy-testing include interface/formatting for logging and visual examination of tests, and also random case generation. For random case generation, please entail to the next section: [Random Case Generation](#random-case-generation). For interface functionalities. There might be some distinctions for each features and frameworks. However, It is arguably universal that interface functionalities such be able to print out the number of case correct for each suite / number of total cases. It is also arguable that every framework's interface functionalities should have logging functionalities, in which the full case is printed out and revealed to the tester, alongside session compaction, in which if there are too many sessions in one testing run, each suites should only reveal the number of cases correct / total, instead of printing out the incorrect cases also to the terminal.



<!-- ------------------------------ -------------------------------------------------------------->
## Random Case Generation
Random case generation should be relatively simple and controlled, but also unbiased in its controlled domain of generation as well. Its control domain should be relatively big, covering simple, clean, and trivial cases to recognizably faulty cases that might be tricky to catch for our features. Here are some notes for this section:

- Each case-generation files (.c) must have a configuration struct (eg: str_rand_mod in [str_casegen.c](/test/case_gen/str_casegen.c)) that supports the containment of pluggable components or probability. Each file should also have dispatched probabilites for certain components/case based generation modes. They must also have initialization functions, in which the user inputs key configuration choice that is permanent and entropy-states (xoshiro256_state)

- Key component configuration entirely dictates the numerical amounts configuration, or, more precisely,numerical amounts configuration is automatically not set and MUST NOT be touched if "key components configuration" of such components are FALSE / NOT set to TRUE.

- Any early implementation may accept the use of simpler & less cryptographically-secured combination of srand(time(NULL)) and rand() by ANSI-C <stdlib.h>, though TRNG/HWRNG-seeded entropy are preferred for more unbiased source of entropy for xoshiro256++ (as defined and implemented in [/util/sec_util.c](/util/sec_util.c))

- Regarding entropy collection via TRNG/HWRNG, it is generally preferred to use lib-dnml's OS-dispatched entropy collector family - _GET_ENTROPY_*(), as defined and implemented in [/intrinsics/intrinsics.h](/intrinsics/intrinsics.h) - for cross-platform compatibility and stability of usage from the library's standardization. However, early prototypes may use OS-specific entropy harvest ONLY for prototyping of functionality and reliability, BUT final implementation must use the general function like _GET_ENTROPY_FAST()  or another specialized entropy-collection function that supports at least the dispatching of MacOS, Linux, AND Windows (64 bit).

- Any future development may be welcomed for the introduction of higher quality, less biased, and generally more robust on statistical quality than the default xoshiro256++ algorithm (such as PCG). However, the RNG algorithm should be ***RELATIVELY SIMPLE***, ***FAST***, ***DOESN'T REQUIRE ABSURD STATE SIZES***, and especially ***NOT CRYPTOGRAPHICALLY-SECURED***. This is to ensure testing execution speed, ease of development integration and velocity. Some disqualifiers for testing RNG would be *Mersenne Twister 19937* (requires a 19937 bit-sized state and relatively slow), *ChaCha20* (Cryptographically-secured), and any *AES*, *Hash-based*, or some *Elliptic-curve-based PRNGs*.



<!-- ------------------------------ -------------------------------------------------------------->
## Runners and their setup
The setup of runners is crucial for our testing, so here are some notes on how to properly setup a runner for our features.

- The runner must contain one global list of edge cases. It can be stored as a global array in memory, a dynamically loadable list of test cases into a suite's memory, or in file in which the input is parsed into memory.

- The runner must have adequate setup for the containment of failure case (for both edge case and random case) and the logging and printing of it the terminal/a log file (.txt). This container is preferrably stack-allocated for speed and ease of resource management.

- <span style="color:red"> CRITICAL: </span> For runners that are designed to test the arithmetic and mathematical correctness (and therefore excludes and I/O and Low-level Intrinsic tests), most of the runners prefer the utilization of Rust for builtin Multithreading and Async features for testing execution speedup. Therefore, it is highly advised for developers to write test in Rust and download Cargo for such tests. However, it is also mandatory to write potentially multi-thread, Pure C runners for such categories of test for backward compatibility with older systems and highly-constrained systems. For such backup units, it is acceptable to use platform-exclusive features like POSIX theads, mutex, etc.


<!-- ------------------------------ -------------------------------------------------------------->
## Quality-of-life Scripts
In the development of testing runners and their edge cases, you might find it useful to use the Python and Lua scripts to help you quickly and efficiently convert certain objects (Strings, Numbers, etc) into a different form with different characteristics. It is highly advised for such scripts to be used, as they greatly increase development velocity, but the usage and development of such scripts have some rules. Here are some notable rules applicable to testing, found in [SCRIPTS.md](/scripts/SCRIPTS.md):

- For testing, please run the Bash script [dev_setup.sh](/scripts/dev_setup.sh) to download both necessary dependencies (CMake, Ninja, Rust + Cargo) and also Quality of Life ones (RISC-V64 Toolchains, Python, Lua, etc) for the best and msot integrated development experience if you're on a typical, modern desktop environment

- However, for people who are on a more restricted development environment, or just wants a minimalsitic development stack, please execute the Bash script [need_dependency.sh](/scripts/need_dependecy.sh) to only install CMake, Ninja, and Rust's Cargo.

- Quality of Life scripts must be seperated into different directories with different suffixes representing the sole language that the scripts inside will be written in (eg: bigInt.py ---> written in Python, side_script.lua ----> written in Lua) for compatibility with the root directory program script_launcher.lua. It is also highly advised that scripts are organized based on functionality and operation type

- The main scripting language for lib-dnml is primarily Python (for its robust integrated library, high scripting velocity, and integrated bigInt and bigFloat arithmetic) and Lua (light, robust, highly compatible with C). However, other scripting languages might be permitted as well depending on the user's second-hand expertise (Ruby, Go, JavaScript with Node.JS, etc).



## Execution Scripts
For each category of feature/directory, it is mandatory for there to be a ```CMakeLists.txt``` file for building and compiling multiple C source file alongside outside dependecies, ```CARGO.toml``` for directories that contains Rust-based test runners, ```run_*.sh``` for executing the requested runners into logfiles, and ```*_concat.sh``` for concatenating multiple different log files from different runners into one log file for that test run.

Additionally, ```run_*.sh``` must have some sort of interface/parameter for the tester to input the desired runners to execute, and ```*_concat.sh``` must showcase which log files from which runners were concatenated