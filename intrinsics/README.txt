===========================================
            Directory Overview: 
===========================================

This directory is reserved for the following:
- Functions that executes an operation that is primitive-like (operates exclusively on primitive
  types like uint64_t, int64_t, etc), but isn't innately available portably to the disposable
  of the programming language C (__ADDC_U64__, __WMUL_128__) (1)

- CPU Architecture specific intrinsics (follows (1)):
    + x86_64 SYSTEM-V AMD ABI:  .S files for intrinsics with different categories
    + x86_64 WIN64 ABI:         .S files for intrinsics with different categories
    + ARM64:                    .S files for intrinsics with different categories
    + RV64:                     .S files for intrinsics with different categories

- Portable C fallback for unrecognized/unsupported CPU architectures/platforms:
    + /zvanillc: Performance-based intrinsics written to be as optimized as possible
    + /crt_vanillc: Cryptographically-secured intrinsics for security

- Intrinsic dispatching based on the follow:
    + Dispatching of lib-dnml hand-written intrinsics based on platforms (intrin.base.c)
    + Dispatching of underlying engine intrinsic based on platforms, which may instead 
      utilize compiler extensions if available instead for operation correctness certainty 
      (crt_intrin.c + intrinsics.c)


Any other purposes may be devolved into another directory suitable for contribution needs, or,
upon the approval of a Pull Request / Issues, a new directory for a new need that fit closely
within this hierarchy of processes


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
        You are here!   |                     |
                        |   3. /intrinsics    |  <--- (Hardware Architecture / SIMD)
                        |_____________________|
                                    ^
                                    |
                                    |
                 _____________      |
                |             |____/
                | 2. /include |                  <--- (2. Base API / Header Definitions)
                |_____________|

   (Note: Level 1 is reserved for workspace configuration files/scripts outside source folders)
===================================================================================================