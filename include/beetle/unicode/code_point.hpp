/*
 * MIT License
 *
 * Copyright (c) 2023 programmersunited
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

#ifndef BEETLE_UNICODE_CODE_POINT_HPP
#define BEETLE_UNICODE_CODE_POINT_HPP

#include <compare>
#include <concepts>
#include <cstdint>
#include <expected>
#include <format>
#include <iterator>
#include <stdexcept>
#include <string>

#include "beetle/unicode/error.hpp"
#include "beetle/unicode/tags.hpp"
#include "core/internal/assert.hpp"

/**
 * Contains the Unicode code point with its helpers and literals.
 *
 * @file code_point.hpp
 */

namespace beetle {

namespace unicode {

// Forward declare for literals
class CodePoint;

}  // namespace unicode

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
[[nodiscard]] constexpr auto to_integer(unicode::CodePoint code_point) noexcept -> IntegerType;

inline namespace literals {

inline namespace unicode_literals {

/**
 * User-defined literal for constructing an Unicode code point using the given value.
 *
 * @param code_point The Unicode code point
 *
 * @return The constructed Unicode code point
 */
[[nodiscard]] consteval auto operator""_U(unsigned long long int code_point) -> unicode::CodePoint;

}  // namespace unicode_literals

}  // namespace literals

namespace unicode {

/**
 * Check if the given value is larger than the maximum Unicode code point.
 *
 * @param code_point The Unicode code point
 *
 * @return True if the value is out of range, otherwise false
 */
[[nodiscard]] constexpr auto is_out_of_range(std::unsigned_integral auto code_point) noexcept -> bool {
    return code_point > 0x10FFFFU;
}

/**
 * Check if the given value is a Unicode surrogate.
 *
 * @note RFC 3629 contraints Unicode code points to not include high and low surrogates to match UTF-16
 *
 * @param code_point The Unicode code point
 *
 * @return True if the value is a surrogate, otherwise false
 */
[[nodiscard]] constexpr auto is_surrogate(std::unsigned_integral auto code_point) noexcept -> bool {
    constexpr auto high_surrogate = 0xD800U;
    constexpr auto low_surrogate = 0xDFFFU;

    return code_point >= high_surrogate && code_point <= low_surrogate;
}

/**
 * Check if the given value is a valid Unicode code point.
 *
 * @param code_point The Unicode code point to check
 *
 * @return True if valid, otherwise false
 */
[[nodiscard]] constexpr auto is_code_point(std::unsigned_integral auto code_point) noexcept -> bool {
    return !unicode::is_out_of_range(code_point) && !unicode::is_surrogate(code_point);
}

/**
 * A representation of an Unicode code point.
 */
class CodePoint final {
   public:
    /**
     * The underlying integer type for the Unicode code point.
     */
    using value_type = char32_t;

    /**
     * Default constructor.
     */
    constexpr CodePoint() noexcept = default;

    /**
     * Constructs an Unicode code point using the given value.
     *
     * @note This is used to bypass the validation check.
     *
     * @param value The value for Unicode code point
     */
    constexpr explicit CodePoint(unsafe::no_validation_t, value_type value) noexcept : m_data{value} {
        BEETLE_ASSERT(unicode::is_code_point(value), "The given value is not a valid Unicode code point.");
    }

    [[nodiscard]] constexpr auto operator<=>(CodePoint const&) const noexcept = default;

    /**
     * Implicit conversion operator for the underlying integer type of a Unicode code point.
     *
     * @see unicode::CodePoint::value_type
     *
     * @return This Unicode code point value
     */
    constexpr operator value_type() const noexcept { return this->m_data; }

    /**
     * Return the maximum Unicode code point value.
     *
     * @return The maximum Unicode code point
     */
    [[nodiscard]] static constexpr auto max() noexcept -> CodePoint {
        return CodePoint{unsafe::no_validation, 0x10FFFFU};
    }

    /**
     * Return the minimum Unicode code point value.
     *
     * @return The minimum Unicode code point
     */
    [[nodiscard]] static constexpr auto min() noexcept -> CodePoint { return CodePoint{}; }

    friend consteval auto literals::unicode_literals::operator""_U(unsigned long long int code_point) -> CodePoint;

   private:
    /**
     * The underlying integer representation of the Unicode code point.
     *
     * @note The default constructed value is 0.
     */
    value_type m_data{};
};

[[nodiscard]] constexpr auto operator<=>(CodePoint code_point, std::unsigned_integral auto value) noexcept {
    return static_cast<CodePoint::value_type>(code_point) <=> value;
}

[[nodiscard]] constexpr auto operator<=>(std::unsigned_integral auto value, CodePoint code_point) noexcept {
    return value <=> static_cast<CodePoint::value_type>(code_point);
}

[[nodiscard]] constexpr auto operator==(CodePoint code_point, std::unsigned_integral auto value) noexcept -> bool {
    return static_cast<CodePoint::value_type>(code_point) == value;
}

[[nodiscard]] constexpr auto operator==(std::unsigned_integral auto value, CodePoint code_point) noexcept -> bool {
    return code_point == value;
}

namespace code_points {

/**
 * Unicode replacement character.
 *
 * @note It is common practice to replace an invalid Unicode code point with the replacement character.
 */
inline constexpr CodePoint replacement_character{unsafe::no_validation, 0xFFFDU};

}  // namespace code_points

[[nodiscard]] constexpr auto validate(std::unsigned_integral auto code_point) -> unicode::ErrorCode {
    if (unicode::is_surrogate(code_point)) {
        return unicode::ErrorCode{unicode::Error::eSurrogate};
    } else if (unicode::is_out_of_range(code_point)) {
        return unicode::ErrorCode{unicode::Error::eOutOfRange};
    }

    return unicode::ErrorCode{};
}

/**
 * Construct an Unicode code point using the given value.
 *
 * @see CodePoint::Error
 *
 * @param code_point The value for the Unicode code point
 *
 * @return The Unicode code point if the given value is valid, otherwise Error enumeration value.
 */
[[nodiscard]] constexpr auto make_code_point(std::unsigned_integral auto code_point) noexcept
    -> std::expected<CodePoint, unicode::Error> {
    auto const error_code = unicode::validate(code_point);

    if (error_code) {
        return std::unexpected{error_code};
    }

    return CodePoint{unsafe::no_validation, static_cast<CodePoint::value_type>(code_point)};
}

/**
 * Construct an Unicode code point using the given value.
 *
 * @param code_point        The value for the Unicode code point
 * @param sanitize_value    The value to use if the given code point is not valid
 *
 * @return The Unicode code point if the given value is valid, otherwise Error enumeration value.
 */
[[nodiscard]] constexpr auto make_code_point(std::unsigned_integral auto code_point, CodePoint sanitize_value) noexcept
    -> CodePoint {
    if (unicode::is_code_point(code_point)) {
        return CodePoint{unsafe::no_validation, static_cast<CodePoint::value_type>(code_point)};
    }

    return sanitize_value;
}

/**
 * Transcode the given Unicode code point to a different UTF-N format.
 *
 * @tparam Output Has to be weakly incrementable and has to be UTF-8 or UTF-16.
 *
 * @param code_point    The Unicode code point to transcode
 * @param result        The beginning of the UTF-N destination range
 *
 * @return The output iterator to past-the-end element of the last transcoded code unit
 */
template <std::weakly_incrementable Output>
    requires std::same_as<std::iter_value_t<Output>, char8_t> || std::same_as<std::iter_value_t<Output>, char16_t>
constexpr auto transcode(CodePoint code_point, Output result) -> Output {
    auto const raw_value = CodePoint::value_type{code_point};

    if constexpr (std::same_as<std::iter_value_t<Output>, char8_t>) {
        if (code_point <= 0x7FU) {
            // 0xxx xxxx
            *result = static_cast<char8_t>(raw_value);
            ++result;
        } else if (code_point <= 0x07FFU) {
            // 0000 0111 1111 1111
            //       xxx xxyy yyyy
            // 110x xxxx 10yy yyyy
            *result = static_cast<char8_t>((raw_value >> 6) | 0xDFU);
            ++result;
            *result = static_cast<char8_t>(raw_value & 0x3FU) | 0x80U;
            ++result;
        } else if (code_point <= 0xFFFFU) {
            // 1111 1111 1111 1111
            // xxxx yyyy yyzz zzzz
            // 1110 xxxx 10yy yyyy 10zz zzzz
            *result = static_cast<char8_t>((raw_value >> 12) | 0xE0U);
            ++result;
            *result = static_cast<char8_t>(((raw_value >> 6) & 0x3FU) | 0x80U);
            ++result;
            *result = static_cast<char8_t>((raw_value & 0x3FU) | 0x80U);
            ++result;
        } else {  // code_point <= 10FFFF
            // 0000 0000 0001 0000 1111 1111 1111 1111
            //              w wwxx xxxx yyyy yyzz zzzz 1111 0www 10xx xxxx 10yy yyyy 10zz zzzz
            *result = static_cast<char8_t>((raw_value >> 18) | 0xF0U);
            ++result;
            *result = static_cast<char8_t>(((raw_value >> 12) & 0x3FU) | 0x80U);
            ++result;
            *result = static_cast<char8_t>(((raw_value >> 6) & 0x3FU) | 0x80U);
            ++result;
            *result = static_cast<char8_t>((raw_value & 0x3FU) | 0x80U);
            ++result;
        }
    } else {
        if (code_point <= 0xD7FFU || (code_point >= 0xE000U && code_point <= 0xFFFFU)) {
            *result = static_cast<char16_t>(raw_value);
            ++result;
        } else {  // code_point >= 0x010000U && code_point <= 0x10FFFFU
            // yyyy yyyy yyxx xxxx xxxx
            auto const value = raw_value - 0x10000U;

            // 1101 10yy yyyy yyyy -> 0xD800 - 0xDBFF
            *result = 0xD800U + (value >> 10);
            ++result;

            // 1101 11xx xxxx xxxx -> 0xDC00 - 0xDFFF
            *result = 0xDC00U + (0x03FFU & value);
            ++result;
        }
    }

    return result;
}

/**
 * Give access to Unicode literals when doing `using namespace beetle::unicode`
 */
using namespace literals::unicode_literals;

// NOLINTNEXTLINE
namespace cdpts = code_points;

}  // namespace unicode

inline namespace literals {

inline namespace unicode_literals {

[[nodiscard]] consteval auto operator""_U(unsigned long long int code_point) -> unicode::CodePoint {
    if (!unicode::is_code_point(code_point)) {
        throw std::invalid_argument{"The given value is not a valid Unicode code point."};
    }

    return unicode::CodePoint{unicode::unsafe::no_validation, static_cast<unicode::CodePoint::value_type>(code_point)};
}

}  // namespace unicode_literals

}  // namespace literals

[[nodiscard]] constexpr auto to_integer(unicode::CodePoint code_point) noexcept -> unicode::CodePoint::value_type {
    return static_cast<unicode::CodePoint::value_type>(code_point);
}

template <typename IntegerType>
    requires std::integral<IntegerType>
[[nodiscard]] constexpr auto to_integer(unicode::CodePoint code_point) noexcept -> IntegerType {
    return IntegerType(beetle::to_integer(code_point));
}

/**
 * Return a string representation of a Unicode code point.
 *
 * @note Follows the format U+<hex_number>. For example: U+10FFFF.
 *
 * @param code_point The Unicode code point to stringify
 *
 * @return The string representation of the Unicode code point
 */
[[nodiscard]] auto to_string(unicode::CodePoint code_point) -> std::string;

// NOLINTNEXTLINE
namespace uc = unicode;

}  // namespace beetle

/**
 * Insert the given Unicode code point into the given stream.
 *
 * @param[out]  stream      The stream to insert into
 * @param       code_point  The Unicode code point to insert
 *
 * @return A reference to the given stream
 */
auto operator<<(std::ostream& stream, beetle::unicode::CodePoint code_point) -> std::ostream&;

/**
 * Supports unsigned number formatting and Unicode notation (U+1A3E).
 *
 * {}, {:} and {:U} will use Unicode notation, everything else is number formatting
 */
template <class CharT>
class std::formatter<beetle::unicode::CodePoint, CharT> {
   public:
    constexpr auto parse(auto& context) {
        auto it{std::begin(context)};
        auto const end{std::cend(context)};

        // {} format specifier
        if (it == end || *it == '}') {
            return it;
        }

        // {:U} format specifier
        if (*it == 'U') {
            ++it;

            if (it != end && *it != '}') {
                throw std::format_error{"Invalid Unicode code point format specifier."};
            }

            return it;
        }

        // Pass-through format specifier
        this->m_is_unicode_output = false;

        this->m_format_value = "{:";
        for (; it != end; ++it) {
            auto const letter = *it;

            this->m_format_value += letter;

            if (letter == '}') {
                return it;
            }
        }

        return it;
    }

    auto format(beetle::unicode::CodePoint code_point, auto& context) const {
        auto const formattable_cdpt = beetle::to_integer<std::uint32_t>(code_point);

        if (this->m_is_unicode_output) {
            // U+0000 - U+10FFFF
            return std::format_to(context.out(), "U+{:04X}", formattable_cdpt);
        }

        return std::vformat_to(context.out(), this->m_format_value, std::make_format_args(formattable_cdpt));
    }

   private:
    std::string m_format_value;
    bool m_is_unicode_output{true};
};

#endif
