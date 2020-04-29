cmake_minimum_required(VERSION 3.15)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
#SET(CMAKE_CXX_FLAGS_DEBUG "-O0") # for py extension debugging
set(PYTHON_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR}/venv/bin/python3)
#set(PYBIND11_CPP_STANDARD -std=c++1z) # Experimental C++17 support

include(FetchContent)
FetchContent_Declare(
        pybind
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        GIT_TAG        v2.5.0
)
FetchContent_MakeAvailable(pybind)

project(BigIntegerCpp)

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#
#add_subdirectory(tests)
#
#add_library(BigIntegerCpp SHARED
#            BigInteger.cpp
#            BigInteger.h
#            BigNumber.cpp
#            BigNumber.h
#           )
#

include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

# we use this to get code coverage
if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif()

add_subdirectory(src)

include(cmake/googletest.cmake)
fetch_googletest(
    ${PROJECT_SOURCE_DIR}/cmake
    ${PROJECT_BINARY_DIR}/googletests
    )

enable_testing()
add_subdirectory(tests)

add_subdirectory(bindings)

pybind11_add_module(pybiginteger bindings/python.cpp)
target_link_libraries(pybiginteger PRIVATE BigInteger)
