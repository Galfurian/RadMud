/// @file   direction.hpp
/// @brief  Define the direction enum.
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

#include <vector>
#include <string>

class Coordinates;

/// Provides a complete control on directions.
enum class Direction
{
    None,   ///< No direction.
    North,  ///< North.
    South,  ///< South.
    West,   ///< West.
    East,   ///< East.
    Up,     ///< Up.
    Down    ///< Down.
};

/// @brief Returns the direction as string.
std::string direction_to_string(Direction direction);

/// @brief Returns the direction from string.
Direction string_to_direction(const std::string & value);

/// @brief Returns the opposite direction.
Direction get_opposite(Direction direction);

/// @brief Returns the direction in terms of coordinates.
Coordinates get_coordinates(Direction direction);

/// Vector with all the possible directions.
std::vector<Direction> get_all_directions();
