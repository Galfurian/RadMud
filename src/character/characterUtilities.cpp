/// @file   characterUtilities.cpp
/// @author Enrico Fraccaroli
/// @date   Jan 29 2017
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

#include "character/characterUtilities.hpp"
#include "model/submodel/resourceModel.hpp"
#include "creation/production.hpp"
#include "model/submodel/toolModel.hpp"
#include "structure/room.hpp"

std::vector<std::shared_ptr<BodyPart::BodyWeapon> >
GetActiveNaturalWeapons(Character *character)
{
	std::vector<std::shared_ptr<BodyPart::BodyWeapon> > naturalWeapons;
	for (auto const &bodyPart : character->race->bodyParts) {
		// If the body part is holding an item, skip it.
		if (character->findItemAtBodyPart(bodyPart) != nullptr)
			continue;
		// If the body part cannot be used as natural weapon, skip it.
		if (bodyPart->weapon == nullptr)
			continue;
		// Add the natural weapon to the vector.
		naturalWeapons.emplace_back(bodyPart->weapon);
	}
	return naturalWeapons;
}

void FindCoinsInContainer(ItemVector const &container, ItemVector &foundCoins,
						  const bool iterative)
{
	// Analyze the content.
	for (auto item : container) {
		if (item->getType() == ModelType::Currency) {
			foundCoins.emplace_back(item);
		} else if (iterative) {
			if (item->getType() == ModelType::Container) {
				// Cast the item to container.
				auto containerItem = dynamic_cast<ContainerItem *>(item);
				// Iterate inside the container's content.
				FindCoinsInContainer(containerItem->content, foundCoins,
									 iterative);
			}
		}
	}
}

ItemVector FindPosessedCoins(Character *character)
{
	ItemVector foundCoins;
	FindCoinsInContainer(character->equipment, foundCoins, false);
	FindCoinsInContainer(character->inventory, foundCoins, false);
	foundCoins.orderBy(ItemVector::ByPrice);
	return foundCoins;
}

Item *FindNearbyItem(Character *character, std::string const &key, unsigned int &number,
					 const SearchOptionsCharacter &searchOptions)
{
	// Check the lights.
	bool roomLit = true;
	bool inventoryLit = true;
	if (searchOptions.checkLightLevels) {
		roomLit = character->room->isLit();
		inventoryLit = character->inventoryIsLit();
	}
	// Get the item.
	Item *item = nullptr;
	if (searchOptions.searchInRoom && (character->room != nullptr) && roomLit) {
		item = character->room->findItem(key, number);
	}
	if (searchOptions.searchInEquipment && (item == nullptr)) {
		if (roomLit || inventoryLit) {
			item = ItemUtils::FindItemIn(character->equipment, key, number);
		} else if (searchOptions.randomIfNoLight &&
				   (!character->equipment.empty())) {
			auto it = TRand<size_t>(0, character->equipment.size() - 1);
			item = character->equipment[it];
		}
	}
	if (searchOptions.searchInInventory && (item == nullptr)) {
		if (roomLit || inventoryLit) {
			item = ItemUtils::FindItemIn(character->inventory, key, number);
		} else if (searchOptions.randomIfNoLight &&
				   (!character->inventory.empty())) {
			auto it = TRand<size_t>(0, character->inventory.size() - 1);
			item = character->inventory[it];
		}
	}
	return item;
}

bool FindNearbyResouces(
	Character *character,
	std::map<ResourceType, unsigned int> const &requiredResources,
	std::vector<std::pair<Item *, unsigned int> > &foundResources,
	const SearchOptionsCharacter &searchOptions, ResourceType &missing)
{
	// Create a function which reduces the required quantity and checks if
	// the zero has been reached.
	auto DecrementRequired = [&](Item *item, unsigned int &required) {
		// Set the used quantity.
		auto used = std::min(item->quantity, required);
		// Add the item to the list of used resources.
		foundResources.emplace_back(std::make_pair(item, used));
		// Reduce quantity and check if we've reached the needed quantity.
		return ((required -= used) == 0);
	};
	for (auto const &resource : requiredResources) {
		// Quantity of ingredients that has to be found.
		auto required = resource.second;
		// Check if we need to search inside the room.
		// TODO: Probably I need to check if a given item is not selected twice.
		if (searchOptions.searchInRoom && (required > 0)) {
			for (auto const &item : character->room->items) {
				// Check if the item is a valid resource.
				if (!ItemUtils::IsValidResource(item, resource.first))
					continue;
				// Decrement the required quantity. If we've reached the
				// needed quantity stop the loop.
				if (DecrementRequired(item, required))
					break;
			}
		}
		// Check if we need to search inside the character's equipment.
		if (searchOptions.searchInEquipment && (required > 0)) {
			for (auto const &item : character->equipment) {
				// Check if the item is a valid resource.
				if (!ItemUtils::IsValidResource(item, resource.first))
					continue;
				// Decrement the required quantity. If we've reached the
				// needed quantity stop the loop.
				if (DecrementRequired(item, required))
					break;
			}
		}
		// Check if we need to search inside the character's inventory.
		if (searchOptions.searchInInventory && (required > 0)) {
			for (auto const &item : character->inventory) {
				// Check if the item is a valid resource.
				if (!ItemUtils::IsValidResource(item, resource.first))
					continue;
				// Decrement the required quantity. If we've reached the
				// needed quantity stop the loop.
				if (DecrementRequired(item, required))
					break;
			}
		}
		MudLog(LogLevel::Debug, "[%s] %s/%s", resource.first.toString(),
					required, resource.second);
		// If the ingredients are still not enough, return false.
		if (required > 0) {
			missing = resource.first;
			return false;
		}
	}
	return true;
}

Item *FindNearbyTool(Character *character, const ToolType &toolType,
					 const ItemVector &exceptions,
					 const SearchOptionsCharacter &searchOptions)
{
	if (searchOptions.searchInRoom) {
		for (auto item : character->room->items) {
			if (ItemUtils::IsValidTool(item, exceptions, toolType))
				return item;
		}
	}
	if (searchOptions.searchInEquipment) {
		for (auto item : character->equipment) {
			if (ItemUtils::IsValidTool(item, exceptions, toolType))
				return item;
		}
	}
	if (searchOptions.searchInInventory) {
		for (auto item : character->inventory) {
			if (ItemUtils::IsValidTool(item, exceptions, toolType))
				return item;
		}
	}
	return nullptr;
}

bool FindNearbyTools(Character *character, std::vector<ToolType> requiredTools,
					 ItemVector &foundTools,
					 const SearchOptionsCharacter &searchOptions)
{
	for (auto const &requiredTool : requiredTools) {
		auto tool =
			FindNearbyTool(character, requiredTool, foundTools, searchOptions);
		if (tool == nullptr)
			return false;
		foundTools.emplace_back(tool);
	}
	return true;
}

Item *FindNearbyBuilding(Character *character,
						 std::shared_ptr<ItemModel> buildingModel,
						 const SearchOptionsCharacter &searchOptions)
{
	// Create a function which checks if the given item is of the required type.
	auto IsValidBuilding = [buildingModel](Item *item) {
		// Check the pointer to the model.
		if (item->model == nullptr)
			return false;
		return buildingModel->vnum == item->model->vnum;
	};
	if (searchOptions.searchInRoom) {
		for (auto item : character->room->items) {
			// Check if it is a valid item.
			if (IsValidBuilding(item))
				return item;
		}
	}
	if (searchOptions.searchInEquipment) {
		for (auto item : character->equipment) {
			// Check if it is a valid item.
			if (IsValidBuilding(item))
				return item;
		}
	}
	if (searchOptions.searchInInventory) {
		for (auto item : character->inventory) {
			// Check if it is a valid item.
			if (IsValidBuilding(item))
				return item;
		}
	}
	return nullptr;
}

bool MoveCharacterTo(Character *character, Room *destination,
					 const std::string &msgDepart, const std::string &msgArrive,
					 const std::string &msgChar)
{
	// Check if the function has received a valid character.
	if (WrongAssert(character == nullptr))
		return false;
	// Check if the function has received a valid destination.
	if (WrongAssert(destination == nullptr))
		return false;
	// Check if the character is inside a valid room.
	if (WrongAssert(character->room == nullptr))
		return false;
	// Activate the entrance event for every mobile in the room.
	for (auto mobile : character->room->getAllMobile(character)) {
		if (mobile->canSee(character)) {
			mobile->triggerEventExit(character);
		}
	}
	// Show a message to the character, if is set.
	if (!msgChar.empty())
		character->sendMsg(msgChar);
	// Tell others where the character went and remove s/he from the old room.
	character->room->sendToAll(msgDepart, { character });
	// Remove the player from the current room.
	character->room->removeCharacter(character);
	// Add the character to the destionation room.
	destination->addCharacter(character);
	// Look around new room.
	character->doCommand("look");
	// Tell others s/he has arrived and move the character to the new room.
	destination->sendToAll(msgArrive, { character });
	// Activate the entrance event for every mobile in the room.
	for (auto mobile : character->room->getAllMobile(character)) {
		if (mobile->canSee(character)) {
			mobile->triggerEventEnter(character);
		}
	}
	return true;
}

bool HasRequiredKnowledge(Character *character,
						  std::vector<Knowledge> const &required)
{
	if (WrongAssert(character == nullptr))
		return false;
	return std::find_if(
			   required.begin(), required.end(),
			   [&character](Knowledge const &k) {
				   if (!((character->skillManager.getKnowledge(k) > 0) ||
						 (character->effectManager.getKnowledge(k) > 0))) {
					   MudLog(LogLevel::Debug, "Missing: %s",
								   k.toString());
					   return false;
				   }
				   return true;
			   }) != required.end();
}

bool ParseAbilities(std::map<Ability, unsigned int> &abilities,
					std::string const &source)
{
	if (source.empty())
		return false;
	std::vector<std::string> charList = SplitString(source, ";");
	if (charList.size() != 5)
		return false;
	// Support function.
	auto SetAbilityScore = [](std::string const &s) {
		auto value = ToNumber<unsigned int>(s);
		return (value <= 60) ? value : 60;
	};
	// Clean the map.
	abilities.clear();
	// Set the abilities.
	abilities[Ability::Strength] = SetAbilityScore(charList[0]);
	abilities[Ability::Agility] = SetAbilityScore(charList[1]);
	abilities[Ability::Perception] = SetAbilityScore(charList[2]);
	abilities[Ability::Constitution] = SetAbilityScore(charList[3]);
	abilities[Ability::Intelligence] = SetAbilityScore(charList[4]);
	return true;
}