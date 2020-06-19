cmake_minimum_required(VERSION 3.15)
project(BigIntegerCpp VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

option(BUILD_PYTHON_BINDINGS "Build the Python bindings" ON)
option(BUILD_SHARED_LIB "Build a shared library" ON)
option(BUILD_STATIC_LIB "Build a static library" OFF)
option(BUILD_TESTS "Build unit tests" OFF)

set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/BigInteger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/BigNumber.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/BigIntegerCalculator/AddSub.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/BigIntegerCalculator/BitsBuffer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/BigIntegerCalculator/DivRem.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/BigIntegerCalculator/FastReducer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/BigIntegerCalculator/GcdInv.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/BigIntegerCalculator/PowMod.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/BigIntegerCalculator/SquMul.cpp
)
set(PUBLIC_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include/public/BigInteger.h)
set(PRIVATE_INCLUDE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/BigIntegerCalculator.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/BigNumber.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/BitsBuffer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/exceptions.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/FastReducer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/NumericsHelpers.h
)

if (BUILD_SHARED_LIB OR BUILD_STATIC_LIB OR BUILD_TESTS)
    add_library(BigIntegerCpp-obj OBJECT ${SOURCES})
    target_include_directories(BigIntegerCpp-obj
            PUBLIC
                $<INSTALL_INTERFACE:include/public>
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/public>
            PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    if (BUILD_SHARED_LIB)
        add_library(BigIntegerCpp-shared SHARED $<TARGET_OBJECTS:BigIntegerCpp-obj>)
        set_target_properties(BigIntegerCpp-shared
            PROPERTIES
            OUTPUT_NAME "BigIntegerCpp"
        )
    endif()
    if (BUILD_STATIC_LIB)
        add_library(BigIntegerCpp-static STATIC $<TARGET_OBJECTS:BigIntegerCpp-obj>)
        set_target_properties(BigIntegerCpp-static
                PROPERTIES
                OUTPUT_NAME "BigIntegerCpp"
        )
    endif()
endif()

if (BUILD_PYTHON_BINDINGS)
    add_subdirectory(bindings/python)
endif()

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()

##############
# Installation


#include(GNUInstallDirs)
#set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/BigIntegerCpp)
#
#install(TARGETS BigIntegerCpp-obj
#        EXPORT BigIntegerCpp-targets
#        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # shared
#        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} # static
#)
#
#set_target_properties(BigIntegerCpp-obj PROPERTIES EXPORT_NAME BigIntegerCpp)
#
#install(DIRECTORY include/public DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
#
#install(EXPORT BigIntegerCpp-targets
#        FILE
#        BigIntegerCppTargets.cmake
#        DESTINATION
#        ${INSTALL_CONFIGDIR}
#        )
#
##Create a ConfigVersion.cmake file
#include(CMakePackageConfigHelpers)
#write_basic_package_version_file(
#        ${CMAKE_CURRENT_BINARY_DIR}/BigIntegerCppConfigVersion.cmake
#        VERSION ${PROJECT_VERSION}
#        COMPATIBILITY AnyNewerVersion
#)
#
#configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/BigIntegerCppConfig.cmake.in
#        ${CMAKE_CURRENT_BINARY_DIR}/BigIntegerCppConfig.cmake
#        INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
#        )
#
##Install the config, configversion and custom find modules
#install(FILES
#        ${CMAKE_CURRENT_BINARY_DIR}/BigIntegerCppConfig.cmake
#        ${CMAKE_CURRENT_BINARY_DIR}/BigIntegerCppConfigVersion.cmake
#        DESTINATION ${INSTALL_CONFIGDIR}
#        )
#
#export(EXPORT BigIntegerCpp-targets
#        FILE ${CMAKE_CURRENT_BINARY_DIR}/BigIntegerCppTargets.cmake)