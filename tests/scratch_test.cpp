#include "gtest/gtest.h"
#include "../src/BigInteger.h"
#include <random>
#include "MyBigInt.h"
#include "stack_calc.h"
#include <chrono>

TEST(scratch1, scratch_tests)
{
    std::string s("12937123987123987123987123987123987123987123129387129387123987123987");
//    auto t1 = std::chrono::high_resolution_clock::now();
//    BigInteger::Parse(s);
//    auto t2 = std::chrono::high_resolution_clock::now();
//    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();

//    std::cout << duration;
    std::vector<int> sizes;
    sizes.push_back(sizeof(int));
    sizes.push_back(sizeof(long));
    sizes.push_back(sizeof(long long));
    sizes.push_back(sizeof(double));
    for (auto i : sizes)
        std::cout << std::to_string(i) << "\n";
}
