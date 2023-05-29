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

#ifndef BEETLE_UTF8_EXCEPTION_HPP
#define BEETLE_UTF8_EXCEPTION_HPP

#include <optional>
#include <string>

#include "beetle/core/exception.hpp"
#include "beetle/utf8/error.hpp"

/**
 * UTF-8 exception class for the Beetle library.
 *
 * @file exception.hpp
 */

namespace beetle {

namespace utf8 {

inline namespace v1 {

/**
 * Defines a type for a UTF-8 exception.
 */
class Exception : public beetle::Exception {
   public:
    using base_type = beetle::Exception;

    /**
     * Construct an Exception using the given what message, UTF-8 string and the associated error code.
     *
     * @param what_message  The explanatory string of the error
     * @param utf8_str      The UTF-8 string containing the UTF-8 error
     * @param error_code    The error code associated with the given UTF-8 string
     */
    Exception(std::string what_message, std::optional<std::u8string> utf8_str, utf8::ErrorCode error_code)
        : base_type{std::move(what_message)}, m_str{std::move(utf8_str)}, m_error_code{error_code} {}

    /**
     * Construct an Exception using the given what message and the associated error code.
     *
     * @param what_message  The explanatory string of the error
     * @param error_code    The error code associated with the given UTF-8 string
     */
    Exception(std::string what_message, utf8::ErrorCode error_code)
        : Exception{std::move(what_message), std::nullopt, error_code} {}

    /**
     * Destructor.
     */
    ~Exception() override = default;

    /**
     * Return the invalid UTF-8 string, empty if one was not provided.
     *
     * @return The invalid UTF-8 string
     */
    [[nodiscard]] std::optional<std::u8string> const& get_utf8_str() const& noexcept { return this->m_str; }

    /**
     * Return the error code associated with the invalid UTF-8 string.
     *
     * @return The error code
     */
    [[nodiscard]] utf8::ErrorCode get_error_code() const noexcept { return this->m_error_code; }

   private:
    /**
     * The invalid UTF-8 string.
     */
    std::optional<std::u8string> m_str{std::nullopt};

    /**
     * The error code associated with the invalid UTF-8 string.
     */
    utf8::ErrorCode m_error_code;
};

}  // namespace v1

}  // namespace utf8

}  // namespace beetle

#endif
