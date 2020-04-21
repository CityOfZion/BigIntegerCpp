#include "../src/BigInteger.h"
#include "gtest/gtest.h"
#include "Random.h"
#include "stack_calc.h"
#include "utils.h"
#include "../src/exceptions.h"

static void VerifyMultiplyString(const std::string& opstring)
{
    StackCalc sc(opstring);
    while (sc.DoNextOperation())
    {
        ASSERT_EQ(sc.snCalc.top().ToString(), sc.myCalc.top().ToString());
    }
}

static void VerifyIdentityString(const std::string& opstring1, const std::string& opstring2)
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
    return GetRandomByteArray(random, random.Next(0, 1024));
}

static std::string Print(byte_array bytes)
{
    return MyBigIntImp::Print(bytes);
}

TEST(operator_tests, op_multiply_RunMultiplyPositive) {
    int s_samples = 10;
    Random s_random(100);

    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // Multiply Method - One Large BigInteger
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        VerifyMultiplyString(Print(tempByteArray1) + "u*");
    }

    // Multiply Method - Two Large BigIntegers
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = GetRandomByteArray(s_random);
        VerifyMultiplyString(Print(tempByteArray1) + Print(tempByteArray2) + "b*");
    }

    // Multiply Method - Two Small BigIntegers
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = GetRandomByteArray(s_random, 2);
        VerifyMultiplyString(Print(tempByteArray1) + Print(tempByteArray2) + "b*");
    }

    // Multiply Method - One large and one small BigIntegers
    for (int i = 0; i < s_samples; i++)
    {
        try
        {
            tempByteArray1 = GetRandomByteArray(s_random);
            tempByteArray2 = GetRandomByteArray(s_random, 2);
            VerifyMultiplyString(Print(tempByteArray1) + Print(tempByteArray2) + "b*");

            tempByteArray1 = GetRandomByteArray(s_random, 2);
            tempByteArray2 = GetRandomByteArray(s_random);
            VerifyMultiplyString(Print(tempByteArray1) + Print(tempByteArray2) + "b*");
        }
        catch (...)
        {
            std::cerr << "Oops, should this happen ?" << std::endl;
            std::cerr << "Array1: " + Print(tempByteArray1) << std::endl;
            std::cerr << "Array2: " + Print(tempByteArray2) << std::endl;
        }
    }
}

TEST(operator_tests, op_multiply_RunMultiplyPositiveWith0) {
    int s_samples = 10;
    Random s_random(100);

    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // Multiply Method - One large BigIntegers and zero
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = byte_array{ 0 };
        VerifyMultiplyString(Print(tempByteArray1) + Print(tempByteArray2) + "b*");

        tempByteArray1 = byte_array{ 0 };
        tempByteArray2 = GetRandomByteArray(s_random);
        VerifyMultiplyString(Print(tempByteArray1) + Print(tempByteArray2) + "b*");
    }

    // Multiply Method - One small BigIntegers and zero
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = byte_array{ 0 };
        VerifyMultiplyString(Print(tempByteArray1) + Print(tempByteArray2) + "b*");

        tempByteArray1 = byte_array{ 0 };
        tempByteArray2 = GetRandomByteArray(s_random, 2);
        VerifyMultiplyString(Print(tempByteArray1) + Print(tempByteArray2) + "b*");
    }
}

TEST(operator_tests, op_multiply_RunMultiplyAxiomXmult1) {
    int s_samples = 10;
    Random s_random(100);

    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // Axiom: X*1 = X
    auto int32_max = std::to_string(std::numeric_limits<int32_t>::max());
    auto int64_max = std::to_string(std::numeric_limits<int64_t>::max());
    VerifyIdentityString(int32_max + " " + BigInteger::One().ToString() + " b*", int32_max);
    VerifyIdentityString(int64_max + " " + BigInteger::One().ToString() + " b*", int64_max);

    for (int i = 0; i < s_samples; i++)
    {
        auto randBigInt = Print(GetRandomByteArray(s_random));
        VerifyIdentityString(randBigInt + BigInteger::One().ToString() + " b*", randBigInt + "u+");
    }
}

TEST(operator_tests, op_multiply_RunMultiplyAxiomXmult0) {
    int s_samples = 10;
    Random s_random(100);

    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // Axiom: X*0 = 0
    auto int_max = std::to_string(std::numeric_limits<int>::max());
    auto long_max = std::to_string(std::numeric_limits<long>::max());
    VerifyIdentityString(int_max + " " + BigInteger::Zero().ToString() + " b*", BigInteger::Zero().ToString());
    VerifyIdentityString(long_max + " " + BigInteger::Zero().ToString() + " b*", BigInteger::Zero().ToString());

    for (int i = 0; i < s_samples; i++)
    {
        auto randBigInt = Print(GetRandomByteArray(s_random));
        VerifyIdentityString(randBigInt + BigInteger::Zero().ToString() + " b*", BigInteger::Zero().ToString());
    }
}

TEST(operator_tests, op_multiply_RunMultiplyAxiomComm) {
    // Check interesting cases for boundary conditions
    // You'll either be shifting a 0 or 1 across the boundary
    // 32 bit boundary  n2=0
    VerifyMultiplyString(std::to_string(pow(2, 32)) + " 2 b*");

    // 32 bit boundary  n1=0 n2=1
    VerifyMultiplyString(std::to_string(pow(2, 33)) + " 2 b*");
}

TEST(operator_tests, op_multiply_RunMultiplyBoundary) {
    // Check interesting cases for boundary conditions
    // You'll either be shifting a 0 or 1 across the boundary
    // 32 bit boundary  n2=0
    VerifyMultiplyString(std::to_string(pow(2, 32)) + " 2 b*");

    // 32 bit boundary  n1=0 n2=1
    VerifyMultiplyString(std::to_string(pow(2, 33)) + " 2 b*");
}
