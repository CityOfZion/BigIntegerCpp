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
            char* overrideDigits;

            char* digits()
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

        static BigNumber::BigNumberBuffer Create();
        static BigInteger ParseBigInteger(std::string value);
        static bool TryParseBigInteger(std::string value, BigInteger& result);
        static bool TryStringToBigInteger(std::string value, std::stringstream& digits, int precision, int scale, int sign);
        static bool NumberToBigInteger(BigNumberBuffer& bignumber, BigInteger& result);


    private:
        BigNumber() {}

        
};
