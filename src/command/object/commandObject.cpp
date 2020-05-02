/// @file   commandObject.cpp
/// @author Enrico Fraccaroli
/// @date   Aug 23 2014
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

#include "command/object/commandObject.hpp"

#include "command/object/commandObjectLightSource.hpp"
#include "command/object/commandObjectManagement.hpp"
#include "command/object/commandObjectContainer.hpp"
#include "command/object/commandObjectCharacter.hpp"
#include "command/object/commandObjectProcess.hpp"
#include "command/object/commandObjectLiquids.hpp"
#include "command/object/commandObjectShop.hpp"
#include "command/object/commandObjectFood.hpp"
#include "mud.hpp"

void LoadObjectCommands()
{
	// ////////////////////////////////////////////////////////////////////////
	// COMMAND MANAGEMENT
	Mud::instance().addCommand(std::make_shared<Command>(
		DoTake, "take", "(item|all) [(container)]",
		"Take something from the ground or from a container.", false, true, false));
	Mud::instance().addCommand(std::make_shared<Command>(DoDrop, "drop", "(item|all)",
														 "Drop an object.", false, true, false));
	Mud::instance().addCommand(std::make_shared<Command>(DoPut, "put", "(something) (container)",
														 "Put something inside a container.", false,
														 true, false));
	Mud::instance().addCommand(std::make_shared<Command>(
		DoGive, "give", "(item|all) (someone)", "Give an object to someone.", false, false, false));

	// ////////////////////////////////////////////////////////////////////////
	// COMMAND LIQUIDS
	Mud::instance().addCommand(std::make_shared<Command>(DoDrink, "drink", "(liquid container)",
														 "Drink from a container of liquids.",
														 false, false, false));
	Mud::instance().addCommand(std::make_shared<Command>(
		DoFill, "fill", "(liquid container) (source of liquid)",
		"Fill a container of liquids from a source of liquid.", false, false, false));
	Mud::instance().addCommand(std::make_shared<Command>(
		DoPour, "pour", "(liquid container) [liquid container]",
		"Pour the content of the container into another one or on the ground.", false, false,
		false));

	// ////////////////////////////////////////////////////////////////////////
	// Commands for food.
	Mud::instance().addCommand(std::make_shared<Command>(
		DoEat, "eat", "(food)", "Eats the provided food.", false, false, false));

	// ////////////////////////////////////////////////////////////////////////
	// COMMAND CONTAINER
	Mud::instance().addCommand(std::make_shared<Command>(
		DoOrganize, "organize", "(name|weight|price) [(container|room|inventory)]",
		"Order the desired container or if no target is passed, the room.", false, false, false));
	Mud::instance().addCommand(std::make_shared<Command>(
		DoOpen, "open", "(container)|(direction)",
		"Open a door in a given direction or a container.", false, true, false));
	Mud::instance().addCommand(std::make_shared<Command>(
		DoClose, "close", "(container)|(direction)",
		"Close a door in a given direction or a container.", false, true, false));

	// ////////////////////////////////////////////////////////////////////////
	// COMMAND CHARACTER
	Mud::instance().addCommand(std::make_shared<Command>(
		DoEquipments, "equipments", "", "List all the items you are wearing.", false, true, false));
	Mud::instance().addCommand(
		std::make_shared<Command>(DoWield, "wield", "(item)",
								  "Wield a weapon, a shield or maybe a tool.", false, true, false));
	Mud::instance().addCommand(std::make_shared<Command>(
		DoWear, "wear", "(item)", "Puts on a piece of equipment.", false, false, false));
	Mud::instance().addCommand(std::make_shared<Command>(
		DoRemove, "remove", "(item)", "Remove a worn or wielded item.", false, true, false));
	Mud::instance().addCommand(std::make_shared<Command>(
		DoInventory, "inventory", "", "Show character's inventory.", false, true, false));

	// ////////////////////////////////////////////////////////////////////////
	// COMMAND SHOP
	Mud::instance().addCommand(std::make_shared<Command>(DoDeposit, "deposit", "(coin) (shop)",
														 "Deposit a coin inside a shop.", false,
														 false, false));
	Mud::instance().addCommand(std::make_shared<Command>(
		DoSell, "sell", "(item) (shop)", "Sell an item to a shop keeper.", false, false, false));
	Mud::instance().addCommand(std::make_shared<Command>(
		DoBuy, "buy", "(item) (shop)", "Allows to buy an item from a shop.", false, false, false));
	Mud::instance().addCommand(std::make_shared<Command>(
		DoBalance, "balance", "", "Shows the character's balance.", false, false, false));

	// ////////////////////////////////////////////////////////////////////////
	// COMMAND LIGHT SOURCE
	Mud::instance().addCommand(std::make_shared<Command>(
		DoTurn, "turn", "(item)", "Allows to turn on and off an actionable item.", false, true,
		false));
	Mud::instance().addCommand(
		std::make_shared<Command>(DoKindle, "kindle", "(item) (ignition source)",
								  "Allows to kindle a fire.", false, true, false));
	Mud::instance().addCommand(
		std::make_shared<Command>(DoRefill, "refill", "(light source) (fuel)",
								  "Allows to refill a light source.", false, true, false));

	// ////////////////////////////////////////////////////////////////////////
	// COMMAND LIGHT PROCESS
	Mud::instance().addCommand(std::make_shared<Command>(
		DoDismember, "dismember", "(corpse)", "Allows to dismember a corpse.", false, true, false));
}
