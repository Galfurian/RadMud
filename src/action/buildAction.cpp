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
#include "../structure/room.hpp"
#include "../sqlite/sqliteDbms.hpp"
#include "../utilities/formatter.hpp"

using namespace std::chrono;

BuildAction::BuildAction(
    Character * _actor,
    Building * _schematics,
    Item * _building,
    std::vector<Item *> & _tools,
    std::vector<std::pair<Item *, unsigned int>> & _ingredients,
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

bool BuildAction::check(std::string & error) const
{
    if (!GeneralAction::check(error))
    {
        return false;
    }
    if (building == nullptr)
    {
        Logger::log(LogLevel::Error, "The building is a null pointer.");
        error = "You don't have a building set.";
        return false;
    }
    if (schematics == nullptr)
    {
        Logger::log(LogLevel::Error, "The schematics for a building are a null pointer.");
        error = "You don't have a valid schematics set.";
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
    for (auto it : schematics->ingredients)
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

ActionStatus BuildAction::perform()
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
    actor->remInventoryItem(building);
    actor->room->addBuilding(building);
    // Vector which will contain the list of items to destroy.
    std::vector<Item *> destroyItems;
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
    for (auto iterator : tools)
    {
        // Update the condition of the involved objects.
        if (iterator->triggerDecay())
        {
            actor->sendMsg(iterator->getName(true) + " falls into pieces.");
            destroyItems.push_back(iterator);
        }
    }
    // Consume the items.
    for (auto it : destroyItems)
    {
        it->removeFromMud();
        it->removeOnDB();
        delete (it);
    }
    // Send conclusion message.
    actor->sendMsg("You have finished building %s.\n\n",
                   Formatter::yellow() + schematics->buildingModel->getName() + Formatter::reset());
    return ActionStatus::Finished;
}

unsigned int BuildAction::getConsumedStamina(Character * character)
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
