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

    const static int ReducerThreshold = 32;
};

