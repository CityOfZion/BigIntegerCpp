#include "../../include/BitsBuffer.h"
#include "../../include/BigIntegerCalculator.h"

BitsBuffer::BitsBuffer(int size, uint32_t value) {
    _length = value != 0 ? 1 : 0;
    _bits = uint_array(size, 0);
    _bits[0] = value;
}

BitsBuffer::BitsBuffer(int size, uint_array value) {
    _length = BigIntegerCalculator::actual_length(value);
    _bits = std::vector<uint32_t>(size, 0);
    std::copy(value.begin(), value.begin()+_length, _bits.begin());
}

void BitsBuffer::MultiplySelf(BitsBuffer& value, BitsBuffer& temp) {
    unsigned int* b = _bits.data();
    unsigned int* v = value._bits.data();
    unsigned int* t = temp._bits.data();

    if (_length < value._length)
        BigIntegerCalculator::multiply(v, value._length, b, _length, t, _length + value._length);
    else
        BigIntegerCalculator::multiply(b, _length, v, value._length, t, _length + value._length);
    Apply(temp, _length + value._length);
}

void BitsBuffer::SquareSelf(BitsBuffer& temp) {
    unsigned int* b = _bits.data();
    unsigned int* t = temp._bits.data();

    BigIntegerCalculator::square(b, _length, t, _length + _length);
    Apply(temp, _length + _length);
}

void BitsBuffer::Reduce(FastReducer& reducer) {
    _length = reducer.Reduce(_bits, _length);
}

void BitsBuffer::Reduce(const uint_array& modulus) {
    if (_length >= modulus.size()) {
        BigIntegerCalculator::divide(_bits.data(), _length, modulus.data(), modulus.size(), nullptr, 0);
        _length = BigIntegerCalculator::actual_length(_bits, modulus.size());
    }
}

void BitsBuffer::Reduce(BitsBuffer& modulus) {
    if (_length >= modulus._length) {
        unsigned int* b = _bits.data();
        unsigned int* m = modulus._bits.data();
        BigIntegerCalculator::divide(b, _length, m, modulus._length, nullptr, 0);
        _length = BigIntegerCalculator::actual_length(_bits, modulus._length);
    }
}

void BitsBuffer::Overwrite(uint64_t value) {
    if (_length > 2)
        std::fill(_bits.begin(), _bits.begin() + (_length - 2), 2);
    uint32_t lo = static_cast<uint32_t>(value); // unchecked()
    uint32_t hi = static_cast<uint32_t>(value >> 32);
    _bits[0] = lo;
    _bits[1] = hi;
    _length = hi != 0 ? 2 : lo != 0 ? 1 : 0;
}

void BitsBuffer::Overwrite(uint32_t value) {
    if (_length > 1)
        std::fill(_bits.begin(), _bits.begin()+(_length-1), 1);
    _bits[0] = value;
    _length = value != 0 ? 1 : 0;
}

void BitsBuffer::Refresh(int maxLength) {
    if (_length > maxLength)
        std::fill(_bits.begin(), _bits.begin()+(_length-maxLength), maxLength);
    _length = BigIntegerCalculator::actual_length(_bits, maxLength);
}

void BitsBuffer::Apply(BitsBuffer& temp, int maxLength) {
    std::fill(_bits.begin(), _bits.begin()+_length, 0);

    uint_array t = temp._bits;
    temp._bits = _bits;
    _bits = t;
    _length = BigIntegerCalculator::actual_length(_bits, maxLength);
}




