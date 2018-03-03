/// @file   structureUtils.hpp
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

#pragma once

#include <vector>
#include <string>
#include <cmath>

#include "room.hpp"

class Item;

class Character;

class Direction;

class Terrain;

class Area;

/// @brief Structure which contains options used to check rooms connections.
struct MovementOptions
{
    /// The character which performs the move.
    Character * character;
    /// If the movement can be performed during close combat.
    bool allowedInCloseCombat;
    /// The required amount of stamina required to move.
    unsigned int requiredStamina;
};

/// @brief Structure which contains options used to select specific rooms.
struct RoomSelectionOptions
{
    /// The selected terrain.
    Terrain * terrain;
};

namespace StructUtils
{
/// @brief Provides the distance between the source and the target.
/// @param source The source coordinates.
/// @param target The target coordinates.
/// @return The distance between the two coordinates.
inline int getDistance(Coordinates const & source,
                       Coordinates const & target)
{
    return static_cast<int>(
        std::sqrt(
            std::pow(source.x - target.x, 2) +
            std::pow(source.y - target.y, 2) +
            std::pow(source.z - target.z, 2)));
}

/// Functions used to get the distance between two rooms.
int getRoomDistance(Room * from, Room * to);

/// @brief Provides the direction of the target w.r.t. the source.
/// @param source The source coordinates.
/// @param target The target coordinates.
/// @return The direction from the source to the target.
Direction getDirection(const Coordinates & source, const Coordinates & target);

/// @brief Provides the list of directions where an exit is present.
/// @param room The room from which the check must be performed.
/// @return Vector of directions.
std::vector<Direction> getDirections(Room * room);

/// @brief Provides the list of rooms connected to the given one.
/// @param room The room from which the check must be performed.
/// @return Vector of connected rooms.
std::vector<Room *> getConnectedRooms(Room * room);

/// @brief Returns the exit which connects the first room to the second one.
/// @param from The starting room.
/// @param to   The target room.
/// @return The connection between the first and second room.
std::shared_ptr<Exit> getConnection(Room * from, Room * to);

/// Function used to check the equality between two rooms.
inline bool roomsAreEqual(Room * r1, Room * r2)
{
    if ((r1 == nullptr) || (r2 == nullptr)) return false;
    return (r1->vnum == r2->vnum);
}

/// Function used to get the neighbours of the given room.
std::vector<Room *> getNeighbours(Room * r);

/// @brief Check the connection between the two given rooms and the given
///         options.
/// @param options  The character that wants to move.
/// @param r1       The first room.
/// @param r2       The second room.
/// @param error    A reference to a string which will contain error
///                 message in case of failure.
/// @return <b>True</b> if there is a valid connection between the rooms,<br>
///         <b>False</b> otherwise.
bool checkConnection(const MovementOptions & options,
                     Room * r1,
                     Room * r2,
                     std::string & error);

/// @brief Checks if there is a valid connection on the given direction.
bool checkConnection(const MovementOptions & options,
                     Room * r1,
                     const Direction & direction,
                     std::string & error);

/// @brief Search for a built door inside the room.
/// @return The contained door if there is one.
Item * findDoor(Room * room);

std::vector<Room *> selectRooms(Area * area,
                                Room * startingRoom,
                                RoomSelectionOptions options);

/// @brief A Field of View algorithm which provides all the rooms
///         which are inside the radius of the field of view.
/// @param origin The coordinate of the central room.
/// @param radius The radius of visibility of the character.
/// @return A vector containing all the coordinates of valid rooms.
std::vector<Coordinates> fov(Coordinates & origin,
                             const int & radius,
                             Area * area);

/// @brief A Field of View algorithm which provides all the rooms
///         which are inside the radius of the field of view.
/// @param origin The coordinate of the central room.
/// @param radius The radius of visibility of the character.
/// @return A vector containing all the coordinates of valid rooms.
std::vector<Coordinates> fov3d(Coordinates & origin,
                               const int & radius,
                               Area * area);

/// @brief Determine if a coordinate is in sight from a starting one.
/// @param source The coordinates of the origin.
/// @param target The coordinates of the target room.
/// @param radius   The radius of visibility.
/// @return <b>True</b> if the target room is in sight,<br>
///         <b>False</b> otherwise.
bool los(const Coordinates & source,
         const Coordinates & target,
         const int & radius,
         Area * area);

}
