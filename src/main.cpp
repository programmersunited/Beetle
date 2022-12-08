#include <iostream>
#include <iomanip>

#include "beetle/version.hpp"
#include "beetle/assert.hpp"
#include "beetle/code_unit.hpp"

auto usage() -> void {
    std::cout << "Beetle Version " << beetle::version_string << '\n';
    std::cout << "Usage: beetle [OPTION]... [FILE]..." << '\n';
    std::cout << "A simple utility to encode, decode and verify UTF-8." << '\n';
}

auto parse_command_line(int argc, [[maybe_unused]] char** argv) -> void {
    // No arguments passed
    if (argc == 1) {
        usage();
        return;
    }

    // Parse arguments here...
    // TODO(programmersunited): Implement after library is done.
}

auto main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) -> int {
    // TODO(programmersunited): Uncomment and implement after library is done.
    // parse_command_line(argc, argv);
    beetle::assert(true, "This is a test message!");

    constexpr auto is_ascii = beetle::utf8::is_ascii(65);
    std::cout << "Is 'A' ASCII? " << std::boolalpha << is_ascii << '\n';

    return EXIT_SUCCESS;
}
