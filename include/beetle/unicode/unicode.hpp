/*
 * MIT License
 *
 * Copyright (c) 2022 programmersunited
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef BEETLE_UNICODE_UNICODE_HPP
#define BEETLE_UNICODE_UNICODE_HPP

#include <compare>
#include <concepts>
#include <cstdint>
#include <iomanip>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>

namespace beetle {

namespace unicode {

class CodePoint;

}  // namespace unicode

inline namespace literals {

inline namespace unicode_literals {

/**
 * User-defined literal for constructing an Unicode code point using the given value.
 *
 * @param code_point The Unicode code point
 *
 * @throws std::invalid_argument if the given value is too large to be a Unicode code point
 *
 * @return The constructed Unicode value
 */
[[nodiscard]] constexpr unicode::CodePoint operator""_U(unsigned long long int code_point);

}  // namespace unicode_literals

}  // namespace literals

/**
 * Convert the given Unicode code point to the given integer type.
 *
 * @tparam IntegerType The integer type to convert to
 *
 * @param value The Unicode value to convert
 *
 * @return The Unicode value as the given integer type
 */
template <typename IntegerType>
    requires std::integral<IntegerType>
[[nodiscard]] constexpr IntegerType to_integer(unicode::CodePoint code_point) noexcept;

namespace unicode {

/**
 * An Unicode Exception.
 */
class Exception : public std::exception {
   public:
    /**
     * Construct an Unicode exception using the given unsigned value.
     *
     * @param value The unsigned value for the exception message
     */
    Exception(std::unsigned_integral auto code_point) {
        // TODO: Change to std::format when supported by more compilers
        std::stringstream stream;
        stream << "Unicode value is too large: U+" << std::hex << code_point;

        this->m_msg = stream.str();
    }

    ~Exception() override = default;

    /**
     * Return the error message for this exception.
     *
     * @return The error message
     */
    [[nodiscard]] char const* what() const noexcept override { return this->m_msg.c_str(); }

   private:
    /**
     * The error message for this Unicode exception.
     */
    std::string m_msg;
};

namespace {

/**
 * Tag type to not validate Unicode code point.
 */
struct NoUnicodeValidation {};

}  // namespace

/**
 * A representation of an Unicode code point.
 *
 * @note This will throw on construction if the given value is over the valid Unicode code point max value.
 */
class CodePoint {
   public:
    /**
     * The underlying integer type for the Unicode code point.
     */
    using value_type = std::uint32_t;

    /**
     * The minimum Unicode code point value.
     */
    static inline constexpr value_type min_value{0x0U};

    /**
     * The maximum Unicode code point value.
     */
    static inline constexpr value_type max_value{0x10FFFFU};

    /**
     * Default constructor.
     */
    constexpr CodePoint() noexcept = default;

    /**
     * Construct Unicode code point using the given value.
     *
     * @param value The Unicode code point's value
     *
     * @throws unicode::Exception if the given value is too large to be a Unicode code point
     */
    constexpr CodePoint(value_type value) : m_data{validate(value)} {}

    [[nodiscard]] constexpr auto operator<=>(CodePoint const&) const noexcept = default;

    /**
     * Assign the given value to this Unicode code point.
     *
     * @param other The new value for this Unicode code point
     *
     * @return A reference to this Unicode code point
     *
     * @throws unicode::Exception if the given value is too large to be an Unicode code point
     */
    constexpr CodePoint& operator=(value_type other) {
        this->m_data = CodePoint::validate(other);

        return *this;
    }

    /**
     * Construct an Unicode code point using the given value.
     *
     * @param code_point The value for the Unicode code point
     *
     * @return The Unicode code point if the given value is valid, otherwise std::nullopt.
     */
    [[nodiscard]] static constexpr std::optional<CodePoint> create(value_type code_point) noexcept {
        if (CodePoint::is_valid(code_point)) {
            return CodePoint{NoUnicodeValidation{}, code_point};
        }

        return std::nullopt;
    }

    /**
     * Check if the given value is a valid Unicode code point.
     *
     * @param code_point The Unicode code point to check
     *
     * @return true if valid, otherwise false.
     */
    [[nodiscard]] static constexpr bool is_valid(std::unsigned_integral auto code_point) noexcept {
        return code_point <= CodePoint::max_value;
    }

    template <typename IntegerType>
        requires std::integral<IntegerType>
    friend constexpr IntegerType beetle::to_integer(unicode::CodePoint code_point) noexcept;

    friend constexpr CodePoint literals::unicode_literals::operator""_U(unsigned long long int code_point);

   private:
    /**
     * The underlying integer representation of the Unicode code point.
     *
     * @note The default constructed value is 0.
     */
    value_type m_data{0};

    /**
     * Constructs an Unicode code point using the given value.
     *
     * @param value The value for Unicode code point
     *
     * @note This is used to bypass the validation check.
     */
    constexpr CodePoint(NoUnicodeValidation, value_type value) : m_data{value} {}

    /**
     * Validate if the given value is a valid Unicode code point.
     *
     * @param value The value to validate
     *
     * @return The given value
     *
     * @throws std::invalid_argument if the given value is not a valid Unicode code point, otherwise true.
     */
    [[nodiscard]] static constexpr value_type validate(value_type value) {
        return CodePoint::is_valid(value) ? value : throw unicode::Exception{value};
    }
};

namespace code_points {

/**
 * Unicode replacement character.
 *
 * @note It is common practice to replace an invalid Unicode code point with the replacement character.
 */
constexpr CodePoint replacement_character{0xFFFDU};

/**
 * The minimum Unicode code point.
 */
constexpr CodePoint min{CodePoint::min_value};

/**
 * The maximum Unicode code point.
 */
constexpr CodePoint max{CodePoint::max_value};

}  // namespace code_points

/**
 * Give access to Unicode literals when doing `using namespace beetle::unicode`
 */
using namespace literals::unicode_literals;

}  // namespace unicode

inline namespace literals {

inline namespace unicode_literals {

[[nodiscard]] constexpr unicode::CodePoint operator""_U(unsigned long long int code_point) {
    if (unicode::CodePoint::is_valid(code_point)) {
        return unicode::CodePoint{unicode::NoUnicodeValidation{},
                                  static_cast<unicode::CodePoint::value_type>(code_point)};
    }

    throw unicode::Exception{code_point};
}

}  // namespace unicode_literals

}  // namespace literals

template <typename IntegerType>
    requires std::integral<IntegerType>
[[nodiscard]] constexpr IntegerType to_integer(unicode::CodePoint code_point) noexcept {
    return IntegerType(code_point.m_data);
}

/**
 * Convert the given Unicode code point to its underlying integer type.
 *
 * @param value The Unicode code point to convert
 *
 * @return The Unicode code point as its underlying integer type
 */
[[nodiscard]] constexpr auto to_integer(unicode::CodePoint code_point) noexcept {
    return beetle::to_integer<unicode::CodePoint::value_type>(code_point);
}

}  // namespace beetle

constexpr void test() {
    // using namespace beetle::literals;
    // using namespace beetle::unicode_literals;
    // using namespace beetle::literals::unicode_literals;
    using namespace beetle::unicode;

    if (0x11_U < 0x1234_U) {
    }
}

#endif
