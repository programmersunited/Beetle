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

#ifndef BEETLE_CHAR_ALGORITHM_HPP
#define BEETLE_CHAR_ALGORITHM_HPP

#include <algorithm>
#include <array>
#include <iterator>
#include <ranges>
#include <string>

#include "beetle/char/iterator.hpp"
#include "beetle/char/validate.hpp"
#include "beetle/code_unit.hpp"
#include "beetle/types.hpp"

/**
 * Contains algorithms to inspect a sequence of code units.
 *
 * @file validate.hpp
 */

namespace beetle::utf8 {

// NOLINTNEXTLINE(readability-identifier-naming)
inline namespace cpp20_v1 {

namespace unsafe {

template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr beetle::ssize calculate_length(Iterator first, Sentinel last) {
    beetle::ssize length{0};

    while (first != last && *first != '\0') {
        utf8::unsafe::advance(first, 1);
        ++length;
    }

    return length;
}

template <std::ranges::input_range Range>
    requires std::convertible_to<std::ranges::range_value_t<Range>, char8_t>
[[nodiscard]] constexpr beetle::ssize calculate_length(Range&& range) {
    return unsafe::calculate_length(std::ranges::begin(range), std::ranges::end(range));
}

}  // namespace unsafe

template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr beetle::ssize calculate_length(Iterator first, Sentinel last) {
    beetle::ssize length{0};

    while (first != last && *first != '\0') {
        utf8::advance(first, 1);
        ++length;
    }

    return length;
}

template <std::ranges::input_range Range>
    requires std::convertible_to<std::ranges::range_value_t<Range>, char8_t>
[[nodiscard]] constexpr beetle::ssize calculate_length(Range&& range) {
    return calculate_length(std::ranges::begin(range), std::ranges::end(range));
}

template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator find_invalid(Iterator first, Sentinel last) {
    auto distance = std::ranges::distance(first, last);

    while (first != last && *first != '\0') {
        auto const leading_byte = *first;

        if (!utf8::is_leading_byte(leading_byte)) {
            return first;
        }

        auto const char_size = utf8::char_size_from_leading_byte(leading_byte);

        if (char_size < distance) {
            return first;
        }

        auto const char_end = std::ranges::next(first, char_size);

        if (!utf8::is_char(first, char_end)) {
            return first;
        }

        distance -= char_size;
        first = char_end;
    }

    return first;
}

template <std::ranges::input_range Range>
    requires std::convertible_to<std::ranges::range_value_t<Range>, char8_t>
[[nodiscard]] constexpr std::ranges::borrowed_iterator_t<Range> find_invalid(Range&& range) {
    return find_invalid(std::ranges::begin(range), std::ranges::end(range));
}

template <std::input_iterator Iterator, std::sentinel_for<Iterator> Sentinel, std::weakly_incrementable Output,
          typename ReplacementChar>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t> &&
             std::convertible_to<ReplacementChar, char8_t> && std::indirectly_copyable<Iterator, Output>
[[nodiscard]] constexpr Iterator copy_first_char(Iterator first, Sentinel last, Output result,
                                                 ReplacementChar replacement_char) {}

template <std::input_iterator Iterator, std::sentinel_for<Iterator> Sentinel, std::weakly_incrementable Output,
          typename ReplacementChar>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t> &&
             std::convertible_to<ReplacementChar, char8_t> && std::indirectly_copyable<Iterator, Output>
[[nodiscard]] constexpr Iterator copy(Iterator first, Sentinel last, Output result, ReplacementChar replacement_char) {
    while (first != last && *first != '\0') {
        first = copy_char(first, last, result, replacement_char);

        auto const char_size = utf8::peek_char_size(*first);

        auto const char_end = std::ranges::next(first, char_size);

        if (!utf8::is_char(first, char_end)) {
            *result = replacement_char;
            first = std::ranges::find_if(first, last, utf8::is_leading_byte);
        } else {
            std::ranges::copy(first, char_end, result);
            first = char_end;
        }
    }
}

}  // namespace cpp20_v1

}  // namespace beetle::utf8

#endif
