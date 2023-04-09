# tests/unit/unicode/CMakeLists.txt
#
# Builds the unit tests.

add_executable(unicode_test
    "${CMAKE_CURRENT_SOURCE_DIR}/unicode_test.cpp"
)

set_target_properties(unicode_test
    PROPERTIES
        CXX_STANDARD ${BEETLE_CXX_STANDARD}
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_include_directories(unicode_test
    PRIVATE
        "${BEETLE_INCLUDES}"
)

# Temp solution
target_sources(unicode_test PRIVATE "${BEETLE_SOURCES}")

target_compile_features(unicode_test
    PUBLIC
        cxx_std_20
)

target_link_libraries(unicode_test
    PRIVATE
        GTest::gtest_main
)

gtest_discover_tests(unicode_test)
