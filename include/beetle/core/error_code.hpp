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

#ifndef BEETLE_CORE_ERROR_CODE_HPP
#define BEETLE_CORE_ERROR_CODE_HPP

#include <compare>
#include <concepts>
#include <string>
#include <type_traits>
#include <utility>

/**
 * An enumeration based error code `constexpr` alternative to std::error_code.
 *
 * @file error_code.hpp
 */

namespace beetle {

inline namespace v1 {

/**
 * An enumeration concept required for ErrorCode.
 *
 * @see beetle::ErrorCode
 *
 * @tparam EnumType The enumeration type for the error code
 */
template <typename EnumType>
concept enum_error_code = requires(EnumType enum_error_code) {
                              std::is_enum_v<EnumType>;
                              { message(enum_error_code) } -> std::convertible_to<std::string>;
                          };

/**
 * An enumeration based error code.
 *
 * @note This was created to be an `constexpr` alternative to std::error_code.
 *
 * @tparam EnumType The enumeration type
 * @tparam NoError  The enumeration value that represents no error
 */
template <typename EnumType, EnumType NoError>
    requires enum_error_code<EnumType>
class ErrorCode {
   public:
    /**
     * The enumeration type for this ErrorCode.
     */
    using value_type = EnumType;

    /**
     * This type is used to reference this ErrorCode.
     *
     * @note This is used to help simplify the API.
     */
    using this_type = ErrorCode<value_type, NoError>;

    /**
     * Default constructor.
     *
     * @note Default constructs this ErrorCode to the no error value.
     */
    constexpr ErrorCode() noexcept = default;

    /**
     * Construct an error code using the given enumerated value.
     *
     * @param value The enumeration value
     */
    constexpr explicit ErrorCode(value_type value) noexcept : m_value{value} {}

    /**
     * Copy-assignment operator.
     *
     * @param value Assign the enumeration value to this ErrorCode
     *
     * @return A reference to this ErrorCode
     */
    constexpr value_type& operator=(value_type other) noexcept {
        this->m_value = other;

        return *this;
    }

    /**
     * Check if this ErrorCode contains an error enumeration value.
     *
     * @return True if error value, otherwise false
     */
    [[nodiscard]] constexpr explicit operator bool() const noexcept { return this->m_value != NoError; }

    /**
     * Enumeration conversion operator.
     *
     * @return The underlying enumeration value from this ErrorCode
     */
    [[nodiscard]] constexpr operator value_type() const noexcept { return this->m_value; }

    /**
     * Swap the underlying enumeration value with the given value.
     *
     * @param[in, out] The enumeration value to swap with
     */
    constexpr void swap(value_type& enum_value) noexcept { std::swap(this->m_value, enum_value); }

    /**
     * Swap this ErrorCode with the given ErrorCode.
     *
     * @param[in, out] other The ErrorCode to swap with
     */
    constexpr void swap(this_type& other) noexcept { std::swap(*this, other); }

   private:
    /**
     * The enumeration value representing the error.
     */
    value_type m_value{NoError};
};

/**
 * Three-way comparison between the two given ErrorCodes.
 *
 * @tparam EnumType The enumeration error code type
 * @tparam NoError  The enumeration value that represents no error
 *
 * @param lhs Left-hand side error code to compare
 * @param rhs Right-hand side error code to compare
 *
 * @return lhs <=> rhs
 */
template <typename EnumType, EnumType NoError>
[[nodiscard]] constexpr auto operator<=>(ErrorCode<EnumType, NoError> lhs, ErrorCode<EnumType, NoError> rhs) noexcept {
    return static_cast<EnumType>(lhs) <=> static_cast<EnumType>(rhs);
}

/**
 * Three-way comparison between the given ErrorCode and the given enumeration error code.
 *
 * @tparam EnumType The enumeration error code type
 * @tparam NoError  The enumeration value that represents no error
 *
 * @param lhs Left-hand side error code to compare
 * @param rhs Right-hand side error enumeration to compare
 *
 * @return lhs <=> rhs
 */
template <typename EnumType, EnumType NoError>
[[nodiscard]] constexpr auto operator<=>(ErrorCode<EnumType, NoError> lhs, EnumType rhs) noexcept {
    return static_cast<EnumType>(lhs) <=> static_cast<EnumType>(rhs);
}

/**
 * Three-way comparison between the given ErrorCode and the given enumeration error code.
 *
 * @tparam EnumType The enumeration error code type
 * @tparam NoError  The enumeration value that represents no error
 *
 * @param lhs Left-hand side error enumeration to compare
 * @param rhs Right-hand side error code to compare
 *
 * @return lhs <=> rhs
 */
template <typename EnumType, EnumType NoError>
[[nodiscard]] constexpr auto operator<=>(EnumType lhs, ErrorCode<EnumType, NoError> rhs) noexcept {
    return static_cast<EnumType>(lhs) <=> static_cast<EnumType>(rhs);
}

/**
 * Comparison between the two given ErrorCodes.
 *
 * @tparam EnumType The enumeration error code type
 * @tparam NoError  The enumeration value that represents no error
 *
 * @param lhs Left-hand side error code to compare
 * @param rhs Right-hand side error code to compare
 *
 * @return True if equal, otherwise false
 */
template <typename EnumType, EnumType NoError>
[[nodiscard]] constexpr bool operator==(ErrorCode<EnumType, NoError> lhs, ErrorCode<EnumType, NoError> rhs) noexcept {
    return static_cast<EnumType>(lhs) == static_cast<EnumType>(rhs);
}

/**
 * Comparison between the given ErrorCode and the given enumeration error code.
 *
 * @tparam EnumType The enumeration error code type
 * @tparam NoError  The enumeration value that represents no error
 *
 * @param lhs Left-hand side error code to compare
 * @param rhs Right-hand side enumeration error code to compare
 *
 * @return True if equal, otherwise false
 */
template <typename EnumType, EnumType NoError>
[[nodiscard]] constexpr bool operator==(ErrorCode<EnumType, NoError> lhs, EnumType rhs) noexcept {
    return static_cast<EnumType>(lhs) == rhs;
}

/**
 * Comparison between the given ErrorCode and the given enumeration error code.
 *
 * @tparam EnumType The enumeration error code type
 * @tparam NoError  The enumeration value that represents no error
 *
 * @param lhs Left-hand side error code to compare
 * @param rhs Right-hand side enumeration error code to compare
 *
 * @return True if equal, otherwise false
 */
template <typename EnumType, EnumType NoError>
[[nodiscard]] constexpr bool operator==(EnumType lhs, ErrorCode<EnumType, NoError> rhs) noexcept {
    return lhs == static_cast<EnumType>(rhs);
}

/**
 * Return the associated error message for the given ErrorCode.
 *
 * @note This calls the message implementation for the enumeration error code
 *
 * @tparam EnumType The enumeration error code type
 * @tparam NoError  The enumeration value that represents no error
 *
 * @param error_code The error code
 *
 * @return The error message
 */
template <typename EnumType, EnumType NoError>
[[nodiscard]] constexpr std::string message(ErrorCode<EnumType, NoError> error_code) {
    return message(static_cast<EnumType>(error_code));
}

/**
 * Convert the given ErrorCode to the given IntegerType.
 *
 * @tparam IntegerType  The integer type to convert to
 * @tparam EnumType     The enumeration error code type
 * @tparam NoError      The enumeration value that represents no error
 *
 * @param error_code The error code
 *
 * @return The integer representation of the given ErrorCode
 */
template <typename IntegerType, typename EnumType, EnumType NoError>
    requires std::integral<IntegerType>
[[nodiscard]] constexpr IntegerType to_integer(ErrorCode<EnumType, NoError> error_code) noexcept {
    return static_cast<IntegerType>(error_code);
}

/**
 * Set the given ErrorCode to the no error enumeration value.
 *
 * @tparam EnumType The enumeration error code type
 * @tparam NoError  The enumeration value that represents no error
 *
 * @param error_code The error code to clear
 */
template <typename EnumType, EnumType NoError>
constexpr void clear(ErrorCode<EnumType, NoError>& error_code) noexcept {
    error_code = NoError;
}

}  // namespace v1

}  // namespace beetle

#endif
