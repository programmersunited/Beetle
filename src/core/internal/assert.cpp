#include "core/internal/assert.hpp"

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <optional>
#include <source_location>
#include <sstream>
#include <string>
#include <string_view>

namespace beetle::internal {

// NOLINTNEXTLINE(readability-identifier-naming)
inline namespace cpp20_v1 {

std::string create_error_message(std::string_view assertion_text, std::optional<std::string_view> message,
                                 std::source_location const location) {
    std::ostringstream assert_msg;
    assert_msg << "Beetle assertion failed!\n";
    assert_msg << "  Assertion:\t" << assertion_text << '\n';

    if (message.has_value()) {
        assert_msg << "  Message:\t" << message.value() << '\n';
    }

    assert_msg << "  File:\t\t" << location.file_name() << " (";
    assert_msg << location.line() << ':' << location.column() << ")\n";
    assert_msg << "  Function:\t" << location.function_name();

    return assert_msg.str();
}

assertion_failure::assertion_failure(std::string_view assertion_text, std::optional<std::string_view> message,
                                     std::source_location const location) {
    auto const error_message = create_error_message(assertion_text, message, location);
    std::printf("%s\n", error_message.c_str());

    std::quick_exit(1);
}

}  // namespace cpp20_v1

}  // namespace beetle::internal
