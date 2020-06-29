#include "../include/public/bigintegercpp/BigInteger.h"
#include "gtest/gtest.h"
#include "Random.h"
#include "stack_calc.h"
#include "utils.h"
#include "../include/exceptions.h"

static void VerifyDivideString(std::string opstring) {
    StackCalc sc(opstring);
    while (sc.DoNextOperation()) {
        ASSERT_EQ(sc.snCalc.top().to_string(), sc.myCalc.top().to_string());
    }
}

static void VerifyIdentityString(std::string opstring1, std::string opstring2) {
    StackCalc sc1(opstring1);
    while (sc1.DoNextOperation()) {
        //Run the full calculation
        sc1.DoNextOperation();
    }

    StackCalc sc2(opstring2);
    while (sc2.DoNextOperation()) {
        //Run the full calculation
        sc2.DoNextOperation();
    }
    ASSERT_EQ(sc1.snCalc.top().to_string(), sc2.snCalc.top().to_string());
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

TEST(divide, RunDivideTwoLargeBI) {
    int s_samples = 10;
    Random s_random(100);
    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // Divide Method - Two Large BigIntegers
    for (int i = 0; i < s_samples; i++) {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = GetRandomByteArray(s_random);
        VerifyDivideString(Print(tempByteArray1) + Print(tempByteArray2) + "bDivide");
    }
}

TEST(divide, RunDivideTwoSmallBI) {
    int s_samples = 10;
    Random s_random(100);
    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // Divide Method - Two Small BigIntegers
    for (int i = 0; i < s_samples; i++) {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = GetRandomByteArray(s_random, 2);
        VerifyDivideString(Print(tempByteArray1) + Print(tempByteArray2) + "bDivide");
    }
}

TEST(divide, RunDivideOneLargeOneSmallBI) {
    int s_samples = 10;
    Random s_random(100);
    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // Divide Method - One large and one small BigIntegers
    for (int i = 0; i < s_samples; i++) {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = GetRandomByteArray(s_random, 2);
        VerifyDivideString(Print(tempByteArray1) + Print(tempByteArray2) + "bDivide");

        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = GetRandomByteArray(s_random);
        VerifyDivideString(Print(tempByteArray1) + Print(tempByteArray2) + "bDivide");
    }
}

TEST(divide, RunDivideOneLargeOneZeroBI) {
    int s_samples = 10;
    Random s_random(100);
    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // Divide Method - One large BigIntegers and zero
    for (int i = 0; i < s_samples; i++) {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = byte_array{0};
        VerifyDivideString(Print(tempByteArray1) + Print(tempByteArray2) + "bDivide");

        EXPECT_THROW(VerifyDivideString(Print(tempByteArray2) + Print(tempByteArray1) + "bDivide"), DivideByZero);
    }
}

TEST(divide, RunDivideOneSmallOneZeroBI) {
    int s_samples = 10;
    Random s_random(100);
    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // Divide Method - One large BigIntegers and zero
    for (int i = 0; i < s_samples; i++) {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = byte_array{0};
        VerifyDivideString(Print(tempByteArray1) + Print(tempByteArray2) + "bDivide");

        EXPECT_THROW(VerifyDivideString(Print(tempByteArray2) + Print(tempByteArray1) + "bDivide"), DivideByZero);
    }
}

TEST(divide, RunDivideOneOverOne) {
    int s_samples = 10;
    Random s_random(100);

    // Axiom: X/1 = X
    VerifyIdentityString(
            BigInteger::one().to_string() + " " + std::to_string(std::numeric_limits<int>::max()) + " bDivide",
            std::to_string(std::numeric_limits<int32_t>::max()));
    VerifyIdentityString(
            BigInteger::one().to_string() + " " + std::to_string(std::numeric_limits<long>::max()) + " bDivide",
            std::to_string(std::numeric_limits<int64_t>::max()));

    for (int i = 0; i < s_samples; i++) {
        std::string randBigInt = Print(GetRandomByteArray(s_random));
        VerifyIdentityString(BigInteger::one().to_string() + " " + randBigInt + "bDivide",
                             randBigInt.substr(0, randBigInt.size() - 1));
    }
}

TEST(divide, RunDivideZeroOverBI) {
    int s_samples = 10;
    Random s_random(100);

    // Axiom: 0/X = 0
    VerifyIdentityString(std::to_string(std::numeric_limits<int>::max()) + " " + BigInteger::zero().to_string() + " bDivide", BigInteger::zero().to_string());
    VerifyIdentityString(std::to_string(std::numeric_limits<long>::max()) + " " + BigInteger::zero().to_string() + " bDivide", BigInteger::zero().to_string());

    for (int i = 0; i < s_samples; i++) {
        std::string randBigInt = Print(GetRandomByteArray(s_random));
        VerifyIdentityString(randBigInt + BigInteger::zero().to_string() + " bDivide", BigInteger::zero().to_string());
    }
}

TEST(divide, RunDivideBoundary) {
    // Check interesting cases for boundary conditions
    // You'll either be shifting a 0 or 1 across the boundary
    // 32 bit boundary  n2=0
    VerifyDivideString(std::to_string(std::pow(2, 32)) + " 2 bDivide");

    // 32 bit boundary  n1=0 n2=1
    VerifyDivideString(std::to_string(std::pow(2, 33)) + " 2 bDivide");
}

TEST(divide, RunOverflow) {
    auto x = BigInteger(byte_array{0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,0});
    auto y = BigInteger(byte_array{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0});
    auto z = BigInteger(byte_array{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0});

    ASSERT_EQ(z, BigInteger::divide(x, y));
}