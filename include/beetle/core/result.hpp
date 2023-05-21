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

#ifndef BEETLE_CORE_RESULT_HPP
#define BEETLE_CORE_RESULT_HPP

#include <system_error>
#include <utility>
#include <variant>

namespace beetle {

// NOLINTNEXTLINE(readability-identifier-naming)
inline namespace cpp20_v1 {

/**
 * In-place construction tag for error value in Result.
 */
struct error_t {};

/**
 *
 *
 * @note To be replaced by std::expected when it's supported by more compilers.
 */
template <typename ResultType, typename ErrorType>
class Result {
   public:
    using result_type = ResultType;

    using error_type = ErrorType;

    using this_type = Result<ResultType, ErrorType>;

    constexpr Result(result_type const& value) noexcept(noexcept(m_value(value))) : m_value{value} {}

    constexpr Result(result_type&& value) noexcept(noexcept(m_value(std::move(value)))) : m_value{std::move(value)} {}

    constexpr Result(error_type const& value) noexcept(noexcept(m_value(value))) : m_value{value} {}

    constexpr Result(error_type&& value) noexcept(noexcept(m_value(std::move(value)))) : m_value{std::move(value)} {}

    template <typename... Args>
    constexpr Result(std::in_place_t, Args&&... args) noexcept(noexcept(m_value(std::in_place_type_t<result_type>{},
                                                                                std::forward<Args>(args)...)))
        : m_value(std::in_place_type_t<result_type>{}, std::forward<Args>(args)...) {}

    template <typename... Args>
    constexpr Result(beetle::error_t, Args&&... args) noexcept(noexcept(m_value(std::in_place_type_t<error_type>{},
                                                                                std::forward<Args>(args)...)))
        : m_value(std::in_place_type_t<error_type>{}, std::forward<Args>(args)...) {}

    template <typename... Args>
    constexpr result_type& emplace(Args&&... args) {
        return this->m_value.emplace(std::forward<Args>(args)...);
    }

    [[nodiscard]] constexpr result_type& value() & { return std::get<result_type>(this->m_value); }

    [[nodiscard]] constexpr result_type const& value() const& { return std::get<result_type>(this->m_value); }

    [[nodiscard]] constexpr result_type&& value() && { return std::move(std::get<result_type>(this->m_value)); }

    [[nodiscard]] constexpr result_type const&& value() const&& {
        return std::move(std::get<result_type>(this->m_value));
    }

    template <typename OtherValue>
    [[nodiscard]] result_type value_or(OtherValue&& default_value) && {
        return bool(*this) ? std::move(*(*this)) : result_type(std::forward<OtherValue>(default_value));
    }

    template <typename OtherValue>
    [[nodiscard]] result_type value_or(OtherValue&& default_value) const& {
        return bool(*this) ? *(*this) : result_type(std::forward<OtherValue>(default_value));
    }

    [[nodiscard]] constexpr bool has_value() const noexcept {
        return std::holds_alternative<result_type>(this->m_value);
    }

    [[nodiscard]] constexpr error_type& error() & { return std::get<error_type>(this->m_value); }

    [[nodiscard]] constexpr error_type const& error() const& { return std::get<error_type>(this->m_value); }

    [[nodiscard]] constexpr error_type&& error() && { return std::move(std::get<error_type>(this->m_value)); }

    [[nodiscard]] constexpr error_type const&& error() const&& {
        return std::move(std::get<error_type>(this->m_value));
    }

    constexpr result_type* operator->() noexcept { return std::get<result_type>(this->m_value); }

    constexpr result_type const* operator->() const noexcept { return std::get<result_type>(this->m_value); }

    constexpr result_type* operator*() noexcept { return std::get<result_type>(this->m_value); }

    constexpr result_type const* operator*() const noexcept { return std::get<result_type>(this->m_value); }

    [[nodiscard]] constexpr bool operator==(this_type const& other) const noexcept { return this->m_value == other; }

    [[nodiscard]] constexpr bool operator!=(this_type const& other) const noexcept { return !(*this == other); }

    [[nodiscard]] constexpr explicit operator bool() const noexcept { return this->has_value(); }

    [[nodiscard]] constexpr this_type& swap(this_type& other) noexcept(noexcept(std::swap(this->m_value,
                                                                                          other.m_value))) {
        std::swap(this->m_value, other.m_value);
    }

   private:
    std::variant<result_type, error_type> m_value{};
};

}  // namespace cpp20_v1

}  // namespace beetle

#endif
