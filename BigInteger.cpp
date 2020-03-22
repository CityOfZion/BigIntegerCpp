#include "BigInteger.h"
#include "BigNumber.h"

#include <limits>
#include <cassert>
#include <cstdlib>
#include <any>


//using tcb::span; // currently unused, not sure if we "need" a span

const BigInteger BigInteger::s_bnMinInt = BigInteger(-1, uint_array {kuMaskHighBit});
const BigInteger BigInteger::s_bnOneInt = BigInteger(1);
const BigInteger BigInteger::s_bnZeroInt = BigInteger(0);
const BigInteger BigInteger::s_bnMinusOneInt = BigInteger(-1);

BigInteger::BigInteger(int value)
{

	if (value == std::numeric_limits<int>::min())
	{
		*this = s_bnMinInt;
	}
	else
	{
		_sign = value;
		_bits = uint_array();

	}
	AssertValid();
}

BigInteger::BigInteger(int n, uint_array value) {
     _sign = n;
    _bits = value;
    AssertValid();
}


BigInteger::BigInteger(uint32_t value) {
    if (value <= std::numeric_limits<uint32_t>::min())
    {
        _sign = (int)value;
        _bits = uint_array();
    }
    else
    {
        _sign = +1;
        _bits = uint_array();
        _bits[0] = value;
    }
    AssertValid();
}

BigInteger::BigInteger(long value) {
    if (std::numeric_limits<int>::min() < value && value <= std::numeric_limits<int>::max())
    {
        _sign = (int)value;
        _bits = uint_array();
    }
    else if (value == std::numeric_limits<int>::min())
    {
        *this = s_bnMinInt;
    }
    else
    {
        uint64_t x = 0;
        if (value < 0)
        {
            x = static_cast<uint64_t>(-value);
            _sign = -1;
        }
        else
        {
            x = static_cast<uint64_t>(value);
            _sign = +1;
        }

        if (x <= std::numeric_limits<uint32_t >::max())
        {
            _bits = uint_array();
            _bits[0] = (uint)x;
        }
        else
        {
            _bits = uint_array();
            _bits[0] = (uint32_t)x;
            _bits[1] = (uint32_t)(x >> kcbitUint);
        }
    }

    //AssertValid();
}

BigInteger::BigInteger(double value) {

    AssertValid();
    // TODO
}

BigInteger Add(BigInteger lhs, BigInteger rhs)
{
    lhs.AssertValid();
    return lhs + rhs;
}

BigInteger Subtract(BigInteger lhs, BigInteger rhs)
{
    return lhs - rhs;
}

BigInteger Add(uint_array& lhs, int lhsSign, uint_array& rhs, int rhsSign)
{
    bool trivialLeft = lhs.empty();
    bool trivialRight = rhs.empty();

    if (trivialLeft && trivialRight)
    {
        return (long)lhsSign + rhsSign;
    }

    if (trivialLeft)
    {
        assert(!rhs.empty());
        uint_array bits = BigInteger::Add(rhs, abs(lhsSign));
        return BigInteger(bits, lhsSign < 0);
    }

    if (trivialRight)
    {
        assert(!lhs.empty());
        uint_array bits = BigInteger::Add(lhs, abs(rhsSign));
        return BigInteger(bits, lhsSign < 0);
    }

    assert(!lhs.empty() && !rhs.empty());

    if (lhs.size() < rhs.size())
    {
        uint_array bits = BigInteger::Add(rhs, lhs);
        return BigInteger(bits, lhsSign < 0);
    }
    else
    {
        uint_array bits = BigInteger::Add(lhs, rhs);
        return BigInteger(bits, lhsSign < 0);
    }
}

BigInteger Subtract(uint_array& lhs, int lhsSign, uint_array& rhs, int rhsSign)
{
    bool trivialLeft = lhs.empty();
    bool trivialRight = rhs.empty();

    if (trivialLeft && trivialRight)
    {
        return (long)lhsSign - rhsSign;
    }

    if (trivialLeft)
    {
        assert(!rhs.empty());
        uint_array bits = BigInteger::Subtract(rhs, abs(lhsSign));
        return BigInteger(bits, lhsSign >= 0);
    }

    if (trivialRight)
    {
        assert(!lhs.empty());
        uint_array bits = BigInteger::Subtract(lhs, abs(rhsSign));
        return BigInteger(bits, lhsSign < 0);
    }

    assert(!lhs.empty() && rhs.empty());

    if (BigInteger::Compare(lhs, rhs) < 0)
    {
        uint_array bits = BigInteger::Subtract(rhs, lhs);
        return BigInteger(bits, lhsSign >= 0);
    }
    else
    {
        uint_array bits = BigInteger::Subtract(lhs, rhs);
        return BigInteger(bits, lhsSign < 0);
    }
}

BigInteger operator -(BigInteger& lhs, BigInteger& rhs)
{
    lhs.AssertValid();
    rhs.AssertValid();

    if ((lhs._sign < 0) != (rhs._sign < 0))
    {
        return Subtract(lhs._bits, lhs._sign, rhs._bits, -1 * rhs._sign);
    }
    return Add(lhs._bits, lhs._sign, rhs._bits, rhs._sign);
}

BigInteger operator +(BigInteger& lhs, BigInteger& rhs)
{
    lhs.AssertValid();
    rhs.AssertValid();

    if ((lhs._sign < 0) != (rhs._sign < 0))
    {
        return Subtract(lhs._bits, lhs._sign, rhs._bits, -1 * rhs._sign);
    }
    return Add(lhs._bits, lhs._sign, rhs._bits, rhs._sign);
}

BigInteger operator *(BigInteger& lhs, BigInteger& rhs)
{
    lhs.AssertValid();
    rhs.AssertValid();

    bool trivialLeft = lhs._bits.empty();
    bool trivialRight = rhs._bits.empty();

    if (trivialLeft && trivialRight)
    {
        return (long)lhs._sign * rhs._sign;
    }

    if (trivialLeft)
    {
        assert(!rhs._bits.empty());
        uint_array bits = BigInteger::Multiply(rhs._bits, abs(lhs._sign));
        return BigInteger(bits, (lhs._sign < 0) ^ (rhs._sign < 0));
    }

    if (trivialRight)
    {
        assert(!lhs._bits.empty());
        uint_array bits = BigInteger::Multiply(lhs._bits, abs(rhs._sign));
        return BigInteger(bits, (lhs._sign < 0) ^ (rhs._sign < 0));
    }

    assert(!lhs._bits.empty() && !rhs._bits.empty());

    if (lhs._bits == rhs._bits)
    {
        uint_array bits = BigInteger::Square(lhs._bits);
        return BigInteger(bits, (lhs._sign < 0) ^ (rhs._sign < 0));
    }

    if (lhs._bits.size() < rhs._bits.size())
    {
        uint_array bits = BigInteger::Multiply(rhs._bits, lhs._bits);
        return BigInteger(bits, (lhs._sign < 0) ^ (rhs._sign < 0));
    }
    else
    {
        uint_array bits = BigInteger::Multiply(lhs._bits, rhs._bits);
        return BigInteger(bits, (lhs._sign < 0) ^ (rhs._sign < 0));
    }
}

static void Multiply(uint32_t* left, int leftLength, uint32_t* right, int rightLength, uint32_t* bits, int bitsLength)
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

    if (rightLength < BigInteger::MultiplyThreshold)
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
        Multiply(leftLow, leftLowLength,
                 rightLow, rightLowLength,
                 bitsLow, bitsLowLength);

        // ... compute z_2 = a_1 * b_1 (multiply again)
        Multiply(leftHigh, leftHighLength,
                 rightHigh, rightHighLength,
                 bitsHigh, bitsHighLength);

        int leftFoldLength = leftHighLength + 1;
        int rightFoldLength = rightHighLength + 1;
        int coreLength = leftFoldLength + rightFoldLength;

        if (coreLength < BigInteger::AllocationThreshold)
        {
            uint32_t* leftFold = new uint32_t[leftFoldLength];
            //new Span<uint>(leftFold, leftFoldLength).Clear();
            uint32_t* rightFold = new uint32_t[rightFoldLength];
            //new Span<uint>(rightFold, rightFoldLength).Clear();
            uint32_t* core = new uint32_t[coreLength];
            //new Span<uint>(core, coreLength).Clear();

            // ... compute z_a = a_1 + a_0 (call it fold...)
            BigInteger::Add(leftHigh, leftHighLength,
                leftLow, leftLowLength,
                leftFold, leftFoldLength);

            // ... compute z_b = b_1 + b_0 (call it fold...)
            BigInteger::Add(rightHigh, rightHighLength,
                rightLow, rightLowLength,
                rightFold, rightFoldLength);

            // ... compute z_1 = z_a * z_b - z_0 - z_2
            BigInteger::Multiply(leftFold, leftFoldLength,
                     rightFold, rightFoldLength,
                     core, coreLength);

            BigInteger::SubtractCore(bitsHigh, bitsHighLength,
                         bitsLow, bitsLowLength,
                         core, coreLength);

            // ... and finally merge the result! :-)
            BigInteger::AddSelf(&bits[n], bitsLength - n, &core[0], coreLength);
        }
        else
        {
            uint32_t* leftFold = new uint32_t[leftFoldLength];
            uint32_t* rightFold = new uint32_t[rightFoldLength];
            uint32_t* core = new uint32_t[coreLength];
            // ... compute z_a = a_1 + a_0 (call it fold...)
            BigInteger::Add(leftHigh, leftHighLength,
                leftLow, leftLowLength,
                leftFold, leftFoldLength);

            // ... compute z_b = b_1 + b_0 (call it fold...)
            BigInteger::Add(rightHigh, rightHighLength,
                rightLow, rightLowLength,
                rightFold, rightFoldLength);

            // ... compute z_1 = z_a * z_b - z_0 - z_2
            BigInteger::Multiply(leftFold, leftFoldLength,
                     rightFold, rightFoldLength,
                     core, coreLength);
            BigInteger::SubtractCore(bitsHigh, bitsHighLength,
                         bitsLow, bitsLowLength,
                         core, coreLength);

            // ... and finally merge the result! :-)
            BigInteger::AddSelf(&bits[n], bitsLength - n, &core[0], coreLength);
        }
    }
}

static uint_array Multiply(uint_array lhs, uint_array rhs)
{
    assert(!lhs.empty());
    assert(!rhs.empty());
    assert(lhs.size() >= rhs.size());

    // Switching to unsafe pointers helps sparing
    // some nasty index calculations...

    uint_array bits(lhs.size() + rhs.size());

    BigInteger::Multiply(&lhs[0], lhs.size(), &rhs[0], rhs.size(), &bits[0], bits.size());

    return bits;

}

static uint_array Multiply(uint_array lhs, uint32_t rhs)
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

static void AddSelf(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength)
{
    assert(lhs >= 0);
    assert(rhs >= 0);
    assert(lhsLength >= rhsLength);

    // Executes the "grammar-school" algorithm for computing z = a + b.
    // Same as above, but we're writing the result directly to a and
    // stop execution, if we're out of b and c is already 0.

    int i = 0;
    long carry = 0L;

    for (; i < rhsLength; i++)
    {
        long digit = lhs[i] + carry + rhs[i];
        lhs[i] = static_cast<uint32_t>(digit);
        carry = digit >> 32;
    }
    for (; carry != 0 && i < lhsLength; i++)
    {
        long digit = lhs[i] + carry;
        lhs[i] = static_cast<uint>(digit);
        carry = digit >> 32;
    }

    assert(carry == 0);
}

static void SubtractSelf(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength)
{

}

static uint_array Square(uint_array& value)
{
    assert(!value.empty());

    // Switching to unsafe pointers helps sparing
    // some nasty index calculations...

    uint_array bits(value.size()*2);
    BigInteger::Square(&value[0], value.size(), &bits[0], bits.size());

    return bits;
}

static void Square(uint32_t* value, int valueLength, uint32_t* bits, int bitsLength)
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

    if (valueLength < BigInteger::SquareThreshold)
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
            long long carry = 0UL;
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
        BigInteger::Square(valueLow, valueLowLength,
               bitsLow, bitsLowLength);

        // ... compute z_2 = a_1 * a_1 (squaring again!)
        BigInteger::Square(valueHigh, valueHighLength,
               bitsHigh, bitsHighLength);

        int foldLength = valueHighLength + 1;
        int coreLength = foldLength + foldLength;

        if (coreLength < BigInteger::AllocationThreshold)
        {
            uint32_t* fold = new uint32_t[foldLength];
            //new Span<uint>(fold, foldLength).Clear();
            uint32_t* core = new uint32_t[coreLength];
            //new Span<uint>(core, coreLength).Clear();

            // ... compute z_a = a_1 + a_0 (call it fold...)
            BigInteger::Add(valueHigh, valueHighLength,
                valueLow, valueLowLength,
                fold, foldLength);

            // ... compute z_1 = z_a * z_a - z_0 - z_2
            BigInteger::Square(fold, foldLength,
                   core, coreLength);

            BigInteger::SubtractCore(bitsHigh, bitsHighLength,
                         bitsLow, bitsLowLength,
                         core, coreLength);

            // ... and finally merge the result! :-)
            AddSelf(&bits[n], bitsLength - n, &core[0], coreLength);
        }
        else
        {
            uint32_t* fold = new uint32_t[foldLength];
            uint32_t* core = new uint32_t[coreLength];
            // ... compute z_a = a_1 + a_0 (call it fold...)
            BigInteger::Add(valueHigh, valueHighLength,
                valueLow, valueLowLength,
                fold, foldLength);

            // ... compute z_1 = z_a * z_a - z_0 - z_2
            BigInteger::Square(fold, foldLength,
                   core, coreLength);

            BigInteger::SubtractCore(bitsHigh, bitsHighLength,
                         bitsLow, bitsLowLength,
                         core, coreLength);

            // ... and finally merge the result! :-)
            AddSelf(&bits[n], bitsLength - n, &core[0], coreLength);
        }
    }
}

static void SubtractCore(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength, uint32_t* core, int coreLength)
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

static BigInteger Multiply(BigInteger& lhs, BigInteger& rhs)
{
    return lhs * rhs;
}

static uint_array Add(uint_array& lhs, uint32_t rhs)
{
    assert(!lhs.empty());
    assert(lhs.size() >= 1);

    // Executes the addition for one big and one 32-bit integer.
    // Thus, we've similar code than below, but there is no loop for
    // processing the 32-bit integer, since it's a single element.

    uint_array bits(lhs.size());

    long digit = (long)lhs[0] - rhs;
    bits[0] = static_cast<uint32_t>(digit);
    long carry = digit >> 32;

    for (size_t i = 1; i < lhs.size(); i++)
    {
        digit = lhs[i] + carry;
        bits[i] = static_cast<uint32_t>(digit);
        carry = digit >> 32;
    }
    bits[lhs.size()] = static_cast<uint32_t>(carry);

    return bits;
}

static uint_array Add(uint_array& lhs, uint_array& rhs)
{
    assert(!lhs.empty());
    assert(!rhs.empty());
    assert(lhs.size() >= rhs.size());

    uint_array bits(lhs.size() + 1);

    BigInteger::Add(&lhs[0], lhs.size(), &rhs[0], rhs.size(), &bits[0], bits.size());

    return bits;
}

static void Add(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength, uint32_t* bits, int bitsLength)
{
    assert(lhsLength >= 0);
    assert(rhsLength >= 0);
    assert(lhsLength >= rhsLength);
    assert(bitsLength == lhsLength + 1);

    int i = 0;
    long long carry = 0L;

    for (; i < rhsLength; i++)
    {
        long digit = (lhs[i] + carry) + rhs[i];
        bits[i] = static_cast<uint32_t>(digit);
        carry = digit >> 32;
    }

    for (; i < lhsLength; i++)
    {
        long long digit = lhs[i] + carry;
        bits[i] = static_cast<uint32_t>(digit);
        carry = digit >> 32;
    }

    assert(carry == 0);
}

static uint_array Subtract(uint_array& lhs, uint32_t rhs)
{
    assert(!lhs.empty());
    assert(lhs.size() >= 1);
    assert(lhs[0] >= rhs || lhs.size() >= 2);

    // Executes the subtraction for one big and one 32-bit integer.
    // Thus, we've similar code than below, but there is no loop for
    // processing the 32-bit integer, since it's a single element.

    uint_array bits(lhs.size());

    long digit = (long)lhs[0] - rhs;
    bits[0] = static_cast<uint32_t>(digit);
    long carry = digit >> 32;

    for (size_t i = 1; i < lhs.size(); i++)
    {
        digit = lhs[i] + carry;
        bits[i] = static_cast<uint32_t>(digit);
        carry = digit >> 32;
    }

    return bits;
}

static uint_array Subtract(uint_array& lhs, uint_array& rhs)
{
    assert(!lhs.empty());
    assert(!rhs.empty());
    assert(lhs.size() >= rhs.size());
    assert(BigInteger::Compare(lhs, rhs) >= 0);

    uint_array bits(lhs.size());

    BigInteger::Subtract(&lhs[0], lhs.size(), &rhs[0], rhs.size(), &bits[0], bits.size());

    return bits;
}

static void Subtract(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength, uint32_t* bits, int bitsLength)
{
    assert(lhsLength >= 0);
    assert(rhsLength >= 0);
    assert(lhsLength >= rhsLength);
    assert(BigInteger::Compare(lhs, lhsLength, rhs, rhsLength) >= 0);
    assert(bitsLength == lhsLength);

    // Executes the "grammar-school" algorithm for computing z = a - b.
    // While calculating z_i = a_i - b_i we take care of overflow
    // Since a_i - b_i doesn't need any additional bit, our carry c
    // has always the value -1 or 0; hence, we're safe here.

    int i = 0;
    long carry = 0L;

    for (; i < rhsLength; i++)
    {
        long digit = lhs[i] + carry - rhs[i];
        bits[i] = static_cast<uint32_t>(digit);
        carry = digit >> 32;
    }
    for (; i < lhsLength; i++)
    {
        long digit = lhs[i] + carry;
        bits[i] = static_cast<uint32_t>(digit);
        carry = digit >> 32;
    }

    assert(carry == 0);
}

BigInteger operator >=(const long& lhs, BigInteger& rhs)
{
    return rhs.CompareTo(lhs);
}

BigInteger operator >=(BigInteger& lhs, const long& rhs)
{
    return lhs.CompareTo(rhs);
}

BigInteger operator %(BigInteger& dividend, BigInteger& divisor)
{
    dividend.AssertValid();
    divisor.AssertValid();
    
    bool trivialDividend = dividend._bits.empty();
    bool trivialDivisor = divisor._bits.empty();
    
    if (trivialDividend && trivialDivisor)
    {
        return dividend._sign % divisor._sign;
    }
    
    if (trivialDividend)
    {
        // The divisor is non-trivial
        // and therefore the bigger one
        return dividend;
    }
    
    if (trivialDivisor)
    {
        assert(!dividend._bits.empty());
        uint32_t remainder = BigInteger::Remainder(dividend._bits, abs(divisor._sign));
        return dividend._sign < 0 ? -1 * remainder : remainder;
    }
    
    assert(!dividend._bits.empty() && !divisor._bits.empty());
    
    if (dividend._bits.size() < divisor._bits.size())
    {
        return dividend;
    }
    uint_array bits = BigInteger::Remainder(dividend._bits, divisor._bits);
    return BigInteger(bits, dividend._sign < 0);
}

static BigInteger DivRem(BigInteger& dividend, BigInteger& divisor, BigInteger& remainder)
{
    dividend.AssertValid();
    divisor.AssertValid();
    
   bool trivialDividend = dividend.GetBits().empty() ;
   bool trivialDivisor = divisor.GetBits().empty();
   
   if (trivialDividend && trivialDivisor)
   {
       remainder = dividend.GetSign() % divisor.GetSign();
       return dividend.GetSign() / divisor.GetSign();
   }
   
   if (trivialDividend)
   {
       // The divisor is non-trivial
       // and therefore the bigger one
       remainder = dividend;
       return BigInteger::Zero();
   }
   
   assert(dividend.GetBits().empty());
   
   if (trivialDivisor)
   {
       uint32_t rest;
       uint_array rbits = dividend.GetBits();
       uint_array bits = BigInteger::Divide(rbits, abs(divisor.GetSign()), rest);
   
       remainder = dividend.GetSign() < 0 ? -1 * rest : rest;
       return BigInteger(bits, (dividend.GetSign() < 0) ^ (divisor.GetSign() < 0));
   }
   
   assert(!divisor.GetBits().empty());
   
   if (dividend.GetBits().size() < divisor.GetBits().size())
   {
       remainder = dividend;
       return BigInteger::Zero();
   }
   else
   {
       uint_array rest;
       uint_array lbits = dividend.GetBits();
       uint_array rbits = divisor.GetBits();
       uint_array bits = BigInteger::Divide(lbits, rbits, rest);
   
       remainder = BigInteger(rest, dividend.GetSign() < 0);
       return BigInteger(bits, (dividend.GetSign() < 0) ^ (divisor.GetSign() < 0));
   }
}

BigInteger operator -(BigInteger& value)
{
    //value.AssertValid();
    return BigInteger(-value._sign, value._bits);
}

BigInteger operator +(BigInteger& value)
{
    //value.AssertValid();
    return value;
}

BigInteger operator ++(BigInteger& value)
{
    BigInteger&& one = BigInteger::One();
    return value + one;
}

BigInteger operator --(BigInteger& value)
{
    //value.AssertValid();
    BigInteger&& one = BigInteger::One();
    return value - one;
}

BigInteger operator +=(BigInteger& rhs, BigInteger& lhs)
{
    //value.AssertValid();
    return lhs + rhs;
}

BigInteger operator -=(BigInteger& rhs, BigInteger& lhs)
{
    //value.AssertValid();
    return lhs - rhs;
}

BigInteger operator /=(BigInteger& rhs, BigInteger& lhs)
{
    //value.AssertValid();
    return lhs / rhs;
}

BigInteger operator *=(BigInteger& rhs, BigInteger& lhs)
{
    //value.AssertValid();
    return lhs * rhs;
}

BigInteger operator ==(BigInteger& rhs, BigInteger& lhs)
{
    //value.AssertValid();
    return rhs.Equals(lhs);
}

BigInteger operator /(BigInteger& dividend, BigInteger& divisor)
{
    dividend.AssertValid();
    divisor.AssertValid();

    bool trivialDividend = dividend._bits.empty();
    bool trivialDivisor = divisor._bits.empty();

    if (trivialDividend && trivialDivisor)
    {
        return dividend._sign / divisor._sign;
    }

    if (trivialDividend)
    {
        // The divisor is non-trivial
        // and therefore the bigger one
        return BigInteger::s_bnZeroInt;
    }

    if (trivialDivisor)
    {
        assert(!dividend._bits.empty());
        uint_array bits = BigInteger::Divide(dividend._bits, abs(divisor._sign));
        return BigInteger(bits, (dividend._sign < 0) ^ (divisor._sign < 0));
    }

    assert(!dividend._bits.empty() && !divisor._bits.empty());

    if (dividend._bits.size() < divisor._bits.size())
    {
        return BigInteger::s_bnZeroInt;
    }
    else
    {
        uint_array bits = BigInteger::Divide(dividend._bits, divisor._bits);
        return BigInteger(bits, (dividend._sign < 0) ^ (divisor._sign < 0));
    }
}

static uint_array Divide(uint_array& lhs, uint32_t rhs, uint32_t& remainder)
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
        uint64_t digit = value / rhs;
        quotient[i] = static_cast<uint32_t>(digit);
        carry = value - digit * rhs;
    }
    remainder = static_cast<uint32_t>(carry);

    return quotient;
}

static uint_array Divide(uint_array& lhs, uint32_t rhs)
{
    assert(!lhs.empty());
    assert(lhs.size() >= 1);

    // Same as above, but only computing the quotient.

    uint_array quotient(lhs.size());

    uint64_t carry = 0UL;
    for (int i = lhs.size() - 1; i >= 0; i--)
    {
        uint64_t value = (carry << 32) | lhs[i];
        uint64_t digit = value / rhs;
        quotient[i] = static_cast<uint32_t>(digit);
        carry = value - digit * rhs;
    }

    return quotient;
}

static uint32_t Remainder(uint_array& lhs, uint rhs)
{
    assert(!lhs.empty());
    assert(lhs.size() >= 1);

    // Same as above, but only computing the remainder.

    uint64_t carry = 0UL;
    for (int i = lhs.size() - 1; i >= 0; i--)
    {
        uint64_t value = (carry << 32) | lhs[i];
        carry = value % rhs;
    }

    return static_cast<uint32_t>(carry);
}

static uint_array Divide(uint_array& lhs, uint_array& rhs, uint_array& remainder)
{
    assert(!lhs.empty());
    assert(!rhs.empty());
    assert(lhs.size() >= 1);
    assert(rhs.size() >= 1);
    assert(lhs.size() >= rhs.size());

    // NOTE: left will get overwritten, we need a local copy

    uint_array localLeft = uint_array(lhs);
    uint_array bits(lhs.size() - rhs.size() + 1);

    BigInteger::Divide(&localLeft[0], localLeft.size(), &rhs[0], rhs.size(), &bits[0], bits.size());

    remainder = localLeft;

    return bits;
}

static uint_array Divide(uint_array& lhs, uint_array& rhs)
{
    assert(!lhs.empty());
    assert(!rhs.empty());
    assert(lhs.size() >= 1);
    assert(rhs.size() >= 1);
    assert(lhs.size() >= rhs.size());

    // Same as above, but only returning the quotient.

    // NOTE: left will get overwritten, we need a local copy
    uint_array localLeft = uint_array(lhs);
    uint_array bits(lhs.size() - rhs.size() + 1);

    BigInteger::Divide(&localLeft[0], localLeft.size(), &rhs[0], rhs.size(), &bits[0], bits.size());

    return bits;
}

static uint_array Remainder(uint_array& lhs, uint_array& rhs)
{
    assert(!lhs.empty());
    assert(!rhs.empty());
    assert(lhs.size() >= 1);
    assert(rhs.size() >= 1);
    assert(lhs.size() >= rhs.size());

    // Same as above, but only returning the remainder.

    // NOTE: left will get overwritten, we need a local copy
    uint_array localLeft = uint_array(lhs);

    BigInteger::Divide(&localLeft[0], localLeft.size(), &rhs[0], rhs.size(), nullptr, 0);

    return localLeft;
}

static void Divide(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength, uint32_t* bits, int bitsLength)
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
    int shift = BigInteger::LeadingZeros(divHi);
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
        uint64_t digit = valHi / divHi;
        if (digit > 0xFFFFFFFF)
            digit = 0xFFFFFFFF;

        // Our first guess may be a little bit to big
        while (BigInteger::DivideGuessTooBig(digit, valHi, valLo, divHi, divLo))
            --digit;

        if (digit > 0)
        {
            // Now it's time to subtract our current quotient
            uint32_t carry = BigInteger::SubtractDivisor(&lhs[n], lhsLength - n, &rhs[0], rhsLength, digit);

            if (carry != t)
            {
                assert(carry == t + 1);

                // Our guess was still exactly one too high
                carry = BigInteger::AddDivisor(&lhs[n], lhsLength - n, &rhs[0], rhsLength);
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

static uint32_t AddDivisor(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength)
{
    assert(lhsLength >= 0);
    assert(rhsLength >= 0);
    assert(lhsLength >= rhsLength);

    // Repairs the dividend, if the last subtract was too much

    uint64_t carry = 0UL;

    for (int i = 0; i < rhsLength; i++)
    {
        ulong digit = (lhs[i] + carry) + rhs[i];
        lhs[i] = static_cast<uint32_t>(digit);
        carry = digit >> 32;
    }

    return static_cast<uint32_t>(carry);
}

static uint32_t SubtractDivisor(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength, uint64_t q)
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

static bool DivideGuessTooBig(uint64_t q, uint64_t valHi, uint32_t valLo, uint32_t divHi, uint32_t divLo)
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

static int LeadingZeros(uint32_t value)
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

static BigInteger Divide(BigInteger& dividend, BigInteger& divisor)
{
    return dividend / divisor;
}

int BigInteger::CompareTo(long other)
{
    AssertValid();

    if (!_bits.empty())
        return (static_cast<long>(_sign)) == (other);
    int cu;
    if ((_sign ^ other) < 0 || (cu = _bits.size()) > 2)
        return _sign;
    uint64_t uu = other < 0 ? static_cast<unsigned long>(-other) : static_cast<unsigned long>(other);
    uint64_t uuTmp = cu == 2 ? BigInteger::MakeUlong(_bits[1], _bits[0]) : _bits[0];
    return (_sign * uuTmp) == uu;
}

int BigInteger::CompareTo(BigInteger& other)
{
    AssertValid();
    other.AssertValid();

    if ((_sign ^ other._sign) < 0)
    {
        // Different signs, so the comparison is easy.
        return _sign < 0 ? -1 : +1;
    }

    // Same signs
    if (!_bits.empty())
    {
        if (!other._bits.empty())
            return _sign < other._sign ? -1 : _sign > other._sign ? +1 : 0;
        return -other._sign;
    }
    int cuThis, cuOther;
    if (!other._bits.empty() || (cuThis = _bits.size()) > (cuOther = other._bits.size()))
        return _sign;
    if (cuThis < cuOther)
        return -_sign;

    int cuDiff = GetDiffLength(_bits, other._bits, cuThis);
    if (cuDiff == 0)
        return 0;
    return _bits[cuDiff - 1] < other._bits[cuDiff - 1] ? -_sign : _sign;
}

static int GetDiffLength(uint_array rgu1, uint_array rgu2, int cu)
{
    for (int iv = cu; --iv >= 0;)
    {
        if (rgu1[iv] != rgu2[iv])
            return iv + 1;
    }
    return 0;
}

static int Compare(uint_array lhs, uint_array rhs)
{
    assert(!lhs.empty());
    assert(!rhs.empty());

    if (lhs.size() < rhs.size())
        return -1;
    if (lhs.size() > rhs.size())
        return 1;

    for (int i = lhs.size() - 1; i >= 0; i--)
    {
        if (lhs[i] < rhs[i])
            return -1;
        if (lhs[i] > rhs[i])
            return 1;
    }
    return 0;
}

static int Compare(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength)
{
    assert(lhsLength >= 0);
    assert(rhsLength >= 0);

    if (lhsLength < rhsLength)
        return -1;
    if (lhsLength > rhsLength)
        return 1;

    for (int i = lhsLength - 1; i >= 0; i--)
    {
        if (lhs[i] < rhs[i])
            return -1;
        if (lhs[i] > rhs[i])
            return 1;
    }

    return 0;
}

static uint64_t MakeUlong(unsigned int uHi, unsigned int uLo)
{
    return (static_cast<uint64_t>(uHi) << BigInteger::kcbitUint) | uLo;
}

BigInteger BigInteger::Zero()
{
	return BigInteger(0);
}

BigInteger BigInteger::One()
{
	return BigInteger(1);
}

BigInteger BigInteger::MinusOne()
{
	return BigInteger(-1);
}

int BigInteger::Sign()
{
    AssertValid();
    return (_sign >> (kcbitUint - 1)) - (-_sign >> (kcbitUint - 1));
}

int BigInteger::GetSign()
{
    return _sign;
}

uint_array BigInteger::GetBits()
{
    return _bits;
}

//bool Equals(std::any obj)
//{
//    //AssertValid();
//
//    //if (!(obj is BigInteger))
//    //    return false;
//    return Equals(std::any_cast<BigInteger>(obj));
//}

bool BigInteger::Equals(BigInteger& other) const
{
    //AssertValid();
    //other.AssertValid();

    if (_sign != other._sign)
        return false;
    if (_bits == other._bits)
        // _sign == other._sign && _bits == null && other._bits == null
        return true;

    if (_bits.empty() || other._bits.empty())
        return false;
    size_t cu = _bits.size();
    if (cu != other._bits.size())
        return false;
    int cuDiff = GetDiffLength(_bits, other._bits, cu);
    return cuDiff == 0;
}

static BigInteger Parse(std::string value)
{
    return BigNumber::ParseBigInteger(value);
}

void BigInteger::AssertValid() {

    if (_bits.empty())
    {
        assert(_sign == 1 || _sign == -1);
        assert(_bits.size() > 0);
        assert(_bits.size() > 1 || _bits[0] >= kuMaskHighBit);
        assert(_bits[_bits.size() - 1] != 0);
    }
    else
    {
        assert(_sign > std::numeric_limits<int>::min());
    }
}

