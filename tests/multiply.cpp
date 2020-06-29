#include "../include/public/bigintegercpp/BigInteger.h"
#include "gtest/gtest.h"
#include "Random.h"
#include "stack_calc.h"
#include "utils.h"
#include "../include/exceptions.h"

static void VerifyMultiplyString(std::string opstring) {
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
    return MyBigIntImp::GetRandomByteArray(random, size);
}

static byte_array GetRandomByteArray(Random random) {
    return GetRandomByteArray(random, random.Next(1, 100));
}


static std::string Print(byte_array bytes) {
    return MyBigIntImp::Print(bytes);
}

TEST(multiply, RunMultiply_TwoLargeBigIntegers) {
    int s_samples = 10;
    Random s_random(100);
    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // Multiply Method - One Large BigInteger
    for (int i = 0; i < s_samples; i++) {
        tempByteArray1 = GetRandomByteArray(s_random);
        VerifyMultiplyString(Print(tempByteArray1) + "uMultiply");
    }

    // Multiply Method - Two Large BigIntegers
    for (int i = 0; i < s_samples; i++) {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = GetRandomByteArray(s_random);
        VerifyMultiplyString(Print(tempByteArray1) + Print(tempByteArray2) + "bMultiply");
    }
}

TEST(multiply, RunMultiply_TwoSmallBigIntegers) {
    int s_samples = 10;
    Random s_random(100);
    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // Multiply Method - Two Small BigIntegers
    for (int i = 0; i < s_samples; i++) {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = GetRandomByteArray(s_random, 2);
        VerifyMultiplyString(Print(tempByteArray1) + Print(tempByteArray2) + "bMultiply");
    }
}

TEST(multiply, RunMultiply_OneLargeOneSmall) {
    int s_samples = 10;
    Random s_random(100);
    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // Multiply Method - One large and one small BigIntegers
    for (int i = 0; i < s_samples; i++) {
        try {
            tempByteArray1 = GetRandomByteArray(s_random);
            tempByteArray2 = GetRandomByteArray(s_random, 2);
            VerifyMultiplyString(Print(tempByteArray1) + Print(tempByteArray2) + "bMultiply");

            tempByteArray1 = GetRandomByteArray(s_random, 2);
            tempByteArray2 = GetRandomByteArray(s_random);
            VerifyMultiplyString(Print(tempByteArray1) + Print(tempByteArray2) + "bMultiply");
        }
        catch (...) {
            std::cout << "Array1: " + Print(tempByteArray1) << "\n";
            std::cout << "Array2: " + Print(tempByteArray2) << "\n";
            throw;
        }
    }
}

TEST(multiply, RunMultiply_OneLargeOneZero) {
    int s_samples = 10;
    Random s_random(100);
    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // Multiply Method - One large BigIntegers and zero
    for (int i = 0; i < s_samples; i++) {
        tempByteArray1 = GetRandomByteArray(s_random);
        tempByteArray2 = byte_array{0};
        VerifyMultiplyString(Print(tempByteArray1) + Print(tempByteArray2) + "bMultiply");

        tempByteArray1 = byte_array{0};
        tempByteArray2 = GetRandomByteArray(s_random);
        VerifyMultiplyString(Print(tempByteArray1) + Print(tempByteArray2) + "bMultiply");
    }
}

TEST(multiply, RunMultiply_OneSmallOneZero) {
    int s_samples = 10;
    Random s_random(100);
    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // Multiply Method - One small BigIntegers and zero
    for (int i = 0; i < s_samples; i++) {
        tempByteArray1 = GetRandomByteArray(s_random, 2);
        tempByteArray2 = byte_array{0};
        VerifyMultiplyString(Print(tempByteArray1) + Print(tempByteArray2) + "bMultiply");

        tempByteArray1 = byte_array{0};
        tempByteArray2 = GetRandomByteArray(s_random, 2);
        VerifyMultiplyString(Print(tempByteArray1) + Print(tempByteArray2) + "bMultiply");
    }
}


TEST(multiply, RunMultiply_AxiomXX1) {
    int s_samples = 10;
    Random s_random(100);
    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // Axiom: X*1 = X
    VerifyIdentityString(
            std::to_string(std::numeric_limits<int>::max()) + " " + BigInteger::one().to_string() + " bMultiply",
            std::to_string(std::numeric_limits<int32_t>::max()));
    VerifyIdentityString(
            std::to_string(std::numeric_limits<long>::max()) + " " + BigInteger::one().to_string() + " bMultiply",
            std::to_string(std::numeric_limits<int64_t>::max()));

    for (int i = 0; i < s_samples; i++) {
        std::string randBigInt = Print(GetRandomByteArray(s_random));
        VerifyIdentityString(randBigInt + BigInteger::one().to_string() + " bMultiply", randBigInt + "u+");
    }
}

TEST(multiply, RunMultiply_AxiomXX0) {
    int s_samples = 10;
    Random s_random(100);
    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // Axiom: X*0 = 0
    VerifyIdentityString(
            std::to_string(std::numeric_limits<int>::max()) + " " + BigInteger::zero().to_string() + " bMultiply",
            BigInteger::zero().to_string());
    VerifyIdentityString(
            std::to_string(std::numeric_limits<long>::max()) + " " + BigInteger::zero().to_string() + " bMultiply",
            BigInteger::zero().to_string());

    for (int i = 0; i < s_samples; i++) {
        std::string randBigInt = Print(GetRandomByteArray(s_random));
        VerifyIdentityString(randBigInt + BigInteger::zero().to_string() + " bMultiply",
                             BigInteger::zero().to_string());
    }
}

TEST(multiply, RunMultiply_Commutat) {
    int s_samples = 10;
    Random s_random(100);
    byte_array tempByteArray1;
    byte_array tempByteArray2;

    // Axiom: a*b = b*a
    VerifyIdentityString(
            std::to_string(std::numeric_limits<int>::max()) + " " + std::to_string(std::numeric_limits<long>::max()) +
            " bMultiply",
            std::to_string(std::numeric_limits<long>::max()) + " " + std::to_string(std::numeric_limits<int>::max()) +
            " bMultiply");

    for (int i = 0; i < s_samples; i++) {
        std::string randBigInt1 = Print(GetRandomByteArray(s_random));
        std::string randBigInt2 = Print(GetRandomByteArray(s_random));
        VerifyIdentityString(randBigInt1 + randBigInt2 + "bMultiply", randBigInt2 + randBigInt1 + "bMultiply");
    }
}

TEST(multiply, RunMultiply_Boundary) {
    // Check interesting cases for boundary conditions
    // You'll either be shifting a 0 or 1 across the boundary
    // 32 bit boundary  n2=0
    VerifyMultiplyString(std::to_string(std::pow(2, 32)) + " 2 bMultiply");

    // 32 bit boundary  n1=0 n2=1
    VerifyMultiplyString(std::to_string(std::pow(2, 33)) + " 2 bMultiply");
}
