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
        unsigned long uu;
    };

    static double GetDoubleFromParts(int sign, int exp, unsigned long man) {
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
                du.uu = (man & 0x000FFFFFFFFFFFFF) | (static_cast<unsigned long>(exp) << 52);
        }

        if (sign < 0)
            du.uu |= 0x8000000000000000;
        return du.dbl;
    }
};