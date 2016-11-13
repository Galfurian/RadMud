/// @file   object.hpp
/// @author Enrico Fraccaroli
/// @date   Aug 23 2016
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

#include "argumentHandler.hpp"
#include "character.hpp"

/// Loads all the commands concerning objects.
void LoadObjectCommands();

/// @defgroup ComInterfaces Commands List Interface.
/// @brief All the functions necessary to handle the commands that a player can execute.
/// @{

/// Allow the character to pick up an object, or take an object from another one.
void DoTake(Character * character, ArgumentHandler & args);

/// Allow the character to drop an object.
void DoDrop(Character * character, ArgumentHandler & args);

/// Allow the character to give an object to another character.
void DoGive(Character * character, ArgumentHandler & args);

/// List all the item equipped by the character.
void DoEquipments(Character * character, ArgumentHandler & args);

/// Wield a object, if it can be wielded.
void DoWield(Character * character, ArgumentHandler & args);

/// Wear a object, if it can be weared.
void DoWear(Character * character, ArgumentHandler & args);

/// Remove an equipped or wielded item.
void DoRemove(Character * character, ArgumentHandler & args);

/// List the character's inventory.
void DoInventory(Character * character, ArgumentHandler & args);

/// Organize the items in the room.
void DoOrganize(Character * character, ArgumentHandler & args);

/// Open a door if not unlocked.
void DoOpen(Character * character, ArgumentHandler & args);

/// Close a door.
void DoClose(Character * character, ArgumentHandler & args);

/// Put an object inside a container.
void DoPut(Character * character, ArgumentHandler & args);

/// Drink from a container of liquids.
void DoDrink(Character * character, ArgumentHandler & args);

/// Fill a container of liquids from a source of liquid.
void DoFill(Character * character, ArgumentHandler & args);

/// Pour the container of liquids into another one or on the ground.
void DoPour(Character * character, ArgumentHandler & args);

/// Deposit a currency inside a shop.
void DoDeposit(Character * character, ArgumentHandler & args);

/// Sell an item.
void DoSell(Character * character, ArgumentHandler & args);

/// Buy an item.
void DoBuy(Character * character, ArgumentHandler & args);

/// Shows the total ammount of monney.
void DoBalance(Character * character, ArgumentHandler & args);

///@}
