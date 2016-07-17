/// @file   buildAction.cpp
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

#include "buildAction.hpp"
#include "../logger.hpp"
#include "../character.hpp"
#include "../sqlite/SQLiteDbms.hpp"
#include "../formatter.hpp"
#include "../room.hpp"

using namespace std::chrono;

BuildAction::BuildAction(
    Character * _actor,
    Building * _schematics,
    Item * _building,
    ItemVector & _tools,
    ItemVector & _ingredients,
    unsigned int _cooldown) :
        GeneralAction(_actor, system_clock::now() + seconds(_cooldown)),
        schematics(_schematics),
        building(_building),
        tools(_tools),
        ingredients(_ingredients)
{
    Logger::log(LogLevel::Debug, "Created building action.");
}

BuildAction::~BuildAction()
{
    Logger::log(LogLevel::Debug, "Deleted building action.");
}

bool BuildAction::check() const
{
    bool correct = GeneralAction::check();
    correct &= this->checkBuilding();
    correct &= this->checkSchematics();
    correct &= this->checkTools();
    correct &= this->checkIngredients();
    return correct;
}

ActionType BuildAction::getType() const
{
    return ActionType::Building;
}

std::string BuildAction::getDescription() const
{
    return "building";
}

std::string BuildAction::stop()
{
    return "You stop building.";
}

bool BuildAction::perform()
{
    // Check if the cooldown is ended.
    if (!this->checkElapsed())
    {
        return false;
    }
    // Check the character stamina.
    unsigned int consumedStamina;
    if (!checkHasStamina(consumedStamina))
    {
        actor->sendMsg("\nYou are too tired right now.\n");
        // Set the status to Error.
        this->actionStatus = ActionStatus::Error;
        return false;
    }
    if (!checkBuilding() || !checkSchematics() || !checkTools() || !checkIngredients())
    {
        actor->sendMsg("\nYou have failed your action.\n");
        // Set the status to Error.
        this->actionStatus = ActionStatus::Error;
        return false;
    }

    // Consume the stamina.
    actor->remStamina(consumedStamina, true);

    SQLiteDbms::instance().beginTransaction();
    actor->remInventoryItem(building);
    actor->room->addBuilding(building);
    building->updateOnDB();
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
        "You have finished building %s.\n\n",
        Formatter::yellow() + schematics->buildingModel->getName() + Formatter::reset());
    return true;
}

bool BuildAction::checkHasStamina(unsigned int & consumed) const
{
    // Check if the actor has enough stamina to execute the action.
    if (!actor->hasStaminaFor(consumed, ActionType::Building))
    {
        return false;
    }
    return true;
}

bool BuildAction::checkBuilding() const
{
    if (building == nullptr)
    {
        Logger::log(LogLevel::Error, "The building is a null pointer.");
        return false;
    }
    return true;
}

bool BuildAction::checkSchematics() const
{
    if (schematics == nullptr)
    {
        Logger::log(LogLevel::Error, "The schematics for a building are a null pointer.");
        return false;
    }
    return true;
}

bool BuildAction::checkIngredients() const
{
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

bool BuildAction::checkTools() const
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