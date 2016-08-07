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
#include "../character.hpp"
#include "../formatter.hpp"
#include "../room.hpp"
#include "../sqlite/sqliteDbms.hpp"
#include "../utilities/logger.hpp"

using namespace std::chrono;

CraftAction::CraftAction(
    Character * _actor,
    Production * _production,
    Material * _material,
    ItemVector & _tools,
    ItemVector & _ingredients,
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

bool CraftAction::check() const
{
    bool correct = GeneralAction::check();
    correct &= this->checkProduction();
    correct &= this->checkMaterial();
    correct &= this->checkTools();
    correct &= this->checkIngredients();
    return correct;
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
    if (!checkProduction() || !checkMaterial() || !checkTools() || !checkIngredients())
    {
        actor->sendMsg("\nYou have failed your action.\n");
        return ActionStatus::Error;
    }
    // Get the amount of required stamina.
    unsigned int consumedStamina = actor->getConsumedStaminaFor(ActionType::Crafting);
    // Check if the actor has enough stamina to execute the action.
    if (consumedStamina > actor->getStamina())
    {
        actor->sendMsg("\nYou are too tired right now.\n");
        return ActionStatus::Error;
    }
    // Consume the stamina.
    actor->remStamina(consumedStamina, true);

    SQLiteDbms::instance().beginTransaction();
    ItemVector createdItems;
    for (unsigned int it = 0; it < production->quantity; ++it)
    {
        ItemModel * outcomeModel = production->outcome;
        // Create the item.
        Item * newItem = outcomeModel->createItem(actor->getName(), material, ItemQuality::Normal);
        if (newItem == nullptr)
        {
            // Log a warning.
            Logger::log(
                LogLevel::Warning,
                actor->getName() + ":craft = New item is a null pointer.");
            // Rollback the database.
            SQLiteDbms::instance().rollbackTransection();
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
    SQLiteDbms::instance().endTransaction();

    // Add the created items into the character inventory.
    bool dropped = false;
    SQLiteDbms::instance().beginTransaction();
    for (auto createdItem : createdItems)
    {
        if (!actor->canCarry(createdItem))
        {
            actor->room->addItem(createdItem);
            dropped = true;
        }
        else
        {
            actor->addInventoryItem(createdItem);
        }
        createdItem->updateOnDB();
    }
    // Update the tools.
    ItemVector toDestroy;
    for (auto iterator : tools)
    {
        // Update the condition of the involved objects.
        if (iterator->triggerDecay())
        {
            actor->sendMsg(iterator->getName() + " falls into pieces.");
            toDestroy.push_back(iterator);
        }
    }
    for (auto it : ingredients)
    {
        toDestroy.push_back(it);
    }
    for (auto it : toDestroy)
    {
        it->destroy();
    }
    SQLiteDbms::instance().endTransaction();

    // Send conclusion message.
    actor->sendMsg(
        "%s %s.\n\n",
        production->profession->finishMessage,
        Formatter::yellow() + createdItems.back()->getName() + Formatter::reset());
    if (dropped)
    {
        actor->sendMsg(
            "Since you can't carry them, some of the items have been placed on the ground.\n\n");
    }
    return ActionStatus::Finished;
}

bool CraftAction::checkProduction() const
{
    bool correct = true;
    if (production == nullptr)
    {
        Logger::log(LogLevel::Error, "The production is a null pointer.");
        correct = false;
    }
    else
    {
        if (production->outcome == nullptr)
        {
            Logger::log(LogLevel::Error, "The production outcome is a null pointer.");
            correct = false;
        }
        if (production->profession == nullptr)
        {
            Logger::log(LogLevel::Error, "The production profession is a null pointer.");
            correct = false;
        }
    }
    return correct;
}

bool CraftAction::checkMaterial() const
{
    if (material == nullptr)
    {
        Logger::log(LogLevel::Error, "The material is a null pointer.");
        return false;
    }
    return true;
}

bool CraftAction::checkIngredients() const
{
    if (ingredients.empty())
    {
        Logger::log(LogLevel::Error, "No used ingredients have been set.");
        return false;
    }
    for (auto iterator : ingredients)
    {
        // Check if the ingredient has been deleted.
        if (iterator == nullptr)
        {
            Logger::log(LogLevel::Error, "One of the ingredients is a null pointer.");
            return false;
        }
    }
    return true;
}

bool CraftAction::checkTools() const
{
    if (tools.empty())
    {
        Logger::log(LogLevel::Error, "No used tools have been set.");
        return false;
    }
    for (auto iterator : tools)
    {
        // Check if the tool has been deleted.
        if (iterator == nullptr)
        {
            Logger::log(LogLevel::Error, "One of the tools is a null pointer.");
            return false;
        }
    }
    return true;
}
