#include "../src/BigInteger.h"
#include "gtest/gtest.h"
#include "Random.h"
#include "stack_calc.h"
#include "utils.h"
#include "../src/exceptions.h"

static void VerifyDivideString(std::string opstring)
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
        //Run the full calculation
        sc1.DoNextOperation();
    }

    StackCalc sc2(opstring2);
    while (sc2.DoNextOperation())
    {
        //Run the full calculation
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
    return GetRandomByteArray(random, random.Next(1, 100));
}


static std::string Print(byte_array bytes)
{
    return MyBigIntImp::Print(bytes);
}

TEST(operator_tests, op_divide_RunDividePositivenonZero) {
    int s_samples = 10;
    Random s_random(100);

    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // Divide Method - Two Large BigIntegers
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = GetRandomByteArray(s_random);
        VerifyDivideString(Print(tempByteArray1) + Print(tempByteArray2) + "b/");
    }

    // Divide Method - Two Small BigIntegers
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = GetRandomByteArray(s_random, 2);
        VerifyDivideString(Print(tempByteArray1) + Print(tempByteArray2) + "b/");
    }

    // Divide Method - One large and one small BigIntegers
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = GetRandomByteArray(s_random, 2);
        VerifyDivideString(Print(tempByteArray1) + Print(tempByteArray2) + "b/");

        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = GetRandomByteArray(s_random);
        VerifyDivideString(Print(tempByteArray1) + Print(tempByteArray2) + "b/");
    }
}

TEST(operator_tests, op_divide_RunDivideNegative) {
    int s_samples = 10;
    Random s_random(100);

    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // Divide Method - One large BigIntegers and zero
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = byte_array{ 0 };
        VerifyDivideString(Print(tempByteArray1) + Print(tempByteArray2) + "b/");

        EXPECT_THROW(VerifyDivideString(Print(tempByteArray2) + Print(tempByteArray1) + "b/"), DivideByZero);
    }

    // Divide Method - One small BigIntegers and zero
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = byte_array{ 0 };
        VerifyDivideString(Print(tempByteArray1) + Print(tempByteArray2) + "b/");

        EXPECT_THROW(VerifyDivideString(Print(tempByteArray2) + Print(tempByteArray1) + "b/"), DivideByZero);
    }
}

TEST(operator_tests, op_divide_RunDivideAxiomX1) {
    int s_samples = 10;
    Random s_random(100);

    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // Axiom: X/1 = X
    auto int_max = std::to_string(std::numeric_limits<int>::max());
    auto int64_max = std::to_string(std::numeric_limits<int64_t>::max());
    VerifyIdentityString(BigInteger::One().ToString() + " " + int_max + " b/", int_max);
    VerifyIdentityString(BigInteger::One().ToString() + " " + int64_max + " b/", int64_max);

    for (int i = 0; i < s_samples; i++) {
        auto randBigInt = Print(GetRandomByteArray(s_random));
        VerifyIdentityString(BigInteger::One().ToString() + " " + randBigInt + "b/", randBigInt.substr(0, randBigInt.size() - 1));
    }
}

TEST(operator_tests, op_divide_RunBoundary) {
    int s_samples = 10;
    Random s_random(100);

    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // Check interesting cases for boundary conditions
    // You'll either be shifting a 0 or 1 across the boundary
    // 32 bit boundary  n2=0
    VerifyDivideString(std::to_string(pow(2, 32)) + " 2 b/");

    // 32 bit boundary  n1=0 n2=1
    VerifyDivideString(std::to_string(pow(2, 33)) + " 2 b/");
}

TEST(operator_tests, op_divide_RunOverflow) {

    auto x = BigInteger(byte_array{0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,0});
    auto y = BigInteger(byte_array{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0});
    auto z = BigInteger(byte_array {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0});

    ASSERT_EQ(z, x/y);
}