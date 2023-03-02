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

#ifndef BEETLE_CHAR_FIXED_CHARACTER_HPP
#define BEETLE_CHAR_FIXED_CHARACTER_HPP

#include <algorithm>
#include <array>
#include <iterator>
#include <ranges>
#include <string>

#include "beetle/char/iterator.hpp"
#include "beetle/char/validate.hpp"
#include "beetle/code_unit.hpp"
#include "beetle/types.hpp"

/**
 * Contains algorithms to inspect a sequence of code units.
 *
 * @file validate.hpp
 */

namespace beetle {

namespace utf8 {

// NOLINTNEXTLINE(readability-identifier-naming)
inline namespace cpp20_v1 {

template <std::int8_t Size>
class FixedCharacter {
   public:
    static_assert(Size > 0 && Size < 5, "A valid UTF-8 character is between [1, 4] bytes long.");

    using size_type = std::int8_t;

    using storage_type = std::array<char8_t, Size>;

    using reference = typename storage_type::reference;
    using const_reference = typename storage_type::const_reference;

    using iterator = typename storage_type::iterator;
    using const_iterator = typename storage_type::const_iterator;

    constexpr FixedCharacter() {}

    [[nodiscard]] constexpr reference operator[](size_type index) {
        // TODO: Add assert for bounds checking and character validation
        return this->m_storage[index];
    }

    [[nodiscard]] constexpr const_reference operator[](size_type index) const {
        // TODO: Add assert for bounds checking and character validation
        return this->m_storage[index];
    }

    constexpr char8_t* data() noexcept { return std::data(this->m_storage); }

    constexpr char8_t const* data() const noexcept { return std::data(this->m_storage); }

    constexpr iterator begin() noexcept { return std::begin(this->m_storage); }

    constexpr const_iterator begin() const noexcept { return std::begin(this->m_storage); }

    constexpr iterator end() noexcept { return std::end(this->m_storage); }

    constexpr const_iterator end() const noexcept { return std::end(this->m_storage); }

    [[nodiscard]] constexpr size_type size() const noexcept { return Size; }

   private:
    storage_type m_storage;
};

template <std::int8_t Size>
constexpr decltype(auto) leading_byte(utf8::FixedCharacter<Size>& character) noexcept {
    return character[0];
}

template <std::int8_t Size>
constexpr decltype(auto) leading_byte(utf8::FixedCharacter<Size> const& character) noexcept {
    return character[0];
}

}  // namespace cpp20_v1

}  // namespace utf8

template <std::int8_t Size>
constexpr decltype(auto) front(utf8::FixedCharacter<Size>& character) noexcept {
    return utf8::leading_byte(character);
}

template <std::int8_t Size>
constexpr decltype(auto) front(utf8::FixedCharacter<Size> const& character) noexcept {
    return utf8::leading_byte(character);
}

template <std::int8_t Size>
constexpr decltype(auto) back(utf8::FixedCharacter<Size>& character) noexcept {
    return character[character.size() - 1];
}

template <std::int8_t Size>
constexpr decltype(auto) back(utf8::FixedCharacter<Size> const& character) noexcept {
    return character[character.size() - 1];
}

template <std::int8_t Size>
[[nodiscard]] constexpr bool empty(utf8::FixedCharacter<Size> const& character) noexcept {
    return character.size() == 0;
}

template <std::int8_t Size>
constexpr decltype(auto) at(utf8::FixedCharacter<Size> const& character,
                            typename utf8::FixedCharacter<Size>::size_type index) {
    if (index >= character.size() || index < 0) {
        throw std::out_of_range{"Error: Index is out of bounds for UTF-8 character."};
    }

    return character[index];
}

template <std::int8_t Size>
constexpr decltype(auto) at(utf8::FixedCharacter<Size>& character,
                            typename utf8::FixedCharacter<Size>::size_type index) {
    if (index >= character.size() || index < 0) {
        throw std::out_of_range{"Error: Index is out of bounds for UTF-8 character."};
    }

    return character[index];
}

}  // namespace beetle

#endif
