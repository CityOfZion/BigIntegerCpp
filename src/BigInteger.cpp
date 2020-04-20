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

const BigInteger BigInteger::s_bnMinInt = BigInteger(-1, uint_array {kuMaskHighBit});
const BigInteger BigInteger::s_bnOneInt = BigInteger(1);
const BigInteger BigInteger::s_bnZeroInt = BigInteger(0);
const BigInteger BigInteger::s_bnMinusOneInt = BigInteger(-1);

const double double_NaN = static_cast<double>(0.0)/static_cast<double>(0.0);


BigInteger::BigInteger()
{
	_sign = 0;
    _bits = uint_array();
}

BigInteger::BigInteger(int value)
{
	if (value == std::numeric_limits<int>::min())
	{
		*this = s_bnMinInt;
	}
	else
	{
		_sign = value;
		_bits.clear();
	}
	AssertValid();
}

BigInteger::BigInteger(uint32_t value) {
    if (value <= static_cast<uint32_t>(std::numeric_limits<int>::max()))
    {
        _sign = value;
        _bits.clear();
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

BigInteger::BigInteger(unsigned long value) {
    if (value <= static_cast<unsigned long>(std::numeric_limits<int>::max()))
    {
        _sign = static_cast<int>(value);
        _bits.clear();
    }
    else if (value <= static_cast<unsigned long>(std::numeric_limits<unsigned int>::max()))
    {
        _sign = +1;
        _bits.push_back(static_cast<unsigned int>(value));
    }
    else
    {
        _sign = +1;
        _bits.push_back(static_cast<unsigned int>(value));
        _bits.push_back(static_cast<unsigned int>(value >> kcbitUint));
    }

    AssertValid();
}

BigInteger::BigInteger(long long value)
{
    if (std::numeric_limits<int>::min() < value && value <= std::numeric_limits<int>::max())
    {
        _sign = static_cast<int>(value);
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

        if (x <= std::numeric_limits<uint32_t>::max())
        {
            _bits.push_back(static_cast<uint32_t>(x));
        }
        else
        {
            _bits.push_back(static_cast<uint32_t>(x));
            _bits.push_back(static_cast<uint32_t>(x >> kcbitUint));
        }
    }

    AssertValid();
}

BigInteger::BigInteger(uint64_t value)
{
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
            NumericsHelpers::DangerousMakeTwosComplement(val); // Mutates val

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
    NumericsHelpers::DangerousMakeTwosComplement(value); // mutates val

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
    bool trivialLeft = lhs.empty();
    bool trivialRight = rhs.empty();

    if (trivialLeft && trivialRight)
    {
        return BigInteger(static_cast<long long>(lhsSign) + rhsSign);
    }

    if (trivialLeft)
    {
        assert(!rhs.empty());
        uint_array bits = BigIntegerCalculator::Add(rhs, abs(lhsSign));
        return BigInteger(bits, lhsSign < 0);
    }

    if (trivialRight)
    {
        assert(!lhs.empty());
        uint_array bits = BigIntegerCalculator::Add(lhs, abs(rhsSign));
        return BigInteger(bits, lhsSign < 0);
    }

    assert(!lhs.empty() && !rhs.empty());

    if (lhs.size() < rhs.size())
    {
        uint_array bits = BigIntegerCalculator::Add(rhs, lhs);
        return BigInteger(bits, lhsSign < 0);
    }
    else
    {
        uint_array bits = BigIntegerCalculator::Add(lhs, rhs);
        return BigInteger(bits, lhsSign < 0);
    }
}

BigInteger BigInteger::Subtract(uint_array& lhs, int lhsSign, uint_array& rhs, int rhsSign)
{
    bool trivialLeft = lhs.empty();
    bool trivialRight = rhs.empty();

    if (trivialLeft && trivialRight)
    {
        return BigInteger(static_cast<long long>(lhsSign) - rhsSign);
    }

    if (trivialLeft)
    {
        assert(!rhs.empty());
        uint_array bits = BigIntegerCalculator::Subtract(rhs, abs(lhsSign));
        return BigInteger(bits, lhsSign >= 0);
    }

    if (trivialRight)
    {
        assert(!lhs.empty());
        uint_array bits = BigIntegerCalculator::Subtract(lhs, abs(rhsSign));
        return BigInteger(bits, lhsSign < 0);
    }

    assert(!lhs.empty() && !rhs.empty());

    if (BigIntegerCalculator::Compare(lhs, rhs) < 0)
    {
        uint_array bits = BigIntegerCalculator::Subtract(rhs, lhs);
        return BigInteger(bits, lhsSign >= 0);
    }
    else
    {
        uint_array bits = BigIntegerCalculator::Subtract(lhs, rhs);
        return BigInteger(bits, lhsSign < 0);
    }
}

BigInteger BigInteger::operator -(const BigInteger& right)
{
    BigInteger& rhs = const_cast<BigInteger &>(right);
    BigInteger lhs = *this;
    lhs.AssertValid();
    rhs.AssertValid();

    if ((lhs._sign < 0) != (rhs._sign < 0))
        return BigInteger::Add(lhs._bits, lhs._sign, rhs._bits, -1 * rhs._sign);
    return BigInteger::Subtract(lhs._bits, lhs._sign, rhs._bits, rhs._sign);
}

BigInteger BigInteger::operator+(const BigInteger& right)
{
    BigInteger& rhs = const_cast<BigInteger &>(right);
    BigInteger lhs = *this;
    lhs.AssertValid();
    rhs.AssertValid();

    if ((lhs._sign < 0) != (rhs._sign < 0))
        return BigInteger::Subtract(lhs._bits, lhs._sign, rhs._bits, -1 * rhs._sign);
    return BigInteger::Add(lhs._bits, lhs._sign, rhs._bits, rhs._sign);
}

BigInteger BigInteger::operator*(const BigInteger& rhs)
{
    BigInteger lhs = *this;
    lhs.AssertValid();
    rhs.AssertValid();

    bool trivialLeft = lhs._bits.empty();
    bool trivialRight = rhs._bits.empty();

    if (trivialLeft && trivialRight)
        return BigInteger(static_cast<long long>(lhs._sign) * rhs._sign);

    if (trivialLeft)
    {
        assert(!rhs._bits.empty());
        uint_array bits = BigIntegerCalculator::Multiply(rhs._bits, abs(lhs._sign));
        return BigInteger(bits, (lhs._sign < 0) ^ (rhs._sign < 0));
    }

    if (trivialRight)
    {
        assert(!lhs._bits.empty());
        uint_array bits = BigIntegerCalculator::Multiply(lhs._bits, abs(rhs._sign));
        return BigInteger(bits, (lhs._sign < 0) ^ (rhs._sign < 0));
    }

    assert(!lhs._bits.empty() && !rhs._bits.empty());

    if (lhs._bits == rhs._bits)
    {
        uint_array bits = BigIntegerCalculator::Square(lhs._bits);
        return BigInteger(bits, (lhs._sign < 0) ^ (rhs._sign < 0));
    }

    if (lhs._bits.size() < rhs._bits.size())
    {
        uint_array bits = BigIntegerCalculator::Multiply(rhs._bits, lhs._bits);
        return BigInteger(bits, (lhs._sign < 0) ^ (rhs._sign < 0));
    }
    else
    {
        uint_array bits = BigIntegerCalculator::Multiply(lhs._bits, rhs._bits);
        return BigInteger(bits, (lhs._sign < 0) ^ (rhs._sign < 0));
    }
}

BigInteger BigInteger::Multiply(BigInteger& lhs, BigInteger& rhs)
{
    return lhs * rhs;
}

BigInteger BigInteger::operator%(const BigInteger& div)
{
    BigInteger divisor = div;
    BigInteger dividend = *this;
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
        long remainder = BigIntegerCalculator::Remainder(dividend._bits, abs(divisor._sign));
        return dividend._sign < 0 ? -1 * remainder : remainder;
    }

    assert(!dividend._bits.empty() && !divisor._bits.empty());

    if (dividend._bits.size() < divisor._bits.size())
    {
        return dividend;
    }

    uint_array bits = BigIntegerCalculator::Remainder(dividend._bits, divisor._bits);
    return BigInteger(bits, dividend._sign < 0);
}

BigInteger BigInteger::DivRem(BigInteger& dividend, BigInteger& divisor, BigInteger& remainder)
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
       uint_array bits = BigIntegerCalculator::Divide(rbits, abs(divisor.GetSign()), rest);

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
       uint_array bits = BigIntegerCalculator::Divide(lbits, rbits, rest);

       remainder = BigInteger(rest, dividend.GetSign() < 0);
       return BigInteger(bits, (dividend.GetSign() < 0) ^ (divisor.GetSign() < 0));
   }
}

BigInteger BigInteger::operator -()
{
    return BigInteger(-(*this)._sign, (*this)._bits);
}

BigInteger BigInteger::operator+()
{
    return BigInteger(+(*this)._sign, (*this)._bits);
}

BigInteger& BigInteger::operator++()
{
    BigInteger&& one = BigInteger::One();
    *this = *this + one;
    return *this;
}

BigInteger& BigInteger::operator--()
{
    //value.AssertValid();
    BigInteger&& one = BigInteger::One();
    *this = *this - one;
    return *this;
}

BigInteger BigInteger::operator+=(const uint64_t rhs)
{
    BigInteger value = BigInteger(rhs);
    *this = (*this + value);
    return *this;
}

BigInteger BigInteger::operator+=(const BigInteger& rhs)
{
    rhs.AssertValid();
    *this = (*this + rhs);
    return *this;
}

BigInteger BigInteger::operator-=(const BigInteger& rhs)
{
    rhs.AssertValid();
    *this = (*this - rhs);
    return *this;
}

BigInteger BigInteger::operator /=(const BigInteger& rhs)
{
    rhs.AssertValid();
    *this = (*this / rhs);
    return *this / rhs;
}

BigInteger BigInteger::operator *=(const BigInteger& rhs)
{
    *this = (*this * rhs);
    return *this;
}

bool BigInteger::operator ==(const BigInteger& rhs)
{
    return this->Equals(rhs);
}

bool BigInteger::operator !=(const BigInteger& rhs)
{
    return !(this->Equals(rhs));
}

BigInteger BigInteger::operator/(const BigInteger& div)
{
    BigInteger& divisor = const_cast<BigInteger &>(div);
    BigInteger dividend = *this;
    dividend.AssertValid();
    divisor.AssertValid();

    bool trivialDividend = dividend._bits.empty();
    bool trivialDivisor = divisor._bits.empty();

    if (trivialDividend && trivialDivisor)
        return static_cast<long long>(dividend._sign / divisor._sign);

    if (trivialDividend)
    {
        // The divisor is non-trivial
        // and therefore the bigger one
        return BigInteger::s_bnZeroInt;
    }

    if (trivialDivisor)
    {
        assert(!dividend._bits.empty());
        uint_array bits = BigIntegerCalculator::Divide(dividend._bits, abs(divisor._sign));
        return BigInteger(bits, (dividend._sign < 0) ^ (divisor._sign < 0));
    }

    assert(!dividend._bits.empty() && !divisor._bits.empty());

    if (dividend._bits.size() < divisor._bits.size())
    {
        return BigInteger::s_bnZeroInt;
    }
    else
    {
        uint_array bits = BigIntegerCalculator::Divide(dividend._bits, divisor._bits);
        return BigInteger(bits, (dividend._sign < 0) ^ (divisor._sign < 0));
    }
}

BigInteger BigInteger::Divide(BigInteger& dividend, BigInteger& divisor)
{
    return dividend / divisor;
}

int BigInteger::CompareTo(const long other) const
{
    AssertValid();

    if (!_bits.empty())
        return (static_cast<long>(_sign)) == (other);
    int cu;
    if ((_sign ^ other) < 0 || (cu = _bits.size()) > 2)
        return _sign;
    uint64_t uu = other < 0 ? static_cast<unsigned long>(-other) : static_cast<unsigned long>(other);
    uint64_t uuTmp = cu == 2 ? NumericsHelpers::MakeUlong(_bits[1], _bits[0]) : _bits[0];
    return (_sign * uuTmp) == uu;
}

int BigInteger::CompareTo(const BigInteger& other) const
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
            xd.push_back(static_cast<uint32_t>(-x.GetSign()));
        }
        else
        {
            xd.push_back(static_cast<uint32_t>(x.GetSign()));
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
    negx = GetPartsForBitManipulation(*this, xd, xl);

    int zl = xl + digitShift + 1;
    uint_array zd(zl, 0);

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
            unsigned int rot = xd[i];
            zd[i + digitShift] = rot << smallShift | carry;
            carry = rot >> carryShift;
        }
        zd[i + digitShift] = carry;
    }
    return BigInteger(zd, negx);
}

uint_array BigInteger::ToUInt32Array()
{
    if (_bits.size() == 0 && _sign == 0)
        return uint_array();

    uint_array dwords;
    uint32_t highDWord;

    if (_bits.size() == 0)
    {
        dwords = uint_array{static_cast<uint32_t>(_sign)};
        highDWord = (_sign < 0) ? std::numeric_limits<uint32_t>::max() : 0;
    }
    else if (_sign == -1)
    {
        dwords = _bits;
        NumericsHelpers::DangerousMakeTwosComplement(dwords);  // Mutates dwords
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
    uint_array trimmed(msb + 1 + (needExtraByte ? 1 : 0), 0);
    std::copy(dwords.begin(), dwords.begin()+(msb+1), trimmed.begin());

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
    uint_array z(std::max(x.size(), y.size()), 0);
    uint32_t xExtend = (lhs._sign < 0) ? std::numeric_limits<uint32_t>::max() : 0;
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
    negx = GetPartsForBitManipulation(*this, xd, xl);

    if (negx)
    {
        if (shift >= (kcbitUint * xl))
        {
            return s_bnMinusOneInt;
        }
        uint_array temp = xd;
        xd = temp;
        NumericsHelpers::DangerousMakeTwosComplement(xd); // Mutates xd
    }

    int zl = xl - digitShift;
    if (zl < 0) zl = 0;
    uint_array zd(zl, 0);

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
        NumericsHelpers::DangerousMakeTwosComplement(zd); // Mutates zd
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
        assert(_bits[_bits.size() - 1] != 0);
    }
    else
    {
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
            return std::numeric_limits<double>::infinity();
        else
            return -std::numeric_limits<double>::infinity();
    }

    unsigned long h = _bits[length -1];
    unsigned long m = length > 1 ? _bits[length - 2] : 0;
    unsigned long l = length > 2 ? _bits[length - 3] : 0;

    int z = NumericsHelpers::CbitHighZero(static_cast<unsigned int>(h));

    int exp = (length - 2) * 32 - z;
    unsigned long man = (h << (32 + z)) | (m << z) | (l >> (32 - z));

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

    if (baseValue == std::numeric_limits<double>::infinity())
        return value.IsOne() ? 0.0 : double_NaN;

    if (baseValue == 0.0 && !value.IsOne())
        return double_NaN;

    if (value._bits.empty())
        return log(value._sign) / log(baseValue);

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



BigInteger BigInteger::ModPow(BigInteger value, BigInteger exponent, BigInteger modulus) {
    if (exponent._sign < 0)
        throw std::out_of_range("SR.ArgumentOutOfRange_MustBeNonNeg");

    value.AssertValid();
    exponent.AssertValid();
    modulus.AssertValid();

    bool trivialValue = value._bits.empty();
    bool trivialExponent = exponent._bits.empty();
    bool trivialModulus = modulus._bits.empty();

    if (trivialModulus) {
        uint32_t bits;
        if (trivialValue) {
            if (trivialExponent)
                bits = BigIntegerCalculator::Pow(abs(value._sign), abs(exponent._sign), abs(modulus._sign));
            else
                bits = BigIntegerCalculator::Pow(abs(value._sign), exponent._bits, abs(modulus._sign));
        } else {
            if (trivialExponent)
                bits = BigIntegerCalculator::Pow(value._bits, abs(exponent._sign), abs(modulus._sign));
            else
                bits = BigIntegerCalculator::Pow(value._bits, exponent._bits, abs(modulus._sign));
        }
        return value._sign < 0 && !exponent.IsEven() ? -1 * bits : bits;
    } else {
        uint_array bits;
        if (trivialValue) {
            if (trivialExponent)
                bits = BigIntegerCalculator::Pow(abs(value._sign), abs(exponent._sign), modulus._bits);
            else
                bits = BigIntegerCalculator::Pow(abs(value._sign), exponent._bits, modulus._bits);
        } else {
            if (trivialExponent)
                bits = BigIntegerCalculator::Pow(value._bits, abs(exponent._sign), modulus._bits);
            else
                bits = BigIntegerCalculator::Pow(value._bits, exponent._bits, modulus._bits);
        }

        return BigInteger(bits, value._sign < 0 && !exponent.IsEven());
    }
}

BigInteger BigInteger::Remainder(BigInteger& dividend, BigInteger& divisor)
{
    return dividend % divisor;
}

BigInteger BigInteger::GreatestCommonDivisor(BigInteger &left, BigInteger &right) {
    left.AssertValid();
    right.AssertValid();

    bool trivialLeft = left._bits.empty();
    bool trivialRight = right._bits.empty();

    if (trivialLeft && trivialRight)
    {
        return BigIntegerCalculator::Gcd((uint32_t)abs(left._sign), abs(right._sign));
    }

    if (trivialLeft)
    {
        assert(!right._bits.empty());
        if (left._sign != 0) {
            auto result = BigIntegerCalculator::Gcd(right._bits, abs(left._sign));
            return BigInteger{result};
        } else {
            return BigInteger(right._bits, false);
        }
    }

    if (trivialRight)
    {
        assert(!left._bits.empty());
        if (left._sign != 0) {
            auto result = BigIntegerCalculator::Gcd(left._bits, abs(right._sign));
            return BigInteger{result};
        } else {
            return BigInteger(left._bits, false);
        }
    }

    assert(!left._bits.empty() && !right._bits.empty());

    if (BigIntegerCalculator::Compare(left._bits, right._bits) < 0)
    {
        return GreatestCommonDivisor(right._bits, left._bits);
    }
    else
    {
        return GreatestCommonDivisor(left._bits, right._bits);
    }
}

BigInteger BigInteger::GreatestCommonDivisor(uint_array leftBits, uint_array rightBits)
{
    assert(BigIntegerCalculator::Compare(leftBits, rightBits) >= 0);

    // Short circuits to spare some allocations...
    if (rightBits.size() == 1)
    {
        uint temp = BigIntegerCalculator::Remainder(leftBits, rightBits[0]);
        return BigIntegerCalculator::Gcd(rightBits[0], temp);
    }

    if (rightBits.size() == 2)
    {
        auto tempBits = BigIntegerCalculator::Remainder(leftBits, rightBits);

        unsigned long left = (static_cast<unsigned long>(rightBits[1]) << 32) | rightBits[0];
        unsigned long right = (static_cast<unsigned long>(tempBits[1]) << 32) | tempBits[0];

        unsigned long res = BigIntegerCalculator::Gcd(left, right);
        return BigInteger{res};
    }

    auto bits = BigIntegerCalculator::Gcd(leftBits, rightBits);
    return BigInteger(bits, false);
}

BigInteger BigInteger::operator~() {
    auto res = this + One();
    return -(*res);
}

BigInteger BigInteger::Abs(BigInteger &value) {
    return (value >= Zero()) ? value : -value;
}

BigInteger::BigInteger(double value) {
    if (!double_IsFinite(value))
    {
        if (double_IsInfinity(value))
        {
            throw std::overflow_error("SR.Overflow_BigIntInfinity");
        }
        else // NaN
        {
            throw std::overflow_error("SR.Overflow_NotANumber");
        }
    }

    _sign = 0;
    _bits.clear();

    int sign, exp;
    unsigned long man;
    bool fFinite;
    NumericsHelpers::GetDoubleParts(value, sign, exp, man, fFinite);
    assert(sign == +1 || sign == -1);

    if (man == 0)
    {
        *this = Zero();
        return;
    }

    assert(man < (1UL << 53));
    assert(exp <= 0 || man >= (1UL << 52));

    if (exp <= 0)
    {
        if (exp <= -kcbitUlong)
        {
            *this = Zero();
            return;
        }
        *this = man >> -exp;
        if (sign < 0)
            _sign = -_sign;
    }
    else if (exp <= 11)
    {
        *this = man << exp;
        if (sign < 0)
            _sign = -_sign;
    }
    else
    {
        // Overflow into at least 3 uints.
        // Move the leading 1 to the high bit.
        man <<= 11;
        exp -= 11;

        // Compute cu and cbit so that exp == 32 * cu - cbit and 0 <= cbit < 32.
        int cu = (exp - 1) / kcbitUint + 1;
        int cbit = cu * kcbitUint - exp;
        assert(0 <= cbit && cbit < kcbitUint);
        assert(cu >= 1);

        // Populate the uints.
        _bits = uint_array(cu+2, 0);
        _bits[cu + 1] = (uint)(man >> (cbit + kcbitUint));
        _bits[cu] = static_cast<unsigned int>((man >> cbit));
        if (cbit > 0)
            _bits[cu - 1] = static_cast<unsigned int>(man) << (kcbitUint - cbit);
        _sign = sign;
    }

    AssertValid();

}

bool BigInteger::double_IsFinite(double value) {
    auto bits = *((long*)&value);
    return (bits & 0x7FFFFFFFFFFFFFFF) < 0x7FF0000000000000;
}

bool BigInteger::double_IsNaN(double value) {
    auto bits = *((long*)&value);
    return (bits & 0x7FFFFFFFFFFFFFFF) > 0x7FF0000000000000;
}

bool BigInteger::double_IsInfinity(double value) {
    auto bits = *((long*)&value);
    return (bits & 0x7FFFFFFFFFFFFFFF) == 0x7FF0000000000000;
}


