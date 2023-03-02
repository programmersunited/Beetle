# CMakeLists.txt
#
# This is the root CMakeLists.txt file for this project. This file contains the 
# options that are available for the project and will build the targets based on
# said options.

# cmake_dependent_option requires 3.22
cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

project(Beetle
    VERSION 0.0.1
    DESCRIPTION "A modern ISO C++20 UTF-8 library."
    LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

################################################################################
#                                                                              #
# Project Options                                                              #
#                                                                              #
################################################################################

# Some options depend on previous options
include(CMakeDependentOption)

option(BEETLE_BUILD_DOCS "Build documentation." ON)

option(BEETLE_BUILD_TESTS "Build tests." ON)
cmake_dependent_option(BEETLE_BUILD_UNIT_TESTS "Build unit tests." ON "BEETLE_BUILD_TESTS" OFF)
cmake_dependent_option(BEETLE_BUILD_INTEGRATION_TESTS "Build integration tests." ON "BEETLE_BUILD_TESTS" OFF)

option(BEETLE_BUILD_EXAMPLES "Build examples." ON)

option(BEETLE_ENABLE_ASSERTS "Asserts will be active regardless of configuration mode." ON)

################################################################################
#                                                                              #
# Add Sub-directories                                                          #
#                                                                              #
################################################################################

# Add internal API and source files
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src")

#if(BEETLE_BUILD_DOCS)
#    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/doc")
#endif()

#if(BEETLE_BUILD_TESTS AND PROJECT_IS_TOP_LEVEL)
#    enable_testing()
#    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/tests")
#endif()

#if(BEETLE_BUILD_EXAMPLES AND PROJECT_IS_TOP_LEVEL)
#    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/examples")
#endif()
