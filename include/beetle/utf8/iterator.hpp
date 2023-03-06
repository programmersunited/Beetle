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

#ifndef BEETLE_UTF8_ITERATOR_HPP
#define BEETLE_UTF8_ITERATOR_HPP

#include <concepts>
#include <iterator>

#include "beetle/utf8/error.hpp"
#include "utf8/internal/code_unit.hpp"
#include "utf8/internal/dfa.hpp"

/**
 * Free functions to iterator over UTF-8 code units.
 *
 * @note API based off of the C++20 standard <iterator> header for ranges.
 * @note The unsafe version of the functions provide the same functionality with little to no validation.
 *
 * @file iterator.hpp
 */

namespace beetle::utf8 {

namespace unsafe {

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
constexpr std::iter_difference_t<Iterator> advance(Iterator& iterator, std::iter_difference_t<Iterator> num,
                                                   Sentinel bound);

template <std::input_or_output_iterator Iterator>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator next(Iterator iterator) {
    // TODO: Add assert to check for leading byte
    return std::ranges::next(iterator, internal::unsafe::char_size_from_leading_byte(*iterator));
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
        --iterator;
    } while (utf8::internal::is_continuation_byte(*iterator));

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
constexpr std::iter_difference_t<Iterator> advance(Iterator& iterator, std::iter_difference_t<Iterator> num,
                                                   Sentinel bound) {
    while (num > 0 && iterator != bound) {
        --num;
        iterator = utf8::unsafe::next(iterator);
    }

    if constexpr (std::bidirectional_iterator<Iterator>) {
        while (num < 0 && iterator != bound) {
            ++num;
            iterator = utf8::unsafe::prev(iterator);
        }
    }

    return num;
}

}  // namespace cpp20_v1

}  // namespace unsafe

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
constexpr std::iter_difference_t<Iterator> advance(Iterator& iterator, std::iter_difference_t<Iterator> num,
                                                   Sentinel bound);

template <std::input_or_output_iterator Iterator>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator next(Iterator iterator) {
    auto const possible_end = std::ranges::next(iterator, 4);
    auto const ending_state = utf8::internal::advance_forward_once(iterator, possible_end);

    if (ending_state != utf8::internal::State::eAccept) {
        throw beetle::make_error_condition(utf8::internal::ending_state_to_error(ending_state));
    }

    return iterator;
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
    auto const possible_end = std::ranges::prev(iterator, 4);
    auto const ending_state = utf8::internal::advance_backward_once(iterator, possible_end);

    if (ending_state != utf8::internal::State::eAccept) {
        throw beetle::make_error_condition(utf8::internal::ending_state_to_error(ending_state));
    }

    return iterator;
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
constexpr std::iter_difference_t<Iterator> advance(Iterator& iterator, std::iter_difference_t<Iterator> num,
                                                   Sentinel bound) {
    while (num > 0 && iterator != bound) {
        --num;
        iterator = utf8::next(iterator);
    }

    if constexpr (std::bidirectional_iterator<Iterator>) {
        while (num < 0 && iterator != bound) {
            ++num;
            iterator = utf8::prev(iterator);
        }
    }

    return num;
}

}  // namespace cpp20_v1

}  // namespace beetle::utf8

#endif
