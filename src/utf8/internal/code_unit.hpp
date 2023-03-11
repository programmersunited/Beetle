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

#include "core/internal/assert.hpp"

/**
 * Inspect and transform a single UTF-8 code unit.
 *
 * @file code_unit.hpp
 */

namespace beetle::utf8::internal {

// NOLINTNEXTLINE(readability-identifier-naming)
inline namespace cpp20_v1 {

/**
 * Check if the given code unit is ASCII.
 *
 * @param code_unit The code unit to check
 *
 * @return True if ASCII otherwise false
 */
[[nodiscard]] constexpr bool is_ascii(char8_t code_unit) noexcept {
    // 0xxx xxxx
    return code_unit < 0x80U;
}

/**
 * Check if the given code unit is a valid leading byte of a multi-byte UTF-8 character.
 *
 * @note 0xC0 and 0C1 are overlong encoded leading bytes. This will also return false for these two values.
 *
 * @param code_unit The UTF-8 code unit to check
 *
 * @return True if a leading byte of a multi-byte UTF-8 character otherwise false
 */
[[nodiscard]] constexpr bool is_mb_leading_byte(char8_t code_unit) noexcept {
    return code_unit >= 0xC2U && code_unit <= 0xF4U;
}

/**
 * Check if the given code unit is a valid leading byte of a UTF-8 character.
 *
 * @param code_unit The UTF-8 code unit to check
 *
 * @return True if leading byte, otherwise false
 */
[[nodiscard]] constexpr bool is_leading_byte(char8_t code_unit) noexcept {
    return is_ascii(code_unit) || is_mb_leading_byte(code_unit);
}

/**
 * Check if the given code unit is a continuation byte.
 *
 * @param code_unit
 *
 * @return True if continuation byte, otherwise false
 */
[[nodiscard]] constexpr bool is_continuation_byte(char8_t code_unit) noexcept {
    // 10xx xxxx
    return (code_unit & 0xC0U) == 0x80U;
}

/**
 * Decode the given UTF-8 continuation byte.
 *
 * @note An encoded continuation byte has the format: 10xx xxxx
 *
 * @param continuation_byte The continuation byte to decode
 *
 * @return 00xx xxxx
 */
[[nodiscard]] constexpr char8_t decode_continuation_byte(char8_t continuation_byte) noexcept {
    BEETLE_ASSERT(is_continuation_byte(continuation_byte));

    // 10xx xxxx
    return continuation_byte & 0x3FU;
}

namespace unsafe {

/**
 * Return the *possible* UTF-8 character size based on the given leading byte.
 *
 * @note Undefined behavior if the given UTF-8 code unit is a leading byte.
 *
 * @param code_unit
 *
 * return UTF-8 character size
 */
[[nodiscard]] constexpr std::int8_t char_size_from_leading_byte(char8_t code_unit) noexcept {
    BEETLE_ASSERT(utf8::internal::is_leading_byte(code_unit));

    return static_cast<std::int8_t>(is_ascii(code_unit) + std::countl_zero(static_cast<unsigned char>(~code_unit)));
}

}  // namespace unsafe

}  // namespace cpp20_v1

}  // namespace beetle::utf8::internal

#endif
