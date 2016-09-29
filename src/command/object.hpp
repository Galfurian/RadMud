/// @file   object.hpp
/// @author Enrico Fraccaroli
/// @date   Aug 23 2016
/// @copyright
/// Copyright (c) 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
/// Permission to use, copy, modify, and distribute this software for any
/// purpose with or without fee is hereby granted, provided that the above
/// copyright notice and this permission notice appear in all copies.
///
/// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
/// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
/// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
/// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
/// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
/// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
/// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#pragma once

#include "argumentHandler.hpp"
#include "../character/character.hpp"

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
