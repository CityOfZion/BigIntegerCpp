#include "BigNumber.h"
#include "BigInteger.h"
#include <sstream>
#include <limits>
#include <cassert>

static bool TryParseBigInteger(std::string value, BigInteger& result);
//static bool NumberToBigInteger(BigNumber::BigNumberBuffer& bignumber, BigInteger& result);

static BigNumber::BigNumberBuffer Create()
{
    BigNumber::BigNumberBuffer number;
    number.digits = std::stringstream("", std::ios_base::ate | std::ios_base::in | std::ios_base::out);
    return number;
}

static BigInteger ParseBigInteger(std::string value)
{
    if (value.empty())
    {
        throw std::invalid_argument("value must not be NULL");
    }

    BigInteger result = BigInteger::Zero();
    if (!TryParseBigInteger(value, result))
    {
        throw std::runtime_error("value could not be parsed to a BigInteger");
    }

    return result;
}

static bool IsWhite(char ch)
{
    return (((ch) == 0x20) || ((ch) >= 0x09 && (ch) <= 0x0D));
}

static char* MatchChars(char* p, char* pEnd, std::string str)
{
    char *stringPointer = &str[0];

    return MatchChars(p, pEnd, stringPointer);
}

static char* MatchChars(char* p, char* pEnd, char* str)
{
    assert(p != nullptr && pEnd != nullptr && p <= pEnd && str != nullptr);

    if (*str == '\0')
    {
        return nullptr;
    }

    // We only hurt the failure case
    // This fix is for French or Kazakh cultures. Since a user cannot type 0xA0 as a
    // space character we use 0x20 space character instead to mean the same.
    while (true)
    {
        char cp = p < pEnd ? *p : '\0';
        if (cp != *str && !(*str == u'\u00a0' && cp == '\u0020'))
        {
            break;
        }
        p++;
        str++;
        if (*str == '\0') return p;
    }
    return nullptr;
}

static bool ParseNumber(char* str, char* strEnd, Number::NumberBuffer& number, std::stringstream& sb)
{
    assert(str != nullptr);
    assert(strEnd != nullptr);
    assert(str <= strEnd);

    const int StateSign = 0x0001;
    const int StateParens = 0x0002;
    const int StateDigits = 0x0004;
    const int StateNonZero = 0x0008;
    const int StateDecimal = 0x0010;

    number.scale = 0;
    number.sign = false;
    std::string decSep = ".";   // Decimal separator from NumberFormatInfo.
    std::string groupSep = ""; // Group separator from NumberFormatInfo.

    int state = 0;
    bool bigNumber = (!sb.str().empty()); // When a StringBuilder is provided then we use it in place of the number.digits char[50]
    int maxParseDigits = bigNumber ? std::numeric_limits<int>::max() : 32; // NumberMaxDigits

    char* p = str;
    char ch = p < strEnd ? *p : '\0';
    char* next;

    std::string dig = number.digits();
    char const * NegativeSign = "-";
    char const * PositiveSign = "+";

    // currently hardcoded -> should change though
    bool AllowHexSpecifier = true;
    bool AllowDecimalPoint = true;
    bool AllowThousands = true;

    while (true)
    {
        // Eat whitespace unless we've found a sign which isn't followed by a currency symbol.
        // "-Kr 1231.47" is legal but "- 1231.47" is not.
        if (!IsWhite(ch) || ((state & StateSign) != 0 ))
        {
            if (((state & StateSign) == 0) && ((next = MatchChars(p, strEnd, PositiveSign)) != nullptr
                        || ((next = MatchChars(p, strEnd, NegativeSign)) != nullptr && (number.sign = true))))
            {
                state |= StateSign;
                p = next - 1;
            }
            else
            {
                break;
            }
        }
        ch = ++p < strEnd ? *p : '\0';
    }

    int digCount = 0;
    int digEnd = 0;
    while (true)
    {
        if ((ch >= '0' && ch <= '9') || (AllowHexSpecifier && ((ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))))
        {
            state |= StateDigits;

            if (ch != '0' || (state & StateNonZero) != 0 || (bigNumber && AllowHexSpecifier))
            {
                if (digCount < maxParseDigits)
                {
                    if (bigNumber)
                    {
                        sb << ch;
                    }
                    else
                    {
                        dig[digCount++] = ch;
                    }

                    //if (ch != '0' || parseDecimal)
                    if (ch != '0')
                    {
                        digEnd = digCount;
                    }
                }
                if ((state & StateDecimal) == 0)
                {
                    number.scale++;
                }
                state |= StateNonZero;
            }
            else if ((state & StateDecimal) != 0)
            {
                number.scale--;
            }
        }
        else if (AllowDecimalPoint && ((state & StateDecimal) == 0) && ((next = MatchChars(p, strEnd, decSep)) != nullptr))
        {
            state |= StateDecimal;
            p = next - 1;
        }
        else if (AllowThousands && ((state & StateDigits) != 0) && ((state & StateDecimal) == 0) 
                && ((next = MatchChars(p, strEnd, groupSep)) != nullptr))
        {
            p = next - 1;
        }
        else
        {
            break;
        }
        ch = ++p < strEnd ? *p : '\0';
    }

    number.precision = digEnd;
    if (bigNumber)
        sb << '\0';
    else
        dig[digEnd] = '\0';
    if ((state & StateDigits) != 0)
    {
        //while (true)
        //{
        //    if (!IsWhite(ch) || (options & NumberStyles.AllowTrailingWhite) == 0)
        //    {
        //        if (((options & NumberStyles.AllowTrailingSign) != 0 && ((state & StateSign) == 0)) && ((next = MatchChars(p, strEnd, numfmt.PositiveSign)) != null || (((next = MatchChars(p, strEnd, numfmt.NegativeSign)) != null) && (number.sign = true))))
        //        {
        //            state |= StateSign;
        //            p = next - 1;
        //        }
        //        else if (ch == ')' && ((state & StateParens) != 0))
        //        {
        //            state &= ~StateParens;
        //        }
        //        else
        //        {
        //            break;
        //        }
        //    }
        //    ch = ++p < strEnd ? *p : '\0';
        //}
        if ((state & StateParens) == 0)
        {
            if ((state & StateNonZero) == 0)
            {
                number.scale = 0;

                if ((state & StateDecimal) == 0)
                {
                    number.sign = false;
                }
            }
            str = p;
            return true;
        }
    }
    str = p;
    return false;
}

static bool TryStringToNumber(std::string value, Number::NumberBuffer& buffer, std::stringstream& receiver)
{

    char* stringPointer = &value[0];
    char* p = stringPointer;

    if (!ParseNumber(p, p+value.length(), buffer, receiver))
    {
        return false;
    }

    return true;
}

static bool TryStringToBigInteger(std::string value, std::stringstream& receiver, int& precision, int& scale, int& sign)
{
    Number::NumberBuffer numberBuffer;
    numberBuffer.overrideDigits.append((const char *)0x1);

    if (!TryStringToNumber(value, numberBuffer, receiver))
    {
        precision = 0;
        scale = 0;
        sign = false;

        return false;
    }
    else
    {
        precision = numberBuffer.precision;
        scale = numberBuffer.scale;
        sign = numberBuffer.sign;

        return true;
    }
}

static bool NumberToBigInteger(BigNumber::BigNumberBuffer& number, BigInteger& value)
{
    int i = number.scale;
    int cur = 0;

    BigInteger ten = 10;
    value = 0;
    while (--i >= 0)
    {
        value *= ten; // todo *= operator
        if (number.digits.str()[cur] != '\0')
        {
            value += number.digits.str()[cur++] - '0'; // todo += biginit + int
        }
    }
    while (number.digits.str()[cur] != '\0')
    {
        if (number.digits.str()[cur++] != '0') return false; // Disallow non-zero trailing decimal places
    }
    if (number.sign)
    {
        value = value * (-1); // todod -operator
    }
    return true;
}


static bool TryParseBigInteger(std::string value, BigInteger& result)
{
    BigNumber::BigNumberBuffer bignumber = BigNumber::Create();
    int precision;
    int scale;
    int sign;

    if (!TryStringToBigInteger(value, bignumber.digits, precision, scale, sign));
    {
        return false;
    }

    // TODO HexNumberToBigInteger
    if (!NumberToBigInteger(bignumber, result));
    {
        return false;
    }

    return true;
}

static bool NumberToBigInteger(BigNumber::BigNumberBuffer& bignumber, BigInteger& result)
{

}


