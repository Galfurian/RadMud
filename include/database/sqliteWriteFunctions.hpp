/// @file   sqliteWriteFunctions.hpp
/// @author Enrico Fraccaroli
/// @date   feb 05 2017
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

#include <memory>
#include <item/subitem/shopItem.hpp>

// Forward declarations.
class Area;

class Room;

class Exit;

class Player;

class Item;

/// @brief Allows to save the given player inside the database.
bool SavePlayer(Player * player);

/// @brief Allows to save the skills of the given player inside the database.
bool SavePlayerSkills(Player * player);

/// @brief Allows to save the lua variables of the given player inside the
/// database.
bool SavePlayerLuaVariables(Player * player);

/// @brief Allows to save the items posessed by the given player inside the
/// database.
bool SaveItemPlayer(Player * player,
                    Item * item,
                    const unsigned int & bodyPartVnum);

/// @brief Allows to save the information concerning the given shop inside
/// the database.
bool SaveShopItem(ShopItem * item,
                  const bool & transaction);

/// @brief Allows to save the given item inside the database.
bool SaveItem(Item * item,
              const bool & transaction);

/// @brief Allows to save the given area inside the database.
bool SaveArea(Area * area);

/// @brief Allows to save the given room inside the database.
bool SaveRoom(Room * room);

/// @brief Allows to save the given pair of room and area inside the database.
bool SaveAreaList(Area * area, Room * room);

/// @brief Allows to save the given exit inside the database.
bool SaveRoomExit(const std::shared_ptr<Exit> & roomExit);
