/// @file   craftAction.cpp
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

#include "craftAction.hpp"

#include "resourceModel.hpp"
#include "profession.hpp"
#include "production.hpp"
#include "updater.hpp"
#include "logger.hpp"
#include "room.hpp"

#include <cassert>
#include "formatter.hpp"

CraftAction::CraftAction(
	Character *_actor, Production *_production, ItemVector const &_tools,
	std::vector<std::pair<Item *, unsigned int> > const &_ingredients) :
	GeneralAction(_actor),
	production(_production),
	tools(_tools),
	ingredients(_ingredients),
	material()
{
	// Debugging message.
	MudLog(LogLevel::Debug, "Created CraftAction.");
	// Determine the material of the creation based on the ammount of each
	// single involved item.
	this->determineMaterial();
	// Reset the cooldown of the action.
	this->resetCooldown(this->getCooldown());
}

CraftAction::~CraftAction()
{
	MudLog(LogLevel::Debug, "Deleted crafting action.");
}

bool CraftAction::check(std::string &error) const
{
	if (!GeneralAction::check(error))
		return false;
	if (production == nullptr) {
		error = "You don't have a production set.";
		return false;
	}
	if (production->outcome == nullptr) {
		error = "You don't have a production set.";
		return false;
	}
	if (production->profession == nullptr) {
		error = "You don't have a production set.";
		return false;
	}
	if (material == nullptr) {
		error = "You don't know how to craft the item.";
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

ActionType CraftAction::getType() const
{
	return ActionType::Crafting;
}

std::string CraftAction::getDescription() const
{
	return this->production->profession->action;
}

bool CraftAction::start()
{
	std::string error;
	if (!this->check(error)) {
		actor->sendMsg(error + "\n\n");
		return false;
	}
	// Send the messages.
	actor->sendMsg("%s %s.\n", production->profession->startMessage,
				   Formatter::yellow(production->outcome->getName()));
	actor->room->sendToAll("%s has started %s something...\n", { actor },
						   actor->getNameCapital(),
						   production->profession->action);
	return true;
}

std::string CraftAction::stop()
{
	return this->production->profession->interruptMessage + ".";
}

ActionStatus CraftAction::perform()
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
	// Vector which will contain the list of created items and items to destroy.
	ItemVector createdItems;
	// Add the ingredients to the list of items to destroy.
	for (auto it : ingredients) {
		// If the ingredient is completely depleted, destroy it.
		if (it.first->quantity == it.second)
			MudUpdater::instance().addItemToDestroy(it.first);
		else
			it.first->quantity -= it.second;
	}
	// Get the outcome model.
	auto outcomeModel = production->outcome;
	if (HasFlag(outcomeModel->modelFlags, ModelFlag::CanBeStacked)) {
		// Create the item.
		// TODO: FIX WRONG TYPE OF MATERIAL!
		auto newItem =
			outcomeModel->createItem(actor->getName(), material, false,
									 ItemQuality::Normal, production->quantity);
		if (newItem == nullptr) {
			MudLog(LogLevel::Error, "Crafted item is a null pointer.");
			actor->sendMsg("\nYou have failed your action.\n");
			return ActionStatus::Error;
		}
		createdItems.emplace_back(newItem);
	} else {
		for (unsigned int it = 0; it < production->quantity; ++it) {
			// Create the item.
			auto newItem = outcomeModel->createItem(actor->getName(), material);
			if (newItem == nullptr) {
				MudLog(LogLevel::Error, "Crafted item is a null pointer.");
				// Delete all the items created so far.
				for (auto createdItem : createdItems) {
					MudUpdater::instance().addItemToDestroy(createdItem);
				}
				// Notify the character.
				actor->sendMsg("\nYou have failed your action.\n");
				return ActionStatus::Error;
			}
			createdItems.emplace_back(newItem);
		}
	}
	// Flag which will become true if some items are dropped to the ground.
	bool dropped = false;
	// Add the created items to the character's inventory.
	for (auto createdItem : createdItems) {
		if (!actor->canCarry(createdItem, createdItem->quantity)) {
			actor->room->addItem(createdItem);
			dropped = true;
		} else {
			actor->addInventoryItem(createdItem);
		}
	}
	for (auto it : tools) {
		// Update the condition of the involved objects.
		it->triggerDecay();
		if (it->condition < 0)
			actor->sendMsg("%s falls into pieces.", it->getNameCapital(true));
	}
	// Send conclusion message.
	actor->sendMsg("%s %s.\n\n", production->profession->finishMessage,
				   createdItems.back()->getName(true));
	if (dropped) {
		actor->sendMsg("some of the items have been placed on the ground.\n\n");
	}
	return ActionStatus::Finished;
}

unsigned int CraftAction::getCooldown()
{
	assert(actor && "Actor is nullptr");
	assert(production && "Production is nullptr");
	double requiredTime = production->time;
	MudLog(LogLevel::Debug, "Base time  :%s", requiredTime);
	for (auto knowledge : production->requiredKnowledge) {
		requiredTime -=
			(requiredTime * actor->effectManager.getKnowledge(knowledge)) / 100;
	}
	MudLog(LogLevel::Debug, "With skill :%s", requiredTime);
	return static_cast<unsigned int>(requiredTime);
}

void CraftAction::determineMaterial()
{
	/// TODO: Simplify...
	// The material of the outcome is determined by summing the weights of
	// the items with the same material. Then, the material with the highest
	// weight is chosen.
	struct MaterialEntry {
		/// The material.
		Material *material;
		/// The accumulated weight for the material.
		double weight;

		/// @brief Constructor.
		MaterialEntry() : material(), weight()
		{
			// Nothing to do.
		}

		/// @brief Constructor.
		MaterialEntry(Material *_material, double _weight) :
			material(_material),
			weight(_weight)
		{
			// Nothing to do.
		}
	};
	std::vector<MaterialEntry> materials;
	for (auto ingr : ingredients) {
		// Check if the ingredient has a material.
		if (ingr.first->composition == nullptr) {
			continue;
		}
		// Create a new material entry.
		MaterialEntry entry(ingr.first->composition,
							ingr.first->getWeight(false) * ingr.second);
		bool found = false;
		for (auto it : materials) {
			if (it.material->vnum == entry.material->vnum) {
				found = true;
				it.weight += entry.weight;
				break;
			}
		}
		if (!found) {
			materials.emplace_back(entry);
		}
	}
	double weight = 0;
	for (auto it : materials) {
		if (it.weight > weight) {
			material = it.material;
			weight = it.weight;
		}
	}
}

unsigned int CraftAction::getConsumedStamina(Character *character)
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
