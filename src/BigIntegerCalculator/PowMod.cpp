#include <stdexcept>
#include "BigIntegerCalculator.h"
#include "BitsBuffer.h"

uint_array BigIntegerCalculator::Pow(uint32_t value, uint32_t power) {
    int size = PowBound(power, 1, 1);
    BitsBuffer v(size, value);
    return PowCore(power, v);
}

uint_array BigIntegerCalculator::Pow(uint_array& value, uint32_t power) {
    int size = PowBound(power, value.size(), 1);
    BitsBuffer v(size, value);
    return PowCore(power, v);
}

uint_array BigIntegerCalculator::PowCore(uint32_t power, BitsBuffer& value) {
    int size = value.GetSize();
    auto temp = BitsBuffer(size, 0);
    auto result = BitsBuffer(size, 1);

    PowCore(power, value, result, temp);

    return result.GetBits();
}

int BigIntegerCalculator::PowBound(uint32_t power, int valueLength, int resultLength) {
    while (power != 0) {
        if ((power & 1) == 1) {
            if (valueLength < INT32_MAX - resultLength) {
                resultLength += valueLength;
            } else {
                throw std::overflow_error("checked() int overflow");
            }
        }

        if (power != 1) {
            if (valueLength < INT32_MAX - valueLength) {
                valueLength += valueLength;
            } else {
                throw std::overflow_error("checked() int overflow");
            }
        }

        power = power >> 1;
    }
    return resultLength;
}

void BigIntegerCalculator::PowCore(uint32_t power, BitsBuffer& value, BitsBuffer& result, BitsBuffer& temp) {
    while (power != 0) {
        if ((power & 1) == 1)
            result.MultiplySelf(value, temp);
        if (power != 1)
            value.SquareSelf(temp);
        power = power >> 1;
    }
}

int BigIntegerCalculator::ActualLength(uint_array& value, int length) {
    while (length > 0 && value[length - 1] == 0)
        --length;
    return length;
}

uint32_t BigIntegerCalculator::PowCore(uint32_t power, uint32_t modulus, uint64_t value, uint64_t result) {
    while (power != 0) {
        if ((power & 1) == 1)
            result = (result * value) % modulus;
        if (power != 1)
            value = (value * value) % modulus;
        power = power >> 1;
    }
    return static_cast<uint32_t>(result % modulus);
}

uint32_t BigIntegerCalculator::Pow(uint_array& value, uint32_t power, uint32_t modulus) {
    uint32_t v = Remainder(value, modulus);
    return PowCore(power, modulus, v, 1);
}

uint32_t BigIntegerCalculator::PowCore(uint_array& power, uint32_t modulus, uint64_t value, uint64_t result) {
    for (auto& p : power) {
        for (int j = 0; j < 32; j++) {
            if ((p & 1) == 1)
                result = (result * value) % modulus;
            value = (value * value) % modulus;
            p = p >> 1;
        }
    }
    return PowCore(power[power.size()-1], modulus, value, result);
}

uint32_t BigIntegerCalculator::Pow(uint_array& value, uint_array& power, uint32_t modulus) {
    auto v = Remainder(value, modulus);
    return PowCore(power, modulus, v, 1);
}

uint_array BigIntegerCalculator::Pow(uint32_t value, uint32_t power, uint_array& modulus) {
    int size = modulus.size() + modulus.size();
    BitsBuffer v(size, value);
    return PowCore(power, modulus, v);
}

uint_array BigIntegerCalculator::PowCore(uint32_t power, uint_array& modulus, BitsBuffer& value) {
    int size = value.GetSize();

    BitsBuffer temp(size, 0);
    BitsBuffer result(size, 1);

    if (modulus.size() < ReducerThreshold) {
        PowCore(power, modulus, value, result, temp);
    } else {
      FastReducer reducer(modulus);
      PowCore(power, reducer, value, result, temp);
    }
    return result.GetBits();
}

uint_array BigIntegerCalculator::Pow(uint_array& value, uint32_t power, uint_array& modulus) {
    if (value.size() > modulus.size())
        value = Remainder(value, modulus);

    int size = modulus.size() + modulus.size();
    BitsBuffer v(size, value);
    return PowCore(power, modulus, v);
}

void BigIntegerCalculator::PowCore(uint_array& power, uint_array& modulus, BitsBuffer& value, BitsBuffer& result,
                                   BitsBuffer& temp) {
    for(auto& p : power) {
        for (int j = 0; j < 32; j++) {
            if ((p & 1) == 1) {
                result.MultiplySelf(value, temp);
                result.Reduce(modulus);
            }
            value.SquareSelf(temp);
            value.Reduce(modulus);
            p = p >> 1;
        }
    }
    PowCore(power[power.size()-1], modulus, value, result, temp);
}

void BigIntegerCalculator::PowCore(uint32_t power, uint_array& modulus, BitsBuffer& value, BitsBuffer& result,
                                   BitsBuffer& temp) {
    while (power != 0) {
        if ((power & 1) == 1) {
            result.MultiplySelf(value, temp);
            result.Reduce(modulus);
        }
        if (power != 1) {
            value.SquareSelf(temp);
            value.Reduce(modulus);
        }
        power = power >> 1;
    }
}

uint_array BigIntegerCalculator::Pow(uint32_t value, uint_array& power, uint_array& modulus) {
    int size = modulus.size() + modulus.size();
    BitsBuffer v(size, value);
    return PowCore(power, modulus, v);
}

uint_array BigIntegerCalculator::Pow(uint_array& value, uint_array& power, uint_array& modulus) {
    if (value.size() > modulus.size())
        value = Remainder(value, modulus);

    int size = modulus.size() + modulus.size();
    BitsBuffer v(size, value);
    return PowCore(power, modulus, v);
}

uint_array BigIntegerCalculator::PowCore(uint_array& power, uint_array& modulus, BitsBuffer& value) {
    int size = value.GetSize();

    BitsBuffer temp(size, 0);
    BitsBuffer result(size, 1);

    if (modulus.size() < ReducerThreshold) {
        PowCore(power, modulus, value, result, temp);
    } else {
        FastReducer reducer(modulus);
        PowCore(power, reducer, value, result, temp);
    }
    return result.GetBits();
}

void BigIntegerCalculator::PowCore(uint_array& power, FastReducer& reducer, BitsBuffer& value, BitsBuffer& result,
                                   BitsBuffer& temp) {
    for (auto& p : power) {
        for (int j = 0; j < 32; j++) {
            if ((p & 1) == 1) {
                result.MultiplySelf(value, temp);
                result.Reduce(reducer);
            }
            value.SquareSelf(temp);
            value.Reduce(reducer);
            p = p >> 1;
        }
    }
    PowCore(power[power.size()-1], reducer, value, result, temp);
}

void BigIntegerCalculator::PowCore(uint32_t power, FastReducer& reducer, BitsBuffer& value, BitsBuffer& result,
                                   BitsBuffer& temp) {
    while (power != 0) {
        if ((power & 1) == 1) {
            result.MultiplySelf(value, temp);
            result.Reduce(reducer);
        }
        if (power != 1) {
            value.SquareSelf(temp);
            value.Reduce(reducer);
        }
        power = power >> 1;
    }
}

uint32_t BigIntegerCalculator::Pow(uint32_t value, uint32_t power, uint32_t modulus) {
    return PowCore(power, modulus, value, 1);
}

uint32_t BigIntegerCalculator::Pow(uint32_t value, uint_array& power, uint32_t modulus) {
    return PowCore(power, modulus, value, 1);
}
