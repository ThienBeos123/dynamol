# lib-dnml Coding Standards

> Library-wide conventions for all contributors. Module-specific design patterns
> and architecture decisions live in `CRYPT-API-DESIGN.md` (Drypto) and `PERF-API-DESIGN.md` (Dynamol).

---

## Table of Contents
1. [Language and Compiler Standards](#1-language-and-compiler-standards)
2. [Naming Conventions](#2-naming-conventions)
3. [Error Handling](#3-error-handling)
4. [Memory Management](#4-memory-management)
5. [Formatting and Code Style](#5-formatting-and-code-style)
6. [Function Sorting and File Structure](#6-function-sorting-and-file-structure)

---
<!-- ---------------------------------------------------------------------------------------- -->
## 1. Language and Compiler Standards

- **C99 minimum**, C11+ preferred
- All public headers must be C++ compatible via `extern "C"`
- No compiler extensions outside of intrinsic dispatching files (`eg: intrinsics.c`)
- Intrinsic files are architecture-gated via `#ifdef`
- `__int128` is permitted only when `__SIZEOF_INT128__` is `1/true`, and preferrably dispatched to also have alternative portable fallback on `#define __SIZEOF_INT128__ 0`
- VLAs are **forbidden** in all files outside of Dynamol (timing variance, C17+ optionality)




---
<!-- ---------------------------------------------------------------------------------------- -->
## 2. Naming Conventions

### Functions

| Category             | Pattern                   | Example                      |
|----------------------|---------------------------|------------------------------|
| Public API           | `type_verb_noun`          | `crint_mut_add`              |
| Internal algorithm   | `__TYPE_VERB_NOUN__`      | `__CRINT_MAGADD__`           |
| Internal utility     | `__lib_verb_adj_noun`     | `__libdnml_smemwipe_u64`     |
| Intrinsics           | `__CRT_VERB_NOUN__`       | `__CRT_MUL_U64__`            |
| Workspace functions  | `__TYPE_ALGO_WS__`        | `__CRINT_MUL_WS__`           |

### Types

| Category       | Pattern        | Example                          |
|----------------|----------------|----------------------------------|
| Public types   | `snake_case`   | `crint`, `bigInt`, `dnml_status` |
| Internal types | `_snake_case`  | `_ctx`, `_arena`                 |

### Macros

| Category              | Pattern           | Example           |
|-----------------------|-------------------|-------------------|
| Public constants      | `MODULE_CONSTANT` | `FAKE_BUF_CAP`    |
| Low-level macros      | `MACRO_NAME`      | `CHOOSE_OPTION`   |
| Top-level macros      | `macro_name`      | `pbv_crint_clear` |
| Error string literals | `snake_case`      | `full_contract`   |

### Files

| Category           | Pattern                 | Example               | Layer              |
|--------------------|-------------------------|-----------------------|--------------------|
| High-level headers | `module_name.h`         | `crt_div.h`           | /util and above    |
| Low-level headers  | `__name.h`              | `__arm64_conn__.h`    | /intrinsics        |
| Intrinsic source   | `_arch_abi_module.S/.c` | `_x86_sysv_arith.S`   | /intrinsics        |





---
<!-- ---------------------------------------------------------------------------------------- -->
## 3. Error Handling

### Two-Tier System

#### Tier 1: Test Asserts (`DNML_TEST_ASSERT`)
Fires **only in debug builds** (`-DDNML_DEBUG`). Used for early-exit hints on contract
violations (NULL pointers, invalid structs, poisoned state) to surface bugs during development.

Requirements:
- Use only where object scope is limited (at most 1–2 heap-allocated crints in scope at exit)
- Must free any arena or heap allocations **before** input objects in the cleanup block

```c
DNML_TEST_ASSERT((crint_pvalidate(x)), full_contract, { crint_free(x); });
DNML_TEST_ASSERT((!x->poisoned), crint_poisoned, { crint_free(x); });
```

#### Tier 2: Error Return Codes
Runtime error signaling. Always propagated branchlessly via `CHOOSE_OPTION`.

Requirements:
- **Do NOT free any heap resources** on error propagation (cleanup is post-operation only)
- **DO zeroize stack-allocated sensitive locals** on early returns in Drypto

```c
CHOOSE_OPTION((ret_stat), (x->poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
```

### Contract vs Runtime Decision Table

| Condition        | Category | Mechanism                         | Early Return Safe? |
|------------------|----------|-----------------------------------|--------------------|
| NULL pointer     | Contract | `DNML_TEST_ASSERT` + return code  | Yes                |
| Invalid struct   | Contract | `DNML_TEST_ASSERT` + return code  | Yes                |
| Poisoned input   | Runtime  | `CHOOSE_OPTION` + no-op           | **No** (Drypto)    |
| OOM              | Runtime  | `CHOOSE_OPTION` + pseudo-no-op    | **No** (Drypto)    |
| Division by zero | Runtime  | `CHOOSE_OPTION` + poison output   | **No** (Drypto)    |

> **Note:** In Dynamol, all of the above may early return without constant-time concerns.





---
<!-- ---------------------------------------------------------------------------------------- -->
## 4. Memory Management

### Ownership Rules
- Every `crint`/`bigInt` has **exactly one owner** at all times
- Ownership transfer via `crint_transfer` (O(1), invalidates source)
- Never call `free` on stack-allocated buffers (fake buffers, arena temporaries)
- Full type contracts: see [CRYPT_NUM_CONTRACT.txt](/drypto/CRYPT_NUM_CONTRACT.txt)
  and [BIGNUM_CONTRACT.txt](/dynamol/BIGNUM_CONTRACT.txt)

### Sanitization Policy

| Module  | On Free | On Transfer | On OOM |
|---------|---------|-------------|--------|
| Drypto  | Always  | Always      | Always |
| Dynamol | Never   | Never       | Never  |

### Allocation Hierarchy

```
Drypto:  Heap (per-object) ──► Fake stack buffer (NOP/pseudo-NOP cases)
Dynamol: Arena (pre-sized) ──► Heap (public API surface only)
```

### Arena Rules (Dynamol only)
- Arena is **pre-sized** at the public API level via workspace functions
- Arena must **never grow** during algorithm execution
- Always `arena_mark` before allocation, `arena_reset` after
- `arena_clear` / `arena_destruct` only in `DNML_TEST_ASSERT` debug cleanup blocks

```c
size_t ws = __BIGINT_TOOM_WS__(a->n, b->n);
arena_grow(_ARENA, ws + a->n * 2);
size_t mark = arena_mark(_ARENA);
// ... algorithm ...
arena_reset(_ARENA, mark);
```





---
<!-- ---------------------------------------------------------------------------------------- -->
## 5. Formatting and Code Style

See `.clang-format` for automated enforcement. Column limit: **130 characters**.

### Core Rules
- Indent: 4 spaces (no tabs)
- Pointer alignment: right (`limb_t *ptr`, not `limb_t* ptr`)
- Use `// clang-format off/on` to preserve intentionally compact sections

### Code Compactness

The style prioritizes **horizontal density** to keep function bodies compact for editor folding.
Related declarations, assignments, and short guards are grouped on single lines where the intent
remains clear. This is not a licence for obfuscation—group by logical relationship, not by line budget.

**Declarations and assignments** — group related locals on one line:
```c
// Good
size_t limb_shift = k / U64_BITS, bshift = k % U64_BITS;
limb_t fake_dst; limb_t *dst; uint64_t discarded_bits = 0;

// Bad (wastes vertical space for no clarity benefit)
size_t limb_shift = k / U64_BITS;
size_t bshift = k % U64_BITS;
limb_t fake_dst;
```

**Short guards** — place on one line with closing brace continuation:
```c
if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { x = 0; val = 0; return CRINT_NULL; }
if (!crint_pvalidate(x)) { x = 0; val = 0; return CRINT_ERR_INVAL; }
```

**Post-operation cleanup** — compact onto `// clang-format off/on` lines:
```c
/* Aggressive Post-operation Cleanup */ // clang-format off
ret_stat = 0; new_stat = 0; limb_shift = 0; bshift = 0; mask = 0;
fake_dst = 0; dst = 0; discarded_bits = 0; pbv_crint_clear(fake_src);
pbv_crint_clear(x); k = 0; err = 0; return ret_stat; // clang-format on
```

**Loop bodies with per-iteration cleanup** — group pre-calculations, assignment, and cleanup:
```c
for (size_t i = 0; _lib_crt_lt(i, x->cap); ++i) {
    size_t index; CHOOSE_OPTION((index), (_lib_crt_lt(i, x->n)), (i), (0));
    dst = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &x->limbs[i] : &fake_dst;
    /* Pre-calculations */
    uint64_t prev_dbits = discarded_bits, iso_mask = (UINT64_C(1) << bshift) - 1;
    uint64_t dbit_calc = x->limbs[index] & (iso_mask << U64_BITS - bshift);
    uint64_t dst_val = (x->limbs[index] << bshift) | prev_dbits;
    /* Assignment + Per-iteration Clearance */
    CHOOSE_OPTION((discarded_bits), (_lib_crt_lt(i, x->n)), (dbit_calc), (discarded_bits));
    CHOOSE_OPTION((*dst), (_lib_crt_lt(i, x->n)), (dst_val), (*dst));
    index = 0; prev_dbits = 0; dbit_calc = 0; dst_val = 0;
}
```

### Comment Style

```c
//* ====== SECTION HEADERS ====== *//    // Major file sections
/* ------ subsection ------ */           // Subsections within a section
// Single line comment                   // Inline annotation
/* Multi-line note:
*  continued here */                     // Multi-line explanatory note
```





---
<!-- ---------------------------------------------------------------------------------------- -->
## 6. Function Sorting and File Structure

Source files are divided into clearly labelled sections with section headers.
Functions within each section are sorted by **functional variant** first, then by **operand type width**:

```
Section order (example: crint_bitops.c):
  §1  Functional, unary ops         (crint_not, crint_rshift, crint_lshift, crint_lshiftg)
  §2  Mutative, unary ops           (crint_mut_not, crint_mut_rshift, ...)
  §3  Mutative, fixed-width ops     (crint_mut_andu64, crint_mut_oru64, ...)
  §4  Mutative, explicit-width ops  (crint_mutex_andu64, crint_mutex_and, ...)
  §5  Functional, fixed-width ops   (crint_andu64, crint_oru64, ...)
  §6  Functional, explicit-width    (crint_ex_andu64, crint_ex_and, ...)
```

Within each section, functions follow this operand type order where applicable:
```
  u64 variants → i64 variants → crint variants
```

**Internal algorithm files** (`__CRINT_MAG*__`) are sorted:
```
  MAGADD → MAGSUB → MAGMUL → MAGDIV → MAGMOD → MAGMUL_U64 → MAGDIVMOD_U64
  then: modular variants (MAGMADD, MAGMSUB, MAGMMUL, ...)
```

Section headers use the standard format:
```c
//* ================================= SECTION NAME ================================== *//
/* ------ subsection ------ */
```

**Intrinsic files** (`crt_intrin.c`) are sorted by operation category:
```
  §1  Single-limb arithmetic   (ADD, SUB, MUL, DIV)
  §2  Bitwise operations       (CLZ, CTZ, POPCOUNT)
  §3  Wide operations          (128-bit add, sub, etc.)
```




---
<!-- ---------------------------------------------------------------------------------------- -->
## 7. Algorithmic Side Note
This section details side-notes of some algorithmic and coding styles and patterns that is useful for programmatical safety, for the C programming language, despite its exceptional capability for performance and optimizations, is notoriosly unsafe and lenient, leading ot unexpected behaviors.

### Loop declarations
Loops are one of the most fundamental part of programming as a whole, in which it lets the user repeats repetitve tasks without copy and pasting code. However, with the wrong bounds checking and iteration incrementation/decrementation, it is one of the largest, yet hidden, source of bugs in the lib-dnml codebase. One mistake regarding iterators (often times named simply as a variable i) of unsigned types are bounds checking on a decrementing loop, often times written like this. 

```c
for (size_t i = TOP_BOUNDS - 1; i >= 0; --i) {
    // code over here ...
}
```

For cases where incrementing iterators would still yield the loop counts, it is the much preferred style, often written like this:

```c
for (size_t i = 0; i < TOP_BOUNDS; ++i) {
    // code over here ...
}
```

This is for the decrementing style only works for iterator of signed types, since sign types can go below zero and successfully trigger the exit condition of going below zero. However, for unsigned types, such conditions can't happen, due to how on iterations i, where `i == 0`, decrementing `i` would lead to unsigned integer wrap-around to the maximum value of the type's size. This would lead to a loose, endless loop. However, for cases in which it is absolutely necessary to use an unsigned integer type for the loop iterator, where the loop trajectory is descending (when iterating backwards through a string, for instance), the loop should be written like this:

```c
for (size_t i = TOP_BOUNDS; i > 0; --i) {
    // code over here ...
    array[i - 1] = ...;
    // Next code over there and there
}
```

This prevents underflow wraparounds for unsigned types like `size_t`, where we never have to go below zero to meet the bounds condition. However, for cases where the loop bounds is proven to fit sufficiently in the range of the signed integer type of `int64_t (2^63 - 1)`, then it is still preferred for the loop iterator to be of type int64_t and written like this for ease of development and index-accessing, since having to decrement from the iterator for every memory accesses of a buffer is error-prone and elusive:

```c
// WON'T CAUSE INFINITE RUNTIME SINCE SIGNED INTEGERS CAN BE CHECKED IF THEY GO BELOW ZERO
for (int64_t i = TOP_BOUNDS - 1; i >= 0; --i) {
    // code over here ...
}
```

