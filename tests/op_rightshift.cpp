#include "../src/BigInteger.h"
#include "gtest/gtest.h"
#include "Random.h"
#include "stack_calc.h"
#include "utils.h"

static void VerifyRightShiftString(std::string opstring)
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

TEST(operator_tests, shift_right) {
    int s_samples = 10;
    Random s_random(100);

    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // RightShift Method - Large BigIntegers - large + Shift
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = GetRandomPosByteArray(s_random, 2);
        VerifyRightShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b>>");
    }

    // RightShift Method - Large BigIntegers - small + Shift
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = byte_array{ static_cast<byte>(s_random.Next(1, 32)) };
        VerifyRightShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b>>");
    }

    // RightShift Method - Large BigIntegers - 32 bit Shift
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = byte_array{ static_cast<byte>(32) };
        VerifyRightShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b>>");
    }
    // RightShift Method - Large BigIntegers - large - Shift
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = GetRandomNegByteArray(s_random, 2);
        VerifyRightShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b>>");
    }

    // RightShift Method - Large BigIntegers - small - Shift
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = byte_array {static_cast<byte>(s_random.Next(-31, 0)) };
        VerifyRightShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b>>");
    }

    // RightShift Method - Large BigIntegers - -32 bit Shift
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = byte_array{ static_cast<byte>(0xe0) };
        VerifyRightShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b>>");
    }

    // RightShift Method - Large BigIntegers - 0 bit Shift
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = byte_array{ static_cast<byte>(0) };
        VerifyRightShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b>>");
    }

    // RightShift Method - Small BigIntegers - large + Shift
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = GetRandomPosByteArray(s_random, 2);
        VerifyRightShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b>>");
    }

    // RightShift Method - Small BigIntegers - small + Shift
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = byte_array{ static_cast<byte>(s_random.Next(1, 32)) };
        VerifyRightShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b>>");
    }

    // RightShift Method - Small BigIntegers - 32 bit Shift
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = byte_array{static_cast<byte>(32) };
        VerifyRightShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b>>");
    }
    // RightShift Method - Small BigIntegers - large - Shift
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = GetRandomNegByteArray(s_random, 2);
        VerifyRightShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b>>");
    }

    // RightShift Method - Small BigIntegers - small - Shift
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = byte_array{ static_cast<byte>(s_random.Next(-31, 0)) };
        VerifyRightShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b>>");
    }

    // RightShift Method - Small BigIntegers - -32 bit Shift
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = byte_array{ static_cast<byte>(0xe0) };
        VerifyRightShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b>>");
    }

    // RightShift Method - Small BigIntegers - 0 bit Shift
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = byte_array{ static_cast<byte>(0) };
        VerifyRightShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b>>");
    }

    // RightShift Method - Positive BigIntegers - Shift to 0
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomPosByteArray(s_random, 100);
//        tempByteArray2 = BitConverter.GetBytes(s_random.Next(8 * tempByteArray1.Length, 1000));
        int tmp_int = s_random.Next(8 * tempByteArray1.size(), 1000);
        tempByteArray2 = to_byte_array<int>(tmp_int);
        VerifyRightShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b>>");
    }

    // RightShift Method - Negative BigIntegers - Shift to -1
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomNegByteArray(s_random, 100);
//        tempByteArray2 = BitConverter.GetBytes(s_random.Next(8 * tempByteArray1.size(), 1000));
        int tmp_int = s_random.Next(8 * tempByteArray1.size(), 1000);
        tempByteArray2 = to_byte_array<int>(tmp_int);
        VerifyRightShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b>>");
    }
}