/// @file   heightMap.hpp
/// @author Enrico Fraccaroli
/// @date   Jan 06 2017
/// @copyright
/// Copyright (c) 2017 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
/// Permission is hereby granted, free of charge, to any person obtaining a
/// copy of this software and associated documentation files (the "Software"),
/// to deal in the Software without restriction, including without limitation
/// the rights to use, copy, modify, merge, publish, distribute, sublicense,
/// and/or sell copies of the Software, and to permit persons to whom the
/// Software is furnished to do so, subject to the following conditions:
///     The above copyright notice and this permission notice shall be included
///     in all copies or substantial portions of the Software.
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
/// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
/// DEALINGS IN THE SOFTWARE.

#pragma once

#include <string>

/// The different types of heights.
class HeightMap
{
public:
    /// List of possible heights.
    enum Enum
    {
        Void,
        DeepWater,
        ShallowWater,
        Coast,
        Plain,
        Hill,
        Mountain,
        HighMountain
    };

    /// @brief Constructor from number.
    HeightMap(const unsigned int & _height);

    /// @brief Constructor from enum.
    HeightMap(const Enum & _height);

    /// @brief Check is the given number is a valid height.
    static bool isValid(const unsigned int & _height);

    /// @brief Returns the height as string.
    std::string toString() const;

    /// @brief Returns the height as number.
    unsigned int toUInt() const;

    /// @brief Returns the height as a symbol.
    std::string toSymbol(bool color = true) const;

    /// @brief Eheight operator w.r.t. a height enum.
    bool operator==(const HeightMap::Enum & rhs) const;

    /// @brief Ineheight operator w.r.t. a height enum.
    bool operator!=(const HeightMap::Enum & rhs) const;

private:
    /// Internal height value.
    Enum height;
};
