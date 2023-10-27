// clang-format off

#include "beetle/unicode/code_point.hpp"

// clang-format on

#include <format>
#include <ostream>
#include <string>

namespace beetle {

auto to_string(unicode::CodePoint code_point) -> std::string { return std::format("{}", code_point); }

}  // namespace beetle

auto operator<<(std::ostream& stream, beetle::unicode::CodePoint code_point) -> std::ostream& {
    stream << beetle::to_string(code_point);

    return stream;
}
