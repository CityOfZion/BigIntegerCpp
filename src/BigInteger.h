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
        BigInteger(int value);
        BigInteger(uint32_t value);
        BigInteger(long value);
        BigInteger(long long value);
        BigInteger(uint64_t value);
        BigInteger(double value);
        BigInteger(int n, uint_array value);
        BigInteger(uint_array value, bool negative);
        BigInteger(byte_array value, bool isUnsigned = false, bool isBigEndian = false);

        friend bool operator<(const BigInteger& lhs, const BigInteger& rhs) { return lhs.CompareTo(rhs) < 0; }
        friend bool operator>(const BigInteger& lhs, const BigInteger& rhs) { return rhs < lhs; }
        friend bool operator<=(const BigInteger& lhs, const BigInteger& rhs) { return !(lhs > rhs); }
        friend bool operator>=(const BigInteger& lhs, const BigInteger& rhs) { return !(lhs < rhs); }
        friend bool operator==(const BigInteger& lhs, const BigInteger& rhs) { return lhs.Equals(rhs); };

        bool operator ==(const BigInteger& rhs);
        bool operator !=(const BigInteger& rhs);

        // explicit casts
        operator int();
        operator unsigned int();
        operator long();
        operator unsigned long();
        operator double();
        operator float();

        bool IsPowerOfTwo();
        bool IsZero();
        bool IsOne();
        bool IsEven();

        static BigInteger Zero();
        static BigInteger One();
        static BigInteger MinusOne();

        static BigInteger Parse(std::string value);
        static bool TryParse(std::string value, BigInteger& result);

        static BigInteger Negate(BigInteger& value);
        static BigInteger Abs(BigInteger& value);

        static double Log(BigInteger value);
        static double Log(BigInteger value, double baseValue);
        static double Log10(BigInteger value);

        static BigInteger ModPow(BigInteger value, BigInteger exponent, BigInteger modulus);
        static BigInteger Pow(BigInteger value, int exponent);

        static BigInteger GreatestCommonDivisor(BigInteger& left, BigInteger& right);
        static BigInteger GreatestCommonDivisor(uint_array leftBits, uint_array rightBits);

        //bool Equals(long long other) const;
        //bool Equals(unsigned long other) const;
        //bool Equals(std::any obj);
        bool Equals(uint64_t other) const;
        bool Equals(const BigInteger& other) const;

        // Divide & Modulo
        BigInteger operator%(const BigInteger& rhs);
        BigInteger operator/(const BigInteger& divisor);
        BigInteger operator/=(const BigInteger& divisor);
        static BigInteger Divide(BigInteger& dividend, BigInteger& divisor);
        static BigInteger DivRem(BigInteger& dividend, BigInteger& divisor, BigInteger& remainder);
        static BigInteger Remainder(BigInteger& dividend, BigInteger& divisor);

        // Multiply & Square
        BigInteger operator*(const BigInteger& rhs);
        BigInteger operator*=(const BigInteger& rhs);
        static BigInteger Multiply(BigInteger& lhs, BigInteger& rhs);

        // Add
        BigInteger operator+();
        BigInteger operator+(const BigInteger& rhs);
        BigInteger operator+=(const BigInteger& rhs);
        BigInteger operator+=(uint64_t rhs);
        BigInteger& operator ++();
        static BigInteger Add(BigInteger& lhs, BigInteger& rhs);
        BigInteger Add(uint_array& lhs, int lhsSign, uint_array& rhs, int rhsSign);

        // Subtract
        BigInteger operator -();
        BigInteger operator -(const BigInteger& rhs);
        BigInteger operator -=(const BigInteger& rhs);
        BigInteger& operator --();
        static BigInteger Subtract(BigInteger& lhs, BigInteger& rhs);
        BigInteger Subtract(uint_array& lhs, int lhsSign, uint_array& rhs, int rhsSign);

        // Compare
        int CompareTo(long other) const;
        int CompareTo(const BigInteger& other) const;

        BigInteger operator <<(int shift);
        BigInteger operator >>(int shift);
        BigInteger operator ^(BigInteger& rhs);
        BigInteger operator &(BigInteger& rhs);
        BigInteger operator |(BigInteger& rhs);
        BigInteger operator ~();

        enum GetBytesMode { AllocateArray, Count }; // currently AllocateArray only
        int Sign();
        int GetSign() ;
        uint_array GetBits();
        int GetHashCode();

        std::string ToString();
        uint_array ToUInt32Array();
        byte_array ToByteArray(bool isUnsigned = false, bool isBigEndian = false);
        byte_array ToByteArray(GetBytesMode mode, bool isUnsigned = false, bool isBigEndian = false, int* bytesWritten = 0);
        int GetByteCount(bool isUnsigned = false);

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

    static int GetDiffLength(uint_array rgu1, uint_array rgu2, int cu);
    static bool GetPartsForBitManipulation(BigInteger& x, uint_array& xd, int& xl);
    void AssertValid() const;
};

