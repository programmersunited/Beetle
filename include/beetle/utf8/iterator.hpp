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
 * Iterate over UTF-8 code units.
 *
 * @note API based off of the C++20 standard <iterator> header for ranges including the niebloid design.
 * @note The unsafe version of the functions provide the same functionality with little to no validation.
 *
 * @file iterator.hpp
 */

namespace beetle::utf8 {

namespace unsafe {

// NOLINTNEXTLINE(readability-identifier-naming)
inline namespace cpp20_v1 {

struct next_fn;
struct prev_fn;
struct advance_fn;

struct next_fn {
    /**
     * Return an iterator pointing to the next UTF-8 character's leading byte.
     *
     * @note Undefined behavior if the given iterator does not point to a leading byte.
     * @note Undefined behavior if the given iterator does not point to a range long enough to contain the UTF-8
     * character measured by the leading byte.
     *
     * @param iterator The iterator
     *
     * @return An iterator pointing to the next UTF-8 character's leading byte
     */
    template <std::input_or_output_iterator Iterator>
        requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
    [[nodiscard]] constexpr Iterator operator()(Iterator iterator) const {
        return std::ranges::next(iterator, internal::unsafe::char_size_from_leading_byte(*iterator));
    }

    /**
     * Return an iterator pointing to the nth UTF-8 character's leading byte.
     *
     * @note Undefined behavior if the given iterator does not point to a leading byte.
     * @note Undefined behavior if the given iterator does not point to a range long enough to contain the UTF-8
     * character's measured by their leading byte.
     *
     * @param iterator  The iterator
     * @param n         The number of UTF-8 characters to advance
     *
     * @return An iterator pointing to the nth UTF-8 character's leading byte
     */
    template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
        requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
    [[nodiscard]] constexpr Iterator operator()(Iterator iterator, std::iter_difference_t<Iterator> n) const {
        utf8::unsafe::advance_fn(iterator, n);

        return iterator;
    }

    /**
     * Return an iterator pointing to the given bound.
     *
     * @note Undefined behavior if the given iterator does not point to a leading byte.
     * @note Undefined behavior if the given bound does not end on a leading byte or on a sentinel after a UTF-8
     * character.
     *
     * @param iterator  The iterator
     * @param bound     The sentinel denoting the end of the range the iterator points to
     *
     * @return The first iterator equivalent to the given bound
     */
    template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
        requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
    [[nodiscard]] constexpr Iterator operator()(Iterator iterator, Sentinel bound) const {
        utf8::unsafe::advance_fn(iterator, bound);

        return iterator;
    }

    /**
     * Return an iterator pointing to the nth UTF-8 character's leading byte or the bound, whichever is first.
     *
     * @note Undefined behavior if the given iterator does not point to a leading byte.
     * @note Undefined behavior if the given bound does not end on a leading byte or on a sentinel after a UTF-8
     * character.
     *
     * @param iterator  The iterator
     * @param n         The number of UTF-8 characters to advance
     * @param bound     The sentinel denoting the end of the range the iterator points to
     *
     * @return An iterator pointing to the nth UTF-8 character's leading byte, or the first iterator equivalent to
     * bound, whichever is first.
     */
    template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
        requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
    [[nodiscard]] constexpr Iterator operator()(Iterator iterator, std::iter_difference_t<Iterator> n,
                                                Sentinel bound) const {
        utf8::unsafe::advance_fn(iterator, n, bound);

        return iterator;
    }
};

struct prev_fn {
    /**
     * Return the predecessor of the given iterator
     *
     * @note Undefined behavior if the iterated UTF-8 character is not valid
     *
     * @param iterator The iterator
     *
     * @return the predecessor of the given iterator
     */
    template <std::bidirectional_iterator Iterator>
        requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
    [[nodiscard]] constexpr Iterator operator()(Iterator iterator) const {
        do {
            --iterator;
        } while (utf8::internal::is_continuation_byte(*iterator));

        return iterator;
    }

    /**
     * Return the nth predecessor of the given iterator
     *
     * @param iterator  The iterator
     * @param n         The number of UTF-8 characters the given iterator should be descended
     *
     * @return the nth predecessor of the given iterator
     */
    template <std::bidirectional_iterator Iterator>
        requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
    [[nodiscard]] constexpr Iterator operator()(Iterator iterator, std::iter_difference_t<Iterator> n) const {
        utf8::unsafe::advance_fn(iterator, -n);

        return iterator;
    }

    /**
     * Return the nth predecessor of the given iterator, or the first iterator equivalent to bound, whichever is first.
     *
     * @param iterator  The iterator
     * @param n         The number of UTF-8 characters the given iterator should be descended
     * @param bound     The iterator denoting the beginning of the range the given iterator points to
     *
     * @return the nth predecessor or the bound, whichever is first
     */
    template <std::bidirectional_iterator Iterator>
        requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
    [[nodiscard]] constexpr Iterator operator()(Iterator iterator, std::iter_difference_t<Iterator> n,
                                                Iterator bound) const {
        utf8::unsafe::advance_fn(iterator, -n, bound);

        return iterator;
    }
};

struct advance_fn {
    /**
     * Increment the given iterator by the given n times.
     *
     * @note Undefined behavior if the range contains any invalid UTF-8 characters.
     *
     * @param[in, out]  iterator  The iterator to be advanced
     * @param           n         The number of UTF-8 characters the given iterator should be traversed
     *
     * @return the difference between the given distance and the actual distance traversed
     */
    template <std::input_or_output_iterator Iterator>
        requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
    constexpr void operator()(Iterator& iterator, std::iter_difference_t<Iterator> n) const {
        while (n > 0) {
            --n;
            iterator = utf8::unsafe::next_fn(iterator);
        }

        if constexpr (std::bidirectional_iterator<Iterator>) {
            while (n < 0) {
                ++n;
                iterator = utf8::unsafe::prev_fn(iterator);
            }
        }
    }

    /**
     * Increment the given iterator by the given n times or until the first iterator is equal to the bound, whichever
     * comes first.
     *
     * @note Undefined behavior if the range [iterator, bound) contains any invalid UTF-8 characters.
     *
     * @param[in, out]  iterator  The iterator to be advanced
     * @param           bound     The iterator denoting the end of the range the given iterator points to
     */
    template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
        requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
    constexpr void operator()(Iterator& iterator, Sentinel bound) const {
        while (iterator != bound) {
            iterator = utf8::unsafe::next_fn(iterator);
        }
    }

    /**
     * Increment the given iterator by the given n times or until the first iterator is equal to the bound, whichever
     * comes first.
     *
     * @note Undefined behavior if the range [iterator, bound) contains any invalid UTF-8 characters.
     *
     * @param[in, out]  iterator  The iterator to be advanced
     * @param           n         The number of UTF-8 characters the given iterator should be traversed
     * @param           bound     The iterator denoting the end of the range the given iterator points to
     *
     * @return the difference between the given distance and the actual distance traversed
     */
    template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
        requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
    constexpr std::iter_difference_t<Iterator> operator()(Iterator& iterator, std::iter_difference_t<Iterator> n,
                                                          Sentinel bound) const {
        while (n > 0 && iterator != bound) {
            --n;
            iterator = utf8::unsafe::next_fn(iterator);
        }

        if constexpr (std::bidirectional_iterator<Iterator>) {
            while (n < 0 && iterator != bound) {
                ++n;
                iterator = utf8::unsafe::prev_fn(iterator);
            }
        }

        return n;
    }
};

/**
 * Increment a UTF-8 character iterator by given distance or to a bound
 *
 * @note The iterator will always point to the UTF-8 character's leading byte or the bound
 * @note UTF-8 character validation is not performed
 */
inline constexpr next_fn const next{};

/** Decrement a UTF-8 character iterator by given distance or to a bound
 *
 * @note The iterator will always point to the UTF-8 character's leading byte or the bound
 * @note UTF-8 character validation is not performed
 */
inline constexpr prev_fn const prev{};

/**
 * Advances a UTF-8 character iterator by given distance or to a given bound
 *
 * @note The iterator will always point to the UTF-8 character's leading byte or the bound
 * @note UTF-8 character validation is not performed
 */
inline constexpr advance_fn const advance{};

}  // namespace cpp20_v1

}  // namespace unsafe

// NOLINTNEXTLINE(readability-identifier-naming)
inline namespace cpp20_v1 {

struct next_once_fn;
struct next_fn;

struct prev_once_fn;
struct prev_fn;

struct advance_fn;

struct next_once_fn {
    /**
     * Return an iterator pointing to the next UTF-8 character's leading byte.
     *
     * @param iterator The iterator
     *
     * @throw std::error_condition containing the relevant error code if any invalid UTF-8 character is detected
     *
     * @return The successor of the given iterator or bound, whichever is first
     */
    template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
        requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
    [[nodiscard]] constexpr Iterator operator()(Iterator iterator, Sentinel bound) const {
        auto const ending_state = utf8::internal::DFA::advance_forward_once(iterator, bound);

        if (ending_state != utf8::internal::DFA::State::eAccept) {
            throw utf8::internal::DFA::make_error_condition(ending_state);
        }

        return iterator;
    }
};

struct next_fn {
    /**
     * Return an iterator pointing to the next UTF-8 character's leading byte.
     *
     * @note Undefined behavior if the given iterator does not point to a leading byte.
     * @note Undefined behavior if the given iterator does not point to a range long enough to contain the UTF-8
     * character measured by the leading byte.
     *
     * @param iterator The iterator to increment
     *
     * @throw std::error_condition containing the relevant error code if any invalid UTF-8 character is detected
     *
     * @return An iterator pointing to the next UTF-8 character's leading byte
     */
    template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
        requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
    [[nodiscard]] constexpr Iterator operator()(Iterator iterator, Sentinel bound) const {
        utf8::advance_fn(iterator, bound);

        return iterator;
    }

    /**
     * Return an iterator pointing to the nth UTF-8 character's leading byte or the bound, whichever is first.
     *
     * @param iterator  The iterator
     * @param n         The number of UTF-8 characters to advance
     * @param bound     The sentinel denoting the end of the range the iterator points to
     *
     * @throw std::error_condition containing the relevant error code if any invalid UTF-8 character is detected
     *
     * @return An iterator pointing to the nth UTF-8 character's leading byte, or the first iterator equivalent to
     * bound, whichever is first.
     */
    template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
        requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
    [[nodiscard]] constexpr Iterator operator()(Iterator iterator, std::iter_difference_t<Iterator> n,
                                                Sentinel bound) const {
        utf8::advance_fn(iterator, n, bound);

        return iterator;
    }
};

struct prev_once_fn {
    /**
     * Return the predecessor of the given iterator
     *
     * @note Undefined behavior if the iterated UTF-8 character is not valid
     *
     * @param iterator  The iterator
     * @param bound     The iterator denoting the beginning of the range the given iterator points to
     *
     * @throw std::error_condition containing the relevant error code if any invalid UTF-8 character is detected
     *
     * @return the predecessor of the given iterator
     */
    template <std::bidirectional_iterator Iterator>
        requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
    [[nodiscard]] constexpr Iterator operator()(Iterator iterator, Iterator bound) const {
        // Could point to leading byte or std::end(iterator)
        // Needs to point at the end of a possible UTF-8 character
        --iterator;

        auto const ending_state = utf8::internal::DFA::advance_backward_once(iterator, bound);

        if (ending_state != utf8::internal::DFA::State::eAccept) {
            throw utf8::internal::DFA::make_error_condition(ending_state);
        }

        return iterator;
    }
};

struct prev_fn {
    /**
     * Return the nth predecessor of the given iterator, or the first iterator equivalent to bound, whichever is first.
     *
     * @param iterator  The iterator
     * @param n         The number of UTF-8 characters the given iterator should be descended
     * @param bound     The iterator denoting the end of the range the given iterator points to
     *
     * @throw std::error_condition containing the relevant error code if any invalid UTF-8 character is detected
     *
     * @return the nth predecessor or the bound, whichever is first
     */
    template <std::bidirectional_iterator Iterator>
        requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
    [[nodiscard]] constexpr Iterator operator()(Iterator iterator, std::iter_difference_t<Iterator> n,
                                                Iterator bound) const {
        utf8::advance_fn(iterator, -n, bound);

        return iterator;
    }
};

struct advance_fn {
    /**
     * Increment the given iterator by the given n times or until the first iterator is equal to the bound, whichever
     * comes first.
     *
     * @param[in, out]  iterator  The iterator to be advanced
     * @param           bound     The iterator denoting the end of the range the given iterator points to
     *
     * @throw std::error_condition containing the relevant error code if any invalid UTF-8 character is detected
     */
    template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
        requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
    constexpr void operator()(Iterator& iterator, Sentinel bound) const {
        while (iterator != bound) {
            iterator = utf8::next_once_fn(iterator, bound);
        }
    }

    /**
     * Increment the given iterator by the given n times or until the first iterator is equal to the bound, whichever
     * comes first.
     *
     * @param[in, out]  iterator  The iterator to be advanced
     * @param           n         The number of UTF-8 characters the given iterator should be traversed
     * @param           bound     The iterator denoting the end of the range the given iterator points to
     *
     * @throw std::error_condition containing the relevant error code if any invalid UTF-8 character is detected
     *
     * @return the difference between the given distance and the actual distance traversed
     */
    template <std::input_or_output_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
        requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
    constexpr std::iter_difference_t<Iterator> operator()(Iterator& iterator, std::iter_difference_t<Iterator> n,
                                                          Sentinel bound) const {
        while (n > 0 && iterator != bound) {
            --n;
            iterator = utf8::next_once_fn(iterator, bound);
        }

        if constexpr (std::bidirectional_iterator<Iterator>) {
            while (n < 0 && iterator != bound) {
                ++n;
                iterator = utf8::prev_once_fn(iterator, bound);
            }
        }

        return n;
    }
};

/**
 * Increment a UTF-8 character iterator by one
 *
 * @note The iterator will always point to the UTF-8 character's leading byte
 */
inline constexpr next_once_fn const next_once{};

/**
 * Increment a UTF-8 character iterator by given distance or to a bound
 *
 * @note The iterator will always point to the UTF-8 character's leading byte or the bound
 */
inline constexpr next_fn const next{};

/**
 * Decrement a UTF-8 character iterator by one
 *
 * @note The iterator will always point to the UTF-8 character's leading byte
 */
inline constexpr prev_once_fn const prev_once{};

/**
 * Decrement a UTF-8 character iterator by given distance or to a bound
 *
 * @note The iterator will always point to the UTF-8 character's leading byte or the bound
 */
inline constexpr prev_fn const prev{};

/**
 * Advances a UTF-8 character iterator by given distance or to a given bound
 *
 * @note The iterator will always point to the UTF-8 character's leading byte or the bound
 */
inline constexpr advance_fn const advance{};

}  // namespace cpp20_v1

}  // namespace beetle::utf8

#endif
