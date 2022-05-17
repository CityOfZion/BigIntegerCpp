#set(PYBIND11_CPP_STANDARD -std=c++1z) # Experimental C++17 support
message(STATUS "Using Python: ${PYTHON_EXECUTABLE}")

if (NOT pybind11_FOUND)
    FetchContent_Declare(
            pybind11
            GIT_REPOSITORY https://github.com/pybind/pybind11.git
            GIT_TAG        v2.9.2
    )
    FetchContent_MakeAvailable(pybind11)
endif()

# create a monolith module
# saves us a lot of pain when creating a wheel later on
pybind11_add_module(pybiginteger python-bindings.cpp ${bigintegercpp_SOURCES} ${bigintegercpp_INCLUDE} ${bigintegercpp_PRIVATE_INCLUDE})
