#include "BigInteger.h"
#include <sstream>
#include <string>


class Number
{
    public:
        struct NumberBuffer
        {
            int precision;
            int scale;
            int sign;
            std::string overrideDigits;

            std::string digits()
            {
                return overrideDigits;
            }
        };
};

class BigNumber
{
    public:

        struct BigNumberBuffer
        {
            std::stringstream digits;
            int precision;
            int scale;
            bool sign;
        };

        static BigNumberBuffer Create();
        static BigInteger ParseBigInteger(std::string value);
        static bool TryParseBigInteger(std::string value, BigInteger& result);
        static bool NumberToBigInteger(BigNumberBuffer& bignumber, BigInteger& result);
        static std::string FormatBigInteger(BigInteger& value);


    private:
        BigNumber() {}

        
};
