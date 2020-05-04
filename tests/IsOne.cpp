#include "../src/BigInteger.h"
#include "gtest/gtest.h"
#include "Random.h"
#include "stack_calc.h"
#include "utils.h"

static void VerifyIsOne(BigInteger bigInt, bool expectedAnswer)
{
    ASSERT_EQ(expectedAnswer, bigInt.is_one());
}

TEST(is_one, IsOneTest) {
    int s_seed = 0;
    Random random(s_seed);

    //Just basic tests
    // zero
    VerifyIsOne(BigInteger::zero(), false);

    // Negative one
    VerifyIsOne(BigInteger::minus_one(), false);

    // one
    VerifyIsOne(BigInteger::one(), true);

    // -Int32.MaxValue
    auto int_max = std::numeric_limits<int>::max();
    VerifyIsOne(static_cast<BigInteger>(int_max) * BigInteger(-1), false);

    // Int32.MaxValue
    VerifyIsOne(static_cast<BigInteger>(int_max), false);

    // int32.MaxValue + 1
    VerifyIsOne(static_cast<BigInteger>(int_max) + BigInteger(1), false);

    // UInt32.MaxValue
    auto uint_max = std::numeric_limits<unsigned int>::max();
    VerifyIsOne(static_cast<BigInteger>(uint_max), false);

    // Uint32.MaxValue + 1
    VerifyIsOne(static_cast<BigInteger>(uint_max) + BigInteger(1), false);
}