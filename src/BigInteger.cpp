#include "../include/public/bigintegercpp/BigInteger.h"
#include "../include/NumericsHelpers.h"
#include "../include/exceptions.h"

#include <limits>
#include <cassert>
#include <cstdlib>
#include <any>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "../include/BigIntegerCalculator.h"

const BigInteger BigInteger::s_bnMinInt = BigInteger(-1, uint_array{kuMaskHighBit});
const BigInteger BigInteger::s_bnOneInt = BigInteger(1);
const BigInteger BigInteger::s_bnZeroInt = BigInteger(0);
const BigInteger BigInteger::s_bnMinusOneInt = BigInteger(-1);


BigInteger::BigInteger() {
    _sign = 0;
    _bits = uint_array();
}

BigInteger::BigInteger(int32_t value) {
    if (value == std::numeric_limits<int32_t>::min()) {
        *this = s_bnMinInt;
    } else {
        _sign = value;
        _bits.clear();
    }
    assert_valid();
}

BigInteger::BigInteger(uint32_t value) {
    if (value <= static_cast<uint32_t>(std::numeric_limits<int32_t>::max())) {
        _sign = static_cast<int32_t>(value);
        _bits.clear();
    } else {
        _sign = +1;
        _bits.push_back(value);
    }
    assert_valid();
}

BigInteger::BigInteger(int64_t value) {
    if (std::numeric_limits<int32_t>::min() < value && value <= std::numeric_limits<int32_t>::max()) {
        _sign = static_cast<int32_t>(value);
    } else if (value == std::numeric_limits<int32_t>::min()) {
        *this = s_bnMinInt;
    } else {
        uint64_t x = 0;
        if (value < 0) {
            x = static_cast<uint64_t>(-value);
            _sign = -1;
        } else {
            x = static_cast<uint64_t>(value);
            _sign = +1;
        }

        if (x <= std::numeric_limits<uint32_t>::max()) {
            _bits.push_back(static_cast<uint32_t>(x));
        } else {
            _bits.push_back(static_cast<uint32_t>(x));
            _bits.push_back(static_cast<uint32_t>(x >> kcbitUint));
        }
    }

    assert_valid();
}

BigInteger::BigInteger(uint64_t value) {
    if (value <= static_cast<uint64_t>(std::numeric_limits<int32_t>::max())) {
        _sign = static_cast<int32_t>(value);
    } else if (value <= std::numeric_limits<uint32_t>::max()) {
        _sign = +1;
        _bits.push_back(static_cast<uint32_t>(value));
    } else {
        _sign = +1;
        _bits.push_back(static_cast<uint32_t>(value));
        _bits.push_back(static_cast<uint32_t>(value >> kcbitUint));
    }

    assert_valid();
}

BigInteger::BigInteger(int n, uint_array value) {
    _sign = n;
    _bits = value;
    assert_valid();
}

BigInteger::BigInteger(byte_array value, bool isUnsigned, bool isBigEndian) {
    int byteCount = value.size();

    bool isNegative;
    if (byteCount > 0) {
        byte mostSignificantByte = isBigEndian ? value[0] : value[byteCount - 1];
        isNegative = (mostSignificantByte & 0x80) != 0 && !isUnsigned;

        if (mostSignificantByte == 0) {
            // Try to conserve space as much as possible by checking for wasted leading byte[] entries
            if (isBigEndian) {
                int offset = 1;

                while (offset < byteCount && value[offset] == 0) {
                    offset++;
                }

                value = byte_array(value.begin() + offset, value.end());
                byteCount = value.size();
            } else {
                byteCount -= 2;
                while (byteCount >= 0 && value[byteCount] == 0) {
                    byteCount--;
                }
                byteCount++;
            }
        }
    } else {
        isNegative = false;
    }

    if (byteCount == 0) {
        _sign = 0;
        _bits = uint_array();
        assert_valid();
        return;
    }

    if (byteCount <= 4) {
        _sign = isNegative ? static_cast<int>(0xffffffff) : 0;

        if (isBigEndian) {
            for (int i = 0; i < byteCount; i++) {
                _sign = (_sign << 8) | value[i];
            }
        } else {
            for (int i = byteCount - 1; i >= 0; i--) {
                _sign = (_sign << 8) | value[i];
            }
        }

        _bits = uint_array();
        if (_sign < 0 && !isNegative) {
            // Int32 overflow
            // Example: Int64 value 2362232011 (0xCB, 0xCC, 0xCC, 0x8C, 0x0)
            // can be naively packed into 4 bytes (due to the leading 0x0)
            // it overflows into the int32 sign bit
            _bits.push_back(static_cast<uint32_t>(_sign));
            _sign = +1;
        }
        if (_sign == std::numeric_limits<int>::min()) {
            *this = s_bnMinInt;
        }
    } else {
        int unalignedBytes = byteCount % 4;
        int dwordCount = byteCount / 4 + (unalignedBytes == 0 ? 0 : 1);
        uint_array val(dwordCount);
        int byteCountMinus1 = byteCount - 1;

        // Copy all dwords, except don't do the last one if it's not a full four bytes
        int curDword, curByte;

        if (isBigEndian) {
            curByte = byteCount - sizeof(int);
            for (curDword = 0; curDword < dwordCount - (unalignedBytes == 0 ? 0 : 1); curDword++) {
                for (int byteInDword = 0; byteInDword < 4; byteInDword++) {
                    byte curByteValue = value[curByte];
                    val[curDword] = (val[curDword] << 8) | curByteValue;
                    curByte++;
                }

                curByte -= 8;
            }
        } else {
            curByte = sizeof(int) - 1;
            for (curDword = 0; curDword < dwordCount - (unalignedBytes == 0 ? 0 : 1); curDword++) {
                for (int byteInDword = 0; byteInDword < 4; byteInDword++) {
                    byte curByteValue = value[curByte];
                    val[curDword] = (val[curDword] << 8) | curByteValue;
                    curByte--;
                }

                curByte += 8;
            }
        }

        // Copy the last dword specially if it's not aligned
        if (unalignedBytes != 0) {
            if (isNegative) {
                val[dwordCount - 1] = 0xffffffff;
            }

            if (isBigEndian) {
                for (curByte = 0; curByte < unalignedBytes; curByte++) {
                    byte curByteValue = value[curByte];
                    val[curDword] = (val[curDword] << 8) | curByteValue;
                }
            } else {
                for (curByte = byteCountMinus1; curByte >= byteCount - unalignedBytes; curByte--) {
                    byte curByteValue = value[curByte];
                    val[curDword] = (val[curDword] << 8) | curByteValue;
                }
            }
        }

        if (isNegative) {
            NumericsHelpers::dangerous_make_twos_complement(val); // Mutates val

            // Pack _bits to remove any wasted space after the twos complement
            int len = val.size() - 1;
            while (len >= 0 && val[len] == 0) len--;
            len++;

            if (len == 1) {
                switch (val[0]) {
                    case 1: // abs(-1)
                        *this = s_bnMinusOneInt;
                        return;

                    case kuMaskHighBit: // abs(Int32.MinValue)
                        *this = s_bnMinInt;
                        return;

                    default:
                        if (static_cast<int>(val[0]) > 0) {
                            _sign = (-1) * ((int) val[0]);
                            _bits = uint_array();
                            assert_valid();
                            return;
                        }

                        break;
                }
            }

            if (static_cast<size_t>(len) != val.size()) {
                _sign = -1;
                _bits = uint_array();
                _bits = uint_array(val.begin(), val.begin() + len);
            } else {
                _sign = -1;
                _bits = val;
            }
        } else {
            _sign = +1;
            _bits = val;
        }
    }
    assert_valid();
}

BigInteger::BigInteger(uint_array value) {
    int dwordCount = value.size();
    bool isNegative = dwordCount > 0 && ((value[dwordCount - 1] & 0x80000000) == 0x80000000);

    // Try to conserve space as much as possible by checking for wasted leading uint[] entries
    while (dwordCount > 0 && value[dwordCount - 1] == 0) dwordCount--;

    if (dwordCount == 0) {
        // BigInteger.zero
        *this = BigInteger::s_bnZeroInt;
        assert_valid();
        return;
    }
    if (dwordCount == 1) {
        if (static_cast<int>(value[0]) < 0 && !isNegative) {
            _bits.push_back(value[0]);
            _sign = +1;
        }
            // Handle the special cases where the BigInteger likely fits into _sign
        else if (std::numeric_limits<int>::min() == static_cast<int>(value[0])) {
            *this = BigInteger::s_bnMinInt;
        } else {
            _sign = static_cast<int>(value[0]);
        }
        assert_valid();
        return;
    }

    if (!isNegative) {
        // Handle the simple positive value cases where the input is already in sign magnitude
        if (dwordCount != static_cast<int>(value.size())) {
            _sign = +1;
            _bits = uint_array(value.begin(), value.begin() + dwordCount);
        }
            // No trimming is possible.  Assign value directly to _bits.
        else {
            _sign = +1;
            _bits = value;
        }
        assert_valid();
        return;
    }

    // Finally handle the more complex cases where we must transform the input into sign magnitude
    NumericsHelpers::dangerous_make_twos_complement(value); // mutates val

    // Pack _bits to remove any wasted space after the twos complement
    int len = value.size();
    while (len > 0 && value[len - 1] == 0) len--;

    // The number is represented by a single dword
    if (len == 1 && static_cast<int>((value[0])) > 0) {
        if (value[0] == 1 /* abs(-1) */) {
            *this = s_bnMinusOneInt;
        } else if (value[0] == kuMaskHighBit /* abs(Int32.MinValue) */) {
            *this = s_bnMinInt;
        } else {
            _sign = (-1) * ((int) value[0]);
        }
    }
        // The number is represented by multiple dwords.
        // Trim off any wasted uint values when possible.
    else if (len != static_cast<int>(value.size())) {
        _sign = -1;
        _bits = uint_array(value.begin(), value.begin() + len);
    }
        // No trimming is possible.  Assign value directly to _bits.
    else {
        _sign = -1;
        _bits = value;
    }
    assert_valid();
    return;
}


BigInteger::BigInteger(uint_array value, bool negative) {
    int len;

    // Try to conserve space as much as possible by checking for wasted leading uint[] entries
    // sometimes the uint[] has leading zeros from bit manipulation operations & and ^
    for (len = value.size(); len > 0 && value[len - 1] == 0; len--);

    if (len == 0)
        *this = s_bnZeroInt;
        // Values like (Int32.MaxValue+1) are stored as "0x80000000" and as such cannot be packed into _sign
    else if (len == 1 && value[0] < kuMaskHighBit) {
        _sign = (negative ? -(int) value[0] : (int) value[0]);
        _bits = uint_array();
        // Although Int32.MinValue fits in _sign, we represent this case differently for negate
        if (_sign == std::numeric_limits<int>::min())
            *this = s_bnMinInt;
    } else {
        _sign = negative ? -1 : +1;
        _bits.swap(value);
    }

    while (!_bits.empty() && _bits[_bits.size() - 1] == 0) {
        _bits.pop_back();
    }
    assert_valid();
}

bool BigInteger::is_power_of_two() const {
    assert_valid();

    if (_bits.size() == 0)
        return (_sign & (_sign - 1)) == 0 && _sign != 0;

    if (_sign != 1)
        return false;

    int iu = _bits.size() - 1;
    if ((_bits[iu] & (_bits[iu] - 1)) != 0)
        return false;

    while (--iu >= 0) {
        if (_bits[iu] != 0)
            return false;
    }

    return true;
}

bool BigInteger::is_zero() const {
    assert_valid();
    return _sign == 0;
}

bool BigInteger::is_one() const {
    assert_valid();
    return _sign == 1 && _bits.size() == 0;
}

bool BigInteger::is_even() const {
    assert_valid();
    return _bits.size() == 0 ? (_sign & 1) == 0 : (_bits[0] & 1) == 0;
}

BigInteger BigInteger::add(BigInteger& lhs, const BigInteger& rhs) {
    lhs.assert_valid();
    return lhs + rhs;
}

BigInteger BigInteger::subtract(BigInteger& lhs, const BigInteger& rhs) {
    return lhs - rhs;
}

BigInteger BigInteger::add(const uint_array& lhs, int lhsSign, const uint_array& rhs, int rhsSign) {
    bool trivialLeft = lhs.empty();
    bool trivialRight = rhs.empty();

    if (trivialLeft && trivialRight) {
        return BigInteger(static_cast<int64_t>(lhsSign) + rhsSign);
    }

    if (trivialLeft) {
        assert(!rhs.empty());
        uint_array bits = BigIntegerCalculator::add(rhs, std::abs(lhsSign));
        return BigInteger(bits, lhsSign < 0);
    }

    if (trivialRight) {
        assert(!lhs.empty());
        uint_array bits = BigIntegerCalculator::add(lhs, std::abs(rhsSign));
        return BigInteger(bits, lhsSign < 0);
    }

    assert(!lhs.empty() && !rhs.empty());

    if (lhs.size() < rhs.size()) {
        uint_array bits = BigIntegerCalculator::add(rhs, lhs);
        return BigInteger(bits, lhsSign < 0);
    } else {
        uint_array bits = BigIntegerCalculator::add(lhs, rhs);
        return BigInteger(bits, lhsSign < 0);
    }
}

BigInteger BigInteger::subtract(const uint_array& lhs, int lhsSign, const uint_array& rhs, int rhsSign) {
    bool trivialLeft = lhs.empty();
    bool trivialRight = rhs.empty();

    if (trivialLeft && trivialRight) {
        return BigInteger(static_cast<int64_t>(lhsSign) - rhsSign);
    }

    if (trivialLeft) {
        assert(!rhs.empty());
        uint_array bits = BigIntegerCalculator::subtract(rhs, std::abs(lhsSign));
        return BigInteger(bits, lhsSign >= 0);
    }

    if (trivialRight) {
        assert(!lhs.empty());
        uint_array bits = BigIntegerCalculator::subtract(lhs, std::abs(rhsSign));
        return BigInteger(bits, lhsSign < 0);
    }

    assert(!lhs.empty() && !rhs.empty());

    if (BigIntegerCalculator::compare(lhs, rhs) < 0) {
        uint_array bits = BigIntegerCalculator::subtract(rhs, lhs);
        return BigInteger(bits, lhsSign >= 0);
    } else {
        uint_array bits = BigIntegerCalculator::subtract(lhs, rhs);
        return BigInteger(bits, lhsSign < 0);
    }
}

BigInteger operator-(BigInteger lhs, const BigInteger& rhs) {
    lhs -= rhs;
    return lhs;
}

BigInteger& BigInteger::operator-=(const BigInteger& rhs) {
    BigInteger lhs = *this;
    lhs.assert_valid();
    rhs.assert_valid();

    if ((lhs._sign < 0) != (rhs._sign < 0)) {
        *this = BigInteger::add(lhs._bits, lhs._sign, rhs._bits, -1 * rhs._sign);
        return *this;
    }
    *this = BigInteger::subtract(lhs._bits, lhs._sign, rhs._bits, rhs._sign);
    return *this;
}

BigInteger operator*(BigInteger lhs, const BigInteger& rhs) {
    lhs *= rhs;
    return lhs;
}

BigInteger& BigInteger::operator*=(const BigInteger& rhs) {
    BigInteger lhs = *this;
    lhs.assert_valid();
    rhs.assert_valid();

    bool trivialLeft = lhs._bits.empty();
    bool trivialRight = rhs._bits.empty();

    if (trivialLeft && trivialRight) {
        *this = BigInteger(static_cast<int64_t>(lhs._sign) * rhs._sign);
        return *this;
    }

    if (trivialLeft) {
        assert(!rhs._bits.empty());
        uint_array bits = BigIntegerCalculator::multiply(rhs._bits, std::abs(lhs._sign));
        *this = BigInteger(bits, (lhs._sign < 0) ^ (rhs._sign < 0));
        return *this;
    }

    if (trivialRight) {
        assert(!lhs._bits.empty());
        uint_array bits = BigIntegerCalculator::multiply(lhs._bits, std::abs(rhs._sign));
        *this = BigInteger(bits, (lhs._sign < 0) ^ (rhs._sign < 0));
        return *this;
    }

    assert(!lhs._bits.empty() && !rhs._bits.empty());

    if (lhs._bits == rhs._bits) {
        uint_array bits = BigIntegerCalculator::square(lhs._bits);
        *this = BigInteger(bits, (lhs._sign < 0) ^ (rhs._sign < 0));
        return *this;
    }

    if (lhs._bits.size() < rhs._bits.size()) {
        uint_array bits = BigIntegerCalculator::multiply(rhs._bits, lhs._bits);
        *this = BigInteger(bits, (lhs._sign < 0) ^ (rhs._sign < 0));
        return *this;
    } else {
        uint_array bits = BigIntegerCalculator::multiply(lhs._bits, rhs._bits);
        *this = BigInteger(bits, (lhs._sign < 0) ^ (rhs._sign < 0));
        return *this;
    }
}

BigInteger BigInteger::multiply(BigInteger& lhs, const BigInteger& rhs) {
    return lhs * rhs;
}

BigInteger operator%(BigInteger lhs, const BigInteger& rhs) {
    lhs %= rhs;
    return lhs;
}

BigInteger& BigInteger::operator%=(const BigInteger& div) {
    BigInteger divisor = div;
    BigInteger& dividend = *this;
    dividend.assert_valid();
    divisor.assert_valid();

    bool trivialDividend = dividend._bits.empty();
    bool trivialDivisor = divisor._bits.empty();

    if (trivialDividend && trivialDivisor) {
        if (divisor._sign == 0)
            throw DivideByZero();
        *this = dividend._sign % divisor._sign;
        return *this;
    }

    if (trivialDividend) {
        // The divisor is non-trivial
        // and therefore the bigger one
        return dividend;
    }

    if (trivialDivisor) {
        assert(!dividend._bits.empty());
        int64_t remainder = BigIntegerCalculator::remainder(dividend._bits, std::abs(divisor._sign));
        *this = dividend._sign < 0 ? -1 * remainder : remainder;
        return *this;
    }

    assert(!dividend._bits.empty() && !divisor._bits.empty());

    if (dividend._bits.size() < divisor._bits.size()) {
        return dividend;
    }

    uint_array bits = BigIntegerCalculator::remainder(dividend._bits, divisor._bits);
    *this = BigInteger(bits, dividend._sign < 0);
    return *this;
}

BigInteger BigInteger::div_rem(BigInteger& dividend, BigInteger& divisor, BigInteger& remainder) {
    dividend.assert_valid();
    divisor.assert_valid();

    bool trivialDividend = dividend.get_bits().empty();
    bool trivialDivisor = divisor.get_bits().empty();

    if (trivialDividend && trivialDivisor) {
        if (divisor._sign == 0)
            throw DivideByZero();
        remainder = dividend._sign % divisor._sign;
        return dividend._sign / divisor._sign;
    }

    if (trivialDividend) {
        // The divisor is non-trivial
        // and therefore the bigger one
        remainder = dividend;
        return BigInteger::zero();
    }

    if (trivialDivisor) {
        uint32_t rest;
        uint_array rbits = dividend.get_bits();
        uint_array bits = BigIntegerCalculator::divide(rbits, std::abs(divisor._sign), rest);

        remainder = dividend._sign < 0 ? -1 * rest : rest;
        return BigInteger(bits, (dividend._sign < 0) ^ (divisor._sign < 0));
    }

    if (dividend.get_bits().size() < divisor.get_bits().size()) {
        remainder = dividend;
        return BigInteger::zero();
    } else {
        uint_array rest;
        uint_array lbits = dividend.get_bits();
        uint_array rbits = divisor.get_bits();
        uint_array bits = BigIntegerCalculator::divide(lbits, rbits, rest);

        remainder = BigInteger(rest, dividend._sign < 0);
        return BigInteger(bits, (dividend._sign < 0) ^ (divisor._sign < 0));
    }
}

BigInteger BigInteger::operator-() {
    return BigInteger(-(*this)._sign, (*this)._bits);
}

BigInteger BigInteger::operator+() {
    return BigInteger(+(*this)._sign, (*this)._bits);
}

BigInteger& BigInteger::operator++() {
    *this += BigInteger::one();
    return *this;
}

BigInteger& BigInteger::operator++(int dummy) {
    return ++(*this);
}

BigInteger& BigInteger::operator--() {
    //value.assert_valid();
    *this -= BigInteger::one();
    return *this;
}

BigInteger& BigInteger::operator--(int unused) {
    return --(*this);
}


BigInteger operator+(BigInteger lhs, const BigInteger& rhs) {
    lhs += rhs;
    return lhs;
}

BigInteger& BigInteger::operator+=(const BigInteger& rhs) {
    auto& lhs = *this;
    lhs.assert_valid();
    rhs.assert_valid();

    if ((lhs._sign < 0) != (rhs._sign < 0)) {
        *this = BigInteger::subtract(lhs._bits, lhs._sign, rhs._bits, -1 * rhs._sign);
        return *this;
    }
    *this = BigInteger::add(lhs._bits, lhs._sign, rhs._bits, rhs._sign);
    return *this;
}

BigInteger operator/(BigInteger lhs, const BigInteger& rhs) {
    lhs /= rhs;
    return lhs;
}

BigInteger& BigInteger::operator/=(const BigInteger& div) {
    auto& divisor = div;
    BigInteger dividend = *this;
    dividend.assert_valid();
    divisor.assert_valid();

    bool trivialDividend = dividend._bits.empty();
    bool trivialDivisor = divisor._bits.empty();

    if (trivialDividend && trivialDivisor) {
        if (divisor._sign == 0)
            throw DivideByZero();
        *this = static_cast<int64_t>(dividend._sign / divisor._sign);
        return *this;
    }

    if (trivialDividend) {
        // The divisor is non-trivial
        // and therefore the bigger one
        *this = BigInteger::s_bnZeroInt;
        return *this;
    }

    if (trivialDivisor) {
        assert(!dividend._bits.empty());
        uint_array bits = BigIntegerCalculator::divide(dividend._bits, std::abs(divisor._sign));
        *this = BigInteger(bits, (dividend._sign < 0) ^ (divisor._sign < 0));
        return *this;
    }

    assert(!dividend._bits.empty() && !divisor._bits.empty());

    if (dividend._bits.size() < divisor._bits.size()) {
        *this = BigInteger::s_bnZeroInt;
        return *this;
    } else {
        uint_array bits = BigIntegerCalculator::divide(dividend._bits, divisor._bits);
        *this = BigInteger(bits, (dividend._sign < 0) ^ (divisor._sign < 0));
        return *this;
    }
}

BigInteger BigInteger::divide(BigInteger& dividend, const BigInteger& divisor) {
    return dividend / divisor;
}

int BigInteger::compare_to(const BigInteger& other) const {
    assert_valid();
    other.assert_valid();

    if ((_sign ^ other._sign) < 0) {
        // Different signs, so the comparison is easy.
        return _sign < 0 ? -1 : +1;
    }

    // Same signs
    if (_bits.empty()) {
        if (other._bits.empty())
            return _sign < other._sign ? -1 : _sign > other._sign ? +1 : 0;
        return -other._sign;
    }
    int cuThis, cuOther;
    if (other._bits.empty() || (cuThis = _bits.size()) > (cuOther = other._bits.size()))
        return _sign;
    if (cuThis < cuOther)
        return -_sign;

    int cuDiff = get_diff_length(_bits, other._bits, cuThis);
    if (cuDiff == 0)
        return 0;
    return _bits[cuDiff - 1] < other._bits[cuDiff - 1] ? -_sign : _sign;
}

int BigInteger::get_diff_length(const uint_array& rgu1, const uint_array& rgu2, int cu) {
    for (int iv = cu; --iv >= 0;) {
        if (rgu1[iv] != rgu2[iv])
            return iv + 1;
    }
    return 0;
}

BigInteger BigInteger::zero() {
    return BigInteger(0);
}

BigInteger BigInteger::one() {
    return BigInteger(1);
}

BigInteger BigInteger::minus_one() {
    return BigInteger(-1);
}

int BigInteger::sign() const {
    assert_valid();
    return (_sign >> (kcbitUint - 1)) - (-_sign >> (kcbitUint - 1));
}

uint_array BigInteger::get_bits() const {
    return _bits;
}

bool BigInteger::equals(const BigInteger& other) const {
    assert_valid();
    other.assert_valid();

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
    int cuDiff = get_diff_length(_bits, other._bits, cu);
    return cuDiff == 0;
}

bool BigInteger::try_parse(const std::string& s, BigInteger& result) {
    try {
        result = parse(s);
        return true;
    } catch (...) {
        return false;
    }
}

BigInteger BigInteger::parse(const std::string& s) {
    auto b = BigInteger::zero();
    bool is_negative = false;
    unsigned int start_idx = 0;
    if (s[0] == '-') {
        is_negative = true;
        start_idx = 1;
    } else if (s[0] == '+') {
        start_idx = 1;
    }

    for (auto it = s.begin() + start_idx; it != s.end(); it++) {
        auto ch = *it;
        if (ch >= '0' && ch <= '9') {
            b *= 10;
            b += static_cast<unsigned int>(ch - '0');
        } else {
            break;
        }
    }

    if (is_negative)
        b = -b;
    return b;
}

std::string BigInteger::to_string() const {
    // Direct port of the following without format specifier support.
    // https://github.com/dotnet/runtime/blob/b93d5707caa7421ca4696746384b6a14c3e28d63/src/libraries/System.Runtime.Numerics/src/System/Numerics/BigNumber.cs#L601
    if (_bits.size() == 0) {
        return std::to_string(_sign);
    }

    // First convert to base 10^9.
    const uint32_t kuBase = 1000000000; // 10^9
    const int kcchBase = 9;

    int cuSrc = _bits.size();
    int cuMax;

    if (cuSrc > std::numeric_limits<int32_t>::max() / 10) {
        throw std::overflow_error("SR.Format_TooLarge");
    } else {
        cuMax = cuSrc * 10 / 9 + 2;
    }

    uint_array rguDst(cuMax);
    int cuDst = 0;

    for (int iuSrc = cuSrc; --iuSrc >= 0;) {
        uint32_t uCarry = _bits[iuSrc];
        for (int iuDst = 0; iuDst < cuDst; iuDst++) {
            assert(rguDst[iuDst] < kuBase);
            uint64_t uuRes = NumericsHelpers::make_ulong(rguDst[iuDst], uCarry);
            rguDst[iuDst] = static_cast<uint32_t>(uuRes % kuBase);
            uCarry = static_cast<uint32_t>(uuRes / kuBase);
        }

        if (uCarry != 0) {
            rguDst[cuDst++] = uCarry % kuBase;
            uCarry /= kuBase;
            if (uCarry != 0)
                rguDst[cuDst++] = uCarry;
        }

    }

    int cchMax;
    if (cuDst > std::numeric_limits<int32_t>::max() / kcchBase) {
        throw std::overflow_error("SR.Format_TooLarge");
    } else {
        cchMax = cuDst * kcchBase;
    }

    // We'll pass the rgch buffer to native code, which is going to treat it like a string of digits, so it needs
    // to be null terminated.  Let's ensure that we can allocate a buffer of that size.
    int rgchBufSize;
    if (cchMax == std::numeric_limits<int32_t>::max()) {
        throw std::overflow_error("SR.Format_TooLarge");
    } else {
        rgchBufSize = cchMax + 1;
    }

//    char rgch[rgchBufSize];
    char *rgch = new char[rgchBufSize];
    int ichDst = cchMax;
    for (int iuDst = 0; iuDst < cuDst - 1; iuDst++) {
        uint32_t uDig = rguDst[iuDst];
        assert(uDig < kuBase);
        for (int cch = kcchBase; --cch >= 0;) {
            rgch[--ichDst] = static_cast<char>('0' + (uDig % 10));
            uDig /= 10;
        }
    }

    for (uint32_t uDig = rguDst[cuDst - 1]; uDig != 0;) {
        rgch[--ichDst] = static_cast<char>('0' + uDig % 10);
        uDig /= 10;
    }

    if (_sign < 0) {
        std::string negativeSign = "-";
        for (int i = negativeSign.size() - 1; i > -1; i--)
            rgch[--ichDst] = negativeSign[i];
    }

    auto s = std::string(&rgch[ichDst], cchMax - ichDst);
    delete [] rgch;
    return s;
}

bool BigInteger::get_parts_for_bit_manipulation(const BigInteger& x, uint_array& xd, int& xl) {
    if (x.get_bits().size() == 0) {
        if (x._sign < 0) {
            xd.push_back(static_cast<uint32_t>(-x._sign));
        } else {
            xd.push_back(static_cast<uint32_t>(x._sign));
        }
    } else {
        xd = x.get_bits();
    }
    xl = (x.get_bits().size() == 0 ? 1 : x.get_bits().size());
    return x._sign < 0;
}

BigInteger operator<<(BigInteger lhs, const int shift) {
    lhs <<= shift;
    return lhs;
}

BigInteger& BigInteger::operator<<=(const int shift) {
    if (shift == 0) {
        return *this;
    } else if (shift == std::numeric_limits<int>::min()) {
        *this = ((*this >> std::numeric_limits<int>::max()) >> 1);
        return *this;
    } else if (shift < 0) {
        *this = *this >> -shift;
        return *this;
    }

    int digitShift = shift / kcbitUint;
    int smallShift = shift - (digitShift * kcbitUint);

    uint_array xd;
    int xl;
    bool negx;
    negx = get_parts_for_bit_manipulation(*this, xd, xl);

    int zl = xl + digitShift + 1;
    uint_array zd(zl, 0);

    if (smallShift == 0) {
        for (int i = 0; i < xl; i++) {
            zd[i + digitShift] = xd[i];
        }
    } else {
        int carryShift = kcbitUint - smallShift;
        uint32_t carry = 0;
        int i;
        for (i = 0; i < xl; i++) {
            unsigned int rot = xd[i];
            zd[i + digitShift] = rot << smallShift | carry;
            carry = rot >> carryShift;
        }
        zd[i + digitShift] = carry;
    }
    *this = BigInteger(zd, negx);
    return *this;
}

uint_array BigInteger::to_uint32_array() const {
    if (_bits.size() == 0 && _sign == 0)
        return uint_array();

    uint_array dwords;
    uint32_t highDWord;

    if (_bits.size() == 0) {
        dwords = uint_array{static_cast<uint32_t>(_sign)};
        highDWord = (_sign < 0) ? std::numeric_limits<uint32_t>::max() : 0;
    } else if (_sign == -1) {
        dwords = _bits;
        NumericsHelpers::dangerous_make_twos_complement(dwords);  // Mutates dwords
        highDWord = std::numeric_limits<uint32_t>::max();
    } else {
        dwords = _bits;
        highDWord = 0;
    }

    // Find highest significant byte
    int msb;
    for (msb = dwords.size() - 1; msb > 0; msb--) {
        if (dwords[msb] != highDWord) break;
    }
    // Ensure high bit is 0 if positive, 1 if negative
    bool needExtraByte = (dwords[msb] & 0x80000000) != (highDWord & 0x80000000);

    //uint_array trimmed = new uint[msb + 1 + (needExtraByte ? 1 : 0)];
    uint_array trimmed(msb + 1 + (needExtraByte ? 1 : 0), 0);
    std::copy(dwords.begin(), dwords.begin() + (msb + 1), trimmed.begin());

    if (needExtraByte) trimmed[trimmed.size() - 1] = highDWord;
    return trimmed;
}

BigInteger operator^(BigInteger lhs, const BigInteger& rhs) {
    lhs ^= rhs;
    return lhs;
}

BigInteger& BigInteger::operator^=(const BigInteger& rhs) {
    BigInteger lhs = *this;
    if (lhs._bits.size() == 0 && rhs._bits.size() == 0) {
        *this = lhs._sign ^ rhs._sign;
        return *this;
    }

    uint_array x = lhs.to_uint32_array();
    uint_array y = rhs.to_uint32_array();
    uint_array z(std::max(x.size(), y.size()), 0);
    uint32_t xExtend = (lhs._sign < 0) ? std::numeric_limits<uint32_t>::max() : 0;
    uint32_t yExtend = (rhs._sign < 0) ? std::numeric_limits<uint32_t>::max() : 0;

    for (size_t i = 0; i < z.size(); i++) {
        uint32_t xu = (i < x.size()) ? x[i] : xExtend;
        uint32_t yu = (i < y.size()) ? y[i] : yExtend;
        z[i] = xu ^ yu;
    }

    *this = BigInteger(z);
    return *this;
}

BigInteger operator>>(BigInteger lhs, const int shift) {
    lhs >>= shift;
    return lhs;
}

BigInteger& BigInteger::operator>>=(const int shift) {
    if (shift == 0) {
        return *this;
    } else if (shift == std::numeric_limits<int>::min()) {
        *this = ((*this << std::numeric_limits<int>::max()) << 1);
        return *this;
    } else if (shift < 0) {
        *this <<= -shift;
        return *this;
    }

    int digitShift = shift / kcbitUint;
    int smallShift = shift - (digitShift * kcbitUint);

    uint_array xd;
    int xl;
    bool negx;
    negx = get_parts_for_bit_manipulation(*this, xd, xl);

    if (negx) {
        if (shift >= (kcbitUint * xl)) {
            *this = s_bnMinusOneInt;
            return *this;
        }
        uint_array temp = xd;
        xd = temp;
        NumericsHelpers::dangerous_make_twos_complement(xd); // Mutates xd
    }

    int zl = xl - digitShift;
    if (zl < 0) zl = 0;
    uint_array zd(zl, 0);

    if (smallShift == 0) {
        for (int i = xl - 1; i >= digitShift; i--) {
            zd[i - digitShift] = xd[i];
        }
    } else {
        int carryShift = kcbitUint - smallShift;
        uint32_t carry = 0;
        for (int i = xl - 1; i >= digitShift; i--) {
            uint32_t rot = xd[i];
            if (negx && i == xl - 1)
                // Sign-extend the first shift for negative ints then let the carry propagate
                zd[i - digitShift] = (rot >> smallShift) | (0xFFFFFFFF << carryShift);
            else
                zd[i - digitShift] = (rot >> smallShift) | carry;
            carry = rot << carryShift;
        }
    }
    if (negx) {
        NumericsHelpers::dangerous_make_twos_complement(zd); // Mutates zd
    }
    *this = BigInteger(zd, negx);
    return *this;
}

BigInteger operator&(BigInteger lhs, const BigInteger& rhs) {
    lhs &= rhs;
    return lhs;
}

BigInteger& BigInteger::operator&=(const BigInteger& rhs) {
    BigInteger lhs = *this;
    if (lhs.is_zero() || rhs.is_zero()) {
        *this = BigInteger::zero();
        return *this;
    }

    if (lhs.get_bits().size() == 0 && rhs.get_bits().size() == 0) {
        *this = lhs._sign & rhs._sign;
        return *this;
    }

    uint_array x = lhs.to_uint32_array();
    uint_array y = rhs.to_uint32_array();
    uint_array z(std::max(x.size(), y.size()), 0);
    unsigned int xExtend = (lhs._sign < 0) ? std::numeric_limits<uint32_t>::max() : 0;
    unsigned int yExtend = (rhs._sign < 0) ? std::numeric_limits<uint32_t>::max() : 0;

    for (size_t i = 0; i < z.size(); i++) {
        unsigned int xu = (i < x.size()) ? x[i] : xExtend;
        unsigned int yu = (i < y.size()) ? y[i] : yExtend;
        z[i] = xu & yu;
    }
    *this = BigInteger(z);
    return *this;
}

BigInteger BigInteger::negate(const BigInteger& value) {
    return BigInteger(-value._sign, value._bits);
}

int Compare(BigInteger& lhs, BigInteger& rhs) {
    return lhs.compare_to(rhs);
}

static BigInteger& Max(BigInteger& lhs, BigInteger& rhs) {
    if (lhs.compare_to(rhs) < 0)
        return rhs;
    return lhs;
}

static BigInteger& Min(BigInteger& lhs, BigInteger& rhs) {
    if (lhs.compare_to(rhs) <= 0)
        return lhs;
    return rhs;
}

BigInteger operator|(BigInteger lhs, const BigInteger& rhs) {
    lhs |= rhs;
    return lhs;
}

BigInteger& BigInteger::operator|=(const BigInteger& rhs) {
    BigInteger lhs = *this;
    if (lhs.is_zero()) {
        *this = rhs;
        return *this;
    }
    if (rhs.is_zero()) {
        return *this;
    }

    if (lhs._bits.size() == 0 && rhs._bits.size() == 0) {
        *this = lhs._sign | rhs._sign;
        return *this;
    }

    uint_array x = lhs.to_uint32_array();
    uint_array y = rhs.to_uint32_array();
    uint_array z(std::max(x.size(), y.size()));
    unsigned int xExtend = (lhs._sign < 0) ? std::numeric_limits<uint32_t>::max() : 0;
    unsigned int yExtend = (rhs._sign < 0) ? std::numeric_limits<uint32_t>::max() : 0;

    for (size_t i = 0; i < z.size(); i++) {
        unsigned int xu = (i < x.size()) ? x[i] : xExtend;
        unsigned int yu = (i < y.size()) ? y[i] : yExtend;
        z[i] = xu | yu;
    }
    *this = BigInteger(z);
    return *this;
}

int BigInteger::get_byte_count(bool isUnsigned) const {
    int bytesWritten = 0;
    this->to_byte_array(GetBytesMode::Count, isUnsigned, false, &bytesWritten);
    return bytesWritten;
}

byte_array BigInteger::to_byte_array(bool isUnsigned, bool isBigEndian) const {
    int ignored = 0;
    return this->to_byte_array(GetBytesMode::AllocateArray, isUnsigned, isBigEndian, &ignored);
}

byte_array BigInteger::to_byte_array(GetBytesMode mode, bool isUnsigned, bool isBigEndian, int* bytesWritten) const {
    int sign = _sign;
    if (sign == 0) {
        switch (mode) {
            case GetBytesMode::Count:
                *bytesWritten = 1;
                return byte_array();
            default:
                *bytesWritten = 1;
                return byte_array(1, 0);
        }
    }

    if (isUnsigned && sign < 0) {
        throw std::runtime_error("Overflow error can't have unsigned & sign < 0");
    }

    byte highByte;
    int nonZeroDwordIndex = 0;
    uint32_t highDword;
    uint_array bits = _bits;
    if (bits.size() == 0) {
        highByte = static_cast<byte>((sign < 0) ? 0xff : 0x00);
        highDword = static_cast<uint32_t>(sign);
    } else if (sign == -1) {
        highByte = 0xff;

        // If sign is -1, we will need to two's complement bits.
        // Previously this was accomplished via NumericsHelpers.DangerousMakeTwosComplement()
        // however, we can do the two's complement on the stack so as to avoid
        // creating a temporary copy of bits just to hold the two's complement.
        // one special case in DangerousMakeTwosComplement() is that if the array
        // is all zeros, then it would allocate a new array with the high-order
        // uint set to 1 (for the carry). In our usage, we will not hit this case
        // because a bits array of all zeros would represent 0, and this case
        // would be encoded as _bits = null and _sign = 0.
        assert(bits.size() > 0);
        assert(bits[bits.size() - 1] != 0);
        while (bits[nonZeroDwordIndex] == 0U) {
            nonZeroDwordIndex++;
        }

        highDword = ~bits[bits.size() - 1];
        if (bits.size() - 1 == nonZeroDwordIndex) {
            // This will not overflow because highDword is less than or equal to uint.MaxValue - 1.
            assert(highDword <= std::numeric_limits<uint32_t>::max() - 1);
            highDword += 1U;
        }
    } else {
        assert(sign == 1);
        highByte = 0x00;
        highDword = bits[bits.size() - 1];
    }

    byte msb;
    int msbIndex;
    if ((msb = static_cast<byte>((highDword >> 24))) != highByte) {
        msbIndex = 3;
    } else if ((msb = static_cast<byte>((highDword >> 16))) != highByte) {
        msbIndex = 2;
    } else if ((msb = static_cast<byte>((highDword >> 8))) != highByte) {
        msbIndex = 1;
    } else {
        msb = static_cast<byte>(highDword);
        msbIndex = 0;
    }

    // Ensure high bit is 0 if positive, 1 if negative
    bool needExtraByte = (msb & 0x80) != (highByte & 0x80) && !isUnsigned;
    int length = msbIndex + 1 + (needExtraByte ? 1 : 0);
    if (bits.size() != 0) {
        length = 4 * (bits.size() - 1) + length;
    }

    byte_array destination;
    switch (mode) {
        case GetBytesMode::Count:
            *bytesWritten = length;
            return byte_array();
        default:
            destination = byte_array(length);
            break;
    }

    int curByte = isBigEndian ? length - 1 : 0;
    int increment = isBigEndian ? -1 : 1;

    if (bits.size() != 0) {
        for (size_t i = 0; i < bits.size() - 1; i++) {
            unsigned int dword = bits[i];

            if (sign == -1) {
                dword = ~dword;
                if (i <= nonZeroDwordIndex) {
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
    if (msbIndex != 0) {
        curByte += increment;
        destination[curByte] = static_cast<byte>((highDword >> 8));
        if (msbIndex != 1) {
            curByte += increment;
            destination[curByte] = static_cast<byte>((highDword >> 16));
            if (msbIndex != 2) {
                curByte += increment;
                destination[curByte] = static_cast<byte>((highDword >> 24));
            }
        }
    }

    // Assert we're big endian, or little endian consistency holds.
    assert(isBigEndian || (!needExtraByte && curByte == length - 1) || (needExtraByte && curByte == length - 2));
    // Assert we're little endian, or big endian consistency holds.
    assert(!isBigEndian || (!needExtraByte && curByte == 0) || (needExtraByte && curByte == 1));

    if (needExtraByte) {
        curByte += increment;
        destination[curByte] = highByte;
    }

    return destination;
}

void BigInteger::assert_valid() const {
#ifndef NDEBUG
    if (!_bits.empty()) {
        assert(_sign == 1 || _sign == -1);
        assert(_bits.size() > 0);
        assert(_bits.size() > 1 || _bits[0] >= kuMaskHighBit);
        assert(_bits[_bits.size() - 1] != 0);
    } else {
        assert(_sign > std::numeric_limits<int>::min());
    }
#endif
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

    unsigned long h = _bits[length - 1];
    unsigned long m = length > 1 ? _bits[length - 2] : 0;
    unsigned long l = length > 2 ? _bits[length - 3] : 0;

    int z = NumericsHelpers::cbit_high_zero(static_cast<unsigned int>(h));

    int exp = (length - 2) * 32 - z;
    unsigned long man = (h << (32 + z)) | (m << z) | (l >> (32 - z));

    return NumericsHelpers::get_double_from_parts(_sign, exp, man);
}

BigInteger::operator float() {
    return static_cast<float>(static_cast<double>(*this));
}

double BigInteger::log(const BigInteger& value) {
    return BigInteger::log(value, 2.7182818284590451);
}

double BigInteger::log(const BigInteger& value, double baseValue) {
    if (value._sign < 0 || baseValue == 1.0)
        return std::nan("");

    if (baseValue == std::numeric_limits<double>::infinity())
        return value.is_one() ? 0.0 : std::nan("");

    if (baseValue == 0.0 && !value.is_one())
        return std::nan("");

    if (value._bits.empty())
        return NumericsHelpers::csharp_log_wrapper(value._sign, baseValue);

    uint64_t h = value._bits[value._bits.size() - 1];
    uint64_t m = value._bits.size() > 1 ? value._bits[value._bits.size() - 2] : 0;
    uint64_t l = value._bits.size() > 2 ? value._bits[value._bits.size() - 3] : 0;

    int c = NumericsHelpers::cbit_high_zero(static_cast<unsigned int>(h));
    int64_t b = static_cast<int64_t>(value._bits.size() * 32 - c);

    uint64_t x = (h << (32 + c)) | (m << c) | (l >> (32 - c));
    return NumericsHelpers::csharp_log_wrapper(x, baseValue) +
           (b - 64) / NumericsHelpers::csharp_log_wrapper(baseValue, 2);
}

double BigInteger::log10(const BigInteger& value) {
    return BigInteger::log(value, 10);
}

BigInteger BigInteger::pow(const BigInteger& value, int exponent) {
    if (exponent < 0)
        throw std::out_of_range("SR.ArgumentOutOfRange_MustBeNonNeg");

    value.assert_valid();

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
                      ? BigIntegerCalculator::pow(std::abs(value._sign), std::abs(exponent))
                      : BigIntegerCalculator::pow(value._bits, std::abs(exponent));

    return BigInteger(bits, value._sign < 0 && (exponent & 1) != 0);
}


BigInteger BigInteger::mod_pow(const BigInteger& value, const BigInteger& exponent, const BigInteger& modulus) {
    if (exponent._sign < 0)
        throw std::out_of_range("SR.ArgumentOutOfRange_MustBeNonNeg");

    value.assert_valid();
    exponent.assert_valid();
    modulus.assert_valid();

    bool trivialValue = value._bits.empty();
    bool trivialExponent = exponent._bits.empty();
    bool trivialModulus = modulus._bits.empty();

    if (trivialModulus) {
        uint32_t bits;
        if (trivialValue) {
            if (trivialExponent)
                bits = BigIntegerCalculator::pow(std::abs(value._sign), std::abs(exponent._sign),
                                                 std::abs(modulus._sign));
            else
                bits = BigIntegerCalculator::pow(std::abs(value._sign), exponent._bits, std::abs(modulus._sign));
        } else {
            if (trivialExponent)
                bits = BigIntegerCalculator::pow(value._bits, std::abs(exponent._sign), std::abs(modulus._sign));
            else
                bits = BigIntegerCalculator::pow(value._bits, exponent._bits, std::abs(modulus._sign));
        }
        return value._sign < 0 && !exponent.is_even() ? -1 * bits : bits;
    } else {
        uint_array bits;
        if (trivialValue && trivialExponent) {
            bits = BigIntegerCalculator::pow(std::abs(value._sign), std::abs(exponent._sign), modulus._bits);
        } else {
            if (trivialValue)
                bits = BigIntegerCalculator::pow(std::abs(value._sign), exponent._bits, modulus._bits);
            else {
                if (trivialExponent)
                    bits = BigIntegerCalculator::pow(value._bits, std::abs(exponent._sign), modulus._bits);
                else
                    bits = BigIntegerCalculator::pow(value._bits, exponent._bits, modulus._bits);
            }
        }
        return BigInteger(bits, value._sign < 0 && !exponent.is_even());
    }
}

BigInteger BigInteger::remainder(BigInteger& dividend, const BigInteger& divisor) {
    return dividend % divisor;
}

BigInteger BigInteger::greatest_common_divisor(const BigInteger& left, const BigInteger& right) {
    left.assert_valid();
    right.assert_valid();

    bool trivialLeft = left._bits.empty();
    bool trivialRight = right._bits.empty();

    if (trivialLeft && trivialRight) {
        return BigIntegerCalculator::gcd((uint32_t) std::abs(left._sign), std::abs(right._sign));
    }

    if (trivialLeft) {
        assert(!right._bits.empty());
        if (left._sign != 0) {
            auto result = BigIntegerCalculator::gcd(right._bits, std::abs(left._sign));
            return BigInteger{result};
        } else {
            return BigInteger(right._bits, false);
        }
    }

    if (trivialRight) {
        assert(!left._bits.empty());
        if (right._sign != 0) {
            auto result = BigIntegerCalculator::gcd(left._bits, std::abs(right._sign));
            return BigInteger{result};
        } else {
            return BigInteger(left._bits, false);
        }
    }

    assert(!left._bits.empty() && !right._bits.empty());

    if (BigIntegerCalculator::compare(left._bits, right._bits) < 0) {
        return greatest_common_divisor(right._bits, left._bits);
    } else {
        return greatest_common_divisor(left._bits, right._bits);
    }
}

BigInteger BigInteger::greatest_common_divisor(const uint_array& leftBits, const uint_array& rightBits) {
    assert(BigIntegerCalculator::compare(leftBits, rightBits) >= 0);

    // Short circuits to spare some allocations...
    if (rightBits.size() == 1) {
        unsigned int temp = BigIntegerCalculator::remainder(leftBits, rightBits[0]);
        return BigIntegerCalculator::gcd(rightBits[0], temp);
    }

    if (rightBits.size() == 2) {
        auto tempBits = BigIntegerCalculator::remainder(leftBits, rightBits);

        uint64_t left = (static_cast<uint64_t>(rightBits[1]) << 32) | rightBits[0];
        uint64_t right = (static_cast<uint64_t>(tempBits[1]) << 32) | tempBits[0];

        uint64_t res = BigIntegerCalculator::gcd(left, right);
        return BigInteger{res};
    }

    auto bits = BigIntegerCalculator::gcd(leftBits, rightBits);
    return BigInteger(bits, false);
}

BigInteger BigInteger::operator~() {
    auto res = *this + one();
    return -(res);
}

BigInteger BigInteger::abs(BigInteger& value) {
    return (value >= zero()) ? value : -value;
}

BigInteger::BigInteger(double value) {
    if (!double_IsFinite(value)) {
        if (double_IsInfinity(value)) {
            throw std::overflow_error("SR.Overflow_BigIntInfinity");
        } else // NaN
        {
            throw std::overflow_error("SR.Overflow_NotANumber");
        }
    }

    _sign = 0;
    _bits.clear();

    int sign, exp;
    uint64_t man;
    bool fFinite;
    NumericsHelpers::get_double_parts(value, sign, exp, man, fFinite);
    assert(sign == +1 || sign == -1);

    if (man == 0) {
        *this = zero();
        return;
    }

    assert(man < (1UL << 53));
    assert(exp <= 0 || man >= (1UL << 52));

    if (exp <= 0) {
        if (exp <= -kcbitUlong) {
            *this = zero();
            return;
        }
        *this = man >> -exp;
        if (sign < 0)
            _sign = -_sign;
    } else if (exp <= 11) {
        *this = man << exp;
        if (sign < 0)
            _sign = -_sign;
    } else {
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
        _bits = uint_array(cu + 2, 0);
        _bits[cu + 1] = (unsigned int) (man >> (cbit + kcbitUint));
        _bits[cu] = static_cast<unsigned int>((man >> cbit));
        if (cbit > 0)
            _bits[cu - 1] = static_cast<unsigned int>(man) << (kcbitUint - cbit);
        _sign = sign;
    }

    assert_valid();

}

bool BigInteger::double_IsFinite(double value) {
    auto bits = *((long*) &value);
    return (bits & 0x7FFFFFFFFFFFFFFF) < 0x7FF0000000000000;
}

bool BigInteger::double_IsNaN(double value) {
    auto bits = *((long*) &value);
    return (bits & 0x7FFFFFFFFFFFFFFF) > 0x7FF0000000000000;
}

bool BigInteger::double_IsInfinity(double value) {
    auto bits = *((long*) &value);
    return (bits & 0x7FFFFFFFFFFFFFFF) == 0x7FF0000000000000;
}

int BigInteger::get_hash_code() const {
    if (_bits.empty())
        return _sign;
    int hash = _sign;
    for (int iv = _bits.size(); --iv >= 0;)
        hash = NumericsHelpers::combine_hash(hash, static_cast<int>(_bits[iv]));
    return hash;
}
