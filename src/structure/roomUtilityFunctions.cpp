/// @file   roomUtilityFunctions.cpp
/// @author Enrico Fraccaroli
/// @date   Feb 11 2017
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

#include "roomUtilityFunctions.hpp"
#include "character.hpp"
#include "area.hpp"
#include "room.hpp"

std::vector<Direction> GetAvailableDirections(Room * r)
{
    std::vector<Direction> directions;
    if (r != nullptr)
    {
        for (auto it : r->exits)
        {
            if (it->destination != nullptr)
            {
                directions.push_back(it->direction);
            }
        }
    }
    return directions;
}

std::vector<Room *> GetConnectedRooms(Room * r)
{
    std::vector<Room *> connectedRooms;
    if (r != nullptr)
    {
        for (auto it : r->exits)
        {
            if (it->destination != nullptr)
            {
                connectedRooms.push_back(it->destination);
            }
        }
    }
    return connectedRooms;
}

bool RoomCheckConnection(Room * r1, Room * r2)
{
    if ((r1 == nullptr) || (r2 == nullptr)) return false;
    for (auto destination : GetConnectedRooms(r1))
    {
        if (destination->vnum == r2->vnum)
        {
            return true;
        }
    }
    return false;
}

bool RoomCharacterCanMove(Room * r1, Room * r2, Character * character)
{
    if ((r1 == nullptr) || (r2 == nullptr) || (character == nullptr))
    {
        return false;
    }
    if (!RoomCheckConnection(r1, r2))
    {
        return false;
    }
    // Get the direction from the first to the second room.
    auto direction = Area::getDirection(r1->coord, r2->coord);
    // Get the connection between the two.
    auto connection = r1->findExit(direction);
    if (connection == nullptr)
    {
        return false;
    }
    // If the direction is upstairs, check if there is a stair.
    if (direction == Direction::Up)
    {
        if (!HasFlag(connection->flags, ExitFlag::Stairs))
        {
            return false;
        }
    }
    // Check if the destination is correct.
    if (connection->destination == nullptr)
    {
        return false;
    }
    // Check if the destination is bocked by a door.
    auto door = connection->destination->findDoor();
    if (door != nullptr)
    {
        if (HasFlag(door->flags, ItemFlag::Closed))
        {
            return false;
        }
    }
    // Check if the destination has a floor.
    auto destDown = connection->destination->findExit(Direction::Down);
    if (destDown != nullptr)
    {
        if (!HasFlag(destDown->flags, ExitFlag::Stairs))
        {
            return false;
        }
    }
    // Check if the destination is forbidden for mobiles.
    return !(character->isMobile() &&
             HasFlag(connection->flags, ExitFlag::NoMob));
}

int RoomGetDistance(Room * r1, Room * r2)
{
    if ((r1 == nullptr) || (r2 == nullptr)) return INT_MAX;
    return Area::getDistance(r1->coord, r2->coord);
}

bool RoomAreEqual(Room * r1, Room * r2)
{
    if ((r1 == nullptr) || (r2 == nullptr)) return false;
    return (r1->vnum == r2->vnum);
}

std::vector<Room *> RoomGetNeighbours(Room * r)
{
    std::vector<Room *> neighbours;
    for (auto neighbour : r->exits)
    {
        if (neighbour->destination != nullptr)
        {
            neighbours.emplace_back(neighbour->destination);
        }
    }
    return neighbours;
}
