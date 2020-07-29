![](https://raw.githubusercontent.com/CityOfZion/visual-identity/develop/_CoZ%20Branding/_Logo/_Logo%20icon/_PNG%20200x178px/CoZ_Icon_DARKBLUE_200x178px.png)
    
# BigIntegerCpp
[![CircleCI](https://circleci.com/gh/CityOfZion/neo3-python.svg?style=shield)](https://circleci.com/gh/CityOfZion/neo3-python)

A C++ port of the [C# BigInteger](https://docs.microsoft.com/en-us/dotnet/api/system.numerics.biginteger?view=netcore-3.1) class, with bindings for Python.

# Building

The default configuration builds a shared library

    mkdir build
    cd build
    cmake ..
    cmake --build .

### Build options

You can define various options during the configure stage to control the output

| Var                   | Default | Comment                      |
|-----------------------|---------|------------------------------|
| BUILD_SHARED_LIB      | ON      |                              |
| BUILD_STATIC_LIB      | OFF     |                              |
| BUILD_TESTING         | OFF     |                              |

### Building the bindings

Read [here](bindings/README.md).

### Running the tests
Test can be ran as follows

    cmake -D BUILD_TESTING=ON ..
    cmake --build .
    ./tests/unit_tests

The output should be similar to

    ...
    
    [ RUN      ] properties.RunMinusOneTests
    [       OK ] properties.RunMinusOneTests (2 ms)
    [----------] 3 tests from properties (7 ms total)
    [----------] Global test environment tear-down
    [==========] 117 tests from 22 test suites ran. (17481 ms total)
    [  PASSED  ] 117 tests.

## Including in your CMake project
   The project can be included in 2 ways

    add_subdirectory(BigIntegerCpp)
    
or if it has been installed via `make install` you can use

    find_library(bigintegercpp)
   
It is possible to build both a `SHARED` and `STATIC` library simultaneously, as such 2 link targets exists to differentiate between them, respectively

    BigIntegerCpp::BigIntegerCpp
    BigIntegerCpp::BigIntegerCpp-static

To use in your target add

    target_include_directories(your_target PUBLIC ${bigintegercpp_INCLUDE_DIRS})
    target_link_libraries(your_target PRIVATE BigIntegerCpp::BigIntegerCpp)
    
Source.cpp

    #include <bigintegercpp/BigInteger.h>
    
## FAQ
1) Why this project? 

   It is purpose build for use in relationship to the [NEO blockchain](https://github.com/neo-project/) project. 
   In order to create a compliant port of their virtual machine a need for a compliant BigInteger implementation exists. 
   Any difference, in for example the [modulo implementation](https://en.wikipedia.org/wiki/Modulo_operation#Variants_of_the_definition), can result in VM execution deviation. This is just one of the many problems we've encountered after creating a pure Python port. 

2) Should I use this project?

   If you have to ask this question, then no.

3) How fast is it?
    
   We don't know. The focus has been on conformity to the C# BigInteger class, not on speed. If you want speed you might want to look at https://gmplib.org/

4) Are there any known behavioural deviations from the C# implementation?

   The only known deviations are in the string parsing and conversion to string methods. Specifically, the overloads with `IFormatProvider` are not supported.
   Only base10 parsing is supported. String input may be prepended with `+` or `-`. Any whitespace is considered the end of the input.
   
   Deviations in any other parts are considered bugs. Please report them.
