#include "../include/public/BigInteger.h"
#include "gtest/gtest.h"
#include <string>
#include "../include/NumericsHelpers.h"
#include "stack_calc.h"
#include "Random.h"


static void VerifyLogString(std::string opstring) {
    StackCalc sc(opstring);
    while (sc.DoNextOperation()) {
        ASSERT_EQ(sc.snCalc.top().to_string(), sc.myCalc.top().to_string());
    }
}

static byte_array GetRandomPosByteArray(Random random, int size)
{
    byte_array value(size, 0);

    for (int i = 0; i < value.size(); ++i)
    {
        value[i] = (byte)random.Next(0, 256);
    }
    value[value.size() - 1] &= 0x7F;

    return value;
}

static byte_array GetRandomNegByteArray(Random random, int size)
{
    byte_array value(size, 0);

    for (int i = 0; i < value.size(); ++i)
    {
        value[i] = (byte)random.Next(0, 256);
    }
    value[value.size() - 1] |= 0x80;

    return value;
}

static byte_array GetRandomByteArray(Random random, int size) {
    return MyBigIntImp::GetRandomByteArray(random, size);
}

static byte_array GetRandomByteArray(Random random) {
    return GetRandomByteArray(random, random.Next(0, 100));
}

static std::string Print(byte_array bytes) {
    return MyBigIntImp::Print(bytes);
}

static bool ApproxEqual(double value1, double value2) {
    //Special case values;
    if (std::isnan(value1)) {
        return std::isnan(value2);
    }
    if (value1 == -std::numeric_limits<double>::infinity()) {
        return value2 == -std::numeric_limits<double>::infinity();
    }
    if (value1 == std::numeric_limits<double>::infinity()) {
        return value2 == std::numeric_limits<double>::infinity();
    }
    if (value2 == 0) {
        return (value1 == 0);
    }

    double result = std::abs((value1 / value2) - 1);
    return (result <= 1 * std::pow(10, -15));
}

static void VerifyIdentityString(std::string opstring1, std::string opstring2) {
    StackCalc sc1(opstring1);
    while (sc1.DoNextOperation()) {
        //Run the full calculation
        sc1.DoNextOperation();
    }

    StackCalc sc2(opstring2);
    while (sc2.DoNextOperation()) {
        //Run the full calculation
        sc2.DoNextOperation();
    }
    ASSERT_EQ(sc1.snCalc.top().to_string(), sc2.snCalc.top().to_string());
}

static void LargeValueLogTests(int startShift, int bigShiftLoopLimit, int smallShift = 0, int smallShiftLoopLimit = 1) {
    BigInteger init = BigInteger::one() << startShift;
    double logbase = 2;

    for (int i = 0; i < smallShiftLoopLimit; i++) {
        BigInteger temp = init << ((i + 1) * smallShift);

        for (int j = 0; j < bigShiftLoopLimit; j++) {
            temp = temp << (std::numeric_limits<int>::max() / 10);
            double expected =
                    (double) startShift +
                    smallShift * (double) (i + 1) +
                    (std::numeric_limits<int>::max() / 10) * (double) (j + 1);
            ASSERT_TRUE(ApproxEqual(BigInteger::log(temp, logbase), expected));
        }
    }
}


TEST(log_tests, log) {
    int s_samples = 10;
    Random s_random(100);
    byte_array tempByteArray1;
    byte_array tempByteArray2;

    BigInteger bi;

    // Log Method - Log(1,+Infinity)
    ASSERT_EQ(0, BigInteger::log(1, std::numeric_limits<double>::infinity()));

    // Log Method - Log(1,0)
    VerifyLogString("0 1 bLog");

    // Log Method - Log(0, >1)
    for (int i = 0; i < s_samples; i++) {
        tempByteArray1 = GetRandomPosByteArray(s_random, 10);
        VerifyLogString(Print(tempByteArray1) + "0 bLog");
    }

    // Log Method - Log(0, 0>x>1)
    for (int i = 0; i < s_samples; i++) {
        ASSERT_EQ(std::numeric_limits<double>::infinity(), BigInteger::log(0, s_random.NextDouble()));
    }

    // Log Method - base = 0
    auto b1 = BigInteger(1);
    for (int i = 0; i < s_samples; i++) {
        bi = 1;
        while (bi == b1) {
            bi = BigInteger(GetRandomPosByteArray(s_random, 8));
        }
        ASSERT_TRUE((std::isnan(BigInteger::log(bi, 0))));
    }

    // Log Method - base = 1
    for (int i = 0; i < s_samples; i++) {
        tempByteArray1 = GetRandomByteArray(s_random);
        VerifyLogString("1 " + Print(tempByteArray1) + "bLog");
    }

    //     Log Method - base = NaN
    for (int i = 0; i < s_samples; i++) {
        ASSERT_TRUE(BigInteger::double_IsNaN(BigInteger::log(BigInteger(GetRandomByteArray(s_random, 10)), std::nan(""))));
    }

    // Log Method - base = +Infinity
    for (int i = 0; i < s_samples; i++) {
        ASSERT_TRUE(std::isnan(BigInteger::log(BigInteger(GetRandomByteArray(s_random, 10)), std::numeric_limits<double>::infinity())));
    }

    // Log Method - Log(0,1)
    VerifyLogString("1 0 bLog");

    // Log Method - base < 0
    for (int i = 0; i < s_samples; i++) {
        tempByteArray1 = GetRandomByteArray(s_random, 10);
        tempByteArray2 = GetRandomNegByteArray(s_random, 1);
        VerifyLogString(Print(tempByteArray2) + Print(tempByteArray1) + "bLog");
        ASSERT_TRUE(std::isnan(
                BigInteger::log(BigInteger(GetRandomByteArray(s_random, 10)), -s_random.NextDouble())));
    }

    // Log Method - value < 0
    for (int i = 0; i < s_samples; i++) {
        tempByteArray1 = GetRandomNegByteArray(s_random, 10);
        tempByteArray2 = GetRandomPosByteArray(s_random, 1);
        VerifyLogString(Print(tempByteArray2) + Print(tempByteArray1) + "bLog");
    }

    // Log Method - Small BigInteger and 0<base<0.5
    for (int i = 0; i < s_samples; i++) {
        auto d = GetRandomPosByteArray(s_random, 10);
        BigInteger temp(d);
        double newbase = std::min(s_random.NextDouble(), 0.5);
        ASSERT_TRUE(ApproxEqual(BigInteger::log(temp, newbase), NumericsHelpers::csharp_log_wrapper((double) temp, newbase)));
    }

    // Log Method - Large BigInteger and 0<base<0.5
    for (int i = 0; i < s_samples; i++) {
        BigInteger temp = BigInteger(GetRandomPosByteArray(s_random, s_random.Next(1, 100)));
        double newbase = std::min(s_random.NextDouble(), 0.5);
        ASSERT_TRUE(ApproxEqual(BigInteger::log(temp, newbase), NumericsHelpers::csharp_log_wrapper((double) temp, newbase)));
    }

    // Log Method - two small BigIntegers
    for (int i = 0; i < s_samples; i++) {
        tempByteArray1 = GetRandomPosByteArray(s_random, 2);
        tempByteArray2 = GetRandomPosByteArray(s_random, 3);
        VerifyLogString(Print(tempByteArray1) + Print(tempByteArray2) + "bLog");
    }

    // Log Method - one small and one large BigIntegers
    for (int i = 0; i < s_samples; i++) {
        tempByteArray1 = GetRandomPosByteArray(s_random, 1);
        tempByteArray2 = GetRandomPosByteArray(s_random, s_random.Next(1, 100));
        VerifyLogString(Print(tempByteArray1) + Print(tempByteArray2) + "bLog");
    }

    // Log Method - two large BigIntegers
    for (int i = 0; i < s_samples; i++) {
        tempByteArray1 = GetRandomPosByteArray(s_random, s_random.Next(1, 100));
        tempByteArray2 = GetRandomPosByteArray(s_random, s_random.Next(1, 100));
        VerifyLogString(Print(tempByteArray1) + Print(tempByteArray2) + "bLog");
    }

    // Log Method - Very Large BigInteger 1 << 128 << Int.MaxValue and 2
    LargeValueLogTests(128, 1);
}

TEST(log_tests, log_large_value) {
    LargeValueLogTests(0, 4, 64, 3);
}