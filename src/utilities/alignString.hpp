/// @file   alignString.hpp
/// @author Enrico Fraccaroli
/// @date   Feb 13 2017
/// @copyright
/// Copyright (c) 2017 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
/// Permission to use, copy, modify, and distribute this software for any
/// purpose with or without fee is hereby granted, provided that the above
/// copyright notice and this permission notice appear in all copies.
///
/// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
/// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
/// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
/// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
/// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
/// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
/// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#pragma once

#include <string>
#include <sstream>
#include <iomanip>

/// Kinds of string alignment.
using StringAlign = enum class StringAlign_t
{
    Left,
    Center,
    Right,
};

/// @brief Align the given string.
/// @param source    The source string.
/// @param alignment The kind of alignment.
/// @param width     The total width of the string.
/// @return The aligned string.
template<typename ValueType>
std::string AlignString(
    const ValueType & source,
    const StringAlign & alignment,
    const size_t & width)
{
    unsigned int padding;
    // Create a string stream.
    std::ostringstream oss;
    // Align the string.
    if (alignment == StringAlign::Left)
    {
        oss << ' ';
        // Set the width.
        oss << std::setw(static_cast<int>(width) - 1);
        // Set the alignment.
        oss << std::left;
        // Set the string.
        oss << source;
    }
    else if (alignment == StringAlign::Center)
    {
        // Transform the source into string.
        std::ostringstream sourceStream;
        sourceStream << source;
        padding = static_cast<unsigned int>(width - sourceStream.str().size());
        for (unsigned int i = 0; i < (padding / 2); i++)
        {
            oss << ' ';
        }
        oss << source;
        for (unsigned int i = 0; i < (padding / 2); i++)
        {
            oss << ' ';
        }
        // if odd #, add 1 space
        if (padding > 0 && padding % 2 != 0)
        {
            oss << ' ';
        }
    }
    else if (alignment == StringAlign::Right)
    {
        // Set the width.
        oss << std::setw(static_cast<int>(width) - 1);
        // Set the alignment.
        oss << std::right;
        // Set the string.
        oss << source;
        oss << ' ';
    }
    return oss.str();
}
