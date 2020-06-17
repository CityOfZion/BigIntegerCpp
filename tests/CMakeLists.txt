if (NOT GTEST_FOUND)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        release-1.10.0
    )
    FetchContent_MakeAvailable(googletest)
endif()

enable_testing()

add_executable(
        unit_tests
        bigint_test_sa.cpp
        BigInteger_AddTests.cpp
        BigInteger_SubtractTests.cpp
        TobyteArray.cpp
        MyBigInt.cpp
        MyBigInt.h
        Random.cpp
        Random.h
        BitArray.cpp
        BitArray.h
        stack_calc.cpp
        utils.h
        utils.cpp
        op_leftshift.cpp
        op_rightshift.cpp
        op_xor.cpp
        op_or.cpp
        op_not.cpp
        op_modulus.cpp
        op_and.cpp
        op_decrement.cpp
        op_increment.cpp
        op_divide.cpp
        op_multiply.cpp
        IsEven.cpp
        IsOne.cpp
        IsZero.cpp
        IsPowerOfTwo.cpp
        max.cpp
        min.cpp
        sign.cpp
        remainder.cpp
        pow.cpp
        modpow.cpp
        log.cpp
        log02.cpp
        log10.cpp
        divide.cpp
        multiply.cpp
        absolutevalue.cpp
        divrem.cpp
        gcd.cpp
        properties.cpp)

target_link_libraries(
        unit_tests
        gtest_main
        $<TARGET_OBJECTS:BigIntegerCpp-obj>
)

add_test(
        NAME
        add
        COMMAND
        ${CMAKE_BINARY_DIR}/tests/unit_tests
)
