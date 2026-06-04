# libdnml Coding Standards

> This document defines the coding conventions, patterns, and safety rules for all contributors to libdnml.
> It applies to both the **Dynamol** (performance) and **Drypto** (cryptographic) modules unless explicitly noted.

---

## Table of Contents
1. [Language and Compiler Standards](#1-language-and-compiler-standards)
2. [Naming Conventions](#2-naming-conventions)
3. [Error Handling](#3-error-handling)
4. [Memory Management](#4-memory-management)
5. [Constant-Time Requirements](#5-constant-time-requirements-drypto-only)
6. [Formatting](#6-formatting)
7. [Algorithm Implementation Rules](#7-algorithm-implementation-rules)
8. [Post-Operation Cleanup](#9-post-operation-cleanup-drypto)

---

## 1. Language and Compiler Standards

- **C99 minimum**, C11 preferred
- All public headers must be C++ compatible via `extern "C"`
- No compiler extensions except in architecture-specific intrinsic files
- Intrinsic files are architecture-gated via `#ifdef`
- `__int128` is permitted in portable fallbacks only when `__SIZEOF_INT128__` is defined
- VLAs are **forbidden** in all files (timing variance, C11 optionality)

---

## 2. Naming Conventions

### Functions

| Category             | Pattern                   | Example                      |
|----------------------|---------------------------|------------------------------|
| Public API           | `type-name_verb_noun`     | `crint_mut_add`              |
| Internal algorithm   | `__TYPE-NAME_VERB_NOUN__` | `__CRINT_MAGADD__`           |
| Internal utility     | `__verb_adj_noun`         | `__libdnml_smemwipe_u64`     |
| Intrinsics           | `_platform_verb_noun`     | `_cintrin_wdiv128`           |
| Workspace functions  | `__TYPE-NAME_ALGO_WS__`   | `__CRINT_MUL_WS__`           |

### Types

| Category       | Pattern        | Example                      |
|----------------|----------------|------------------------------|
| Public types   | `snake_case`   | `crint`, `bigInt`, `dnml_status` |
| Internal types | `_snake_case`  | `_ctx`, `_arena`             |

### Macros

| Category              | Pattern          | Example              |
|-----------------------|------------------|----------------------|
| Public constants      | `MODULE_CONSTANT`| `FAKE_BUF_CAP`       |
| Low-level Macros      | `MACRO_NAME`     | `CHOOSE_OPTION`      |
| Top-level Macros      | `macro_name`     | `pbv_crint_clear`    |
| Error string literals | `snake_case`     | `full_contract`      |

### Files

| Category            | Pattern                  | Example               | Notes/Levels         |
|---------------------|--------------------------|-----------------------|----------------------|
| High-level headers  | `module_name.h`          | `crt_div.h`           | /util -> above+      |
| Low-level headers   | `__name.h`               | `__arm64_conn__.h`    | /intrinsics -> below |
| Intrinsic source    | `_arch_abi_module.S/.c`  | `_x86_sysv_arith.S`   | /intrinsics          |

---

## 3. Error Handling

### Two-Tier System

#### Tier 1: Test Asserts (`DNML_TEST_ASSERT`)
- Fires **only in debug builds** (`-DDNML_DEBUG`)
- Use for: poisoned state, early-exit hints, internal invariant cross-checks
- Usage requirements: 
    - Use only in small debugging tests to not leak too much memory upon program exit, where there might be only at most 1-2 crint outside of function scope.
    - Any DNML_TEST_ASSERT usage must either free any heap-allocated objects like arena first --> then input objects

```c
DNML_TEST_ASSERT(!x->poisoned, crint_poisoned, { crint_free(x); });
```

#### Tier 2: Error Return Codes
- Runtime error signaling, always propagated via `CHOOSE_OPTION`
- Use for: OOM, poisoned propagation, invalid runtime operations
- Requirements: DO NOT free any resources at all (except zeroizing stack-allocated ones on early returns for cryptography)

```c
CHOOSE_OPTION((ret_stat), (x->poisoned), (CRINT_POISON), (ret_stat));
```

### Contract vs Runtime Distinction

| Condition           | Category | Mechanism                                                                |
|---------------------|----------|--------------------------------------------------------------------------|
| NULL pointer        | Contract | `DNML_TEST_ASSERT` + Return Code                                         |
| Invalid struct      | Contract | `DNML_TEST_ASSERT` + Return Code                                         |
| OOM                 | Runtime  | Return code + (pseudo no-op) (`DNML_TEST_ASSERT` in perf-based modules)  |
| Poisoned input      | Runtime  | Return code + no-op (`DNML_TEST_ASSERT` in perf-based modules)           |
| Division by zero    | Runtime  | Return code + poison (`DNML_TEST_ASSERT` in perf-based modules)          |

---

## 4. Memory Management

### Ownership Rules
- Every `crint`/`bigInt` has **exactly one owner** at all times
- Ownership transfer is done via `crint_transfer` (O(1), invalidates source)
- Never call `free` on stack-allocated buffers (fake buffers, arena temporaries)
- Ownership transfers must be explicitly documented in function signatures
- For more details, please detail to the following:
    - [Big Numbers Type Contracts](/dynamol/BIGNUM_CONTRACT.txt)
    - [Crypt Numbers Type Contracts](/drypto/CRYPT_NUM_CONTRACT.txt)

### Sanitization Policy

| Module  | On Free | On Transfer | On OOM  |
|---------|---------|-------------|---------|
| Drypto  | Always  | Always      | Always  |
| Dynamol | Never   | Never       | Never   |

### Allocation Hierarchy

```
Drypto:  Heap (per-object) ──► Fake stack buffer (NOP cases only)
Dynamol: Arena (pre-sized) ──► Heap (public API surface only)
```

### Arena Rules (Dynamol only)
- Arena is **pre-sized** at the public API level via workspace functions
- Arena must **never grow** during algorithm execution
- Always `arena_mark` before allocation, `arena_reset` after
- NEVER `arena_clear` or `arena_destruct` unless DNML_TEST_ASSERT-ing in debug builds

```c
size_t ws = __BIGINT_TOOM_WS__(a->n, b->n);
size_t size_required = ws + a->n * 2;
arena_grow(_ARENA, size_required);
size_t mark = arena_mark(_ARENA);
// ... algorithm ...
arena_reset(_ARENA, mark);
```

---

## 5. Constant-Time Requirements (Drypto only)

### Mandatory Rules
- No conditional branches on **secret data values**
- No early returns based on **operand magnitude or bits**
- No **data-dependent memory access patterns**
- No **variable-length loops** based on secret data (unless it is an algorithm)

### Approved Patterns

**Conditional assignment (branchless):**
```c
CHOOSE_OPTION((dst), (cond), (val_true), (val_false));


**Pointer multiplexing:**
```c
crint *op = (ret_stat == CRINT_SUCCESS) ? &real_buf : &fake_buf;
```

### Forbidden Patterns

```c
// FORBIDDEN: Branch on operand value
if (x->limbs[i] > threshold) { ... }

// FORBIDDEN: Early return on operand property
if (x->n == 0) return CRINT_SUCCESS;

// FORBIDDEN: Potential Conditional Branching assembly emittion
CHOOSE_OPTION((x->n), (ret_stat == CRINT_SUCCESS), ())
```

### NOP Policy (Drypto)
When `ret_stat != CRINT_SUCCESS`, all operations must execute on fake stack buffers
to maintain timing consistency. Fake buffers are:
- Stack-allocated at function entry (size: `FAKE_BUF_CAP`)
- Pre-filled with non-zero sentinel values
- Zeroized post-operation unconditionally

```c
// Configurable — see API-DESIGN.md §7 for sizing guidance
#define CRINT_FAKE_BUF_CAP 128
```

---

## 6. Formatting

See `.clang-format` for automated enforcement. Column limit is **130 characters**

### Key Rules
- Indent: 4 spaces (no tabs)
- Related declarations & assignments may be grouped on one line where readable
- Block-scoped statements may follow closing `}` on the same line
- Use `// clang-format off/on` for intentionally compact sections

### Comment Style

```c
//* ====== SECTION HEADERS ====== *//    // Major sections
/* ------ subsection ------ */           // Subsections
// Single line comment                   // Inline annotation
```

---

## 7. Algorithm Implementation Rules

### Workspace Functions
Every algorithm with arena temporaries **must** expose a workspace function:

```c
size_t __BIGINT_ALGO_WS__(size_t a_size, size_t b_size);
```

Workspace function contracts:
- Returns a **safe upper bound** (never exact)
- Does **not** account for alignment padding (handled by arena internally)
- Called **only** at the public API level, never inside algorithms

### Recursion Policy

| Module  | Recursion | Rationale                              |
|---------|-----------|----------------------------------------|
| Dynamol | Allowed   | Performance; stack depth is bounded    |
| Drypto  | Forbidden | Timing variance; stack access leakage  |

## 8. Post-Operation Cleanup (Drypto)

All sensitive locals must be zeroized before return like this (NOT ORDERED):

1. Fake buffer wipe (`__libdnml_smemwipe_u64`)
2. Fake struct/pass-by-value parameter metadata clear (`pbv_crint_clear`)
3. Local variables zeroed
4. Pointers nulled
5. Return

```c
// Standard post-operation cleanup block
__libdnml_smemwipe_u64(fsbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
__libdnml_smemwipe_u64(fdbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
pbv_crint_clear(fake_src);
pbv_crint_clear(fake_dst);
local_var = 0;
ptr = 0;
return ret_stat;
```