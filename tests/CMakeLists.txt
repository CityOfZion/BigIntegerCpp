add_executable(
    unit_tests
    bigint_test_sa.cpp
    bigint_test_add.cpp
    bigint_test_sub.cpp
    bigint_test_byte_array.cpp
    op_leftshift.cpp
    MyBigInt.cpp
    MyBigInt.h
    Random.cpp
    Random.h
    BitArray.cpp
    BitArray.h
    scratch_test.cpp
    stack_calc.cpp
    utils.h
    op_rightshift.cpp
    op_xor.cpp op_or.cpp)

target_link_libraries(
    unit_tests
    gtest_main
    BigInteger
    )

add_test(
    NAME 
        add
    COMMAND 
        ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/unit_tests
)
