/// @file   roomUtilityFunctions.hpp
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

class Room;

class Character;

class Direction;

/// @brief Provides the list of directions where an exit is present.
/// @param r The room from which the check must be performed.
/// @return Vector of directions.
std::vector<Direction> GetAvailableDirections(Room * r);

/// @brief Provides the list of rooms connected to the given one.
/// @param r The room from which the check must be performed.
/// @return Vector of connected rooms.
std::vector<Room *> GetConnectedRooms(Room * r);

/// @brief Checks if the two given room are connected.
/// @param r1 The first room.
/// @param r2 The second room.
/// @return If there is a connection between the two rooms.
bool RoomCheckConnection(Room * r1, Room * r2);

/// Functions used to get the distance between two rooms.
int RoomGetDistance(Room * r1, Room * r2);

/// Function used to check the equality between two rooms.
bool RoomAreEqual(Room * r1, Room * r2);

/// Function used to get the neighbours of the given room.
std::vector<Room *> RoomGetNeighbours(Room * r);

/// @brief Structure which contains options used to check rooms connections.
struct MovementOptions
{
public:
    /// Allows the function to check if the character can perform the move.
    Character * character;
    /// Allows to set if the given character can perform the move during
    /// close combat.
    bool allowedInCloseCombat;
    /// Allows to set the required amount of stamina required to move.
    unsigned int requiredStamina;

    /// @brief Constructor.
    MovementOptions();

    /// @brief Copy constructor.
    MovementOptions(const MovementOptions & other);

    /// @brief Destructor.
    ~MovementOptions();
};

/// @brief Check the connection between the two given rooms and the given
///         options.
/// @param options  The character that wants to move.
/// @param r1       The first room.
/// @param r2       The second room.
/// @param error    A reference to a string which will contain error
///                 message in case of failure.
/// @return <b>True</b> if there is a valid connection between the rooms,<br>
///         <b>False</b> otherwise.
bool CheckConnection(const MovementOptions & options,
                     Room * r1,
                     Room * r2,
                     std::string & error);

/// @brief Checks if there is a valid connection on the given direction.
bool CheckConnection(const MovementOptions & options,
                     Room * r1,
                     const Direction & direction,
                     std::string & error);
