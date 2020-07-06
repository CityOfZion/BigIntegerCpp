#pragma once

#include <any>
#include <climits>
#include <vector>
#include <string>
#include <iostream>

using uint_array = std::vector<uint32_t>;
using byte_array = std::vector<unsigned char>;
using byte = unsigned char;

class BigInteger {
public:
    static constexpr unsigned int kuMaskHighBit = static_cast<unsigned int>(INT_MIN);
    static constexpr int kcbitUint = 32;
    static constexpr int kcbitUlong = 64;
    static constexpr int DecimalScaleFactorMask = 0x00FF0000;
    static constexpr int DecimalSignMask = static_cast<int>(0x80000000);

    BigInteger();

    BigInteger(int32_t value);

    BigInteger(uint32_t value);

    BigInteger(int64_t value);

    BigInteger(uint64_t value);

    BigInteger(double value);

    BigInteger(int n, uint_array value);

    BigInteger(uint_array value, bool negative);

    BigInteger(byte_array value, bool isUnsigned = false, bool isBigEndian = false);

    friend bool operator<(const BigInteger& lhs, const BigInteger& rhs) { return lhs.compare_to(rhs) < 0; }

    friend bool operator>(const BigInteger& lhs, const BigInteger& rhs) { return rhs < lhs; }

    friend bool operator<=(const BigInteger& lhs, const BigInteger& rhs) { return !(lhs > rhs); }

    friend bool operator>=(const BigInteger& lhs, const BigInteger& rhs) { return !(lhs < rhs); }

    friend bool operator==(const BigInteger& lhs, const BigInteger& rhs) { return lhs.equals(rhs); }

    friend bool operator!=(const BigInteger& lhs, const BigInteger& rhs) { return !lhs.equals(rhs); }

    // explicit casts
    operator int();

    operator unsigned int();

    operator long();

    operator unsigned long();

    operator double();

    operator float();

    bool is_power_of_two() const;

    bool is_zero() const;

    bool is_one() const;

    bool is_even() const;

    static BigInteger zero();

    static BigInteger one();

    static BigInteger minus_one();

    static BigInteger parse(const std::string& s);

    static bool try_parse(const std::string& s, BigInteger& result);

    static BigInteger negate(const BigInteger& value);

    static BigInteger abs(BigInteger& value);

    static double log(const BigInteger& value);

    static double log(const BigInteger& value, double baseValue);

    static double log10(const BigInteger& value);

    static BigInteger mod_pow(const BigInteger& value, const BigInteger& exponent, const BigInteger& modulus);

    static BigInteger pow(const BigInteger& value, int exponent);

    static BigInteger greatest_common_divisor(const BigInteger& left, const BigInteger& right);

    static BigInteger greatest_common_divisor(const uint_array& leftBits, const uint_array& rightBits);

    // Returns a value that indicates whether the current instance and a specified BigInteger object have the same value.
    bool equals(const BigInteger& other) const;

    // Compares this instance to a second BigInteger and returns an integer that indicates whether the value of this instance is less than, equal to, or greater than the value of the specified object.
    int compare_to(const BigInteger& other) const;

    // Divide & Modulo
    BigInteger& operator/=(const BigInteger& divisor);

    BigInteger& operator%=(const BigInteger& rhs);

    static BigInteger divide(BigInteger& dividend, const BigInteger& divisor);

    static BigInteger div_rem(BigInteger& dividend, BigInteger& divisor, BigInteger& remainder);

    static BigInteger remainder(BigInteger& dividend, const BigInteger& divisor);

    // Multiply & square
    BigInteger& operator*=(const BigInteger& rhs);

    static BigInteger multiply(BigInteger& lhs, const BigInteger& rhs);

    // Add
    BigInteger operator+();

    BigInteger& operator+=(const BigInteger& rhs);

    BigInteger& operator++(); // prefix
    BigInteger& operator++(int unused); // postfix

    static BigInteger add(BigInteger& lhs, const BigInteger& rhs);

    BigInteger add(const uint_array& lhs, int lhsSign, const uint_array& rhs, int rhsSign);

    // subtract
    BigInteger operator-();

    BigInteger& operator-=(const BigInteger& rhs);

    BigInteger& operator--(); // prefix
    BigInteger& operator--(int unused); // postfix
    static BigInteger subtract(BigInteger& lhs, const BigInteger& rhs);

    BigInteger subtract(const uint_array& lhs, int lhsSign, const uint_array& rhs, int rhsSign);

    BigInteger& operator<<=(const int shift);

    BigInteger& operator>>=(const int shift);

    BigInteger& operator^=(const BigInteger& rhs);

    BigInteger& operator&=(const BigInteger& rhs);

    BigInteger& operator|=(const BigInteger& rhs);

    BigInteger operator~();

    enum GetBytesMode {
        AllocateArray, Count
    }; // currently AllocateArray only
    int sign() const; // equals the C# sign property. Returns negative, zero or positive
    int sign_internal() const { return _sign; }; // return the actual internal _sign value
    uint_array get_bits() const;

    int get_hash_code() const;

    std::string to_string() const;

    uint_array to_uint32_array() const;

    byte_array to_byte_array(bool isUnsigned = false, bool isBigEndian = false) const;

    byte_array
    to_byte_array(GetBytesMode mode, bool isUnsigned = false, bool isBigEndian = false, int* bytesWritten = 0) const;

    int get_byte_count(bool isUnsigned = false) const;

    static bool double_IsFinite(double value);

    static bool double_IsInfinity(double value);

    static bool double_IsNaN(double value);

private:
    int _sign;
    uint_array _bits;

    static const BigInteger s_bnMinInt;
    static const BigInteger s_bnOneInt;
    static const BigInteger s_bnZeroInt;
    static const BigInteger s_bnMinusOneInt;

    BigInteger(uint_array value);

    static int get_diff_length(const uint_array& rgu1, const uint_array& rgu2, int cu);

    static bool get_parts_for_bit_manipulation(const BigInteger& x, uint_array& xd, int& xl);

    void assert_valid() const;
};

BigInteger operator+(BigInteger lhs, const BigInteger& rhs);

BigInteger operator-(BigInteger lhs, const BigInteger& rhs);

BigInteger operator*(BigInteger lhs, const BigInteger& rhs);

BigInteger operator/(BigInteger lhs, const BigInteger& rhs);

BigInteger operator%(BigInteger lhs, const BigInteger& rhs);

BigInteger operator&(BigInteger lhs, const BigInteger& rhs);

BigInteger operator|(BigInteger lhs, const BigInteger& rhs);

BigInteger operator^(BigInteger lhs, const BigInteger& rhs);

BigInteger operator<<(BigInteger lhs, const int shift);

BigInteger operator>>(BigInteger lhs, const int shift);
