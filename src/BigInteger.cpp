#include "BigInteger.h"
#include "BigNumber.h"
#include "NumericsHelpers.h"

#include <limits>
#include <cassert>
#include <cstdlib>
#include <any>
#include <iostream>
#include <cmath>
#include <BigIntegerCalculator/BigIntegerCalculator.h>


//using tcb::span; // currently unused, not sure if we "need" a span

const BigInteger BigInteger::s_bnMinInt = BigInteger(-1, uint_array {kuMaskHighBit});
const BigInteger BigInteger::s_bnOneInt = BigInteger(1);
const BigInteger BigInteger::s_bnZeroInt = BigInteger(0);
const BigInteger BigInteger::s_bnMinusOneInt = BigInteger(-1);

const double double_NaN = static_cast<double>(0.0)/static_cast<double>(0.0);
const double double_PositiveInfinity = static_cast<double>(1.0)/static_cast<double>(0.0);
const double double_NegativeInfinity = static_cast<double>(-1.0)/static_cast<double>(0.0);

BigInteger::BigInteger()
{
	_sign = 0;
    _bits = uint_array();
}

BigInteger::BigInteger(int value)
{

    //std::cout << "int constructor value: " << value << " minint: " << std::numeric_limits<int>::min() << std::endl;
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

BigInteger::BigInteger(uint32_t value) {
    //std::cout << "uint32_t constructor: " << value << std::endl;
    //std::cout << "uint32_t limit: " << std::numeric_limits<uint32_t>::min() << std::endl;
    if (value <= static_cast<uint32_t>(std::numeric_limits<int>::max()))
    {
        _sign = value;
        //_bits = uint_array();
    }
    else
    {
        _sign = +1;
        _bits.push_back(value);
    }
    AssertValid();
}

BigInteger::BigInteger(long value)
{
    *this = BigInteger(static_cast<long long>(value));
}

BigInteger::BigInteger(long long value)
{
    //std::cout << "long long constructor: " << value << std::endl;
    //std::cout << "int min: " << std::numeric_limits<int>::min() << std::endl;
    //std::cout << "int min: " << std::numeric_limits<int>::max() << std::endl;
    if (std::numeric_limits<int>::min() < value
            && value <= std::numeric_limits<int>::max())
    {
        //std::cout << "if " << std::endl;
        _sign = static_cast<int>(value);
        //_bits = ;
    }
    else if (value == std::numeric_limits<int>::min())
    {
        *this = s_bnMinInt;
    }
    else
    {
        //std::cout << "else " << value << std::endl;
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

        if (x <= std::numeric_limits<uint32_t>::max())
        {
            _bits.push_back(static_cast<uint32_t>(x));
        }
        else
        {
            //std::cout << "uint cast: " << static_cast<uint32_t>(x) << std::endl;
            //std::cout << "uint cast2: " << static_cast<uint32_t>(x >> kcbitUint) << std::endl;
            _bits.push_back(static_cast<uint32_t>(x));
            _bits.push_back(static_cast<uint32_t>(x >> kcbitUint));
        }
    }

    AssertValid();
}

BigInteger::BigInteger(uint64_t value)
{
    //std::cout << "uint64_t constructor" << std::endl;
    if (value <= static_cast<uint64_t>(std::numeric_limits<int>::max()))
    {
        _sign = static_cast<int>(value);
    }
    else if (value <= std::numeric_limits<uint32_t>::max())
    {
        _sign = +1;
        _bits.push_back(static_cast<uint32_t>(value));
    }
    else
    {
        _sign = +1;
        _bits.push_back(static_cast<uint32_t>(value));
        _bits.push_back(static_cast<uint32_t>(value >> kcbitUint));
    }

    AssertValid();
}

BigInteger::BigInteger(int n, uint_array value) {
     _sign = n;
    _bits = value;
    AssertValid();
}

BigInteger::BigInteger(byte_array value, bool isUnsigned, bool isBigEndian)
{
    int byteCount = value.size();

    bool isNegative;
    if (byteCount > 0)
    {
        byte mostSignificantByte = isBigEndian ? value[0] : value[byteCount - 1];
        isNegative = (mostSignificantByte & 0x80) != 0 && !isUnsigned;

        if (mostSignificantByte == 0)
        {
            // Try to conserve space as much as possible by checking for wasted leading byte[] entries
            if (isBigEndian)
            {
                int offset = 1;

                while (offset < byteCount && value[offset] == 0)
                {
                    offset++;
                }

                value = byte_array(value.begin() + offset, value.end());
                byteCount = value.size();
            }
            else
            {
                byteCount -= 2;
                while (byteCount >= 0 && value[byteCount] == 0)
                {
                    byteCount--;
                }
                byteCount++;
            }
        }
    }
    else
    {
        isNegative = false;
    }

    if (byteCount == 0)
    {
        // BigInteger.Zero
        _sign = 0;
        _bits = uint_array();
        AssertValid();
        return;
    }

    if (byteCount <= 4)
    {
        _sign = isNegative ? static_cast<int>(0xffffffff) : 0;

        if (isBigEndian)
        {
            for (int i = 0; i < byteCount; i++)
            {
                _sign = (_sign << 8) | value[i];
            }
        }
        else
        {
            for (int i = byteCount - 1; i >= 0; i--)
            {
                _sign = (_sign << 8) | value[i];
            }
        }

        _bits = uint_array();
        if (_sign < 0 && !isNegative)
        {
            // Int32 overflow
            // Example: Int64 value 2362232011 (0xCB, 0xCC, 0xCC, 0x8C, 0x0)
            // can be naively packed into 4 bytes (due to the leading 0x0)
            // it overflows into the int32 sign bit
            _bits.push_back(static_cast<uint32_t>(_sign));
            _sign = +1;
        }
        if (_sign == std::numeric_limits<int>::min())
        {
            *this = s_bnMinInt;
        }
    }
    else
    {
        int unalignedBytes = byteCount % 4;
        int dwordCount = byteCount / 4 + (unalignedBytes == 0 ? 0 : 1);
        uint_array val(dwordCount);
        int byteCountMinus1 = byteCount - 1;

        // Copy all dwords, except don't do the last one if it's not a full four bytes
        int curDword, curByte;

        if (isBigEndian)
        {
            curByte = byteCount - sizeof(int);
            for (curDword = 0; curDword < dwordCount - (unalignedBytes == 0 ? 0 : 1); curDword++)
            {
                for (int byteInDword = 0; byteInDword < 4; byteInDword++)
                {
                    byte curByteValue = value[curByte];
                    val[curDword] = (val[curDword] << 8) | curByteValue;
                    curByte++;
                }

                curByte -= 8;
            }
        }
        else
        {
            curByte = sizeof(int) - 1;
            for (curDword = 0; curDword < dwordCount - (unalignedBytes == 0 ? 0 : 1); curDword++)
            {
                for (int byteInDword = 0; byteInDword < 4; byteInDword++)
                {
                    byte curByteValue = value[curByte];
                    val[curDword] = (val[curDword] << 8) | curByteValue;
                    curByte--;
                }

                curByte += 8;
            }
        }

        // Copy the last dword specially if it's not aligned
        if (unalignedBytes != 0)
        {
            if (isNegative)
            {
                val[dwordCount - 1] = 0xffffffff;
            }

            if (isBigEndian)
            {
                for (curByte = 0; curByte < unalignedBytes; curByte++)
                {
                    byte curByteValue = value[curByte];
                    val[curDword] = (val[curDword] << 8) | curByteValue;
                }
            }
            else
            {
                for (curByte = byteCountMinus1; curByte >= byteCount - unalignedBytes; curByte--)
                {
                    byte curByteValue = value[curByte];
                    val[curDword] = (val[curDword] << 8) | curByteValue;
                }
            }
        }

        if (isNegative)
        {
            BigInteger::DangerousMakeTwosComplement(val); // Mutates val

            // Pack _bits to remove any wasted space after the twos complement
            int len = val.size() - 1;
            while (len >= 0 && val[len] == 0) len--;
            len++;

            if (len == 1)
            {
                switch (val[0])
                {
                    case 1: // abs(-1)
                        *this = s_bnMinusOneInt;
                        return;

                    case kuMaskHighBit: // abs(Int32.MinValue)
                        *this = s_bnMinInt;
                        return;

                    default:
                        if (static_cast<int>(val[0]) > 0)
                        {
                            _sign = (-1) * ((int)val[0]);
                            _bits = uint_array();
                            AssertValid();
                            return;
                        }

                        break;
                }
            }

            if (static_cast<size_t>(len) != val.size())
            {
                _sign = -1;
                _bits = uint_array();
                _bits = uint_array(val.begin(), val.begin() + len);
            }
            else
            {
                _sign = -1;
                _bits = val;
            }
        }
        else
        {
            _sign = +1;
            _bits = val;
        }
    }
    AssertValid();
}

BigInteger::BigInteger(uint_array value)
{
    // TODO
    if (value.size() == 0)
        throw std::runtime_error("value cannot be null");

    int dwordCount = value.size();
    bool isNegative = dwordCount > 0 && ((value[dwordCount - 1] & 0x80000000) == 0x80000000);

    // Try to conserve space as much as possible by checking for wasted leading uint[] entries
    while (dwordCount > 0 && value[dwordCount - 1] == 0) dwordCount--;

    if (dwordCount == 0)
    {
        // BigInteger.Zero
        *this = BigInteger::s_bnZeroInt;
        AssertValid();
        return;
    }
    if (dwordCount == 1)
    {
        if (static_cast<int>(value[0]) < 0 && !isNegative)
        {
            _bits.push_back(value[0]);
            _sign = +1;
        }
        // Handle the special cases where the BigInteger likely fits into _sign
        else if (std::numeric_limits<int>::min() == static_cast<int>(value[0]))
        {
            *this = BigInteger::s_bnMinInt;
        }
        else
        {
            _sign = static_cast<int>(value[0]);
        }
        AssertValid();
        return;
    }

    if (!isNegative)
    {
        // Handle the simple positive value cases where the input is already in sign magnitude
        if (dwordCount != static_cast<int>(value.size()))
        {
            _sign = +1;
            _bits = uint_array(value.begin(), value.begin() + dwordCount);
        }
        // No trimming is possible.  Assign value directly to _bits.
        else
        {
            _sign = +1;
            _bits = value;
        }
        AssertValid();
        return;
    }

    // Finally handle the more complex cases where we must transform the input into sign magnitude
    BigInteger::DangerousMakeTwosComplement(value); // mutates val

    // Pack _bits to remove any wasted space after the twos complement
    int len = value.size();
    while (len > 0 && value[len - 1] == 0) len--;

    // The number is represented by a single dword
    if (len == 1 && static_cast<int>((value[0])) > 0)
    {
        if (value[0] == 1 /* abs(-1) */)
        {
            *this = s_bnMinusOneInt;
        }
        else if (value[0] == kuMaskHighBit /* abs(Int32.MinValue) */)
        {
            *this = s_bnMinInt;
        }
        else
        {
            _sign = (-1) * ((int)value[0]);
        }
    }
    // The number is represented by multiple dwords.
    // Trim off any wasted uint values when possible.
    else if (len != static_cast<int>(value.size()))
    {
        _sign = -1;
        _bits = uint_array(value.begin(), value.begin() + len);
    }
    // No trimming is possible.  Assign value directly to _bits.
    else
    {
        _sign = -1;
        _bits = value;
    }
    AssertValid();
    return;
}



BigInteger::BigInteger(uint_array value, bool negative)
{
    if (value.size() == 0)
        throw std::runtime_error("value is empty");

    int len;

    // Try to conserve space as much as possible by checking for wasted leading uint[] entries
    // sometimes the uint[] has leading zeros from bit manipulation operations & and ^
    for (len = value.size(); len > 0 && value[len - 1] == 0; len--);

    if (len == 0)
        *this = s_bnZeroInt;
    // Values like (Int32.MaxValue+1) are stored as "0x80000000" and as such cannot be packed into _sign
    else if (len == 1 && value[0] < kuMaskHighBit)
    {
        _sign = (negative ? -(int)value[0] : (int)value[0]);
        _bits = uint_array();
        // Although Int32.MinValue fits in _sign, we represent this case differently for negate
        if (_sign == std::numeric_limits<int>::min())
            *this = s_bnMinInt;
    }
    else
    {
        _sign = negative ? -1 : +1;
        _bits.swap(value);
    }

    while (!_bits.empty() && _bits[_bits.size() - 1] == 0) {
        _bits.pop_back();
    }
    AssertValid();
}

bool BigInteger::IsPowerOfTwo()
{
    AssertValid();

    if (_bits.size() == 0)
        return (_sign & (_sign - 1)) == 0 && _sign != 0;

    if (_sign != 1)
        return false;

    int iu = _bits.size() - 1;
    if ((_bits[iu] & (_bits[iu] - 1)) != 0)
        return false;

    while (--iu >= 0)
    {
        if (_bits[iu] != 0)
            return false;
    }

    return true;
}

bool BigInteger::IsZero()
{
    AssertValid();
    return _sign == 0;
}

bool BigInteger::IsOne()
{
    AssertValid();
    return _sign == 1 && _bits.size() == 0;
}

bool BigInteger::IsEven()
{
    AssertValid();
    return _bits.size() == 0 ? (_sign & 1) == 0 : (_bits[0] & 1) == 0;
}

BigInteger BigInteger::Add(BigInteger& lhs, BigInteger& rhs)
{
    lhs.AssertValid();
    return lhs + rhs;
}

BigInteger BigInteger::Subtract(BigInteger& lhs, BigInteger& rhs)
{
    return lhs - rhs;
}

BigInteger BigInteger::Add(uint_array& lhs, int lhsSign, uint_array& rhs, int rhsSign)
{
    //std::cout << "add now " << std::endl;
    //std::cout << "lhsSign: " << lhsSign << std::endl;
    //std::cout << "rhsSign: " << rhsSign << std::endl;
    bool trivialLeft = lhs.empty();
    bool trivialRight = rhs.empty();
    //std::cout << "trivial: " << trivialLeft << " " << trivialRight <<  std::endl;

    if (trivialLeft && trivialRight)
    {
        return BigInteger(static_cast<long long>(lhsSign) + rhsSign);
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
        //for (auto a : bits)
        //{
        //    std::cout << "auto: " << a << std::endl;
        //}
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
        //for (auto elem : bits)
        //{
        //    std::cout << "elem: " << elem << std::endl;
        //}
        return BigInteger(bits, lhsSign < 0);
    }
}

BigInteger BigInteger::Subtract(uint_array& lhs, int lhsSign, uint_array& rhs, int rhsSign)
{
    bool trivialLeft = lhs.empty();
    bool trivialRight = rhs.empty();
    //std::cout << "sub now " << std::endl;
    //std::cout << "lhsSign: " << lhsSign << std::endl;
    //std::cout << "rhsSign: " << rhsSign << std::endl;
    //std::cout << "trivial: " << trivialLeft << " " << trivialRight <<  std::endl;

    if (trivialLeft && trivialRight)
    {
        return BigInteger(static_cast<long long>(lhsSign) - rhsSign);
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

    assert(!lhs.empty() && !rhs.empty());

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

BigInteger BigInteger::operator -(BigInteger& rhs)
{
    BigInteger lhs = *this;
    lhs.AssertValid();
    rhs.AssertValid();

    if ((lhs._sign < 0) != (rhs._sign < 0))
    {
        //std::cout << "add" << std::endl;
        return BigInteger::Add(lhs._bits, lhs._sign, rhs._bits, -1 * rhs._sign);
    }
    //std::cout << "sub" << std::endl;
    return BigInteger::Subtract(lhs._bits, lhs._sign, rhs._bits, rhs._sign);
}

BigInteger BigInteger::operator +(BigInteger& rhs)
{
    BigInteger lhs = *this;
    lhs.AssertValid();
    rhs.AssertValid();
    //std::cout << "*operator sign left: " << lhs._sign << " sign right: " << rhs._sign<< std::endl;
    //std::cout << "*operator left: " << lhs.ToString() << " right: " << rhs.ToString() << std::endl;

    if ((lhs._sign < 0) != (rhs._sign < 0))
    {
        //std::cout << "sub::::::" << std::endl;
        return BigInteger::Subtract(lhs._bits, lhs._sign, rhs._bits, -1 * rhs._sign);
    }
    //std::cout << "Add::::::" << std::endl;
    return BigInteger::Add(lhs._bits, lhs._sign, rhs._bits, rhs._sign);
}

BigInteger BigInteger::operator *(BigInteger& rhs)
{
    BigInteger lhs = *this;
    lhs.AssertValid();
    rhs.AssertValid();

    bool trivialLeft = lhs._bits.empty();
    bool trivialRight = rhs._bits.empty();
    //std::cout << "*operator trivialLeft: " << trivialLeft << " trivialRight: " << trivialRight << std::endl;
    //std::cout << "*operator sign left: " << lhs._sign << " sign right: " << rhs._sign<< std::endl;

    if (trivialLeft && trivialRight)
    {
        //std::cout << "trivvvv: " << static_cast<long long>(lhs._sign) * rhs._sign << std::endl;
        return BigInteger(static_cast<long long>(lhs._sign) * rhs._sign);
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

void BigInteger::Multiply(uint32_t* left, int leftLength, uint32_t* right, int rightLength, uint32_t* bits, int bitsLength)
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

uint_array BigInteger::Multiply(uint_array lhs, uint_array rhs)
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

uint_array BigInteger::Multiply(uint_array lhs, uint32_t rhs)
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

void BigInteger::AddSelf(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength)
{
    assert(*lhs >= 0);
    assert(*rhs >= 0);
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

//void BigInteger::SubtractSelf(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength)
//{
//
//}

uint_array BigInteger::Square(uint_array& value)
{
    assert(!value.empty());

    // Switching to unsafe pointers helps sparing
    // some nasty index calculations...

    uint_array bits(value.size()*2);
    BigInteger::Square(&value[0], value.size(), &bits[0], bits.size());

    return bits;
}

void BigInteger::Square(uint32_t* value, int valueLength, uint32_t* bits, int bitsLength)
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
            BigInteger::AddSelf(&bits[n], bitsLength - n, &core[0], coreLength);
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
            BigInteger::AddSelf(&bits[n], bitsLength - n, &core[0], coreLength);
        }
    }
}

void BigInteger::SubtractCore(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength, uint32_t* core, int coreLength)
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

BigInteger BigInteger::Multiply(BigInteger& lhs, BigInteger& rhs)
{
    return lhs * rhs;
}

uint_array BigInteger::Add(uint_array& lhs, uint32_t rhs)
{
    assert(!lhs.empty());
    assert(lhs.size() >= 1);

    // Executes the addition for one big and one 32-bit integer.
    // Thus, we've similar code than below, but there is no loop for
    // processing the 32-bit integer, since it's a single element.

    uint_array bits(lhs.size() + 1);

    long long digit = static_cast<long long>(lhs[0]) + rhs;
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

uint_array BigInteger::Add(uint_array& lhs, uint_array& rhs)
{
    assert(!lhs.empty());
    assert(!rhs.empty());
    assert(lhs.size() >= rhs.size());

    uint_array bits(lhs.size() + 1);

    BigInteger::Add(lhs.data(), lhs.size(), rhs.data(), rhs.size(), bits.data(), bits.size());

    return bits;
}

void BigInteger::Add(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength, uint32_t* bits, int bitsLength)
{
    assert(lhsLength >= 0);
    assert(rhsLength >= 0);
    assert(lhsLength >= rhsLength);
    assert(bitsLength == lhsLength + 1);

    int i = 0;
    uint64_t carry = 0L;

    for (; i < rhsLength; i++)
    {
        uint64_t digit = (lhs[i] + carry) + rhs[i];
        bits[i] = static_cast<uint32_t>(digit);
        carry = digit >> 32;
    }

    for (; i < lhsLength; i++)
    {
        uint64_t digit = lhs[i] + carry;
        bits[i] = static_cast<uint32_t>(digit);
        carry = digit >> 32;
    }

    bits[i] = static_cast<uint32_t>(carry);
}

uint_array BigInteger::Subtract(uint_array& lhs, uint32_t rhs)
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

uint_array BigInteger::Subtract(uint_array& lhs, uint_array& rhs)
{
    assert(!lhs.empty());
    assert(!rhs.empty());
    assert(lhs.size() >= rhs.size());
    assert(BigInteger::Compare(lhs, rhs) >= 0);

    uint_array bits(lhs.size());

    BigInteger::Subtract(lhs.data(), lhs.size(), rhs.data(), rhs.size(), bits.data(), bits.size());

    return bits;
}

void BigInteger::Subtract(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength, uint32_t* bits, int bitsLength)
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

BigInteger BigInteger::operator >=(BigInteger& rhs)
{
    return this->CompareTo(rhs);
}

BigInteger BigInteger::operator >=(long rhs)
{
    return this->CompareTo(rhs);
}

BigInteger BigInteger::operator %(BigInteger& divisor)
{
    BigInteger dividend = *this;
    dividend.AssertValid();
    divisor.AssertValid();

    bool trivialDividend = dividend._bits.empty();
    bool trivialDivisor = divisor._bits.empty();
    //std::cout << "trivialDividend: " << trivialDividend << " trivialDivisor: " << trivialDivisor << std::endl;
    //std::cout << "dividend: " << dividend.ToString() << " divisor: " << divisor.ToString() << std::endl;

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
        long remainder = BigInteger::Remainder(dividend._bits, abs(divisor._sign));
        //std::cout << "remainder: " << -1 * remainder << std::endl;
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

BigInteger BigInteger::DivRem(BigInteger& dividend, BigInteger& divisor, BigInteger& remainder)
{
    //dividend.AssertValid();
    //divisor.AssertValid();

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

BigInteger BigInteger::operator -()
{
    return BigInteger(-(*this)._sign, (*this)._bits);
}

BigInteger BigInteger::operator +()
{
    return BigInteger(+(*this)._sign, (*this)._bits);
}

BigInteger& BigInteger::operator ++(int value)
{
    BigInteger&& one = BigInteger::One();
    *this = *this + one;
    return *this;
}

BigInteger& BigInteger::operator --(int value)
{
    //value.AssertValid();
    BigInteger&& one = BigInteger::One();
    *this = *this - one;
    return *this;
}

BigInteger BigInteger::operator +=(uint64_t rhs)
{
    //std::cout << "uint += operator" << std::endl;
    //value.AssertValid();
    BigInteger value = BigInteger(rhs);
    *this = (*this + value);
    //std::cout << "adding: " << value.ToString() << std::endl;
    return *this;
}

BigInteger BigInteger::operator +=(BigInteger& rhs)
{
    //value.AssertValid();
    *this = (*this + rhs);
    return *this;
}

BigInteger BigInteger::operator -=(BigInteger& rhs)
{
    //value.AssertValid();
    *this = (*this - rhs);
    return *this;
}

BigInteger BigInteger::operator /=(BigInteger& rhs)
{
    //value.AssertValid();
    *this = (*this / rhs);
    return *this / rhs;
}

BigInteger BigInteger::operator *=(BigInteger& rhs)
{
    *this = (*this * rhs);
    return *this;
}

bool operator ==(const BigInteger& lhs, const BigInteger& rhs)
{
    return lhs.Equals(rhs);
}

bool BigInteger::operator ==(const BigInteger& rhs)
{
    return this->Equals(rhs);
}

bool BigInteger::operator !=(const BigInteger& rhs)
{
    return !(this->Equals(rhs));
}

BigInteger BigInteger::operator /(BigInteger& divisor)
{
    BigInteger dividend = *this;
    dividend.AssertValid();
    divisor.AssertValid();

    bool trivialDividend = dividend._bits.empty();
    bool trivialDivisor = divisor._bits.empty();

    if (trivialDividend && trivialDivisor)
    {
        //std::cout << dividend._sign << " / " << divisor._sign << std::endl;
        return static_cast<long long>(dividend._sign / divisor._sign);
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

uint_array BigInteger::Divide(uint_array& lhs, uint32_t rhs, uint32_t& remainder)
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

uint_array BigInteger::Divide(uint_array& lhs, uint32_t rhs)
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

uint32_t BigInteger::Remainder(uint_array& lhs, uint rhs)
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

uint_array BigInteger::Divide(uint_array& lhs, uint_array& rhs, uint_array& remainder)
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

uint_array BigInteger::Divide(uint_array& lhs, uint_array& rhs)
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

uint_array BigInteger::Remainder(uint_array& lhs, uint_array& rhs)
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

void BigInteger::Divide(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength, uint32_t* bits, int bitsLength)
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

uint32_t BigInteger::AddDivisor(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength)
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

uint32_t BigInteger::SubtractDivisor(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength, uint64_t q)
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

bool BigInteger::DivideGuessTooBig(uint64_t q, uint64_t valHi, uint32_t valLo, uint32_t divHi, uint32_t divLo)
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

int BigInteger::LeadingZeros(uint32_t value)
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

BigInteger BigInteger::Divide(BigInteger& dividend, BigInteger& divisor)
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

int BigInteger::GetDiffLength(uint_array rgu1, uint_array rgu2, int cu)
{
    for (int iv = cu; --iv >= 0;)
    {
        if (rgu1[iv] != rgu2[iv])
            return iv + 1;
    }
    return 0;
}

int BigInteger::Compare(uint_array lhs, uint_array rhs)
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

int BigInteger::Compare(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength)
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

uint64_t BigInteger::MakeUlong(unsigned int uHi, unsigned int uLo)
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

bool BigInteger::Equals(uint64_t rhs) const
{
    BigInteger other = BigInteger(rhs);

    this->AssertValid();
    other.AssertValid();

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

bool BigInteger::Equals(const BigInteger& other) const
{
    AssertValid();
    other.AssertValid();

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

bool BigInteger::TryParse(std::string value, BigInteger& result)
{
    return BigNumber::TryParseBigInteger(value, result);
}

BigInteger BigInteger::Parse(std::string value)
{
    return BigNumber::ParseBigInteger(value);
}

std::string BigInteger::ToString()
{
    return BigNumber::FormatBigInteger(*this);
}

bool BigInteger::GetPartsForBitManipulation(BigInteger& x, uint_array& xd, int& xl)
{
    if (x.GetBits().size() == 0)
    {
        if (x.GetSign() < 0)
        {
            auto temp = uint_array(static_cast<uint32_t>(-x.GetSign()));
            xd.swap(temp);
        }
        else
        {
            auto temp = uint_array(static_cast<uint32_t>(x.GetSign()));
            xd.swap(temp);
        }
    }
    else
    {
        xd = x.GetBits();
    }
    xl = (x.GetBits().size() == 0 ? 1 : x.GetBits().size());
    return x.GetSign() < 0;
}

BigInteger BigInteger::operator <<(int shift)
{
    if (shift == 0)
    {
        return *this;
    }
    else if (shift == std::numeric_limits<int>::min())
    {
        return ((*this>> std::numeric_limits<int>::max()) >> 1);
    }
    else if (shift < 0)
    {
        return *this >> -shift;
    }

    int digitShift = shift / kcbitUint;
    int smallShift = shift - (digitShift * kcbitUint);

    uint_array xd; int xl; bool negx;
    negx = BigInteger::GetPartsForBitManipulation(*this, xd, xl);

    int zl = xl + digitShift + 1;
    uint_array zd(zl);

    if (smallShift == 0)
    {
        for (int i = 0; i < xl; i++)
        {
            zd[i + digitShift] = xd[i];
        }
    }
    else
    {
        int carryShift = kcbitUint - smallShift;
        uint32_t carry = 0;
        int i;
        for (i = 0; i < xl; i++)
        {
            uint rot = xd[i];
            zd[i + digitShift] = rot << smallShift | carry;
            carry = rot >> carryShift;
        }
        zd[i + digitShift] = carry;
    }
    return BigInteger(zd, negx);

}

void BigInteger::DangerousMakeTwosComplement(uint_array& d)
{
    //dangerous because mutates d!
    if (d.size() > 0)
    {
        d[0] = ~d[0] + 1;

        size_t i = 1;
        // first do complement and +1 as long as carry is needed
        for (; d[i - 1] == 0 && i < d.size(); i++)
        {
            d[i] = ~d[i] + 1;
        }
        // now ones complement is sufficient
        for (; i < d.size(); i++)
        {
            d[i] = ~d[i];
        }
    }
}

uint_array BigInteger::ToUInt32Array()
{
    if (_bits.size() == 0 && _sign == 0)
        return uint_array();

    uint_array dwords;
    uint32_t highDWord;

    if (_bits.size() == 0)
    {
        dwords = uint_array(static_cast<uint32_t>(_sign));
        highDWord = (_sign < 0) ? std::numeric_limits<uint32_t>::max() : 0;
    }
    else if (_sign == -1)
    {
        dwords = _bits;
        DangerousMakeTwosComplement(dwords);  // Mutates dwords
        highDWord = std::numeric_limits<uint32_t>::max();
    }
    else
    {
        dwords = _bits;
        highDWord = 0;
    }

    // Find highest significant byte
    int msb;
    for (msb = dwords.size() - 1; msb > 0; msb--)
    {
        if (dwords[msb] != highDWord) break;
    }
    // Ensure high bit is 0 if positive, 1 if negative
    bool needExtraByte = (dwords[msb] & 0x80000000) != (highDWord & 0x80000000);

    //uint_array trimmed = new uint[msb + 1 + (needExtraByte ? 1 : 0)];
    uint_array trimmed;
    trimmed.swap(dwords);

    if (needExtraByte) trimmed[trimmed.size() - 1] = highDWord;
    return trimmed;
}

BigInteger BigInteger::operator ^(BigInteger& rhs)
{
    BigInteger lhs = *this;
    if (lhs._bits.size() == 0 && rhs._bits.size() == 0)
    {
        return lhs._sign ^ rhs._sign;
    }

    uint_array x = lhs.ToUInt32Array();
    uint_array y = rhs.ToUInt32Array();
    uint_array z;;
    uint32_t xExtend = (lhs._sign < 0) ? std::numeric_limits<uint32_t>::min() : 0;
    uint32_t yExtend = (rhs._sign < 0) ? std::numeric_limits<uint32_t>::max() : 0;

    for (size_t i = 0; i < z.size(); i++)
    {
        uint32_t xu = (i < x.size()) ? x[i] : xExtend;
        uint32_t yu = (i < y.size()) ? y[i] : yExtend;
        z[i] = xu ^ yu;
    }

    return BigInteger(z);

}

BigInteger BigInteger::operator >>(int shift)
{
    if (shift == 0)
    {
        return *this;
    }
    else if (shift == std::numeric_limits<int>::min())
    {
        return ((*this << std::numeric_limits<int>::max()) << 1);
    }
    else if (shift < 0)
    {
        return *this << -shift;
    }

    int digitShift = shift / kcbitUint;
    int smallShift = shift - (digitShift * kcbitUint);

    uint_array xd; int xl; bool negx;
    negx = BigInteger::GetPartsForBitManipulation(*this, xd, xl);

    if (negx)
    {
        if (shift >= (kcbitUint * xl))
        {
            return s_bnMinusOneInt;
        }
        uint_array temp = xd;
        xd = temp;
        BigInteger::DangerousMakeTwosComplement(xd); // Mutates xd
    }

    int zl = xl - digitShift;
    if (zl < 0) zl = 0;
    uint_array zd(zl);

    if (smallShift == 0)
    {
        for (int i = xl - 1; i >= digitShift; i--)
        {
            zd[i - digitShift] = xd[i];
        }
    }
    else
    {
        int carryShift = kcbitUint - smallShift;
        uint32_t carry = 0;
        for (int i = xl - 1; i >= digitShift; i--)
        {
            uint32_t rot = xd[i];
            if (negx && i == xl - 1)
                // Sign-extend the first shift for negative ints then let the carry propagate
                zd[i - digitShift] = (rot >> smallShift) | (0xFFFFFFFF << carryShift);
            else
                zd[i - digitShift] = (rot >> smallShift) | carry;
            carry = rot << carryShift;
        }
    }
    if (negx)
    {
        BigInteger::DangerousMakeTwosComplement(zd); // Mutates zd
    }
    return BigInteger(zd, negx);

}

BigInteger BigInteger::operator &(BigInteger& rhs)
{
    BigInteger lhs = *this;
    if (lhs.IsZero() || rhs.IsZero())
    {
        return BigInteger::Zero();
    }

    if (lhs.GetBits().size() == 0 && rhs.GetBits().size() == 0)
    {
        return lhs.GetSign() & rhs.GetSign();
    }

    uint_array x = lhs.ToUInt32Array();
    uint_array y = rhs.ToUInt32Array();
    uint_array z;
    uint xExtend = (lhs.GetSign() < 0) ? std::numeric_limits<uint32_t>::max() : 0;
    uint yExtend = (rhs.GetSign() < 0) ? std::numeric_limits<uint32_t>::max() : 0;

    for (size_t i = 0; i < z.size(); i++)
    {
        uint xu = (i < x.size()) ? x[i] : xExtend;
        uint yu = (i < y.size()) ? y[i] : yExtend;
        z[i] = xu & yu;
    }
    return BigInteger(z);

}

BigInteger BigInteger::Negate(BigInteger& value)
{
    return -value;
}

int Compare(BigInteger& lhs, BigInteger& rhs)
{
    return lhs.CompareTo(rhs);
}

static BigInteger& Max(BigInteger& lhs, BigInteger& rhs)
{
    if (lhs.CompareTo(rhs) < 0)
        return rhs;
    return lhs;
}

static BigInteger& Min(BigInteger& lhs, BigInteger& rhs)
{
    if (lhs.CompareTo(rhs) <= 0)
        return lhs;
    return rhs;
}

BigInteger BigInteger::operator |(BigInteger& rhs)
{
    BigInteger lhs = *this;
    if (lhs.IsZero())
        return rhs;
    if (rhs.IsZero())
        return lhs;

    if (lhs._bits.size() == 0 && rhs._bits.size() == 0)
    {
        return lhs._sign | rhs._sign;
    }

    uint_array x = lhs.ToUInt32Array();
    uint_array y = rhs.ToUInt32Array();
    uint_array z(std::max(x.size(), y.size()));
    uint xExtend = (lhs._sign < 0) ? std::numeric_limits<uint32_t>::max() : 0;
    uint yExtend = (rhs._sign < 0) ? std::numeric_limits<uint32_t>::max() : 0;

    for (size_t i = 0; i < z.size(); i++)
    {
        uint xu = (i < x.size()) ? x[i] : xExtend;
        uint yu = (i < y.size()) ? y[i] : yExtend;
        z[i] = xu | yu;
    }
    return BigInteger(z);
}

int BigInteger::GetByteCount(bool isUnsigned)
{
    int bytesWritten = 0;
    this->ToByteArray(GetBytesMode::Count, false, false, &bytesWritten);

    return bytesWritten;
}

byte_array BigInteger::ToByteArray(bool isUnsigned, bool isBigEndian)
{
    int ignored = 0;
    return this->ToByteArray(GetBytesMode::AllocateArray, false, false, &ignored);
}

byte_array BigInteger::ToByteArray(GetBytesMode mode, bool isUnsigned, bool isBigEndian, int* bytesWritten)
{
    //int bytesWritten = 0;
    //assert(mode == GetBytesMode.AllocateArray || mode == GetBytesMode.Count || mode == GetBytesMode.Span, $"Unexpected mode {mode}.");
    //assert(mode == GetBytesMode.Span || destination.IsEmpty, $"If we're not in span mode, we shouldn't have been passed a destination.");

    int sign = _sign;
    if (sign == 0)
    {
        switch (mode)
        {
            case GetBytesMode::Count:
                *bytesWritten = 1;
                return byte_array();
            default:
                *bytesWritten = 1;
                return byte_array(1,0);
        }
    }

    if (isUnsigned && sign < 0)
    {
        throw std::runtime_error("Overflow error can't have unsigned & sing < 0");
    }

    byte highByte;
    int nonZeroDwordIndex = 0;
    uint32_t highDword;
    uint_array bits = _bits;
    if (bits.size() == 0)
    {
        highByte = static_cast<byte>((sign < 0) ? 0xff : 0x00);
        highDword = static_cast<uint32_t>(sign);
    }
    else if (sign == -1)
    {
        highByte = 0xff;

        // If sign is -1, we will need to two's complement bits.
        // Previously this was accomplished via NumericsHelpers.DangerousMakeTwosComplement()
        // however, we can do the two's complement on the stack so as to avoid
        // creating a temporary copy of bits just to hold the two's complement.
        // One special case in DangerousMakeTwosComplement() is that if the array
        // is all zeros, then it would allocate a new array with the high-order
        // uint set to 1 (for the carry). In our usage, we will not hit this case
        // because a bits array of all zeros would represent 0, and this case
        // would be encoded as _bits = null and _sign = 0.
        assert(bits.size() > 0);
        assert(bits[bits.size() - 1] != 0);
        while (bits[nonZeroDwordIndex] == 0U)
        {
            nonZeroDwordIndex++;
        }

        highDword = ~bits[bits.size() - 1];
        if (bits.size() - 1 == nonZeroDwordIndex)
        {
            // This will not overflow because highDword is less than or equal to uint.MaxValue - 1.
            assert(highDword <= std::numeric_limits<uint32_t>::max() - 1);
            highDword += 1U;
        }
    }
    else
    {
        assert(sign == 1);
        highByte = 0x00;
        highDword = bits[bits.size() - 1];
    }

    byte msb;
    int msbIndex;
    if ((msb = static_cast<byte>((highDword >> 24))) != highByte)
    {
        msbIndex = 3;
    }
    else if ((msb = static_cast<byte>((highDword >> 16))) != highByte)
    {
        msbIndex = 2;
    }
    else if ((msb = static_cast<byte>((highDword >> 8))) != highByte)
    {
        msbIndex = 1;
    }
    else
    {
        msb = static_cast<byte>(highDword);
        msbIndex = 0;
    }

    // Ensure high bit is 0 if positive, 1 if negative
    bool needExtraByte = (msb & 0x80) != (highByte & 0x80) && !isUnsigned;
    int length = msbIndex + 1 + (needExtraByte ? 1 : 0);
    if (bits.size() != 0)
    {
        length = 4 * (bits.size() - 1) + length;
    }

    byte_array destination;
    switch (mode)
    {
        case GetBytesMode::Count:
            *bytesWritten = length;
            return byte_array();
        default:
            destination = byte_array(length);
            break;
    }

    int curByte = isBigEndian ? length - 1 : 0;
    int increment = isBigEndian ? -1 : 1;

    if (bits.size() != 0)
    {
        for (size_t i = 0; i < bits.size() - 1; i++)
        {
            uint dword = bits[i];

            if (sign == -1)
            {
                dword = ~dword;
                if (i <= nonZeroDwordIndex)
                {
                    dword = dword + 1U;
                }

            }

            destination[curByte] = static_cast<byte>(dword);
            curByte += increment;
            destination[curByte] = static_cast<byte>((dword >> 8));
            curByte += increment;
            destination[curByte] = static_cast<byte>((dword >> 16));
            curByte += increment;
            destination[curByte] = static_cast<byte>((dword >> 24));
            curByte += increment;
        }
    }

    assert(msbIndex >= 0 && msbIndex <= 3);
    destination[curByte] = static_cast<byte>(highDword);
    if (msbIndex != 0)
    {
        curByte += increment;
        destination[curByte] = static_cast<byte>((highDword >> 8));
        if (msbIndex != 1)
        {
            curByte += increment;
            destination[curByte] = static_cast<byte>((highDword >> 16));
            if (msbIndex != 2)
            {
                curByte += increment;
                destination[curByte] = static_cast<byte>((highDword >> 24));
            }
        }
    }

    // Assert we're big endian, or little endian consistency holds.
    assert(isBigEndian || (!needExtraByte && curByte == length - 1) || (needExtraByte && curByte == length - 2));
    // Assert we're little endian, or big endian consistency holds.
    assert(!isBigEndian || (!needExtraByte && curByte == 0) || (needExtraByte && curByte == 1));

    if (needExtraByte)
    {
        curByte += increment;
        destination[curByte] = highByte;
    }

    return destination;
}

void BigInteger::AssertValid() const
{

    if (!_bits.empty())
    {
        assert(_sign == 1 || _sign == -1);
        assert(_bits.size() > 0);
        assert(_bits.size() > 1 || _bits[0] >= kuMaskHighBit);
        //for (auto a : _bits)
        //{
        //    std::cout << "bits: " << a << std::endl;
        //}
        assert(_bits[_bits.size() - 1] != 0);
    }
    else
    {
        //std::cout << "_sign: " << _sign << " intMin: " << std::numeric_limits<int>::min() << std::endl;
        assert(_sign > std::numeric_limits<int>::min());
    }
}

BigInteger::operator int() {
    if (_bits.empty())
        return _sign;

    if (_bits.size() > 1)
        throw std::overflow_error("SR.Overflow_Int32");

    if (_sign > 0) {
        if (_bits[0] > INT32_MAX)
            throw std::overflow_error("checked() size > INT32_MAX");
        else
            return static_cast<int>(_bits[0]);
    }
    if (_bits[0] > kuMaskHighBit)
        throw std::overflow_error("SR.Overflow_Int32");

    return -static_cast<int>(_bits[0]);
}

BigInteger::operator unsigned int() {
    if (_bits.empty()) {
        if (_sign < 0)
            throw std::overflow_error("checked() UInt32 overflow");
        else
            return static_cast<unsigned int>(_sign);
    } else if (_bits.size() > 1 || _sign < 0) {
        throw std::overflow_error("SR.Overflow_UInt32");
    } else {
        return _bits[0];
    }
}

BigInteger::operator long() {
    if (_bits.empty())
        return _sign;

    int len = _bits.size();
    if (len > 2)
        throw std::overflow_error("SR.Overflow_Int64");

    unsigned long uu;
    if (len > 1) {
        uu = (static_cast<unsigned long>(_bits[1]) << kcbitUint) | _bits[0];
    } else {
        uu = _bits[0];
    }

    long ll = _sign > 0 ? static_cast<long>(uu) : -static_cast<long>(uu);
    if ((ll > 0 && _sign > 0) || (ll < 0 && _sign < 0))
        return ll;
    throw std::overflow_error("SR.Overflow_Int64");
}

BigInteger::operator unsigned long() {
    if (_bits.empty()) {
        if (_sign < 0)
            throw std::overflow_error("checked() UInt32 overflow");
        else
            return static_cast<unsigned long>(_sign);
    }

    int len = _bits.size();
    if (len > 2 || _sign < 0) {
        throw std::overflow_error("SR.Overflow_UInt64");
    }

    if (len > 1) {
        return (static_cast<unsigned long>(_bits[1]) << kcbitUint) | _bits[0];
    }
    return _bits[0];
}

BigInteger::operator double() {
    if (_bits.empty())
        return _sign;

    int length = _bits.size();
    const int InfinityLength = 1024 / kcbitUint;

    if (length > InfinityLength) {
        if (_sign == 1)
            return static_cast<double>(1.0)/static_cast<double>(0.0); // PositiveInfinity
        else
            return static_cast<double>(-1.0)/static_cast<double>(0.0); // NegativeInfinity
    }

    unsigned long h = _bits[length -1];
    unsigned long m = length > 1 ? _bits[length - 2] : 0;
    unsigned long l = length > 2 ? _bits[length - 3] : 0;

    int z = NumericsHelpers::CbitHighZero(static_cast<unsigned int>(h));

    int exp = (length - 2) * 32 - z;
    ulong man = (h << (32 + z)) | (m << z) | (l >> (32 - z));

    return NumericsHelpers::GetDoubleFromParts(_sign, exp, man);
}

BigInteger::operator float() {
    return static_cast<float>(static_cast<double>(*this));
}

double BigInteger::Log(BigInteger value) {
    return Log(value, 2.7182818284590451);
}

double BigInteger::Log(BigInteger value, double baseValue) {
    if (value._sign < 0 || baseValue == 1.0)
        return double_NaN;

    if (baseValue == double_PositiveInfinity)
        return value.IsOne() ? 0.0 : double_NaN;

    if (baseValue == 0.0 && !value.IsOne())
        return double_NaN;

    if (value._bits.empty())
        return log(value) / log(baseValue);

    uint64_t h = value._bits[value._bits.size() - 1];
    uint64_t m = value._bits.size() > 1 ? value._bits[value._bits.size() - 2] : 0;
    uint64_t l = value._bits.size() > 2 ? value._bits[value._bits.size() - 3] : 0;

    int c = NumericsHelpers::CbitHighZero(static_cast<unsigned int>(h));
    int64_t b = static_cast<int64_t>(value._bits.size() * 32 - c);

    uint64_t x = (h << (32 + c)) | (m << c) | (l >> (32 - c));
    return (log(x) / log(baseValue)) + (b - 64) / log2(baseValue);
}

double BigInteger::Log10(BigInteger value) {
    return Log(value, 10);
}

BigInteger BigInteger::Pow(BigInteger value, int exponent) {
    if (exponent < 0)
        throw std::out_of_range("SR.ArgumentOutOfRange_MustBeNonNeg");

    value.AssertValid();

    if (exponent == 0)
        return s_bnOneInt;
    if (exponent == 1)
        return value;

    bool trivialValue = value._bits.empty();

    if (trivialValue) {
        if (value._sign == 1)
            return value;
        if (value._sign == -1)
            return (exponent & 1) != 0 ? value : s_bnOneInt;
        if (value._sign == 0)
            return value;
    }
    uint_array bits = trivialValue
            ? BigIntegerCalculator::Pow(abs(value._sign), abs(exponent))
            : BigIntegerCalculator::Pow(value._bits, abs(exponent));

    return BigInteger(bits, value._sign < 0 && (exponent & 1) != 0);
}

void BigInteger::SubtractSelf(uint32_t *left, int leftLength, uint32_t *right, int rightLength) {
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

