#include "../src/BigInteger.h"
#include "gtest/gtest.h"
#include "Random.h"
#include "stack_calc.h"
#include "utils.h"

static void VerifyDecrementString(std::string opstring)
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


TEST(operator_tests, op_decrement) {
    int s_samples = 10;
    Random s_random(100);

    byte_array tempByteArray1;

    // Decrement Method - Large BigIntegers
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        VerifyDecrementString(Print(tempByteArray1) + "u--");
    }

    // Decrement Method - Small BigIntegers
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        VerifyDecrementString(Print(tempByteArray1) + "u--");
    }

    // Decrement Method - zero
    VerifyDecrementString("0 u--");

    // Decrement Method - -1
    VerifyDecrementString("-1 u--");

    // Decrement Method - 1
    VerifyDecrementString("1 u--");

    // Decrement Method - Int32.MinValue
    std::string str_int32_min = std::to_string(std::numeric_limits<int32_t>::min());
    VerifyDecrementString(str_int32_min + " u--");

    // Decrement Method - Int32.MinValue-1
    VerifyDecrementString(str_int32_min + " -1 b+ u--");

    // Decrement Method - Int32.MinValue+1
    VerifyDecrementString(str_int32_min + " 1 b+ u--");

    // Decrement Method - Int32.MaxValue
    std::string str_int32_max = std::to_string(std::numeric_limits<int32_t>::max());
    VerifyDecrementString(str_int32_max + " u--");

    // Decrement Method - Int32.MaxValue-1
    VerifyDecrementString(str_int32_max + " -1 b+ u--");

    // Decrement Method - Int32.MaxValue+1
    VerifyDecrementString(str_int32_max + " 1 b+ u--");

    // Decrement Method - Int64.MinValue
    std::string str_int64_min = std::to_string(std::numeric_limits<int64_t>::min());
    VerifyDecrementString(str_int64_min + " u--");

    // Decrement Method - Int64.MinValue-1
    VerifyDecrementString(str_int64_min + " -1 b+ u--");

    // Decrement Method - Int64.MinValue+1
    VerifyDecrementString(str_int64_min + " 1 b+ u--");

    // Decrement Method - Int64.MaxValue
    std::string str_int64_max = std::to_string(std::numeric_limits<int64_t>::max());
    VerifyDecrementString(str_int64_max + " u--");

    // Decrement Method - Int64.MaxValue-1
    VerifyDecrementString(str_int64_max + " -1 b+ u--");

    // Decrement Method - Int64.MaxValue+1
    VerifyDecrementString(str_int64_max + " 1 b+ u--");
}