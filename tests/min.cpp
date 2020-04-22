#include "../src/BigInteger.h"
#include "gtest/gtest.h"
#include "Random.h"
#include "stack_calc.h"

static void VerifyMinString(std::string opstring)
{
    StackCalc sc(opstring);
    while (sc.DoNextOperation())
    {
        ASSERT_EQ(sc.snCalc.top().ToString(), sc.myCalc.top().ToString());
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


TEST(min_tests, RunMinTests) {
    int s_samples = 10;
    Random s_random(100);

    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // Min Method - Two Large BigIntegers
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = GetRandomByteArray(s_random);
        VerifyMinString(Print(tempByteArray1) + Print(tempByteArray2) + "bMin");
    }

    // Min Method - Two Small BigIntegers
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = GetRandomByteArray(s_random, 2);
        VerifyMinString(Print(tempByteArray1) + Print(tempByteArray2) + "bMin");
    }

    // Min Method - One large and one small BigIntegers
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = GetRandomByteArray(s_random, 2);
        VerifyMinString(Print(tempByteArray1) + Print(tempByteArray2) + "bMin");

        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = GetRandomByteArray(s_random);
        VerifyMinString(Print(tempByteArray1) + Print(tempByteArray2) + "bMin");
    }

    // Min Method - One large BigIntegers and zero
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = byte_array{ 0 };
        VerifyMinString(Print(tempByteArray1) + Print(tempByteArray2) + "bMin");

        tempByteArray1 = byte_array{ 0 };
        tempByteArray2 = GetRandomByteArray(s_random);
        VerifyMinString(Print(tempByteArray1) + Print(tempByteArray2) + "bMin");
    }

    // Min Method - One small BigIntegers and zero
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = byte_array{ 0 };
        VerifyMinString(Print(tempByteArray1) + Print(tempByteArray2) + "bMin");

        tempByteArray1 = byte_array{ 0 };
        tempByteArray2 = GetRandomByteArray(s_random, 2);
        VerifyMinString(Print(tempByteArray1) + Print(tempByteArray2) + "bMin");
    }


    // Min Method - One large BigIntegers and -1
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = byte_array{ 0xFF };
        VerifyMinString(Print(tempByteArray1) + Print(tempByteArray2) + "bMin");

        tempByteArray1 = byte_array{ 0xFF };
        tempByteArray2 = GetRandomByteArray(s_random);
        VerifyMinString(Print(tempByteArray1) + Print(tempByteArray2) + "bMin");
    }

    // Min Method - One small BigIntegers and -1
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = byte_array{ 0xFF };
        VerifyMinString(Print(tempByteArray1) + Print(tempByteArray2) + "bMin");

        tempByteArray1 = byte_array{ 0xFF };
        tempByteArray2 = GetRandomByteArray(s_random, 2);
        VerifyMinString(Print(tempByteArray1) + Print(tempByteArray2) + "bMin");
    }


    // Min Method - One large BigIntegers and 1
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = byte_array{ 1 };
        VerifyMinString(Print(tempByteArray1) + Print(tempByteArray2) + "bMin");

        tempByteArray1 = byte_array{ 1 };
        tempByteArray2 = GetRandomByteArray(s_random);
        VerifyMinString(Print(tempByteArray1) + Print(tempByteArray2) + "bMin");
    }

    // Min Method - One small BigIntegers and 1
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = byte_array{ 1 };
        VerifyMinString(Print(tempByteArray1) + Print(tempByteArray2) + "bMin");

        tempByteArray1 = byte_array{ 1 };
        tempByteArray2 = GetRandomByteArray(s_random, 2);
        VerifyMinString(Print(tempByteArray1) + Print(tempByteArray2) + "bMin");
    }
}