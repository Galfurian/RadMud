/// @file   aimAction.cpp
/// @brief  Implementation of the class for aiming a target.
/// @author Enrico Fraccaroli
/// @date   Oct 13 2016
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

#include "aimAction.hpp"

#include "character.hpp"
#include "characterUtilities.hpp"
#include "area.hpp"
#include "room.hpp"
#include <cassert>
#include "structureUtils.hpp"

AimAction::AimAction(Character *_actor, Character *_target) :
	GeneralAction(_actor),
	target(_target)
{
	// Debugging message.
	Logger::log(LogLevel::Debug, "Created aim action.");
	// Reset the cooldown of the action.
	this->resetCooldown(this->getCooldown());
}

AimAction::~AimAction()
{
	Logger::log(LogLevel::Debug, "Deleted aim action.");
}

bool AimAction::check(std::string &error) const
{
	if (!GeneralAction::check(error)) {
		Logger::log(LogLevel::Error, "Error with the general action.");
		error = "You failed your action.";
		return false;
	}
	if (target == nullptr) {
		Logger::log(LogLevel::Error, "The target is a null pointer.");
		error = "You don't have a valid target.";
		return false;
	}
	if (!actor->isAtRange(target, actor->getViewDistance())) {
		error = target->getNameCapital() + " is out of sight.";
		return false;
	}
	return true;
}

ActionType AimAction::getType() const
{
	return ActionType::Aim;
}

std::string AimAction::getDescription() const
{
	return "aiming";
}

bool AimAction::start()
{
	std::string error;
	if (!this->check(error)) {
		actor->sendMsg(error + "\n\n");
		return false;
	}
	// Send the starting message.
	actor->sendMsg("You start aiming at %s...\n", target->getName());
	actor->room->sendToAll("%s starts aiming...\n", { actor },
						   actor->getNameCapital());
	return true;
}

std::string AimAction::stop()
{
	if (target != nullptr)
		return "You stop aiming at " + target->getName() + ".";
	return "You stop aiming.";
}

ActionStatus AimAction::perform()
{
	std::string error;
	if (!this->check(error)) {
		actor->sendMsg(error + "\n\n");
		return ActionStatus::Error;
	}
	if (GetActiveWeapons<RangedWeaponItem>(actor).empty()) {
		actor->sendMsg("You don't have a ranged weapon equipped.\n\n");
		return ActionStatus::Error;
	}
	// Send the message.
	actor->sendMsg("You are now aiming at %s...\n\n", target->getName());
	// Set the aimed character.
	actor->combatHandler.setAimedTarget(target);
	return ActionStatus::Finished;
}

unsigned int AimAction::getCooldown()
{
	assert(actor && "Actor is nullptr");
	assert(target && "Projectile is nullptr");
	unsigned int requiredTime = 2;
	if ((actor != nullptr) && (target != nullptr)) {
		if ((actor->room != nullptr) && (target->room != nullptr)) {
			requiredTime = SafeSum(requiredTime,
								   StructUtils::getRoomDistance(actor->room,
																target->room));
		}
	}
	return requiredTime;
}
