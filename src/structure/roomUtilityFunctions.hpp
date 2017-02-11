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

/// @brief Checks if the two given room are connected.
/// @param r1        The first room.
/// @param r2        The second room.
/// @param character The character who wants to go from r1 to r2.
/// @return If there is a connection between the two rooms.
bool RoomCharacterCanMove(Room * r1, Room * r2, Character * character);

/// Functions used to get the distance between two rooms.
int RoomGetDistance(Room * r1, Room * r2);

/// Function used to check the equality between two rooms.
bool RoomAreEqual(Room * r1, Room * r2);

/// Function used to get the neighbours of the given room.
std::vector<Room *> RoomGetNeighbours(Room * r);
