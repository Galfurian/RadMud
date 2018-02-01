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

#include "characterUtilities.hpp"
#include "rangedWeaponItem.hpp"
#include "meleeWeaponItem.hpp"
#include "resourceModel.hpp"
#include "toolModel.hpp"
#include "character.hpp"
#include "logger.hpp"
#include "room.hpp"

std::vector<std::shared_ptr<BodyPart::BodyWeapon>> GetActiveNaturalWeapons(
    Character * character)
{
    std::vector<std::shared_ptr<BodyPart::BodyWeapon>> naturalWeapons;
    for (auto const & bodyPart : character->race->bodyParts)
    {
        // If the body part is holding an item, skip it.
        if (character->findItemAtBodyPart(bodyPart) != nullptr) continue;
        // If the body part cannot be used as natural weapon, skip it.
        if (bodyPart->weapon == nullptr) continue;
        // Add the natural weapon to the vector.
        naturalWeapons.emplace_back(bodyPart->weapon);
    }
    return naturalWeapons;
}

void FindCoinsInContainer(ItemVector const & container,
                          ItemVector & foundCoins,
                          const bool iterative)
{
    // Analyze the content.
    for (auto item : container)
    {
        if (item->getType() == ModelType::Currency)
        {
            foundCoins.emplace_back(item);
        }
        else if (iterative)
        {
            if (item->getType() == ModelType::Container)
            {
                // Cast the item to container.
                auto containerItem = dynamic_cast<ContainerItem *>(item);
                // Iterate inside the container's content.
                FindCoinsInContainer(containerItem->content,
                                     foundCoins,
                                     iterative);
            }
        }
    }
}

ItemVector FindPosessedCoins(Character * character)
{
    ItemVector foundCoins;
    FindCoinsInContainer(character->equipment, foundCoins, false);
    FindCoinsInContainer(character->inventory, foundCoins, false);
    foundCoins.orderBy(ItemVector::ByPrice);
    return foundCoins;
}

// -----------------------------------------------------------------------------
SearchOptionsCharacter::SearchOptionsCharacter() :
    searchInRoom(),
    searchInInventory(),
    searchInEquipment()
{
    // Nothing to do.
}

SearchOptionsCharacter::SearchOptionsCharacter(
    const SearchOptionsCharacter & other) :
    searchInRoom(other.searchInRoom),
    searchInInventory(other.searchInInventory),
    searchInEquipment(other.searchInEquipment)
{
    // Nothing to do.
}

SearchOptionsCharacter::~SearchOptionsCharacter()
{
    // Nothing to do.
}
// -----------------------------------------------------------------------------

bool FindNearbyResouces(
    Character * character,
    std::map<ResourceType, unsigned int> requiredResources,
    std::vector<std::pair<Item *, unsigned int>> & foundResources,
    const SearchOptionsCharacter & searchOptions)
{
    // Create a function which checks if the given item is of the required type.
    auto IsValidResource = [](Item * item, const ResourceType & resourceType)
    {
        // Check the pointer to the model.
        if (item->model == nullptr) return false;
        // Check if the item is a resource.
        if (item->model->getType() != ModelType::Resource) return false;
        // Cast the model to ResourceModel.
        auto resourceModel = item->model->toResource();
        // Check if the type of resource is the same.
        return (resourceModel->resourceType == resourceType);
    };
    // Create a function which reduces the required quantity and checks if
    // the zero has been reached.
    auto DecrementRequired = [&foundResources](Item * item,
                                               unsigned int & requiredQuantity)
    {
        // Get the available quantity.
        auto quantityAvailable = item->quantity;
        // Set the used quantity.
        auto quantityUsed = quantityAvailable;
        if (quantityAvailable > requiredQuantity)
        {
            quantityUsed = (quantityAvailable - requiredQuantity);
        }
        // Add the item to the list of used resources.
        foundResources.emplace_back(std::make_pair(item, quantityUsed));
        // Reduce the quantity needed.
        requiredQuantity -= quantityUsed;
        // Returns if we've reached the needed quantity.
        return (requiredQuantity == 0);
    };
    for (auto resource : requiredResources)
    {
        // Quantity of ingredients that has to be found.
        auto quantityNeeded = resource.second;
        // Check if we need to search inside the room.
        // TODO: Probably I need to check if a given item is not selected twice.
        if (searchOptions.searchInRoom)
        {
            for (auto item : character->room->items)
            {
                // Check if the item is a valid resource.
                if (!IsValidResource(item, resource.first)) continue;
                // Decrement the required quantity. If we've reached the
                // needed quantity stop the loop.
                if (DecrementRequired(item, quantityNeeded)) break;
            }
        }
        // Check if we need to search inside the character's equipment.
        if (searchOptions.searchInEquipment)
        {
            for (auto item : character->equipment)
            {
                // Check if the item is a valid resource.
                if (!IsValidResource(item, resource.first)) continue;
                // Decrement the required quantity. If we've reached the
                // needed quantity stop the loop.
                if (DecrementRequired(item, quantityNeeded)) break;
            }
        }
        // Check if we need to search inside the character's inventory.
        if (searchOptions.searchInInventory)
        {
            for (auto item : character->inventory)
            {
                // Check if the item is a valid resource.
                if (!IsValidResource(item, resource.first)) continue;
                // Decrement the required quantity. If we've reached the
                // needed quantity stop the loop.
                if (DecrementRequired(item, quantityNeeded)) break;
            }
        }
        // If the ingredients are still not enough, return false.
        if (quantityNeeded > 0) return false;
    }
    return true;
}

Item * FindNearbyTool(
    Character * character,
    const ToolType & toolType,
    const ItemVector & exceptions,
    const SearchOptionsCharacter & searchOptions)
{
    // Create a function which checks if the given item is of the required type.
    auto IsValidTool = [toolType, exceptions](Item * item)
    {
        // Check the pointer to the model.
        if (item->model == nullptr) return false;
        // Check if the item is a resource.
        if (item->model->getType() != ModelType::Tool) return false;
        // Cast the model to ResourceModel.
        auto toolModel = item->model->toTool();
        // Check if the type of resource is the same.
        if (toolModel->toolType != toolType) return false;
        // Check if the item is inside the exception list.
        return std::find_if(exceptions.begin(),
                            exceptions.end(),
                            [item](Item * exception)
                            {
                                return (item->vnum == exception->vnum);
                            }) == exceptions.end();
    };
    if (searchOptions.searchInRoom)
    {
        for (auto item : character->room->items)
        {
            // Check if it is a valid item.
            if (IsValidTool(item)) return item;
        }
    }
    if (searchOptions.searchInEquipment)
    {
        for (auto item : character->equipment)
        {
            // Check if it is a valid item.
            if (IsValidTool(item)) return item;
        }
    }
    if (searchOptions.searchInInventory)
    {
        for (auto item : character->inventory)
        {
            // Check if it is a valid item.
            if (IsValidTool(item)) return item;
        }
    }
    return nullptr;
}

bool FindNearbyTools(
    Character * character,
    std::vector<ToolType> requiredTools,
    ItemVector & foundTools,
    const SearchOptionsCharacter & searchOptions)
{
    for (auto const & requiredTool : requiredTools)
    {
        auto tool = FindNearbyTool(character,
                                   requiredTool,
                                   foundTools,
                                   searchOptions);
        if (tool == nullptr) return false;
        foundTools.emplace_back(tool);
    }
    return true;
}

Item * FindNearbyBuilding(
    Character * character,
    std::shared_ptr<ItemModel> buildingModel,
    const SearchOptionsCharacter & searchOptions)
{
    // Create a function which checks if the given item is of the required type.
    auto IsValidBuilding = [buildingModel](Item * item)
    {
        // Check the pointer to the model.
        if (item->model == nullptr) return false;
        return buildingModel->vnum == item->model->vnum;
    };
    if (searchOptions.searchInRoom)
    {
        for (auto item : character->room->items)
        {
            // Check if it is a valid item.
            if (IsValidBuilding(item)) return item;
        }
    }
    if (searchOptions.searchInEquipment)
    {
        for (auto item : character->equipment)
        {
            // Check if it is a valid item.
            if (IsValidBuilding(item)) return item;
        }
    }
    if (searchOptions.searchInInventory)
    {
        for (auto item : character->inventory)
        {
            // Check if it is a valid item.
            if (IsValidBuilding(item)) return item;
        }
    }
    return nullptr;
}

bool MoveCharacterTo(
    Character * character,
    Room * destination,
    const std::string & msgDepart,
    const std::string & msgArrive,
    const std::string & msgChar)
{
    // Check if the function has received a valid character.
    if (WrongAssert(character == nullptr)) return false;
    // Check if the function has received a valid destination.
    if (WrongAssert(destination == nullptr)) return false;
    // Check if the character is inside a valid room.
    if (WrongAssert(character->room == nullptr)) return false;
    // Activate the entrance event for every mobile in the room.
    for (auto mobile : character->room->getAllMobile(character))
    {
        if (mobile->canSee(character))
        {
            mobile->triggerEventExit(character);
        }
    }
    // Show a message to the character, if is set.
    if (!msgChar.empty()) character->sendMsg(msgChar);
    // Tell others where the character went and remove s/he from the old room.
    character->room->sendToAll(msgDepart, {character});
    // Remove the player from the current room.
    character->room->removeCharacter(character);
    // Add the character to the destionation room.
    destination->addCharacter(character);
    // Look around new room.
    character->doCommand("look");
    // Tell others s/he has arrived and move the character to the new room.
    destination->sendToAll(msgArrive, {character});
    // Activate the entrance event for every mobile in the room.
    for (auto mobile : character->room->getAllMobile(character))
    {
        if (mobile->canSee(character))
        {
            mobile->triggerEventEnter(character);
        }
    }
    return true;
}

bool HasRequiredKnowledge(Character * character, Production * production)
{
    if (WrongAssert(character == nullptr)) return false;
    if (WrongAssert(production == nullptr)) return false;
    return std::find_if(
        production->requiredKnowledge.begin(),
        production->requiredKnowledge.end(),
        [&character](Knowledge const & k)
        {
            return (character->skillManager.getKnowledge(k) > 0) ||
                   (character->effectManager.getKnowledge(k) > 0);
        }) != production->requiredKnowledge.end();
}