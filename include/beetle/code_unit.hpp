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

#ifndef BEETLE_CODE_UNIT_CODE_UNIT_HPP
#define BEETLE_CODE_UNIT_CODE_UNIT_HPP

#include <cstdint>
#include <optional>

#include "beetle/code_unit/exception.hpp"

/**
 * Contains ways to inspect a single UTF-8 code unit.
 *
 * @file code_unit.hpp
 */

namespace beetle::utf8 {

// NOLINTNEXTLINE(readability-identifier-naming)
inline namespace cpp20_v1 {

/**
 * The first leading byte value.
 *
 * @note This should not be used in a range to check if a value is a leading
 * byte.
 *
 * @see beetle::is_leading_byte
 * @see beetle::is_leading_multiple_bytes
 */
inline constexpr auto g_firstLeadingByte = char8_t{0};              // 0xxx xxxx

/**
 * The last leading byte value.
 *
 * @note This should not be used in a range to check if a value if a leading
 * byte.
 *
 * @see beetle::is_leading_byte
 * @see beetle::is_leading_multiple_bytes
 */
inline constexpr auto g_lastLeadingByte = char8_t{0b1111'0111};    // 1111 0xxx

/**
 * Checks if the given code unit is ASCII.
 *
 * @param code_unit The code unit to check
 *
 * @return True if ASCII otherwise false
 */
[[nodiscard]] constexpr auto is_ascii(char8_t code_unit) noexcept -> bool {
    // Check if format is in 0xxx xxxx
    return (code_unit & 0x80U) == 0x00U; // NOLINT(*-magic-numbers)
}

/**
 * Checks if the given code unit is *not* ASCII.
 *
 * @param code_unit The code unit to check
 *
 * @return True if not ASCII otherwise false
 */
[[nodiscard]] constexpr auto is_not_ascii(char8_t code_unit) noexcept -> bool {
    return !is_ascii(code_unit);
}

/**
 * Checks if the given UTF-8 code unit is a leading byte and not ASCII.
 *
 * @param code_unit The code unit to check
 *
 * @return True if a leading byte and not ASCII, otherwise false.
 */
[[nodiscard]] constexpr auto is_leading_multiple_bytes(
    char8_t code_unit) noexcept -> bool {
    // NOLINTNEXTLINE(*-magic-numbers)
    return code_unit >= 0b1100'0000 && code_unit <= g_lastLeadingByte;
}

/**
 * Checks if the given UTF-8 code unit is a leading byte.
 *
 * @param code_unit The code unit to check
 *
 * @return True if the given code unit is a leading byte, otherwise false
 */
[[nodiscard]] constexpr auto is_leading_byte(char8_t code_unit) noexcept -> bool {
    return is_ascii(code_unit) || is_leading_multiple_bytes(code_unit);
}

/**
 * Checks if the given code unit is a continuation byte.
 *
 * @note A continuation byte has the following format: 10xx xxxx
 *
 * @param code_unit The code unit to check
 *
 * @return True if a continuation byte, otherwise false
 */
[[nodiscard]] constexpr auto is_continuation_byte(char8_t code_unit) noexcept -> bool {
    return (code_unit & 0xC0U) == 0x80U; // NOLINT(*-magic-numbers)
}

/**
 * Checks if the given code unit is a valid UTF-8 character code unit.
 *
 * @param code_unit The code unit to check
 *
 * @return True if the given code unit is a valid UTF-8 character code unit
 * otherwise false
 */
[[nodiscard]] constexpr auto is_valid_byte(char8_t code_unit) noexcept -> bool {
    return is_continuation_byte(code_unit) || is_leading_byte(code_unit);
}

/**
 * Checks if the given code unit is an invalid UTF-8 code unit.
 *
 * @param code_unit The code unit to check
 *
 * @return True if the given code unit is an invalid UTF-8 character code unit
 * otherwise false
 */
[[nodiscard]] constexpr auto is_invalid_byte(char8_t code_unit) noexcept -> bool {
    return !is_valid_byte(code_unit);
}

/**
 * 
 *
 * @note Does not check for overlong encoding.
 */
[[nodiscard]] constexpr auto is_leading_mb_2(char8_t code_unit) -> bool {
    // NOLINTNEXTLINE(*-magic-numbers)
    return (code_unit & 0xE0U) == 0xC0U; // 110x xxxx
}

/**
 * 
 *
 * @note Does not check for overlong encoding.
 */
[[nodiscard]] constexpr auto is_leading_mb_3(char8_t code_unit) -> bool {
    // NOLINTNEXTLINE(*-magic-numbers)
    return (code_unit & 0xF0U) == 0xE0U; // 1110 xxxx
}

/**
 * 
 *
 * @note Does not check for overlong encoding.
 */
[[nodiscard]] constexpr auto is_leading_mb_4(char8_t code_unit) -> bool {
    // NOLINTNEXTLINE(*-magic-numbers)
    return (code_unit & 0xF8U) == 0xF0U; // 1111 0xxx
}

/**
 * Returns the size of the leading code unit byte.
 *
 * @precondition The given code unit is a leading byte.
 *
 * @throws std::invalid_argument if the given code unit is not a leading byte
 * code unit.
 *
 * @return The size of the leading byte code unit.
 */
[[nodiscard]] constexpr auto leading_byte_size(char8_t code_unit) -> std::int8_t {
    // NOLINTBEGIN(*-around-statements)
    if (is_ascii(code_unit))        return 1;
    if (is_leading_mb_2(code_unit)) return 2;
    if (is_leading_mb_3(code_unit)) return 3;
    if (is_leading_mb_4(code_unit)) return 4;
    // NOLINTEND(*-around-statements)

    throw beetle::exceptions::utf8::ExpectingLeadingByte{code_unit};
}

/**
 * Returns the *possible* size of the UTF-8 character based on the first
 * byte/code unit.
 *
 * @note This does not mean the following continuation bytes, if there are any,
 * will be valid.
 *
 * @param code_unit The UTF-8 code unit to check
 *
 * @return The size of the UTF-8 character based on the code unit if valid,
 * otherwise std::nullopt.
 */
[[nodiscard]] constexpr auto try_leading_byte_size(char8_t code_unit) noexcept -> std::optional<std::int8_t> {
    if (is_leading_byte(code_unit)) [[likely]] {
        return leading_byte_size(code_unit);
    }
    
    return std::nullopt;
}

/**
 * Returns the *possible* size of the UTF-8 character based on the first
 * byte/code unit.
 *
 * @note This does not mean the following continuation bytes, if there are any,
 * will be valid.
 *
 * @param code_unit The UTF-8 code unit to check
 *
 * @return The size of the UTF-8 character based on the code unit if valid,
 * otherwise std::nullopt.
 */
[[nodiscard]] constexpr auto peek_char_size(char8_t code_unit) noexcept -> std::optional<std::int8_t> {
    if (is_leading_byte(code_unit)) [[likely]] {
        return leading_byte_size(code_unit);
    }
    
    return std::nullopt;
}

}  // namespace cpp20_v1

}  // namespace beetle::utf8

#endif

