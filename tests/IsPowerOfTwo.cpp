#include "../include/public/BigInteger.h"
#include "gtest/gtest.h"
#include "Random.h"
#include "stack_calc.h"
#include "utils.h"

static int s_seed = 0;
static int onePosition = 0;

static void VerifyIsPowerOfTwo(BigInteger bigInt, bool expectedAnswer) {
    ASSERT_EQ(expectedAnswer, bigInt.is_power_of_two());
}

static bool CheckExpected(byte_array value) {
    int valueOne = 0;
    bool expected = false;
    BitArray value2(value);

    // Count the number of 1's in the value
    for (int i = 0; i < value2.size(); i++) {
        if (value2[i]) {
            valueOne++;
        }
    }

    // If only one 1 and value is positive.
    if ((1 == valueOne) && (!value2[value2.size() - 1])) {
        expected = true;
    } else {
        expected = false;
    }
    return expected;
}

static byte_array GetRandomByteArray(Random random, bool isSmall) {
    byte_array value;
    int byteValue;

    if (isSmall == true) {
        byteValue = random.Next(0, 32);
        byte_array value(byteValue, 0);
    } else {
        byteValue = random.Next(32, 128);
        byte_array value(byteValue, 0);
    }

    for (int i = 0; i < value.size(); i++) {
        value[i] = static_cast<byte>(random.Next(0, 256));
    }
    return value;
}

static byte_array GetPowerOfTwoByteArray(Random random) {
    double exactOnePosition;

    byte_array value(32, 0);
    for (int m = 0; m < value.size(); m++) {
        value[m] = 0x00;
    }

    onePosition = random.Next(0, 32);
    exactOnePosition = random.Next(0, 8);
    //ensure it isn't the sign bit that we would be flipping to 1.
    if (onePosition == 31) {
        exactOnePosition = random.Next(0, 7);
    }
    value[onePosition] = static_cast<byte>(pow(2, exactOnePosition));

    return value;
}

TEST(is_poweroftwo, RunIsPowerOfTwoTests) {
    Random random(s_seed);

    //random cases

    auto byteArray1 = GetRandomByteArray(random, true);
    auto byteArray2 = GetRandomByteArray(random, false);
    auto byteArray3 = GetPowerOfTwoByteArray(random);

    BigInteger bigIntegerSmall(byteArray1);
    BigInteger bigIntegerLarge(byteArray2);
    BigInteger bigIntegerPowerOfTwo(byteArray3);

    // Just basic tests
    // Large Power Of Two
    auto int_max = std::numeric_limits<int>::max();
    VerifyIsPowerOfTwo(static_cast<BigInteger>(int_max) +BigInteger(1) , true);

    // Large Non Power Of Two
    VerifyIsPowerOfTwo(static_cast<BigInteger>(int_max) + BigInteger(2), false);


    // Small Power Of Two
    auto short_max = std::numeric_limits<short>::max();
    VerifyIsPowerOfTwo(static_cast<BigInteger>(short_max) + BigInteger(1), true);

    // Small Non Power Of Two
    VerifyIsPowerOfTwo(static_cast<BigInteger>(int_max) - BigInteger(2), false);

    // zero Case, 1, -1
    // zero
    VerifyIsPowerOfTwo(BigInteger::zero(), false);

    // one
    VerifyIsPowerOfTwo(BigInteger::one(), true);

    // Negative one
    VerifyIsPowerOfTwo(BigInteger::minus_one(), false);

    // Random Small BigInteger
    VerifyIsPowerOfTwo(bigIntegerSmall, CheckExpected(byteArray1));

    // Random Large BigInteger
    VerifyIsPowerOfTwo(bigIntegerLarge, CheckExpected(byteArray2));

    // Random BigInteger Power of Two
    VerifyIsPowerOfTwo(bigIntegerPowerOfTwo, true);
}