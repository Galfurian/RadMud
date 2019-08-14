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

#include "direction.hpp"
#include "coordinates.hpp"
#include "utils.hpp"

Direction::Direction(const std::string &_direction, const bool &exact) :
	BaseEnumerator()
{
	if ((exact && (_direction == "north")) ||
		(!exact && BeginWith("north", _direction))) {
		value = North;
	} else if ((exact && (_direction == "south")) ||
			   (!exact && BeginWith("south", _direction))) {
		value = South;
	} else if ((exact && (_direction == "west")) ||
			   (!exact && BeginWith("west", _direction))) {
		value = West;
	} else if ((exact && (_direction == "east")) ||
			   (!exact && BeginWith("east", _direction))) {
		value = East;
	} else if ((exact && (_direction == "up")) ||
			   (!exact && BeginWith("up", _direction))) {
		value = Up;
	} else if ((exact && (_direction == "down")) ||
			   (!exact && BeginWith("down", _direction))) {
		value = Down;
	} else
		value = None;
}

std::string Direction::toString() const
{
	if (value == Direction::North)
		return "north";
	if (value == Direction::South)
		return "south";
	if (value == Direction::West)
		return "west";
	if (value == Direction::East)
		return "east";
	if (value == Direction::Up)
		return "up";
	if (value == Direction::Down)
		return "down";
	return "none";
}

Direction Direction::getOpposite() const
{
	if (value == Direction::North)
		return Direction::South;
	if (value == Direction::South)
		return Direction::North;
	if (value == Direction::West)
		return Direction::East;
	if (value == Direction::East)
		return Direction::West;
	if (value == Direction::Up)
		return Direction::Down;
	if (value == Direction::Down)
		return Direction::Up;
	return Direction::None;
}

Coordinates Direction::getCoordinates() const
{
	if (value == Direction::North)
		return Coordinates(0, +1, 0);
	if (value == Direction::South)
		return Coordinates(0, -1, 0);
	if (value == Direction::West)
		return Coordinates(-1, 0, 0);
	if (value == Direction::East)
		return Coordinates(+1, 0, 0);
	if (value == Direction::Up)
		return Coordinates(0, 0, +1);
	if (value == Direction::Down)
		return Coordinates(0, 0, -1);
	return Coordinates(0, 0, 0);
}

std::vector<Direction> Direction::getAllDirections()
{
	std::vector<Direction> directions;
	directions.emplace_back(Direction::North);
	directions.emplace_back(Direction::South);
	directions.emplace_back(Direction::West);
	directions.emplace_back(Direction::East);
	directions.emplace_back(Direction::Up);
	directions.emplace_back(Direction::Down);
	return directions;
}