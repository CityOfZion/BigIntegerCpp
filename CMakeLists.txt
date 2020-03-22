cmake_minimum_required(VERSION 3.15)
project(BigIntegerCpp)

set(CMAKE_CXX_STANDARD 17)

add_library(BigIntegerCpp SHARED BigInteger.cpp BigInteger.h)
