/// @file   unloadAction.cpp
/// @brief  Implementation of the class for unloading activities.
/// @author Enrico Fraccaroli
/// @date   Oct 10 2016
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

#include "action/unloadAction.hpp"

#include "database/sqliteDbms.hpp"
#include "character/character.hpp"
#include "structure/room.hpp"

#include <cassert>

UnloadAction::UnloadAction(Character *_actor, Item *_item) :
	GeneralAction(_actor),
	item(_item)
{
	// Debugging message.
	MudLog(LogLevel::Debug, "Created UnloadAction.");
	// Reset the cooldown of the action.
	this->resetCooldown(this->getCooldown());
}

UnloadAction::~UnloadAction()
{
	MudLog(LogLevel::Debug, "Deleted unload action.");
}

bool UnloadAction::check(std::string &error) const
{
	if (!GeneralAction::check(error)) {
		return false;
	}
	if (item == nullptr) {
		error = "The item you want to unload is missing.";
		return false;
	}
	if (item->getType() == ModelType::Magazine) {
		// Cast the item to magazine.
		auto magazine = static_cast<MagazineItem *>(item);
		// Get the loaded projectile.
		if (magazine->getAlreadyLoadedProjectile() == nullptr) {
			error = item->getNameCapital(true) + " is already empty.";
			return false;
		}
	}
	if (item->getType() == ModelType::RangedWeapon) {
		// Cast the item to ranged weapon.
		auto magazine = static_cast<RangedWeaponItem *>(item);
		// Get the loaded magazine.
		if (magazine->getAlreadyLoadedMagazine() == nullptr) {
			error = item->getNameCapital(true) + " is already empty.";
			return false;
		}
	}
	return true;
}

ActionType UnloadAction::getType() const
{
	return ActionType::Unload;
}

std::string UnloadAction::getDescription() const
{
	return "unloading";
}

bool UnloadAction::start()
{
	std::string error;
	if (!this->check(error)) {
		actor->sendMsg(error + "\n\n");
		return false;
	}
	// Send the starting message.
	actor->sendMsg("You start unloading %s.\n", item->getName(true));
	actor->room->sendToAll("%s starts unloading %s...\n", { actor },
						   actor->getNameCapital(),
						   item->getName(true));
	return true;
}

std::string UnloadAction::stop()
{
	if (item != nullptr) {
		return "You stop unloading " + item->getName(true) + ".";
	}
	return "You stop unloading.";
}

ActionStatus UnloadAction::perform()
{
	std::string error;
	if (!this->check(error)) {
		actor->sendMsg(error + "\n\n");
		return ActionStatus::Error;
	}
	// Prepare a variable for the loaded item.
	Item *loadedItem = nullptr;
	if (item->getType() == ModelType::Magazine) {
		// Cast the item to magazine.
		auto magazine = static_cast<MagazineItem *>(item);
		// Get the loaded projectile.
		loadedItem = magazine->getAlreadyLoadedProjectile();
	}
	if (item->getType() == ModelType::RangedWeapon) {
		// Cast the item to ranged weapon.
		auto magazine = static_cast<RangedWeaponItem *>(item);
		// Get the loaded magazine.
		loadedItem = magazine->getAlreadyLoadedMagazine();
	}
	// Start a transaction.
	SQLiteDbms::instance().beginTransaction();
	// Take out the item.
	item->takeOut(loadedItem);
	// Add the item to the inventory of the actor.
	actor->addInventoryItem(loadedItem);
	// End the transaction.
	SQLiteDbms::instance().endTransaction();
	// Show a message.
	actor->sendMsg("You finish unloading %s.\n\n", item->getName(true));
	return ActionStatus::Finished;
}

unsigned int UnloadAction::getCooldown()
{
	assert(actor && "Actor is nullptr");
	assert(item && "Item is nullptr");
	if (item->getType() == ModelType::Magazine) {
		// Transform the item to magazine.
		auto magazine = static_cast<MagazineItem *>(item);
		// Get the loaded projectile.
		auto loadedProjectile = magazine->getAlreadyLoadedProjectile();
		if (loadedProjectile != nullptr) {
			return static_cast<unsigned int>(
				loadedProjectile->getWeight(false) *
				loadedProjectile->quantity);
		}
	}
	return 1;
}
