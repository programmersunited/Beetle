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

#ifndef BEETLE_CODE_UNIT_EXCEPTION_HPP
#define BEETLE_CODE_UNIT_EXCEPTION_HPP

#ifdef __cpp_lib_format
#include <format>
#endif

#include "beetle/exception.hpp"

/**
 * Contains exceptions related to UTF-8 code units.
 */
namespace beetle {

inline namespace exceptions {

namespace utf8 {

// NOLINTNEXTLINE(readability-identifier-naming)
inline namespace cpp20_v1 {

class UnexpectedCodeUnit : public beetle::exceptions::cpp20_v1::Exception {
    public:
        using this_type = UnexpectedCodeUnit;
        using base_type = Exception;

        using base_type::what;

        explicit UnexpectedCodeUnit(char8_t code_unit, std::string msg);

#ifdef __cpp_lib_format
        template <class... Args>
        UnexpectedCodeUnit(char8_t code_unit, std::format_string<Args...> fmt, Args&&... args) 
            : Exception{code_unit, std::format(std::move(fmt), std::forward<Args>(args)...)}
#endif

        explicit UnexpectedCodeUnit(char8_t code_unit);

        [[nodiscard]] char8_t get_code_unit() const noexcept;

    private:
       char8_t m_code_unit; 
};

class ExpectingLeadingByte : public UnexpectedCodeUnit {
    public:
        using this_type = ExpectingLeadingByte;
        using base_type = UnexpectedCodeUnit;

        using base_type::base_type;
        using base_type::what;
        using base_type::get_code_unit;

        explicit ExpectingLeadingByte(char8_t code_unit);
};

}  // namespace cpp20_v1

}  // namespace utf8

}  // namespace exceptions

namespace utf8_exceptions = exceptions::utf8; // NOLINT

}  // namespace beetle

#endif

