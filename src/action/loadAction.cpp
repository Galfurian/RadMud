/// @file   loadAction.cpp
/// @brief  Implementation of the class for loading activities.
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

#include "action/loadAction.hpp"

#include "model/submodel/magazineModel.hpp"
#include "database/sqliteDbms.hpp"
#include "character/character.hpp"
#include "structure/room.hpp"

#include <cassert>

LoadAction::LoadAction(Character *_actor, MagazineItem *_magazine, Item *_projectile,
					   const unsigned int &_amount) :
	GeneralAction(_actor), magazine(_magazine), projectile(_projectile), amount(_amount)
{
	// Debugging message.
	MudLog(LogLevel::Debug, "Created LoadAction.");
	// Reset the cooldown of the action.
	this->resetCooldown(this->getCooldown());
}

LoadAction::~LoadAction()
{
	MudLog(LogLevel::Debug, "Deleted load action.");
}

bool LoadAction::check(std::string &error) const
{
	if (!GeneralAction::check(error)) {
		return false;
	}
	if (magazine == nullptr) {
		MudLog(LogLevel::Error, "The magazine is a null pointer.");
		error = "You don't have a valid magazine to load.";
		return false;
	}
	if (projectile == nullptr) {
		MudLog(LogLevel::Error, "The projectile is a null pointer.");
		error = "You don't have valid projectiles to load.";
		return false;
	}
	auto loadedProjectile = magazine->getAlreadyLoadedProjectile();
	if (loadedProjectile != nullptr) {
		// If there are projectiles inside, check if the two types of
		//  projectiles are compatible.
		if (!projectile->canStackWith(loadedProjectile)) {
			error = "You cannot stack the item with the one inside.";
			return false;
		}
		if (magazine->model->to<MagazineModel>()->maxAmount <= loadedProjectile->quantity) {
			error = "The item is already at full capacity.";
			return false;
		}
	}
	return true;
}

ActionType LoadAction::getType() const
{
	return ActionType::Load;
}

std::string LoadAction::getDescription() const
{
	return "loading";
}

bool LoadAction::start()
{
	std::string error;
	if (!this->check(error)) {
		actor->sendMsg(error + "\n\n");
		return false;
	}
	// Send the starting message.
	actor->sendMsg("You start loading %s with %s.\n", magazine->getName(true),
				   projectile->getName(true));
	actor->room->sendToAll("%s starts loading %s with %s...\n", { actor }, actor->getNameCapital(),
						   magazine->getName(true), projectile->getName(true));
	return true;
}

std::string LoadAction::stop()
{
	return "You stop loading " + magazine->getName(true) + ".";
}

ActionStatus LoadAction::perform()
{
	std::string error;
	if (!this->check(error)) {
		actor->sendMsg(error + "\n\n");
		return ActionStatus::Error;
	}
	// First check if there are already some projectiles inside the magazine.
	auto loadedProjectile = magazine->getAlreadyLoadedProjectile();
	if (loadedProjectile != nullptr) {
		SQLiteDbms::instance().beginTransaction();
		if (projectile->quantity < amount) {
			magazine->putInside(projectile);
		} else {
			loadedProjectile->quantity += amount;
			projectile->quantity -= amount;
			loadedProjectile->updateOnDB();
			projectile->updateOnDB();
		}
		SQLiteDbms::instance().endTransaction();
	} else {
		SQLiteDbms::instance().beginTransaction();
		if (projectile->quantity <= amount) {
			actor->remInventoryItem(projectile);
			magazine->putInside(projectile);
		} else {
			auto newProjectileStack = projectile->removeFromStack(actor, amount);
			if (newProjectileStack == nullptr) {
				// Rollback the transaction.
				SQLiteDbms::instance().rollbackTransection();
				actor->sendMsg("Something is gone wrong while you were loading %s.\n\n",
							   magazine->getName(true));
				return ActionStatus::Error;
			}
			magazine->putInside(newProjectileStack);
		}
		SQLiteDbms::instance().endTransaction();
	}
	actor->sendMsg("You have finished loading %s with %s...\n\n", magazine->getName(true),
				   projectile->getName(true));
	return ActionStatus::Finished;
}

unsigned int LoadAction::getCooldown()
{
	assert(actor && "Actor is nullptr");
	assert(projectile && "Projectile is nullptr");
	return static_cast<unsigned int>(projectile->getWeight(false) * amount);
}
