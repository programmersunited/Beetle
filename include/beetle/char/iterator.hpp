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

#ifndef BEETLE_ITERATOR_HPP
#define BEETLE_ITERATOR_HPP

#include <algorithm>
#include <ranges>
#include <stdexcept>

#include "beetle/assert.hpp"
#include "beetle/code_unit.hpp"
#include "beetle/validate.hpp"

/**
 * Contains algorithms to inspect a sequence of code units.
 *
 * @file validate.hpp
 */

namespace beetle::utf8 {

// NOLINTNEXTLINE(readability-identifier-naming)
inline namespace cpp20_v1 {

namespace unsafe {

/**
 * Iterates to the first code unit of a valid UTF-8 character.
 *
 * @param iterator The iterator to advance
 *
 * @precondition iterator Currently points to a leading code unit byte
 * @precondition iterator Iterates over a valid UTF-8 character within the 
 *                        range [iterator, iterator + leading byte size)
 */
template <std::input_or_output_iterator Iterator>
requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator next(Iterator iterator) {
    // This will throw if not leading byte
    auto const size = beetle::utf8::leading_byte_size(*iterator);
    auto const sentinel = std::ranges::next(iterator, size);

    return std::ranges::next(iterator, sentinel);
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
 */
template <std::input_or_output_iterator Iterator>
requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
[[nodiscard]] constexpr Iterator next(Iterator iterator) {
    // This will throw if not leading byte
    auto const size = beetle::utf8::leading_byte_size(*iterator);
    auto const sentinel = std::ranges::next(iterator, size);

    beetle::assert(beetle::utf8::is_valid_character(*iterator, sentinel),
        "@precondition iterator Iterates over a valid UTF-8 character.");

    return std::ranges::next(iterator, sentinel);
}

}  // namespace cpp20_v1

}  // namespace beetle::utf8

#endif

