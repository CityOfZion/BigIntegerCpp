#include "gtest/gtest.h"
#include "../src/BigInteger.h"
#include <random>

TEST(scratch1, scratch_tests)
{
    std::mt19937 bla(123);
    auto val = BigInteger(123);
    auto exp = BigInteger(3);
    auto x = (long)BigInteger::Pow(val, 3);

    int i = 0;

}
