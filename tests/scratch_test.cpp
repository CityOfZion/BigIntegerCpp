#include "gtest/gtest.h"
#include "../src/BigInteger.h"
#include <random>

TEST(scratch1, scratch_tests)
{
    std::mt19937 bla(123);
    auto val = BigInteger(123);
    auto n = ~val;
    auto exp = BigInteger(3);
    auto x = (long)BigInteger::Pow(val, 3);

    int long_size = sizeof(unsigned long);
    int ll_size = sizeof(unsigned long long);
    int i = 0;



}
