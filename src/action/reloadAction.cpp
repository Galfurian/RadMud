/// @file   reloadAction.cpp
/// @brief  Implementation of the class for reloading activities.
/// @author Enrico Fraccaroli
/// @date   Oct 9 2016
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

#include "reloadAction.hpp"

#include "sqliteDbms.hpp"
#include "character.hpp"
#include <cassert>

ReloadAction::ReloadAction(Character *_actor, RangedWeaponItem *_weapon,
						   Item *_magazine) :
	GeneralAction(_actor),
	weapon(_weapon),
	magazine(_magazine)
{
	// Debugging message.
	Logger::log(LogLevel::Debug, "Created ReloadAction.");
	// Reset the cooldown of the action.
	this->resetCooldown(this->getCooldown());
}

ReloadAction::~ReloadAction()
{
	Logger::log(LogLevel::Debug, "Deleted reload action.");
}

bool ReloadAction::check(std::string &error) const
{
	if (!GeneralAction::check(error)) {
		return false;
	}
	if (weapon == nullptr) {
		Logger::log(LogLevel::Error, "The weapon is a null pointer.");
		error = "The weapon you want to reaload is missing.";
		return false;
	}
	if (magazine == nullptr) {
		Logger::log(LogLevel::Error, "The magazine is a null pointer.");
		error = "The magazine you want to use is missing.";
		return false;
	}
	if (!weapon->isEmpty()) {
		Logger::log(LogLevel::Error, "The weapon already contains a magazine.");
		error = "The weapon already contains a magazine.";
		return false;
	}
	return true;
}

ActionType ReloadAction::getType() const
{
	return ActionType::Reload;
}

std::string ReloadAction::getDescription() const
{
	return "reloading";
}

std::string ReloadAction::stop()
{
	if ((weapon != nullptr) && (magazine != nullptr)) {
		return "You stop reloading " + weapon->getName(true) + " with " +
			   magazine->getName(true) + ".";
	}
	return "You stop reloading.";
}

ActionStatus ReloadAction::perform()
{
	std::string error;
	if (!this->check(error)) {
		actor->sendMsg(error + "\n\n");
		return ActionStatus::Error;
	}
	SQLiteDbms::instance().beginTransaction();
	if (!actor->remEquipmentItem(magazine) &&
		!actor->remInventoryItem(magazine)) {
		// Rollback the transaction.
		SQLiteDbms::instance().rollbackTransection();
		actor->sendMsg(
			"Something is gone wrong while you were reloading %s...\n\n",
			weapon->getName(true));
		return ActionStatus::Error;
	}
	weapon->putInside(magazine);
	SQLiteDbms::instance().endTransaction();
	actor->sendMsg("You have finished reloading %s...\n\n",
				   this->weapon->getName(true));
	return ActionStatus::Finished;
}

unsigned int ReloadAction::getCooldown()
{
	assert(actor && "Actor is nullptr");
	assert(weapon && "Weapon is nullptr");
	assert(magazine && "Magazine is nullptr");
	return static_cast<unsigned int>(1 + SafeLog10(weapon->getWeight(false)) +
									 SafeLog10(magazine->getWeight(false)));
}
