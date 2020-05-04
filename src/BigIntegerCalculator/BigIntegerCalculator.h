#pragma once
#include <vector>
#include <cstdint>
#include "BitsBuffer.h"

using uint_array = std::vector<uint32_t>;

class BigIntegerCalculator {
public:
    static uint_array Pow(uint32_t value, uint32_t power);
    static uint_array Pow(const uint_array& value, uint32_t power);
    static uint_array Pow(uint32_t value, uint32_t power, const uint_array& modulus);
    static uint_array Pow(const uint_array& value, uint32_t power, const uint_array& modulus);
    static uint_array Pow(uint32_t value, const uint_array& power, const uint_array& modulus);
    static uint_array Pow(const uint_array& value, const uint_array& power, const uint_array& modulus);

    static uint32_t Pow(uint32_t value, uint32_t power, uint32_t modulus);
    static uint32_t Pow(const uint_array& value, uint32_t power, uint32_t modulus);
    static uint32_t Pow(uint32_t value, const uint_array& power, uint32_t modulus);
    static uint32_t Pow(const uint_array& value, const uint_array& power, uint32_t modulus);

    static int PowBound(uint32_t power, int valueLength, int resultLength);
    static int ActualLength(uint_array& value) { return ActualLength(value, value.size()); };
    static int ActualLength(uint_array& value, int length);

    static uint32_t PowCore(const uint_array& power, uint32_t modulus, uint64_t value, uint64_t result);
    static uint32_t PowCore(uint32_t power, uint32_t modulus, uint64_t value, uint64_t result);

    static uint_array PowCore(uint32_t power, BitsBuffer& value);
    static uint_array PowCore(const uint_array& power, const uint_array& modulus, BitsBuffer& value);
    static uint_array PowCore(uint32_t power, const uint_array& modulus, BitsBuffer& value);
    static void PowCore(uint32_t power, BitsBuffer& value, BitsBuffer& result, BitsBuffer& temp);
    static void PowCore(uint32_t power, const uint_array& modulus, BitsBuffer& value, BitsBuffer& result, BitsBuffer& temp);
    static void PowCore(uint32_t power, FastReducer& reducer, BitsBuffer& value, BitsBuffer& result, BitsBuffer& temp);
    static void PowCore(const uint_array& power, const uint_array& modulus, BitsBuffer& value, BitsBuffer& result, BitsBuffer& temp);
    static void PowCore(const uint_array& power, FastReducer& reducer, BitsBuffer& value, BitsBuffer& result, BitsBuffer& temp);

    static uint_array Add(const uint_array& lhs, uint32_t rhs);
    static uint_array Add(const uint_array& lhs, const uint_array& rhs);
    static uint_array Subtract(const uint_array& lhs, uint32_t rhs);
    static uint_array Subtract(const uint_array& lhs, const uint_array& rhs);
    static void SubtractSelf(uint32_t *left, int leftLength, uint32_t *right, int rightLength);
    static uint_array Square(uint_array& value);
    static void Square(uint32_t* value, int valueLength, uint32_t* bits, int bitsLength);
    static uint_array Multiply(uint_array lhs, uint32_t rhs);
    static uint_array Multiply(uint_array lhs, uint_array rhs);
    static void Multiply(uint32_t* left, int leftLength, uint32_t* right, int rightLength, uint32_t* bits, int bitsLength);
    static uint_array Divide(uint_array& lhs, uint32_t rhs, uint32_t& remainder); // maybe change to a more explicit out parameter
    static uint_array Divide(uint_array& lhs, uint32_t rhs);
    static uint32_t Remainder(const uint_array& lhs, const uint rhs);
    static uint_array Divide(uint_array& lhs, uint_array& rhs, uint_array& remainder);
    static uint_array Divide(const uint_array& lhs, const uint_array& rhs);
    static uint_array Remainder(const uint_array& lhs, const uint_array& rhs);
    static void Divide(uint32_t* lhs, int lhsLength, const uint32_t* rhs, int rhsLength, uint32_t* bits, int bitsLength);
    static uint32_t AddDivisor(uint32_t* lhs, int lhsLength, const uint32_t* rhs, int rhsLength);
    static uint32_t SubtractDivisor(uint32_t* lhs, int lhsLength, const uint32_t* rhs, int rhsLength, uint64_t q);

    static int Compare(uint_array lhs, uint_array rhs);
    static int Compare(const uint32_t* lhs, int lhsLength, const uint32_t* rhs, int rhsLength);

    static uint32_t Gcd(uint32_t left, uint32_t right);
    static uint64_t Gcd(uint64_t left, uint64_t right);
    static uint32_t Gcd(uint_array left, uint32_t right);
    static uint_array Gcd(uint_array left, uint_array right);
    static void Gcd(BitsBuffer& left, BitsBuffer& right);

private:
    static constexpr int AllocationThreshold = 256;
    static constexpr int ReducerThreshold = 32;
    static constexpr int MultiplyThreshold = 32;
    static constexpr int SquareThreshold = 32;

    static void Add(const uint32_t* lhs, int lhsLength, const uint32_t* rhs, int rhsLength, uint32_t* bits, int bitsLength);
    static void AddSelf(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength);
    static void Subtract(const uint32_t* lhs, int lhsLength, const uint32_t* rhs, int rhsLength, uint32_t* bits, int bitsLength);
    static void SubtractCore(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength, uint32_t* core, int coreLength);
    static bool DivideGuessTooBig(uint64_t q, uint64_t valHi, uint32_t valLo, uint32_t divHi, uint32_t divLo);
    static int LeadingZeros(uint32_t value);

    static void ExtractDigits(BitsBuffer& xBuffer, BitsBuffer& yBuffer,uint64_t& x, uint64_t& y);
    static void LehmerCore(BitsBuffer& xBuffer, BitsBuffer& yBuffer, long a, long b, long c, long d);


};

