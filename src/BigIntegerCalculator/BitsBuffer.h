#pragma once

#include <cstdint>
#include <BigInteger.h>
#include "FastReducer.h"


struct BitsBuffer {
public:
    BitsBuffer(int size, uint32_t value);
    BitsBuffer(int size, uint_array value);
    void MultiplySelf(BitsBuffer& value, BitsBuffer& temp);
    void SquareSelf(BitsBuffer& temp);
    void Reduce(FastReducer& reducer);
    void Reduce(uint_array modulus);
    void Reduce(BitsBuffer& modulus);
    void Overwrite(uint64_t value);
    void Overwrite(uint32_t value);
    uint_array GetBits() { return _bits; }
    int GetSize() { return _bits.size(); }
    int GetLength() { return _length; }
    void Refresh(int maxLength);
    void Apply(BitsBuffer& tempt, int maxLength);


private:
    uint_array _bits;
    int _length;
};