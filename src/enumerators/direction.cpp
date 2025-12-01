/// @file   direction.cpp
/// @brief  Implements the direction functions.
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

#include "enumerators/direction.hpp"
#include "structure/coordinates.hpp"
#include "utilities/utils.hpp"

std::string direction_to_string(Direction direction)
{
    switch (direction)
    {
        case Direction::North:
            return "north";
        case Direction::South:
            return "south";
        case Direction::West:
            return "west";
        case Direction::East:
            return "east";
        case Direction::Up:
            return "up";
        case Direction::Down:
            return "down";
        default:
            return "none";
    }
}

Direction direction_from_string(const std::string & value)
{
    if (value == "north") return Direction::North;
    else if (value == "south") return Direction::South;
    else if (value == "west") return Direction::West;
    else if (value == "east") return Direction::East;
    else if (value == "up") return Direction::Up;
    else if (value == "down") return Direction::Down;
    else return Direction::None;
}

Direction get_opposite(Direction direction)
{
    switch (direction)
    {
        case Direction::North:
            return Direction::South;
        case Direction::South:
            return Direction::North;
        case Direction::West:
            return Direction::East;
        case Direction::East:
            return Direction::West;
        case Direction::Up:
            return Direction::Down;
        case Direction::Down:
            return Direction::Up;
        default:
            return Direction::None;
    }
}

Coordinates get_coordinates(Direction direction)
{
    switch (direction)
    {
        case Direction::North:
            return Coordinates(0, +1, 0);
        case Direction::South:
            return Coordinates(0, -1, 0);
        case Direction::West:
            return Coordinates(-1, 0, 0);
        case Direction::East:
            return Coordinates(+1, 0, 0);
        case Direction::Up:
            return Coordinates(0, 0, +1);
        case Direction::Down:
            return Coordinates(0, 0, -1);
        default:
            return Coordinates(0, 0, 0);
    }
}

std::vector<Direction> get_all_directions()
{
    return {Direction::North, Direction::South, Direction::West, Direction::East, Direction::Up, Direction::Down};
}