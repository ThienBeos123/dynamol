# Drypto (cryptInt) API Design

> Architecture specification for the Drypto module (`crint`). This document covers design
> decisions, safety invariants, memory model, branchless enforcement, and dependency usage.
> It is **not** a usage manual — for the full type contract see [CRYPT_NUM_CONTRACT.txt](/drypto/CRYPT_NUM_CONTRACT.txt).

---

## Table of Contents
1. [Module Purpose and Constraints](#1-module-purpose-and-constraints)
2. [Type Definition and Validity States](#2-type-definition-and-validity-states)
3. [Status Codes](#3-status-codes)
4. [Memory Model and Ownership](#4-memory-model-and-ownership)
5. [Error Handling Model](#5-error-handling-model)
6. [Branchless Enforcement Model](#6-branchless-enforcement-model)
7. [Dependency Usage](#7-dependency-usage)
8. [Configurable Constants](#8-configurable-constants)
9. [Platform Support](#9-platform-support)

---
<!-- =========================================================================================== -->
## 1. Module Purpose and Constraints

Drypto provides arbitrary-precision integer arithmetic with the following hard guarantees:

- **Constant-time execution** on all secret-data-bearing operations
- **No timing variance** from operand values, magnitudes, or bit patterns
- **Aggressive memory sanitization** — all heap and stack buffers containing sensitive data are zeroized before release
- **No recursion** — all algorithms are iterative to prevent stack-depth timing leakage

> **Rule:** Never mix `crint` and `bigInt` in any code path that handles secret or sensitive data.
> `bigInt` is explicitly not constant-time and must never appear in cryptographic workflows.



---
<!-- =========================================================================================== -->
## 2. Type Definition and Validity States

```c
typedef struct {
    limb_t  *limbs;     // Heap-allocated limb array (little-endian, limbs[0] = LSL)
    size_t   n;         // Active limb count; canonical: limbs[n-1] != 0 when n > 0
    size_t   cap;       // Allocated capacity in limbs
    int8_t   sign;      // 1 = positive/zero,  -1 = negative
    uint8_t  poisoned;  // Non-zero: struct is unusable, all ops become no-ops
} crint;
```

### Validity States

| State            | `limbs`  | `n`    | `cap`  | `sign`    | `poisoned` |
|------------------|----------|--------|--------|-----------|------------|
| Valid, non-zero  | non-NULL | > 0    | >= n   | 1 or -1   | 0          |
| Valid, zero      | non-NULL | 0      | >= 1   | 1         | 0          |
| Poisoned         | non-NULL | any    | >= 1   | any       | non-zero   |
| Invalid (assert) | NULL     | any    | any    | any       | any        |

A `crint` with `limbs == NULL` is a **hard contract violation** and triggers `DNML_TEST_ASSERT`.
A `crint` with `poisoned != 0` is a **runtime error state** and is handled branchlessly.

Full contract specification: [CRYPT_NUM_CONTRACT.txt](/drypto/CRYPT_NUM_CONTRACT.txt)





---
<!-- =========================================================================================== -->
## 3. Status Codes

```c
typedef enum {
    CRINT_SUCCESS    = 200,   // Operation succeeded
    CRINT_POISON,             // 201 — Input was poisoned; result is poisoned no-op
    CRINT_ERR_RANGE,          // 202 — Value out of representable range
    CRINT_ERR_DOMAIN,         // 203 — Mathematically undefined (e.g. division by zero)
    CRINT_NULL,               // 204 — NULL pointer passed as input
    CRINT_ERR_INVAL,          // 205 — Struct in fully invalid state
    CRINT_ERR_SINVAL,         // 206 — Struct in partial/storage-invalid state
    DNML_ALLOC_OOM,           // 207 — Heap allocation failed
} dnml_status;
```
`CRINT_SUCCESS = 200` is intentional — it avoids ambiguity with zero-initialized memory and
makes accidental `memset`-zeroed status fields detectable.





---
<!-- =========================================================================================== -->
## 4. Memory Model and Ownership
### Allocation Strategy
All `crint` limb buffers are individually heap-allocated. There is no shared arena.
This enables:
- Per-object sanitization on free (guaranteed by `crint_free`)
- Graceful OOM: one allocation failure does not corrupt other objects
- Simpler ownership tracking compared to arena-based designs

Temporaries inside algorithm functions are also heap-allocated and freed explicitly
or transferred to the result via `crint_transfer`.

### Ownership Semantics
**Pass-by-pointer (`crint *x`):** Function borrows `x`. Caller retains ownership.

**Pass-by-value (`crint y`):** Function takes ownership of `y`. Caller must NOT free `y`
after the call. The function is responsible for consuming or freeing it.

**`crint_transfer(dst, src)`:** O(1) ownership transfer. `src` is invalidated
(limbs set to NULL, metadata zeroed). `dst` takes ownership of `src`'s buffer.
The previous buffer owned by `dst` must be freed by the caller before transfer,
or saved and freed after.

> **Known limitation:** The current API does not force the caller to save `dst`'s old buffer
> before a mutative call that reallocates. A future API refactorization may mandate this
> explicitly. For now, callers using mutative functions are responsible for tracking and
> disabling/NULL-ing any previously held buffer references, since such actions are considered
> contract invariant violations in current API version

### Thread Safety
Each `crint` owns its own buffer. Operations are not thread-safe on a shared `crint`.

| Operation                  | Thread Safe | Notes                               |
|----------------------------|-------------|-------------------------------------|
| Read-only (non-mutating)   | Yes         | No shared mutable state             |
| Mutative (`crint_mut_*`)   | No          | Caller must synchronize externally  |
| `crint_transfer`           | No          | Mutates both dst and src            |
| `malloc`/`free` internally | Yes         | libc heap is thread-safe            |

### Sanitization Policy
Every deallocation path **must** zeroize the limb buffer before `free`:
```c
// crint_free internals (simplified)
__libdnml_smemwipe_u64(x->limbs, x->cap, 0, x->cap - 1, false);
free(x->limbs);
x->limbs = NULL; x->n = 0; x->cap = 0;
```
Stack-local temporaries (fake buffers, pass-by-value parameters) are also zeroized in the
post-operation cleanup block before the function returns. See §6.





---
<!-- =========================================================================================== -->
## 5. Error Handling Model

### Which errors may early-return

| Condition          | Early Return Allowed? | Rationale                                                          |
|--------------------|-----------------------|--------------------------------------------------------------------|
| NULL pointer input | **Yes**               | Metadata only; no secret data on the stack yet                     |
| Invalid struct     | **Yes**               | No secret, valid data is loaded + invalid data is virtually usless |
| Poisoned input     | **No**                | Timing of poison check leaks operand state                         |
| OOM                | **No**                | Timing of failed alloc leaks operand size                          |
| Division by zero   | **No**                | Must propagate without branching on operand                        |

NULL pointer and invalid-struct early returns **must** zeroize any pass-by-value
parameters before returning:

```c
if (_lib_crt_eq((ptr_t)x, (ptr_t)(NULL))) { pbv_crint_clear(y); x = 0; return CRINT_NULL; }
if (!crint_pvalidate(x) | !crint_validate(y)) { pbv_crint_clear(y); x = 0; return CRINT_ERR_INVAL; }
```

All other errors are propagated via `CHOOSE_OPTION` and handled in the post-operation
cleanup block.

### Poisoned Input Propagation

A poisoned input does not terminate the function. It sets `ret_stat` and redirects
all subsequent operations to fake stack buffers via pointer multiplexing.
The result is always returned as poisoned with NULL limbs.

```c
CHOOSE_OPTION((ret_stat), (x->poisoned & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))), (CRINT_POISON), (ret_stat));
```

### OOM Propagation

OOM on a temporary allocation sets `ret_stat = DNML_ALLOC_OOM` and redirects
to pre-allocated fake buffers. The real operands are untouched.

```c
CHOOSE_OPTION((ret_stat),
    (_lib_crt_eq(snew_stat, DNML_ALLOC_OOM) & (_lib_crt_eq(ret_stat, CRINT_SUCCESS))),
    (DNML_ALLOC_OOM), (ret_stat)
);
```

---

## 6. Branchless Enforcement Model

All operations on secret data must execute without data-dependent branches.

### Core Primitive: `CHOOSE_OPTION`

```c
#define CHOOSE_OPTION(dest, cond, a, b) do { (dest) = _lib_crt_select((cond), (a), (b)); } while(0);
```

`_lib_crt_select` compiles to a `cmov`-equivalent (mask-based selection on targets without
native cmov). This is the **only approved conditional assignment mechanism** for secret-data paths.

### Pointer Multiplexing

On failure (OOM or poison), operation pointers are redirected to fake stack buffers.
Both the real and fake paths always execute the same code:

```c
crint *src  = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? x         : &fake_src;
crint *prod = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &tmp_prod : &fake_prod;
__CRINT_MAGMUL_U64__(prod, src, _lib_crt_select(..., val, fake_val));
```

### Success Mask Pattern

For bulk metadata operations, a full-width mask gates all assignments:

```c
uint64_t mask = (uint64_t)(-(int64_t)(_lib_crt_eq(ret_stat, CRINT_SUCCESS)));
// mask = 0xFFFFFFFFFFFFFFFF on success, 0x0000000000000000 on failure
res.n    = actual_n    & mask;
res.cap  = actual_cap  & mask;
res.sign = actual_sign & mask;
```

### Conditional Free via NULL Masking

Result limbs are freed conditionally without branching:
```c
// free(NULL) is a guaranteed no-op per ANSI-C
limb_t *chosen_freed = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? NULL : res.limbs;
free(chosen_freed);
res.limbs = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? res.limbs : 0;
```

### NOP Buffer Policy

Operations that cannot be trivially no-op'd (normalization, memcpy, memset) execute on
a pre-allocated fixed-size fake buffer:

```c
limb_t fake_buf[FAKE_BUF_CAP] = {0};
crint fake_normalized = { 
    .limbs = fake_buf, .n = FAKE_BUF_CAP, 
    .cap = FAKE_BUF_CAP, .sign = 1, .poisoned = false 
};
crint *norm_crint = (_lib_crt_eq(ret_stat, CRINT_SUCCESS)) ? &res : &fake_normalized;
crint_normalize(norm_crint);
```

The fake buffer is always zeroized in the post-operation cleanup block.

### Forbidden Patterns

```c
// FORBIDDEN — branch on secret operand value
if (x->limbs[i] != 0) { ... }

// FORBIDDEN — early return based on operand magnitude
if (x->n == 0) return CRINT_SUCCESS;

// FORBIDDEN — variable loop bound from secret data
for (size_t i = 0; i < x->n; ++i) { ... } 
// Only valid if x->n is not secret OR essential to retain algorithmic correctnes
```

### Post-Operation Cleanup

Every function ends with an aggressive cleanup block that zeroizes all sensitive locals.
This block uses `// clang-format off/on` for compactness:

```c
/* Aggressive Post-operation Cleanup */ // clang-format off
__libdnml_smemwipe_u64(fsbuf, FAKE_BUF_CAP, 0, FAKE_BUF_CAP - 1, false);
pbv_crint_clear(fake_src); pbv_crint_clear(x);
ret_stat = 0; new_stat = 0; limb_shift = 0; mask = 0; fake_dst = 0;
dst = 0; discarded_bits = 0; k = 0; err = 0; return ret_stat; // clang-format on
```

---

## 7. Dependency Usage

### Cryptographically Secured Intrinsics (`crt_intrin.h` / `intrinsics.h`)

All single-limb arithmetic in Drypto **must** use the cryptographic intrinsic layer,
not raw C operators. The intrinsic layer dispatches to the optimal hardware instruction
(compiler built-in → vendor intrinsic → architecture assembly → portable fallback):

```c
// Required for carry-propagating addition
uint64_t __CRT_ADD_U64__(uint64_t a, uint64_t b, uint8_t *carry);

// Required for borrow-propagating subtraction
uint64_t __CRT_SUB_U64__(uint64_t a, uint64_t b, uint8_t *borrow);

// Required for 128-bit product
uint64_t __CRT_MUL_U64__(uint64_t a, uint64_t b, uint64_t *hi);

// Required for 128-bit / 64-bit division
uint64_t __CRT_DIV_U128__(uint64_t lo, uint64_t hi, uint64_t div,
                           uint64_t *rhat, uint8_t *overflowed);
```

Using raw C arithmetic (`a + b`, `a * b`) in Drypto algorithm code is **forbidden**.
Overflows, carries, and borrows must be handled explicitly via these intrinsics to prevent
undefined behavior and ensure the compiler does not introduce branches via optimization.

### Lower-Level Utilities (`crt_util.h`)

Memory utilities in `crt_util.h` provide constant-time memory operations with mandatory
`noop` parameters to allow fake-buffer execution without branching:
```c
// Always iterates full range; noop = true writes to buffer but caller discards result
void __libdnml_smemwipe_u64(uint64_t *buf, size_t len, size_t start, size_t end, bool noop);
void __libdnml_smemset_u64(uint64_t *buf, uint8_t val, size_t len, size_t start, size_t end, bool noop);
void __libdnml_smemcpy_u64(uint64_t *dst, uint64_t *src, size_t len, size_t srclen,
                            size_t start, size_t end, bool noop);

// DEFINED AND DECLARED INSIDE crt_util.h
#define CHOOSE_OPTION(dest, cond, a, b) do { (dest) = _lib_crt_select((cond), (a), (b)); } while(0);
#define RETURN_OPTION(cond, a, b) do { return _lib_crt_select((cond), (a), (b)); } while(0);

#define crtmin(x, y) _lib_crt_select(_lib_crt_lt(x, y), x, y)
#define crtmax(x, y) _lib_crt_select(_lib_crt_gt(x, y), x, y)
```

### Lower-Level Utilities (`crt_intrin.c`/`intrinsics.h`)
Normal operations like comparison operators must also use any available cryptographically-secured version defined in `crt_intrin.c`/`intrinsics.h`.
```c
// These must be ALWAYS used on all data regardless of security importance to enforce habits
// DEFINED AND DECLARED inside crt_intrin.c
uint64_t __CRT_DIV_U128__(uint64_t lo, uint64_t hi, uint64_t div, uint64_t *rhat, uint8_t *overflowed);

// Unsigned Integers comparsions
uint8_t _lib_crt_lt(uint64_t x, uint64_t y); // <
uint8_t _lib_crt_gt(uint64_t x, uint64_t y); // >
uint8_t _lib_crt_leq(uint64_t x, uint64_t y); // <=
uint8_t _lib_crt_geq(uint64_t x, uint64_t y); // >=

// Signed Integers comparisons
uint8_t _lib_crt_lti64(int64_t x, int64_t y);
uint8_t _lib_crt_gti64(int64_t x, int64_t y);
uint8_t _lib_crt_leqi64(int64_t x, int64_t y);
uint8_t _lib_crt_geqi64(int64_t x, int64_t y);
uint8_t _lib_crt_ispos(int64_t x);
uint8_t _lib_crt_isneg(int64_t x);

// Type agnostic/general - can be used on both signed and unsigned integers
uint8_t _lib_crt_neq(uint64_t x, uint64_t y); // Non-equivalence
uint8_t _lib_crt_eq(uint64_t x, uint64_t y); // Equivalence
uint64_t _lib_crt_select(uint8_t cond, uint64_t a, uint64_t b); // cmov-equivalent being dispatched
```
However, for normal 64-bit exclusive and below operations (`+, -, *, /, %`), such oeprations are inherently cryptographically-secure, tasking constant-time of operation especially on optimized CPU architectures and compilation modes (eg: `clang -march=native -O2 ... -o ...` should be emit relatively cryptograpphically-secured instructions for such primitive operatins).

---

## 8. Configurable Constants

### NOP Buffer Size (`FAKE_BUF_CAP`)

```c
// Defined in crint.h — modify before building for your deployment target
//
// Controls the size of the stack-allocated fake buffer used in pseudo-NOP paths
// (normalization, memcpy, memset on failure/poison cases).
//
// Sizing:
//   64  limbs =  4096-bit  (RSA-4096 minimum)
//   128 limbs =  8192-bit  (recommended default)
//   256 limbs = 16384-bit  (high-security applications)
//   512 limbs = 32768-bit  (extreme — 4KB stack per function call)
//
// DO NOT use VLAs or runtime-computed sizes. Compile-time constant only.
// Larger values increase per-thread stack pressure:
//   128 limbs × 8 bytes × ~4 fake bufs per function ≈ 4–8 KB per call frame
//   192 threads × 8 KB ≈ 1.5 MB total stack overhead

#define FAKE_BUF_CAP 128
```

---

## 9. Platform Support

| Architecture | ABI                 | Intrinsic File               | Status   |
|--------------|---------------------|------------------------------|----------|
| x86-64       | SysV AMD64 + WIN64  | `_x86_(sysv/win64)_arith.S`  | Full     |
| ARM64        | AAPCS64 + Darwin    | `_arm64_arith.S`             | Full     |
| RISC-V 64    | RV64 psABI (LP64)   | `_rv64_arith.S`              | Full     |
| Other        | N/A                 | `_vanillc_arith.c`           | Fallback |