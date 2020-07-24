#include "../../include/BigIntegerCalculator.h"
#include <cassert>

uint_array BigIntegerCalculator::add(const uint_array& lhs, uint32_t rhs) {
    assert(!lhs.empty());
    assert(lhs.size() >= 1);

    // Executes the addition for one big and one 32-bit integer.
    // Thus, we've similar code than below, but there is no loop for
    // processing the 32-bit integer, since it's a single element.

    uint_array bits(lhs.size() + 1);

    long long digit = static_cast<long long>(lhs[0]) + rhs;
    bits[0] = static_cast<uint32_t>(digit);
    long carry = digit >> 32;

    for (size_t i = 1; i < lhs.size(); i++) {
        digit = lhs[i] + carry;
        bits[i] = static_cast<uint32_t>(digit);
        carry = digit >> 32;
    }
    bits[lhs.size()] = static_cast<uint32_t>(carry);

    return bits;
}

uint_array BigIntegerCalculator::add(const uint_array& lhs, const uint_array& rhs) {
    assert(!lhs.empty());
    assert(!rhs.empty());
    assert(lhs.size() >= rhs.size());

    uint_array bits(lhs.size() + 1);

    add(lhs.data(), lhs.size(), rhs.data(), rhs.size(), bits.data(), bits.size());

    return bits;
}

void BigIntegerCalculator::add(const uint32_t* lhs, int lhsLength, const uint32_t* rhs, int rhsLength, uint32_t* bits,
                               int bitsLength) {
    assert(lhsLength >= 0);
    assert(rhsLength >= 0);
    assert(lhsLength >= rhsLength);
    assert(bitsLength == lhsLength + 1);

    int i = 0;
    uint64_t carry = 0L;

    for (; i < rhsLength; i++) {
        uint64_t digit = (lhs[i] + carry) + rhs[i];
        bits[i] = static_cast<uint32_t>(digit);
        carry = digit >> 32;
    }

    for (; i < lhsLength; i++) {
        uint64_t digit = lhs[i] + carry;
        bits[i] = static_cast<uint32_t>(digit);
        carry = digit >> 32;
    }

    bits[i] = static_cast<uint32_t>(carry);
}

void BigIntegerCalculator::add_self(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength) {
    assert(*lhs >= 0);
    assert(*rhs >= 0);
    assert(lhsLength >= rhsLength);

    // Executes the "grammar-school" algorithm for computing z = a + b.
    // Same as above, but we're writing the result directly to a and
    // stop execution, if we're out of b and c is already 0.

    int i = 0;
    long carry = 0L;

    for (; i < rhsLength; i++) {
        long digit = lhs[i] + carry + rhs[i];
        lhs[i] = static_cast<uint32_t>(digit);
        carry = digit >> 32;
    }
    for (; carry != 0 && i < lhsLength; i++) {
        long digit = lhs[i] + carry;
        lhs[i] = static_cast<unsigned int>(digit);
        carry = digit >> 32;
    }

    assert(carry == 0);
}

uint_array BigIntegerCalculator::subtract(const uint_array& lhs, uint32_t rhs) {
    assert(!lhs.empty());
    assert(lhs.size() >= 1);
    assert(lhs[0] >= rhs || lhs.size() >= 2);

    // Executes the subtraction for one big and one 32-bit integer.
    // Thus, we've similar code than below, but there is no loop for
    // processing the 32-bit integer, since it's a single element.

    uint_array bits(lhs.size());

    long digit = (long) lhs[0] - rhs;
    bits[0] = static_cast<uint32_t>(digit);
    long carry = digit >> 32;

    for (size_t i = 1; i < lhs.size(); i++) {
        digit = lhs[i] + carry;
        bits[i] = static_cast<uint32_t>(digit);
        carry = digit >> 32;
    }

    return bits;
}

uint_array BigIntegerCalculator::subtract(const uint_array& lhs, const uint_array& rhs) {
    assert(!lhs.empty());
    assert(!rhs.empty());
    assert(lhs.size() >= rhs.size());
    assert(compare(lhs, rhs) >= 0);

    uint_array bits(lhs.size());

    subtract(lhs.data(), lhs.size(), rhs.data(), rhs.size(), bits.data(), bits.size());

    return bits;
}

void
BigIntegerCalculator::subtract(const uint32_t* lhs, int lhsLength, const uint32_t* rhs, int rhsLength, uint32_t* bits,
                               int bitsLength) {
    assert(lhsLength >= 0);
    assert(rhsLength >= 0);
    assert(lhsLength >= rhsLength);
    assert(compare(lhs, lhsLength, rhs, rhsLength) >= 0);
    assert(bitsLength == lhsLength);

    // Executes the "grammar-school" algorithm for computing z = a - b.
    // While calculating z_i = a_i - b_i we take care of overflow
    // Since a_i - b_i doesn't need any additional bit, our carry c
    // has always the value -1 or 0; hence, we're safe here.

    int i = 0;
    long carry = 0L;

    for (; i < rhsLength; i++) {
        long digit = lhs[i] + carry - rhs[i];
        bits[i] = static_cast<uint32_t>(digit);
        carry = digit >> 32;
    }
    for (; i < lhsLength; i++) {
        long digit = lhs[i] + carry;
        bits[i] = static_cast<uint32_t>(digit);
        carry = digit >> 32;
    }

    assert(carry == 0);
}

void BigIntegerCalculator::subtract_self(uint32_t* left, int leftLength, uint32_t* right, int rightLength) {
    int i = 0;
    long carry = 0;

    for (; i < rightLength; i++) {
        long digit = (left[i] + carry) - right[i];
        left[i] = static_cast<uint32_t>(digit); // unchecked()
        carry = digit >> 32;
    }
    for (; carry != 0 && i < leftLength; i++) {
        long digit = left[i] + carry;
        left[i] = static_cast<uint32_t>(digit);
        carry = digit >> 32;
    }
}

int BigIntegerCalculator::compare(uint_array lhs, uint_array rhs) {
    assert(!lhs.empty());
    assert(!rhs.empty());

    if (lhs.size() < rhs.size())
        return -1;
    if (lhs.size() > rhs.size())
        return 1;

    for (int i = lhs.size() - 1; i >= 0; i--) {
        if (lhs[i] < rhs[i])
            return -1;
        if (lhs[i] > rhs[i])
            return 1;
    }
    return 0;
}

int BigIntegerCalculator::compare(const uint32_t* lhs, int lhsLength, const uint32_t* rhs, int rhsLength) {
    assert(lhsLength >= 0);
    assert(rhsLength >= 0);

    if (lhsLength < rhsLength)
        return -1;
    if (lhsLength > rhsLength)
        return 1;

    for (int i = lhsLength - 1; i >= 0; i--) {
        if (lhs[i] < rhs[i])
            return -1;
        if (lhs[i] > rhs[i])
            return 1;
    }

    return 0;
}