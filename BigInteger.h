
#pragma once
#include <any>
#include <climits>
#include <vector>
#include <string>

typedef std::vector<uint32_t> uint_array;
typedef std::vector<unsigned char> byte_array;

class BigInteger {
    private:

        int _sign;
        std::vector<uint32_t> _bits;

        static const BigInteger s_bnMinInt; //= BigInteger(-1, std::vector<unsigned int> {kuMaskHighBit});
        static const BigInteger s_bnOneInt;
        static const BigInteger s_bnZeroInt;
        static const BigInteger s_bnMinusOneInt;

        void AssertValid();
        static int GetDiffLength(uint_array rgu1, uint_array rgu2, int cu);
        static unsigned long MakeUlong(unsigned int uHi, unsigned int uLo);

    public:
        static constexpr unsigned int kuMaskHighBit = static_cast<unsigned int>(INT_MIN);
        static constexpr int kcbitUint = 32;
        static constexpr int kcbitUlong = 64;
        static constexpr int DecimalScaleFactorMask = 0x00FF0000;
        static constexpr int DecimalSignMask = static_cast<int>(0x80000000);

        BigInteger(int value);
        BigInteger(uint32_t value);
        BigInteger(long value);
        BigInteger(int n, std::vector<uint32_t> value);
        // ignore float BigInteger(float value);
        BigInteger(double value);
        //BigInteger(const std::vector<unsigned char> value);
        BigInteger(uint_array value, bool negative);
        //BigInteger(std::vector<uint32_t> value);

        //static BigInteger Zero();
        //static BigInteger One();
        //static BigInteger MinusOne();

        //bool IsPowerofTwo();
        //bool IsZero();
        //bool IsOne();
        //bool IsEven();

        //int Sign();
        //int Parse(std::string value);

        int CompareTo(long other);
        int CompareTo(BigInteger& other);

        //static int Compare(const BigInteger& left, const BigInteger& right);

        //static BigInteger Abs(BigInteger value);
        //static BigInteger Add(BigInteger left, BigInteger right);
        //static BigInteger Subtract(BigInteger left, BigInteger right);
        //static BigInteger Multiply(BigInteger left, BigInteger right);
        //static BigInteger Divide(BigInteger left, BigInteger right);
        //static BigInteger Remainder(BigInteger left, BigInteger right);
        //static BigInteger DivRem(BigInteger left, BigInteger right, BigInteger& remainder); // is & correct here?

        //static BigInteger Negate(BigInteger value);

        //static double Log(BigInteger value, double baseValue);
        //static double Log10(BigInteger value);

        //static BigInteger Max(BigInteger left, BigInteger right);
        //static BigInteger Min(BigInteger left, BigInteger right);

        //static BigInteger ModPow(BigInteger value, BigInteger exponent, BigInteger modulus);
        //static BigInteger Pow(BigInteger left, int exponent);

        ////bool Equals(std::any obj) const;
        //bool Equals(long long other) const;
        //bool Equals(unsigned long other) const;
        //bool Equals(BigInteger other) const;

        //int CompareTo(long long other) const;
        //int CompareTo(unsigned long other) const;
        //int CompareTo(BigInteger other) const;
        ////int CompareTo(std::any obj) const;

        //std::vector<unsigned char> ToByteArray(bool isUnsigned = false, bool isBigEndian = false);
        //int GetByteCount(bool isUnsigned = false);
        //std::string ToString();


        //friend BigInteger operator -(const BigInteger& lhs, const BigInteger& rhs);
        friend BigInteger operator +(BigInteger& lhs, BigInteger& rhs);
        //friend BigInteger operator *(const BigInteger& lhs, const BigInteger& rhs);
        //friend BigInteger operator /(const BigInteger& lhs, const BigInteger& rhs);
        //friend BigInteger operator %(const BigInteger& lhs, const BigInteger& rhs);

        //friend BigInteger operator <(const BigInteger& lhs, const BigInteger& rhs);
        //friend BigInteger operator >(const BigInteger& lhs, const BigInteger& rhs);

        //friend BigInteger operator <=(const BigInteger& lhs, const BigInteger& rhs);
        //friend BigInteger operator >=(const BigInteger& lhs, const BigInteger& rhs);

        //friend BigInteger operator ==(const BigInteger& lhs, const BigInteger& rhs);
        //friend BigInteger operator !=(const BigInteger& lhs, const BigInteger& rhs);

        //friend BigInteger operator <(const BigInteger& lhs, long& rhs);
        //friend BigInteger operator <=(const BigInteger& lhs, long& rhs);
        //friend BigInteger operator >(const BigInteger& lhs, long& rhs);
        //friend BigInteger operator >=(const BigInteger& lhs, long& rhs);
        friend BigInteger operator >=(const long& lhs, BigInteger& rhs);

        //friend BigInteger operator ==(const BigInteger& lhs, long& rhs);
        //friend BigInteger operator !=(const BigInteger& lhs, long& rhs);

        //friend BigInteger operator &(const BigInteger& lhs, const BigInteger& rhs);
        //friend BigInteger operator |(const BigInteger& lhs, const BigInteger& rhs);
        //friend BigInteger operator ^(const BigInteger& lhs, const BigInteger& rhs);
        //friend BigInteger operator <<(const BigInteger& lhs, const BigInteger& rhs);
        //friend BigInteger operator >>(const BigInteger& lhs, const BigInteger& rhs);
        //friend BigInteger operator ~(const BigInteger& value);

        //friend BigInteger operator +(const BigInteger& value);
        //friend BigInteger operator ++(const BigInteger& value);
        //friend BigInteger operator -(const BigInteger& value);
        //friend BigInteger operator --(const BigInteger& value);

        // Add
        BigInteger Add(BigInteger& lhs, BigInteger& rhs);
        BigInteger Add(uint_array& lhs, int lhsSign, uint_array& rhs, int rhsSign);
        static uint_array Add(uint_array& lhs, uint32_t rhs);
        static uint_array Add(uint_array& lhs, uint_array& rhs);

        static void Add(uint_array* lhs, int lhsLength
                , uint_array* rhs, int rhsLength, uint_array* bits, int bitsLength);

        static uint_array AddSelf(uint_array* lhs, int lhsLength
                , uint_array* rhs, int rhsLength);

        // Subtract
        BigInteger Subtract(BigInteger& lhs, BigInteger& rhs);
        BigInteger Subtract(uint_array& lhs, int lhsSign, uint_array& rhs, int rhsSign);
        static uint_array Subtract(uint_array& lhs, uint32_t rhs);
        static uint_array Subtract(uint_array& lhs, uint_array& rhs);

        static void Subtract(uint_array* lhs, int lhsLength
                , uint_array* rhs, int rhsLength, uint_array* bits, int bitsLength);

        static uint_array SubtractSelf(uint_array* lhs, int lhsLength
                , uint_array* rhs, int rhsLength);

        // Compare
        static int Compare(uint_array lhs, uint_array rhs);
        static int Compare(uint_array* lhs, int lhsLength, uint_array* rhs, int rhsLength);




        //BigInteger(std::vector<unsigned char> value);

        //friend bool operator<(const BigInteger& lhs, const BigInteger& rhs) { return lhs._value < rhs._value; }
        //friend bool operator>(const BigInteger& lhs, const BigInteger& rhs) { return rhs._value < lhs._value; }
        //friend bool operator<=(const BigInteger& lhs, const BigInteger& rhs) { return !(lhs._value > rhs._value); }
        //friend bool operator>=(const BigInteger& lhs, const BigInteger& rhs) { return !(lhs._value < rhs._value); }
        //friend bool operator==(const BigInteger& lhs, const BigInteger& rhs) { return lhs._value == rhs._value; }
        //friend bool operator!=(const BigInteger& lhs, const BigInteger& rhs) { return !(lhs._value == rhs._value); }

        //operator int() { return _value; }
        //std::vector<unsigned char> ToArray();

};

