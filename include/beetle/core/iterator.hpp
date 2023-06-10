/*
 * MIT License
 *
 * Copyright (c) 2023 programmersunited
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

#ifndef BEETLE_CORE_ITERATOR_HPP
#define BEETLE_CORE_ITERATOR_HPP

#include "core/internal/assert.hpp"

/**
 * Provides generic iterator support for the Beetle library.
 *
 * @file iterator.hpp
 */

namespace beetle {

inline namespace v1 {

/**
 * An empty class type used to denote the end of a null-terminated string.
 *
 * @note This is supposed to be used in ranges as the sentinel for C-style strings.
 */
struct null_terminated_sentinel_t {};

/**
 * A constant of type null_terminated_sentinel_t.
 *
 * @see null_terminated_sentinel_t
 */
inline constexpr null_terminated_sentinel_t null_terminated_sentinel{};

/**
 * Check if the first character in the C-string is the null-character.
 *
 * @param c_str The C-string to check
 *
 * @return True if the first character is the null-character
 */
template <typename CharType>
[[nodiscard]] constexpr bool operator==(CharType const* c_str,
                                        null_terminated_sentinel_t) noexcept(noexcept(*c_str == '\0')) {
    BEETLE_ASSERT(c_str != nullptr, "C-string cannot be null.");

    return *c_str == '\0';
}

}  // namespace v1

}  // namespace beetle

#endif
