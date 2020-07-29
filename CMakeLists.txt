cmake_minimum_required(VERSION 3.15)
project(BigIntegerCpp VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

option(BUILD_PYTHON_BINDINGS "Build the Python bindings" OFF)
option(BUILD_SHARED_LIB "Build a shared library" ON)
option(BUILD_STATIC_LIB "Build a static library" OFF)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/public/bigintegercpp/version.h)

set(bigintegercpp_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/BigInteger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/BigIntegerCalculator/AddSub.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/BigIntegerCalculator/BitsBuffer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/BigIntegerCalculator/DivRem.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/BigIntegerCalculator/FastReducer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/BigIntegerCalculator/GcdInv.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/BigIntegerCalculator/PowMod.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/BigIntegerCalculator/SquMul.cpp
        )
set(bigintegercpp_INCLUDE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/public/bigintegercpp/BigInteger.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/public/bigintegercpp/version.h
        )
set(bigintegercpp_PRIVATE_INCLUDE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/BigIntegerCalculator.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/BitsBuffer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/exceptions.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/FastReducer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/NumericsHelpers.h
        )

# expose ${bigintegercpp_INCLUDE_DIRS} for downstream projects to consume
set(bigintegercpp_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include/public/ CACHE INTERNAL "")

if (BUILD_SHARED_LIB OR BUILD_STATIC_LIB OR BUILD_TESTING)
    add_library(BigIntegerCpp-obj SHARED ${bigintegercpp_SOURCES})
    target_include_directories(BigIntegerCpp-obj
            PUBLIC
            $<INSTALL_INTERFACE:include/public>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/public>
            PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            )
    set_target_properties(BigIntegerCpp-obj PROPERTIES
            POSITION_INDEPENDENT_CODE ON
            )
    if (BUILD_SHARED_LIB)
        add_library(BigIntegerCpp-shared SHARED $<TARGET_OBJECTS:BigIntegerCpp-obj>)
        set_target_properties(BigIntegerCpp-shared
                PROPERTIES
                OUTPUT_NAME "BigIntegerCpp"
                )
        add_library(BigIntegerCpp::BigIntegerCpp ALIAS BigIntegerCpp-shared)
    endif()
    if (BUILD_STATIC_LIB)
        add_library(BigIntegerCpp-static STATIC $<TARGET_OBJECTS:BigIntegerCpp-obj>)
        add_library(BigIntegerCpp::BigIntegerCpp-static ALIAS BigIntegerCpp-static)
    endif()
endif()

if (BUILD_PYTHON_BINDINGS)
    add_subdirectory(bindings/python)
endif()

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests)
endif()

##############
# Installation

include(GNUInstallDirs)


# can't use generator expression for target list
if (BUILD_SHARED_LIB AND BUILD_STATIC_LIB)
    install(TARGETS BigIntegerCpp-shared BigIntegerCpp-static
            EXPORT BigIntegerCpp-targets
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
elseif (BUILD_SHARED_LIB)
    install(TARGETS BigIntegerCpp-shared
            EXPORT BigIntegerCpp-targets
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
elseif (BUILD_STATIC_LIB)
    install(TARGETS BigIntegerCpp-static
            EXPORT BigIntegerCpp-targets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

install(DIRECTORY include/public/bigintegercpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT BigIntegerCpp-targets
        FILE BigIntegerCppTargets.cmake
        NAMESPACE BigIntegerCpp::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/BigIntegerCpp)

#Create a ConfigVersion.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/BigIntegerCppConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)

include(${CMAKE_CURRENT_BINARY_DIR}/BigIntegerCppConfigVersion.cmake)
configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/BigIntegerCppConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/BigIntegerCppConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )

export(EXPORT BigIntegerCpp-targets
        FILE ${CMAKE_CURRENT_BINARY_DIR}/BigIntegerCppTargets.cmake
        NAMESPACE BigIntegerCpp::)
