/// @file   characterUtilities.hpp
/// @brief This file contains all the utility functions concerning a character.
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

#pragma once

#include <vector>
#include <map>

#include "character/bodyPart.hpp"
#include "item/itemVector.hpp"
#include "item/item.hpp"
#include "character/character.hpp"

class MeleeWeaponItem;

class RangedWeaponItem;

class Character;

class ResourceType;

class ToolType;

class Room;

class Direction;

class Production;

class ItemModel;

template <typename WeaponItemType>
std::vector<WeaponItemType *> GetActiveWeapons(Character *character)
{
	std::vector<WeaponItemType *> weapons;
	for (auto item : character->equipment) {
		// Cast the item to the desired type of weapon.
		auto wpn(dynamic_cast<WeaponItemType *>(item));
		if (wpn == nullptr)
			continue;
		// If at least one of the occupied body parts can be used to wield
		// a weapon, consider it an active weapon.
		for (auto const &bodyPart : item->occupiedBodyParts) {
			// Skip bodyparts which cannot wield.
			if (!HasFlag(bodyPart->flags, BodyPartFlag::CanWield))
				continue;
			// Store the item and break the loop otherwise the weapon would be
			// added for each occupied body part.
			weapons.emplace_back(wpn);
			break;
		}
	}
	return weapons;
}

/// @brief Provides the list of active natural weapons.
/// @param character The target character.
/// @return Vector of natural weapons.
std::vector<std::shared_ptr<BodyPart::BodyWeapon> > GetActiveNaturalWeapons(Character *character);

/// @brief Search the coins inside the given container.
/// @param character The target character.
/// @return List of found coins.
void FindCoinsInContainer(ItemVector const &container, ItemVector &foundCoins,
						  bool iterative = false);

/// @brief Search the coins on the character.
/// @param character The target character.
/// @return List of found coins.
ItemVector FindPosessedCoins(Character *character);

/// @brief Structure used to determine the options of the search.
struct SearchOptionsCharacter {
public:
	/// Allows the function to search inside the room.
	bool searchInRoom;
	/// Allows the function to search inside the character's inventory.
	bool searchInInventory;
	/// Allows the function to search inside the character's equipment.
	bool searchInEquipment;
	/// Checks if the lights is present.
	bool checkLightLevels;
	/// If there is no light it will pick a random item from inventory
	/// and equipment.
	bool randomIfNoLight;

	/// @brief Constructor.
	explicit SearchOptionsCharacter(bool _searchInRoom = false, bool _searchInInventory = false,
									bool _searchInEquipment = false, bool _checkLightLevels = false,
									bool _randomIfNoLight = false) :
		searchInRoom(_searchInRoom),
		searchInInventory(_searchInInventory),
		searchInEquipment(_searchInEquipment),
		checkLightLevels(_checkLightLevels),
		randomIfNoLight(_randomIfNoLight)
	{
		// Nothing to do.
	}

	/// @brief Destructor.
	~SearchOptionsCharacter() = default;

	static SearchOptionsCharacter allOptions()
	{
		SearchOptionsCharacter searchOptions;
		searchOptions.searchInRoom = true;
		searchOptions.searchInInventory = true;
		searchOptions.searchInEquipment = true;
		searchOptions.checkLightLevels = true;
		searchOptions.randomIfNoLight = true;
		return searchOptions;
	}
};

/// @brief Search the given item name nearby the character.
Item *FindNearbyItem(Character *character, std::string const &key, unsigned int number,
					 unsigned int *number_ptr, const SearchOptionsCharacter &searchOptions);

/// @brief Search the list of ingredients nearby the character.
/// @param character            The target character.
/// @param requiredResources    The list of required resources.
/// @param foundResources       The list of found resources.
/// @param searchOptions        The search options.
/// @return <b>True</b> if the operation goes well,<br>
///         <b>False</b> otherwise.
bool FindNearbyResouces(Character *character,
						std::map<ResourceType, unsigned int> const &requiredResources,
						std::vector<std::pair<Item *, unsigned int> > &foundResources,
						const SearchOptionsCharacter &searchOptions, ResourceType &missing);

/// @brief Search the given type of tool in the proximity of the character.
/// @param character        The target character.
/// @param toolType         The type of tool that has to be searched.
/// @param exceptions       Items which cannot be selected.
/// @param searchOptions    The search options.
/// @return The searched tool.
Item *FindNearbyTool(Character *character, const ToolType &toolType, const ItemVector &exceptions,
					 const SearchOptionsCharacter &searchOptions);

/// @brief Search the list of tools nearby the character.
/// @param character        The target character.
/// @param requiredTools    The list of required tools.
/// @param foundTools       The list of found tools.
/// @param searchOptions    The search options.
/// @return <b>True</b> if the operation goes well,<br>
///         <b>False</b> otherwise.
bool FindNearbyTools(Character *character, std::vector<ToolType> requiredTools,
					 ItemVector &foundTools, const SearchOptionsCharacter &searchOptions);

/// @brief Search the required building.
/// @param character        The target character.
/// @param buildingModel    The building.
/// @param searchOptions    The search options.
/// @return <b>True</b> if the operation goes well,<br>
///         <b>False</b> otherwise.
Item *FindNearbyBuilding(Character *character, std::shared_ptr<ItemModel> buildingModel,
						 const SearchOptionsCharacter &searchOptions);

/// @brief Move the character to another room.
/// @param character   The target character.
/// @param destination The destination room.
/// @param msgDepart   Tell people in original room.
/// @param msgArrive   Tell people in new room.
/// @param msgChar     What to tell the character.
/// @return <b>True</b> if the operation goes well,<br>
///         <b>False</b> otherwise.
bool MoveCharacterTo(Character *character, Room *destination, const std::string &msgDepart,
					 const std::string &msgArrive, const std::string &msgChar = "");

/// @brief Checks if the character has the required knowledge for the
///         given production.
/// @param character    The target character.
/// @param production   The production to check.
/// @return <b>True</b> has the required knowledge,<br>
///         <b>False</b> otherwise.
bool HasRequiredKnowledge(Character *character, std::vector<Knowledge> const &required);

/// @brief Parse the abilities from the given string.
/// @param abilities The map where the abilities should be stored.
/// @param source    The string which containts the values
/// @return <b>True</b> if the string is correct,<br>
///         <b>False</b> otherwise.
bool ParseAbilities(std::map<Ability, unsigned int> &abilities, std::string const &source);