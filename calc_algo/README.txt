===========================================
            Directory Overview: 
===========================================

This directory is reserved purely for algorithm implementations, standing as the core hub of mathematical
maturity of lib-dnml. In its entirety, its containment consists of only, and only, algorithms derived
from utilities from /util and mathematical intrinsics from /intrinsics. 

Algorithms would be divided into smal files that represent each algorithms supposed operation 
(multiplication, division, etc). Some files' categorization may be more broad (eg; num_theory.c) to
ensure there is not too many files to maintain and update, in which each files are too short.

For performance-derived algorithms (used in Dynamol), each algorithm must also provide, alongside
the algorithmic execution function, a workspace function that calculates and returns and conservative
estimate of an upperbound, covering all possible cases for internal algorithmic allocations (temporaries).
However, this is not mandated, and should not be followed, for cryptographical algorithm implementations,
in which such algorithms are used in Drypto, for they internally uses relation-independent, individual
heap allocations that reduce error handling complexity. For more details regarding allocation models, please
detail yourself to /drypto/CRYPT-API-DESING.md for cryptographically-secured algorithms, and
/dynamol/PERF-API-DESIGN.md for performance-derived ones.

For any operations, there are, and must be, dispatch functions that dispatch executions of different
algorithms based on the input size to reduce overhead of asymptotical algorithms like FFT for small
input sizes, while also keeping algorithmic complexity in check as input sizes grow.

Any other purposes may be devolved into another directory suitable for contribution needs, or,
upon the approval of a Pull Request / Issues, a new directory for a new need the is fit closely
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
        You are here!   |                     |
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
                                    |
                 _____________      |
                |             |____/
                | 2. /include |                  <--- (2. Base API / Header Definitions)
                |_____________|

   (Note: Level 1 is reserved for workspace configuration files/scripts outside source folders)
===================================================================================================