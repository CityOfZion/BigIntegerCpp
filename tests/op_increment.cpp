#include "../include/public/bigintegercpp/BigInteger.h"
#include "gtest/gtest.h"
#include "Random.h"
#include "stack_calc.h"
#include "utils.h"

static void VerifyIncrementString(std::string opstring)
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


TEST(operator_tests, op_increment) {
    int s_samples = 10;
    Random s_random(100);

    byte_array tempByteArray1;

    // Increment Method - Large BigIntegers
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        VerifyIncrementString(Print(tempByteArray1) + "u++");
    }

    // Increment Method - Small BigIntegers
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        VerifyIncrementString(Print(tempByteArray1) + "u++");
    }

    // Increment Method - zero
    VerifyIncrementString("0 u++");

    // Increment Method - -1
    VerifyIncrementString("-1 u++");

    // Increment Method - 1
    VerifyIncrementString("1 u++");

    // Increment Method - Int32.MinValue
    std::string str_int32_min = std::to_string(std::numeric_limits<int32_t>::min());
    VerifyIncrementString(str_int32_min + " u++");

    // Increment Method - Int32.MinValue-1
    VerifyIncrementString(str_int32_min + " -1 b+ u++");

    // Increment Method - Int32.MinValue+1
    VerifyIncrementString(str_int32_min + " 1 b+ u++");

    // Increment Method - Int32.MaxValue
    std::string str_int32_max = std::to_string(std::numeric_limits<int32_t>::max());
    VerifyIncrementString(str_int32_max + " u++");

    // Increment Method - Int32.MaxValue-1
    VerifyIncrementString(str_int32_max + " -1 b+ u++");

    // Increment Method - Int32.MaxValue+1
    VerifyIncrementString(str_int32_max + " 1 b+ u++");

    // Increment Method - Int64.MinValue
    std::string str_int64_min = std::to_string(std::numeric_limits<int64_t>::min());
    VerifyIncrementString(str_int64_min + " u++");

    // Increment Method - Int64.MinValue-1
    VerifyIncrementString(str_int64_min + " -1 b+ u++");

    // Increment Method - Int64.MinValue+1
    VerifyIncrementString(str_int64_min + " 1 b+ u++");

    // Increment Method - Int64.MaxValue
    std::string str_int64_max = std::to_string(std::numeric_limits<int64_t>::max());
    VerifyIncrementString(str_int64_max + " u++");

    // Increment Method - Int64.MaxValue-1
    VerifyIncrementString(str_int64_max + " -1 b+ u++");

    // Increment Method - Int64.MaxValue+1
    VerifyIncrementString(str_int64_max + " 1 b+ u++");
}