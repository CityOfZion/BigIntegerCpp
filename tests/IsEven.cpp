#include "../include/public/bigintegercpp/BigInteger.h"
#include "gtest/gtest.h"
#include "Random.h"
#include "stack_calc.h"
#include "utils.h"

static void VerifyIsEven(BigInteger bigInt, bool expectedAnswer)
{
    ASSERT_EQ(expectedAnswer, bigInt.is_even());
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
        VerifyIsEven(BigInteger::parse(bigInt1) * BigInteger(2), true);
    }

    // Large Random Odd Number

    for (int i = 0; i < Reps; i++) {
        auto bigInt1 = BuildRandomNumber(random.Next() % MaxDigits + 1, random.Next());
        VerifyIsEven((BigInteger::parse(bigInt1) * BigInteger(2)) - BigInteger::one(), false);
    }

    // Small Even Number
    VerifyIsEven(static_cast<BigInteger>(std::numeric_limits<short>::max()) - BigInteger::one(), true);

    // Small Odd Number
    VerifyIsEven(static_cast<BigInteger>(std::numeric_limits<short>::max()) - BigInteger(2), false);


    //Negative tests
    // Large Negative Even Number
    VerifyIsEven((static_cast<BigInteger>(std::numeric_limits<int64_t>::max()) + BigInteger::one()) * BigInteger(-1), true);

    // Large Negative Odd Number
    VerifyIsEven((static_cast<BigInteger>(std::numeric_limits<int64_t>::max()) + BigInteger(2)) * BigInteger(-1), false);


    // Large Negative Random Even Number
    for (int i = 0; i < Reps; i++) {
        auto bigInt2 = BuildRandomNumber(random.Next() % MaxDigits + 1, random.Next());
        VerifyIsEven(BigInteger::parse(bigInt2) * BigInteger(-2), true);
    }

    // Small Negative Even Number
    VerifyIsEven((static_cast<BigInteger>(std::numeric_limits<short>::max()) - BigInteger::one()) * BigInteger(-1), true);

    // Small Negative Odd Number
    VerifyIsEven((static_cast<BigInteger>(std::numeric_limits<short>::max()) - BigInteger(2)) * BigInteger(-1), false);


    //zero Case, 1, -1
    // zero
    VerifyIsEven(BigInteger::zero(), true);

    // one
    VerifyIsEven(BigInteger::one(), false);

    // Negative one
    VerifyIsEven(BigInteger::minus_one(), false);
}