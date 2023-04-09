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
#include <optional>
#include <stdexcept>

namespace beetle {

// NOLINTNEXTLINE(readability-identifier-naming)
inline namespace cpp20_v1 {

namespace {

/**
 * Tag type to not validate unicode input.
 */
struct NoUnicodeValidation {};

}  // namespace

/**
 * A representation of a Unicode value.
 *
 * @note This will throw on construction if the given value is over the valid Unicode max value.
 */
class Unicode {
   public:
    /**
     * The underlying integer type for the Unicode value.
     */
    using value_type = std::uint32_t;

    /**
     * Default constructor.
     */
    constexpr Unicode() noexcept = default;

    /**
     * Construct Unicode using the given value.
     *
     * @param value The unicode value
     *
     * @throws std::invalid_argument if the given value is too large to be a Unicode value
     */
    constexpr Unicode(value_type value) : m_data{validate(value)} {}

    [[nodiscard]] constexpr auto operator<=>(Unicode const&) const noexcept = default;

    /**
     * Assign the given value to this Unicode.
     *
     * @param other The new value for this Unicode
     *
     * @return A reference to this Unicode
     *
     * @throws std::invalid_argument if the given value is too large to be a Unicode value
     */
    constexpr Unicode& operator=(value_type other) {
        this->m_data = Unicode::validate(other);

        return *this;
    }

    /**
     * Construct an Unicode value using the given value.
     *
     * @param value The value for the Unicode value
     *
     * @return The Unicode value if the given value is valid, otherwise std::nullopt.
     */
    [[nodiscard]] static constexpr std::optional<Unicode> create(value_type value) noexcept {
        if (Unicode::is_valid(value)) {
            return Unicode{NoUnicodeValidation{}, value};
        }

        return std::nullopt;
    }

    /**
     * Check if the given value is a valid Unicode value.
     *
     * @param value The value to check
     *
     * @return true if valid, otherwise false.
     */
    [[nodiscard]] static constexpr bool is_valid(std::unsigned_integral auto value) noexcept {
        return value <= 0x10FFFFU;
    }

    template <typename IntegerType>
        requires std::integral<IntegerType>
    friend constexpr IntegerType to_integer(Unicode value) noexcept;

    friend constexpr Unicode operator""_U(unsigned long long int unicode_value);

   private:
    /**
     * The underlying integer representation of the Unicode value.
     *
     * @note The default constructed value is 0.
     */
    value_type m_data{0};

    /**
     * Constructs an Unicode value using the given value.
     *
     * @param value The value for Unicode
     *
     * @note This is used to bypass the validation check.
     */
    constexpr Unicode(NoUnicodeValidation, value_type value) : m_data{value} {}

    /**
     * Validate if the given value is a valid Unicode value.
     *
     * @param value The value to validate
     *
     * @return The given value
     *
     * @throws std::invalid_argument if the given value is not a valid Unicode value, otherwise true.
     */
    [[nodiscard]] static constexpr value_type validate(value_type value) {
        return Unicode::is_valid(value) ? value : throw std::invalid_argument{"Invalid Unicode value."};
    }
};

/**
 * User-defined literal for constructing a Unicde value using the given value.
 *
 * @param unicode_value The Unicode value
 *
 * @throws std::invalid_argument if the given value is too large to be a Unicode value
 *
 * @return The constructed Unicode value
 */
[[nodiscard]] constexpr Unicode operator""_U(unsigned long long int unicode_value) {
    if (Unicode::is_valid(unicode_value)) {
        return Unicode{NoUnicodeValidation{}, static_cast<Unicode::value_type>(unicode_value)};
    }

    throw std::invalid_argument{"Invalid Unicode value."};
}

/**
 * Convert the given Unicode value to the given integer type.
 *
 * @tparam IntegerType The integer type to convert to
 *
 * @param value The Unicode value to convert
 *
 * @return The Unicode value as the given integer type
 */
template <typename IntegerType>
    requires std::integral<IntegerType>
[[nodiscard]] constexpr IntegerType to_integer(Unicode value) noexcept {
    return IntegerType(value.m_data);
}

/**
 * Convert the given Unicode value to its underlying integer type.
 *
 * @param value The Unicode value to convert
 *
 * @return The Unicode value as its underlying integer type
 */
[[nodiscard]] constexpr auto to_integer(Unicode value) noexcept {
    return beetle::to_integer<Unicode::value_type>(value);
}

}  // namespace cpp20_v1

}  // namespace beetle

#endif
