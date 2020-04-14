#include "BigNumber.h"
#include "BigInteger.h"
#include "NumericsHelpers.h"
#include <sstream>
#include <limits>
#include <cassert>
#include <iostream>

BigNumber::BigNumberBuffer BigNumber::Create()
{
    BigNumber::BigNumberBuffer number;
    number.digits = std::stringstream("", std::ios_base::ate | std::ios_base::in | std::ios_base::out);
    return number;
}

BigInteger BigNumber::ParseBigInteger(std::string value)
{
    if (value.empty())
    {
        throw std::invalid_argument("value must not be NULL");
    }

    BigInteger result = BigInteger::Zero();
    if (!BigNumber::TryParseBigInteger(value, result))
    {
        throw std::runtime_error("value could not be parsed to a BigInteger");
    }

    return result;
}

static bool IsWhite(char ch)
{
    return (((ch) == 0x20) || ((ch) >= 0x09 && (ch) <= 0x0D));
}

static char* MatchChar(char* p, char* pEnd, char* str)
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
        if (cp != *str && !(*str == u'\u00a0' && cp == u'\u0020'))
        {
            break;
        }
        p++;
        str++;
        if (*str == '\0') return p;
    }
    return nullptr;
}

static char* MatchChars(char* p, char* pEnd, std::string str)
{
    char *stringPointer = &str[0];

    return MatchChar(p, pEnd, stringPointer);
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
    bool bigNumber = true; //(!sb.str().empty()); // When a StringBuilder is provided then we use it in place of the number.digits char[50]
    int maxParseDigits = std::numeric_limits<int>::max(); // NumberMaxDigits

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
                    sb << ch;

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

    sb << '\0';

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

    if (!ParseNumber(p, p + value.length(), buffer, receiver))
    {
        return false;
    }
    return true;
}

static bool TryStringToBigInteger(std::string value, std::stringstream& receiver, int& precision, int& scale, int& sign)
{
    Number::NumberBuffer numberBuffer;
    numberBuffer.overrideDigits.append(static_cast<unsigned char>(0x01), 1);

    if (!TryStringToNumber(value, numberBuffer, receiver))
    {
        precision = 0;
        scale = 0;
        sign = 0;

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

bool BigNumber::NumberToBigInteger(BigNumber::BigNumberBuffer& number, BigInteger& value)
{
    int i = number.scale;
    int cur = 0;

    BigInteger ten = 10;
    value = 0;

    while (--i >= 0)
    {
        value *= ten;
        if (number.digits.str()[cur] != '\0')
        {
            BigInteger temp = BigInteger(static_cast<int>(number.digits.str()[cur++] - '0'));
            value += temp;
        }
    }

    while (number.digits.str()[cur] != '\0')
    {
        if (number.digits.str()[cur++] != '0') return false; // Disallow non-zero trailing decimal places
    }

    if (number.sign)
    {
        value = -value;
    }
    return true;
}


bool BigNumber::TryParseBigInteger(std::string value, BigInteger& result)
{
    BigNumber::BigNumberBuffer bignumber = BigNumber::Create();
    int precision;
    int scale;
    int sign;

    if (!TryStringToBigInteger(value, bignumber.digits, precision, scale, sign))
    {
        return false;
    }

    bignumber.scale = scale;
    bignumber.precision = precision;
    bignumber.sign = sign;

    // TODO HexNumberToBigInteger
    if (!NumberToBigInteger(bignumber, result))
    {
        return false;
    }

    return true;
}


std::string BigNumber::FormatBigInteger(BigInteger& value)
{
    //TODO HEX
    //assert(formatString == null || formatString.Length == formatSpan.Length);

    int digits = 0;
    //char fmt = ParseFormatSpecifier(formatSpan, out digits);
    //if (fmt == 'x' || fmt == 'X')
    //{
    //    return FormatBigIntegerToHex(targetSpan, value, fmt, digits, info, destination, out charsWritten, out spanSuccess);
    //}


    if (value.GetBits().size() == 0)
    {
        return std::to_string(value.GetSign());
    }

    // First convert to base 10^9.
    const uint32_t kuBase = 1000000000; // 10^9
    const int kcchBase = 9;

    int cuSrc = value.GetBits().size();
    int cuTemp;
    int cuMax;
    if (((cuTemp = cuSrc * 10) < cuSrc))
    {
        throw std::overflow_error("Overflow during string conversion [cuTemp]");
    }
    else
    {
        cuMax = cuTemp / 9 + 2;
    }

    uint_array rguDst(cuMax);
    int cuDst = 0;

    for (int iuSrc = cuSrc; --iuSrc >= 0;)
    {
        uint32_t uCarry = value.GetBits()[iuSrc];
        for (int iuDst = 0; iuDst < cuDst; iuDst++)
        {
            assert(rguDst[iuDst] < kuBase);
            uint64_t uuRes = NumericsHelpers::MakeUlong(rguDst[iuDst], uCarry);
            rguDst[iuDst] = static_cast<uint32_t>(uuRes % kuBase);
            uCarry = static_cast<uint32_t>(uuRes / kuBase);
        }

        if (uCarry != 0)
        {
            rguDst[cuDst++] = uCarry % kuBase;
            uCarry /= kuBase;
            if (uCarry != 0)
                rguDst[cuDst++] = uCarry;
        }

    }

    int cchMax;
    if (((cchMax = cuDst * kcchBase) < cuDst))
    {
        throw std::overflow_error("Overflow during string conversion [cchMax]");
    }

    int rgchBufSize;

    // We'll pass the rgch buffer to native code, which is going to treat it like a string of digits, so it needs
    // to be null terminated.  Let's ensure that we can allocate a buffer of that size.
    if (((rgchBufSize = cchMax + 1) < cchMax))
    {
        throw std::overflow_error("Overflow during string conversion [rgchBufSize]");
    }

    char rgch[rgchBufSize];

    int ichDst = cchMax;

    for (int iuDst = 0; iuDst < cuDst - 1; iuDst++)
    {
        uint32_t uDig = rguDst[iuDst];
        assert(uDig < kuBase);
        for (int cch = kcchBase; --cch >= 0;)
        {
            rgch[--ichDst] = static_cast<char>('0' + (uDig % 10));
            uDig /= 10;
        }
    }

    for (uint32_t uDig = rguDst[cuDst - 1]; uDig != 0;)
    {
        rgch[--ichDst] = (char)('0' + uDig % 10);
        uDig /= 10;
    }


    //if (!decimalFmt)
    //{
    //    // sign = true for negative and false for 0 and positive values
    //    bool sign = (value._sign < 0);
    //    // The cut-off point to switch (G)eneral from (F)ixed-point to (E)xponential form
    //    int precision = 29;
    //    int scale = cchMax - ichDst;

    //    Span<char> stackSpace = stackalloc char[128]; // arbitrary stack cut-off
    //    var sb = new ValueStringBuilder(stackSpace);
    //    FormatProvider.FormatBigInteger(ref sb, precision, scale, sign, formatSpan, info, rgch, ichDst);

    //    if (targetSpan)
    //    {
    //        spanSuccess = sb.TryCopyTo(destination, out charsWritten);
    //        return null;
    //    }
    //    else
    //    {
    //        charsWritten = 0;
    //        spanSuccess = false;
    //        return sb.ToString();
    //    }
    //}

    // Format Round-trip decimal
    // This format is supported for integral types only. The number is converted to a string of
    // decimal digits (0-9), prefixed by a minus sign if the number is negative. The precision
    // specifier indicates the minimum number of digits desired in the resulting string. If required,
    // the number is padded with zeros to its left to produce the number of digits given by the
    // precision specifier.
    int numDigitsPrinted = cchMax - ichDst;
    while (digits > 0 && digits > numDigitsPrinted)
    {
        // pad leading zeros
        rgch[--ichDst] = '0';
        digits--;
    }

    if (value.GetSign() < 0)
    {
        std::string negativeSign = "-";
        for (int i = negativeSign.size() - 1; i > -1; i--)
            rgch[--ichDst] = negativeSign[i];
    }

    std::string a = std::string(rgch);
    return std::string(rgch, cchMax + 1);
}
