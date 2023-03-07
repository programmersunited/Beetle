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

#ifndef BEETLE_CORE_INTERNAL_MACRO_HELPERS_HPP
#define BEETLE_CORE_INTERNAL_MACRO_HELPERS_HPP

// NOLINTBEGIN(cppcoreguidelines-macro-usage)

// Make into string literal
#define BEETLE_STR(x) #x

// Concatenate two arguments together
#define BEETLE_CAT(A, B) A##B

// Creates NAME_NUM output
#define BEETLE_SELECT(NAME, NUM) BEETLE_CAT(NAME##_, NUM)

// Binds NAME to the given ARGS
#define BEETLE_BIND(NAME, ARGS) NAME ARGS

// BEETLE_VA_SIZE helper to get __VA_ARGS__ count
#define BEETLE_GET_COUNT(_1, _2, _3, _4, _5, _6, COUNT, ...) COUNT

// Gets the number of arguments from __VA_ARGS__
#define BEETLE_VA_SIZE(...) BEETLE_GET_COUNT(__VA_ARGS__, 6, 5, 4, 3, 2, 1, -1)

#define BEETLE_VA_SELECT(NAME, ...)                  \
    BEETLE_SELECT(NAME, BEETLE_VA_SIZE(__VA_ARGS__)) \
    (__VA_ARGS__)

// NOLINTEND(cppcoreguidelines-macro-usage)

#endif
