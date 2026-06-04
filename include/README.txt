===========================================
            Directory Overview: 
===========================================

This directory is reserved for the following:


------ LIGHT, QOL-like REASONS ------
- Underlying, Assisting types (eg: _libdnml_mem/arena.h)
- Library-wide Configurations (eg: /_libdnml_config, Debugging, Algorithm Thresholds, etc)
- Core, multi-directory/library-wide shared operation types (eg: libdnml_types.h, dnml_status.h)
- Library-wide, shared EXTERNED constants (char_tables.h)

------- FORMAL, MAIN REASONS -------
- Compile-time platform detection and dispatching:
    + Operating Systems: Windows, Linux, BSDs, MacOS, iOS, Android (dnml_sys/__platform.h)
    + CPU Architecture: x86_64, ARM64 (ARMv8, ARMv8.5, ARMv9), RV64 (RV64I, RV64GC)
    + ABI / Calling Conventions: Win64 (x64), System-V AMD (x64)
    + Compilers: MSVC, GCC, Clang
- Runtime, Hardware Capability detection and CPUID
- Various other system-related detection work



Any other may be devolved into a directory that utilizes /include global #include <> semantics to leverage
their convenience advantage, such as /util or /intrinsics.


========================================================================================
                             Directory Hierarchy Visualization                         
========================================================================================

    [ 8. /test ] <--- (Optional / Main Consumer Layer)
         |
         v
    _________________________________________________________________
   |                                                                 |
   |                           7. /diostream                         |
   |_________________________________________________________________|
         ^                          ^                          ^
         +--------------------------+--------------------------+
         |                          |                          |
         |                          |                          |
    ___________                ___________                ___________
   |           |              |           |              |           |
   |  /dbedded |              |  /drypto  |              |  /dynamol |  <--- (6. High-Level Domains)
   |___________|              |___________|              |___________|
         \                          |                          /
          \_________________________+_________________________/
                                    |
                                    v
                         _____________________
                        |                     |
                        |    5. /calc_algo    |  <--- (Core Algorithms / Calculations)
                        |_____________________|
                                    |
                                    v
                         _____________________
                        |                     |
                        |       4. /util      |  <--- (General Utilities)
                        |_____________________|
                                    |
                                    v
                         _____________________
                        |                     |
                        |   3. /intrinsics    |  <--- (Hardware Architecture / SIMD)
                        |_____________________|
                                    ^
                                    |
                  You are here!     |
                 _____________      |
                |             |____/
                | 2. /include |                  <--- (2. Base API / Header Definitions)
                |_____________|

   (Note: Level 1 is reserved for workspace configuration files/scripts outside source folders)
===================================================================================================
 
            