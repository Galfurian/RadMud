/// @file   direction.cpp
/// @brief  Implements the direction class methods.
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

#include <functional>
#include <utilities/utils.hpp>
#include "direction.hpp"
#include "coordinates.hpp"

Direction::Direction() :
    direction(None)
{
    // Nothing to do.
}

Direction::Direction(unsigned int & _direction) :
    direction()
{
    if (_direction == 1) direction = North;
    else if (_direction == 2) direction = South;
    else if (_direction == 3) direction = West;
    else if (_direction == 4) direction = East;
    else if (_direction == 5) direction = Up;
    else if (_direction == 6) direction = Down;
    else direction = None;
}

Direction::Direction(Enum _direction) :
    direction(_direction)
{
    // Nothing to do.
}

Direction::Direction(const std::string & _direction, const bool & exact) :
    direction()
{
    if ((exact && (_direction == "north")) ||
        (!exact && BeginWith("north", _direction)))
    {
        direction = North;
    }
    else if ((exact && (_direction == "south")) ||
             (!exact && BeginWith("south", _direction)))
    {
        direction = South;
    }
    else if ((exact && (_direction == "west")) ||
             (!exact && BeginWith("west", _direction)))
    {
        direction = West;
    }
    else if ((exact && (_direction == "east")) ||
             (!exact && BeginWith("east", _direction)))
    {
        direction = East;
    }
    else if ((exact && (_direction == "up")) ||
             (!exact && BeginWith("up", _direction)))
    {
        direction = Up;
    }
    else if ((exact && (_direction == "down")) ||
             (!exact && BeginWith("down", _direction)))
    {
        direction = Down;
    }
    else direction = None;
}

bool Direction::isValid(const unsigned int & _direction)
{
    return (_direction >= 1) && (_direction <= 6);
}

bool Direction::isValid(const std::string & _direction)
{
    if (_direction == "north") return true;
    if (_direction == "south") return true;
    if (_direction == "west") return true;
    if (_direction == "east") return true;
    if (_direction == "up") return true;
    return (_direction == "down");
}

std::string Direction::toString() const
{
    if (direction == Direction::North) return "north";
    if (direction == Direction::South) return "south";
    if (direction == Direction::West) return "west";
    if (direction == Direction::East) return "east";
    if (direction == Direction::Up) return "up";
    if (direction == Direction::Down) return "down";
    return "none";
}

unsigned int Direction::toUInt() const
{
    return static_cast<unsigned int>(direction);
}

Direction Direction::getOpposite() const
{
    if (direction == Direction::North) return Direction::South;
    if (direction == Direction::South) return Direction::North;
    if (direction == Direction::West) return Direction::East;
    if (direction == Direction::East) return Direction::West;
    if (direction == Direction::Up) return Direction::Down;
    if (direction == Direction::Down) return Direction::Up;
    return Direction::None;
}

Coordinates Direction::getCoordinates() const
{
    if (direction == Direction::North) return Coordinates(0, +1, 0);
    if (direction == Direction::South) return Coordinates(0, -1, 0);
    if (direction == Direction::West) return Coordinates(-1, 0, 0);
    if (direction == Direction::East) return Coordinates(+1, 0, 0);
    if (direction == Direction::Up) return Coordinates(0, 0, +1);
    if (direction == Direction::Down) return Coordinates(0, 0, -1);
    return Coordinates(0, 0, 0);
}

bool Direction::operator==(const Direction & rhs) const
{
    return direction == rhs.direction;
}

bool Direction::operator!=(const Direction & rhs) const
{
    return direction != rhs.direction;
}

bool Direction::operator<(const Direction & rhs) const
{
    return direction < rhs.direction;
}

void Direction::luaRegister(lua_State * L)
{
    luabridge::getGlobalNamespace(L)
        .beginClass<Direction>("Direction")
        .addConstructor < void(*)(const std::string &)>()
        .addFunction("toString", &Direction::toString)
//        .addEnum("North", Direction::North)
//        .addEnum("South", Direction::South)
//        .addEnum("West", Direction::West)
//        .addEnum("East", Direction::East)
//        .addEnum("Up", Direction::Up)
//        .addEnum("Down", Direction::Down)
        .endClass();
}
