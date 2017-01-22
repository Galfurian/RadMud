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

#include "updater.hpp"
#include "logger.hpp"
#include "room.hpp"

CraftAction::CraftAction(Character * _actor,
                         Production * _production,
                         ItemVector & _tools,
                         std::vector<std::pair<Item *, unsigned int>> & _ingredients) :
    GeneralAction(_actor),
    production(_production),
    tools(_tools),
    ingredients(_ingredients),
    material()
{
    // Debugging message.
    Logger::log(LogLevel::Debug, "Created CraftAction.");
    // Determine the material of the creation based on the ammount of each
    // single involved item.
    this->determineMaterial();
    // Reset the cooldown of the action.
    this->resetCooldown(CraftAction::getCooldown(_actor, _production));
}

CraftAction::~CraftAction()
{
    Logger::log(LogLevel::Debug, "Deleted crafting action.");
}

bool CraftAction::check(std::string & error) const
{
    if (!GeneralAction::check(error))
    {
        return false;
    }
    if (production == nullptr)
    {
        Logger::log(LogLevel::Error, "The production is a null pointer.");
        error = "You don't have a production set.";
        return false;
    }
    else
    {
        if (production->outcome == nullptr)
        {
            Logger::log(LogLevel::Error, "The outcome is a null pointer.");
            error = "You don't have a production set.";
            return false;
        }
        if (production->profession == nullptr)
        {
            Logger::log(LogLevel::Error, "The profession is a null pointer.");
            error = "You don't have a production set.";
            return false;
        }
    }
    if (material == nullptr)
    {
        Logger::log(LogLevel::Error, "The material is a null pointer.");
        error = "You don't know how to craft the item.";
        return false;
    }
    for (auto iterator : ingredients)
    {
        // Check if the ingredient has been deleted.
        if (iterator.first == nullptr)
        {
            Logger::log(LogLevel::Error, "Null pointer ingredient.");
            error = "One of your ingredient is missing.";
            return false;
        }
    }
    if (tools.empty())
    {
        Logger::log(LogLevel::Error, "No used tools have been set.");
        error = "One or more tools are missing.";
        return false;
    }
    for (auto iterator : tools)
    {
        // Check if the tool has been deleted.
        if (iterator == nullptr)
        {
            Logger::log(LogLevel::Error, "One of the tools is a null pointer.");
            error = "One or more tools are missing.";
            return false;
        }
    }
    // Check if the actor has enough stamina to execute the action.
    if (this->getConsumedStamina(actor) > actor->stamina)
    {
        error = "You are too tired right now.";
        return false;
    }
    // Add the ingredients to the list of items to destroy.
    for (auto it : production->ingredients)
    {
        auto required = it.second;
        for (auto it2 : ingredients)
        {
            auto item = it2.first;
            if (item->getType() == ModelType::Resource)
            {
                auto resourceModel = item->model->toResource();
                if (resourceModel->resourceType == it.first)
                {
                    required -= item->quantity;
                    if (required <= 0)
                    {
                        break;
                    }
                }
            }
        }
        if (required > 0)
        {
            error = "You don't have enough materials.";
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

std::string CraftAction::stop()
{
    return this->production->profession->interruptMessage + ".";
}

ActionStatus CraftAction::perform()
{
    // Check if the cooldown is ended.
    if (!this->checkElapsed())
    {
        return ActionStatus::Running;
    }
    // Check the values of the action.
    std::string error;
    if (!this->check(error))
    {
        actor->sendMsg(error + "\n\n");
        return ActionStatus::Error;
    }
    // Get the amount of required stamina and consume it.
    auto consumedStamina = this->getConsumedStamina(actor);
    actor->remStamina(consumedStamina, true);
    // Vector which will contain the list of created items and items to destroy.
    ItemVector createdItems;
    // Add the ingredients to the list of items to destroy.
    for (auto it : ingredients)
    {
        auto ingredient = it.first;
        if (ingredient->quantity == it.second)
        {
            // Add the ingredient to the list of items that has to be deleted.
            MudUpdater::instance().addItemToDestroy(ingredient);
        }
        else
        {
            ingredient->quantity -= it.second;
        }
    }
    // Get the outcome model.
    auto outcomeModel = production->outcome;
    if (HasFlag(outcomeModel->modelFlags, ModelFlag::CanBeStacked))
    {
        // Create the item.
        // TODO: FIX WRONG TYPE OF MATERIAL!
        auto newItem = outcomeModel->createItem(actor->getName(),
                                                material,
                                                false,
                                                ItemQuality::Normal,
                                                production->quantity);
        if (newItem == nullptr)
        {
            Logger::log(LogLevel::Error, "Crafted item is a null pointer.");
            actor->sendMsg("\nYou have failed your action.\n");
            return ActionStatus::Error;
        }
        createdItems.push_back(newItem);
    }
    else
    {
        for (unsigned int it = 0; it < production->quantity; ++it)
        {
            // Create the item.
            auto newItem = outcomeModel->createItem(actor->getName(), material);
            if (newItem == nullptr)
            {
                Logger::log(LogLevel::Error, "Crafted item is a null pointer.");
                // Delete all the items created so far.
                for (auto createdItem : createdItems)
                {
                    MudUpdater::instance().addItemToDestroy(createdItem);
                }
                // Notify the character.
                actor->sendMsg("\nYou have failed your action.\n");
                return ActionStatus::Error;
            }
            createdItems.push_back(newItem);
        }
    }
    // Flag which will become true if some items are dropped to the ground.
    bool dropped = false;
    // Add the created items to the character's inventory.
    for (auto createdItem : createdItems)
    {
        if (!actor->canCarry(createdItem, createdItem->quantity))
        {
            actor->room->addItem(createdItem);
            dropped = true;
        }
        else
        {
            actor->addInventoryItem(createdItem);
        }
    }
    for (auto iterator : tools)
    {
        // Update the condition of the involved objects.
        iterator->triggerDecay();
        if (iterator->condition < 0)
        {
            actor->sendMsg("%s falls into pieces.",
                           iterator->getNameCapital(true));
        }
    }
    // Send conclusion message.
    actor->sendMsg("%s %s.\n\n",
                   production->profession->finishMessage,
                   createdItems.back()->getName(true));
    if (dropped)
    {
        actor->sendMsg("some of the items have been placed on the ground.\n\n");
    }
    return ActionStatus::Finished;
}

void CraftAction::determineMaterial()
{
    /// TODO: Simplify...
    // The material of the outcome is determined by summing the weights of
    // the items with the same material. Then, the material with the highest
    // weight is chosen.
    struct MaterialEntry
    {
        /// The material.
        Material * material;
        /// The accumulated weight for the material.
        double weight;

        /// @brief Constructor.
        MaterialEntry() :
            material(),
            weight()
        {
            // Nothing to do.
        }

        /// @brief Constructor.
        MaterialEntry(Material * _material, double _weight) :
            material(_material),
            weight(_weight)
        {
            // Nothing to do.
        }
    };
    std::vector<MaterialEntry> materials;
    for (auto ingr : ingredients)
    {
        // Check if the ingredient has a material.
        if (ingr.first->composition == nullptr)
        {
            continue;
        }
        // Create a new material entry.
        MaterialEntry entry(ingr.first->composition,
                            ingr.first->getWeight(false) * ingr.second);
        bool found = false;
        for (auto it : materials)
        {
            if (it.material->vnum == entry.material->vnum)
            {
                found = true;
                it.weight += entry.weight;
                break;
            }
        }
        if (!found)
        {
            materials.emplace_back(entry);
        }
    }
    double weight = 0;
    for (auto it : materials)
    {
        if (it.weight > weight)
        {
            material = it.material;
            weight = it.weight;
        }
    }
}

unsigned int CraftAction::getConsumedStamina(Character * character)
{
    // BASE     [+1.0]
    // STRENGTH [-0.0 to -1.40]
    // WEIGHT   [+1.6 to +2.51]
    // CARRIED  [+0.0 to +2.48]
    unsigned int consumedStamina = 1;
    consumedStamina -= character->getAbilityLog(Ability::Strength);
    consumedStamina = SafeSum(consumedStamina, SafeLog10(character->weight));
    consumedStamina = SafeSum(consumedStamina,
                              SafeLog10(character->getCarryingWeight()));
    return consumedStamina;
}

unsigned int CraftAction::getCooldown(Character *, Production * _production)
{
    auto requiredTime = _production->time;

    return static_cast<unsigned int>(requiredTime);
}
