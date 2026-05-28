#ifndef dnml_bi_func_hpp
#define dnml_bi_func_hpp

#ifdef __cplusplus
extern "C" {
#endif

#include "bigInt_func.h"
#include <cstdint>
#include <string>
#include <cstring>
#include <stdexcept>
#include <memory>

namespace dnml {

class BigInt {
private:
    bigInt _data;
    static dnml_status check_and_return(dnml_status status) { return status; }
    static void check_status(dnml_status status) {
        if (status != BIGINT_SUCCESS) { throw std::runtime_error("BigInt Operation Failed"); }
    }
public:
    //* ======================================================================== *//
    //* CONSTRUCTORS & DESTRUCTORS
    //* ======================================================================== *//
    BigInt() { check_status(bigInt_new(&_data)); }
    explicit BigInt(size_t fixed_size) { check_status(bigInt_snew(&_data, fixed_size)); }
    BigInt(const BigInt& other) { check_status(bigInt_binew(&_data, &other._data)); }
    BigInt(BigInt&& other) noexcept { _data = other._data; std::memset(&other._data, 0, sizeof(bigInt)); }
    explicit BigInt(uint64_t value) { check_status(bigInt_new_u64(&_data, value)); }
    explicit BigInt(int64_t value) { check_status(bigInt_new_i64(&_data, value)); }
    explicit BigInt(long double value) { check_status(bigInt_new_f128(&_data, value)); }
    explicit BigInt(const std::string& str) { check_status(bigInt_strinit(&_data, str.c_str())); }
    BigInt(const std::string& str, uint8_t base) { check_status(bigInt_strbinit(&_data, str.c_str(), base)); }
    ~BigInt() { bigInt_free(&_data); } //* Destructor

    //* ======================================================================== *//
    //* ASSIGNMENT OPERATORS
    //* ======================================================================== *//
    /* Primitive -> BigInt */
    BigInt& operator=(const BigInt& other) { if (this != &other) { bigInt_set(other._data, &_data); } return *this; }
    BigInt& operator=(BigInt&& other) noexcept { 
        if (this != &other) {
            bigInt_free(&_data); _data = other._data;
            std::memset(&other._data, 0, sizeof(bigInt));
        } return *this;
    }
    BigInt& operator=(uint64_t value) { bigInt_getu64(value, &_data); return *this; }
    BigInt& operator=(int64_t value) { bigInt_geti64(value, &_data); return *this; }
    BigInt& operator=(long double value) { bigInt_getf128(value, &_data); return *this; }
    BigInt& get(long double val) { check_status(bigInt_getf128_safe(val, &_data)); }
    /* BigInt -> Primitive */
    void set(uint64_t* receiver) const { bigInt_setu64(_data, receiver); } // Truncative
    void set(int64_t* receiver) const { bigInt_seti64(_data, receiver); } // Truncative
    void set(long double* receiver) const { bigInt_setf128(_data, receiver); } // Truncative
    void sset(uint64_t* receiver) const { check_status(bigInt_setu64_safe(_data, receiver)); }
    void sset(int64_t* receiver) const { check_status(bigInt_seti64_safe(_data, receiver)); }
    void sset(long double* receiver) const { check_status(bigInt_setf128_safe(_data, receiver)); }


    //* ======================================================================== *//
    //* CONVERSION OPERATORS
    //* ======================================================================== *//
    /* BigInt -> Primitive */
    explicit operator uint64_t() const { return bigInt_tou64(_data); }
    explicit operator int64_t() const { return bigInt_toi64(_data); }
    explicit operator long double() const { return bigInt_tof128(_data); }
    operator uint64_t() const { return bigInt_tou64(_data); }
    operator int64_t() const { return bigInt_toi64(_data); }
    operator long double() const { return bigInt_tof128(_data); }
    // Safe conversions (throw on error)
    uint64_t to_uint64() const { dnml_status err;
        uint64_t result = bigInt_tou64_safe(_data, &err);
        check_status(err); return result;
    }
    int64_t to_int64() const { dnml_status err;
        int64_t result = bigInt_toi64_safe(_data, &err);
        check_status(err); return result;
    }
    long double to_f128() const { dnml_status err;
        long double result = bigInt_tof128_safe(_data, &err);
        check_status(err); return result;
    }


    //* ======================================================================== *//
    //* BITWISE OPERATIONS (FUNCTIONAL - return new BigInt)
    //* ======================================================================== *//
    BigInt operator~() const { dnml_status err;
        bigInt result = bigInt_not(_data, &err);
        check_status(err); BigInt bn;
        bigInt_free(&bn._data); bn._data = result; return bn;
    }
    BigInt operator>>(size_t k) const { dnml_status err;
        bigInt result = bigInt_rshift(_data, k, &err);
        check_status(err); BigInt bn;
        bigInt_free(&bn._data); bn._data = result; return bn;
    }
    BigInt operator<<(size_t k) const { dnml_status err;
        bigInt result = bigInt_lshift(_data, k, &err);
        check_status(err); BigInt bn;
        bigInt_free(&bn._data); bn._data = result; return bn;
    }
    BigInt operator&(const BigInt& other) const { dnml_status err;
        bigInt result = bigInt_and(_data, other._data, &err);
        check_status(err); BigInt bn;
        bigInt_free(&bn._data); bn._data = result; return bn;
    }
    BigInt operator&(uint64_t value) const { dnml_status err;
        bigInt result = bigInt_andu64(_data, value, &err);
        check_status(err); BigInt bn;
        bigInt_free(&bn._data); bn._data = result; return bn;
    }
    BigInt operator|(const BigInt& other) const { dnml_status err;
        bigInt result = bigInt_or(_data, other._data, &err);
        check_status(err); BigInt bn;
        bigInt_free(&bn._data); bn._data = result; return bn;
    }
    BigInt operator|(uint64_t value) const { dnml_status err;
        bigInt result = bigInt_oru64(_data, value, &err);
        check_status(err); BigInt bn;
        bigInt_free(&bn._data); bn._data = result; return bn;
    }
    BigInt operator^(const BigInt& other) const { dnml_status err;
        bigInt result = bigInt_xor(_data, other._data, &err);
        check_status(err); BigInt bn;
        bigInt_free(&bn._data); bn._data = result; return bn;
    }
    BigInt operator^(uint64_t value) const { dnml_status err;
        bigInt result = bigInt_xoru64(_data, value, &err);
        check_status(err); BigInt bn;
        bigInt_free(&bn._data); bn._data = result; return bn;
    }


    //* ======================================================================== *//
    //* BITWISE OPERATIONS (MUTATIVE - in-place modification)
    //* ======================================================================== *//
    BigInt& rshift_mut(size_t k) { bigInt_mut_rshift(&_data, k); return *this; }
    BigInt& lshift_mut(size_t k) { bigInt_mut_lshift(&_data, k); return *this; }
    // Compound assignment operators (mutative)
    BigInt& operator>>=(size_t k) { return rshift_mut(k); }
    BigInt& operator<<=(size_t k) { return lshift_mut(k); }
    // BITWISE AND
    BigInt& and_mut(const BigInt& other) { check_status(bigInt_mut_and(&_data, other._data)); return *this; }
    BigInt& and_mut(uint64_t value) { check_status(bigInt_mut_andu64(&_data, value)); return *this; }
    BigInt& operator&=(const BigInt& other) { return and_mut(other); }
    BigInt& operator&=(uint64_t value) { return and_mut(value); }
    // BITWISE OR
    BigInt& or_mut(const BigInt& other) { check_status(bigInt_mut_or(&_data, other._data)); return *this; }
    BigInt& or_mut(uint64_t value) { check_status(bigInt_mut_oru64(&_data, value)); return *this; }
    BigInt& operator|=(const BigInt& other) { return or_mut(other); }
    BigInt& operator|=(uint64_t value) { return or_mut(value); }
    // BITWISE XOR
    BigInt& xor_mut(const BigInt& other) { check_status(bigInt_mut_xor(&_data, other._data)); return *this; }
    BigInt& xor_mut(uint64_t value) { check_status(bigInt_mut_xoru64(&_data, value)); return *this; }
    BigInt& operator^=(const BigInt& other) { return xor_mut(other); }
    BigInt& operator^=(uint64_t value) { return xor_mut(value); }
    // BITWISE_NAND
    BigInt& nand_mut(const BigInt& other) { check_status(bigInt_mut_nand(&_data, other._data)); return *this; }
    BigInt& nand_mut(uint64_t value) { check_status(bigInt_mut_nandu64(&_data, value)); return *this; }
    // BITWISE NOR
    BigInt& nor_mut(const BigInt& other) { check_status(bigInt_mut_nor(&_data, other._data)); return *this; }
    BigInt& nor_mut(uint64_t value) { check_status(bigInt_mut_noru64(&_data, value)); return *this; }
    // XNOR (mutative)
    BigInt& xnor_mut(const BigInt& other) { check_status(bigInt_mut_xnor(&_data, other._data)); return *this; }
    BigInt& xnor_mut(uint64_t value) { check_status(bigInt_mut_xnoru64(&_data, value)); return *this; }



    // ========================================================================
    // COPY OPERATIONS (FUNCTIONAL)
    // ========================================================================
    // Standard copy (functional)
    BigInt copy() const { dnml_status err;
        bigInt result = bigInt_copy(_data, &err);
        check_status(err); BigInt bn;
        bigInt_free(&bn._data); bn._data = result; return bn;
    }
    // Copy Over (functional)
    BigInt copy_over(size_t output_cap) const { dnml_status err;
        bigInt result = bigInt_ocopy(_data, output_cap, &err);
        check_status(err); BigInt bn;
        bigInt_free(&bn._data); bn._data = result; return bn;
    }
    // Copy Truncative Over (functional)
    BigInt copy_truncover(size_t output_cap) const { dnml_status err;
        bigInt result = bigInt_tover_copy(_data, output_cap, &err);
        check_status(err); BigInt bn;
        bigInt_free(&bn._data); bn._data = result; return bn;
    }

    // ========================================================================
    // COPY OPERATIONS (MUTATIVE)
    // ========================================================================
    BigInt& copy_mut(const BigInt& source) { check_status(bigInt_mut_copy(&_data, source._data)); return *this; }
    BigInt& copy_deep_mut(const BigInt& source) { check_status(bigInt_mut_dcopy(&_data, source._data)); return *this; }
    BigInt& copy_over_mut(const BigInt& source) { check_status(bigInt_mut_ocopy(&_data, source._data)); return *this; }
    void copy_truncover_mut(const BigInt& source) { bigInt_mut_tover_copy(&_data, source._data); }



    // ========================================================================
    // UTILITIES
    // ========================================================================
    void canonicalize() { bigInt_canonicalize(&_data); }
    void normalize() { bigInt_normalize(&_data); }
    void resize(size_t k) { check_status(bigInt_resize(&_data, k)); }  
    void reserve(size_t k) { check_status(bigInt_reserve(&_data, k)); }
    void shrink(size_t k) { check_status(bigInt_shrink(&_data, k)); }
    void reset() { bigInt_reset(&_data); }
    bool validate() const { return bigInt_validate(_data); }
    size_t size() const { return _data.n; }
    size_t capacity() const { return _data.cap; }



    // ========================================================================
    // STRING CONVERSION
    // ========================================================================
    //! THIS IS COMPLEX

    // ========================================================================
    // I/O OPERATIONS
    // ========================================================================
    // Print to stdout
    void print(uint8_t base = 10) const { check_status(bigInt_putb(_data, base)); }
    void fprint(FILE* stream, uint8_t base = 10) const { check_status(bigInt_fputb(stream, _data, base)); }



    // ========================================================================
    // RAW ACCESS (for interop with C functions)
    // ========================================================================
    bigInt* get_data() { return &_data; }
    const bigInt* get_data() const { return &_data; }
};

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

// Create BigInt from primitive (functional style)
inline BigInt make_bigint(uint64_t value) { return BigInt(value); }
inline BigInt make_bigint(int64_t value) { return BigInt(value); }
inline BigInt make_bigint(long double value) { return BigInt(value); }


// ============================================================================
// OPERATOR OVERLOADS (Free Functions for symmetric operations)
// ============================================================================
inline BigInt operator&(uint64_t lhs, const BigInt& rhs) { return rhs & lhs; }
inline BigInt operator|(uint64_t lhs, const BigInt& rhs) { return rhs | lhs; }
inline BigInt operator^(uint64_t lhs, const BigInt& rhs) { return rhs ^ lhs; }


}  // namespace dnml
#ifdef __cplusplus
}
#endif


#endif