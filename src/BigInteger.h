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

        friend bool operator<(const BigInteger& lhs, const BigInteger& rhs) { return lhs.CompareTo(rhs) < 0; }
        friend bool operator>(const BigInteger& lhs, const BigInteger& rhs) { return rhs < lhs; }
        friend bool operator<=(const BigInteger& lhs, const BigInteger& rhs) { return !(lhs > rhs); }
        friend bool operator>=(const BigInteger& lhs, const BigInteger& rhs) { return !(lhs < rhs); }
        friend bool operator==(const BigInteger& lhs, const BigInteger& rhs) { return lhs.Equals(rhs); }
        friend bool operator!=(const BigInteger& lhs, const BigInteger& rhs) { return !lhs.Equals(rhs); }

        // explicit casts
        operator int();
        operator unsigned int();
        operator long();
        operator unsigned long();
        operator double();
        operator float();

        bool IsPowerOfTwo() const;
        bool IsZero() const;
        bool IsOne() const;
        bool IsEven() const;

        static BigInteger Zero();
        static BigInteger One();
        static BigInteger MinusOne();

        static BigInteger Parse(std::string value);
        static bool TryParse(std::string value, BigInteger& result);

        static BigInteger Negate(BigInteger& value);
        static BigInteger Abs(BigInteger& value);

        static double Log(const BigInteger& value);
        static double Log(const BigInteger& value, double baseValue);
        static double Log10(const BigInteger& value);

        static BigInteger ModPow(const BigInteger& value, const BigInteger& exponent, const BigInteger& modulus);
        static BigInteger Pow(const BigInteger& value, int exponent);

        static BigInteger GreatestCommonDivisor(const BigInteger& left, const BigInteger& right);
        static BigInteger GreatestCommonDivisor(const uint_array& leftBits, const uint_array& rightBits);

        // Returns a value that indicates whether the current instance and a specified BigInteger object have the same value.
        bool Equals(const BigInteger& other) const;

        // Compares this instance to a second BigInteger and returns an integer that indicates whether the value of this instance is less than, equal to, or greater than the value of the specified object.
        int CompareTo(const BigInteger& other) const;

        // Divide & Modulo
        BigInteger& operator/=(const BigInteger& divisor);
        BigInteger& operator%=(const BigInteger& rhs);
        static BigInteger Divide(BigInteger& dividend, const BigInteger& divisor);
        static BigInteger DivRem(BigInteger& dividend, BigInteger& divisor, BigInteger& remainder);
        static BigInteger Remainder(BigInteger& dividend, const BigInteger& divisor);

        // Multiply & Square
        BigInteger& operator*=(const BigInteger& rhs);
        static BigInteger Multiply(BigInteger& lhs, const BigInteger& rhs);

        // Add
        BigInteger operator+();
        BigInteger& operator+=(const BigInteger& rhs);
        BigInteger& operator++(); // prefix
        BigInteger& operator++(int unused); // postfix

        static BigInteger Add(BigInteger& lhs, const BigInteger& rhs);
        BigInteger Add(const uint_array& lhs, int lhsSign, const uint_array& rhs, int rhsSign);

        // Subtract
        BigInteger operator-();
        BigInteger& operator-=(const BigInteger& rhs);
        BigInteger& operator--(); // prefix
        BigInteger& operator--(int unused); // postfix
        static BigInteger Subtract(BigInteger& lhs, const BigInteger& rhs);
        BigInteger Subtract(const uint_array& lhs, int lhsSign, const uint_array& rhs, int rhsSign);

        BigInteger& operator<<=(const int shift);
        BigInteger& operator>>=(const int shift);
        BigInteger& operator^=(const BigInteger& rhs);
        BigInteger& operator&=(const BigInteger& rhs);
        BigInteger& operator|=(const BigInteger& rhs);
        BigInteger operator ~();

        enum GetBytesMode { AllocateArray, Count }; // currently AllocateArray only
        int Sign() const;
        int GetSign() const;
        uint_array GetBits() const;
        int GetHashCode() const;

        std::string ToString() const;
        uint_array ToUInt32Array() const;
        byte_array ToByteArray(bool isUnsigned = false, bool isBigEndian = false) const;
        byte_array ToByteArray(GetBytesMode mode, bool isUnsigned = false, bool isBigEndian = false, int* bytesWritten = 0) const;
        int GetByteCount(bool isUnsigned = false) const;

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

    static int GetDiffLength(const uint_array& rgu1, const uint_array& rgu2, int cu);
    static bool GetPartsForBitManipulation(const BigInteger& x, uint_array& xd, int& xl);
    void AssertValid() const;
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
