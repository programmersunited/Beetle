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

#ifndef BEETLE_CORE_INTERNAL_ASSERT_HPP
#define BEETLE_CORE_INTERNAL_ASSERT_HPP

#include <optional>
#include <source_location>
#include <string_view>

#include "core/internal/macro_helpers.hpp"

/**
 * @file assert.hpp
 */

namespace beetle::internal {

// NOLINTNEXTLINE(readability-identifier-naming)
inline namespace cpp20_v1 {

[[nodiscard]] std::string create_error_message(std::string_view assertion_text,
                                               std::optional<std::string_view> message = std::nullopt,
                                               std::source_location const location = std::source_location::current());

struct assertion_failure {
    explicit assertion_failure(std::string_view assertion_text, std::optional<std::string_view> message = std::nullopt,
                               std::source_location const location = std::source_location::current());
};

inline constexpr auto assertion(bool condition, std::string_view assertion_text,
                                std::optional<std::string_view> message = std::nullopt,
                                std::source_location const location = std::source_location::current()) {
    if (!condition) {
        throw assertion_failure{assertion_text, message, location};
    }
}

}  // namespace cpp20_v1

}  // namespace beetle::internal

// NOLINTBEGIN(cppcoreguidelines-macro-usage)

#ifdef BEETLE_DEBUG

#define BEETLE_ASSERT_2(ASSERTION, MESSAGE) \
    beetle::internal::assertion(ASSERTION, BEETLE_STR(ASSERTION), MESSAGE, std::source_location::current());

#define BEETLE_ASSERT_1(ASSERTION) BEETLE_ASSERT_2(ASSERTION, std::nullopt)

#define BEETLE_ASSERT(...) BEETLE_VA_SELECT(BEETLE_ASSERT, __VA_ARGS__)
#else
#define BEETLE_ASSERT(...)
#endif

// NOLINTEND(cppcoreguidelines-macro-usage)

#endif
