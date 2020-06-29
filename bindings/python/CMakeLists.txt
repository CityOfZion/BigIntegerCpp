set(PYBIND11_CPP_STANDARD -std=c++1z) # Experimental C++17 support
message(STATUS "Using Python: ${PYTHON_EXECUTABLE}")

if (NOT pybind11_FOUND)
    FetchContent_Declare(
            pybind11
            GIT_REPOSITORY https://github.com/pybind/pybind11.git
            GIT_TAG        v2.5.0
    )
    FetchContent_MakeAvailable(pybind11)
endif()

# copy files into a folder to make it easy to build a wheel
configure_file(setup.py.in ${CMAKE_CURRENT_BINARY_DIR}/pybiginteger/setup.py)

# create a monolith module
# saves us a lot of pain when creating a wheel later on
pybind11_add_module(pybiginteger python-bindings.cpp ${bigintegercpp_SOURCES} ${bigintegercpp_INCLUDE} ${bigintegercpp_PRIVATE_INCLUDE})

set_target_properties(pybiginteger
        PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pybiginteger"
)

function(check_wheel_exists)
    message(STATUS "Checking if wheel exists")
    execute_process(
        COMMAND
        ${PYTHON_EXECUTABLE} "-c" "import wheel; print(wheel.__version__)"
        RESULT_VARIABLE WHEEL_RETURN
        OUTPUT_VARIABLE WHEEL_VERSION_STRING
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if (${WHEEL_RETURN} MATCHES 0)
        set(WHEEL_FOUND TRUE CACHE INTERNAL "WHEEL_FOUND")
        set(WHEEL_VERSION ${WHEEL_VERSION_STRING} CACHE INTERNAL "WHEEL_VERSION")
    else()
        set(WHEEL_FOUND FALSE CACHE INTERNAL "WHEEL_FOUND")
    endif()
endfunction()

# We check if Wheel exists, if not we give it one attempt to install
check_wheel_exists()
if (NOT ${WHEEL_FOUND})
    message(STATUS "Wheel not found. Installing...")
    string(REGEX REPLACE "python(.*)$" "pip" PIP_EXECUTABLE ${PYTHON_EXECUTABLE})
    execute_process(
        COMMAND
        ${PIP_EXECUTABLE} install wheel
    )

    check_wheel_exists()
    if (NOT ${WHEEL_FOUND})
        message(STATUS "Failed to find/install wheel. Cannot package bindings into wheel..")
    endif()
endif()

if (${WHEEL_FOUND})
    message(STATUS "Found wheel: ${WHEEL_VERSION}")
    add_custom_command(TARGET pybiginteger
        POST_BUILD
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/pybiginteger
        COMMAND ${CMAKE_COMMAND} -E echo "Packacking wheel"
        COMMAND ${PYTHON_EXECUTABLE} setup.py bdist_wheel --dist-dir=${CMAKE_CURRENT_BINARY_DIR}
        COMMAND ${CMAKE_COMMAND} -E echo "Wheel output can be found in ${CMAKE_CURRENT_BINARY_DIR}"
    )
endif()