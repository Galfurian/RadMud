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

#include "structureUtils.hpp"
#include "mechanismModel.hpp"
#include "doubleOperators.hpp"
#include "area.hpp"

namespace StructUtils
{

int getRoomDistance(Room * from, Room * to)
{
    if ((from == nullptr) || (to == nullptr)) return INT_MAX;
    return getDistance(from->coord, to->coord);
}

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

std::shared_ptr<Exit> getConnection(Room * from, Room * to)
{
    if ((from == nullptr) || (to == nullptr)) return nullptr;
    for (auto const & it : from->exits)
    {
        if (it->destination != nullptr)
        {
            if (it->destination->vnum == to->vnum) return it;
        }
    }
    return nullptr;
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
    // Get the connection between the two.
    auto connection = StructUtils::getConnection(r1, r2);
    // Check if there is a connection between the two rooms.
    if (connection == nullptr)
    {
        error = "You cannot go that way.";
        return false;
    }
    // Check if there is water inside the destination room.
    if (r2->liquidContent.first != nullptr)
    {
        error = "Do you want to swim maybe?";
        return false;
    }
    // Get the direction from the first to the second room.
    auto direction = StructUtils::getDirection(r1->coord, r2->coord);
    // If the direction is upstairs, check if there is a stair.
    if (direction == Direction::Up)
    {
        if (!HasFlag(connection->flags, ExitFlag::Stairs))
        {
            error = "You can't go upstairs, there are no stairs.";
            return false;
        }
    }
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
            for (auto const & iterator : character->combatHandler)
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
        for (auto const & dir : Direction::getAllDirections())
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

std::vector<Coordinates> fov(Coordinates & origin,
                             const int & radius,
                             Area * area)
{
    std::vector<Coordinates> cfov;
    auto CheckCoordinates = [&](const Coordinates & coordinates)
    {
        if (std::find(cfov.begin(), cfov.end(), coordinates) == cfov.end())
        {
            if (origin == coordinates)
            {
                cfov.emplace_back(coordinates);
            }
            else if (los(origin, coordinates, radius, area))
            {
                cfov.emplace_back(coordinates);
            }
        }
    };
    Coordinates point;

    while (point.x <= radius)
    {
        while ((point.y <= point.x) && (point.square() <= pow(radius, 2)))
        {
            // ---------------------------------------------------------
            CheckCoordinates(Coordinates(origin.x + point.x,
                                         origin.y + point.y,
                                         origin.z + point.z));
            // ---------------------------------------------------------
            CheckCoordinates(Coordinates(origin.x - point.x,
                                         origin.y + point.y,
                                         origin.z + point.z));
            // ---------------------------------------------------------
            CheckCoordinates(Coordinates(origin.x + point.x,
                                         origin.y - point.y,
                                         origin.z + point.z));
            // ---------------------------------------------------------
            CheckCoordinates(Coordinates(origin.x - point.x,
                                         origin.y - point.y,
                                         origin.z + point.z));
            // ---------------------------------------------------------
            CheckCoordinates(Coordinates(origin.x + point.y,
                                         origin.y + point.x,
                                         origin.z + point.z));
            // ---------------------------------------------------------
            CheckCoordinates(Coordinates(origin.x - point.y,
                                         origin.y + point.x,
                                         origin.z + point.z));
            // ---------------------------------------------------------
            CheckCoordinates(Coordinates(origin.x + point.y,
                                         origin.y - point.x,
                                         origin.z + point.z));
            // ---------------------------------------------------------
            CheckCoordinates(Coordinates(origin.x - point.y,
                                         origin.y - point.x,
                                         origin.z + point.z));
            // ---------------------------------------------------------
            ++point.y;
        }
        ++point.x;
        point.y = 0;
    }
    return cfov;
}

std::vector<Coordinates> fov3d(Coordinates & origin,
                               const int & radius,
                               Area * area)
{
    int x0 = origin.x, y0 = origin.y, z0 = origin.z;
    std::vector<Coordinates> fov_coords;
    auto isinside = [&](int x, int y, int z)
    {
        return (x * x) + (y * y) + (z * z) <= (radius * radius);
    };
    auto check = [&](int x, int y, int z)
    {
        Coordinates coord(x, y, z);
        if (origin == coord)
        {
            fov_coords.emplace_back(coord);
        }
        else if (los(origin, coord, radius, area))
        {
            fov_coords.emplace_back(coord);
        }
    };
    for (int z = 0; z < (radius * 2); ++z)
    {
        if (!isinside(0, 0, z)) continue;
        for (int y = 0; y < (radius * 2); ++y)
        {
            if (!isinside(0, y, z)) continue;
            for (int x = 0; x < (radius * 2); ++x)
            {
                if (!isinside(x, y, z)) continue;
                // Upper-Half.
                // Lower Right.
                check(x0 + x, y0 + y, z0 + z);
                // Lower Left.
                check(x0 + x, y0 - y, z0 + z);
                // Upper Right.
                check(x0 - x, y0 + y, z0 + z);
                // Upper Left.
                check(x0 - x, y0 - y, z0 + z);
                // Lower-Half.
                // Lower Right.
                check(x0 + x, y0 + y, z0 - z);
                // Lower Left.
                check(x0 + x, y0 - y, z0 - z);
                // Upper Right.
                check(x0 - x, y0 + y, z0 - z);
                // Upper Left.
                check(x0 - x, y0 - y, z0 - z);
            }
        }
    }
    return fov_coords;
}

bool los(const Coordinates & source,
         const Coordinates & target,
         const int & radius,
         Area * area)
{
    // Deal with the easiest case.
    if (source == target) return true;
    // Check if there is a room at the given coordinates.
    if (area->getRoom(target) == nullptr) return false;
    // Ensure that the line will not extend too long.
    if (StructUtils::getDistance(source, target) > radius) return false;
    std::cout << target.toString() << " : ";
    // Evaluates the difference.
    double dx = target.x - source.x;
    double dy = target.y - source.y;
    double dz = target.z - source.z;
    // Evaluate the distance between the
    double distance = std::sqrt((dx * dx) + (dy * dy) + (dz * dz));
    // Evaluate the unit increment for both X and Y.
    // Decrease the value of precision for a faster execution with
    //  a worsening in terms of accuracy (default 6).
    double precision = 5;
    double unitx = dx / (distance * precision);
    double unity = dy / (distance * precision);
    double unitz = dz / (distance * precision);
    // Evaluate the minimum value of increment.
    double min = 0.1;
    auto absX = std::abs(unitx);
    auto absY = std::abs(unity);
    auto absZ = std::abs(unitz);
    if (absX > 0.0) min = std::min(min, absX);
    if (absY > 0.0) min = std::min(min, absY);
    if (absZ > 0.0) min = std::min(min, absZ);
    // Set the initial values for X and Y.
    double x = source.x + 0.5;
    double y = source.y + 0.5;
    double z = source.z + 0.5;
    Coordinates coordinates(source.x, source.y, source.z);
    //Coordinates previous = coordinates;
    for (double i = 0; IsLEqual(i, distance); i += min)
    {
        // Evaluate the integer version of the coordinates
        //  using the floor value.
        coordinates = Coordinates(std::floor(x), std::floor(y), std::floor(z));
        if (!area->isValid(coordinates))
        {
            std::cout << " cannot reach.\n";
            return false;
        }
#if 0
        if (coordinates.z > previous.z)
        {
            auto downstair = area->getRoom(previous);
            if (!downstair->findExit(Direction::Up))
            {
                std::cout << " no z connection up.\n";
                return false;
            }
        }
        if (coordinates.z < previous.z)
        {
            auto upstair = area->getRoom(previous);
            if (!upstair->findExit(Direction::Down))
            {
                std::cout << " no z connection down.\n";
                return false;
            }
        }
#endif
        if (coordinates == target)
        {
            std::cout << " found.\n";
            return true;
        }
        // Increment the coordinates.
        x += unitx;
        y += unity;
        z += unitz;
    }
    return false;
}

}
