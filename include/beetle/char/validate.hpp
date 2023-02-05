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
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef BEETLE_CHAR_VALIDATE_HPP
#define BEETLE_CHAR_VALIDATE_HPP

#include <algorithm>
#include <iterator>
#include <ranges>
#include <stdexcept>
#include <string>

#include "beetle/code_unit.hpp"
#include "beetle/code_unit/exception.hpp"
#include "beetle/core/assert.hpp"
#include "beetle/core/types.hpp"

/**
 * Contains ways to inspect a single UTF-8 code unit.
 *
 * @file validate.hpp
 */

namespace beetle::utf8 {

// NOLINTNEXTLINE(readability-identifier-naming)
inline namespace cpp20_v1 {

template <typename Range>
concept code_unit_input_range =
    std::ranges::input_range<Range> && std::convertible_to<std::ranges::range_value_t<Range>, char8_t>;

template <typename Iterator>
concept code_unit_input_iterator =
    std::input_iterator<Iterator> && std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>;

// ========================= VALIDATING A CHARACTER ========================= //

template <code_unit_input_iterator InputIt, std::sentinel_for<InputIt> Sentinel>
[[nodiscard]] constexpr bool is_overlong_encoded(InputIt first, Sentinel last) {
    auto const distance = std::ranges::distance(first, last);

    auto const leading_byte = *first;
    auto const char_size = utf8::try_leading_byte_size(leading_byte);

    if (char_size != distance || utf8::is_ascii(leading_byte)) {
        return false;
    }

    if (utf8::is_leading_mb_2(leading_byte)) {
        return (leading_byte & 0x1FU) < 0x02U;
    }

    if (utf8::is_leading_mb_3(leading_byte)) {
        // 1010 0000 is the lowest acceptable continuation byte value
        return leading_byte == 0xE0U && (*(++first) & 0x3FU) < 0x20U;
    }

    if (utf8::is_leading_mb_4(leading_byte)) {
        // 1001 0000 is the lowest acceptable continuation byte value
        return leading_byte == 0xF0U && (*(++first) & 0x3FU) < 0x10U;
    }

    // TODO: Add throw or make better
    return false;
}

template <code_unit_input_iterator InputIt, std::sentinel_for<InputIt> Sentinel>
[[nodiscard]] constexpr bool is_mb_char(InputIt first, Sentinel last) {
    auto const distance = std::ranges::distance(first, last);

    auto const leading_byte = *first;
    auto const char_size = utf8::try_leading_byte_size(leading_byte);

    if (char_size != distance || utf8::is_ascii(leading_byte)) {
        return false;
    }

    auto const are_cont_bytes = std::ranges::all_of(std::ranges::next(first), last, utf8::is_continuation_byte);

    return are_cont_bytes && !utf8::is_overlong_encoded(first, last);
}

template <code_unit_input_iterator InputIt, std::sentinel_for<InputIt> Sentinel>
[[nodiscard]] constexpr bool is_char(InputIt first, Sentinel last) {
    return utf8::is_ascii(*first) || utf8::is_mb_char(first, last);
}

[[nodiscard]] constexpr bool is_char(code_unit_input_range auto&& range) {
    return beetle::utf8::is_char(std::ranges::begin(range), std::ranges::end(range));
}

}  // namespace cpp20_v1

}  // namespace beetle::utf8

#endif
