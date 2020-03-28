cmake_minimum_required(VERSION 3.15)

set(This BigIntegerTests)

set(Sources
    BigIntegerTests.cpp
)

add_executable(${This} ${Sources})
target_link_libraries(${This} PUBLIC
    gtest_main
    BigInteger
)

add_test(
    NAME ${This}
    COMMAND ${This}
)
