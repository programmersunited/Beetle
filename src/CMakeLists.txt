# src/CMakeLists.txt
#
# This creates the target, sets the properties and calls the modules.

################################################################################
#                                                                              #
# Project's Target Settings                                                    #
#                                                                              #
################################################################################

set(BEETLE_UTILITY_ENTRYPOINT "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp" CACHE INTERNAL "")

# Create project's main target - has to be done before using it
add_executable(Beetle
    ${BEETLE_UTILITY_ENTRYPOINT}
    "${CMAKE_CURRENT_SOURCE_DIR}/exception.cpp"
)

set_target_properties(Beetle
    PROPERTIES
        OUTPUT_NAME "Beetle_${Beetle_VERSION_MAJOR}_${Beetle_VERSION_MINOR}_${Beetle_VERSION_PATCH}_${CMAKE_SYSTEM_NAME}_${CMAKE_CXX_COMPILER_ID}"
        DEBUG_POSTFIX "_debug"
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

################################################################################
# Includes & Sources                                                           #
################################################################################

# Create folder for generated header file(s)
set(BEETLE_GENERATED_INCLUDE "${CMAKE_BINARY_DIR}/include" CACHE INTERNAL "")
file(MAKE_DIRECTORY "${BEETLE_GENERATED_INCLUDE}")

# Initialize version header file with info from CMake
configure_file("${PROJECT_SOURCE_DIR}/cmake/version.hpp.in" "${BEETLE_GENERATED_INCLUDE}/beetle/version.hpp")

target_include_directories(Beetle
    PUBLIC
        "../include"
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${BEETLE_GENERATED_INCLUDE}"
)

#target_sources(Beetle
#    PUBLIC
#        "${CMAKE_CURRENT_SOURCE_DIR}/assert.cpp"
#)

################################################################################
# Add Hidden API Modules and Source Files                                      #
################################################################################

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/core")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/utf8")
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/code_unit")

################################################################################
# Compiling - Features, flags & definitions                                    #
################################################################################

# The current target is C++20 but most compilers do not fully support C++20 yet
target_compile_features(Beetle
    PUBLIC
        cxx_std_20
)

target_compile_options(Beetle
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>: /W4 /WX>
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>: -Wall -Wextra -pedantic -Wconversion -Werror -Wfatal-errors -Wshadow>
)

target_compile_definitions(Beetle
    PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:BEETLE_DEBUG>
        $<$<BOOL:${BEETLE_ENABLE_ASSERTS}>:BEETLE_ENABLE_ASSERTS>
)
