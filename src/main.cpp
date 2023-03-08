#include <array>
#include <iomanip>
#include <iostream>
#include <string>

#include "beetle/beetle.hpp"

void usage() {
    // TODO: Finish usage after library is done.
    std::cout << "Beetle Version " << beetle::version_string << '\n';
    std::cout << "A simple utility to encode, decode and verify UTF-8.\n";
    std::cout << "Usage: beetle [OPTION]... [FILE]...\n";
}

void parse_command_line(int argc, [[maybe_unused]] char** argv) {
    // No arguments passed
    if (argc <= 1) {
        usage();
        return;
    }

    // Parse arguments here...
    // TODO: Implement after library is done.
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
    // TODO: Implement after library is done.
    // parse_command_line(argc, argv);

    static constexpr auto const u8_str = std::array<char8_t, 7>{0xEDU, 0x95U, 0x9CU, 0xF0U, 0x90U, 0x8DU, 0x88U};

    static constexpr auto const u8_str_begin = std::ranges::begin(u8_str);
    static constexpr auto const u8_str_end = std::ranges::end(u8_str);

    static constexpr auto const u8_hangul_syllables = std::u8string_view{u8_str.data(), 3};
    static constexpr auto const u8_hwair = std::u8string_view{std::ranges::next(u8_str_begin, 3), u8_str_end};

    namespace utf8 = beetle::utf8;

    // ================================================= VALIDATING ================================================= //

    static_assert(utf8::is_valid(u8_str));
    static_assert(!utf8::is_valid(u8_str_begin, std::ranges::prev(u8_str_end)));

    static_assert(utf8::find_invalid(u8_str) == u8_str_end);

    // ================================================= INSPECTING ================================================= //

    static_assert(utf8::str_len(u8_str) == 2);
    static_assert(utf8::str_len(u8_hangul_syllables) == 1);
    static_assert(utf8::str_len(u8_hwair) == 1);

    // ================================================= ITERATING ================================================== //

    static constexpr auto const next_it = utf8::next(u8_str_begin, u8_str_end);
    static_assert(*next_it == 0xF0U);

    static constexpr auto const prev_it = utf8::prev(next_it, u8_str_end);
    static_assert(*prev_it == *u8_str_begin);

    // ================================================== ENCODING ================================================== //

    // static constexpr auto const hangul_syllables = beetle::Unicode{0xD55CU};

    /*
    constexpr auto encoded_hangul_syllables = std::array<char8_t, 3>{0};
    utf8::encode(hangul_syllables, std::ranges::begin(encoded_hangul_syllables));
    */

    // ================================================== DECODING ================================================== //

    // constexpr auto const hangul_syllables = beetle::Unicode{0xD55CU};

    // Advance over the first character and stop at the next byte of the next character
    // static_assert(*beetle::utf8::next(std::ranges::begin(u8_chars)) == 0xF0U);
    // static_assert(*beetle::utf8::next(std::ranges::begin(u8_chars)) == 0xF0U);

    return EXIT_SUCCESS;
}
