#include <array>
#include <cstdlib>
#include <iomanip>
#include <iostream>

// #include "beetle/code_unit.hpp"
#include "beetle/utf8/algorithm.hpp"
#include "beetle/utf8/iterator.hpp"
#include "beetle/version.hpp"
#include "utf8/internal/dfa.hpp"

void usage() {
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
    // TODO(programmersunited): Implement after library is done.
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
    // TODO(programmersunited): Uncomment and implement after library is done.
    // parse_command_line(argc, argv);

    // Dollar sign
    // std::array<char8_t, 1> character = {0x24U};

    // Pound sign
    // std::array<char8_t, 2> character = {0xC2U, 0xA3U};

    // Devanagari
    // std::array<char8_t, 3> character = {0xE0U, 0xA4U, 0xB9U};

    // Euro sign
    // std::array<char8_t, 3> character = {0xE2U, 0x82U, 0xACU};

    // Hangul Syllables
    // std::array<char8_t, 3> character = {0xEDU, 0x95U, 0x9CU};

    // Hwair
    // std::array<char8_t, 4> character = {0xF0U, 0x90U, 0x8DU, 0x88U};

    // Overlong encoding for euro sign
    // std::array<char8_t, 4> character = {0xF0U, 0x82U, 0x82U, 0xACU};

    // std::array<char8_t, 4> character = {0xEDU, 0x07U, 0x82U, 0xACU};

    // std::array<char8_t, 1> character = {0x84U};

    // Hwair - missing last byte
    // std::array<char8_t, 3> character = {0xF0U, 0x90U, 0x8DU};

    /*
    auto first = std::ranges::begin(character);
    auto state = beetle::utf8::internal::advance_forward_once(first, std::ranges::end(character));
    */

    /*
    char32_t code_point{};
    auto first = std::ranges::begin(character);
    auto state =
        beetle::utf8::internal::decode_and_advance_forward_once(first, std::ranges::end(character), code_point);
    */

    /*
    // For reverse
    auto first = std::ranges::prev(std::ranges::end(character));
    auto last = std::ranges::prev(std::ranges::begin(character));
    auto state = beetle::utf8::internal::advance_backward_once(first, last);
    */

    /*
    char32_t code_point{};
    auto first = std::ranges::prev(std::ranges::end(character));
    auto last = std::ranges::prev(std::ranges::begin(character));
    auto state = beetle::utf8::internal::decode_and_advance_backward_once(first, last, code_point);

    if (state == beetle::utf8::internal::State::eAccept) {
        std::cout << "Success!\n";
        std::cout << "0x" << std::hex << std::setw(8) << std::setfill('0') << static_cast<std::uint32_t>(code_point)
                  << '\n';
    } else {
        std::cout << "Failed!\n";
    }

    std::cout << "Is overlong encoded? " << std::boolalpha << (state == beetle::utf8::internal::State::eErrOvrlg)
              << '\n';
    std::cout << "Is missing bytes? " << std::boolalpha << (state == beetle::utf8::internal::State::eErrMiss) << '\n';
    */

    constexpr std::array<char8_t, 7> const chars = {0xEDU, 0x95U, 0x9CU, 0xF0U, 0x90U, 0x8DU, 0x88U};

    auto it = std::ranges::begin(chars);

    auto next_it = beetle::utf8::unsafe::next(it);

    std::cout << "0x" << std::hex << static_cast<int>(*next_it) << '\n';

    return EXIT_SUCCESS;
}
