#include "../include/public/BigInteger.h"
#include "gtest/gtest.h"
#include <string>
#include "stack_calc.h"
#include "Random.h"

static void VerifyLogString(std::string opstring) {
    StackCalc sc(opstring);
    while (sc.DoNextOperation()) {
        ASSERT_EQ(sc.snCalc.top().to_string(), sc.myCalc.top().to_string());
    }
}

static byte_array GetRandomByteArray(Random random, int size) {
    return MyBigIntImp::GetRandomByteArray(random, size);
}

static byte_array GetRandomByteArray(Random random) {
    return GetRandomByteArray(random, random.Next(0, 1024));
}

static std::string Print(byte_array bytes) {
    return MyBigIntImp::Print(bytes);
}

TEST(log_tests, log02) {
    int s_samples = 10;
    Random s_random(100);
    byte_array tempByteArray1;


    // Log Method - Large BigIntegers
    for (int i = 0; i < s_samples; i++) {
        tempByteArray1 = GetRandomByteArray(s_random);
        VerifyLogString(Print(tempByteArray1) + "uLog");
    }

    // Log Method - Small BigIntegers
    for (int i = 0; i < s_samples; i++) {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        VerifyLogString(Print(tempByteArray1) + "uLog");
    }

    // Log Method - zero
    VerifyLogString("0 uLog");

    // Log Method - -1
    VerifyLogString("-1 uLog");

    // Log Method - 1
    VerifyLogString("1 uLog");

    // Log Method - Int32.MinValue
    auto int32_min = std::to_string(std::numeric_limits<int32_t>::min());
    auto int32_max = std::to_string(std::numeric_limits<int32_t>::max());
    VerifyLogString(int32_min + " uLog");

    // Log Method - Int32.MinValue-1
    VerifyLogString(int32_min + " -1 b+ uLog");

    // Log Method - Int32.MinValue+1
    VerifyLogString(int32_min + " 1 b+ uLog");

    // Log Method - Int32.MaxValue
    VerifyLogString(int32_max + " uLog");

    // Log Method - Int32.MaxValue-1
    VerifyLogString(int32_max + " -1 b+ uLog");

    // Log Method - Int32.MaxValue+1
    VerifyLogString(int32_max + " 1 b+ uLog");

    // Log Method - Int64.MinValue
    auto int64_min = std::to_string(std::numeric_limits<int64_t>::min());
    auto int64_max = std::to_string(std::numeric_limits<int64_t>::max());

    VerifyLogString(int64_min + " uLog");

    // Log Method - Int64.MinValue-1
    VerifyLogString(int64_min + " -1 b+ uLog");

    // Log Method - Int64.MinValue+1
    VerifyLogString(int64_min + " 1 b+ uLog");

    // Log Method - Int64.MaxValue
    VerifyLogString(int64_max + " uLog");

    // Log Method - Int64.MaxValue-1
    VerifyLogString(int64_max + " -1 b+ uLog");

    // Log Method - Int64.MaxValue+1
    VerifyLogString(int64_max + " 1 b+ uLog");
}