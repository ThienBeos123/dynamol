# libdnml API Design

> This document specifies the public API design, ownership semantics, configurable constants, and usage patterns
> for libdnml. It is intended for both **library users** and **contributors** implementing new functionality.

---

## Table of Contents
1. [Module Overview](#1-module-overview)
2. [Type Definitions](#2-type-definitions)
3. [Status Codes](#3-status-codes)
4. [Function Categories](#4-function-categories)
5. [Ownership Semantics](#5-ownership-semantics)
6. [Poisoned State](#6-poisoned-state)
7. [Configurable Constants](#7-configurable-constants)
8. [Thread Safety](#8-thread-safety)
9. [Platform Support](#9-platform-support)
10. [Usage Examples](#10-usage-examples)

---

## 1. Module Overview

| Module  | Type      | Header      | Purpose                               |
|---------|-----------|-------------|---------------------------------------|
| Dynamol | `bigInt`  | `bigInt.h`  | Performance-optimized arithmetic      |
| Drypto  | `crint`   | `crint.h`   | Cryptographically-secured arithmetic  |

> **Rule:** Never use `bigInt` in cryptographic code paths. `crint` and `bigInt` must not be mixed
> in any operation that handles secret or sensitive data.

---

## 2. Type Definitions

### `crint` (Drypto)

```c
typedef struct {
    limb_t  *limbs;     // Heap-allocated limb array (little-endian, index 0 = LSL)
    size_t   n;         // Active limb count (canonical: limbs[n-1] != 0 when n > 0)
    size_t   cap;       // Allocated capacity in limbs
    int8_t   sign;      // 1 = positive/zero, -1 = negative
    uint8_t  poisoned;  // Non-zero: struct is unusable, all ops become no-ops
} crint;
```

### Validity States

| State               | `limbs`    | `n`      | `cap`   | `sign`       | `poisoned` |
|---------------------|------------|----------|---------|--------------|------------|
| Valid, non-zero     | non-NULL   | > 0      | >= n    | 1 or -1      | 0          |
| Valid, zero         | non-NULL   | 0        | >= 1    | 1            | 0          |
| Poisoned            | non-NULL   | any      | >= 1    | any          | non-zero   |
| Invalid (assert)    | NULL       | any      | any     | any          | any        |

Please detail the full [contact](/drypto/CRYPT_NUM_CONTRACT.txt) .txt file for more detail

---

## 3. Status Codes

```c
typedef enum {
    CRINT_SUCCESS = 200,
    CRINT_POISON,            // 201
    CRINT_ERR_RANGE,         // 202
    CRINT_ERR_DOMAIN,        // 203
    CRINT_NULL,              // 204
    CRINT_ERR_INVAL,         // 205
    CRINT_ERR_SINVAL,        // 206
} dnml_status;
```

> **Rule:** Status codes must never be ignored. Always propagate via `CHOOSE_OPTION` in internal code,
> or check explicitly in user code.

---

## 4. Function Categories

### 4.1 Constructors

Allocate and initialize a `crint`. Caller owns the result and must call `crint_free`.

```c
// Allocate empty crint (n = 0, cap = 1)
dnml_status  crint_new(crint *x);

// Allocate with explicit capacity
dnml_status  crint_snew(crint *x, size_t cap);

// Construct from primitives
dnml_status  crint_fromu64(crint *x, uint64_t val);
dnml_status  crint_fromi64(crint *x, int64_t val);
dnml_status  crint_fromf128(crint *x, long double val);
dnml_status  crint_fromstr(crint *x, const char *str, uint8_t base);
```

### 4.2 Destructors

```c
// Zeroize and free limbs, clear metadata
void         crint_free(crint *x);

// Reset value to zero without freeing buffer
void         crint_reset(crint *x);
```

### 4.3 Mutative Functions (`crint_mut_*`)

Modify `x` in-place. `x` owns the result on return.
Pass-by-value operands (`crint y`) are **consumed** by the function.

```c
dnml_status  crint_mut_add(crint *x, crint y);          // x += y  (y consumed)
dnml_status  crint_mut_sub(crint *x, crint y);          // x -= y  (y consumed)
dnml_status  crint_mut_mul(crint *x, crint y);          // x *= y  (y consumed)
dnml_status  crint_mut_mulu64(crint *x, uint64_t val);  // x *= val
dnml_status  crint_mut_lshift(crint *x, size_t k);      // x <<= k  (truncative)
dnml_status  crint_mut_lshiftg(crint *x, size_t k);     // x <<= k  (expanding)
dnml_status  crint_mut_rshift(crint *x, size_t k);      // x >>= k
dnml_status  crint_mut_not(crint *x);                   // x = ~x
dnml_status  crint_mut_and(crint *x, crint y);          // x &= y  (y consumed)
dnml_status  crint_mut_or(crint *x, crint y);           // x |= y  (y consumed)
dnml_status  crint_mut_xor(crint *x, crint y);          // x ^= y  (y consumed)
```

### 4.4 Functional Functions (`crint_*`)

Return a newly allocated `crint`. Caller owns the result and must call `crint_free`.
Pass-by-pointer operands (`crint `) are **borrowed** (not consumed).

```c
crint        crint_add(crint a, crint b, dnml_status *err);
crint        crint_sub(crint a, crint b, dnml_status *err);
crint        crint_mul(crint a, crint b, dnml_status *err);
crint        crint_not(crint x, dnml_status *err);          // x consumed
crint        crint_lshift(crint x, size_t k, dnml_status *err);
crint        crint_lshiftg(crint x, size_t k, dnml_status *err);
crint        crint_rshift(crint x, size_t k, dnml_status *err);
crint        crint_and(crint a, crint b, dnml_status *err);
crint        crint_or(crint a, crint b, dnml_status *err);
crint        crint_xor(crint a, crint b, dnml_status *err);
```

### 4.5 Assignment Functions (`crint_set_*`)

Write the value of a `crint` into a primitive-type variable via pointer.
Matches the C API pattern: `bigInt_setu64(x, &receiver)`.

```c
// Unsafe (truncative)
void         crint_set_u64(crint x, uint64_t *dst);
void         crint_set_i64(crint x, int64_t *dst);
void         crint_set_f128(crint x, long double *dst);

// Safe (returns error on overflow)
dnml_status  crint_set_u64_safe(crint x, uint64_t *dst);
dnml_status  crint_set_i64_safe(crint x, int64_t *dst);
dnml_status  crint_set_f128_safe(crint x, long double *dst);
```

### 4.6 Conversion Functions (`crint_to_*`)

Return the primitive value directly. No allocation.

```c
uint64_t     crint_to_u64(crint x);
int64_t      crint_to_i64(crint x);
long double  crint_to_f128(crint x);
```

Modular reductions to primitives (no allocation, no OOM possible):

```c
dnml_status  crint_mod_u64(crint x, uint64_t mod, uint64_t *result);
dnml_status  crint_mod_i64(crint x, int64_t mod, int64_t *result);
```

### 4.7 Comparison Functions

All comparison functions are **constant-time** in Drypto.
No early exits on operand value.

```c
int8_t       crint_cmp(crint a, crint b);     // -1, 0, or 1
uint8_t      crint_eq(crint a, crint b);      // 1 if a == b
uint8_t      crint_lt(crint a, crint b);      // 1 if a < b
uint8_t      crint_gt(crint a, crint b);      // 1 if a > b
uint8_t      crint_lte(crint a, crint b);     // 1 if a <= b
uint8_t      crint_gte(crint a, crint b);     // 1 if a >= b
uint8_t      crint_is_zero(crint x);
uint8_t      crint_is_one(crint x);
uint8_t      crint_is_even(crint x);
```

### 4.8 Utility Functions

```c
// Ownership transfer (O(1), src becomes invalid after call)
dnml_status  crint_transfer(crint *dst, crint *src);

// Deep copy (O(n), dst is independently owned)
dnml_status  crint_copy(crint *dst, crint src);

// Validation
uint8_t      crint_validate(crint x);           // Full arithmetic validity
uint8_t      crint_pvalidate(crint x);   // Pointer + metadata validity
uint8_t      crint_svalidate(crint x);   // Storage validity only

// Capacity management
dnml_status  crint_reserve(crint *x, size_t cap);
```

---

## 5. Ownership Semantics

### Pass-by-Value (`crint y`)
The function **takes ownership** of `y`. The caller must not use `y` after the call,
and must not call `crint_free` on it.

```c
crint a, b;
crint_fromu64(&a, 100);
crint_fromu64(&b, 200);

crint_mut_add(&a, b);   // b is consumed here
// DO NOT: crint_free(&b);  <-- double-free, undefined behavior
// DO NOT: use b after this point
```

### Pass-by-Pointer (`crint y`)
The function **borrows** `y`. The caller retains ownership and must call `crint_free` themselves.

```c
crint a, b, c;
dnml_status err;
crint_fromu64(&a, 100);
crint_fromu64(&b, 200);

c = crint_add(&a, &b, &err);   // a and b are borrowed, c is newly owned

crint_free(&a);                 // Caller still owns a and b
crint_free(&b);
crint_free(&c);                 // Caller owns c
```

### Ownership After Transfer
`crint_transfer` moves ownership from `src` to `dst` in O(1).
`src` is invalidated (limbs = NULL, n = 0) after transfer.

```c
crint a, b;
crint_fromu64(&a, 999);
crint_snew(&b, 4);

crint_transfer(&b, &a);   // b now owns a's buffer, a is invalidated

// DO NOT: crint_free(&a); <-- a->limbs is NULL, nop but misleading
crint_free(&b);           // b owns the buffer, free it here
```

---

## 6. Poisoned State

A `crint` becomes poisoned when:
- An arithmetic operation fails (OOM, division by zero)
- A poisoned `crint` is used as any operand

### Behavior of Poisoned Inputs
- All operations become **no-ops** (constant-time fake operations on stack buffers)
- The result is always poisoned
- No sensitive data in any operand is read or written
- The poisoned struct must be freed with `crint_free` (safe on poisoned state)

```c
crint a, b;
dnml_status err;

crint_fromu64(&a, 42);
crint_snew(&b, 0);       // OOM or invalid, b is poisoned

crint c = crint_add(&a, &b, &err);

// c is always poisoned if b is poisoned
assert(c.poisoned != 0);
assert(err == CRINT_POISON);

crint_free(&a);
crint_free(&b);
crint_free(&c);           // Always safe on poisoned crints
```

### Checking Poison State

```c
if (result.poisoned) {
    crint_free(&result);
    return handle_error();
}
```

---

## 7. Configurable Constants

### Security: NOP Buffer Size

```c
// crint.h (modify before building)
//
// Size of the stack-allocated fake buffer used for constant-time NOP operations
// when ret_stat != CRINT_SUCCESS (OOM, poisoned, etc.)
//
// Sizing guidance:
//   64  limbs =  4096-bit (RSA-4096 minimum coverage)
//   128 limbs =  8192-bit (recommended default)
//   256 limbs = 16384-bit (high-security / post-quantum adjacent)
//   512 limbs = 32768-bit (extreme; 4KB stack per thread)
//
// WARNING: Do NOT use VLAs or runtime-computed sizes here.
//          Compile-time constant is mandatory for constant-time guarantees.
//
// WARNING: Larger values increase stack usage per thread:
//          128 limbs × 8 bytes × N fake buffers ≈ 1–8 KB per function call
//          On servers with 192 threads, 128 limbs adds ~1.5 MB total stack usage.

#define CRINT_FAKE_BUF_CAP 128
```

### Algorithm Dispatch Thresholds: Drypto

```c
// Division: limb count of divisor at which algorithm switches
#define CRINT_SHORT_DIV    1     // b->n == 1: short division (single limb)
#define CRINT_NEWTON       64    // b->n >= 64: Newton-Raphson reciprocal
                                 // 1 <= b->n < 64: Barrett reduction
```

### Algorithm Dispatch Thresholds: Dynamol

```c
// Multiplication: limb count of the larger operand
#define BIGINT_SCHOOLBOOK  8     // n < 8:    Schoolbook O(n^2)
#define BIGINT_KARATSUBA   32    // n < 32:   Karatsuba  O(n^1.585)
#define BIGINT_TOOM3       128   // n < 128:  Toom-Cook 3
#define BIGINT_NTT         512   // n >= 128: NTT/FFT

// Division: limb count of divisor
#define BIGINT_SHORT       1     // b->n == 1: short division
#define BIGINT_KNUTH       32    // b->n < 32:  Knuth-D
#define BIGINT_BURNIKEL    128   // b->n < 128: Burnikel-Ziegler
                                 // b->n >= 128: Newton reciprocal
```

---

## 8. Thread Safety

| Operation                  | Thread Safe | Notes                                |
|----------------------------|-------------|--------------------------------------|
| Read-only (`crint_* `)     | Yes         | No shared mutable state              |
| Mutative (`crint_mut_*`)   | No          | Caller must synchronize              |
| Heap allocation            | Yes         | libc `malloc`/`free` are thread-safe |
| `crint_transfer`           | No          | Mutates both src and dst             |

> **Recommendation:** Each thread should own its own `crint`/`bigInt` instances.
> Do not share mutable state across threads without external synchronization.

---

## 9. Platform Support

| Architecture | ABI         | Intrinsic File        | Status   |
|--------------|-------------|-----------------------|----------|
| x86-64       | SysV AMD64  | `_x86_sysv_arith.S`   | Full     |
| ARM64        | AAPCS64     | `_arm64_arith.S`      | Full     |
| RISC-V 64    | RV64 psABI  | `_rv64_arith.S`       | Full     |
| Other        | N/A         | `_vanillc_arith.c`    | Fallback |

The portable fallback (`_vanillc_arith.c`) uses no inline assembly and no compiler extensions,
relying only on C99 standard arithmetic.

---

## 10. Usage Examples

### Basic Arithmetic

```c
#include <crint.h>

int main(void) {
    dnml_status err;
    crint a, b, c;

    // Construction
    crint_fromu64(&a, 0xDEADBEEFCAFEBABEULL);
    crint_fromu64(&b, 0x123456789ABCDEF0ULL);

    // Functional: returns new crint, a and b borrowed
    c = crint_add(&a, &b, &err);
    if (err != CRINT_SUCCESS || c.poisoned) {
        crint_free(&a);
        crint_free(&b);
        crint_free(&c);
        return 1;
    }

    // Mutative: modifies a in-place, b is consumed
    err = crint_mut_mul(&a, b);
    if (err != CRINT_SUCCESS) {
        crint_free(&a);
        crint_free(&c);
        return 1;
    }
    // b must NOT be freed here (consumed by crint_mut_mul)

    // Extraction
    uint64_t result;
    err = crint_set_u64_safe(&c, &result);
    if (err != CRINT_SUCCESS) {
        crint_free(&a);
        crint_free(&c);
        return 1;
    }

    // Cleanup
    crint_free(&a);
    crint_free(&c);
    return 0;
}
```

### RSA Modular Exponentiation

```c
#include <crint.h>

// Decrypt RSA ciphertext: plaintext = ciphertext^private_key mod modulus
// All constant-time via Drypto.
dnml_status rsa_decrypt(
    crint       *plaintext,
    crint ciphertext,
    crint private_key,
    crint modulus
) {
    dnml_status err;

    *plaintext = crint_modexp(ciphertext, private_key, modulus, &err);

    if (err != CRINT_SUCCESS || plaintext->poisoned) {
        crint_free(plaintext);
        return err;
    }

    return CRINT_SUCCESS;
}
```

### ECDSA Scalar Multiplication

```c
#include <crint.h>

// Compute k*G on elliptic curve (constant-time, scalar k is secret)
dnml_status ecdsa_scalar_mult(
    ec_point    *result,
    crint scalar_k,
    const ec_point *base_point
) {
    // scalar_k is secret: Drypto guarantees no timing leakage
    dnml_status err = ec_point_scalar_mul(result, scalar_k, base_point, &err);
    if (err != CRINT_SUCCESS) {
        return err;
    }
    return CRINT_SUCCESS;
}
```

### Error Propagation Pattern

```c
// Standard pattern for chained operations
dnml_status compute_result(crint *out, crint a, crint b) {
    dnml_status err;
    crint tmp;

    tmp = crint_mul(a, b, &err);
    if (err != CRINT_SUCCESS) return err;

    err = crint_mut_lshift(&tmp, 3);
    if (err != CRINT_SUCCESS) {
        crint_free(&tmp);
        return err;
    }

    crint_transfer(out, &tmp);    // O(1), tmp invalidated
    return CRINT_SUCCESS;
}
```