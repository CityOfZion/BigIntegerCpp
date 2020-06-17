#include "../../include/FastReducer.h"
#include "../../include/BigIntegerCalculator.h"

FastReducer::FastReducer(uint_array modulus) {
    uint_array r(modulus.size() * 2 + 1, 0);
    r[r.size() - 1] = 1;

    _mu = BigIntegerCalculator::divide(r, modulus);
    _modulus = modulus;

    _q1 = uint_array(modulus.size() * 2 + 2);
    _q2 = uint_array(modulus.size() * 2 + 1);

    _muLength = BigIntegerCalculator::actual_length(_mu);
}

int FastReducer::Reduce(uint_array value, int length) {
    if (length < _modulus.size())
        return length;

    int l1 = DivMul(value, length, _mu, _muLength, _q1, _modulus.size() - 1);
    int l2 = DivMul(_q1, l1, _modulus, _modulus.size(), _q2, _modulus.size() + 1);
    return SubMod(value, length, _q2, l2, _modulus, _modulus.size() + 1);
}

int FastReducer::DivMul(uint_array& left, int leftLength, uint_array& right, int rightLength, uint_array& bits, int k) {
    std::fill(bits.begin(), bits.end(), 0);
    if (leftLength > k) {
        leftLength -= k;

        unsigned int* l = left.data();
        unsigned int* r = right.data();
        unsigned int* b = bits.data();
        if (leftLength < rightLength)
            BigIntegerCalculator::multiply(r, rightLength, l + k, leftLength, b, leftLength + rightLength);
        else
            BigIntegerCalculator::multiply(l + k, leftLength, r, rightLength, b, leftLength + rightLength);

        return BigIntegerCalculator::actual_length(bits, leftLength + rightLength);
    }

    return 0;
}

int FastReducer::SubMod(uint_array left, int leftLength, uint_array right, int rightLength, uint_array modulus, int k) {
    if (leftLength > k)
        leftLength = k;

    if (rightLength > k)
        rightLength = k;

    unsigned int* l = left.data();
    unsigned int* r = right.data();
    unsigned int* m = modulus.data();

    BigIntegerCalculator::subtract_self(l, leftLength, r, rightLength);
    leftLength = BigIntegerCalculator::actual_length(left, leftLength);

    while (BigIntegerCalculator::compare(l, leftLength, m, modulus.size()) >= 0) {
        BigIntegerCalculator::subtract_self(l, leftLength, m, modulus.size());
        leftLength = BigIntegerCalculator::actual_length(left, leftLength);
    }

    std::fill(left.begin(), left.begin()+leftLength, left.size() - leftLength);
    return leftLength;
}
