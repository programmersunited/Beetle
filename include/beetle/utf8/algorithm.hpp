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
#include "core/internal/assert.hpp"
#include "utf8/internal/code_unit.hpp"
#include "utf8/internal/dfa.hpp"

/**
 * Algorithms to inspect, validate, decode, and encode UTF-8 character(s).
 *
 * @file algorithm.hpp
 */

namespace beetle::utf8 {

namespace internal {

// NOLINTNEXTLINE(readability-identifier-naming)
inline namespace cpp20_v1 {

/**
 * Return the UTF-8 character length of the UTF-8 string between the given range up to and not including the first null
 * character.
 *
 * @param       first   The iterator
 * @param       last    The sentinel denoting the end of the range the given iterator points to
 * @param[out]  length  The UTF-8 character length
 *
 * @return The ending DFA state
 */
template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr utf8::internal::DFA::State impl_safe_str_len(Iterator first, Sentinel last,
                                                                     beetle::ssize& length) {
    BEETLE_ASSERT(first != last);

    length = 0;

    while (first != last && *first != '\0') {
        auto const state = utf8::internal::DFA::advance_forward_once(first, last);

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

/**
 * Return the character length of the UTF-8 string between the given range up to and not including the first null
 * character.
 *
 * @note Undefined behavior if the given range [first, last) is not a valid UTF-8 string.
 *
 * @param first The iterator
 * @param last  The sentinel denoting the end of the range first points to
 *
 * @return The UTF-8 string's character length
 */
template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr beetle::ssize str_len(Iterator first, Sentinel last) {
    BEETLE_ASSERT(first != last);

    beetle::ssize length{0};

    while (first != last && *first != '\0') {
        first = utf8::unsafe::next(first);

        ++length;
    }

    return length;
}

/**
 * Return the character length of the UTF-8 string between the given range up to and not including the first null
 * character.
 *
 * @note Undefined behavior if the given range is not a valid UTF-8 string.
 *
 * @param range The UTF-8 string
 *
 * @return The UTF-8 string's character length
 */
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
using encode_result = std::ranges::in_out_result<Input, Output>;

template <typename Input, typename Output>
using decode_result = std::ranges::in_out_result<Input, Output>;

template <typename Input, typename Output>
using sanitize_result = std::ranges::in_out_result<Input, Output>;

/**
 * Return the character length of the UTF-8 string between the given range up to and not including the first null
 * character.
 *
 * @param       first       The iterator
 * @param       last        The sentinel denoting the end of the range first points to
 * @param[out]  condition   The error if the given UTF-8 string is not valid
 *
 * @return The UTF-8 string's character length
 */
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

/**
 * Return the character length of the UTF-8 string between the given range up to and not including the first null
 * character.
 *
 * @param       range       The UTF-8 string
 * @param[out]  condition   The error if the given UTF-8 string is not valid
 *
 * @return The UTF-8 string's character length
 */
template <std::ranges::input_range Range>
    requires std::convertible_to<std::ranges::range_value_t<Range>, char8_t>
[[nodiscard]] constexpr beetle::ssize str_len(Range&& range, std::error_condition& condition) {
    return utf8::str_len(std::ranges::begin(range), std::ranges::end(range), condition);
}

/**
 * Return the character length of the UTF-8 string between the given range up to and not including the first null
 * character.
 *
 * @param first The iterator
 * @param last  The sentinel denoting the end of the range first points to
 *
 * @throw A std::error_condition if the UTF-8 string is invalid between the given range [first, last)
 *
 * @return The UTF-8 string's character length
 */
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

/**
 * Return the character length of the UTF-8 string between the given range up to and not including the first null
 * character.
 *
 * @param range The UTF-8 string
 *
 * @throw A std::error_condition if the UTF-8 string is invalid between the given range
 *
 * @return The UTF-8 string's character length
 */
template <std::ranges::input_range Range>
    requires std::convertible_to<std::ranges::range_value_t<Range>, char8_t>
[[nodiscard]] constexpr beetle::ssize str_len(Range&& range) {
    return utf8::str_len(std::ranges::begin(range), std::ranges::end(range));
}

/**
 * Return the first iterator to a UTF-8 code unit that is a leading byte in the given range [first, last).
 *
 * @note This does not perform any UTF-8 validation. This searches the given UTF-8 code units for the first code unit
 * that is a leading byte.
 *
 * @param first The iterator
 * @param last  The sentinel denoting the end of the given range first points to
 *
 * @return The first iterator to a leading byte or last if not found
 */
template <std::input_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator find_leading_byte(Iterator first, Sentinel last) {
    return std::ranges::find_if(first, last, utf8::internal::is_leading_byte);
}

/**
 * Return the first iterator to a UTF-8 code unit that is a leading byte in the given range.
 *
 * @note This does not perform any UTF-8 validation. This searches the given UTF-8 code units for the first code unit
 * that is a leading byte.
 *
 * @param range The UTF-8 code unit range
 *
 * @return The first iterator to a leading byte or an iterator equal to the end of the given range if not found
 */
template <std::ranges::input_range Range>
    requires std::convertible_to<std::ranges::range_value_t<Range>, char8_t>
[[nodiscard]] constexpr std::ranges::borrowed_iterator_t<Range> find_leading_byte(Range&& range) {
    return utf8::find_leading_byte(std::ranges::begin(range), std::ranges::end(range));
}

/**
 * Return the first iterator to the first invalid UTF-8 code unit in the given range [first, last).
 *
 * @param first The iterator
 * @param last  The sentinel denoting the end of the given range first points to
 *
 * @return The first iterator to a leading byte or last if not found
 */
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

/**
 * Return the first iterator to an invalid UTF-8 code unit in the given range.
 *
 * @param range The UTF-8 string
 *
 * @return The first iterator to an invalid UTF-8 code unit or an iterator equal to the end of the given range if not
 * found
 */
template <std::ranges::input_range Range>
    requires std::convertible_to<std::ranges::range_value_t<Range>, char8_t>
[[nodiscard]] constexpr std::ranges::borrowed_iterator_t<Range> find_invalid(Range&& range) {
    return utf8::find_invalid(std::ranges::begin(range), std::ranges::end(range));
}

/**
 * Check if the given range [first, last) is a valid UTF-8 string.
 *
 * @param first The iterator
 * @param last  The sentinel denoting the end of the given range first points to
 *
 * @return True if the given range is a valid UTF-8 string, otherwise false
 */
template <std::input_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr bool is_valid(Iterator first, Sentinel last) {
    auto is_valid = true;

    while (first != last) {
        auto const ending_state = utf8::internal::DFA::advance_forward_once(first, last);

        if (ending_state != utf8::internal::DFA::State::eAccept) {
            is_valid = false;
            break;
        }
    }

    return is_valid;
}

/**
 * Check if the given range is a valid UTF-8 string.
 *
 * @param range The UTF-8 string
 *
 * @return True if the given range is a valid UTF-8 string, otherwise false
 */
template <std::ranges::input_range Range>
    requires std::convertible_to<std::ranges::range_value_t<Range>, char8_t>
[[nodiscard]] constexpr bool is_valid(Range&& range) {
    return utf8::is_valid(std::ranges::begin(range), std::ranges::end(range));
}

/**
 * Encode the given Unicode code point to a UTF-8 character to the given result.
 *
 * @param       code_point  The Unicode code point
 * @param[out]  result      The beginning of UTF-8 character destination range
 *
 * @return The iterator after the UTF-8 character
 */
template <std::weakly_incrementable Output>
constexpr Output encode(unicode::CodePoint code_point, Output result) {
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

    return result;
}

/**
 * Encode the given Unicode code point range [first, last) to the UTF-8 character range.
 *
 * @param       first   The beginning of the Unicode code point range to encode
 * @param       last    The one past the last element of the Unicode code point range to encode
 * @param[out]  result  The beginning of the UTF-8 character destination range
 *
 * @return A result containing an input iterator equal to last and an output iterator past the last element encoded.
 */
template <std::input_iterator Iterator, std::sentinel_for<Iterator> Sentinel, std::weakly_incrementable Output>
constexpr encode_result<Iterator, Output> encode(Iterator first, Sentinel last, Output result) {
    while (first != last) {
        result = utf8::encode(*first, result);
    }

    return {std::move(first), std::move(result)};
}

/**
 * Encode the given Unicode code point range to the beginning of the given UTF-8 character destination range.
 *
 * @param       range   The Unicode code point range
 * @param[out]  result  The beginning of the UTF-8 character destination range
 *
 * @return A result containing an input iterator equal to last and an output iterator past the last element encoded.
 */
template <std::ranges::input_range Range, std::weakly_incrementable Output>
constexpr encode_result<std::ranges::borrowed_iterator_t<Range>, Output> encode(Range&& range, Output result) {
    return utf8::encode(std::ranges::begin(range), std::ranges::end(range), result);
}

/**
 * Decode the first UTF-8 character in the given range [first, last), and advance the given `first` iterator to after
 * the decoded UTF-8 character of the given range.
 *
 * @param[in, out]  first   The iterator
 * @param           last    The sentinel denoting the end of the range the given iterator points to
 *
 * @throw std::error_condition if the UTF-8 character is not valid
 *
 * @return The Unicode code point
 */
template <std::input_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr unicode::CodePoint decode_and_advance(Iterator& first, Sentinel last) {
    auto code_point = char32_t{0};
    auto const ending_state = utf8::internal::DFA::decode_and_advance_forward_once(first, last, code_point);

    if (ending_state != utf8::internal::DFA::State::eAccept) {
        throw utf8::internal::DFA::make_error_condition(ending_state);
    }

    return unicode::CodePoint{static_cast<unicode::CodePoint::value_type>(code_point)};
}

/**
 * Decode the UTF-8 character to an Unicode code point in the given range [first, last).
 *
 * @param first The iterator
 * @param last  The sentinel denoting the end of the range the given iterator points to
 *
 * @throw std::length_error if the given range is too long for a valid UTF-8 character
 *
 * @return The Unicode code point
 */
template <std::input_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr unicode::CodePoint decode(Iterator first, Sentinel last) {
    auto code_point = utf8::decode_and_advance(first, last);

    if (first != last) {
        throw std::length_error{"The range is too long for a valid UTF-8 character."};
    }

    return code_point;
}

/**
 * Decode the UTF-8 character to an Unicode code point in the given range.
 *
 * @param range The UTF-8 character
 *
 * @throw std::length_error if the given range is too long for a valid UTF-8 character
 *
 * @return The Unicode code point
 */
template <std::ranges::input_range Range>
[[nodiscard]] constexpr unicode::CodePoint decode(Range&& range) {
    return utf8::decode(std::ranges::begin(range), std::ranges::end(range));
}

/**
 * Decode one or multiple UTF-8 characters from the given range [first, last), and copy the results to the given output.
 *
 * @param       first   The iterator
 * @param       last    The sentinel denoting the end of the range the given iterator points to
 * @param[out]  result  The beginning of the Unicode range destination
 *
 * @return An input iterator equal to the given last iterator, and an output iterator past the last element copied
 */
template <std::input_iterator Iterator, std::sentinel_for<Iterator> Sentinel, std::weakly_incrementable Output>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t> &&
             std::convertible_to<typename std::iter_value_t<Output>, unicode::CodePoint>
[[nodiscard]] constexpr utf8::decode_result<Iterator, Output> decode(Iterator first, Sentinel last, Output result) {
    while (first != last) {
        auto code_point = char32_t{0};
        auto const ending_state = utf8::internal::DFA::decode_and_advance_forward_once(first, last, code_point);

        if (ending_state != utf8::internal::DFA::State::eAccept) {
            break;
        }

        *result = static_cast<unicode::CodePoint::value_type>(code_point);
        ++result;
    }

    return {std::move(first), std::move(result)};
}

/**
 * Decode one or multiple UTF-8 characters from the given range, and copy the results to the given output.
 *
 * @param       range   The range of UTF-8 character(s)
 * @param[out]  result  The beginning of the Unicode range destination
 *
 * @return An input iterator equal to the given last iterator, and an output iterator past the last element copied
 */
template <std::ranges::input_range Range, std::weakly_incrementable Output>
    requires std::convertible_to<typename std::ranges::iterator_t<Range>, char8_t> &&
             std::convertible_to<typename std::iter_value_t<Output>, unicode::CodePoint>
[[nodiscard]] constexpr utf8::decode_result<std::ranges::borrowed_iterator_t<Range>, Output> decode(Range&& range,
                                                                                                    Output result) {
    return utf8::decode(std::ranges::begin(range), std::ranges::end(range), result);
}

/**
 * Copy the range of the given range [first, last), and replace any invalid UTF-8 character(s) with the given Unicode
 * code point encoded to UTF-8 to the given output.
 *
 * @note The default Unicode code point to replace an invalid UTF-8 character is the Unicode replacement character. This
 * character is the diamond with a question mark in the middle.
 *
 * @param       first                   The iterator
 * @param       last                    The sentinel denoting the end of the range the given iterator points to
 * @param[out]  result                  The beginning of the destination range
 * @param       replacement_code_point  The Unicode code point to replace an invalid UTF-8 character
 *
 * @return An input iterator equal to the given last iterator, and an output iterator past the last element copied
 */
template <std::input_iterator Iterator, std::sentinel_for<Iterator> Sentinel, std::weakly_incrementable Output>
    requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t> &&
             std::indirectly_copyable<Iterator, Output>
constexpr utf8::sanitize_result<Iterator, Output> sanitize(
    Iterator first, Sentinel last, Output result,
    unicode::CodePoint replacement_code_point = unicode::code_points::replacement_character) {
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

/**
 * Copy the range of the given range [first, last), and replace any invalid UTF-8 character(s) with the given Unicode
 * code point encoded to UTF-8 to the given output.
 *
 * @note The default Unicode code point to replace an invalid UTF-8 character is the Unicode replacement character. This
 * character is the diamond with a question mark in the middle.
 *
 * @param       range                   The range of UTF-8 character(s)
 * @param[out]  result                  The beginning of the destination range
 * @param       replacement_code_point  The Unicode code point to replace an invalid UTF-8 character
 *
 * @return An input iterator equal to the given last iterator, and an output iterator past the last element copied
 */
template <std::ranges::input_range Range, std::weakly_incrementable Output>
    requires std::indirectly_copyable<std::ranges::iterator_t<Range>, Output>
constexpr utf8::sanitize_result<std::ranges::borrowed_iterator_t<Range>, Output> sanitize(
    Range&& range, Output result,
    unicode::CodePoint replacement_code_point = unicode::code_points::replacement_character) {
    return sanitize(std::ranges::begin(range), std::ranges::end(range), result, replacement_code_point);
}

}  // namespace cpp20_v1

}  // namespace beetle::utf8

#endif
