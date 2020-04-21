#include "../src/BigInteger.h"
#include "gtest/gtest.h"
#include "Random.h"
#include "stack_calc.h"
#include "utils.h"
#include "../src/exceptions.h"

static void VerifyRemainderString(std::string opstring)
{
    StackCalc sc(opstring);
    while (sc.DoNextOperation())
    {
        ASSERT_EQ(sc.snCalc.top().ToString(), sc.myCalc.top().ToString());
    }
}

static void VerifyIdentityString(std::string opstring1, std::string opstring2)
{
    StackCalc sc1(opstring1);
    while (sc1.DoNextOperation())
    {
        sc1.DoNextOperation();
    }

    StackCalc sc2(opstring1);
    while (sc2.DoNextOperation())
    {
        sc2.DoNextOperation();
    }

    ASSERT_EQ(sc1.snCalc.top().ToString(), sc2.snCalc.top().ToString());
}

static byte_array GetRandomByteArray(Random random, int size)
{
    return MyBigIntImp::GetRandomByteArray(random, size);
}

static byte_array GetRandomByteArray(Random random)
{
    return GetRandomByteArray(random, random.Next(0, 1024));
}

static std::string Print(byte_array bytes)
{
    return MyBigIntImp::Print(bytes);
}

TEST(operator_tests, op_modulus_RunRemainderTestsPositive) {
    int s_samples = 10;
    Random s_random(100);

    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // Remainder Method - Two Large BigIntegers
    for (int i = 0; i < s_samples; i++) {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = GetRandomByteArray(s_random);
        VerifyRemainderString(Print(tempByteArray1) + Print(tempByteArray2) + "b%");
    }

    // Remainder Method - Two Small BigIntegers
    for (int i = 0; i < s_samples; i++) {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = GetRandomByteArray(s_random, 2);
        VerifyRemainderString(Print(tempByteArray1) + Print(tempByteArray2) + "b%");
    }

    // Remainder Method - One large and one small BigIntegers
    for (int i = 0; i < s_samples; i++) {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = GetRandomByteArray(s_random, 2);
        VerifyRemainderString(Print(tempByteArray1) + Print(tempByteArray2) + "b%");

        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = GetRandomByteArray(s_random);
        VerifyRemainderString(Print(tempByteArray1) + Print(tempByteArray2) + "b%");
    }
}

TEST(operator_tests, op_modulus_RunRemainderNegative) {
    int s_samples = 10;
    Random s_random(100);

    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // Remainder Method - One large BigIntegers and zero
    for (int i = 0; i < s_samples; i++) {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = {0};
        VerifyRemainderString(Print(tempByteArray1) + Print(tempByteArray2) + "b%");

        EXPECT_THROW(VerifyRemainderString(Print(tempByteArray2) + Print(tempByteArray1) + "b%"), DivideByZero);
    }

    // Remainder Method - One small BigIntegers and zero
    for (int i = 0; i < s_samples; i++) {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = {0};
        VerifyRemainderString(Print(tempByteArray1) + Print(tempByteArray2) + "b%");

        EXPECT_THROW(VerifyRemainderString(Print(tempByteArray2) + Print(tempByteArray1) + "b%"), DivideByZero);
    }
}

TEST(operator_tests, op_modulus_RunRemainderBoundary) {
    // Check interesting cases for boundary conditions
    // You'll either be shifting a 0 or 1 across the boundary
    // 32 bit boundary  n2=0
    VerifyRemainderString(std::to_string(pow(2, 32)) + " 2 b%");

    // 32 bit boundary  n1=0 n2=1
    VerifyRemainderString(std::to_string(pow(2, 33)) + " 2 b%");
}

TEST(operator_tests, op_modulus_RunRemainderAxiomXModX) {
    int s_samples = 10;
    Random s_random(100);

    auto int_max = std::to_string(std::numeric_limits<int>::max());
    auto int64_max = std::to_string(std::numeric_limits<int64_t>::max());

    // Axiom: X%X = 0
    VerifyIdentityString(int_max + " " + int_max + " b%", BigInteger::Zero().ToString());
    VerifyIdentityString(int64_max + " " + int64_max + " b%", BigInteger::Zero().ToString());

    for (int i = 0; i < s_samples; i++) {
        std::string randBigInt = Print(GetRandomByteArray(s_random));
        VerifyIdentityString(randBigInt + randBigInt + "b%", BigInteger::Zero().ToString());
    }
}

TEST(operator_tests, op_modulus_RunRemainderAxiomXY) {
    int s_samples = 10;
    Random s_random(100);

    auto int_max = std::to_string(std::numeric_limits<int>::max());
    auto int64_max = std::to_string(std::numeric_limits<int64_t>::max());


    // Axiom: X%(X + Y) = X where Y is 1 if x>=0 and -1 if x<0
    VerifyIdentityString(BigInteger::Parse(int_max).ToString() + "1" + " " + int_max + " b%", int_max + "");
    VerifyIdentityString(BigInteger::Parse(int64_max).ToString() + "1" + " " + int64_max + " b%", int64_max + "");

    for (int i = 0; i < s_samples; i++)
    {
        byte_array test = GetRandomByteArray(s_random);
        std::string randBigInt = Print(test);
        BigInteger modify = BigInteger(1);
        if ((test[test.size() - 1] & 0x80) != 0)
        {
            modify = BigInteger::Negate(modify);
        }
        VerifyIdentityString(randBigInt + modify.ToString() + " bAdd " + randBigInt + "b%", randBigInt.substr(0, randBigInt.size() - 1));
    }

    //  SEGFAULT from here
    // Axiom: X%1 = 0
    auto one_str = BigInteger::One().ToString();
    VerifyIdentityString(BigInteger::One().ToString() + " " + int_max + " b%", BigInteger::Zero().ToString());
    VerifyIdentityString(BigInteger::One().ToString() + " " + int64_max + " b%", BigInteger::Zero().ToString());

    for (int i = 0; i < s_samples; i++)
    {
        std::string randBigInt = Print(GetRandomByteArray(s_random));
        VerifyIdentityString(BigInteger::One().ToString() + " " + randBigInt + "b%", BigInteger::Zero().ToString());
    }

    // Axiom: 0%X = 0
    VerifyIdentityString(int_max + " " + BigInteger::Zero().ToString() + " b%", BigInteger::Zero().ToString());
    VerifyIdentityString(int64_max + " " + BigInteger::Zero().ToString() + " b%", BigInteger::Zero().ToString());

    for (int i = 0; i < s_samples; i++)
    {
        std::string randBigInt = Print(GetRandomByteArray(s_random));
        VerifyIdentityString(randBigInt + BigInteger::Zero().ToString() + " b%", BigInteger::Zero().ToString());
    }
}
