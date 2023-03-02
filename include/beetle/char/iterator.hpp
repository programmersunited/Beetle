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

#ifndef BEETLE_CHAR_ITERATOR_HPP
#define BEETLE_CHAR_ITERATOR_HPP

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <ranges>
#include <stdexcept>
#include <string_view>

#include "beetle/char/validate.hpp"
#include "beetle/code_unit.hpp"
#include "beetle/code_unit/exception.hpp"
#include "beetle/core/assert.hpp"

/**
 * Free functions to iterator over UTF-8 code units.
 *
 * @note The unsafe version of the functions provide the same functionality with little to no validation.
 *
 * @file iterator.hpp
 */

namespace beetle::utf8 {

/**
 * API based off of the C++20 standard <iterator> header for ranges.
 */
// NOLINTNEXTLINE(readability-identifier-naming)
inline namespace cpp20_v1 {

template <std::input_or_output_iterator Iterator>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator next(Iterator iterator);

template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator next(Iterator iterator, Sentinel bound);

template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator next(Iterator iterator, Sentinel bound);

template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator next(Iterator iterator, std::iter_difference_t<Iterator> num, Sentinel bound);

template <std::bidirectional_iterator Iterator>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator prev(Iterator iterator);

template <std::bidirectional_iterator Iterator>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator prev(Iterator iterator, std::iter_difference_t<Iterator> num);

template <std::bidirectional_iterator Iterator>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator prev(Iterator iterator, std::iter_difference_t<Iterator> num, Iterator bound);

template <std::input_or_output_iterator Iterator>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
constexpr void advance(Iterator& iterator, std::iter_difference_t<Iterator> num);

template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
constexpr void advance(Iterator& iterator, Sentinel bound);

template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
constexpr void advance(Iterator& iterator, std::iter_difference_t<Iterator> num, Sentinel bound);

namespace internal {

template <code_unit_input_iterator InputIt, std::sentinel_for<InputIt> Sentinel>
inline constexpr void check_char(InputIt first, Sentinel last) {
    if (!utf8::is_char(first, last)) {
        throw std::runtime_error{"Iterated over an invalid UTF-8 character."};
    }
}

}  // namespace internal

namespace unsafe {

template <std::input_or_output_iterator Iterator>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator next(Iterator iterator);

template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator next(Iterator iterator, Sentinel bound);

template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator next(Iterator iterator, Sentinel bound);

template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator next(Iterator iterator, std::iter_difference_t<Iterator> num, Sentinel bound);

template <std::bidirectional_iterator Iterator>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator prev(Iterator iterator);

template <std::bidirectional_iterator Iterator>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator prev(Iterator iterator, std::iter_difference_t<Iterator> num);

template <std::bidirectional_iterator Iterator>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator prev(Iterator iterator, std::iter_difference_t<Iterator> num, Iterator bound);

template <std::input_or_output_iterator Iterator>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
constexpr void advance(Iterator& iterator, std::iter_difference_t<Iterator> num);

template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
constexpr void advance(Iterator& iterator, Sentinel bound);

template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
constexpr void advance(Iterator& iterator, std::iter_difference_t<Iterator> num, Sentinel bound);

template <std::input_or_output_iterator Iterator>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator next(Iterator iterator) {
    return std::ranges::next(iterator, utf8::leading_byte_size(*iterator));
}

template <std::input_or_output_iterator Iterator>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator next(Iterator iterator, std::iter_difference_t<Iterator> num) {
    utf8::unsafe::advance(iterator, num);

    return iterator;
}

template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator next(Iterator iterator, Sentinel bound) {
    utf8::unsafe::advance(iterator, bound);

    return iterator;
}

template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator next(Iterator iterator, std::iter_difference_t<Iterator> num, Sentinel bound) {
    utf8::unsafe::advance(iterator, num, bound);

    return iterator;
}

template <std::bidirectional_iterator Iterator>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator prev(Iterator iterator) {
    do {
        iterator = std::ranges::prev(iterator);
    } while (!utf8::is_leading_byte(*iterator));

    return iterator;
}

template <std::bidirectional_iterator Iterator>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator prev(Iterator iterator, std::iter_difference_t<Iterator> num) {
    utf8::unsafe::advance(iterator, -num);

    return iterator;
}

template <std::bidirectional_iterator Iterator>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator prev(Iterator iterator, std::iter_difference_t<Iterator> num, Iterator bound) {
    utf8::unsafe::advance(iterator, -num, bound);

    return iterator;
}

template <std::input_or_output_iterator Iterator>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
constexpr void advance(Iterator& iterator, std::iter_difference_t<Iterator> num) {
    while (num > 0) {
        --num;
        iterator = utf8::unsafe::next(iterator);
    }

    if constexpr (std::bidirectional_iterator<Iterator>) {
        while (num < 0) {
            ++num;
            iterator = utf8::unsafe::prev(iterator);
        }
    }
}

template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
constexpr void advance(Iterator& iterator, Sentinel bound) {
    while (iterator != bound) {
        iterator = utf8::unsafe::next(iterator);
    }
}

template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
constexpr void advance(Iterator& iterator, std::iter_difference_t<Iterator> num, Sentinel bound) {
    while (num > 0 && iterator != bound) {
        iterator = utf8::unsafe::next(iterator);
    }

    if constexpr (std::bidirectional_iterator<Iterator>) {
        while (num < 0 && iterator != bound) {
            ++num;
            iterator = utf8::unsafe::prev(iterator);
        }
    }
}

}  // namespace unsafe

/**
 * Iterates to the first code unit of a valid UTF-8 character.
 *
 * @param iterator The iterator to advance
 *
 * @precondition iterator Currently points to a leading code unit byte
 * @precondition iterator Iterates over a valid UTF-8 character within the
 *                        range [iterator, iterator + leading byte size)
 *
 * @note Follows the strong exception guarantee
 */
template <std::input_or_output_iterator Iterator>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator next(Iterator iterator) {
    auto next_iterator = utf8::unsafe::next(iterator);

    utf8::internal::check_char(iterator, next_iterator);

    return next_iterator;
}

template <std::input_or_output_iterator Iterator>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator next(Iterator iterator, std::iter_difference_t<Iterator> num) {
    utf8::advance(iterator, num);

    return iterator;
}

template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator next(Iterator iterator, Sentinel bound) {
    utf8::advance(iterator, bound);

    return iterator;
}

template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator next(Iterator iterator, std::iter_difference_t<Iterator> num, Sentinel bound) {
    utf8::advance(iterator, num, bound);

    return iterator;
}

template <std::bidirectional_iterator Iterator>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator prev(Iterator iterator) {
    // Could point to leading byte or std::end(iterator)
    auto prev_iterator = utf8::unsafe::prev(iterator);

    utf8::internal::check_char(prev_iterator, iterator);

    return prev_iterator;
}

template <std::bidirectional_iterator Iterator>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator prev(Iterator iterator, std::iter_difference_t<Iterator> num) {
    utf8::advance(iterator, -num);

    return iterator;
}

template <std::bidirectional_iterator Iterator>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator prev(Iterator iterator, std::iter_difference_t<Iterator> num, Iterator bound) {
    utf8::advance(iterator, -num, bound);

    return iterator;
}

template <std::input_or_output_iterator Iterator>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
constexpr void advance(Iterator& iterator, std::iter_difference_t<Iterator> num) {
    while (num > 0) {
        --num;
        iterator = utf8::next(iterator);
    }

    if constexpr (std::bidirectional_iterator<Iterator>) {
        while (num < 0) {
            ++num;
            iterator = utf8::prev(iterator);
        }
    }
}

template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
constexpr void advance(Iterator& iterator, Sentinel bound) {
    while (iterator != bound) {
        iterator = utf8::next(iterator);
    }
}

template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
constexpr void advance(Iterator& iterator, std::iter_difference_t<Iterator> num, Sentinel bound) {
    while (num > 0 && iterator != bound) {
        iterator = utf8::next(iterator);
    }

    if constexpr (std::bidirectional_iterator<Iterator>) {
        while (num < 0 && iterator != bound) {
            ++num;
            iterator = utf8::prev(iterator);
        }
    }
}

}  // namespace cpp20_v1

}  // namespace beetle::utf8

#endif
