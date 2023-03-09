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

class Unicode {
   public:
    using value_type = std::uint32_t;

    constexpr Unicode() noexcept = default;

    constexpr Unicode(value_type value) : m_data{validate_construct(value)} {}

    [[nodiscard]] constexpr auto operator<=>(Unicode const&) const noexcept = default;

    [[nodiscard]] constexpr std::strong_ordering operator<=>(value_type value) const noexcept {
        return this->m_data <=> value;
    }

    constexpr Unicode& operator=(value_type other) {
        this->m_data = Unicode::validate(other);

        return *this;
    }

    [[nodiscard]] static constexpr std::optional<Unicode> create(value_type value) noexcept {
        if (Unicode::is_valid(value)) {
            return Unicode{NoUnicodeValidation{}, value};
        }

        return std::nullopt;
    }

    [[nodiscard]] static constexpr bool is_valid(std::integral auto value) noexcept { return value <= 0x10FFFFU; }

    template <typename IntegerType>
        requires std::integral<IntegerType>
    friend constexpr IntegerType to_integer(Unicode value) noexcept;

    friend constexpr Unicode operator""_U(unsigned long long int unicode_value);

   private:
    value_type m_data{0};

    constexpr Unicode(NoUnicodeValidation, value_type value) : m_data{value} {}

    [[noreturn]] static value_type throw_unicode_error() { throw std::invalid_argument{"Invalid Unicode value."}; }

    [[nodiscard]] static constexpr value_type validate_construct(value_type value) {
        return value <= 0x10FFFFU ? value : throw std::invalid_argument{"Invalid Unicode value."};
    }

    [[nodiscard]] static constexpr value_type validate(std::integral auto value) {
        return value <= 0x10FFFFU ? value : throw std::invalid_argument{"Invalid Unicode value."};
    }
};

[[nodiscard]] constexpr Unicode operator""_U(unsigned long long int unicode_value) {
    if (Unicode::is_valid(unicode_value)) {
        return Unicode{NoUnicodeValidation{}, static_cast<Unicode::value_type>(unicode_value)};
    }

    throw std::invalid_argument{"Invalid Unicode value."};
    // Unicode::throw_unicode_error();
}

template <typename IntegerType>
    requires std::integral<IntegerType>
[[nodiscard]] constexpr IntegerType to_integer(Unicode value) noexcept {
    return IntegerType(value.m_data);
}

[[nodiscard]] constexpr auto to_integer(Unicode value) noexcept {
    return beetle::to_integer<Unicode::value_type>(value);
}

}  // namespace cpp20_v1

}  // namespace beetle

#endif
