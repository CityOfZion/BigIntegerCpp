#include "../src/BigInteger.h"
#include "gtest/gtest.h"
#include "Random.h"
#include "stack_calc.h"
#include "utils.h"

static void VerifyIsEven(BigInteger bigInt, bool expectedAnswer)
{
    ASSERT_EQ(expectedAnswer, bigInt.IsEven());
}

TEST(is_even, RunIsEvenTests) {

    const int MaxDigits = 400;
    const int Reps = 5;
    int s_seed = 0;

    Random random(s_seed);

    // Just basic tests
    // Large Even Number
    auto long_max_val = std::numeric_limits<int64_t>::max();
    VerifyIsEven(static_cast<BigInteger>(long_max_val) + BigInteger(1), true);

    // Large Odd Number
    VerifyIsEven(static_cast<BigInteger>(long_max_val) + BigInteger(2), false);

    // Large Random Even Number

    for (int i = 0; i < Reps; i++) {
        auto bigInt1 = BuildRandomNumber(random.Next() % MaxDigits + 1, random.Next());
        VerifyIsEven(BigInteger::Parse(bigInt1) * BigInteger(2), true);
    }

    // Large Random Odd Number

    for (int i = 0; i < Reps; i++) {
        auto bigInt1 = BuildRandomNumber(random.Next() % MaxDigits + 1, random.Next());
        VerifyIsEven((BigInteger::Parse(bigInt1) * BigInteger(2)) - BigInteger::One(), false);
    }

    // Small Even Number
    VerifyIsEven(static_cast<BigInteger>(std::numeric_limits<short>::max()) - BigInteger::One(), true);

    // Small Odd Number
    VerifyIsEven(static_cast<BigInteger>(std::numeric_limits<short>::max()) - BigInteger(2), false);


    //Negative tests
    // Large Negative Even Number
    VerifyIsEven((static_cast<BigInteger>(std::numeric_limits<int64_t>::max()) + BigInteger::One()) * BigInteger(-1), true);

    // Large Negative Odd Number
    VerifyIsEven((static_cast<BigInteger>(std::numeric_limits<int64_t>::max()) + BigInteger(2)) * BigInteger(-1), false);


    // Large Negative Random Even Number
    for (int i = 0; i < Reps; i++) {
        auto bigInt2 = BuildRandomNumber(random.Next() % MaxDigits + 1, random.Next());
        VerifyIsEven(BigInteger::Parse(bigInt2) * BigInteger(-2), true);
    }

    // Small Negative Even Number
    VerifyIsEven((static_cast<BigInteger>(std::numeric_limits<short>::max()) - BigInteger::One()) * BigInteger(-1), true);

    // Small Negative Odd Number
    VerifyIsEven((static_cast<BigInteger>(std::numeric_limits<short>::max()) - BigInteger(2)) * BigInteger(-1), false);


    //Zero Case, 1, -1
    // Zero
    VerifyIsEven(BigInteger::Zero(), true);

    // One
    VerifyIsEven(BigInteger::One(), false);

    // Negative One
    VerifyIsEven(BigInteger::MinusOne(), false);
}