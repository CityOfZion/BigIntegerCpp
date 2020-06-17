#include "../include/public/BigInteger.h"
#include "gtest/gtest.h"
#include "Random.h"
#include "stack_calc.h"
#include "utils.h"

static void VerifyNotString(std::string opstring)
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


TEST(operator_tests, op_not) {
    int s_samples = 10;
    Random s_random(100);

    byte_array tempByteArray1;

    // Not Method - Large BigIntegers
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        VerifyNotString(Print(tempByteArray1) + "u~");
    }

    // Not Method - Small BigIntegers
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        VerifyNotString(Print(tempByteArray1) + "u~");
    }

    // Not Method - zero
    VerifyNotString("0 u~");

    // Not Method - -1
    VerifyNotString("-1 u~");

    // Not Method - 1
    VerifyNotString("1 u~");

    // Not Method - Int32.MinValue
    std::string str_int32_min = std::to_string(std::numeric_limits<int32_t>::min());
    VerifyNotString(str_int32_min + " u~");

    // Not Method - Int32.MinValue-1
    VerifyNotString(str_int32_min + " -1 b+ u~");

    // Not Method - Int32.MinValue+1
    VerifyNotString(str_int32_min + " 1 b+ u~");

    // Not Method - Int32.MaxValue
    std::string str_int32_max = std::to_string(std::numeric_limits<int32_t>::max());
    VerifyNotString(str_int32_max + " u~");

    // Not Method - Int32.MaxValue-1
    VerifyNotString(str_int32_max + " -1 b+ u~");

    // Not Method - Int32.MaxValue+1
    VerifyNotString(str_int32_max + " 1 b+ u~");

    // Not Method - Int64.MinValue
    std::string str_int64_min = std::to_string(std::numeric_limits<int64_t>::min());
    VerifyNotString(str_int64_min + " u~");

    // Not Method - Int64.MinValue-1
    VerifyNotString(str_int64_min + " -1 b+ u~");

    // Not Method - Int64.MinValue+1
    VerifyNotString(str_int64_min + " 1 b+ u~");

    // Not Method - Int64.MaxValue
    std::string str_int64_max = std::to_string(std::numeric_limits<int64_t>::max());
    VerifyNotString(str_int64_max + " u~");

    // Not Method - Int64.MaxValue-1
    VerifyNotString(str_int64_max + " -1 b+ u~");

    // Not Method - Int64.MaxValue+1
    VerifyNotString(str_int64_max + " 1 b+ u~");
}