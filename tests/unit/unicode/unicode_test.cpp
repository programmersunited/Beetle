#include "beetle/unicode/unicode.hpp"

#include <stdexcept>

#include "gtest/gtest.h"

/**
 * Unit tests for beetle/unicode/unicode.hpp
 */
namespace {

TEST(UnicodeTest, construction) {
    using namespace beetle::unicode;

    EXPECT_NO_THROW({ CodePoint{CodePoint::max_value}; });
    EXPECT_NO_THROW({ CodePoint{CodePoint::max_value - 1}; });

    EXPECT_THROW({ CodePoint{CodePoint::max_value + 1}; }, beetle::unicode::Exception);

    EXPECT_EQ(CodePoint{}, CodePoint{0});
    EXPECT_EQ(beetle::to_integer(CodePoint{0x123U}), 0x123U);
}

TEST(UnicodeTest, is_valid) {
    using namespace beetle::unicode;

    EXPECT_TRUE(CodePoint::is_valid(0x00U));
    EXPECT_TRUE(CodePoint::is_valid(CodePoint::max_value));
    EXPECT_TRUE(CodePoint::is_valid(CodePoint::max_value - 1));

    EXPECT_FALSE(CodePoint::is_valid(CodePoint::max_value + 1));
}

TEST(UnicodeTest, literal) {
    using namespace beetle::unicode;

    EXPECT_NO_THROW({ auto const value = 0x00_U; });
    EXPECT_NO_THROW({ auto const value = 0x10FFFE_U; });
    EXPECT_NO_THROW({ auto const value = 0x10FFFF_U; });

    EXPECT_THROW({ auto const value = 0x110000_U; }, beetle::unicode::Exception);
}

TEST(UnicodeTest, create) {
    using namespace beetle::unicode;

    EXPECT_NE(CodePoint::create(CodePoint::max_value - 1), std::nullopt);
    EXPECT_NE(CodePoint::create(CodePoint::max_value), std::nullopt);

    EXPECT_EQ(CodePoint::create(CodePoint::max_value + 1), std::nullopt);
}

}  // namespace
