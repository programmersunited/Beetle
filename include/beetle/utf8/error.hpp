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

#ifndef BEETLE_UTF8_ERROR_HPP
#define BEETLE_UTF8_ERROR_HPP

#include <string>
#include <system_error>
#include <type_traits>

namespace beetle {

namespace utf8 {

// NOLINTNEXTLINE(readability-identifier-naming)
inline namespace cpp20_v1 {

enum class Error {
    /**
     * Expecting a leading byte in the UTF-8 character.
     */
    eLeadingByte = 1,

    /**
     * UTF-8 character is overlong encoded.
     */
    eOverlongEncoded,

    /**
     * Expecting a continuation byte in the UTF-8 character.
     */
    eContinuationByte,

    /**
     * More byte(s) are expected in the UTF-8 character.
     */
    eMissingByte

};

}  // namespace cpp20_v1

}  // namespace utf8

// NOLINTNEXTLINE(readability-identifier-naming)
inline namespace cpp20_v1 {

/**
 * Convert the given error into a string.
 *
 * @param error The error enumeration to stringify
 *
 * @return The string representation of the given error
 */
[[nodiscard]] constexpr std::string to_string(utf8::Error error) {
    switch (error) {
        case utf8::Error::eLeadingByte:
            return "Leading Byte";
            break;

        case utf8::Error::eOverlongEncoded:
            return "Overlong Encoded";
            break;

        case utf8::Error::eContinuationByte:
            return "Continuation Byte";
            break;

        case utf8::Error::eMissingByte:
            return "Missing Byte";
            break;

        default:
            return "Unknown";
    }
}

/**
 * Create an error condition from the given UTF-8 error value.
 *
 * @param error The UTF-8 error code
 *
 * @return The error condition for the given error
 */
[[nodiscard]] std::error_condition make_error_condition(utf8::Error error) noexcept;

}  // namespace cpp20_v1

}  // namespace beetle

namespace std {

template <>
struct is_error_condition_enum<beetle::utf8::Error> : std::true_type {};

}  // namespace std

#endif
