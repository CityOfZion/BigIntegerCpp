#include "gtest/gtest.h"
#include "../src/BigInteger.h"
#include <random>

TEST(scratch1, scratch_tests)
{
    auto b1 = BigInteger(byte_array{8, 24, 67, 103, 87});
    auto b2 = BigInteger(byte_array{8, 24});
    auto r = std::max(b1, b2);
    int i = 0;



}
