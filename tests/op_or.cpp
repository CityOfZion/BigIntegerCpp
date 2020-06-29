#include "../include/public/bigintegercpp/BigInteger.h"
#include "gtest/gtest.h"
#include "Random.h"
#include "stack_calc.h"
#include "utils.h"

static void VerifyOrString(std::string opstring)
{
    StackCalc sc(opstring);
    while (sc.DoNextOperation())
    {
        ASSERT_EQ(sc.snCalc.top().to_string(), sc.myCalc.top().to_string());
    }
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

TEST(operator_tests, op_or) {
    int s_samples = 10;
    Random s_random(100);

    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // Or Method - Two Large BigIntegers
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = GetRandomByteArray(s_random);
        VerifyOrString(Print(tempByteArray1) + Print(tempByteArray2) + "b|");
    }

    // Or Method - Two Small BigIntegers
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = GetRandomByteArray(s_random, 2);
        VerifyOrString(Print(tempByteArray1) + Print(tempByteArray2) + "b|");
    }

    // Or Method - one large and one small BigIntegers
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = GetRandomByteArray(s_random, 2);
        VerifyOrString(Print(tempByteArray1) + Print(tempByteArray2) + "b|");

        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = GetRandomByteArray(s_random);
        VerifyOrString(Print(tempByteArray1) + Print(tempByteArray2) + "b|");
    }

    // Or Method - one large BigIntegers and zero
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = byte_array{ 0 };
        VerifyOrString(Print(tempByteArray1) + Print(tempByteArray2) + "b|");

        tempByteArray1 = byte_array{ 0 };
        tempByteArray2 = GetRandomByteArray(s_random);
        VerifyOrString(Print(tempByteArray1) + Print(tempByteArray2) + "b|");
    }

    // Or Method - one small BigIntegers and zero
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = byte_array{ 0 };
        VerifyOrString(Print(tempByteArray1) + Print(tempByteArray2) + "b|");

        tempByteArray1 = byte_array{ 0 };
        tempByteArray2 = GetRandomByteArray(s_random, 2);
        VerifyOrString(Print(tempByteArray1) + Print(tempByteArray2) + "b|");
    }

    // Or Method - one large BigIntegers and -1
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = byte_array{ 0xFF };
        VerifyOrString(Print(tempByteArray1) + Print(tempByteArray2) + "b|");

        tempByteArray1 = byte_array{ 0xFF };
        tempByteArray2 = GetRandomByteArray(s_random);
        VerifyOrString(Print(tempByteArray1) + Print(tempByteArray2) + "b|");
    }

    // Or Method - one small BigIntegers and -1
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = byte_array{ 0xFF };
        VerifyOrString(Print(tempByteArray1) + Print(tempByteArray2) + "b|");

        tempByteArray1 = byte_array{ 0xFF };
        tempByteArray2 = GetRandomByteArray(s_random, 2);
        VerifyOrString(Print(tempByteArray1) + Print(tempByteArray2) + "b|");
    }

    // Or Method - one large BigIntegers and Int.MaxValue+1
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = byte_array{ 0x00, 0x00, 0x00, 0x80, 0x00 };
        VerifyOrString(Print(tempByteArray1) + Print(tempByteArray2) + "b|");

        tempByteArray1 = byte_array{ 0x00, 0x00, 0x00, 0x80, 0x00 };
        tempByteArray2 = GetRandomByteArray(s_random);
        VerifyOrString(Print(tempByteArray1) + Print(tempByteArray2) + "b|");
    }

    // Or Method - one small BigIntegers and Int.MaxValue+1
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = byte_array{ 0x00, 0x00, 0x00, 0x80, 0x00 };
        VerifyOrString(Print(tempByteArray1) + Print(tempByteArray2) + "b|");

        tempByteArray1 = byte_array{ 0x00, 0x00, 0x00, 0x80, 0x00 };
        tempByteArray2 = GetRandomByteArray(s_random, 2);
        VerifyOrString(Print(tempByteArray1) + Print(tempByteArray2) + "b|");
    }
}