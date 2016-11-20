/// @file   direction.hpp
/// @brief  Define the direction class.
/// @author Enrico Fraccaroli
/// @date   Nov 19 2016
/// @copyright
/// Copyright (c) 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

class Coordinates;

/// Provides a complete controll on directions.
class Direction
{
public:
    /// The possible directions.
    enum Enum
    {
        None,
        North,
        South,
        West,
        East,
        Up,
        Down
    };

    /// @brief Constructor from uint.
    Direction();

    /// @brief Constructor from uint.
    Direction(unsigned int & _direction);

    /// @brief Constructor from enum.
    Direction(Enum _direction);

    /// @brief Constructor from string.
    Direction(const std::string & _direction);

    /// @brief Check is the given number is a valid direction.
    static bool isValid(const unsigned int & _direction);

    /// @brief Check is the given string is a valid direction.
    static bool isValid(const std::string & _direction);

    /// @brief Returns the direction as string.
    std::string toString() const;

    /// @brief Returns the direction as number.
    unsigned int toUInt() const;

    /// @brief Returns the opposite direction.
    Direction getOpposite() const;

    /// @brief Returns the direction in terms of coordinates.
    Coordinates getCoordinates() const;

    /// @brief Equality operator w.r.t. a direction object.
    bool operator==(const Direction & rhs) const;

    /// @brief Equality operator w.r.t. a direction enum.
    bool operator==(const Direction::Enum & rhs) const;

    /// @brief Inequality operator w.r.t. a direction enum.
    bool operator!=(const Direction::Enum & rhs) const;

private:
    /// Internal direction value.
    Enum direction;
};
