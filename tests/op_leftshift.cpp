#include "../src/BigInteger.h"
#include "gtest/gtest.h"
#include <string>
#include "stack_calc.h"
#include "Random.h"
#include "MyBigInt.h"
#include "utils.h"


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
TEST(shift_tests, shift_left)
{
    int s_samples = 10;
    Random s_random(100);

    byte_array  tempByteArray1;
    byte_array  tempByteArray2;

    tempByteArray1 = byte_array{0xcc, 0x5b, 0x00, 0xf5, 0x35, 0x56, 0xa0, 0xca};
    tempByteArray2 = byte_array{0x02};
    VerifyLeftShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b<<");

    // LeftShift Method - Large BigIntegers - large + Shift
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = GetRandomPosByteArray(s_random, 2);
        VerifyLeftShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b<<");
    }

    // LeftShift Method - Large BigIntegers - small + Shift
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = byte_array{static_cast<byte>(s_random.Next(1, 32)) };
        VerifyLeftShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b<<");
    }

    // LeftShift Method - Large BigIntegers - 32 bit Shift
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = byte_array{static_cast<byte>(32) };
        VerifyLeftShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b<<");
    }
    // LeftShift Method - Large BigIntegers - large - Shift
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = GetRandomNegByteArray(s_random, 2);
        VerifyLeftShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b<<");
    }

    // LeftShift Method - Large BigIntegers - small - Shift
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = byte_array{ static_cast<byte>(s_random.Next(-31, 0)) };
        VerifyLeftShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b<<");
    }

    // LeftShift Method - Large BigIntegers - -32 bit Shift
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = byte_array{ static_cast<byte>(0xe0) };
        VerifyLeftShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b<<");
    }

    // LeftShift Method - Large BigIntegers - 0 bit Shift
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = byte_array{ static_cast<byte>(0x0) };
        VerifyLeftShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b<<");
    }

    // LeftShift Method - Small BigIntegers - large + Shift
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = GetRandomPosByteArray(s_random, 2);
        VerifyLeftShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b<<");
    }

    // LeftShift Method - Small BigIntegers - small + Shift
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = byte_array { static_cast<byte>(s_random.Next(1, 32)) };
        VerifyLeftShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b<<");
    }

    // LeftShift Method - Small BigIntegers - 32 bit Shift
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = byte_array { static_cast<byte>(32) };
        VerifyLeftShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b<<");
    }
    // LeftShift Method - Small BigIntegers - large - Shift
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = GetRandomNegByteArray(s_random, 2);
        VerifyLeftShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b<<");
    }

    // LeftShift Method - Small BigIntegers - small - Shift
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = byte_array{static_cast<byte>(s_random.Next(-31, 0)) };
        VerifyLeftShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b<<");
    }

    // LeftShift Method - Small BigIntegers - -32 bit Shift
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = byte_array{ static_cast<byte>(0xe0) };
        VerifyLeftShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b<<");
    }

    // LeftShift Method - Small BigIntegers - 0 bit Shift
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = byte_array{ static_cast<byte>(0) };
        VerifyLeftShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b<<");
    }

    // LeftShift Method - Positive BigIntegers - Shift to 0
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomPosByteArray(s_random, 100);
        auto tmp_int = s_random.Next(-1000, -8 * tempByteArray1.size());
        tempByteArray2 = to_byte_array<int>(tmp_int);
        VerifyLeftShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b<<");
    }

    // LeftShift Method - Negative BigIntegers - Shift to -1
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomNegByteArray(s_random, 100);
//        tempByteArray2 = MyBigIntImp::GetBytes(s_random.Next(-1000, -8 * tempByteArray1.size()));
        auto tmp_int = s_random.Next(-1000, -8 * tempByteArray1.size());
        tempByteArray2 = to_byte_array<int>(tmp_int);
        VerifyLeftShiftString(Print(tempByteArray2) + Print(tempByteArray1) + "b<<");
    }

}