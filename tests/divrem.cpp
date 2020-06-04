#include "../src/BigInteger.h"
#include "gtest/gtest.h"
#include "Random.h"
#include "stack_calc.h"
#include "utils.h"
#include "../src/exceptions.h"

static void VerifyDivRemString(std::string opstring) {
    try {
        StackCalc sc(opstring);
        while (sc.DoNextOperation()) {
            ASSERT_EQ(sc.snCalc.top().to_string(), sc.myCalc.top().to_string());
            sc.VerifyOutParameter();
        }
    } catch (DivideByZero) {
        throw;
    } catch (...) {
        std::cout << "VerifyDivRemString failed: " + opstring << "\n";
        throw;
    }
}

static byte_array GetRandomByteArray(Random random, int size) {
    return MyBigIntImp::GetNonZeroRandomByteArray(random, size);
}

static byte_array GetRandomByteArray(Random random) {
    return GetRandomByteArray(random, random.Next(1, 100));
}


static std::string Print(byte_array bytes) {
    return MyBigIntImp::Print(bytes);
}

TEST(divrem, RunDivRem_TwoLargeBI) {
    int s_samples = 10;
    Random s_random(100);
    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // DivRem Method - Two Large BigIntegers
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = GetRandomByteArray(s_random);
        VerifyDivRemString(Print(tempByteArray1) + Print(tempByteArray2) + "bDivRem");
    }
}

TEST(divrem, RunDivRem_TwoSmallBI) {
    int s_samples = 10;
    Random s_random(100);
    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // DivRem Method - Two Small BigIntegers
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = GetRandomByteArray(s_random, 2);
        VerifyDivRemString(Print(tempByteArray1) + Print(tempByteArray2) + "bDivRem");
    }
}

TEST(divrem, RunDivRem_OneSmallOneLargeBI) {
    int s_samples = 10;
    Random s_random(100);
    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // DivRem Method - One Large and one small BigIntegers
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = GetRandomByteArray(s_random, 2);
        VerifyDivRemString(Print(tempByteArray1) + Print(tempByteArray2) + "bDivRem");

        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = GetRandomByteArray(s_random);
        VerifyDivRemString(Print(tempByteArray1) + Print(tempByteArray2) + "bDivRem");
    }
}

TEST(divrem, RunDivRem_OneLargeOne0BI) {
    int s_samples = 10;
    Random s_random(100);
    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // DivRem Method - One Large BigIntegers and zero
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = byte_array{ 0 };
        VerifyDivRemString(Print(tempByteArray1) + Print(tempByteArray2) + "bDivRem");

        EXPECT_THROW(VerifyDivRemString(Print(tempByteArray2) + Print(tempByteArray1) + "bDivRem"), DivideByZero);
    }
}

TEST(divrem, RunDivRem_OneSmallOne0BI) {
    int s_samples = 10;
    Random s_random(100);
    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // DivRem Method - One small BigIntegers and zero
    for (int i = 0; i < s_samples; i++)
    {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = byte_array { 0 };
        VerifyDivRemString(Print(tempByteArray1) + Print(tempByteArray2) + "bDivRem");

        EXPECT_THROW(VerifyDivRemString(Print(tempByteArray2) + Print(tempByteArray1) + "bDivRem"), DivideByZero);
    }
}

TEST(divrem, Boundary) {
    // Check interesting cases for boundary conditions
    // You'll either be shifting a 0 or 1 across the boundary
    // 32 bit boundary  n2=0
    VerifyDivRemString(std::to_string(std::pow(2, 32)) + " 2 bDivRem");

    // 32 bit boundary  n1=0 n2=1
    VerifyDivRemString(std::to_string(std::pow(2, 33)) + " 2 bDivRem");
}

