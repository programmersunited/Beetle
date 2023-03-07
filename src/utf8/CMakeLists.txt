# src/utf8/CMakeLists.txt

target_sources(Beetle
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/error.cpp"
)

# Add utf8 internal API
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/internal")

