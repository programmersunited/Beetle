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

#include "beetle/core/error_code.hpp"

/**
 * Non-throwing error reporting for UTF-8.
 *
 * @file error.hpp
 */

namespace beetle {

namespace utf8 {

inline namespace v1 {

/**
 * UTF-8 character errors.
 */
enum class Error {
    /**
     * No error.
     */
    eNone = 0,

    /**
     * Expecting a leading byte in the UTF-8 character.
     */
    eLeadingByte,

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
    eMissingByte,

    /**
     * Unknown error.
     */
    eUnknown
};

/**
 * Return the associated error description for the given enumeration error code.
 *
 * @see beetle::ErrorCode
 * @see beetle::message
 *
 * @param error The enumeration error code
 *
 * @return The error description
 */
[[nodiscard]] constexpr std::string message(utf8::Error error) noexcept {
    switch (error) {
        case utf8::Error::eNone:
            return "none";
            break;

        case utf8::Error::eLeadingByte:
            return "expected a leading byte";
            break;

        case utf8::Error::eOverlongEncoded:
            return "detected overlong encoding";
            break;

        case utf8::Error::eContinuationByte:
            return "expected a continuation byte";
            break;

        case utf8::Error::eMissingByte:
            return "expected more bytes";
            break;

        default:
            return "unknown error";
    }
}

/**
 * UTF-8 error code.
 *
 * @note This is used for error reporting in the non-throwing overloads.
 *
 * @see beetle::ErrorCode
 */
using ErrorCode = beetle::ErrorCode<utf8::Error, utf8::Error::eNone>;

}  // namespace v1

}  // namespace utf8

inline namespace v1 {

/**
 * Convert the given error into a string.
 *
 * @param error The error enumeration to stringify
 *
 * @return The string representation of the given error
 */
[[nodiscard]] constexpr std::string to_string(utf8::Error error) {
    switch (error) {
        case utf8::Error::eNone:
            return "None";
            break;

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

}  // namespace v1

}  // namespace beetle

#endif
