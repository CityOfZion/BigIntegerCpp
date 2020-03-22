
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
        uint_array _bits;

        static const BigInteger s_bnMinInt; //= BigInteger(-1, std::vector<unsigned int> {kuMaskHighBit});
        static const BigInteger s_bnOneInt;
        static const BigInteger s_bnZeroInt;
        static const BigInteger s_bnMinusOneInt;

        static int GetDiffLength(uint_array rgu1, uint_array rgu2, int cu);
        static unsigned long MakeUlong(unsigned int uHi, unsigned int uLo);

    public:

        void AssertValid();

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


        BigInteger(int value);
        BigInteger(uint32_t value);
        BigInteger(long value);
        BigInteger(int n, std::vector<uint32_t> value);
        BigInteger(double value);
        //BigInteger(const std::vector<unsigned char> value);
        BigInteger(uint_array value, bool negative);
        //BigInteger(std::vector<uint32_t> value);

        static BigInteger Zero();
        static BigInteger One();
        static BigInteger MinusOne();

        //bool IsPowerofTwo();
        //bool IsZero();
        //bool IsOne();
        //bool IsEven();

        int Sign();
        static BigInteger Parse(std::string value);
        //static bool TryParse(std::string value, BigInteger& result);

        //int Parse(std::string value);

        int CompareTo(long other);
        int CompareTo(BigInteger& other);

        //static int Compare(const BigInteger& left, const BigInteger& right);

        //static BigInteger Negate(BigInteger value);

        //static double Log(BigInteger value, double baseValue);
        //static double Log10(BigInteger value);

        //static BigInteger Max(BigInteger left, BigInteger right);
        //static BigInteger Min(BigInteger left, BigInteger right);

        //static BigInteger ModPow(BigInteger value, BigInteger exponent, BigInteger modulus);
        //static BigInteger Pow(BigInteger left, int exponent);

        //bool Equals(long long other) const;
        //bool Equals(unsigned long other) const;
        //bool Equals(std::any obj);
        bool Equals(BigInteger& other) const;

        //int CompareTo(unsigned long other) const;
        ////int CompareTo(std::any obj) const;

        //std::vector<unsigned char> ToByteArray(bool isUnsigned = false, bool isBigEndian = false);
        //int GetByteCount(bool isUnsigned = false);
        //std::string ToString();


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
        friend BigInteger operator -(BigInteger& value); // todo
        friend BigInteger operator *=(BigInteger& lhs, BigInteger& rhs); // todo
        friend BigInteger operator +=(BigInteger& value, BigInteger& rhs); // todo
        friend BigInteger operator ++(BigInteger& value); // todo
        //friend BigInteger operator --(const BigInteger& value);

        // Divide & Modulo
        friend BigInteger operator %(BigInteger& lhs, BigInteger& rhs);
        friend BigInteger operator /(BigInteger& dividend, BigInteger& divisor);

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
        friend BigInteger operator *(BigInteger& lhs, BigInteger& rhs);
        static BigInteger Multiply(BigInteger& lhs, BigInteger& rhs);
        static uint_array Multiply(uint_array lhs, uint32_t rhs);
        static uint_array Multiply(uint_array lhs, uint_array rhs);
        static void Multiply(uint32_t* left, int leftLength, uint32_t* right, int rightLength, uint32_t* bits, int bitsLength);


        static uint_array Square(uint_array& value);
        static void Square(uint32_t* value, int valueLength, uint32_t* bits, int bitsLength);
        static void SubtractCore(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength, uint32_t* core, int coreLength);

        // Add
        friend BigInteger operator +(BigInteger& lhs, BigInteger& rhs);
        BigInteger Add(BigInteger& lhs, BigInteger& rhs);
        BigInteger Add(uint_array& lhs, int lhsSign, uint_array& rhs, int rhsSign);
        static uint_array Add(uint_array& lhs, uint32_t rhs);
        static uint_array Add(uint_array& lhs, uint_array& rhs);
        static void AddSelf(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength);
        static void Add(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength, uint32_t* bits, int bitsLength);

        // Subtract
        friend BigInteger operator -(BigInteger& lhs, BigInteger& rhs);
        BigInteger Subtract(BigInteger& lhs, BigInteger& rhs);
        BigInteger Subtract(uint_array& lhs, int lhsSign, uint_array& rhs, int rhsSign);
        static uint_array Subtract(uint_array& lhs, uint32_t rhs);
        static uint_array Subtract(uint_array& lhs, uint_array& rhs);
        static void SubtractSelf(uint_array* lhs, int lhsLength, uint_array* rhs, int rhsLength);
        static void Subtract(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength, uint32_t* bits, int bitsLength);

        // Compare
        static int Compare(uint_array lhs, uint_array rhs);
        static int Compare(uint32_t* lhs, int lhsLength, uint32_t* rhs, int rhsLength);


        //BigInteger(std::vector<unsigned char> value);

        //operator int() { return _value; }
        //std::vector<unsigned char> ToArray();

};

