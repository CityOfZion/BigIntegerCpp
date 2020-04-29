#pragma once

class NumericsHelpers {
    public:

    static int CbitHighZero(unsigned int u)
    {
        if (u == 0)
            return 32;

        int cbit = 0;
        if ((u & 0xFFFF0000) == 0)
        {
            cbit += 16;
            u <<= 16;
        }
        if ((u & 0xFF000000) == 0)
        {
            cbit += 8;
            u <<= 8;
        }
        if ((u & 0xF0000000) == 0)
        {
            cbit += 4;
            u <<= 4;
        }
        if ((u & 0xC0000000) == 0)
        {
            cbit += 2;
            u <<= 2;
        }
        if ((u & 0x80000000) == 0)
            cbit += 1;
        return cbit;
    }

    struct DoubleULong {
        double dbl;
        uint64_t uu;
    };

    static double GetDoubleFromParts(int sign, int exp, uint64_t man) {
        DoubleULong du;
        du.dbl = 0;

        if (man == 0)
            du.uu = 0;
        else {
            int cbitShift = CbitHighZero(man) - 11;
            if (cbitShift < 0)
                man >>= -cbitShift;
            else
                man <<= cbitShift;
            exp -= cbitShift;

            exp += 1075;

            if (exp >= 0x7FF)
                du.uu = 0x7FF0000000000000;
            else if (exp <= 0)
            {
                exp--;
                if (exp < -52)
                    du.uu = 0;
                else
                    du.uu = man >> -exp;
            }
            else
                du.uu = (man & 0x000FFFFFFFFFFFFF) | (static_cast<uint64_t>(exp) << 52);
        }

        if (sign < 0)
            du.uu |= 0x8000000000000000;
        return du.dbl;
    }

    static void DangerousMakeTwosComplement(uint_array& d)
    {
        //dangerous because mutates d!
        if (d.size() > 0)
        {
            d[0] = ~d[0] + 1;

            size_t i = 1;
            // first do complement and +1 as long as carry is needed
            for (; d[i - 1] == 0 && i < d.size(); i++)
            {
                d[i] = ~d[i] + 1;
            }
            // now ones complement is sufficient
            for (; i < d.size(); i++)
            {
                d[i] = ~d[i];
            }
        }
    }

    static uint64_t MakeUlong(unsigned int uHi, unsigned int uLo) {
            return (static_cast<uint64_t>(uHi) << BigInteger::kcbitUint) | uLo;
    }

    static void GetDoubleParts(double dbl, int& sign, int& exp, uint64_t& man, bool& fFinite) {
        DoubleULong du;
        du.uu = 0;
        du.dbl = dbl;

        sign = 1 - ((int)(du.uu >> 62) & 2);
        man = du.uu & 0x000FFFFFFFFFFFFF;
        exp = (int)(du.uu >> 52) & 0x7FF;
        if (exp == 0)
        {
            // Denormalized number.
            fFinite = true;
            if (man != 0)
                exp = -1074;
        }
        else if (exp == 0x7FF)
        {
            // NaN or Infinite.
            fFinite = false;
            exp = std::numeric_limits<uint32_t>::max();
        }
        else
        {
            fFinite = true;
            man |= 0x0010000000000000;
            exp -= 1075;
        }
    }

    static uint CombineHash(uint u1, uint u2)
    {
        return ((u1 << 7) | (u1 >> 25)) ^ u2;
    }

    static int CombineHash(int n1, int n2) {
        return static_cast<int>(CombineHash(static_cast<uint>(n1), static_cast<uint>(n2)));
    }
};