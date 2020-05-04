#include "BigIntegerCalculator.h"
#include <cassert>

uint_array BigIntegerCalculator::square(uint_array& value)
{
    assert(!value.empty());

    // Switching to unsafe pointers helps sparing
    // some nasty index calculations...

    uint_array bits(value.size()*2);
    square(&value[0], value.size(), &bits[0], bits.size());

    return bits;
}

void BigIntegerCalculator::square(uint32_t* value, int valueLength, uint32_t* bits, int bitsLength)
{
    assert(valueLength >= 0);
    assert(bitsLength == valueLength + valueLength);

    // Executes different algorithms for computing z = a * a
    // based on the actual length of a. If a is "small" enough
    // we stick to the classic "grammar-school" method; for the
    // rest we switch to implementations with less complexity
    // albeit more overhead (which needs to pay off!).

    // NOTE: useful thresholds needs some "empirical" testing,
    // which are smaller in DEBUG mode for testing purpose.

    if (valueLength < SquareThreshold)
    {
        // Squares the bits using the "grammar-school" method.
        // Envisioning the "rhombus" of a pen-and-paper calculation
        // we see that computing z_i+j += a_j * a_i can be optimized
        // since a_j * a_i = a_i * a_j (we're squaring after all!).
        // Thus, we directly get z_i+j += 2 * a_j * a_i + c.

        // ATTENTION: an ordinary multiplication is safe, because
        // z_i+j + a_j * a_i + c <= 2(2^32 - 1) + (2^32 - 1)^2 =
        // = 2^64 - 1 (which perfectly matches with ulong!). But
        // here we would need an UInt65... Hence, we split these
        // operation and do some extra shifts.

        for (int i = 0; i < valueLength; i++)
        {
            uint64_t carry = 0UL;
            for (int j = 0; j < i; j++)
            {
                uint64_t digit1 = bits[i + j] + carry;
                uint64_t digit2 = static_cast<uint64_t>(value[j]) * value[i];
                bits[i + j] = static_cast<uint64_t>(digit1 + (digit2 << 1));
                carry = (digit2 + (digit1 >> 1)) >> 31;
            }
            uint64_t digits = static_cast<uint64_t>(value[i]) * value[i] + carry;
            bits[i + i] = static_cast<uint32_t>(digits);
            bits[i + i + 1] = static_cast<uint32_t>(digits >> 32);
        }
    }
    else
    {
        // Based on the Toom-Cook multiplication we split value
        // into two smaller values, doing recursive squaring.
        // The special form of this multiplication, where we
        // split both operands into two operands, is also known
        // as the Karatsuba algorithm...

        // https://en.wikipedia.org/wiki/Toom-Cook_multiplication
        // https://en.wikipedia.org/wiki/Karatsuba_algorithm

        // Say we want to compute z = a * a ...

        // ... we need to determine our new length (just the half)
        int n = valueLength >> 1;
        int n2 = n << 1;

        // ... split value like a = (a_1 << n) + a_0
        uint32_t* valueLow = value;
        int valueLowLength = n;
        uint32_t* valueHigh = value + n;
        int valueHighLength = valueLength - n;

        // ... prepare our result array (to reuse its memory)
        uint32_t* bitsLow = bits;
        int bitsLowLength = n2;
        uint32_t* bitsHigh = bits + n2;
        int bitsHighLength = bitsLength - n2;

        // ... compute z_0 = a_0 * a_0 (squaring again!)
        square(valueLow, valueLowLength,
               bitsLow, bitsLowLength);

        // ... compute z_2 = a_1 * a_1 (squaring again!)
        square(valueHigh, valueHighLength,
               bitsHigh, bitsHighLength);

        int foldLength = valueHighLength + 1;
        int coreLength = foldLength + foldLength;

        if (coreLength < AllocationThreshold)
        {
            uint_array fold(foldLength, 0);
            uint_array core(coreLength, 0);

            // ... compute z_a = a_1 + a_0 (call it fold...)
            add(valueHigh, valueHighLength,
                valueLow, valueLowLength,
                fold.data(), foldLength);

            // ... compute z_1 = z_a * z_a - z_0 - z_2
            square(fold.data(), foldLength,
                   core.data(), coreLength);

            subtract_core(bitsHigh, bitsHighLength,
                          bitsLow, bitsLowLength,
                          core.data(), coreLength);

            // ... and finally merge the result! :-)
            add_self(&bits[n], bitsLength - n, &core[0], coreLength);
        }
        else
        {
            uint_array fold(foldLength, 0);
            uint_array core(coreLength, 0);
            // ... compute z_a = a_1 + a_0 (call it fold...)
            add(valueHigh, valueHighLength,
                valueLow, valueLowLength,
                fold.data(), foldLength);

            // ... compute z_1 = z_a * z_a - z_0 - z_2
            square(fold.data(), foldLength, core.data(), coreLength);

            subtract_core(bitsHigh, bitsHighLength,
                          bitsLow, bitsLowLength,
                          core.data(), coreLength);

            // ... and finally merge the result! :-)
            add_self(&bits[n], bitsLength - n, &core[0], coreLength);
        }
    }
}

uint_array BigIntegerCalculator::multiply(uint_array lhs, uint32_t rhs)
{
    assert(!lhs.empty());

    // Executes the multiplication for one big and one 32-bit integer.
    // Since every step holds the already slightly familiar equation
    // a_i * b + c <= 2^32 - 1 + (2^32 - 1)^2 < 2^64 - 1
    // we are safe regarding to overflows.

    size_t i = 0;
    uint64_t carry = 0UL;
    uint_array bits(lhs.size() + 1);

    for (; i < lhs.size(); i++)
    {
        uint64_t digits = static_cast<uint64_t>(lhs[i]) * rhs + carry;
        bits[i] = static_cast<uint32_t>(digits);
        carry = digits >> 32;
    }

    bits[i] = static_cast<uint32_t>(carry);

    return bits;
}

uint_array BigIntegerCalculator::multiply(uint_array lhs, uint_array rhs)
{
    assert(!lhs.empty());
    assert(!rhs.empty());
    assert(lhs.size() >= rhs.size());

    // Switching to unsafe pointers helps sparing
    // some nasty index calculations...

    uint_array bits(lhs.size() + rhs.size());

    multiply(&lhs[0], lhs.size(), &rhs[0], rhs.size(), &bits[0], bits.size());

    return bits;
}

void BigIntegerCalculator::multiply(uint32_t* left, int leftLength, uint32_t* right, int rightLength, uint32_t* bits, int bitsLength)
{
    assert(leftLength >= 0);
    assert(rightLength >= 0);
    assert(leftLength >= rightLength);
    assert(bitsLength == leftLength + rightLength);

    // Executes different algorithms for computing z = a * b
    // based on the actual length of b. If b is "small" enough
    // we stick to the classic "grammar-school" method; for the
    // rest we switch to implementations with less complexity
    // albeit more overhead (which needs to pay off!).

    // NOTE: useful thresholds needs some "empirical" testing,
    // which are smaller in DEBUG mode for testing purpose.

    if (rightLength < MultiplyThreshold)
    {
        // Multiplies the bits using the "grammar-school" method.
        // Envisioning the "rhombus" of a pen-and-paper calculation
        // should help getting the idea of these two loops...
        // The inner multiplication operations are safe, because
        // z_i+j + a_j * b_i + c <= 2(2^32 - 1) + (2^32 - 1)^2 =
        // = 2^64 - 1 (which perfectly matches with ulong!).

        for (int i = 0; i < rightLength; i++)
        {
            uint64_t carry = 0UL;
            for (int j = 0; j < leftLength; j++)
            {
                uint64_t digits = bits[i + j] + carry + static_cast<uint64_t>(left[j]) * right[i];
                bits[i + j] = static_cast<uint32_t>(digits);
                carry = digits >> 32;
            }
            bits[i + leftLength] = static_cast<uint32_t>(carry);
        }
    }
    else
    {
        // Based on the Toom-Cook multiplication we split left/right
        // into two smaller values, doing recursive multiplication.
        // The special form of this multiplication, where we
        // split both operands into two operands, is also known
        // as the Karatsuba algorithm...

        // https://en.wikipedia.org/wiki/Toom-Cook_multiplication
        // https://en.wikipedia.org/wiki/Karatsuba_algorithm

        // Say we want to compute z = a * b ...

        // ... we need to determine our new length (just the half)
        int n = rightLength >> 1;
        int n2 = n << 1;

        // ... split left like a = (a_1 << n) + a_0
        uint32_t* leftLow = left;
        int leftLowLength = n;
        uint32_t* leftHigh = left + n;
        int leftHighLength = leftLength - n;

        // ... split right like b = (b_1 << n) + b_0
        uint32_t* rightLow = right;
        int rightLowLength = n;
        uint32_t* rightHigh = right + n;
        int rightHighLength = rightLength - n;

        // ... prepare our result array (to reuse its memory)
        uint32_t* bitsLow = bits;
        int bitsLowLength = n2;
        uint32_t* bitsHigh = bits + n2;
        int bitsHighLength = bitsLength - n2;

        // ... compute z_0 = a_0 * b_0 (multiply again)
        multiply(leftLow, leftLowLength,
                 rightLow, rightLowLength,
                 bitsLow, bitsLowLength);

        // ... compute z_2 = a_1 * b_1 (multiply again)
        multiply(leftHigh, leftHighLength,
                 rightHigh, rightHighLength,
                 bitsHigh, bitsHighLength);

        int leftFoldLength = leftHighLength + 1;
        int rightFoldLength = rightHighLength + 1;
        int coreLength = leftFoldLength + rightFoldLength;

        if (coreLength < AllocationThreshold)
        {
            uint_array leftFold(leftFoldLength, 0);
            uint_array rightFold(rightFoldLength, 0);
            uint_array core(coreLength, 0);

            // ... compute z_a = a_1 + a_0 (call it fold...)
            BigIntegerCalculator::add(leftHigh, leftHighLength,
                                      leftLow, leftLowLength,
                                      leftFold.data(), leftFoldLength);

            // ... compute z_b = b_1 + b_0 (call it fold...)
            BigIntegerCalculator::add(rightHigh, rightHighLength,
                                      rightLow, rightLowLength,
                                      rightFold.data(), rightFoldLength);

            // ... compute z_1 = z_a * z_b - z_0 - z_2
            multiply(leftFold.data(), leftFoldLength,
                     rightFold.data(), rightFoldLength,
                     core.data(), coreLength);

            subtract_core(bitsHigh, bitsHighLength,
                          bitsLow, bitsLowLength,
                          core.data(), coreLength);

            // ... and finally merge the result! :-)
            add_self(&bits[n], bitsLength - n, &core[0], coreLength);
        }
        else
        {
            uint_array leftFold(leftFoldLength, 0);
            uint_array rightFold(rightFoldLength, 0);
            uint_array core(coreLength, 0);

            // ... compute z_a = a_1 + a_0 (call it fold...)
            add(leftHigh, leftHighLength,
                leftLow, leftLowLength,
                leftFold.data(), leftFoldLength);

            // ... compute z_b = b_1 + b_0 (call it fold...)
            add(rightHigh, rightHighLength,
                rightLow, rightLowLength,
                rightFold.data(), rightFoldLength);

            // ... compute z_1 = z_a * z_b - z_0 - z_2
            multiply(leftFold.data(), leftFoldLength,
                     rightFold.data(), rightFoldLength,
                     core.data(), coreLength);
            subtract_core(bitsHigh, bitsHighLength,
                          bitsLow, bitsLowLength,
                          core.data(), coreLength);

            // ... and finally merge the result! :-)
            add_self(&bits[n], bitsLength - n, &core[0], coreLength);
        }
    }
}

void BigIntegerCalculator::subtract_core(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength, uint32_t* core, int coreLength)
{

    assert(lhsLength>= 0);
    assert(rhsLength >= 0);
    assert(coreLength >= 0);
    assert(lhsLength >= rhsLength);
    assert(coreLength >= lhsLength);

    // Executes a special subtraction algorithm for the multiplication
    // which needs to subtract two different values from a core value
    // while core is always bigger than the sum of these values.

    // NOTE: we could do an ordinary subtraction of course, but we spare
    // one "run", if we do this computation within a single one...

    int i = 0;
    long carry = 0L;

    for (; i < rhsLength; i++)
    {
        long digit = (core[i] + carry) - lhs[i] - rhs[i];
        core[i] = static_cast<uint32_t>(digit);
        carry = digit >> 32;
    }

    for (; i < lhsLength; i++)
    {
        long digit = (core[i] + carry) - lhs[i];
        core[i] = static_cast<uint32_t>(digit);
        carry = digit >> 32;
    }

    for (; carry != 0 && i < coreLength; i++)
    {
        long digit = core[i] + carry;
        core[i] = (uint)digit;
        carry = digit >> 32;
    }
}



