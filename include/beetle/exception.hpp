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

#ifndef BEETLE_EXCEPTION_HPP
#define BEETLE_EXCEPTION_HPP

#ifdef __cpp_lib_format
#include <format>
#endif

#include <stdexcept>
#include <string>

namespace beetle {

inline namespace exceptions {

// NOLINTNEXTLINE(readability-identifier-naming)
inline namespace cpp20_v1 {

/**
 * Base Beetle exception type.
 *
 * @note This should be used if the goal is to catch **ALL** exceptions thrown 
 * by Beetle.
 */
class Exception : public std::exception {
    public:
        using this_type = Exception;
        using base_type = std::exception;

        explicit Exception(std::string msg);

#ifdef __cpp_lib_format
        template <class... Args>
        Exception(std::format_string<Args...> fmt, Args&&... args) 
            : m_msg {std::format(std::move(fmt), std::forward<Args>(args)...)} {}
#endif

        Exception(this_type const& other) = default;

        Exception(this_type&& other) noexcept = default;

        ~Exception() override = default;

        this_type& operator=(this_type const& other) = default;

        this_type& operator=(this_type&& other) noexcept = default;

        [[nodiscard]] char const* what() const noexcept override;

    protected:
        std::string m_msg;
};

}  // namespace cpp20_v1

}  // namespace exceptions

}  // namespace beetle

#endif

