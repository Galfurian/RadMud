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

#include "structure/structureUtils.hpp"
#include "model/submodel/mechanismModel.hpp"
#include "character/character.hpp"
#include "structure/area.hpp"
#include "structure/room.hpp"

namespace StructUtils
{

Direction getDirection(const Coordinates & source,
                       const Coordinates & target)
{
    auto dx = std::abs(source.x - target.x);
    auto dy = std::abs(source.y - target.y);
    auto dz = std::abs(source.z - target.z);
    if ((dx > dy) && (dx > dz))
    {
        if (source.x > target.x) return Direction::West;
        if (source.x < target.x) return Direction::East;
    }
    if ((dy > dx) && (dy > dz))
    {
        if (source.y > target.y) return Direction::South;
        if (source.y < target.y) return Direction::North;
    }
    if ((dz > dx) && (dz > dy))
    {
        if (source.z > target.z) return Direction::Down;
        if (source.z < target.z) return Direction::Up;
    }
    return Direction::None;
}

std::vector<Direction> getDirections(Room * room)
{
    std::vector<Direction> result;
    if (room == nullptr) return result;
    for (auto const & it : room->exits)
    {
        if (it->destination != nullptr) result.emplace_back(it->direction);
    }
    return result;
}

std::vector<Room *> getConnectedRooms(Room * room)
{
    std::vector<Room *> result;
    if (room == nullptr) return result;
    for (auto const & it : room->exits)
    {
        if (it->destination != nullptr) result.emplace_back(it->destination);
    }
    return result;
}

bool checkConnectionBetween(Room * from, Room * to)
{
    if ((from == nullptr) || (to == nullptr)) return false;
    for (auto destination : StructUtils::getConnectedRooms(from))
    {
        if (destination->vnum == to->vnum) return true;
    }
    return false;
}

std::vector<Room *> getNeighbours(Room * room)
{
    std::vector<Room *> result;
    if (room == nullptr) return result;
    for (auto const & it : room->exits)
    {
        if (it->destination != nullptr) result.emplace_back(it->destination);
    }
    return result;
}

bool checkConnection(const MovementOptions & options,
                     Room * r1,
                     Room * r2,
                     std::string & error)
{
    if ((r1 == nullptr) || (r2 == nullptr)) return false;
    // -------------------------------------------------------------------------
    // Check if there is a connection between the two rooms.
    if (!StructUtils::checkConnectionBetween(r1, r2))
    {
        error = "You cannot go that way.";
        return false;
    }
    // -------------------------------------------------------------------------
    // Check if there is water inside the destination room.
    if (r2->liquidContent.first != nullptr)
    {
        error = "Do you want to swim maybe?";
        return false;
    }
    // -------------------------------------------------------------------------
    // Get the direction from the first to the second room.
    auto direction = StructUtils::getDirection(r1->coord, r2->coord);
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
    auto door = StructUtils::findDoor(connection->destination);
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

bool checkConnection(const MovementOptions & options,
                     Room * r1,
                     const Direction & direction,
                     std::string & error)
{
    // Check the room and the direction.
    if ((r1 == nullptr) || (direction == Direction::None)) return false;
    // Get the exit in the given direction.
    auto foundExit = r1->findExit(direction);
    // Check if there is an exit.
    if (foundExit == nullptr)
    {
        error = "You cannot go that way.";
        return false;
    }
    // Check if the destination is set.
    if (foundExit->destination == nullptr)
    {
        error = "You cannot go that way.";
        return false;
    }
    return StructUtils::checkConnection(options,
                                        r1,
                                        foundExit->destination,
                                        error);
}

Item * findDoor(Room * room)
{
    // Check the room.
    if (room == nullptr) return nullptr;
    // Search the door.
    for (auto it : room->items)
    {
        // Check if the item is a mechanism.
        if (it->model->getType() != ModelType::Mechanism) continue;
        // Check if the item is built.
        if (HasFlag(it->flags, ItemFlag::Built)) continue;
        // Cast the model to mechanism.
        auto mechanism = std::static_pointer_cast<MechanismModel>(it->model);
        // Check if the mechanism is a door.
        if (mechanism->mechanismType == MechanismType::Door) return it;
    }
    return nullptr;
}

std::vector<Room *> selectRooms(Area * area,
                                Room * startingRoom,
                                RoomSelectionOptions options)
{
    std::vector<Room *> selectedRooms;
    std::function<void(Room *)> RecursiveSelectRooms;
    RecursiveSelectRooms = [&](Room * room)
    {
        for (auto it : selectedRooms)
        {
            if (room->vnum == it->vnum) return;
        }
        if (options.terrain)
        {
            if (room->terrain->vnum != options.terrain->vnum) return;
        }
        for (auto dir : Direction::getAllDirections())
        {
            auto next(area->getRoom(room->coord + dir));
            if (next)
            {
                RecursiveSelectRooms(next);
            }
        }
    };
    RecursiveSelectRooms(startingRoom);
    return selectedRooms;
}

}
