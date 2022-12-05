# src/core/CMakeLists.txt

target_include_directories(Beetle
    PUBLIC
        "../include"
        "${BEETLE_GENERATED_INCLUDE}"
)

target_sources(Beetle
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/assert.cpp"
)

