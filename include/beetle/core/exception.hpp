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

#ifndef BEETLE_CORE_EXCEPTION_HPP
#define BEETLE_CORE_EXCEPTION_HPP

#include <stdexcept>
#include <string>

#ifdef __cpp_lib_format
#include <format>
#endif

/**
 * Base exception class for the Beetle library.
 *
 * @file exception.hpp
 */

namespace beetle {

inline namespace v1 {

/**
 * Defines a base exception class.
 *
 * @note This is supposed to be used as a way to catch all exceptions thrown from the Beetle library.
 */
class Exception : public std::exception {
   public:
    using base_type = std::exception;

    /**
     * Constructs an exception using the given message.
     *
     * @param message The message for the exception
     */
    Exception(std::string message) : m_message{std::move(message)} {}

#ifdef __cpp_lib_format
    /**
     * Constructs an exception using the given formatted message.
     *
     * @param fmt   An object that represents the format string
     * @param args  The arguments to be formatted
     */
    template <typename... Args>
    Exception(std::format_string<Args...> fmt, Args&&... args)
        : m_message{std::format(fmt, std::forward<Args>(args)...)} {}
#endif

    ~Exception() override = default;

    [[nodiscard]] const char* what() const noexcept override { return this->m_message.c_str(); }

   private:
    /**
     * The exception message.
     */
    std::string m_message;
};

}  // namespace v1

}  // namespace beetle

#endif
