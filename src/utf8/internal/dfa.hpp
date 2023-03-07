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

#ifndef BEETLE_INTERNAL_DFA_HPP
#define BEETLE_INTERNAL_DFA_HPP

#include <array>
#include <cstdint>

#include "beetle/utf8/error.hpp"
#include "core/internal/assert.hpp"
#include "utf8/internal/code_unit.hpp"

/**
 * The internal DFA implementation for validating and decoding UTF-8 forwards and backwards.
 *
 * @file dfa.hpp
 */

namespace beetle::utf8::internal {

// NOLINTNEXTLINE(readability-identifier-naming)
inline namespace cpp20_v1 {

class DFA {
   public:
    /**
     * States in the DFAs.
     *
     * @note The states are designed for the forward DFA but is reused for the backward DFA.
     */
    enum class State : std::uint8_t {
        eS1 = 0,
        eS2,
        eS3,
        eS4,
        eS5,
        eS6,
        eS7,

        /**
         * Same as start.
         *
         * @note Not included in the transition table as a separate row.
         */
        eAccept,

        /**
         * Expecting leading byte.
         */
        eErrLead,

        /**
         * Unexpected overlong byte.
         */
        eErrOvrlg,

        /**
         * Expecting continuation byte.
         */
        eErrCont,

        /**
         * Expecting more bytes (ran out of bytes or unexpected leading byte).
         *
         * @note This is not a possible state to end on in the transition table. This is here for better error
         * reporting.
         */
        eErrMiss
    };

    template <std::input_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
        requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
    [[nodiscard]] static constexpr State advance_forward_once(Iterator& first, Sentinel last) noexcept {
        BEETLE_ASSERT(first != last);

        auto ending_state = State::eAccept;

        if (is_ascii(*first)) {
            ++first;
        } else {
            ending_state = advance_mb_forward_once(first, last);
        }

        return ending_state;
    }

    template <std::bidirectional_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
        requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
    [[nodiscard]] static constexpr State advance_backward_once(Iterator& first, Sentinel last) noexcept {
        BEETLE_ASSERT(first != last);

        auto ending_state = State::eAccept;

        if (is_ascii(*first)) {
            --first;
        } else {
            ending_state = advance_mb_backward_once(first, last);
        }

        return ending_state;
    }

    template <std::input_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
        requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
    [[nodiscard]] static constexpr State decode_and_advance_forward_once(Iterator& first, Sentinel last,
                                                                         char32_t& out_code_unit_32) noexcept {
        BEETLE_ASSERT(first != last);

        auto ending_state = State::eAccept;

        auto const leading_byte = *first;

        if (is_ascii(leading_byte)) {
            out_code_unit_32 = leading_byte;
            ++first;
        } else {
            ending_state = decode_and_advance_mb_forward_once(first, last, out_code_unit_32);
        }

        return ending_state;
    }

    template <std::bidirectional_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
        requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
    [[nodiscard]] static constexpr State decode_and_advance_backward_once(Iterator& first, Sentinel last,
                                                                          char32_t& out_code_unit_32) noexcept {
        BEETLE_ASSERT(first != last);

        auto ending_state = State::eAccept;

        auto const leading_byte = *first;

        if (is_ascii(leading_byte)) {
            out_code_unit_32 = leading_byte;
            --first;
        } else {
            ending_state = decode_and_advance_mb_backward_once(first, last, out_code_unit_32);
        }

        return ending_state;
    }

    template <std::input_iterator Iterator, std::sentinel_for<Iterator> Sentinel, std::weakly_incrementable Output>
        requires std::indirectly_copyable<Iterator, Output> &&
                 std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
    [[nodiscard]] static constexpr State copy_and_advance_forward_once(Iterator& first, Sentinel last,
                                                                       Output result) noexcept {
        BEETLE_ASSERT(first != last);

        auto ending_state = State::eAccept;

        auto const leading_byte = *first;

        if (is_ascii(leading_byte)) {
            ++first;

            *result = leading_byte;
            ++result;
        } else {
            ending_state = copy_and_advance_mb_forward_once(first, last, result);
        }

        return ending_state;
    }

    template <std::input_iterator Iterator, std::sentinel_for<Iterator> Sentinel, std::weakly_incrementable Output>
        requires std::indirectly_copyable<Iterator, Output> &&
                 std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
    [[nodiscard]] static constexpr State copy_and_advance_backward_once(Iterator& first, Sentinel last,
                                                                        Output result) noexcept {
        BEETLE_ASSERT(first != last);

        auto ending_state = State::eAccept;

        auto const leading_byte = *first;

        if (is_ascii(leading_byte)) {
            ++first;

            *result = leading_byte;
            ++result;
        } else {
            ending_state = copy_and_advance_mb_backward_once(first, last, result);
        }

        return ending_state;
    }

    [[nodiscard]] static constexpr utf8::Error ending_state_to_error(State ending_state) noexcept {
        BEETLE_ASSERT(ending_state > State::eAccept && ending_state <= State::eErrMiss);

        // utf8::Error::eLeadingByte starts at 1
        constexpr auto const enum_offset = static_cast<int>(State::eErrLead) - 1;

        return utf8::Error{static_cast<int>(ending_state) - enum_offset};
    }

    [[nodiscard]] static std::error_condition make_error_condition(State ending_state) noexcept {
        return beetle::make_error_condition(ending_state_to_error(ending_state));
    }

   private:
    /**
     * The character classes of the possible UTF-8 code units.
     */
    enum class CharClass : std::uint8_t {
        eASC = 0,  // 00..7f
        eC1,       // 80..8f
        eC2,       // 90..9f
        eC3,       // a0..bf
        eC4,       // c2..df
        eC5,       // e0..e0
        eC6,       // e1..ec, ee..ef
        eC7,       // ed..ed
        eC8,       // f0..f0
        eC9,       // f1..f3
        eC10,      // f4..f4
        eIgl       // f5..ff
    };

    struct LeadingByteInfo {
        /**
         * The state after the start state for the leading byte.
         */
        State next_state;

        /**
         * Pre-computed decoded value of the leading byte.
         */
        std::uint8_t data;
    };

    // ================================================== ADVANCING ================================================= //

    template <std::input_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
        requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
    [[nodiscard]] static constexpr State advance_mb_forward_once(Iterator& first, Sentinel last) noexcept {
        BEETLE_ASSERT(first != last);
        BEETLE_ASSERT(!utf8::internal::is_ascii(*first));

        auto state = get_leading_byte_info(*first).next_state;
        ++first;

        while (first != last && can_advance(state)) {
            auto const code_unit = *first;

            state = advance_state_forward(state, code_unit);
            ++first;
        }

        return get_ending_state(state);
    }

    template <std::bidirectional_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
        requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
    [[nodiscard]] static constexpr State advance_mb_backward_once(Iterator& first, Sentinel last) noexcept {
        BEETLE_ASSERT(first != last);
        BEETLE_ASSERT(!utf8::internal::is_ascii(*first));

        auto const is_continuation_byte = (*first & 0xC0) == 0x80;

        // TODO: Figure out if unlikely should be added here
        if (!is_continuation_byte) {
            return State::eErrCont;
        }

        auto state = State::eS1;
        --first;

        while (first != last && can_advance(state)) {
            auto const code_unit = *first;

            state = advance_state_backward(state, code_unit);
            --first;
        }

        return get_ending_state(state);
    }

    // =========================================== DECODING AND ADVANCING =========================================== //

    template <std::bidirectional_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
        requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
    [[nodiscard]] static constexpr State decode_and_advance_mb_backward_once(Iterator& first, Sentinel last,
                                                                             char32_t& out_code_unit_32) noexcept {
        BEETLE_ASSERT(first != last);
        BEETLE_ASSERT(!utf8::internal::is_ascii(*first));

        // TODO: Clean-up code -- currently just trying to get it to work

        auto const is_continuation_byte = (*first & 0xC0) == 0x80;

        // TODO: Figure out if unlikely should be added here
        if (!is_continuation_byte) {
            return State::eErrCont;
        }

        out_code_unit_32 = *first & 0x3FU;

        auto state = State::eS1;
        --first;

        auto shift_amount = 6;

        while (first != last && !is_mb_leading_byte(*first) && can_advance(state)) {
            auto const continuation_byte = *first;

            auto const decoded_byte = continuation_byte & 0x3FU;  // 10xx xxxx

            out_code_unit_32 = out_code_unit_32 | (static_cast<char32_t>(decoded_byte) << shift_amount);

            state = advance_state_backward(state, continuation_byte);
            --first;

            shift_amount += 6;
        }

        // Bad state or valid leading byte
        if (can_advance(state) && is_mb_leading_byte(*first)) {
            auto const leading_byte = *first;
            auto const leading_byte_data = get_leading_byte_info(leading_byte).data;

            out_code_unit_32 = out_code_unit_32 | (static_cast<char32_t>(leading_byte_data) << shift_amount);

            state = advance_state_backward(state, leading_byte);
            --first;
        }

        return get_ending_state(state);
    }

    template <std::input_iterator Iterator, std::sentinel_for<Iterator> Sentinel>
        requires std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
    [[nodiscard]] static constexpr State decode_and_advance_mb_forward_once(Iterator& first, Sentinel last,
                                                                            char32_t& out_code_unit_32) noexcept {
        BEETLE_ASSERT(first != last);
        BEETLE_ASSERT(!utf8::internal::is_ascii(*first));

        auto const leading_byte_info = get_leading_byte_info(*first);

        out_code_unit_32 = leading_byte_info.data;

        auto state = leading_byte_info.next_state;
        ++first;

        while (first != last && can_advance(state)) {
            auto const continuation_byte = *first;

            auto const decoded_byte = continuation_byte & 0x3FU;  // 10xx xxxx
            constexpr auto const decoded_bit_count = 6;

            out_code_unit_32 = (out_code_unit_32 << decoded_bit_count) | decoded_byte;

            state = advance_state_forward(state, continuation_byte);
            ++first;
        }

        return get_ending_state(state);
    }

    // =========================================== COPYING AND ADVANCING ============================================ //

    template <std::input_iterator Iterator, std::sentinel_for<Iterator> Sentinel, std::weakly_incrementable Output>
        requires std::indirectly_copyable<Iterator, Output> &&
                 std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
    [[nodiscard]] static constexpr State copy_and_advance_mb_forward_once(Iterator& first, Sentinel last,
                                                                          Output result) noexcept {
        BEETLE_ASSERT(first != last);
        BEETLE_ASSERT(!utf8::internal::is_ascii(*first));

        auto state = get_leading_byte_info(*first).next_state;
        ++first;

        while (first != last && can_advance(state)) {
            auto const code_unit = *first;

            *result = code_unit;
            ++result;

            state = advance_state_forward(state, code_unit);
            ++first;
        }

        return get_ending_state(state);
    }

    template <std::input_iterator Iterator, std::sentinel_for<Iterator> Sentinel, std::weakly_incrementable Output>
        requires std::indirectly_copyable<Iterator, Output> &&
                 std::convertible_to<typename std::iter_value_t<Iterator>, char8_t>
    [[nodiscard]] static constexpr State copy_and_advance_mb_backward_once(Iterator& first, Sentinel last,
                                                                           Output result) noexcept {
        BEETLE_ASSERT(first != last);
        BEETLE_ASSERT(!utf8::internal::is_ascii(*first));

        auto const is_continuation_byte = (*first & 0xC0) == 0x80;

        // TODO: Figure out if unlikely should be added here
        if (!is_continuation_byte) {
            return State::eErrCont;
        }

        auto state = State::eS1;
        --first;

        while (first != last && can_advance(state)) {
            auto const code_unit = *first;

            *result = code_unit;
            ++result;

            state = advance_state_backward(state, code_unit);
            --first;
        }

        return get_ending_state(state);
    }

    [[nodiscard]] static constexpr LeadingByteInfo get_leading_byte_info(char8_t leading_byte) noexcept {
        BEETLE_ASSERT(!utf8::internal::is_ascii(leading_byte));

        constexpr auto const first_non_ascii = 0x80U;
        auto const leading_byte_offset = leading_byte - first_non_ascii;

        return TableData::leading_byte_states[leading_byte_offset];
    }

    [[nodiscard]] static constexpr bool can_advance(State current_state) noexcept {
        return current_state < State::eAccept;
    }

    [[nodiscard]] static constexpr State advance_state_forward(State current_state, char8_t code_unit) noexcept {
        auto const char_class = TableData::char_class[code_unit];

        constexpr auto const row_length = 12;

        auto const row_offset = static_cast<std::uint8_t>(static_cast<std::uint8_t>(current_state) * row_length);
        auto const column_offset = static_cast<std::uint8_t>(char_class);

        return TableData::forward_transitions[row_offset + column_offset];
    }

    [[nodiscard]] static constexpr State advance_state_backward(State current_state, char8_t code_unit) noexcept {
        auto const char_class = TableData::char_class[code_unit];

        constexpr auto const row_length = 12;

        auto const row_offset = static_cast<std::uint8_t>(static_cast<std::uint8_t>(current_state) * row_length);
        auto const column_offset = static_cast<std::uint8_t>(char_class);

        return TableData::backward_transitions[row_offset + column_offset];
    }

    [[nodiscard]] static constexpr State get_ending_state(State current_state) noexcept {
        return TableData::ending_states[static_cast<std::uint8_t>(current_state)];
    }

    struct alignas(4096) TableData {
        /**
         * A table containing the character class of each UTF-8 code unit to reduce the sizes of the transition tables.
         */
        static constexpr std::array<CharClass, 256> const char_class = {
            // ============================================= 0x00..0x0F ============================================= //

            CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC,
            CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC,
            CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC,

            // ============================================= 0x10..0x1F ============================================= //

            CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC,
            CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC,
            CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC,

            // ============================================= 0x20..0x2F ============================================= //

            CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC,
            CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC,
            CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC,

            // ============================================= 0x30..0x3F ============================================= //

            CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC,
            CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC,
            CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC,

            // ============================================= 0x40..0x4F ============================================= //

            CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC,
            CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC,
            CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC,

            // ============================================= 0x50..0x5F ============================================= //

            CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC,
            CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC,
            CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC,

            // ============================================= 0x60..0x6F ============================================= //

            CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC,
            CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC,
            CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC,

            // ============================================= 0x70..0x7F ============================================= //

            CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC,
            CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC,
            CharClass::eASC, CharClass::eASC, CharClass::eASC, CharClass::eASC,

            // ============================================= 0x80..0x8F ============================================= //

            CharClass::eC1, CharClass::eC1, CharClass::eC1, CharClass::eC1, CharClass::eC1, CharClass::eC1,
            CharClass::eC1, CharClass::eC1, CharClass::eC1, CharClass::eC1, CharClass::eC1, CharClass::eC1,
            CharClass::eC1, CharClass::eC1, CharClass::eC1, CharClass::eC1,

            // ============================================= 0x90..0x9F ============================================= //

            CharClass::eC2, CharClass::eC2, CharClass::eC2, CharClass::eC2, CharClass::eC2, CharClass::eC2,
            CharClass::eC2, CharClass::eC2, CharClass::eC2, CharClass::eC2, CharClass::eC2, CharClass::eC2,
            CharClass::eC2, CharClass::eC2, CharClass::eC2, CharClass::eC2,

            // ============================================= 0xA0..0xAF ============================================= //

            CharClass::eC3, CharClass::eC3, CharClass::eC3, CharClass::eC3, CharClass::eC3, CharClass::eC3,
            CharClass::eC3, CharClass::eC3, CharClass::eC3, CharClass::eC3, CharClass::eC3, CharClass::eC3,
            CharClass::eC3, CharClass::eC3, CharClass::eC3, CharClass::eC3,

            // ============================================= 0xB0..0xBF ============================================= //

            CharClass::eC3, CharClass::eC3, CharClass::eC3, CharClass::eC3, CharClass::eC3, CharClass::eC3,
            CharClass::eC3, CharClass::eC3, CharClass::eC3, CharClass::eC3, CharClass::eC3, CharClass::eC3,
            CharClass::eC3, CharClass::eC3, CharClass::eC3, CharClass::eC3,

            // ============================================= 0xC0..0xCF ============================================= //

            CharClass::eIgl, CharClass::eIgl, CharClass::eC4, CharClass::eC4, CharClass::eC4, CharClass::eC4,
            CharClass::eC4, CharClass::eC4, CharClass::eC4, CharClass::eC4, CharClass::eC4, CharClass::eC4,
            CharClass::eC4, CharClass::eC4, CharClass::eC4, CharClass::eC4,

            // ============================================= 0xD0..0xDF ============================================= //

            CharClass::eC4, CharClass::eC4, CharClass::eC4, CharClass::eC4, CharClass::eC4, CharClass::eC4,
            CharClass::eC4, CharClass::eC4, CharClass::eC4, CharClass::eC4, CharClass::eC4, CharClass::eC4,
            CharClass::eC4, CharClass::eC4, CharClass::eC4, CharClass::eC4,

            // ============================================= 0xE0..0xEF ============================================= //

            CharClass::eC5, CharClass::eC6, CharClass::eC6, CharClass::eC6, CharClass::eC6, CharClass::eC6,
            CharClass::eC6, CharClass::eC6, CharClass::eC6, CharClass::eC6, CharClass::eC6, CharClass::eC6,
            CharClass::eC6, CharClass::eC7, CharClass::eC6, CharClass::eC6,

            // ============================================= 0xF0..0xFF ============================================= //

            CharClass::eC8, CharClass::eC9, CharClass::eC9, CharClass::eC9, CharClass::eC10, CharClass::eIgl,
            CharClass::eIgl, CharClass::eIgl, CharClass::eIgl, CharClass::eIgl, CharClass::eIgl, CharClass::eIgl,
            CharClass::eIgl, CharClass::eIgl, CharClass::eIgl, CharClass::eIgl};

        /**
         * All possible leading bytes except for ASCII (00..7F).
         */
        static constexpr std::array<LeadingByteInfo, 256 - 0x80> const leading_byte_states = {
            // ======================================= 0x80..0x8F ======================================= //

            LeadingByteInfo{State::eErrLead, 0x80},  // 0x80
            LeadingByteInfo{State::eErrLead, 0x81},  // 0x81
            LeadingByteInfo{State::eErrLead, 0x82},  // 0x82
            LeadingByteInfo{State::eErrLead, 0x83},  // 0x83
            LeadingByteInfo{State::eErrLead, 0x84},  // 0x84
            LeadingByteInfo{State::eErrLead, 0x85},  // 0x85
            LeadingByteInfo{State::eErrLead, 0x86},  // 0x86
            LeadingByteInfo{State::eErrLead, 0x87},  // 0x87
            LeadingByteInfo{State::eErrLead, 0x88},  // 0x88
            LeadingByteInfo{State::eErrLead, 0x80},  // 0x89
            LeadingByteInfo{State::eErrLead, 0x8A},  // 0x8A
            LeadingByteInfo{State::eErrLead, 0x8B},  // 0x8B
            LeadingByteInfo{State::eErrLead, 0x8C},  // 0x8C
            LeadingByteInfo{State::eErrLead, 0x8D},  // 0x8D
            LeadingByteInfo{State::eErrLead, 0x8E},  // 0x8E
            LeadingByteInfo{State::eErrLead, 0x8F},  // 0x8F

            // ======================================= 0x90..0x9F ======================================= //

            LeadingByteInfo{State::eErrLead, 0x90},  // 0x90
            LeadingByteInfo{State::eErrLead, 0x91},  // 0x91
            LeadingByteInfo{State::eErrLead, 0x92},  // 0x92
            LeadingByteInfo{State::eErrLead, 0x93},  // 0x93
            LeadingByteInfo{State::eErrLead, 0x94},  // 0x94
            LeadingByteInfo{State::eErrLead, 0x95},  // 0x95
            LeadingByteInfo{State::eErrLead, 0x96},  // 0x96
            LeadingByteInfo{State::eErrLead, 0x97},  // 0x97
            LeadingByteInfo{State::eErrLead, 0x98},  // 0x98
            LeadingByteInfo{State::eErrLead, 0x99},  // 0x99
            LeadingByteInfo{State::eErrLead, 0x9A},  // 0x9A
            LeadingByteInfo{State::eErrLead, 0x9B},  // 0x9B
            LeadingByteInfo{State::eErrLead, 0x9C},  // 0x9C
            LeadingByteInfo{State::eErrLead, 0x9D},  // 0x9D
            LeadingByteInfo{State::eErrLead, 0x9E},  // 0x9E
            LeadingByteInfo{State::eErrLead, 0x9F},  // 0x9F

            // ======================================= 0xA0..0xAF ======================================= //

            LeadingByteInfo{State::eErrLead, 0xA0},  // 0xA0
            LeadingByteInfo{State::eErrLead, 0xA1},  // 0xA1
            LeadingByteInfo{State::eErrLead, 0xA2},  // 0xA2
            LeadingByteInfo{State::eErrLead, 0xA3},  // 0xA3
            LeadingByteInfo{State::eErrLead, 0xA4},  // 0xA4
            LeadingByteInfo{State::eErrLead, 0xA5},  // 0xA5
            LeadingByteInfo{State::eErrLead, 0xA6},  // 0xA6
            LeadingByteInfo{State::eErrLead, 0xA7},  // 0xA7
            LeadingByteInfo{State::eErrLead, 0xA8},  // 0xA8
            LeadingByteInfo{State::eErrLead, 0xA9},  // 0xA9
            LeadingByteInfo{State::eErrLead, 0xAA},  // 0xAA
            LeadingByteInfo{State::eErrLead, 0xAB},  // 0xAB
            LeadingByteInfo{State::eErrLead, 0xAC},  // 0xAC
            LeadingByteInfo{State::eErrLead, 0xAD},  // 0xAD
            LeadingByteInfo{State::eErrLead, 0xAE},  // 0xAE
            LeadingByteInfo{State::eErrLead, 0xAF},  // 0xAF

            // ======================================= 0xB0..0xBF ======================================= //

            LeadingByteInfo{State::eErrLead, 0xB0},  // 0xB0
            LeadingByteInfo{State::eErrLead, 0xB1},  // 0xB1
            LeadingByteInfo{State::eErrLead, 0xB2},  // 0xB2
            LeadingByteInfo{State::eErrLead, 0xB3},  // 0xB3
            LeadingByteInfo{State::eErrLead, 0xB4},  // 0xB4
            LeadingByteInfo{State::eErrLead, 0xB5},  // 0xB5
            LeadingByteInfo{State::eErrLead, 0xB6},  // 0xB6
            LeadingByteInfo{State::eErrLead, 0xB7},  // 0xB7
            LeadingByteInfo{State::eErrLead, 0xB8},  // 0xB8
            LeadingByteInfo{State::eErrLead, 0xB9},  // 0xB9
            LeadingByteInfo{State::eErrLead, 0xBA},  // 0xBA
            LeadingByteInfo{State::eErrLead, 0xBB},  // 0xBB
            LeadingByteInfo{State::eErrLead, 0xBC},  // 0xBC
            LeadingByteInfo{State::eErrLead, 0xBD},  // 0xBD
            LeadingByteInfo{State::eErrLead, 0xBE},  // 0xBE
            LeadingByteInfo{State::eErrLead, 0xBF},  // 0xBF

            // ======================================= 0xC0..0xCF ======================================= //

            LeadingByteInfo{State::eErrOvrlg, 0xC0},  // 0xC0
            LeadingByteInfo{State::eErrOvrlg, 0xC1},  // 0xC1
            LeadingByteInfo{State::eS1, 0x02},        // 0xC2
            LeadingByteInfo{State::eS1, 0x03},        // 0xC3
            LeadingByteInfo{State::eS1, 0x04},        // 0xC4
            LeadingByteInfo{State::eS1, 0x05},        // 0xC5
            LeadingByteInfo{State::eS1, 0x06},        // 0xC6
            LeadingByteInfo{State::eS1, 0x07},        // 0xC7
            LeadingByteInfo{State::eS1, 0x08},        // 0xC8
            LeadingByteInfo{State::eS1, 0x09},        // 0xC9
            LeadingByteInfo{State::eS1, 0x0A},        // 0xCA
            LeadingByteInfo{State::eS1, 0x0B},        // 0xCB
            LeadingByteInfo{State::eS1, 0x0C},        // 0xCC
            LeadingByteInfo{State::eS1, 0x0D},        // 0xCD
            LeadingByteInfo{State::eS1, 0x0E},        // 0xCE
            LeadingByteInfo{State::eS1, 0x0F},        // 0xCF

            // ======================================= 0xD0..0xDF ======================================= //

            LeadingByteInfo{State::eS1, 0x10},  // 0xD0
            LeadingByteInfo{State::eS1, 0x11},  // 0xD1
            LeadingByteInfo{State::eS1, 0x12},  // 0xD2
            LeadingByteInfo{State::eS1, 0x13},  // 0xD3
            LeadingByteInfo{State::eS1, 0x14},  // 0xD4
            LeadingByteInfo{State::eS1, 0x15},  // 0xD5
            LeadingByteInfo{State::eS1, 0x16},  // 0xD6
            LeadingByteInfo{State::eS1, 0x17},  // 0xD7
            LeadingByteInfo{State::eS1, 0x18},  // 0xD8
            LeadingByteInfo{State::eS1, 0x19},  // 0xD9
            LeadingByteInfo{State::eS1, 0x1A},  // 0xDA
            LeadingByteInfo{State::eS1, 0x1B},  // 0xDB
            LeadingByteInfo{State::eS1, 0x1C},  // 0xDC
            LeadingByteInfo{State::eS1, 0x1D},  // 0xDD
            LeadingByteInfo{State::eS1, 0x1E},  // 0xDE
            LeadingByteInfo{State::eS1, 0x1F},  // 0xDF

            // ======================================= 0xE0..0xEF ======================================= //

            LeadingByteInfo{State::eS2, 0x00},  // 0xE0
            LeadingByteInfo{State::eS4, 0x01},  // 0xE1
            LeadingByteInfo{State::eS4, 0x02},  // 0xE2
            LeadingByteInfo{State::eS4, 0x03},  // 0xE3
            LeadingByteInfo{State::eS4, 0x04},  // 0xE4
            LeadingByteInfo{State::eS4, 0x05},  // 0xE5
            LeadingByteInfo{State::eS4, 0x06},  // 0xE6
            LeadingByteInfo{State::eS4, 0x07},  // 0xE7
            LeadingByteInfo{State::eS4, 0x08},  // 0xE8
            LeadingByteInfo{State::eS4, 0x09},  // 0xE9
            LeadingByteInfo{State::eS4, 0x0A},  // 0xEA
            LeadingByteInfo{State::eS4, 0x0B},  // 0xEB
            LeadingByteInfo{State::eS4, 0x0C},  // 0xEC
            LeadingByteInfo{State::eS3, 0x0D},  // 0xED
            LeadingByteInfo{State::eS4, 0x0E},  // 0xEE
            LeadingByteInfo{State::eS4, 0x0F},  // 0xEF

            // ======================================= 0xF0..0xFF ======================================= //

            LeadingByteInfo{State::eS5, 0x00},       // 0xF0
            LeadingByteInfo{State::eS6, 0x01},       // 0xF1
            LeadingByteInfo{State::eS6, 0x02},       // 0xF2
            LeadingByteInfo{State::eS6, 0x03},       // 0xF3
            LeadingByteInfo{State::eS7, 0x04},       // 0xF4
            LeadingByteInfo{State::eErrLead, 0xF5},  // 0xF5
            LeadingByteInfo{State::eErrLead, 0xF6},  // 0xF6
            LeadingByteInfo{State::eErrLead, 0xF7},  // 0xF7
            LeadingByteInfo{State::eErrLead, 0xF8},  // 0xF8
            LeadingByteInfo{State::eErrLead, 0xF9},  // 0xF9
            LeadingByteInfo{State::eErrLead, 0xFA},  // 0xFA
            LeadingByteInfo{State::eErrLead, 0xFB},  // 0xFB
            LeadingByteInfo{State::eErrLead, 0xFC},  // 0xFC
            LeadingByteInfo{State::eErrLead, 0xFD},  // 0xFD
            LeadingByteInfo{State::eErrLead, 0xFE},  // 0xFE
            LeadingByteInfo{State::eErrLead, 0xFF},  // 0xFF
        };

        /**
         * The transitions for the forward UTF-8 DFA.
         *
         * @ Note Each section is a row and each character class in said row is a column.
         */
        static constexpr std::array<State, 84> const forward_transitions = {

            // ============================================= eS1 ============================================= //

            //     eASC            eC1             eC2             eC3             eC4              eC5
            State::eErrCont, State::eAccept, State::eAccept, State::eAccept, State::eErrCont, State::eErrCont,

            //     eC6             eC7             eC8             eC9             eC10             eIgl
            State::eErrCont, State::eErrCont, State::eErrCont, State::eErrCont, State::eErrCont, State::eErrCont,

            // ============================================= eS2 ============================================= //

            //     eASC            eC1             eC2             eC3             eC4              eC5
            State::eErrCont, State::eErrOvrlg, State::eErrOvrlg, State::eS1, State::eErrCont, State::eErrCont,

            //     eC6             eC7             eC8             eC9             eC10             eIgl
            State::eErrCont, State::eErrCont, State::eErrCont, State::eErrCont, State::eErrCont, State::eErrCont,

            // ============================================= eS3 ============================================= //

            //     eASC            eC1             eC2             eC3             eC4              eC5
            State::eErrCont, State::eS1, State::eS1, State::eErrCont, State::eErrCont, State::eErrCont, State::eErrCont,

            //     eC6             eC7             eC8             eC9             eC10             eIgl
            State::eErrCont, State::eErrCont, State::eErrCont, State::eErrCont, State::eErrCont,

            // ============================================= eS4 ============================================= //

            //     eASC            eC1             eC2             eC3             eC4              eC5
            State::eErrCont, State::eS1, State::eS1, State::eS1, State::eErrCont, State::eErrCont, State::eErrCont,

            //     eC6             eC7             eC8             eC9             eC10             eIgl
            State::eErrCont, State::eErrCont, State::eErrCont, State::eErrCont, State::eErrCont,

            // ============================================= eS5 ============================================= //

            //     eASC            eC1             eC2             eC3             eC4              eC5
            State::eErrCont, State::eErrOvrlg, State::eS4, State::eS4, State::eErrCont, State::eErrCont,

            //     eC6             eC7             eC8             eC9             eC10             eIgl
            State::eErrCont, State::eErrCont, State::eErrCont, State::eErrCont, State::eErrCont, State::eErrCont,

            // ============================================= eS6 ============================================= //

            //     eASC            eC1             eC2             eC3             eC4              eC5
            State::eErrCont, State::eS4, State::eS4, State::eS4, State::eErrCont, State::eErrCont, State::eErrCont,

            //     eC6             eC7             eC8             eC9             eC10             eIgl
            State::eErrCont, State::eErrCont, State::eErrCont, State::eErrCont, State::eErrCont,

            // ============================================= eS7 ============================================= //

            //     eASC            eC1             eC2             eC3             eC4              eC5
            State::eErrCont, State::eS4, State::eErrCont, State::eErrCont, State::eErrCont, State::eErrCont,

            //     eC6             eC7             eC8             eC9             eC10             eIgl
            State::eErrCont, State::eErrCont, State::eErrCont, State::eErrCont, State::eErrCont, State::eErrCont};

        /**
         * The transitions for the backward UTF-8 DFA.
         *
         * @ Note Each section is a row and each character class in said row is a column.
         */
        static constexpr std::array<State, 72> const backward_transitions = {
            // ============================================= eS1 ============================================= //

            //     eASC            eC1             eC2             eC3             eC4              eC5
            State::eErrCont, State::eS2, State::eS2, State::eS3, State::eAccept, State::eErrCont,

            //     eC6             eC7             eC8             eC9             eC10             eIgl
            State::eErrCont, State::eErrCont, State::eErrCont, State::eErrCont, State::eErrCont, State::eErrCont,

            // ============================================= eS2 ============================================= //

            //     eASC            eC1             eC2             eC3             eC4              eC5
            State::eErrCont, State::eS4, State::eS5, State::eS5, State::eErrCont, State::eErrOvrlg,

            //     eC6             eC7             eC8             eC9             eC10             eIgl
            State::eAccept, State::eAccept, State::eErrMiss, State::eErrMiss, State::eErrMiss, State::eErrCont,

            // ============================================= eS3 ============================================= //

            //     eASC            eC1             eC2             eC3             eC4              eC5
            State::eErrCont, State::eS4, State::eS5, State::eS5, State::eErrCont, State::eAccept,

            //     eC6             eC7             eC8             eC9             eC10             eIgl
            State::eAccept, State::eErrCont, State::eErrMiss, State::eErrMiss, State::eErrMiss, State::eErrCont,

            // ============================================= eS4 ============================================= //

            //     eASC            eC1             eC2             eC3             eC4              eC5
            State::eErrLead, State::eErrLead, State::eErrLead, State::eErrLead, State::eErrLead, State::eErrLead,

            //     eC6             eC7             eC8             eC9             eC10             eIgl
            State::eErrLead, State::eErrLead, State::eErrOvrlg, State::eAccept, State::eAccept, State::eErrLead,

            // ============================================= eS5 ============================================= //

            //     eASC            eC1             eC2             eC3             eC4              eC5
            State::eErrLead, State::eErrLead, State::eErrLead, State::eErrLead, State::eErrLead, State::eErrLead,

            //     eC6             eC7             eC8             eC9             eC10             eIgl
            State::eErrLead, State::eErrLead, State::eAccept, State::eAccept, State::eErrLead, State::eErrLead};

        /**
         * Ending states to help produce better errors.
         *
         * @note Equivalent to sending an EOF input to help error-out in-progress states.
         */
        static constexpr std::array<State, 12> const ending_states = {
            //   eS1              eS2                eS3              eS4             eS5             eS6
            State::eErrMiss, State::eErrMiss, State::eErrMiss, State::eErrMiss, State::eErrMiss, State::eErrMiss,

            //     eS7           eAccept          eErrLead         eErrOvrlg         eErrCont        eErrMiss
            State::eErrMiss, State::eAccept, State::eErrLead, State::eErrOvrlg, State::eErrCont, State::eErrMiss};
    };
};

}  // namespace cpp20_v1

}  // namespace beetle::utf8::internal

#endif
