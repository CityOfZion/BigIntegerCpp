#include "gtest/gtest.h"
#include "../src/BigInteger.h"
#include <random>

TEST(scratch1, scratch_tests)
{
    int tmp_int = 1234;
    std::vector<unsigned char> v;
    memcpy(&v[0], &tmp_int, 4);
    std::mt19937 bla(123);
    auto val = BigInteger(123);
    auto exp = BigInteger(3);
    auto x = (long)BigInteger::Pow(val, 3);

    int i = 0;

}
