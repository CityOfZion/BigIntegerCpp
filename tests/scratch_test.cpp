#include "gtest/gtest.h"
#include "../src/BigInteger.h"

TEST(scratch1, scratch_tests)
{
    auto val = BigInteger(123);
    auto exp = BigInteger(3);
    auto x = (long)BigInteger::Pow(val, 3);

    int i = 0;

}
