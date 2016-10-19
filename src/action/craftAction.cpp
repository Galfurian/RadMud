/// @file   craftAction.cpp
/// @brief  Implementation of the class for a build action.
/// @author Enrico Fraccaroli
/// @date   Jul 14 2016
/// @copyright
/// Copyright (c) 2016 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
/// Permission to use, copy, modify, and distribute this software for any
/// purpose with or without fee is hereby granted, provided that the above
/// copyright notice and this permission notice appear in all copies.
///
/// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
/// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
/// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
/// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
/// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
/// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
/// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#include "craftAction.hpp"
#include "../structure/room.hpp"
#include "../sqlite/sqliteDbms.hpp"

using namespace std::chrono;

CraftAction::CraftAction(
    Character * _actor,
    Production * _production,
    Material * _material,
    std::vector<Item *> & _tools,
    std::vector<std::pair<Item *, unsigned int>> & _ingredients,
    unsigned int & _cooldown) :
    GeneralAction(_actor, system_clock::now() + seconds(_cooldown)),
    production(_production),
    material(_material),
    tools(_tools),
    ingredients(_ingredients)
{
    Logger::log(LogLevel::Debug, "Created crafting action.");
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
            Logger::log(LogLevel::Error, "The production outcome is a null pointer.");
            error = "You don't have a production set.";
            return false;
        }
        if (production->profession == nullptr)
        {
            Logger::log(LogLevel::Error, "The production profession is a null pointer.");
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
            Logger::log(LogLevel::Error, "One of the ingredients is a null pointer.");
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
    if (this->getConsumedStamina(actor) > actor->getStamina())
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
                ResourceModel * resourceModel = item->model->toResource();
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
    std::vector<Item *> createdItems, destroyItems;
    // Add the ingredients to the list of items to destroy.
    for (auto it : ingredients)
    {
        auto ingredient = it.first;
        if (ingredient->quantity == it.second)
        {
            destroyItems.push_back(ingredient);
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
        auto newItem = outcomeModel->createItem(
            actor->getName(),
            material,
            false,
            ItemQuality::Normal,
            production->quantity);
        if (newItem == nullptr)
        {
            Logger::log(LogLevel::Error, actor->getName() + "Crafted item is a null pointer.");
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
                Logger::log(LogLevel::Error, actor->getName() + "Crafted item is a null pointer.");
                // Delete all the items created so far.
                for (auto createdItem : createdItems)
                {
                    delete (createdItem);
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
        if (iterator->triggerDecay())
        {
            actor->sendMsg("%s falls into pieces.", iterator->getNameCapital(true));
            destroyItems.push_back(iterator);
        }
    }
    // Consume the items.
    for (auto it : destroyItems)
    {
        // First, remove the item from the mud.
        it->removeFromMud();
        // Then, remove it from the database.
        it->removeOnDB();
        // Finally, delete it.
        delete (it);
    }
    // Send conclusion message.
    actor->sendMsg(
        "%s %s.\n\n",
        production->profession->finishMessage,
        createdItems.back()->getName(true));
    if (dropped)
    {
        actor->sendMsg("some of the items have been placed on the ground.\n\n");
    }
    return ActionStatus::Finished;
}

unsigned int CraftAction::getConsumedStamina(Character * character)
{
    // BASE     [+1.0]
    // STRENGTH [-0.0 to -2.80]
    // WEIGHT   [+1.6 to +2.51]
    // CARRIED  [+0.0 to +2.48]
    return static_cast<unsigned int>(1.0
                                     - character->getAbilityLog(Ability::Strength, 0.0, 1.0)
                                     + SafeLog10(character->weight)
                                     + SafeLog10(character->getCarryingWeight()));
}
