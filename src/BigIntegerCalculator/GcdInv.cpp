#include "BigIntegerCalculator.h"

uint32_t BigIntegerCalculator::Gcd(uint32_t left, uint32_t right) {
    // Executes the classic Euclidean algorithm.

    // https://en.wikipedia.org/wiki/Euclidean_algorithm

    while (right != 0)
    {
        uint temp = left % right;
        left = right;
        right = temp;
    }

    return left;
}

unsigned long BigIntegerCalculator::Gcd(unsigned long left, unsigned long right)
{
    // Same as above, but for 64-bit values.

    while (right > 0xFFFFFFFF)
    {
        unsigned long temp = left % right;
        left = right;
        right = temp;
    }

    if (right != 0)
        return Gcd((uint)right, (uint)(left % right));

    return left;
}

uint32_t BigIntegerCalculator::Gcd(uint_array left, uint32_t right)
{
    assert(left.size() >= 1);
    assert(right != 0);

    // A common divisor cannot be greater than right;
    // we compute the remainder and continue above...

    auto temp = BigIntegerCalculator::Remainder(left, right);

    return Gcd(right, temp);
}

uint_array BigIntegerCalculator::Gcd(uint_array left, uint_array right)
{
assert(left.size() >= 2);
assert(right.size() >= 2);
assert(Compare(left, right) >= 0);

BitsBuffer leftBuffer(left.size(), left);
BitsBuffer rightBuffer(right.size(), right);

BigIntegerCalculator::Gcd(leftBuffer, rightBuffer);

return leftBuffer.GetBits();
}

void BigIntegerCalculator::Gcd(BitsBuffer& left, BitsBuffer& right) {
    assert(left.GetLength() >= 2);
    assert(right.GetLength() >= 2);
    assert(left.GetLength() >= right.GetLength());

    // Executes Lehmer's gcd algorithm, but uses the most
    // significant bits to work with 64-bit (not 32-bit) values.
    // Furthermore we're using an optimized version due to Jebelean.

    // http://cacr.uwaterloo.ca/hac/about/chap14.pdf (see 14.4.2)
    // ftp://ftp.risc.uni-linz.ac.at/pub/techreports/1992/92-69.ps.gz

    while (right.GetLength() > 2)
    {
        unsigned long x, y;

        ExtractDigits(left, right, x, y);

        uint a = 1U, b = 0U;
        uint c = 0U, d = 1U;

        int iteration = 0;

        // Lehmer's guessing
        while (y != 0)
        {
            unsigned long q, r, s, t;

            // Odd iteration
            q = x / y;

            if (q > 0xFFFFFFFF)
                break;

            r = a + q * c;
            s = b + q * d;
            t = x - q * y;

            if (r > 0x7FFFFFFF || s > 0x7FFFFFFF)
                break;
            if (t < s || t + r > y - c)
                break;

            a = (uint)r;
            b = (uint)s;
            x = t;

            ++iteration;
            if (x == b)
                break;

            // Even iteration
            q = y / x;

            if (q > 0xFFFFFFFF)
                break;

            r = d + q * b;
            s = c + q * a;
            t = y - q * x;

            if (r > 0x7FFFFFFF || s > 0x7FFFFFFF)
                break;
            if (t < s || t + r > x - b)
                break;

            d = (uint)r;
            c = (uint)s;
            y = t;

            ++iteration;
            if (y == c)
                break;
        }

        if (b == 0)
        {
            // Euclid's step
            left.Reduce(right);

            BitsBuffer temp = left;
            left = right;
            right = temp;
        }
        else
        {
            // Lehmer's step
            LehmerCore(left, right, a, b, c, d);

            if (iteration % 2 == 1)
            {
                // Ensure left is larger than right
                BitsBuffer temp = left;
                left = right;
                right = temp;
            }
        }
    }

    if (right.GetLength() > 0)
    {
        // Euclid's step
        left.Reduce(right);

        uint_array xBits = right.GetBits();
        uint_array yBits = left.GetBits();

        unsigned long x = (static_cast<unsigned long>(xBits[1]) << 32) | xBits[0];
        unsigned long y = (static_cast<unsigned long>(yBits[1]) << 32) | yBits[0];

        left.Overwrite(Gcd(x, y));
        right.Overwrite(0UL);
    }
}

void BigIntegerCalculator::ExtractDigits(BitsBuffer &xBuffer, BitsBuffer &yBuffer, unsigned long& x, unsigned long& y) {
    assert(xBuffer.GetLength() >= 3);
    assert(yBuffer.GetLength() >= 3);
    assert(xBuffer.GetLength() >= yBuffer.GetLength());

    // Extracts the most significant bits of x and y,
    // but ensures the quotient x / y does not change!

    uint_array xBits = xBuffer.GetBits();
    int xLength = xBuffer.GetLength();

    uint_array yBits = yBuffer.GetBits();
    int yLength = yBuffer.GetLength();

    unsigned long xh = xBits[xLength - 1];
    unsigned long xm = xBits[xLength - 2];
    unsigned long xl = xBits[xLength - 3];

    unsigned long yh, ym, yl;

    // arrange the bits
    switch (xLength - yLength)
    {
        case 0:
            yh = yBits[yLength - 1];
            ym = yBits[yLength - 2];
            yl = yBits[yLength - 3];
            break;

        case 1:
            yh = 0UL;
            ym = yBits[yLength - 1];
            yl = yBits[yLength - 2];
            break;

        case 2:
            yh = 0UL;
            ym = 0UL;
            yl = yBits[yLength - 1];
            break;

        default:
            yh = 0UL;
            ym = 0UL;
            yl = 0UL;
            break;
    }

    // Use all the bits but one, see [hac] 14.58 (ii)
    int z = LeadingZeros((uint)xh);

    x = ((xh << (32 + z)) | (xm << z) | (xl >> (32 - z))) >> 1;
    y = ((yh << (32 + z)) | (ym << z) | (yl >> (32 - z))) >> 1;

    assert(x >= y);
}

void BigIntegerCalculator::LehmerCore(BitsBuffer &xBuffer, BitsBuffer &yBuffer, long a, long b, long c, long d) {
    assert(xBuffer.GetLength() >= 1);
    assert(yBuffer.GetLength() >= 1);
    assert(xBuffer.GetLength() >= yBuffer.GetLength());
    assert(a <= 0x7FFFFFFF && b <= 0x7FFFFFFF);
    assert(c <= 0x7FFFFFFF && d <= 0x7FFFFFFF);

    // Executes the combined calculation of Lehmer's step.

    uint_array x = xBuffer.GetBits();
    uint_array y = yBuffer.GetBits();

    int length = yBuffer.GetLength();

    long xCarry = 0L, yCarry = 0L;
    for (int i = 0; i < length; i++)
    {
        long xDigit = a * x[i] - b * y[i] + xCarry;
        long yDigit = d * y[i] - c * x[i] + yCarry;
        xCarry = xDigit >> 32;
        yCarry = yDigit >> 32;
        x[i] = static_cast<unsigned int>(xDigit);
        y[i] = static_cast<unsigned int>(yDigit);
    }

    xBuffer.Refresh(length);
    yBuffer.Refresh(length);
}
