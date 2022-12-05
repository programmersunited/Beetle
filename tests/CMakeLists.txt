# tests/CMakeLists.txt
#
# Sets up the testing framework and includes the unit tests and integration 
# tests based on previously set options.

################################################################################
#                                                                              #
# Add Testing Framework To Project - GoogleTest                                #
#                                                                              #
################################################################################

# Most of the code below from: 
#   https://google.github.io/googletest/quickstart-cmake.html

include(FetchContent)
FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Bring CMake GoogleTest functions into scope
include(GoogleTest)

################################################################################
#                                                                              #
# Add Tests                                                                    #
#                                                                              #
################################################################################

if(BEETLE_BUILD_UNIT_TESTS)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/unit")
endif()

if(BEETLE_BUILD_INTEGRATION_TESTS)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/integration")
endif()
