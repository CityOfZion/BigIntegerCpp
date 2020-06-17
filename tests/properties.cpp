#include "../include/public/BigInteger.h"
#include "gtest/gtest.h"
#include "Random.h"
#include "stack_calc.h"
#include "utils.h"
#include "../include/exceptions.h"

static byte_array GetRandomByteArray(Random random) {
    return MyBigIntImp::GetRandomByteArray(random, random.Next(0, 1024));
}

TEST(properties, RunZeroTests) {
    int s_samples = 10;
    Random s_random(100);

    BigInteger bigInteger;
    byte_array tempByteArray;

    // BigInteger::zero() == 0
    ASSERT_EQ("0", BigInteger::zero().to_string());
    ASSERT_EQ(BigInteger((uint64_t)(0)), BigInteger::zero());
    ASSERT_EQ(BigInteger((double) (0)), BigInteger::zero());
    ASSERT_EQ(BigInteger(byte_array{0, 0, 0, 0}), BigInteger::zero());
    ASSERT_EQ(BigInteger::one() + BigInteger::minus_one(), BigInteger::zero());
    ASSERT_EQ(BigInteger::one() - BigInteger::one(), BigInteger::zero());
    ASSERT_EQ(BigInteger::minus_one() - BigInteger::minus_one(), BigInteger::zero());

    // Identities with BigInteger::zero()
    for (int i = 0; i < s_samples; i++) {
        tempByteArray = GetRandomByteArray(s_random);
        bigInteger = BigInteger(tempByteArray);

        ASSERT_EQ(BigInteger::zero(), BigInteger::zero() * bigInteger);
        ASSERT_EQ(bigInteger, bigInteger - BigInteger::zero());
        ASSERT_EQ(BigInteger::minus_one() * bigInteger, BigInteger::zero() - bigInteger);
        ASSERT_EQ(bigInteger, bigInteger + BigInteger::zero());
        ASSERT_EQ(bigInteger, BigInteger::zero() + bigInteger);

        EXPECT_THROW(bigInteger / BigInteger::zero(), DivideByZero);

        if (!MyBigIntImp::IsZero(tempByteArray)) {
            ASSERT_EQ(BigInteger::zero(), BigInteger::zero() / bigInteger);
        }
    }
}

TEST(properties, RunOneTests) {
    int s_samples = 10;
    Random s_random(100);

    BigInteger bigInteger;
    byte_array tempByteArray;

    // BigInteger::one() == 1
    ASSERT_EQ("1", BigInteger::one().to_string());
    ASSERT_EQ(BigInteger((int64_t)(1)), BigInteger::one());
    ASSERT_EQ(BigInteger((double) (1)), BigInteger::one());
    ASSERT_EQ(BigInteger(byte_array{1, 0, 0, 0}), BigInteger::one());
    ASSERT_EQ(BigInteger::zero() - BigInteger::minus_one(), BigInteger::one());
    ASSERT_EQ((BigInteger) 671832 / (BigInteger) 671832, BigInteger::one());

    // Identities with BigInteger::one()
    for (int i = 0; i < s_samples; i++) {
        tempByteArray = GetRandomByteArray(s_random);

        bigInteger = BigInteger(tempByteArray);

        ASSERT_EQ(bigInteger, BigInteger::one() * bigInteger);
        ASSERT_EQ(bigInteger, bigInteger / BigInteger::one());
    }
}

TEST(properties, RunMinusOneTests) {
    int s_samples = 10;
    Random s_random(100);

    BigInteger bigInteger;
    byte_array tempByteArray;

    // BigInteger::minus_one() == -1
    ASSERT_EQ("-1", BigInteger::minus_one().to_string());
    ASSERT_EQ(BigInteger((int64_t)(-1)), BigInteger::minus_one());
    ASSERT_EQ(BigInteger((double) (-1)), BigInteger::minus_one());
    ASSERT_EQ(BigInteger(byte_array{0xff, 0xff, 0xff, 0xff}), BigInteger::minus_one());
    ASSERT_EQ(BigInteger::zero() - BigInteger::one(), BigInteger::minus_one());
    ASSERT_EQ((BigInteger) 671832 / (BigInteger) (-671832), BigInteger::minus_one());

    // Identities with BigInteger::minus_one()
    for (int i = 0; i < s_samples; i++) {
        tempByteArray = GetRandomByteArray(s_random);

        bigInteger = BigInteger(tempByteArray);

        ASSERT_EQ(BigInteger::negate(BigInteger(tempByteArray)), BigInteger::minus_one() * bigInteger);

        ASSERT_EQ(BigInteger::negate(BigInteger(tempByteArray)), bigInteger / BigInteger::minus_one());
    }
}