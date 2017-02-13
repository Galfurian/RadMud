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

#include "bodyPart.hpp"
#include "itemVector.hpp"

class MeleeWeaponItem;

class RangedWeaponItem;

class Character;

class ResourceType;

class ToolType;

class Room;

class Direction;

class Production;

/// @brief Provides the list of active melee weapons (Left and Right hands).
/// @param character The target character.
/// @return Vector of melee weapons.
std::vector<MeleeWeaponItem *> GetActiveMeleeWeapons(Character * character);

/// @brief Provides the list of active ranged weapons (Left and Right hands).
/// @param character The target character.
/// @return Vector of ranged weapons.
std::vector<RangedWeaponItem *> GetActiveRangedWeapons(Character * character);

/// @brief Provides the list of active natural weapons.
/// @param character The target character.
/// @return Vector of natural weapons.
std::vector<std::shared_ptr<BodyPart::BodyWeapon>> GetActiveNaturalWeapons(
    Character * character);

/// @brief Search the coins on the character.
/// @param character The target character.
/// @return List of found coins.
ItemVector FindPosessedCoins(Character * character);

/// @brief Structure used to determine the options of the search.
struct SearchOptionsCharacter
{
public:
    /// Allows the function to search inside the room.
    bool searchInRoom;
    /// Allows the function to search inside the character's inventory.
    bool searchInInventory;
    /// Allows the function to search inside the character's equipment.
    bool searchInEquipment;

    /// @brief Constructor.
    SearchOptionsCharacter();

    /// @brief Copy constructor.
    SearchOptionsCharacter(const SearchOptionsCharacter & other);

    /// @brief Destructor.
    ~SearchOptionsCharacter();
};

/// @brief Search the list of ingredients nearby the character.
/// @param character            The target character.
/// @param requiredResources    The list of required resources.
/// @param foundResources       The list of found resources.
/// @param searchOptions        The search options.
/// @return <b>True</b> if the operation goes well,<br>
///         <b>False</b> otherwise.
bool FindNearbyResouces(
    Character * character,
    std::map<ResourceType, unsigned int> requiredResources,
    std::vector<std::pair<Item *, unsigned int>> & foundResources,
    const SearchOptionsCharacter & searchOptions);

/// @brief Search the given type of tool in the proximity of the character.
/// @param character        The target character.
/// @param toolType         The type of tool that has to be searched.
/// @param exceptions       Items which cannot be selected.
/// @param searchOptions    The search options.
/// @return The searched tool.
Item * FindNearbyTool(
    Character * character,
    const ToolType & toolType,
    const ItemVector & exceptions,
    const SearchOptionsCharacter & searchOptions);

/// @brief Search the list of tools nearby the character.
/// @param character        The target character.
/// @param requiredTools    The list of required tools.
/// @param foundTools       The list of found tools.
/// @param searchOptions    The search options.
/// @return <b>True</b> if the operation goes well,<br>
///         <b>False</b> otherwise.
bool FindNearbyTools(
    Character * character,
    std::vector<ToolType> requiredTools,
    ItemVector & foundTools,
    const SearchOptionsCharacter & searchOptions);

/// @brief Move the character to another room.
/// @param character   The target character.
/// @param destination The destination room.
/// @param msgDepart   Tell people in original room.
/// @param msgArrive   Tell people in new room.
/// @param msgChar     What to tell the character.
/// @return <b>True</b> if the operation goes well,<br>
///         <b>False</b> otherwise.
bool MoveCharacterTo(
    Character * character,
    Room * destination,
    const std::string & msgDepart,
    const std::string & msgArrive,
    const std::string & msgChar = "");

/// @brief Checks if the character has the required knowledge for the
///         production.
/// @param character Tha target character.
/// @return <b>True</b> has the required knowledge,<br>
///         <b>False</b> otherwise.
bool HasRequiredKnowledge(Character * character, Production * production);
