/// @file   lua_script.hpp
/// @brief  Define all the methods used by lua to interract and communicate with the mud.
/// @author Enrico Fraccaroli
/// @date   Aug 25 2014
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

#pragma once

#include <string>
#include <vector>

struct lua_State;

class Item;

class Character;

class Room;

class Direction;

namespace luabridge
{
class LuaRef;
}

/// @brief Allow from lua code, to log a string.
/// @param message The message to log.
void LuaLog(std::string message);

/// @brief Allow from lua coden to generate a random integer.
/// @param min Lower bound.
/// @param max Upper bound.
/// @return The generated random value.
int LuaRandom(int min, int max);

/// @brief Allows to stop the script (and the Thread) from inside the Lua
/// Environment.
void LuaStopScript();

/// @brief Allow from lua to load an item.
/// @param character    The character creating the item.
/// @param vnumModel    The vnum of the model.
/// @param vnumMaterial The vnum of the material.
/// @param qualityValue The initial quality of the item.
/// @return The newly created item.
Item * LuaLoadItem(Character * character,
                   unsigned int vnumModel,
                   unsigned int vnumMaterial,
                   unsigned int qualityValue);

/// @brief Provides the path from the character to the destination.
/// @param character    The character which has to move.
/// @param destination  The destination to reach.
/// @return The path (as list of directions) to the destination.
std::vector<Direction> LuaFindPath(
    Character * character,
    Room * destination);

/// @brief Returns the list of rooms in sight.
std::vector<Room *> LuaGetRoomsInSight(Character * character);

/// @brief Returns the list of characters in sight.
std::vector<Character *> LuaGetCharactersInSight(Character * character);

/// @brief Returns the list of items in sight.
std::vector<Item *> LuaGetItemsInSight(Character * character);

/// @brief Register every mud element inside the Lua environment.
void LoadLuaEnvironmet(lua_State * L, const std::string & scriptFile);

