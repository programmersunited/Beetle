#include "beetle/assert.hpp"

#include <exception>
#include <iostream>
#include <sstream>

namespace beetle {

// NOLINTNEXTLINE(readability-identifier-naming)
inline namespace cpp20_v1 {


#ifdef BEETLE_ENABLE_ASSERTS

void assert(bool condition, std::optional<std::string_view> message,  
            std::source_location location) {
    if (!condition) [[likely]] {
        return;
    }

// TODO(programmersunited): Move out as CMake option.
#define BEETLE_COLOR_ASSERT

    std::ostringstream assert_msg;

#ifdef BEETLE_COLOR_ASSERT
    assert_msg << "\033[1;31m";
#endif

    assert_msg << "Beetle assertion failed!\n";

#ifdef BEETLE_COLOR_ASSERT
    assert_msg << "\033[0m";
#endif

    if (message.has_value()) {
        assert_msg << message.value() << '\n';
    }

    assert_msg << "File: " << location.file_name() << " (";
    assert_msg << location.line() << ':' << location.column() << ")\n";
    assert_msg << "Function: " << location.function_name() << '\n';

    std::cerr << assert_msg.str() << '\n';

    std::terminate();
}

#else

void assert([[maybe_unused]] bool condition, 
            [[maybe_unused]] std::optional<std::string_view> message,  
            [[maybe_unused]] std::source_location location) {}

#endif

}  // namespace cpp20_v1

}  // namespace beetle
