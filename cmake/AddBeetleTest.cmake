# AddBeetleTest.cmake

macro(AddBeetleTest arg_test_target arg_sources)
    add_executable(code_unit_test
        "${CMAKE_CURRENT_SOURCE_DIR}/code_unit_test.cpp"
    )

    target_include_directories(code_unit_test
        PRIVATE
            "${Beetle_SOURCE_DIR}/include"
    )

    target_compile_features(code_unit_test
        PUBLIC
            cxx_std_20
    )

    target_link_libraries(code_unit_test GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(code_unit_test)
endmacro()
