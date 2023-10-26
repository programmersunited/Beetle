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

#ifndef BEETLE_UNICODE_ENDIAN_HPP
#define BEETLE_UNICODE_ENDIAN_HPP

#include <bit>

/**
 * Contains functionality related to endian encoding.
 */

namespace beetle {

inline namespace abi_cxx_v1 {

/**
 * A constant that checks if the current platform is mixed-endian.
 *
 * @note According to the standard it is possible to have mixed-endian.
 */
inline constexpr auto is_mixed_endian{std::endian::native != std::endian::big &&
                                      std::endian::native != std::endian::little};

}  // namespace abi_cxx_v1

namespace unicode {

inline namespace abi_cxx_v1 {

/**
 * The default endian encoding used when not explicitly specified.
 *
 * @note RFC 2781 recommends that big-endian encoding be assumed.
 */
inline constexpr auto default_endian{std::endian::big};

}  // namespace abi_cxx_v1

}  // namespace unicode

inline namespace abi_cxx_v1 {

/**
 * A shorthand alias for the unicode namespace.
 */
namespace uc = unicode;  // NOLINT

}  // namespace abi_cxx_v1

}  // namespace beetle

#endif
