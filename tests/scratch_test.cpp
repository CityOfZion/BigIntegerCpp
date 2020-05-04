#include "gtest/gtest.h"
#include "../src/BigInteger.h"
#include <random>
#include "MyBigInt.h"
#include "stack_calc.h"
#include <chrono>

//
//class B {
//public:
//    B();
//    B(int value) :v(value) {};
//    B(unsigned int value) :v(value) {};
////    bool operator<(const B& rhs) const { return true; } // no implicit conversion e.g. auto r1 = 3 < B() fails
////    friend bool operator<(const B& lhs, const B& rhs) { return true; } // yes implicit conversion
//
//private:
//    int v = 0;
//};
//
//bool operator<(const B& lhs, const B& rhs) { return true; } // yes implicit conversion


TEST(scratch1, scratch_tests)
{
    std::string s("12937123987123987123987123987123987123987123129387129387123987123987");
//    auto t1 = std::chrono::high_resolution_clock::now();
//    BigInteger::parse(s);
//    auto t2 = std::chrono::high_resolution_clock::now();
//    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
//    auto r0 = 3 < B();
    auto b1 = BigInteger::one();
    auto r1 = b1 + BigInteger::one();
    ASSERT_EQ(2, r1);
    std::cout << "addr b1: " << &b1 << "\n";
    b1 += BigInteger::one();
    std::cout << "addr b1: " << &b1 << "\n";
    ASSERT_EQ(2, b1);

    b1 = BigInteger::one();
    r1 = b1 - BigInteger::one();
    ASSERT_EQ(0, r1);
    b1 -= BigInteger::one();
    ASSERT_EQ(0, b1);

    b1 = BigInteger::one();
    r1 = b1 * BigInteger(2);
    ASSERT_EQ(2, r1);
    b1 *= BigInteger(2);
    ASSERT_EQ(2, b1);

    b1 = BigInteger(6);
    r1 = b1 / BigInteger(2);
    ASSERT_EQ(3, r1);
    b1 /= BigInteger(2);
    ASSERT_EQ(3, b1);

    b1 = BigInteger::parse("123123123123123123");
    auto b2 = BigInteger::parse("4545645645646456456");
    b1 *= b2;
    std::cout << b1.to_string() << "\n";


}
