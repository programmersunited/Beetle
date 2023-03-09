<h1 align="center">Beetle</h1>
<p align="center">A DFA-based UTF-8 library written in modern constexpr ISO C++20</p>

## Summary

Welcome! This is a simple DFA-based UTF-8 library with a modern constexpr-enabled API written in ISO C++20.  The library offers safe and unsafe alternatives for each non-member function. The safe non-member functions provide an **exception** version, and a non-exception version using `std::error_condition`.  The library will offer C++23's `std::expected` when the feature is more widely supported by most compilers. This  library is designed to be standalone, and not have any external dependencies.

Design Goals:
  1. A simple modern API
  2. Speed

## API

The API offers safe non-member functions along with an unsafe alternative.  The unsafe versions are in the `unsafe` namespace, and will assume the UTF-8 string is valid. The code below uses the safe exception throwing version of the API.  There is another safe version of the API that uses `std::error_condition` for error reporting.

### UTF-8 API


```cpp
#include <array>
#include <iostream>
#include <iterator>
#include <string>

// Include all UTF-8 related headers
#include "beetle/utf8.hpp"

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
    static constexpr auto const u8_str = std::array<char8_t, 7>{0xEDU, 0x95U, 0x9CU, 0xF0U, 0x90U, 0x8DU, 0x88U};

    static constexpr auto const u8_str_begin = std::ranges::begin(u8_str);
    static constexpr auto const u8_str_end = std::ranges::end(u8_str);

    static constexpr auto const u8_hangul_syllables = std::u8string_view{u8_str.data(), 3};
    static constexpr auto const u8_hwair = std::u8string_view{std::ranges::next(u8_str_begin, 3), u8_str_end};

    namespace utf8 = beetle::utf8;

    // ============================== VALIDATING ============================== //

    static_assert(utf8::is_valid(u8_str));
    static_assert(!utf8::is_valid(u8_str_begin, std::ranges::prev(u8_str_end)));

    static_assert(utf8::find_invalid(u8_str) == u8_str_end);

    // ============================== INSPECTING ============================== //

    static_assert(utf8::str_len(u8_str) == 2);
    static_assert(utf8::str_len(u8_hangul_syllables) == 1);
    static_assert(utf8::str_len(u8_hwair) == 1);

    // ============================== ITERATING ============================== //

    static constexpr auto const next_it = utf8::next(u8_str_begin, u8_str_end);
    static_assert(*next_it == 0xF0U);

    static constexpr auto const prev_it = utf8::prev(next_it, u8_str_end);
    static_assert(*prev_it == *u8_str_begin);

    // ============================== ENCODING ============================== //

    static constexpr auto const hangul_syllables = beetle::Unicode{0xD55CU};

    auto encoded_hangul_syllables = std::array<char8_t, 3>{0};
    utf8::encode(hangul_syllables, std::ranges::begin(encoded_hangul_syllables));

    // ============================== DECODING ============================== //

    static constexpr auto const decoded_hangul_syllables = utf8::decode(u8_hangul_syllables);

    static_assert(hangul_syllables == decoded_hangul_syllables);
    
    return EXIT_SUCCESS;
}
```


## TODO

Below is the remaining items in order to reach version 1.0.0.
* UTF-8
    * Add more tests
	* Create Wiki
	* Add UTF-8 string class (still deciding)

## How To Build

### Overview

This project has been designed with modern CMake to make it as easy as possible to integrate the library into your own project. To see the possible build options run `cmake --list-presets` in the project's root directory.

### Prerequisites

If you want to build the project in its entirely and use all of its features you will need the following:
- [CMake](https://cmake.org/) (at least version 3.22) - Cross-platform build system
- [GoogleTest](https://github.com/google/googletest) - For unit and integration testing
- [Doxygen](https://www.doxygen.nl/index.html) - To build the documentation
- [A compiler that supports C++20](https://en.cppreference.com/w/cpp/20) (Clang 15 is recommended)

### Building The Project

#### Overview

This project was developed and tested with Clang 15.  More compiler testing is scheduled and will be updated here.

```bash
# To view all of the build modes
cmake --list-presets

# Build the project in debug mode
cmake --preset debug

# Build the project in release mode
cmake --preset release
```

#### Testing

Testing is done using [GoogleTest](https://github.com/google/googletest).  There are unit tests and integration tests for the project.  The tests can be found in the `tests` folder in the root directory.

#### Documentation

The documentation is inside the `docs` folder in the main directory.  The documentation is generated using [Doxygen](https://www.doxygen.nl/index.html).

#### Examples

Examples are provided to show how to use the library located in the `examples` folder in the root directory. 

## Contributing

Beetle is not accepting contributors at this time.  The project needs to have a solid foundation before accepting contributors.  

## License

This project currently uses the MIT license.  Please look at the [LICENSE](LICENSE) for more details. Each header file in the project contains said license at the top of the file.

## Project Status

The project currently is in its early stages.  Changes make take a while.  Please be patient :). 
