#include "../src/BigInteger.h"
#include "gtest/gtest.h"
#include "Random.h"
#include "stack_calc.h"

static void VerifySignString(std::string opstring)
{
    StackCalc sc(opstring);
    while (sc.DoNextOperation())
    {
        ASSERT_EQ(sc.snCalc.top().to_string(), sc.myCalc.top().to_string());
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
    ASSERT_EQ(sc1.snCalc.top().to_string(), sc2.snCalc.top().to_string());
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

TEST(sign_tests, RunSignTests) {
    int s_samples = 10;
    Random s_random(100);

    byte_array tempByteArray1;

    // Sign Method - Large BigIntegers
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        VerifySignString(Print(tempByteArray1) + "uSign");
    }

    // Sign Method - Small BigIntegers
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        VerifySignString(Print(tempByteArray1) + "uSign");
    }

    // Sign Method - zero
    VerifySignString("0 uSign");

    // Sign Method - -1
    VerifySignString("-1 uSign");

    // Sign Method - 1
    VerifySignString("1 uSign");

    // Sign Method - Int32.MinValue
    auto int32_min = std::to_string(std::numeric_limits<int32_t>::min());
    VerifySignString(int32_min + " uSign");

    // Sign Method - Int32.MinValue-1
    VerifySignString(int32_min + " -1 b+ uSign");

    // Sign Method - Int32.MinValue+1
    VerifySignString(int32_min + " 1 b+ uSign");

    // Sign Method - Int32.MaxValue
    auto int32_max = std::to_string(std::numeric_limits<int32_t>::max());
    VerifySignString(int32_max + " uSign");

    // Sign Method - Int32.MaxValue-1
    VerifySignString(int32_max + " -1 b+ uSign");

    // Sign Method - Int32.MaxValue+1
    VerifySignString(int32_max + " 1 b+ uSign");

    // Sign Method - Int64.MinValue
    auto int64_min = std::to_string(std::numeric_limits<int64_t>::min());
    VerifySignString(int64_min + " uSign");

    // Sign Method - Int64.MinValue-1
    VerifySignString(int64_min + " -1 b+ uSign");

    // Sign Method - Int64.MinValue+1
    VerifySignString(int64_min + " 1 b+ uSign");

    // Sign Method - Int64.MaxValue
    auto int64_max = std::to_string(std::numeric_limits<int64_t>::max());
    VerifySignString(int64_max + " uSign");

    // Sign Method - Int64.MaxValue-1
    VerifySignString(int64_max + " -1 b+ uSign");

    // Sign Method - Int64.MaxValue+1
    VerifySignString(int64_max + " 1 b+ uSign");
}