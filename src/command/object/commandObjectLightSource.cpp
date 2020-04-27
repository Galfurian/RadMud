/// @file   commandObjectLightSource.cpp
/// @author Enrico Fraccaroli
/// @date   Jan 11 2017
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

#include "command/object/commandObjectLightSource.hpp"
#include "character/characterUtilities.hpp"
#include "database/sqliteDbms.hpp"
#include "model/submodel/lightModel.hpp"
#include "model/submodel/toolModel.hpp"
#include "item/subitem/lightItem.hpp"
#include "command/command.hpp"
#include "structure/room.hpp"

bool DoTurn(Character *character, ArgumentHandler &args)
{
	// Check if the character is sleeping.
	if (character->posture == CharacterPosture::Sleep) {
		character->sendMsg("Not while you're sleeping.\n");
		return false;
	}
	// Stop any action the character is executing.
	StopAction(character);
	// Check the number of arguments.
	if (args.size() != 1) {
		character->sendMsg("What do you want to turn on/off?\n");
		return false;
	}
	auto searchOptions = SearchOptionsCharacter::allOptions();
	auto item = FindNearbyItem(character, args[0].getContent(),
							   args[0].getIndex(), nullptr, searchOptions);
	if (item == nullptr) {
		character->sendMsg("You don't see '%s' anywhere.\n",
						   args[0].getContent());
		return false;
	}
	if (item->getType() != ModelType::Light) {
		character->sendMsg("%s is not a light source.\n",
						   item->getNameCapital(true));
		return false;
	}
	// Cast the item to light.
	auto lightItem = static_cast<LightItem *>(item);
	// Get the model.
	auto lightModel = lightItem->model->to<LightModel>();
	if (HasFlag(lightModel->lightSourceFlags, LightModelFlags::AlwaysActive)) {
		character->sendMsg("You cannot turn on/off %s.\n", item->getName(true));
		return false;
	}
	if (lightItem->active) {
		character->sendMsg("You turn off %s.\n", item->getName(true));
		lightItem->active = false;
		return true;
	}
	// Check if the item can be simply turned on and off.
	if (HasFlag(lightModel->lightSourceFlags, LightModelFlags::NeedToKindle)) {
		character->sendMsg("You cannot simply turn on %s.\n",
						   item->getName(true));
		return false;
	}
	if (lightItem->getAutonomy() > 0) {
		character->sendMsg("You turn on %s.\n", item->getName(true));
		lightItem->active = true;
	} else {
		character->sendMsg("You cannot turn on %s.\n", item->getName(true));
		return false;
	}
	return true;
}

bool DoKindle(Character *character, ArgumentHandler &args)
{
	// Check if the character is sleeping.
	if (character->posture == CharacterPosture::Sleep) {
		character->sendMsg("Not while you're sleeping.\n");
		return false;
	}
	// Stop any action the character is executing.
	StopAction(character);
	// Check the number of arguments.
	if (args.size() != 2) {
		character->sendMsg("What do you want to kindle and with what?\n");
		return false;
	}
	// Prepare a variable for the light source.
	Item *lightSource = nullptr;
	// Prepare a variable for the ignition source.
	Item *ignitionSource = nullptr;
	// Check if the room is lit.
	bool roomIsLit = character->room->isLit();
	// Check if the inventory is lit.
	auto inventoryIsLit = character->inventoryIsLit();
	// If the room is lit.
	if (roomIsLit) {
		lightSource =
			character->findNearbyItem(args[0].getContent(), args[0].getIndex());
		ignitionSource =
			character->findNearbyItem(args[1].getContent(), args[1].getIndex());
	} else if (inventoryIsLit) {
		// If the room is not lit but the inventory is.
		lightSource = character->findInventoryItem(args[0].getContent(),
												   args[0].getIndex());
		ignitionSource = character->findInventoryItem(args[1].getContent(),
													  args[1].getIndex());
	} else if (!character->inventory.empty()) {
		// If its pitch black, get random items from the inventory.
		auto inventorySize(character->inventory.size() - 1);
		lightSource = character->inventory.at(TRand<size_t>(0, inventorySize));
		ignitionSource =
			character->inventory.at(TRand<size_t>(0, inventorySize));
	}
	if (lightSource == nullptr) {
		character->sendMsg("You don't see '%s' anywhere.\n",
						   args[0].getContent());
		return false;
	}
	if (lightSource->getType() != ModelType::Light) {
		character->sendMsg("%s is not a light source.\n",
						   lightSource->getNameCapital(true));
		return false;
	}
	if (ignitionSource == nullptr) {
		character->sendMsg("You don't see '%s' anywhere.\n",
						   args[1].getContent());
		return false;
	}
	bool canUseToIgnite = false;
	// Check if the ignition source is a firelighter or an active light source.
	if (ignitionSource->getType() == ModelType::Tool) {
		auto toolModel = ignitionSource->model->to<ToolModel>();
		if (toolModel->toolType == ToolType::Firelighter) {
			canUseToIgnite = true;
		}
	} else if (ignitionSource->getType() == ModelType::Light) {
		// Cast the ignition source to light.
		auto lightItem = static_cast<LightItem *>(ignitionSource);
		// Get the model of the ignition source.
		auto lightModel = lightItem->model->to<LightModel>();
		// Check if the ignition source is active AND is a light source which
		// is activated through manual kindling.
		if (lightItem->isActive() && HasFlag(lightModel->lightSourceFlags,
											 LightModelFlags::NeedToKindle)) {
			canUseToIgnite = true;
		}
	}
	// Stop if it cannot be used to kindle a fire.
	if (!canUseToIgnite) {
		character->sendMsg("%s cannot be used to kindle a fire.\n",
						   ignitionSource->getNameCapital(true));
		return false;
	}
	// Cast the item to light.
	auto lightItem = static_cast<LightItem *>(lightSource);
	// Get the model.
	auto lightModel = lightItem->model->to<LightModel>();
	// Check if the item can be simply turned on and off.
	if (!HasFlag(lightModel->lightSourceFlags, LightModelFlags::NeedToKindle) ||
		HasFlag(lightModel->lightSourceFlags, LightModelFlags::AlwaysActive)) {
		character->sendMsg("%s cannot be kindled.\n",
						   lightItem->getNameCapital(true));
		return false;
	}
	if (lightItem->active) {
		character->sendMsg("%s is already kindled.\n",
						   lightItem->getNameCapital(true));
		return false;
	} else {
		if (lightItem->getAutonomy() > 0) {
			character->sendMsg("You kindled %s using %s.\n",
							   lightItem->getName(true),
							   ignitionSource->getName(true));
			lightItem->active = true;
		} else {
			character->sendMsg("%s cannot be kindled.\n",
							   lightItem->getNameCapital(true));
			return false;
		}
	}
	return true;
}

bool DoRefill(Character *character, ArgumentHandler &args)
{
	// Check if the character is sleeping.
	if (character->posture == CharacterPosture::Sleep) {
		character->sendMsg("Not while you're sleeping.\n");
		return false;
	}
	// Stop any action the character is executing.
	StopAction(character);
	// Check the number of arguments.
	if (args.size() != 2) {
		character->sendMsg("What do you want to refill with what?\n");
		return false;
	}
	// Prepare a variable for the light source.
	Item *item = nullptr, *fuel = nullptr;
	// Check if the room is lit.
	bool roomIsLit = character->room->isLit();
	// Check if the inventory is lit.
	auto inventoryIsLit = character->inventoryIsLit();
	// If the room is lit.
	if (roomIsLit) {
		item =
			character->findNearbyItem(args[0].getContent(), args[0].getIndex());
		fuel =
			character->findNearbyItem(args[1].getContent(), args[1].getIndex());
	} else if (inventoryIsLit) {
		// If the room is not lit but the inventory is.
		item = character->findInventoryItem(args[0].getContent(),
											args[0].getIndex());
		fuel = character->findInventoryItem(args[1].getContent(),
											args[1].getIndex());
	} else {
		character->sendMsg("You can't do that without seeing,"
						   "you could waste most of the fuel.\n");
		return false;
	}
	// Check the light source.
	if (item == nullptr) {
		character->sendMsg("You don't have '%s'.\n", args[0].getContent());
		return false;
	}
	if (item->getType() != ModelType::Light) {
		character->sendMsg("%s is not a light source.\n", args[0].getContent());
		return false;
	}
	if (fuel == nullptr) {
		character->sendMsg("You don't have '%s'.\n", args[1].getContent());
		return false;
	}
	// Cast the item to light.
	auto lightSource = static_cast<LightItem *>(item);
	// Get the amount of fuel required to refill the light source.
	std::string error;
	unsigned int amountToLoad = 0;
	if (!lightSource->getAmountToRefill(fuel, amountToLoad, error)) {
		character->sendMsg(error + "\n");
		return false;
	}
	// Start a transaction.
	SQLiteDbms::instance().beginTransaction();
	if (fuel->quantity <= amountToLoad) {
		// Remove the item from the player's inventory.
		character->remInventoryItem(fuel);
		// Put the item inside the container.
		lightSource->putInside(fuel);
		// Send the messages.
		character->sendMsg("You refill %s with %s.\n",
						   lightSource->getName(true), fuel->getName(true));
		character->room->sendToAll("%s refills %s with %s.\n", { character },
								   character->getNameCapital(),
								   lightSource->getName(true),
								   fuel->getName(true));
	} else {
		// Remove from the stack.
		auto newStack = fuel->removeFromStack(character, amountToLoad);
		if (newStack == nullptr) {
			character->sendMsg("You failed to refill %s with part of %s.\n",
							   lightSource->getName(true), fuel->getName(true));
			// Rollback the transaction.
			SQLiteDbms::instance().rollbackTransection();
			return false;
		}
		// Put the stack inside the container.
		lightSource->putInside(newStack);
		// Send the messages.
		character->sendMsg("You put refill %s with part of %s.\n",
						   lightSource->getName(true), fuel->getName(true));
		character->room->sendToAll("%s refills %s with part of %s.\n",
								   { character }, character->getNameCapital(),
								   lightSource->getName(true),
								   fuel->getName(true));
	}
	// Conclude the transaction.
	SQLiteDbms::instance().endTransaction();
	return true;
}
