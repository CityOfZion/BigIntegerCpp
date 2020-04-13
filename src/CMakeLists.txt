add_library(BigInteger SHARED BigIntegerCalculator/PowMod.cpp BigIntegerCalculator/BigIntegerCalculator.h BigIntegerCalculator/BitsBuffer.cpp BigIntegerCalculator/BitsBuffer.h BigIntegerCalculator/FastReducer.cpp BigIntegerCalculator/FastReducer.h)

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
