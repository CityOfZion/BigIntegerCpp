#include "../src/BigInteger.h"
#include "gtest/gtest.h"
#include "Random.h"
#include "stack_calc.h"
#include "utils.h"

static void VerifyIsZero(BigInteger bigInt, bool expectedAnswer)
{
    ASSERT_EQ(expectedAnswer, bigInt.IsZero());
}

TEST(is_zero, RunIsZeroTests) {
    int s_seed = 0;
    Random random(s_seed);

    //Just basic tests
    // Zero
    VerifyIsZero(BigInteger::Zero(), true);

    // Negative One
    VerifyIsZero(BigInteger::MinusOne(), false);

    // One
    VerifyIsZero(BigInteger::One(), false);

    // -Int32.MaxValue
    auto int_max = std::numeric_limits<int>::max();
    VerifyIsZero(static_cast<BigInteger>(int_max) * BigInteger(-1), false);

    // Int32.MaxValue
    VerifyIsZero(static_cast<BigInteger>(int_max), false);

    // int32.MaxValue + 1
    VerifyIsZero(static_cast<BigInteger>(int_max) + BigInteger(1), false);

    // UInt32.MaxValue
    auto uint_max = std::numeric_limits<unsigned int>::max();
    VerifyIsZero(static_cast<BigInteger>(uint_max), false);

    // Uint32.MaxValue + 1
    VerifyIsZero(static_cast<BigInteger>(uint_max) + BigInteger(1), false);
}