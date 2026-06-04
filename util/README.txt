===========================================
            Directory Overview: 
===========================================

This directory is reserved purely for widely-used utility that isn't endemic to a single file/module.
These utilities are often general-purpose, or, at most, serves a general task repeatable across
a wide range of operations inside a module's file(s). Some notorious examples include

- __libdnml_smemwipe_u64() -- Memory utility used to cryptographically wipe out a buffer. It is in fact
                              an abstraction/wrapper for __libdnml_memwipe_strct with alignments for U64

- __MAG_I64__ -- Type conversion utility to quickly and correctly get the magnitude of a signed integer
                 (int64_t) and return it as uint64_t 
                 (This can be only done portably in pure-C, and therefore resides here)

- __CRINT_TRIM_LZ__ -- Reduces the size metadata to corectly represent a 
                       mathematically efficient and correct state for crint

This is not to be mixed up with /intrinsics,
for intrinsics are low-level operations, typically operating on only fixed-width types like 64-bit
uint64_t or 32-bit uint32_t (and sometimes even __int128_t internally), while these utilities operate
on more sophisticated data types like buffers/arrays or even module-specifc ones like bigInt. 

/intrinsics
are also for operations that are more algorithmic/mathematically vital as opposed to being programmatical state
utility, with each operation being able to be design in a straight-forward manner inside architecture-specific assembly
while also having potentially available compiler-extended intrinsics, as opposed to some of the more complex
patterns seen in utilities, covering loops, conditional branching, weird type conversion, and function calls.


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
        You are here!   |                     |
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
                                    |
                 _____________      |
                |             |____/
                | 2. /include |                  <--- (2. Base API / Header Definitions)
                |_____________|

   (Note: Level 1 is reserved for workspace configuration files/scripts outside source folders)
===================================================================================================