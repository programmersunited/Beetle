#include "beetle/utf8/algorithm.hpp"

#include <array>

#include "gtest/gtest.h"

/**
 * Unit tests for beetle/unicode/unicode.hpp
 */
namespace {

constexpr std::array<char8_t, 4> overlong_encoded = {0xF0, 0x82, 0x82, 0xAC};

constexpr std::array<char8_t, 1> dollar_sign = {0x24};

constexpr std::array<char8_t, 2> pound_sign = {0xC2, 0xA3};

constexpr std::array<char8_t, 2> cyrillic_letter = {0xD0, 0x98};

constexpr std::array<char8_t, 3> devanagari = {0xE0, 0xA4, 0xB9};

constexpr std::array<char8_t, 3> euro_sign = {0xE2, 0x82, 0xAC};

constexpr std::array<char8_t, 3> hangul_syllables = {0xED, 0x95, 0x9C};

constexpr std::array<char8_t, 4> hwair = {0xF0, 0x90, 0x8D, 0x88};

TEST(AlgorithmTest, is_valid) {
    using namespace beetle;

    EXPECT_FALSE(utf8::is_valid(overlong_encoded));
    EXPECT_FALSE(utf8::is_valid(std::begin(pound_sign), std::prev(std::end(pound_sign))));
    EXPECT_FALSE(utf8::is_valid(std::begin(cyrillic_letter), std::prev(std::end(cyrillic_letter))));
    EXPECT_FALSE(utf8::is_valid(std::begin(devanagari), std::prev(std::end(devanagari))));
    EXPECT_FALSE(utf8::is_valid(std::begin(hangul_syllables), std::prev(std::end(hangul_syllables))));
    EXPECT_FALSE(utf8::is_valid(std::begin(hwair), std::prev(std::end(hwair))));
    EXPECT_FALSE(utf8::is_valid(std::next(std::begin(pound_sign)), std::end(pound_sign)));
    EXPECT_FALSE(utf8::is_valid(std::next(std::begin(cyrillic_letter)), std::end(cyrillic_letter)));
    EXPECT_FALSE(utf8::is_valid(std::next(std::begin(devanagari)), std::end(devanagari)));
    EXPECT_FALSE(utf8::is_valid(std::next(std::begin(hangul_syllables)), std::end(hangul_syllables)));
    EXPECT_FALSE(utf8::is_valid(std::next(std::begin(hwair)), std::end(hwair)));

    EXPECT_TRUE(utf8::is_valid(dollar_sign));
    EXPECT_TRUE(utf8::is_valid(pound_sign));
    EXPECT_TRUE(utf8::is_valid(cyrillic_letter));
    EXPECT_TRUE(utf8::is_valid(devanagari));
    EXPECT_TRUE(utf8::is_valid(euro_sign));
    EXPECT_TRUE(utf8::is_valid(hangul_syllables));
    EXPECT_TRUE(utf8::is_valid(hwair));
}

}  // namespace
