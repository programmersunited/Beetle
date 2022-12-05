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

#ifndef BEETLE_VALIDATE_HPP
#define BEETLE_VALIDATE_HPP

#include <algorithm>
#include <ranges>
#include <string>

#include "beetle/code_unit.hpp"
#include "beetle/types.hpp"

/**
 * Contains ways to inspect a single UTF-8 code unit.
 *
 * @file validate.hpp
 */

namespace beetle::utf8 {

// NOLINTNEXTLINE(readability-identifier-naming)
inline namespace cpp20_v1 {

template <typename Range>
concept code_unit_input_range = std::ranges::input_range<Range> &&
    std::convertible_to<std::ranges::range_value_t<Range>, char8_t>;

template <typename Iterator>
concept code_unit_input_iterator = std::input_iterator<Iterator> &&
    std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>;

// ========================= VALIDATING A CHARACTER ========================= //

namespace internal {

template <code_unit_input_iterator InputIt, std::sentinel_for<InputIt> Sentinel>
[[nodiscard]] constexpr bool impl_is_mb_char(InputIt first, std::int8_t length) {
    namespace utf8 = beetle::utf8;

    auto const sentinel = std::ranges::next(first, length);

    return  utf8::is_leading_multiple_bytes(*first) && 
            std::ranges::all_of(std::ranges::next(first), sentinel, 
                                utf8::is_continuation_byte);
}

template <code_unit_input_iterator InputIt, std::sentinel_for<InputIt> Sentinel>
[[nodiscard]] constexpr bool impl_is_mb_char(InputIt first, Sentinel last) {
    return internal::impl_is_mb_char(first, std::ranges::distance(first, last));
}

}  // namespace internal

template <code_unit_input_iterator InputIt, std::sentinel_for<InputIt> Sentinel>
[[nodiscard]] constexpr bool is_mb_character(InputIt first, Sentinel last) {
    return internal::impl_is_mb_char(first, last);
}

template <code_unit_input_iterator InputIt, std::sentinel_for<InputIt> Sentinel>
[[nodiscard]] constexpr bool starts_with_mb_character(InputIt first, Sentinel last) {
    auto const distance = std::ranges::distance(first, last);
    auto const count_opt = beetle::utf8::peek_char_size(*first);

    auto const valid_length = count_opt.has_value() && distance >= *count_opt;

    return valid_length && internal::impl_is_mb_char(first, *count_opt);
}

template <code_unit_input_iterator InputIt, std::sentinel_for<InputIt> Sentinel>
[[nodiscard]] constexpr bool is_valid_character(InputIt first, Sentinel last) {
    namespace utf8 = beetle::utf8;
    return utf8::is_ascii(*first) || utf8::is_mb_character(first, last);
}

[[nodiscard]] constexpr bool is_valid_character(code_unit_input_range auto&& range) {
    return beetle::utf8::is_valid_character(std::ranges::begin(range),
                                            std::ranges::end(range));
}

template <beetle::utf8::code_unit_input_iterator InputIt,
          std::sentinel_for<InputIt> Sentinel>
[[nodiscard]] constexpr bool starts_with_valid_character(InputIt first,
                                                         Sentinel last) {
    namespace utf8 = beetle::utf8;

    auto const count_opt = utf8::peek_char_size(*first);
    auto const distance = std::distance(first, last);

    return (count_opt.has_value() && distance >= count_opt) &&
           (utf8::is_ascii(*first) ||
            std::ranges::all_of(std::ranges::next(first), last,
                                utf8::is_continuation_byte));
}

[[nodiscard]] constexpr bool starts_with_valid_character(code_unit_input_range auto&& range) {
    return beetle::utf8::starts_with_valid_character(std::ranges::begin(range),
                                                    std::ranges::end(range));
}

}  // namespace cpp20_v1

}  // namespace beetle::utf8

#endif

