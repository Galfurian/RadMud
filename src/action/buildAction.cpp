/// @file   buildAction.cpp
/// @brief  Implementation of the class for a build action.
/// @author Enrico Fraccaroli
/// @date   Jul 14 2016
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

#include "buildAction.hpp"

#include "resourceModel.hpp"
#include "formatter.hpp"
#include "building.hpp"
#include "updater.hpp"
#include "logger.hpp"
#include "room.hpp"
#include <cassert>

BuildAction::BuildAction(
	Character *_actor, const std::shared_ptr<Building> &_schematics,
	Item *_building, ItemVector const &_tools,
	std::vector<std::pair<Item *, unsigned int> > const &_ingredients) :
	GeneralAction(_actor),
	schematics(_schematics),
	building(_building),
	tools(_tools),
	ingredients(_ingredients)
{
	// Debugging message.
	Logger::log(LogLevel::Debug, "Created BuildAction.");
	// Reset the cooldown of the action.
	this->resetCooldown(this->getCooldown());
}

BuildAction::~BuildAction()
{
	Logger::log(LogLevel::Debug, "Deleted building action.");
}

bool BuildAction::check(std::string &error) const
{
	if (!GeneralAction::check(error))
		return false;
	if (building == nullptr) {
		error = "You don't have a building set.";
		return false;
	}
	if (schematics == nullptr) {
		error = "You don't have a valid schematics set.";
		return false;
	}
	for (auto const &it : ingredients) {
		// Check if the ingredient has been deleted.
		if (it.first == nullptr) {
			error = "One of your ingredient is missing.";
			return false;
		}
	}
	for (auto const &it : tools) {
		// Check if the tool has been deleted.
		if (it == nullptr) {
			error = "You are missing one of the tools.";
			return false;
		}
	}
	// Check if the actor has enough stamina to execute the action.
	if (getConsumedStamina(actor) > actor->stamina) {
		error = "You are too tired right now.";
		return false;
	}
	// Add the ingredients to the list of items to destroy.
	for (auto const &it : ingredients) {
		if (it.first == nullptr) {
			error = "You don't have one of the ingredients anymore.";
			return false;
		}
		if (it.first->quantity < it.second) {
			error = "You don't have enough " + it.first->getName() + ".";
			return false;
		}
	}
	return true;
}

ActionType BuildAction::getType() const
{
	return ActionType::Building;
}

std::string BuildAction::getDescription() const
{
	return "building";
}

bool BuildAction::start()
{
	std::string error;
	if (!this->check(error)) {
		actor->sendMsg(error + "\n\n");
		return false;
	}
	// Send the messages.
	actor->sendMsg("You start building %s.\n",
				   Formatter::yellow(schematics->buildingModel->getName()));
	// Send the message inside the room.
	actor->room->sendToAll("%s has started building something...\n", { actor },
						   actor->getNameCapital());
	return true;
}

std::string BuildAction::stop()
{
	return "You stop building.";
}

ActionStatus BuildAction::perform()
{
	// Check the values of the action.
	std::string error;
	if (!this->check(error)) {
		actor->sendMsg(error + "\n\n");
		return ActionStatus::Error;
	}
	// Get the amount of required stamina and consume it.
	auto consumedStamina = this->getConsumedStamina(actor);
	actor->remStamina(consumedStamina, true);
	actor->remInventoryItem(building);
	actor->room->addBuilding(building);
	// Add the ingredients to the list of items to destroy.
	for (auto it : ingredients) {
		auto ingredient = it.first;
		if (ingredient->quantity == it.second) {
			// Add the ingredient to the list of items that has to be deleted.
			MudUpdater::instance().addItemToDestroy(ingredient);
		} else {
			ingredient->quantity -= it.second;
		}
	}
	for (auto it : tools) {
		// Update the condition of the involved objects.
		it->triggerDecay();
		if (it->condition < 0) {
			actor->sendMsg(it->getName(true) + " falls into pieces.");
		}
	}
	// Send conclusion message.
	actor->sendMsg("You have finished building %s.\n\n",
				   Formatter::yellow() + schematics->buildingModel->getName() +
					   Formatter::reset());
	return ActionStatus::Finished;
}

unsigned int BuildAction::getCooldown()
{
	assert(actor && "Actor is nullptr");
	assert(schematics && "Schematics is nullptr");
	double requiredTime = schematics->time;
	Logger::log(LogLevel::Debug, "Base time  :%s", requiredTime);
	for (auto knowledge : schematics->requiredKnowledge) {
		requiredTime -=
			(requiredTime * actor->effectManager.getKnowledge(knowledge)) / 100;
	}
	Logger::log(LogLevel::Debug, "With skill :%s", requiredTime);
	return static_cast<unsigned int>(requiredTime);
}

unsigned int BuildAction::getConsumedStamina(Character *character)
{
	// BASE     [+1.0]
	// STRENGTH [-0.0 to -1.40]
	// WEIGHT   [+1.6 to +2.51]
	// CARRIED  [+0.0 to +2.48]
	unsigned int consumedStamina = 1;
	consumedStamina -= character->getAbilityLog(Ability::Strength);
	consumedStamina = SafeSum(consumedStamina, SafeLog10(character->weight));
	consumedStamina =
		SafeSum(consumedStamina, SafeLog10(character->getCarryingWeight()));
	return consumedStamina;
}
