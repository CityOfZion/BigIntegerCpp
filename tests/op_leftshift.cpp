#include <iostream>
#include "../src/BigInteger.h"
#include "gtest/gtest.h"
#include <string>
#include "stack_calc.h"
#include "Random.h"
#include "MyBigInt.h"


static void VerifyLeftShiftString(std::string opstring)
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


// Tests for simple arithmetic
TEST(shift_tests, simple_add)
{
    int s_samples = 10;
    Random s_random(100);

    byte_array  tempByteArray1;
    byte_array  tempByteArray2;

    for (int i = 0; i < s_samples; i++) {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = GetRandomPosByteArray(s_random, 2);
        VerifyLeftShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b<<");
    }

    // TODO: implement
    // MyBigIntImp::DoUnaryOperatorMine
    // MyBigIntImp::DoTertanaryOperatorMine
    // then finish this test case
}