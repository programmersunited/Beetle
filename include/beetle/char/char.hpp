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
#include <ranges>

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

}  // namespace unsafe

template <beetle::utf8::code_unit_input_iterator InputIt, std::sentinel_for<InputIt> Sentinel>
[[nodiscard]] constexpr beetle::ssize calculate_length(InputIt first, Sentinel last) {
    beetle::ssize length{0};

    while (first != last && *first != '\0') {
        beetle::utf8::advance(first, 1);
        ++length;
    }

    return length;
}

}  // namespace cpp20_v1

}  // namespace beetle::utf8

#endif
