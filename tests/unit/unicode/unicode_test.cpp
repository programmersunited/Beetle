#include "beetle/unicode/unicode.hpp"

#include <stdexcept>

#include "gtest/gtest.h"

/**
 * Unit tests for beetle/unicode/unicode.hpp
 */
namespace {

constexpr auto max_unicode_value = 0x10FFFFU;

TEST(UnicodeTest, construction) {
    using namespace beetle;

    EXPECT_NO_THROW({ Unicode{max_unicode_value}; });
    EXPECT_NO_THROW({ Unicode{max_unicode_value - 1}; });

    EXPECT_THROW({ Unicode{max_unicode_value + 1}; }, Unicode::Exception);

    EXPECT_EQ(Unicode{}, Unicode{0});
    EXPECT_EQ(to_integer(Unicode{0x123U}), 0x123U);
}

TEST(UnicodeTest, is_valid) {
    using namespace beetle;

    EXPECT_TRUE(Unicode::is_valid(0x00U));
    EXPECT_TRUE(Unicode::is_valid(max_unicode_value));
    EXPECT_TRUE(Unicode::is_valid(max_unicode_value - 1));

    EXPECT_FALSE(Unicode::is_valid(max_unicode_value + 1));
}

TEST(UnicodeTest, literal) {
    using namespace beetle;

    EXPECT_NO_THROW({ auto const value = 0x00_U; });
    EXPECT_NO_THROW({ auto const value = 0x10FFFE_U; });
    EXPECT_NO_THROW({ auto const value = 0x10FFFF_U; });

    EXPECT_THROW({ auto const value = 0x110000_U; }, Unicode::Exception);
}

TEST(UnicodeTest, create) {
    using namespace beetle;

    EXPECT_NE(Unicode::create(max_unicode_value - 1), std::nullopt);
    EXPECT_NE(Unicode::create(max_unicode_value), std::nullopt);

    EXPECT_EQ(Unicode::create(max_unicode_value + 1), std::nullopt);
}

}  // namespace
