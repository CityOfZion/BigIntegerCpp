#include "BigInteger.h"

#include <limits>
#include <cassert>
#include <cstdlib>
#include <any>

const BigInteger BigInteger::s_bnMinInt = BigInteger(-1, uint_array {kuMaskHighBit});
const BigInteger BigInteger::s_bnOneInt = BigInteger(1);
const BigInteger BigInteger::s_bnZeroInt = BigInteger(0);
const BigInteger BigInteger::s_bnMinusOneInt = BigInteger(-1);


// BigInteger BigInteger::Zero()
//{
//
//	return s_bnZeroInt;
//}

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
        ulong x = 0;
        if (value < 0)
        {
            x = (ulong)-value;
            _sign = -1;
        }
        else
        {
            x = (ulong)value;
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

    AssertValid();

}

BigInteger::BigInteger(double value) {

    AssertValid();
}

BigInteger operator -(BigInteger& lhs, BigInteger& rhs)
{

}

BigInteger Add(BigInteger lhs, BigInteger rhs)
{
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

    uint_array* l = &lhs;
    uint_array* r = &rhs;
    uint_array* b = &bits;
    BigInteger::Add(l, lhs.size(), r, rhs.size(), b, bits.size());

    return bits;
}

static void Add(uint_array* lhs, int lhsLength
        , uint_array* rhs, int rhsLength, uint_array* bits, int bitsLength)
{
    assert(lhsLength >= 0);
    assert(rhsLength >= 0);
    assert(lhsLength >= rhsLength);
    assert(bitsLength == lhsLength + 1);

    int i = 0;
    long carry = 0L;

    uint_array lhsr = *lhs;
    uint_array rhsr = *rhs;
    uint_array bitsr = *bits;

    for (; i < rhsLength; i++)
    {
        long digit = (lhsr[i] + carry) + rhsr[i];
        bitsr[i] = static_cast<uint32_t>(digit);
        carry = digit >> 32;
    }

    for (; i < lhsLength; i++)
    {
        long digit = lhsr[i] + carry;
        bitsr[i] = static_cast<uint32_t>(digit);
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

    // Switching to unsafe pointers helps sparing
    // some nasty index calculations...

    uint_array bits(lhs.size());

    uint_array* l = &lhs;
    uint_array* r = &rhs;
    uint_array* b = &bits;
    BigInteger::Subtract(l, lhs.size(), r, rhs.size(), b, bits.size());

    return bits;
}

static void Subtract(uint_array* lhs, int lhsLength
        , uint_array* rhs, int rhsLength, uint_array* bits, int bitsLength)
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

    uint_array lhsr = *lhs;
    uint_array rhsr = *rhs;
    uint_array bitsr = *bits;
    for (; i < rhsLength; i++)
    {
        long digit = (lhsr[i] + carry) - rhsr[i];
        bitsr[i] = static_cast<uint32_t>(digit);
        carry = digit >> 32;
    }
    for (; i < lhsLength; i++)
    {
        long digit = lhsr[i] + carry;
        bitsr[i] = static_cast<uint32_t>(digit);
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

int BigInteger::CompareTo(long other)
{
    AssertValid();

    if (!_bits.empty())
        return (static_cast<long>(_sign)) == (other);
    int cu;
    if ((_sign ^ other) < 0 || (cu = _bits.size()) > 2)
        return _sign;
    ulong uu = other < 0 ? static_cast<unsigned long>(-other) : static_cast<unsigned long>(other);
    ulong uuTmp = cu == 2 ? BigInteger::MakeUlong(_bits[1], _bits[0]) : _bits[0];
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

static int Compare(uint_array* lhs, int lhsLength, uint_array* rhs, int rhsLength)
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

static unsigned long MakeUlong(unsigned int uHi, unsigned int uLo)
{
    return ((ulong)uHi << BigInteger::kcbitUint) | uLo;
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

