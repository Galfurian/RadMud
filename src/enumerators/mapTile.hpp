/// @file   mapTile.hpp
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

/// The different types of map tiles.
class MapTile
{
public:
    /// List of possible map tiles.
    enum Enum
    {
        Void,
        DeepWater,
        ShallowWater,
        Coast,
        Plain,
        Forest,
        River,
        WaterSpring,
        Hill,
        Mountain,
        HighMountain
    };

    /// @brief Constructor from number.
    MapTile(const unsigned int & _mapTile);

    /// @brief Constructor from enum.
    MapTile(const Enum & _mapTile);

    /// @brief Check is the given number is a valid mapTile.
    static bool isValid(const unsigned int & _mapTile);

    /// @brief Returns the mapTile as string.
    std::string toString() const;

    /// @brief Returns the mapTile as number.
    unsigned int toUInt() const;

    /// @brief Returns the mapTile as a symbol.
    std::string toSymbol(bool color = true) const;

    /// @brief Equality operator w.r.t. a mapTile enum.
    bool operator==(const MapTile & rhs) const;

    /// @brief Inequality operator w.r.t. a mapTile enum.
    bool operator!=(const MapTile & rhs) const;

    /// @brief Greater operator w.r.t. a mapTile enum.
    bool operator>(const MapTile & rhs) const;

    /// @brief Greater-Equal operator w.r.t. a mapTile enum.
    bool operator>=(const MapTile & rhs) const;

    /// @brief Lesser operator w.r.t. a mapTile enum.
    bool operator<(const MapTile & rhs) const;

    /// @brief Lesser-Equal operator w.r.t. a mapTile enum.
    bool operator<=(const MapTile & rhs) const;

private:
    /// Internal mapTile value.
    Enum mapTile;
};
