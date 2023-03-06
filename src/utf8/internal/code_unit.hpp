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

#ifndef BEETLE_UTF8_INTERNAL_CODE_UNIT_HPP
#define BEETLE_UTF8_INTERNAL_CODE_UNIT_HPP

#include <bit>
#include <cstdint>

/**
 * Contains ways to inspect a single UTF-8 code unit.
 *
 * @file code_unit.hpp
 */

namespace beetle::utf8::internal {

// NOLINTNEXTLINE(readability-identifier-naming)
inline namespace cpp20_v1 {

/**
 * Checks if the given code unit is ASCII.
 *
 * @param code_unit The code unit to check
 *
 * @return True if ASCII otherwise false
 */
[[nodiscard]] constexpr bool is_ascii(char8_t code_unit) noexcept {
    // 0xxx xxxx
    return code_unit < 0x80U;
}

[[nodiscard]] constexpr bool is_continuation_byte(char8_t code_unit) noexcept {
    // 10xx xxxx
    return (code_unit & 0xC0U) == 0x80U;
}

namespace unsafe {

/**
 * Returns the *possible* UTF-8 character size based on the given leading byte.
 *
 * @precondition The given UTF-8 code unit is a leading byte.
 *
 * @param code_unit
 *
 * return UTF-8 character size
 */
[[nodiscard]] constexpr std::int8_t char_size_from_leading_byte(char8_t code_unit) noexcept {
    // TODO: Change to custom Beetle assert
    // assert(utf8::is_leading_byte(code_unit));

    // --std c++20 -O2
    //
    // return ((~(code_unit & 0x80)) >> 7) + std::countl_zero(static_cast<unsigned char>(~code_unit));
    //
    // return ((code_unit >> 7) ^ 0x1) + std::countl_zero(static_cast<unsigned char>(~code_unit));
    //
    // return std::countl_one(static_cast<unsigned char>(code_unit ^ 0x80)) + std::countl_zero(static_cast<unsigned
    // char>(~code_unit));
    //

    return static_cast<std::int8_t>(is_ascii(code_unit) + std::countl_zero(static_cast<unsigned char>(~code_unit)));
}

}  // namespace unsafe

}  // namespace cpp20_v1

}  // namespace beetle::utf8::internal

#endif
