#pragma once
#include <vector>
#include <cstdint>
#include "BitsBuffer.h"

using uint_array = std::vector<uint32_t>;

class BigIntegerCalculator {
public:
    static uint_array pow(uint32_t value, uint32_t power);
    static uint_array pow(const uint_array& value, uint32_t power);
    static uint_array pow(uint32_t value, uint32_t power, const uint_array& modulus);
    static uint_array pow(const uint_array& value, uint32_t power, const uint_array& modulus);
    static uint_array pow(uint32_t value, const uint_array& power, const uint_array& modulus);
    static uint_array pow(const uint_array& value, const uint_array& power, const uint_array& modulus);

    static uint32_t pow(uint32_t value, uint32_t power, uint32_t modulus);
    static uint32_t pow(const uint_array& value, uint32_t power, uint32_t modulus);
    static uint32_t pow(uint32_t value, const uint_array& power, uint32_t modulus);
    static uint32_t pow(const uint_array& value, const uint_array& power, uint32_t modulus);

    static int pow_bound(uint32_t power, int valueLength, int resultLength);
    static int actual_length(uint_array& value) { return actual_length(value, value.size()); };
    static int actual_length(uint_array& value, int length);

    static uint32_t pow_core(const uint_array& power, uint32_t modulus, uint64_t value, uint64_t result);
    static uint32_t pow_core(uint32_t power, uint32_t modulus, uint64_t value, uint64_t result);

    static uint_array pow_core(uint32_t power, BitsBuffer& value);
    static uint_array pow_core(const uint_array& power, const uint_array& modulus, BitsBuffer& value);
    static uint_array pow_core(uint32_t power, const uint_array& modulus, BitsBuffer& value);
    static void pow_core(uint32_t power, BitsBuffer& value, BitsBuffer& result, BitsBuffer& temp);
    static void pow_core(uint32_t power, const uint_array& modulus, BitsBuffer& value, BitsBuffer& result, BitsBuffer& temp);
    static void pow_core(uint32_t power, FastReducer& reducer, BitsBuffer& value, BitsBuffer& result, BitsBuffer& temp);
    static void pow_core(const uint_array& power, const uint_array& modulus, BitsBuffer& value, BitsBuffer& result, BitsBuffer& temp);
    static void pow_core(const uint_array& power, FastReducer& reducer, BitsBuffer& value, BitsBuffer& result, BitsBuffer& temp);

    static uint_array add(const uint_array& lhs, uint32_t rhs);
    static uint_array add(const uint_array& lhs, const uint_array& rhs);
    static uint_array subtract(const uint_array& lhs, uint32_t rhs);
    static uint_array subtract(const uint_array& lhs, const uint_array& rhs);
    static void subtract_self(uint32_t *left, int leftLength, uint32_t *right, int rightLength);
    static uint_array square(uint_array& value);
    static void square(uint32_t* value, int valueLength, uint32_t* bits, int bitsLength);
    static uint_array multiply(uint_array lhs, uint32_t rhs);
    static uint_array multiply(uint_array lhs, uint_array rhs);
    static void multiply(uint32_t* left, int leftLength, uint32_t* right, int rightLength, uint32_t* bits, int bitsLength);
    static uint_array divide(uint_array& lhs, uint32_t rhs, uint32_t& remainder); // maybe change to a more explicit out parameter
    static uint_array divide(uint_array& lhs, uint32_t rhs);
    static uint32_t remainder(const uint_array& lhs, const uint rhs);
    static uint_array divide(uint_array& lhs, uint_array& rhs, uint_array& remainder);
    static uint_array divide(const uint_array& lhs, const uint_array& rhs);
    static uint_array remainder(const uint_array& lhs, const uint_array& rhs);
    static void divide(uint32_t* lhs, int lhsLength, const uint32_t* rhs, int rhsLength, uint32_t* bits, int bitsLength);
    static uint32_t add_divisor(uint32_t* lhs, int lhsLength, const uint32_t* rhs, int rhsLength);
    static uint32_t subtract_divisor(uint32_t* lhs, int lhsLength, const uint32_t* rhs, int rhsLength, uint64_t q);

    static int compare(uint_array lhs, uint_array rhs);
    static int compare(const uint32_t* lhs, int lhsLength, const uint32_t* rhs, int rhsLength);

    static uint32_t gcd(uint32_t left, uint32_t right);
    static uint64_t gcd(uint64_t left, uint64_t right);
    static uint32_t gcd(uint_array left, uint32_t right);
    static uint_array gcd(uint_array left, uint_array right);
    static void gcd(BitsBuffer& left, BitsBuffer& right);

private:
    static constexpr int AllocationThreshold = 256;
    static constexpr int ReducerThreshold = 32;
    static constexpr int MultiplyThreshold = 32;
    static constexpr int SquareThreshold = 32;

    static void add(const uint32_t* lhs, int lhsLength, const uint32_t* rhs, int rhsLength, uint32_t* bits, int bitsLength);
    static void add_self(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength);
    static void subtract(const uint32_t* lhs, int lhsLength, const uint32_t* rhs, int rhsLength, uint32_t* bits, int bitsLength);
    static void subtract_core(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength, uint32_t* core, int coreLength);
    static bool divide_guess_too_big(uint64_t q, uint64_t valHi, uint32_t valLo, uint32_t divHi, uint32_t divLo);
    static int leading_zeros(uint32_t value);

    static void extract_digits(BitsBuffer& xBuffer, BitsBuffer& yBuffer, uint64_t& x, uint64_t& y);
    static void lehmer_core(BitsBuffer& xBuffer, BitsBuffer& yBuffer, long a, long b, long c, long d);
};

