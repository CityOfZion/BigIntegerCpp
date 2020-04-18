#pragma once
#include <vector>
#include <cstdint>
#include "BitsBuffer.h"

using uint_array = std::vector<uint32_t>;

class BigIntegerCalculator {
public:
    static uint_array Pow(uint32_t value, uint32_t power);
    static uint_array Pow(uint_array& value, uint32_t power);
    static uint_array Pow(uint32_t value, uint32_t power, uint_array& modulus);
    static uint_array Pow(uint_array& value, uint32_t power, uint_array& modulus);
    static uint_array Pow(uint32_t value, uint_array& power, uint_array& modulus);
    static uint_array Pow(uint_array& value, uint_array& power, uint_array& modulus);

    static uint32_t Pow(uint32_t value, uint32_t power, uint32_t modulus);
    static uint32_t Pow(uint_array& value, uint32_t power, uint32_t modulus);
    static uint32_t Pow(uint32_t value, uint_array& power, uint32_t modulus);
    static uint32_t Pow(uint_array& value, uint_array& power, uint32_t modulus);

    static int PowBound(uint32_t power, int valueLength, int resultLength);
    static int ActualLength(uint_array& value) { return ActualLength(value, value.size()); };
    static int ActualLength(uint_array& value, int length);

    static uint32_t PowCore(uint_array& power, uint32_t modulus, unsigned long value, unsigned long result);
    static uint32_t PowCore(uint32_t power, uint32_t modulus, unsigned long value, unsigned long result);

    static uint_array PowCore(uint32_t power, BitsBuffer& value);
    static uint_array PowCore(uint_array& power, uint_array& modulus, BitsBuffer& value);
    static uint_array PowCore(uint32_t power, uint_array& modulus, BitsBuffer& value);
    static void PowCore(uint32_t power, BitsBuffer& value, BitsBuffer& result, BitsBuffer& temp);
    static void PowCore(uint32_t power, uint_array& modulus, BitsBuffer& value, BitsBuffer& result, BitsBuffer& temp);
    static void PowCore(uint32_t power, FastReducer& reducer, BitsBuffer& value, BitsBuffer& result, BitsBuffer& temp);
    static void PowCore(uint_array& power, uint_array& modulus, BitsBuffer& value, BitsBuffer& result, BitsBuffer& temp);
    static void PowCore(uint_array& power, FastReducer& reducer, BitsBuffer& value, BitsBuffer& result, BitsBuffer& temp);

    static uint_array Add(uint_array& lhs, uint32_t rhs);
    static uint_array Add(uint_array& lhs, uint_array& rhs);
    static uint_array Subtract(uint_array& lhs, uint32_t rhs);
    static uint_array Subtract(uint_array& lhs, uint_array& rhs);
    static void SubtractSelf(uint32_t *left, int leftLength, uint32_t *right, int rightLength);
    static uint_array Square(uint_array& value);
    static void Square(uint32_t* value, int valueLength, uint32_t* bits, int bitsLength);
    static uint_array Multiply(uint_array lhs, uint32_t rhs);
    static uint_array Multiply(uint_array lhs, uint_array rhs);
    static void Multiply(uint32_t* left, int leftLength, uint32_t* right, int rightLength, uint32_t* bits, int bitsLength);
    static uint_array Divide(uint_array& lhs, uint32_t rhs, uint32_t& remainder); // maybe change to a more explicit out parameter
    static uint_array Divide(uint_array& lhs, uint32_t rhs);
    static uint32_t Remainder(uint_array& lhs, uint rhs);
    static uint_array Divide(uint_array& lhs, uint_array& rhs, uint_array& remainder);
    static uint_array Divide(uint_array& lhs, uint_array& rhs);
    static uint_array Remainder(uint_array& lhs, uint_array& rhs);
    static void Divide(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength, uint32_t* bits, int bitsLength);
    static uint32_t AddDivisor(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength);
    static uint32_t SubtractDivisor(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength, uint64_t q);

    static int Compare(uint_array lhs, uint_array rhs);
    static int Compare(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength);

    static uint32_t Gcd(uint32_t left, uint32_t right);
    static unsigned long Gcd(unsigned long left, unsigned long right);
    static uint32_t Gcd(uint_array left, uint32_t right);
    static uint_array Gcd(uint_array left, uint_array right);
    static void Gcd(BitsBuffer& left, BitsBuffer& right);

private:
    static constexpr int AllocationThreshold = 256;
    static constexpr int ReducerThreshold = 32;
    static constexpr int MultiplyThreshold = 32;
    static constexpr int SquareThreshold = 32;

    static void Add(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength, uint32_t* bits, int bitsLength);
    static void AddSelf(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength);
    static void Subtract(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength, uint32_t* bits, int bitsLength);
    static void SubtractCore(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength, uint32_t* core, int coreLength);
    static bool DivideGuessTooBig(uint64_t q, uint64_t valHi, uint32_t valLo, uint32_t divHi, uint32_t divLo);
    static int LeadingZeros(uint32_t value);

    static void ExtractDigits(BitsBuffer& xBuffer, BitsBuffer& yBuffer,unsigned long& x, unsigned long& y);
    static void LehmerCore(BitsBuffer& xBuffer, BitsBuffer& yBuffer, long a, long b, long c, long d);


};

