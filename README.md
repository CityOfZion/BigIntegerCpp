# BigIntegerCpp
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
| BUILD_TESTS           | OFF     |                              |
| BUILD_PYTHON_BINDINGS | OFF     | For more information go [here](bindings/python/README.md) |

### Running the tests
Test can be ran as follows

    cmake -D BUILD_TESTS=ON ..
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

TODO

## FAQ
1) Why this project? 

   It is purpose build for use in relationship to the [NEO blockchain](https://github.com/neo-project/) project. 
   In order to create a compliant port of their virtual machine a need for a compliant BigInteger implementation exists. 
   Any difference in for example the [modulo implementation](https://en.wikipedia.org/wiki/Modulo_operation#Variants_of_the_definition) can result in VM execution deviation. This is just one of the many problems we've encountered after creating a pure Python port. 

2) Should I this project?

   If you have to ask this question, then no.

3) How fast is it?
    
    We don't know. The focus has been on conformity to the C# BigInteger class, not on speed. If you want speed you might want to look at https://gmplib.org/
