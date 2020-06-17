#include "../include/public/BigInteger.h"
#include "gtest/gtest.h"
#include <string>
#include "stack_calc.h"
#include "Random.h"
#include "MyBigInt.h"
#include "utils.h"


static void VerifyPowString(std::string opstring)
{
    StackCalc sc(opstring);
    while (sc.DoNextOperation())
    {
        ASSERT_EQ(sc.snCalc.top().to_string(), sc.myCalc.top().to_string());
    }
}

static void VerifyIdentityString(std::string opstring1, std::string opstring2)
{
    StackCalc sc1(opstring1);
    while (sc1.DoNextOperation())
    {
        //Run the full calculation
        sc1.DoNextOperation();
    }

    StackCalc sc2(opstring2);
    while (sc2.DoNextOperation())
    {
        //Run the full calculation
        sc2.DoNextOperation();
    }
    ASSERT_EQ(sc1.snCalc.top().to_string(), sc2.snCalc.top().to_string());
}

static byte_array GetRandomByteArray(Random random, int size)
{
    return MyBigIntImp::GetRandomByteArray(random, size);
}

static byte_array GetRandomByteArray(Random random)
{
    return GetRandomByteArray(random, random.Next(0, 10));
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

static std::string Print(byte_array bytes)
{
    return MyBigIntImp::Print(bytes);
}

TEST(pow_tests, debug) {
    auto tempByteArray1 = byte_array{8};
    auto tempByteArray2 = byte_array{8 , 24, 67, 103, 87, 79, 48, 10};
    VerifyPowString(Print(tempByteArray1) + Print(tempByteArray2) + "bPow");
}

TEST(pow_tests, RunPowPositive) {
    int s_samples = 10;
    Random s_random(100);

    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // Pow Method - 0^(1)
    VerifyPowString(BigInteger::one().to_string() + " " + BigInteger::zero().to_string() + " bPow");

    // Pow Method - 0^(0)
    VerifyPowString(BigInteger::zero().to_string() + " " + BigInteger::zero().to_string() + " bPow");

    // Pow Method - Two Small BigIntegers
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomPosByteArray(s_random, 1);
        tempByteArray2 = GetRandomByteArray(s_random, 2);
        VerifyPowString(Print(tempByteArray1) + Print(tempByteArray2) + "bPow");
    }

    // Pow Method - one large and one small BigIntegers
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomPosByteArray(s_random, 1);
        tempByteArray2 = GetRandomByteArray(s_random);
        VerifyPowString(Print(tempByteArray1) + Print(tempByteArray2) + "bPow");
    }

    // Pow Method - one large BigIntegers and zero
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = byte_array{ 0 };
        VerifyPowString(Print(tempByteArray2) + Print(tempByteArray1) + "bPow");
    }

    // Pow Method - one small BigIntegers and zero
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomPosByteArray(s_random, 2);
        tempByteArray2 = byte_array{ 0 };
        VerifyPowString(Print(tempByteArray1) + Print(tempByteArray2) + "bPow");
        VerifyPowString(Print(tempByteArray2) + Print(tempByteArray1) + "bPow");
    }
}

TEST(pow_tests, RunPowAxiomXPow1) {
    int s_samples = 10;
    Random s_random(100);

    // Axiom: X^1 = X
    auto str_int_max = std::to_string(std::numeric_limits<int>::max());
    auto str_long_max = std::to_string(std::numeric_limits<long>::max());
    VerifyIdentityString(BigInteger::one().to_string() + " " + str_int_max + " bPow", str_int_max);
    VerifyIdentityString(BigInteger::one().to_string() + " " + str_long_max + " bPow", str_long_max);

    for (int i = 0; i < s_samples; i++)
    {
        auto randBigInt = Print(GetRandomByteArray(s_random));
        VerifyIdentityString(BigInteger::one().to_string() + " " + randBigInt + "bPow", randBigInt.substr(0, randBigInt.size() - 1));
    }
}

TEST(pow_tests, RunPowAxiomXPow0) {
    int s_samples = 10;
    Random s_random(100);

    // Axiom: X^0 = 1
    auto str_int_max = std::to_string(std::numeric_limits<int>::max());
    auto str_long_max = std::to_string(std::numeric_limits<long>::max());

    VerifyIdentityString(BigInteger::zero().to_string() + " " + str_int_max + " bPow", BigInteger::one().to_string());
    VerifyIdentityString(BigInteger::zero().to_string() + " " + str_long_max + " bPow", BigInteger::one().to_string());

    for (int i = 0; i < s_samples; i++)
    {
        auto randBigInt = Print(GetRandomByteArray(s_random));
        VerifyIdentityString(BigInteger::zero().to_string() + " " + randBigInt + "bPow", BigInteger::one().to_string());
    }
}


TEST(pow_tests, RunPowAxiom0PowX) {
    int s_samples = 10;
    Random s_random(100);

    // Axiom: 0^X = 0
    auto str_int_max = std::to_string(std::numeric_limits<int>::max());
    VerifyIdentityString(str_int_max + " " + BigInteger::zero().to_string() + " bPow", BigInteger::zero().to_string());

    for (int i = 0; i < s_samples; i++)
    {
        auto randBigInt = Print(GetRandomPosByteArray(s_random, 4));
        VerifyIdentityString(randBigInt + BigInteger::zero().to_string() + " bPow", BigInteger::zero().to_string());
    }
}

TEST(pow_tests, RunPowAxiom1PowX) {
    int s_samples = 10;
    Random s_random(100);

    // Axiom: 1^X = 1
    auto str_int_max = std::to_string(std::numeric_limits<int>::max());
    VerifyIdentityString(str_int_max + " " + BigInteger::one().to_string() + " bPow", BigInteger::one().to_string());

    for (int i = 0; i < s_samples; i++)
    {
        auto randBigInt = Print(GetRandomPosByteArray(s_random, 4));
        VerifyIdentityString(randBigInt + BigInteger::one().to_string() + " bPow", BigInteger::one().to_string());
    }
}

TEST(pow_tests, RunPowBoundary) {
    // Check interesting cases for boundary conditions
    // You'll either be shifting a 0 or 1 across the boundary
    // 32 bit boundary  n2=0
    VerifyPowString("2 " + std::to_string(pow(2, 32)) + " bPow");

    // 32 bit boundary  n1=0 n2=1
    VerifyPowString("2 " + std::to_string(pow(2, 33)) + " bPow");
}

TEST(pow_tests, RunPowNegative) {
    int s_samples = 10;
    Random s_random(100);

    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // Pow Method - 1^(-1)
    EXPECT_THROW_WITH_MESSAGE(VerifyPowString(
            BigInteger::minus_one().to_string() + " " + BigInteger::one().to_string() + " bPow"), std::out_of_range, "SR.ArgumentOutOfRange_MustBeNonNeg");

    // Pow Method - 0^(-1)
    EXPECT_THROW_WITH_MESSAGE(VerifyPowString(
            BigInteger::minus_one().to_string() + " " + BigInteger::zero().to_string() + " bPow"), std::out_of_range, "SR.ArgumentOutOfRange_MustBeNonNeg");

    // Pow Method - Negative Exponent
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomNegByteArray(s_random, 2);
        tempByteArray2 = GetRandomByteArray(s_random, 2);
        EXPECT_THROW_WITH_MESSAGE(VerifyPowString(Print(tempByteArray1) + Print(tempByteArray2) + "bPow"), std::out_of_range, "SR.ArgumentOutOfRange_MustBeNonNeg");
    }
}
TEST(pow_tests, RunOverflow) {
    byte_array bytes(1000, 0);
    bytes[bytes.size() - 1] = 1;

    EXPECT_THROW_WITH_MESSAGE(BigInteger::pow(BigInteger(bytes), std::numeric_limits<int>::max()), std::overflow_error, "checked() int overflow");
}