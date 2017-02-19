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

MovementOptions::MovementOptions() :
    character(),
    allowedInCloseCombat(),
    requiredStamina()
{
    // Nothing to do.
}

MovementOptions::MovementOptions(const MovementOptions & other) :
    character(other.character),
    allowedInCloseCombat(other.allowedInCloseCombat),
    requiredStamina(other.requiredStamina)
{
    // Nothing to do.
}

MovementOptions::~MovementOptions()
{
    // Nothing to do.
}

bool CheckConnection(const MovementOptions & options,
                     Room * r1,
                     Room * r2,
                     std::string & error)
{
    if ((r1 == nullptr) || (r2 == nullptr))
    {
        return false;
    }
    // -------------------------------------------------------------------------
    // Check if there is a connection between the two rooms.
    if (!RoomCheckConnection(r1, r2))
    {
        error = "You cannot go that way.";
        return false;
    }
    // -------------------------------------------------------------------------
    // Check if there is water inside the destination room.
    if (r2->liquid.first != nullptr)
    {
        error = "Do you want to swim maybe?";
        return false;
    }
    // -------------------------------------------------------------------------
    // Get the direction from the first to the second room.
    auto direction = Area::getDirection(r1->coord, r2->coord);
    // Get the connection between the two.
    auto connection = r1->findExit(direction);
    // If the direction is upstairs, check if there is a stair.
    if (direction == Direction::Up)
    {
        if (!HasFlag(connection->flags, ExitFlag::Stairs))
        {
            error = "You can't go upstairs, there are no stairs.";
            return false;
        }
    }
    // -------------------------------------------------------------------------
    // Check if the destination has a floor.
    auto destDown = connection->destination->findExit(Direction::Down);
    if (destDown != nullptr)
    {
        if (!HasFlag(destDown->flags, ExitFlag::Stairs))
        {
            error = "Do you really want to fall in that pit?";
            return false;
        }
    }
    // -------------------------------------------------------------------------
    // Check if the destination is bocked by a door.
    auto door = connection->destination->findDoor();
    if (door != nullptr)
    {
        if (HasFlag(door->flags, ItemFlag::Closed))
        {
            error = "Maybe you have to open that door first.";
            return false;
        }
    }
    // -------------------------------------------------------------------------
    // Perform the characters related checks.
    if (options.character != nullptr)
    {
        // Get the character.
        auto character = options.character;
        // Get the current action of the character.
        auto currentAction = character->getAction();
        // Check if the character is in combat and the option states that the
        // movement cannot be performed while in close combat.
        if ((currentAction->getType() == ActionType::Combat) &&
            (!options.allowedInCloseCombat))
        {
            // Check if the character is locked into close combat.
            bool lockedInCombat = false;
            // Check if he is in the same room of one of its aggressors.
            for (auto iterator : character->combatHandler)
            {
                if (iterator->aggressor != nullptr)
                {
                    if (iterator->aggressor->room == character->room)
                    {
                        lockedInCombat = true;
                        break;
                    }
                }
            }
            // Check even the aimed character.
            if (character->combatHandler.getAimedTarget() != nullptr)
            {
                if (character->combatHandler.getAimedTarget()->room ==
                    character->room)
                {
                    lockedInCombat = true;
                }
            }
            if (lockedInCombat)
            {
                error = "You cannot move while fighting in close combat.";
            }
            return !lockedInCombat;
        }
        // Check if the character is in a no-walk position.
        if ((character->posture >= CharacterPosture::Sit) &&
            (character->posture <= CharacterPosture::Rest))
        {
            error = "You first need to stand up.";
            return false;
        }
        if (character->posture == CharacterPosture::Sleep)
        {
            error = "You first need to wake up.";
            return false;
        }
        // Check if the actor has enough stamina to execute the action.
        if (options.requiredStamina > character->stamina)
        {
            error = "You are too tired to move.\n";
            return false;
        }
        // Check if the destination is forbidden for mobiles.
        if (character->isMobile() &&
            HasFlag(connection->flags, ExitFlag::NoMob))
        {
            error = "Mobiles cannot move through this exit.\n";
            return false;
        }
    }
    return true;
}

bool CheckConnection(const MovementOptions & options,
                     Room * r1,
                     const Direction & direction,
                     std::string & error)
{
    if ((r1 == nullptr) || (direction == Direction::None))
    {
        return false;
    }
    auto foundExit = r1->findExit(direction);
    if (foundExit == nullptr)
    {
        error = "You cannot go that way.";
        return false;
    }
    if (foundExit->destination == nullptr)
    {
        error = "You cannot go that way.";
        return false;
    }
    return CheckConnection(options, r1, foundExit->destination, error);
}