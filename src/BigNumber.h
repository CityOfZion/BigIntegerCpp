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

        static BigNumberBuffer create();
        static BigInteger parse_biginteger(std::string value);
        static bool try_parse_biginteger(std::string value, BigInteger& result);
        static bool number_to_biginteger(BigNumberBuffer& bignumber, BigInteger& result);
        static std::string format_biginteger(const BigInteger& value);


    private:
        BigNumber() {}

        
};
