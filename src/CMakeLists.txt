add_library(BigInteger SHARED)

set(CMAKE_BUILD_TYPE Debug)

target_sources(
    BigInteger
    PRIVATE
    BigInteger.cpp
    BigNumber.cpp
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/BigInteger.h
        ${CMAKE_CURRENT_LIST_DIR}/BigNumber.h
    )

target_include_directories(
    BigInteger
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
    )
