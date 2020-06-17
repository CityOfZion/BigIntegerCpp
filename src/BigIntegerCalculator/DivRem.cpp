#include "../../include/BigIntegerCalculator.h"
#include <cassert>
#include "../../include/exceptions.h"

uint_array BigIntegerCalculator::divide(uint_array& lhs, uint32_t rhs, uint32_t& remainder)
{

    assert(!lhs.empty());
    assert(lhs.size() >= 1);

    // Executes the division for one big and one 32-bit integer.
    // Thus, we've similar code than below, but there is no loop for
    // processing the 32-bit integer, since it's a single element.

    uint_array quotient(lhs.size());

    uint64_t carry = 0UL;
    for (int i = lhs.size() - 1; i >= 0; i--)
    {
        uint64_t value = (carry << 32) | lhs[i];
        if (rhs == 0)
            throw DivideByZero();
        uint64_t digit = value / rhs;
        quotient[i] = static_cast<uint32_t>(digit);
        carry = value - digit * rhs;
    }
    remainder = static_cast<uint32_t>(carry);

    return quotient;
}

uint_array BigIntegerCalculator::divide(uint_array& lhs, uint32_t rhs)
{
    assert(!lhs.empty());
    assert(lhs.size() >= 1);

    // Same as above, but only computing the quotient.

    uint_array quotient(lhs.size());

    uint64_t carry = 0UL;
    for (int i = lhs.size() - 1; i >= 0; i--)
    {
        uint64_t value = (carry << 32) | lhs[i];
        if (rhs == 0)
            throw DivideByZero();
        uint64_t digit = value / rhs;
        quotient[i] = static_cast<uint32_t>(digit);
        carry = value - digit * rhs;
    }

    return quotient;
}

uint32_t BigIntegerCalculator::remainder(const uint_array& lhs, const uint rhs)
{
    assert(!lhs.empty());
    assert(lhs.size() >= 1);

    // Same as above, but only computing the remainder.

    uint64_t carry = 0UL;
    for (int i = lhs.size() - 1; i >= 0; i--)
    {
        uint64_t value = (carry << 32) | lhs[i];
        if (rhs == 0)
            throw DivideByZero();
        carry = value % rhs;
    }

    return static_cast<uint32_t>(carry);
}

uint_array BigIntegerCalculator::divide(uint_array& lhs, uint_array& rhs, uint_array& remainder)
{
    assert(!lhs.empty());
    assert(!rhs.empty());
    assert(lhs.size() >= 1);
    assert(rhs.size() >= 1);
    assert(lhs.size() >= rhs.size());

    // NOTE: left will get overwritten, we need a local copy

    uint_array localLeft = uint_array(lhs);
    uint_array bits(lhs.size() - rhs.size() + 1);

    divide(&localLeft[0], localLeft.size(), &rhs[0], rhs.size(), &bits[0], bits.size());

    remainder = localLeft;

    return bits;
}


uint_array BigIntegerCalculator::divide(const uint_array& lhs, const uint_array& rhs)
{
    assert(!lhs.empty());
    assert(!rhs.empty());
    assert(lhs.size() >= 1);
    assert(rhs.size() >= 1);
    assert(lhs.size() >= rhs.size());

    // Same as above, but only returning the quotient.

    // NOTE: left will get overwritten, we need a local copy
    uint_array localLeft = uint_array(lhs);
    uint_array localRight = uint_array(rhs);
    uint_array bits(lhs.size() - rhs.size() + 1);

    divide(&localLeft[0], localLeft.size(), &localRight[0], localRight.size(), &bits[0], bits.size());

    return bits;
}

uint_array BigIntegerCalculator::remainder(const uint_array& lhs, const uint_array& rhs)
{
    assert(!lhs.empty());
    assert(!rhs.empty());
    assert(lhs.size() >= 1);
    assert(rhs.size() >= 1);
    assert(lhs.size() >= rhs.size());

    // Same as above, but only returning the remainder.

    // NOTE: left will get overwritten, we need a local copy
    uint_array localLeft = uint_array(lhs);

    divide(&localLeft[0], localLeft.size(), &rhs[0], rhs.size(), nullptr, 0);

    return localLeft;
}

void BigIntegerCalculator::divide(uint32_t* lhs, int lhsLength, const uint32_t* rhs, int rhsLength, uint32_t* bits, int bitsLength)
{
    assert(lhsLength >= 1);
    assert(rhsLength >= 1);
    assert(lhsLength >= rhsLength);
    assert(bitsLength == lhsLength - rhsLength + 1 || bitsLength == 0);

    // Executes the "grammar-school" algorithm for computing q = a / b.
    // Before calculating q_i, we get more bits into the highest bit
    // block of the divisor. Thus, guessing digits of the quotient
    // will be more precise. Additionally we'll get r = a % b.

    uint32_t divHi = rhs[rhsLength - 1];
    uint32_t divLo = rhsLength > 1 ? rhs[rhsLength - 2] : 0;

    // We measure the leading zeros of the divisor
    int shift = leading_zeros(divHi);
    int backShift = 32 - shift;

    // And, we make sure the most significant bit is set
    if (shift > 0)
    {
        uint divNx = rhsLength > 2 ? rhs[rhsLength - 3] : 0;

        divHi = (divHi << shift) | (divLo >> backShift);
        divLo = (divLo << shift) | (divNx >> backShift);
    }

    // Then, we divide all of the bits as we would do it using
    // pen and paper: guessing the next digit, subtracting, ...
    for (int i = lhsLength; i >= rhsLength; i--)
    {
        int n = i - rhsLength;
        uint32_t t = i < lhsLength ? lhs[i] : 0;

        uint64_t valHi = (static_cast<uint64_t>(t) << 32) | lhs[i - 1];
        uint32_t valLo = i > 1 ? lhs[i - 2] : 0;

        // We shifted the divisor, we shift the dividend too
        if (shift > 0)
        {
            uint32_t valNx = i > 2 ? lhs[i - 3] : 0;

            valHi = (valHi << shift) | (valLo >> backShift);
            valLo = (valLo << shift) | (valNx >> backShift);
        }

        // First guess for the current digit of the quotient,
        // which naturally must have only 32 bits...
        if (divHi == 0)
            throw DivideByZero();
        uint64_t digit = valHi / divHi;
        if (digit > 0xFFFFFFFF)
            digit = 0xFFFFFFFF;

        // Our first guess may be a little bit to big
        while (divide_guess_too_big(digit, valHi, valLo, divHi, divLo))
            --digit;

        if (digit > 0)
        {
            // Now it's time to subtract our current quotient
            uint32_t carry = subtract_divisor(&lhs[n], lhsLength - n, &rhs[0], rhsLength, digit);

            if (carry != t)
            {
                assert(carry == t + 1);

                // Our guess was still exactly one too high
                carry = add_divisor(&lhs[n], lhsLength - n, &rhs[0], rhsLength);
                --digit;

                assert(carry == 1);
            }
        }

        // We have the digit!
        if (bitsLength != 0)
            bits[n] = (uint)digit;
        if (i < lhsLength)
            lhs[i] = 0;
    }
}

uint32_t BigIntegerCalculator::add_divisor(uint32_t* lhs, int lhsLength, const uint32_t* rhs, int rhsLength)
{
    assert(lhsLength >= 0);
    assert(rhsLength >= 0);
    assert(lhsLength >= rhsLength);

    // Repairs the dividend, if the last subtract was too much

    uint64_t carry = 0UL;

    for (int i = 0; i < rhsLength; i++)
    {
        uint64_t digit = (lhs[i] + carry) + rhs[i];
        lhs[i] = static_cast<uint32_t>(digit);
        carry = digit >> 32;
    }

    return static_cast<uint32_t>(carry);
}

uint32_t BigIntegerCalculator::subtract_divisor(uint32_t* lhs, int lhsLength, const uint32_t* rhs, int rhsLength, uint64_t q)
{
    assert(lhsLength >= 0);
    assert(rhsLength >= 0);
    assert(lhsLength >= rhsLength);
    assert(q <= 0xFFFFFFFF);

    // Combines a subtract and a multiply operation, which is naturally
    // more efficient than multiplying and then subtracting...

    uint64_t carry = 0UL;

    for (int i = 0; i < rhsLength; i++)
    {
        carry += rhs[i] * q;
        uint32_t digit = static_cast<uint32_t>(carry);
        carry = carry >> 32;
        if (lhs[i] < digit)
            ++carry;
        lhs[i] = lhs[i] - digit;
    }

    return static_cast<uint32_t>(carry);
}

bool BigIntegerCalculator::divide_guess_too_big(uint64_t q, uint64_t valHi, uint32_t valLo, uint32_t divHi, uint32_t divLo)
{
    assert(q <= 0xFFFFFFFF);

    // We multiply the two most significant limbs of the divisor
    // with the current guess for the quotient. If those are bigger
    // than the three most significant limbs of the current dividend
    // we return true, which means the current guess is still too big.

    uint64_t chkHi = divHi * q;
    uint64_t chkLo = divLo * q;

    chkHi = chkHi + (chkLo >> 32);
    chkLo = chkLo & 0xFFFFFFFF;

    if (chkHi < valHi)
        return false;
    if (chkHi > valHi)
        return true;

    if (chkLo < valLo)
        return false;
    if (chkLo > valLo)
        return true;

    return false;
}

int BigIntegerCalculator::leading_zeros(uint32_t value)
{
    if (value == 0)
        return 32;

    int count = 0;
    if ((value & 0xFFFF0000) == 0)
    {
        count += 16;
        value = value << 16;
    }
    if ((value & 0xFF000000) == 0)
    {
        count += 8;
        value = value << 8;
    }
    if ((value & 0xF0000000) == 0)
    {
        count += 4;
        value = value << 4;
    }
    if ((value & 0xC0000000) == 0)
    {
        count += 2;
        value = value << 2;
    }
    if ((value & 0x80000000) == 0)
    {
        count += 1;
    }

    return count;
}