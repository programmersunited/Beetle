# tests/unit/CMakeLists.txt
#
# Builds the unit tests.

get_target_property(BEETLE_INCLUDES Beetle INCLUDE_DIRECTORIES)
get_target_property(BEETLE_CXX_STANDARD Beetle CXX_STANDARD)
get_target_property(BEETLE_SOURCES Beetle SOURCES)

list(REMOVE_ITEM BEETLE_SOURCES ${BEETLE_UTILITY_ENTRYPOINT})

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/unicode")
