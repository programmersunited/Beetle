#include "gtest/gtest.h"

#include <algorithm>
#include <functional>
#include <string_view>
#include <iomanip>
#include <sstream>

#include "beetle/code_unit.hpp"
#include "beetle/code_unit/exception.hpp"

/**
 * Unit tests for beetle/code_unit.hpp
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

auto inclusive_ascii_range(std::function<void(char8_t)> func) -> void {
    inclusive_range(0, 0b0111'1111, func);
}

auto inclusive_mb_2_range(std::function<void(char8_t)> func) -> void {
    inclusive_range(0b1100'0000, 0b1101'1111, func);
}

auto inclusive_mb_3_range(std::function<void(char8_t)> func) -> void {
    inclusive_range(0b1110'0000, 0b1110'1111, func);
}

auto inclusive_mb_4_range(std::function<void(char8_t)> func) -> void {
    inclusive_range(0b1111'0000, 0b1111'0111, func);
}

auto inclusive_mb_range(std::function<void(char8_t)> func) -> void {
    inclusive_range(0b1100'0000, 0b1111'0111, func);
}

auto inclusive_invalid_upper_mb_range(std::function<void(char8_t)> func) -> void {
    inclusive_range(0b1111'1000, 0b1111'1111, func);
}

auto inclusive_continuation_byte_range(std::function<void(char8_t)> func) -> void {
    inclusive_range(0b1000'0000, 0b1011'1111, func);
}

auto inclusive_invalid_range(std::function<void(char8_t)> func) -> void {
    inclusive_invalid_upper_mb_range(func);
}

TEST(CodeUnit, globals) {
    using namespace beetle;

    EXPECT_EQ(utf8::g_firstLeadingByte, 0);
    EXPECT_EQ(utf8::g_lastLeadingByte, 0b1111'0111);
}

TEST(CodeUnit, is_ascii) {
    using namespace beetle;

    inclusive_ascii_range([](char8_t code_unit) -> void {
        EXPECT_TRUE(utf8::is_ascii(code_unit)) 
                << std::quoted(to_hex(code_unit)) << " Is supposed to be ASCII";
    });

    inclusive_continuation_byte_range([](char8_t code_unit) -> void {
        EXPECT_FALSE(utf8::is_ascii(code_unit)) 
                << std::quoted(to_hex(code_unit)) << " Is NOT supposed to be ASCII";
    });

    inclusive_mb_range([](char8_t code_unit) -> void {
        EXPECT_FALSE(utf8::is_ascii(code_unit)) 
            << std::quoted(to_hex(code_unit)) << " Is NOT supposed to be ASCII";
    });

    inclusive_invalid_range([](char8_t code_unit) -> void {
        EXPECT_FALSE(utf8::is_ascii(code_unit)) 
            << std::quoted(to_hex(code_unit)) << " Is NOT supposed to be ASCII.";
    });
}

TEST(CodeUnit, is_not_ascii) {
    using namespace beetle;

    inclusive_ascii_range([](char8_t code_unit) -> void {
        EXPECT_FALSE(utf8::is_not_ascii(code_unit)) 
                << std::quoted(to_hex(code_unit)) << " Is supposed to be ASCII";
    });

    inclusive_continuation_byte_range([](char8_t code_unit) -> void {
        EXPECT_TRUE(utf8::is_not_ascii(code_unit)) 
                << std::quoted(to_hex(code_unit)) << " Is NOT supposed to be ASCII";
    });

    inclusive_mb_range([](char8_t code_unit) -> void {
        EXPECT_TRUE(utf8::is_not_ascii(code_unit)) 
            << std::quoted(to_hex(code_unit)) << " Is NOT supposed to be ASCII";
    });

    inclusive_invalid_range([](char8_t code_unit) -> void {
        EXPECT_TRUE(utf8::is_not_ascii(code_unit)) 
            << std::quoted(to_hex(code_unit)) << " Is NOT supposed to be ASCII.";
    });
}

TEST(CodeUnit, is_leading_byte) {
    using namespace beetle;

    inclusive_ascii_range([](char8_t code_unit) -> void {
        EXPECT_TRUE(utf8::is_leading_byte(code_unit)) 
                << std::quoted(to_hex(code_unit)) << " Is supposed to be a leading byte.";
    });

    inclusive_continuation_byte_range([](char8_t code_unit) -> void {
        EXPECT_FALSE(utf8::is_leading_byte(code_unit)) 
                << std::quoted(to_hex(code_unit)) << " Is NOT supposed to be a leading byte.";
    });

    inclusive_mb_range([](char8_t code_unit) -> void {
        EXPECT_TRUE(utf8::is_leading_byte(code_unit)) 
            << std::quoted(to_hex(code_unit)) << " Is supposed to be a leading byte.";
    });

    inclusive_invalid_range([](char8_t code_unit) -> void {
        EXPECT_FALSE(utf8::is_leading_byte(code_unit)) 
            << std::quoted(to_hex(code_unit)) << " Is NOT supposed to be a leading byte.";
    });
}

TEST(CodeUnit, is_leading_multiple_bytes) {
    using namespace beetle;

    inclusive_ascii_range([](char8_t code_unit) -> void {
        EXPECT_FALSE(utf8::is_leading_multiple_bytes(code_unit)) 
                << std::quoted(to_hex(code_unit)) << " Is NOT supposed to be a leading mb.";
    });

    inclusive_continuation_byte_range([](char8_t code_unit) -> void {
        EXPECT_FALSE(utf8::is_leading_multiple_bytes(code_unit)) 
                << std::quoted(to_hex(code_unit)) << " Is NOT supposed to be a leading mb.";
    });

    inclusive_mb_range([](char8_t code_unit) -> void {
        EXPECT_TRUE(utf8::is_leading_multiple_bytes(code_unit)) 
            << std::quoted(to_hex(code_unit)) << " Is supposed to be a leading mb.";
    });

    inclusive_invalid_range([](char8_t code_unit) -> void {
        EXPECT_FALSE(utf8::is_leading_multiple_bytes(code_unit)) 
            << std::quoted(to_hex(code_unit)) << " Is NOT supposed to be a leading mb.";
    });
}

TEST(CodeUnit, is_continuation_byte) {
    using namespace beetle;

    inclusive_ascii_range([](char8_t code_unit) -> void {
        EXPECT_FALSE(utf8::is_continuation_byte(code_unit)) 
                << std::quoted(to_hex(code_unit)) << " Is NOT supposed to be a continuation byte.";
    });

    inclusive_continuation_byte_range([](char8_t code_unit) -> void {
        EXPECT_TRUE(utf8::is_continuation_byte(code_unit)) 
                << std::quoted(to_hex(code_unit)) << " Is supposed to be a continuation byte.";
    });

    inclusive_mb_range([](char8_t code_unit) -> void {
        EXPECT_FALSE(utf8::is_continuation_byte(code_unit)) 
            << std::quoted(to_hex(code_unit)) << " Is NOT supposed to be a continuation byte.";
    });

    inclusive_invalid_range([](char8_t code_unit) -> void {
        EXPECT_FALSE(utf8::is_continuation_byte(code_unit)) 
            << std::quoted(to_hex(code_unit)) << " Is NOT supposed to be a continuation byte.";
    });
}

TEST(CodeUnit, is_valid_byte) {
    using namespace beetle;

    inclusive_ascii_range([](char8_t code_unit) -> void {
        EXPECT_TRUE(utf8::is_valid_byte(code_unit)) 
                << std::quoted(to_hex(code_unit)) << " Is supposed to be a valid byte.";
    });

    inclusive_continuation_byte_range([](char8_t code_unit) -> void {
        EXPECT_TRUE(utf8::is_valid_byte(code_unit)) 
                << std::quoted(to_hex(code_unit)) << " Is supposed to be a valid byte.";
    });

    inclusive_mb_range([](char8_t code_unit) -> void {
        EXPECT_TRUE(utf8::is_valid_byte(code_unit)) 
            << std::quoted(to_hex(code_unit)) << " Is supposed to be a valid byte.";
    });

    inclusive_invalid_range([](char8_t code_unit) -> void {
        EXPECT_FALSE(utf8::is_valid_byte(code_unit)) 
            << std::quoted(to_hex(code_unit)) << " Is NOT supposed to be a valid byte.";
    });
}

TEST(CodeUnit, is_invalid_byte) {
    using namespace beetle;

    inclusive_ascii_range([](char8_t code_unit) -> void {
        EXPECT_FALSE(utf8::is_invalid_byte(code_unit)) 
                << std::quoted(to_hex(code_unit)) << " Is NOT supposed to be an invalid byte.";
    });

    inclusive_continuation_byte_range([](char8_t code_unit) -> void {
        EXPECT_FALSE(utf8::is_invalid_byte(code_unit)) 
                << std::quoted(to_hex(code_unit)) << " Is NOT supposed to be an invalid byte.";
    });

    inclusive_mb_range([](char8_t code_unit) -> void {
        EXPECT_FALSE(utf8::is_invalid_byte(code_unit)) 
            << std::quoted(to_hex(code_unit)) << " Is NOT supposed to be an invalid byte.";
    });

    inclusive_invalid_range([](char8_t code_unit) -> void {
        EXPECT_TRUE(utf8::is_invalid_byte(code_unit)) 
            << std::quoted(to_hex(code_unit)) << " Is supposed to be an invalid byte.";
    });
}

TEST(CodeUnit, leading_byte_size) {
/*
    using namespace beetle;

    inclusive_ascii_range([](char8_t code_unit) -> void {
        EXPECT_EQ(utf8::leading_byte_size(code_unit), 1) 
                << std::quoted(to_hex(code_unit)) << " Size is supposed to be 1.";
    });

    inclusive_continuation_byte_range([](char8_t code_unit) -> void {
        EXPECT_THROW({
          auto tmp = utf8::leading_byte_size(code_unit);
        }, utf8::ExpectingLeadingByte) << std::quoted(to_hex(code_unit)) 
                                       << " Expecting throw.";;
    });

    // Byte 2
    EXPECT_NO_THROW(EXPECT_EQ(utf8::leading_byte_size(0b1100'0000), 2)); // First
    EXPECT_NO_THROW(EXPECT_EQ(utf8::leading_byte_size(0b1101'1111), 2)); // Last

    inclusive_mb_2_range([](char8_t code_unit) -> void {
        EXPECT_EQ(utf8::leading_byte_size(code_unit), 2) 
            << std::quoted(to_hex(code_unit)) << " Size is supposed to be 2.";
    });

    // Byte 3
    EXPECT_NO_THROW(EXPECT_EQ(utf8::leading_byte_size(0b1110'0000), 3)); // First
    EXPECT_NO_THROW(EXPECT_EQ(utf8::leading_byte_size(0b1110'1111), 3)); // Last

    inclusive_mb_3_range([](char8_t code_unit) -> void {
        EXPECT_EQ(utf8::leading_byte_size(code_unit), 3) 
            << std::quoted(to_hex(code_unit)) << " Size is supposed to be 3.";
    });

    // Byte 4
    EXPECT_NO_THROW(EXPECT_EQ(utf8::leading_byte_size(0b1111'0000), 4)); // First
    EXPECT_NO_THROW(EXPECT_EQ(utf8::leading_byte_size(0b1111'0111), 4)); // Last

    inclusive_mb_4_range([](char8_t code_unit) -> void {
        EXPECT_EQ(utf8::leading_byte_size(code_unit), 4) 
            << std::quoted(to_hex(code_unit)) << " Size is supposed to be 4.";
    });

    inclusive_invalid_range([](char8_t code_unit) -> void {
        EXPECT_THROW({
          auto tmp = utf8::leading_byte_size(code_unit);
        }, utf8::ExpectingLeadingByte)   << std::quoted(to_hex(code_unit)) 
                                    << " Expecting throw.";;
    });
*/
}

TEST(CodeUnit, peek_char_size) {
    using namespace beetle;

    inclusive_ascii_range([](char8_t code_unit) -> void {
        EXPECT_EQ(utf8::peek_char_size(code_unit).value(), 1) 
                << std::quoted(to_hex(code_unit)) << " Size is supposed to be 1.";
    });

    inclusive_continuation_byte_range([](char8_t code_unit) -> void {
        EXPECT_FALSE(utf8::peek_char_size(code_unit).has_value()) 
            << std::quoted(to_hex(code_unit)) << " Size should not be returned.";
    });

    // Byte 2
    EXPECT_NO_THROW(EXPECT_EQ(utf8::peek_char_size(0b1100'0000), 2)); // First
    EXPECT_NO_THROW(EXPECT_EQ(utf8::peek_char_size(0b1101'1111), 2)); // Last

    inclusive_mb_2_range([](char8_t code_unit) -> void {
        EXPECT_EQ(utf8::peek_char_size(code_unit).value(), 2) 
            << std::quoted(to_hex(code_unit)) << " Size is supposed to be 2.";
    });

    // Byte 3
    EXPECT_NO_THROW(EXPECT_EQ(utf8::peek_char_size(0b1110'0000), 3)); // First
    EXPECT_NO_THROW(EXPECT_EQ(utf8::peek_char_size(0b1110'1111), 3)); // Last

    inclusive_mb_3_range([](char8_t code_unit) -> void {
        EXPECT_EQ(utf8::peek_char_size(code_unit).value(), 3) 
            << std::quoted(to_hex(code_unit)) << " Size is supposed to be 3.";
    });

    // Byte 4
    EXPECT_NO_THROW(EXPECT_EQ(utf8::peek_char_size(0b1111'0000), 4)); // First
    EXPECT_NO_THROW(EXPECT_EQ(utf8::peek_char_size(0b1111'0111), 4)); // Last

    inclusive_mb_4_range([](char8_t code_unit) -> void {
        EXPECT_EQ(utf8::peek_char_size(code_unit).value(), 4) 
            << std::quoted(to_hex(code_unit)) << " Size is supposed to be 4.";
    });

    inclusive_invalid_range([](char8_t code_unit) -> void {
        EXPECT_FALSE(utf8::peek_char_size(code_unit).has_value()) 
            << std::quoted(to_hex(code_unit)) << " Size should not be returned.";
    });
}

}  // namespace
