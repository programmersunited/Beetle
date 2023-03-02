#include "beetle/char/validate.hpp"

#include <algorithm>
#include <functional>
#include <iomanip>
#include <sstream>
#include <string_view>

#include "gtest/gtest.h"

/**
 * Unit tests for beetle/validate.hpp
 *
 * @note Tests all code unit values for leading, invalid leading and
 * continuation bytes since the range is small enough.
 */
namespace {

[[nodiscard]] auto to_hex(char8_t code_unit) -> std::string {
    std::stringstream stream;

    stream << "0x" << std::setfill('0') << std::setw(2);
    stream << std::hex << static_cast<int>(code_unit);

    return stream.str();
}

auto inclusive_range(char8_t first, char8_t last, std::function<void(char8_t)> func) -> void {
    // Avoids overflow
    for (; first < last; ++first) {
        func(first);
    }

    func(last);
}

auto inclusive_ascii_range(std::function<void(char8_t)> func) -> void { inclusive_range(0, 0b0111'1111, func); }

auto inclusive_mb_2_range(std::function<void(char8_t)> func) -> void {
    inclusive_range(0b1100'0000, 0b1101'1111, func);
}

auto inclusive_mb_3_range(std::function<void(char8_t)> func) -> void {
    inclusive_range(0b1110'0000, 0b1110'1111, func);
}

auto inclusive_mb_4_range(std::function<void(char8_t)> func) -> void {
    inclusive_range(0b1111'0000, 0b1111'0111, func);
}

auto inclusive_mb_range(std::function<void(char8_t)> func) -> void { inclusive_range(0b1100'0000, 0b1111'0111, func); }

auto inclusive_invalid_upper_mb_range(std::function<void(char8_t)> func) -> void {
    inclusive_range(0b1111'1000, 0b1111'1111, func);
}

auto inclusive_continuation_byte_range(std::function<void(char8_t)> func) -> void {
    inclusive_range(0b1000'0000, 0b1011'1111, func);
}

auto inclusive_invalid_range(std::function<void(char8_t)> func) -> void { inclusive_invalid_upper_mb_range(func); }

TEST(ValidateCharacter, is_char) {
    using namespace beetle;
    // ASCII 0xxx xxxx are valid UTF-8 characters
    constexpr auto utf8_char = std::u8string_view{u8"$"};
    constexpr bool is_valid = utf8::is_char(utf8_char);

    EXPECT_TRUE(is_valid);
    EXPECT_TRUE(utf8::is_char(utf8_char));
    // Implement overlong encoding detection
}

}  // namespace
