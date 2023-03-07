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

#ifndef BEETLE_UTF8_ALGORITHM_HPP
#define BEETLE_UTF8_ALGORITHM_HPP

#include <algorithm>
#include <iterator>
#include <memory>
#include <memory_resource>
#include <ranges>
#include <stdexcept>
#include <string>
#include <system_error>

#include "beetle/core/types.hpp"
#include "beetle/unicode/unicode.hpp"
#include "beetle/utf8/error.hpp"
#include "beetle/utf8/iterator.hpp"
#include "utf8/internal/code_unit.hpp"
#include "utf8/internal/dfa.hpp"

/**
 * Contains algorithms to inspect a sequence of code units.
 *
 * @file validate.hpp
 */

namespace beetle::utf8 {

namespace internal {

// NOLINTNEXTLINE(readability-identifier-naming)
inline namespace cpp20_v1 {

template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] utf8::internal::DFA::State impl_safe_str_len(Iterator first, Sentinel last, beetle::ssize& length) {
    length = 0;

    while (first != last && *first != '\0') {
        auto state = utf8::internal::DFA::advance_forward_once(first, last);

        if (state != utf8::internal::DFA::State::eAccept) {
            length = 0;
            return state;
        }

        ++length;
    }

    return DFA::State::eAccept;
}

}  // namespace cpp20_v1

}  // namespace internal

namespace unsafe {

// NOLINTNEXTLINE(readability-identifier-naming)
inline namespace cpp20_v1 {

template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr beetle::ssize str_len(Iterator first, Sentinel last) {
    beetle::ssize length{0};

    while (first != last && *first != '\0') {
        first = utf8::unsafe::next(first);
        ++length;
    }

    return length;
}

template <std::ranges::input_range Range>
    requires std::convertible_to<std::ranges::range_value_t<Range>, char8_t>
[[nodiscard]] constexpr beetle::ssize str_len(Range&& range) {
    return unsafe::str_len(std::ranges::begin(range), std::ranges::end(range));
}

}  // namespace cpp20_v1

}  // namespace unsafe

// NOLINTNEXTLINE(readability-identifier-naming)
inline namespace cpp20_v1 {

template <typename Input, typename Output>
using decode_result = std::ranges::in_out_result<Input, Output>;

template <typename Input, typename Output>
using sanitize_result = std::ranges::in_out_result<Input, Output>;

template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] beetle::ssize str_len(Iterator first, Sentinel last, std::error_condition& condition) {
    beetle::ssize length{0};

    auto const ending_state = utf8::internal::impl_safe_str_len(first, last, length);

    if (ending_state != utf8::internal::DFA::State::eAccept) {
        condition = utf8::internal::DFA::make_error_condition(ending_state);
    }

    return length;
}

template <std::ranges::input_range Range>
    requires std::convertible_to<std::ranges::range_value_t<Range>, char8_t>
[[nodiscard]] constexpr beetle::ssize str_len(Range&& range, std::error_condition& condition) {
    return utf8::str_len(std::ranges::begin(range), std::ranges::end(range), condition);
}

template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr beetle::ssize str_len(Iterator first, Sentinel last) {
    beetle::ssize length{0};

    auto const ending_state = utf8::internal::impl_safe_str_len(first, last, length);

    if (ending_state != utf8::internal::DFA::State::eAccept) {
        throw utf8::internal::DFA::make_error_condition(ending_state);
    }

    return length;
}

template <std::ranges::input_range Range>
    requires std::convertible_to<std::ranges::range_value_t<Range>, char8_t>
[[nodiscard]] constexpr beetle::ssize str_len(Range&& range) {
    return utf8::str_len(std::ranges::begin(range), std::ranges::end(range));
}

template <std::input_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator find_leading_byte(Iterator first, Sentinel last) {
    return std::ranges::find_if(first, last, utf8::internal::is_leading_byte);
}

template <std::ranges::input_range Range>
    requires std::convertible_to<std::ranges::range_value_t<Range>, char8_t>
[[nodiscard]] constexpr std::ranges::borrowed_iterator_t<Range> find_leading_byte(Range&& range) {
    return utf8::find_leading_byte(std::ranges::begin(range), std::ranges::end(range));
}

template <std::input_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator find_invalid(Iterator first, Sentinel last) {
    while (first != last) {
        auto const ending_state = utf8::internal::DFA::advance_forward_once(first, last);

        if (ending_state != utf8::internal::DFA::State::eAccept) {
            return first;
        }
    }

    return first;
}

template <std::ranges::input_range Range>
    requires std::convertible_to<std::ranges::range_value_t<Range>, char8_t>
[[nodiscard]] constexpr std::ranges::borrowed_iterator_t<Range> find_invalid(Range&& range) {
    return utf8::find_invalid(std::ranges::begin(range), std::ranges::end(range));
}

template <std::input_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator is_valid(Iterator first, Sentinel last) {
    return utf8::find_invalid(first, last) != last;
}

template <std::ranges::input_range Range>
    requires std::convertible_to<std::ranges::range_value_t<Range>, char8_t>
[[nodiscard]] constexpr std::ranges::borrowed_iterator_t<Range> is_valid(Range&& range) {
    return utf8::is_valid(std::ranges::begin(range), std::ranges::end(range));
}

template <std::weakly_incrementable Output>
constexpr void encode(beetle::Unicode code_point, Output result) {
    auto const raw_value = beetle::to_integer(code_point);

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
        //              w wwxx xxxx yyyy yyzz zzzz
        // 1111 0www 10xx xxxx 10yy yyyy 10zz zzzz
        *result = static_cast<char8_t>((raw_value >> 18) | 0xF0U);
        ++result;
        *result = static_cast<char8_t>(((raw_value >> 12) & 0x3FU) | 0x80U);
        ++result;
        *result = static_cast<char8_t>(((raw_value >> 6) & 0x3FU) | 0x80U);
        ++result;
        *result = static_cast<char8_t>((raw_value & 0x3FU) | 0x80U);
        ++result;
    }
}

template <std::input_iterator Iterator, std::sentinel_for<Iterator> Sentinel, std::weakly_incrementable Output>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t> &&
             std::convertible_to<typename std::iter_value_t<Output>, beetle::Unicode>
[[nodiscard]] constexpr utf8::decode_result<Iterator, Output> decode(Iterator first, Sentinel last, Output result) {
    while (first != last) {
        auto code_point = char32_t{0};
        auto const ending_state = utf8::internal::DFA::decode_and_advance_forward_once(first, last, code_point);

        if (ending_state != utf8::internal::DFA::State::eAccept) {
            break;
        }

        *result = static_cast<Unicode::value_type>(code_point);
        ++result;
    }

    return {std::move(first), std::move(result)};
}

template <std::ranges::input_range Range, std::weakly_incrementable Output>
    requires std::convertible_to<typename std::ranges::iterator_t<Range>, char8_t> &&
             std::convertible_to<typename std::iter_value_t<Output>, beetle::Unicode>
[[nodiscard]] constexpr utf8::decode_result<std::ranges::borrowed_iterator_t<Range>, Output> decode(Range&& range,
                                                                                                    Output result) {
    return utf8::decode(std::ranges::begin(range), std::ranges::end(range), result);
}

template <std::input_iterator Iterator, std::sentinel_for<Iterator> Sentinel, std::weakly_incrementable Output>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t> &&
             std::indirectly_copyable<Iterator, Output>
constexpr utf8::sanitize_result<Iterator, Output> sanitize(Iterator first, Sentinel last, Output result,
                                                           beetle::Unicode replacement_code_point = 0xFFFDU) {
    // Convert code unit to UTF-8
    std::u8string replacement_char;
    replacement_char.reserve(4);

    utf8::encode(replacement_code_point, std::back_inserter(replacement_char));

    std::u8string copied_char;
    copied_char.reserve(4);

    while (first != last) {
        auto const ending_state =
            utf8::internal::DFA::copy_and_advance_forward_once(first, last, std::back_inserter(copied_char));

        if (ending_state == utf8::internal::DFA::State::eAccept) {
            std::ranges::copy(copied_char, result);
        } else {
            std::ranges::copy(replacement_char, result);

            // Find start of the next possible UTF-8 character
            first = utf8::find_leading_byte(first, last);
        }

        copied_char.clear();
    }

    return {std::move(first), std::move(result)};
}

template <std::ranges::input_range Range, std::weakly_incrementable Output>
    requires std::indirectly_copyable<std::ranges::iterator_t<Range>, Output>
constexpr utf8::sanitize_result<std::ranges::borrowed_iterator_t<Range>, Output> sanitize(
    Range&& range, Output result, beetle::Unicode replacement_code_point = 0xFFFDU) {
    return sanitize(std::ranges::begin(range), std::ranges::end(range), result, replacement_code_point);
}

}  // namespace cpp20_v1

}  // namespace beetle::utf8

#endif
