# tests/unit/CMakeLists.txt
#
# Builds the unit tests.

get_target_property(BEETLE_INCLUDES Beetle INCLUDE_DIRECTORIES)
get_target_property(BEETLE_CXX_STANDARD Beetle CXX_STANDARD)
get_target_property(BEETLE_SOURCES Beetle SOURCES)

list(REMOVE_ITEM BEETLE_SOURCES ${BEETLE_UTILITY_ENTRYPOINT})

add_executable(code_unit_test
    "${CMAKE_CURRENT_SOURCE_DIR}/code_unit_test.cpp"
)

set_target_properties(code_unit_test
    PROPERTIES
        CXX_STANDARD ${BEETLE_CXX_STANDARD}
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_include_directories(code_unit_test
    PRIVATE
        "${BEETLE_INCLUDES}"
)

# Temp solution
target_sources(code_unit_test PRIVATE "${BEETLE_SOURCES}")

target_compile_features(code_unit_test
    PUBLIC
        cxx_std_20
)

target_link_libraries(code_unit_test
    PRIVATE
        GTest::gtest_main
)

gtest_discover_tests(code_unit_test)

####### Validate Test

add_executable(validate_test
    "${CMAKE_CURRENT_SOURCE_DIR}/validate_test.cpp"
)

target_include_directories(validate_test
    PRIVATE
        "${Beetle_SOURCE_DIR}/include"
)

# Temp solution
target_sources(validate_test PRIVATE "${BEETLE_SOURCES}")

target_compile_features(validate_test
    PUBLIC
        cxx_std_20
)

target_link_libraries(validate_test GTest::gtest_main)

gtest_discover_tests(validate_test)
