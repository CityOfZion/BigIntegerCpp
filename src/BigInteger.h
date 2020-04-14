
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
    private:

        int _sign;
        uint_array _bits;

        static const BigInteger s_bnMinInt;
        static const BigInteger s_bnOneInt;
        static const BigInteger s_bnZeroInt;
        static const BigInteger s_bnMinusOneInt;

        BigInteger(uint_array value);

        static int GetDiffLength(uint_array rgu1, uint_array rgu2, int cu);
        static void DangerousMakeTwosComplement(uint_array& d);
        void AssertValid() const;



    public:
        enum GetBytesMode { AllocateArray, Count }; // currently AllocateArray only
        static unsigned long MakeUlong(unsigned int uHi, unsigned int uLo);

        //getter/setter
        int GetSign() ;
        uint_array GetBits();

        static constexpr unsigned int kuMaskHighBit = static_cast<unsigned int>(INT_MIN);
        static constexpr int kcbitUint = 32;
        static constexpr int kcbitUlong = 64;
        static constexpr int DecimalScaleFactorMask = 0x00FF0000;
        static constexpr int DecimalSignMask = static_cast<int>(0x80000000);

        static constexpr int SquareThreshold = 32;
        static constexpr int AllocationThreshold = 256;
        static constexpr int MultiplyThreshold = 32;


        BigInteger();
        BigInteger(int value);
        BigInteger(uint32_t value);
        BigInteger(long value);
        BigInteger(long long value);
        BigInteger(uint64_t value);
        BigInteger(int n, uint_array value);
        BigInteger(uint_array value, bool negative);
        BigInteger(const byte_array value, bool isUnsigned = false, bool isBigEndian = false);

        // explicit casts
        operator int();
        operator unsigned int();
        operator long();
        operator unsigned long();
        operator double();
        operator float();

        static BigInteger Zero();
        static BigInteger One();
        static BigInteger MinusOne();

        bool IsPowerOfTwo();
        bool IsZero();
        bool IsOne();
        bool IsEven();

        int Sign();

        static BigInteger Parse(std::string value);
        static bool TryParse(std::string value, BigInteger& result);

        std::string ToString();

        static int Compare(BigInteger& lhs, BigInteger& rhs);

        static BigInteger Negate(BigInteger& value);

        static double Log(BigInteger value);
        static double Log(BigInteger value, double baseValue);
        static double Log10(BigInteger value);

        static BigInteger& Max(BigInteger& lhs, BigInteger& rhs);
        static BigInteger& Min(BigInteger& lhs, BigInteger& rhs);

        static BigInteger ModPow(BigInteger value, BigInteger exponent, BigInteger modulus);
        static BigInteger Pow(BigInteger value, int exponent);

        //bool Equals(long long other) const;
        //bool Equals(unsigned long other) const;
        //bool Equals(std::any obj);
        bool Equals(uint64_t other) const;
        bool Equals(const BigInteger& other) const;


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

        BigInteger operator >=(long rhs);
        BigInteger operator >=(BigInteger& rhs);

        bool operator ==(const BigInteger& rhs);
        friend bool operator ==(const BigInteger& lhs, const BigInteger& rhs);

        bool operator !=(const BigInteger& rhs);

        // Divide & Modulo
        BigInteger operator %(BigInteger& rhs);
        BigInteger operator /(BigInteger& divisor);
        BigInteger operator /=(BigInteger& divisor);
        static BigInteger Divide(BigInteger& dividend, BigInteger& divisor);
        static uint_array Divide(uint_array& lhs, uint32_t rhs);
        static uint_array Divide(uint_array& lhs, uint32_t rhs, uint32_t& remainder); // maybe change to a more explicit out parameter
        static uint_array Divide(uint_array& lhs, uint_array& rhs, uint_array& remainder);
        static uint_array Divide(uint_array& lhs, uint_array& rhs);
        static void Divide(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength, uint32_t* bits, int bitsLength);
        static BigInteger DivRem(BigInteger& dividend, BigInteger& divisor, BigInteger& remainder);

        static uint_array Remainder(uint_array& lhs, uint_array& rhs);
        static uint32_t Remainder(uint_array& lhs, uint32_t rhs);

        // Divide helpers
        static int LeadingZeros(uint32_t value);
        static bool DivideGuessTooBig(uint64_t q, uint64_t valHi, uint32_t valLo, uint32_t divHi, uint32_t divLo);
        static uint32_t SubtractDivisor(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength, uint64_t q);
        static uint32_t AddDivisor(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength);

        // Multiply & Square
        BigInteger operator *(BigInteger& rhs);
        BigInteger operator *=(BigInteger& rhs);
        static BigInteger Multiply(BigInteger& lhs, BigInteger& rhs);
        static uint_array Multiply(uint_array lhs, uint32_t rhs);
        static uint_array Multiply(uint_array lhs, uint_array rhs);
        static void Multiply(uint32_t* left, int leftLength, uint32_t* right, int rightLength, uint32_t* bits, int bitsLength);
        static uint_array Square(uint_array& value);
        static void Square(uint32_t* value, int valueLength, uint32_t* bits, int bitsLength);
        static void SubtractCore(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength, uint32_t* core, int coreLength);

        // Add
        BigInteger operator +();
        BigInteger operator +(BigInteger& rhs);
        BigInteger operator +=(BigInteger& rhs);
        BigInteger operator +=(uint64_t rhs);
        BigInteger& operator ++(int value);
        static BigInteger Add(BigInteger& lhs, BigInteger& rhs);
        BigInteger Add(uint_array& lhs, int lhsSign, uint_array& rhs, int rhsSign);
        static uint_array Add(uint_array& lhs, uint32_t rhs);
        static uint_array Add(uint_array& lhs, uint_array& rhs);
        static void AddSelf(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength);
        static void Add(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength, uint32_t* bits, int bitsLength);

        // Subtract
        BigInteger operator -();
        BigInteger operator -(BigInteger& rhs);
        BigInteger operator -=(BigInteger& rhs);
        BigInteger& operator --(int value);
        static BigInteger Subtract(BigInteger& lhs, BigInteger& rhs);
        BigInteger Subtract(uint_array& lhs, int lhsSign, uint_array& rhs, int rhsSign);
        static uint_array Subtract(uint_array& lhs, uint32_t rhs);
        static uint_array Subtract(uint_array& lhs, uint_array& rhs);
        static void SubtractSelf(uint32_t * lhs, int lhsLength, uint32_t * rhs, int rhsLength);
        static void Subtract(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength, uint32_t* bits, int bitsLength);

        // Compare
        static int Compare(uint_array lhs, uint_array rhs);
        static int Compare(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength);
        int CompareTo(long other);
        int CompareTo(BigInteger& other);

        BigInteger operator <<(int shift);
        BigInteger operator >>(int shift);
        BigInteger operator ^(BigInteger& rhs);
        BigInteger operator &(BigInteger& rhs);
        BigInteger operator |(BigInteger& rhs);

        uint_array ToUInt32Array();
        static bool GetPartsForBitManipulation(BigInteger& x, uint_array& xd, int& xl);

        byte_array ToByteArray(bool isUnsigned = false, bool isBigEndian = false);
        byte_array ToByteArray(GetBytesMode mode, bool isUnsigned = false, bool isBigEndian = false, int* bytesWritten = 0);
        int GetByteCount(bool isUnsigned = false);

};

