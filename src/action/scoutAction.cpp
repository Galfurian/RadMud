/// @file   scoutAction.cpp
/// @brief  Implementation of the class for scouting an area.
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

#include "scoutAction.hpp"

#include "effectFactory.hpp"
#include "room.hpp"
#include "area.hpp"
#include <cassert>

ScoutAction::ScoutAction(Character *_actor) : GeneralAction(_actor)
{
	// Debugging message.
	MudLog(LogLevel::Debug, "Created ScoutAction.");
	// Reset the cooldown of the action.
	this->resetCooldown(this->getCooldown());
}

ScoutAction::~ScoutAction()
{
	MudLog(LogLevel::Debug, "Deleted scout action.");
}

bool ScoutAction::check(std::string &error) const
{
	if (!GeneralAction::check(error)) {
		return false;
	}
	if (actor->room == nullptr) {
		MudLog(LogLevel::Error, "The actor's room is a nullptr.");
		return false;
	}
	if (actor->room->area == nullptr) {
		MudLog(LogLevel::Error, "The room's area is a nullptr.");
		return false;
	}
	if (this->getConsumedStamina(actor) > actor->stamina) {
		error = "You are too tired to scout the area.";
		return false;
	}
	return true;
}

ActionType ScoutAction::getType() const
{
	return ActionType::Scout;
}

std::string ScoutAction::getDescription() const
{
	return "scouting";
}

bool ScoutAction::start()
{
	std::string error;
	if (!this->check(error)) {
		actor->sendMsg(error + "\n\n");
		return false;
	}
	// Send the starting message.
	actor->sendMsg("You start scouting the area...\n");
	actor->room->sendToAll("%s starts scouting the area...\n", { actor },
						   actor->getNameCapital());
	return true;
}

std::string ScoutAction::stop()
{
	return "You stop scouting the area.";
}

ActionStatus ScoutAction::perform()
{
	std::string error;
	if (!this->check(error)) {
		actor->sendMsg(error + "\n\n");
		return ActionStatus::Error;
	}
	// Get the amount of required stamina and try to consume it.
	actor->remStamina(this->getConsumedStamina(actor));
	// Update the list of characters in sight.
	actor->combatHandler.updateCharactersInSight();
	// Check if there are characters in sight.
	if (actor->combatHandler.charactersInSight.empty()) {
		actor->sendMsg("You have found nothing...\n");
		return ActionStatus::Error;
	}
	actor->sendMsg("Nearby you can see...\n");
	for (auto it : actor->combatHandler.charactersInSight) {
		actor->sendMsg("    %s\n", it->getName());
	}
	actor->sendMsg("\n");
	// Add the effect.
	unsigned int modifier = actor->getAbilityModifier(Ability::Perception);
	actor->effectManager.addEffect(
		EffectFactory::clearTargets(actor, 2 + modifier), true);
	return ActionStatus::Finished;
}

unsigned int ScoutAction::getCooldown()
{
	assert(actor && "Actor is nullptr");
	// BASE       [+3.0]
	// PERCEPTION [-0.0 to -2.80]
	unsigned int requiredTime = 3;
	requiredTime =
		SafeSum(requiredTime, -actor->getAbilityLog(Ability::Perception));
	return requiredTime;
}

unsigned int ScoutAction::getConsumedStamina(Character *character)
{
	// BASE     [+1.0]
	// STRENGTH [-0.0 to -2.80]
	// WEIGHT   [+1.6 to +2.51]
	// CARRIED  [+0.0 to +2.48]
	unsigned int consumedStamina = 1;
	consumedStamina =
		SafeSum(consumedStamina, -character->getAbilityLog(Ability::Strength));
	consumedStamina = SafeSum(consumedStamina, SafeLog10(character->weight));
	consumedStamina =
		SafeSum(consumedStamina, SafeLog10(character->getCarryingWeight()));
	return consumedStamina;
}
