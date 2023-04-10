# tests/unit/utf8/CMakeLists.txt
#
# Builds the unit tests.

add_executable(algorithm_test
    "${CMAKE_CURRENT_SOURCE_DIR}/algorithm_test.cpp"
)

set_target_properties(algorithm_test
    PROPERTIES
        CXX_STANDARD ${BEETLE_CXX_STANDARD}
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_include_directories(algorithm_test
    PRIVATE
        "${BEETLE_INCLUDES}"
)

# Temp solution
target_sources(algorithm_test PRIVATE "${BEETLE_SOURCES}")

target_compile_features(algorithm_test
    PUBLIC
        cxx_std_20
)

target_link_libraries(algorithm_test
    PRIVATE
        GTest::gtest_main
)

gtest_discover_tests(algorithm_test)
