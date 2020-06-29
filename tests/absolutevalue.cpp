#include "../include/public/bigintegercpp/BigInteger.h"
#include "gtest/gtest.h"
#include "Random.h"
#include "stack_calc.h"
#include "utils.h"

static void VerifyAbsoluteValueString(std::string opstring) {
    StackCalc sc(opstring);
    while (sc.DoNextOperation()) {
        ASSERT_EQ(sc.snCalc.top().to_string(), sc.myCalc.top().to_string());
    }
}

static byte_array GetRandomByteArray(Random random) {
    return MyBigIntImp::GetRandomByteArray(random, random.Next(1, 1024));
}

static std::string Print(byte_array bytes) {
    return MyBigIntImp::Print(bytes);
}

TEST(abs_tests, RunAbsoluteValueTests) {
    int s_samples = 10;
    Random s_random(100);
    byte_array byteArray;

// AbsoluteValue Method - Large BigIntegers
    for (int i = 0; i < s_samples; i++) {
        byteArray = GetRandomByteArray(s_random);
        VerifyAbsoluteValueString(Print(byteArray) + "uAbs");
    }

// AbsoluteValue Method - Small BigIntegers
    for (int i = 0; i < s_samples; i++) {
        byteArray = MyBigIntImp::GetRandomByteArray(s_random, 2);
        VerifyAbsoluteValueString(Print(byteArray) + "uAbs");
    }

// AbsoluteValue Method - zero
    VerifyAbsoluteValueString("0 uAbs");

// AbsoluteValue Method - -1
    VerifyAbsoluteValueString("-1 uAbs");

// AbsoluteValue Method - 1
    VerifyAbsoluteValueString("1 uAbs");

// AbsoluteValue Method - Int32.MinValue
    auto int32_min = std::to_string(std::numeric_limits<int32_t>::min());
    auto int32_max = std::to_string(std::numeric_limits<int32_t>::max());
    VerifyAbsoluteValueString(int32_min + " uAbs");

// AbsoluteValue Method - Int32.MinValue-1
    VerifyAbsoluteValueString(int32_min + " -1 b+ uAbs");

// AbsoluteValue Method - Int32.MinValue+1
    VerifyAbsoluteValueString(int32_min + " 1 b+ uAbs");

// AbsoluteValue Method - Int32.MaxValue
    VerifyAbsoluteValueString(int32_max + " uAbs");

// AbsoluteValue Method - Int32.MaxValue-1
    VerifyAbsoluteValueString(int32_max + " -1 b+ uAbs");

// AbsoluteValue Method - Int32.MaxValue+1
    VerifyAbsoluteValueString(int32_max + " 1 b+ uAbs");

// AbsoluteValue Method - Int64.MinValue
    auto int64_min = std::to_string(std::numeric_limits<int64_t>::min());
    auto int64_max = std::to_string(std::numeric_limits<int64_t>::max());
    VerifyAbsoluteValueString(int64_min + " uAbs");

// AbsoluteValue Method - Int64.MinValue-1
    VerifyAbsoluteValueString(int64_min + " -1 b+ uAbs");

// AbsoluteValue Method - Int64.MinValue+1
    VerifyAbsoluteValueString(int64_min + " 1 b+ uAbs");

// AbsoluteValue Method - Int64.MaxValue
    VerifyAbsoluteValueString(int64_max + " uAbs");

// AbsoluteValue Method - Int64.MaxValue-1
    VerifyAbsoluteValueString(int64_max + " -1 b+ uAbs");

// AbsoluteValue Method - Int64.MaxValue+1
    VerifyAbsoluteValueString(int64_max + " 1 b+ uAbs");

}