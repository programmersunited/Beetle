#ifndef BEETLE_MACRO_HELPERS_HPP
#define BEETLE_MACRO_HELPERS_HPP

// NOLINTBEGIN(cppcoreguidelines-macro-usage)

// Make into string literal
#define BEETLE_STR(x) #x

// Concatenate two arguments together
#define BEETLE_CAT(A, B) A##B

// Creates NAME_NUM output
#define BEETLE_SELECT(NAME, NUM) BEETLE_CAT(NAME##_, NUM)

// Binds NAME to the given ARGS
#define BEETLE_BIND(NAME, ARGS) NAME ARGS

// BEETLE_VA_SIZE helper to get __VA_ARGS__ count
#define BEETLE_GET_COUNT(_1, _2, _3, _4, _5, _6, COUNT, ...) COUNT

// Gets the number of arguments from __VA_ARGS__
#define BEETLE_VA_SIZE(...) BEETLE_GET_COUNT(__VA_ARGS__, 6, 5, 4, 3, 2, 1, -1)

#define BEETLE_VA_SELECT(NAME, ...)                \
    BEETLE_SELECT(NAME, BEETLE_VA_SIZE(__VA_ARGS__)) \
    (__VA_ARGS__)

// NOLINTEND(cppcoreguidelines-macro-usage)

#endif

