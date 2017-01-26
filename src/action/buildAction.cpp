/// @file   buildAction.cpp
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

#include "buildAction.hpp"

#include "formatter.hpp"
#include "updater.hpp"
#include "logger.hpp"
#include "room.hpp"

BuildAction::BuildAction(Character * _actor,
                         Building * _schematics,
                         Item * _building,
                         ItemVector & _tools,
                         std::vector<std::pair<Item *, unsigned int>> & _ingredients) :
    GeneralAction(_actor),
    schematics(_schematics),
    building(_building),
    tools(_tools),
    ingredients(_ingredients)
{
    // Debugging message.
    Logger::log(LogLevel::Debug, "Created BuildAction.");
    // Reset the cooldown of the action.
    this->resetCooldown(BuildAction::getCooldown(_actor, _schematics));
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
        Logger::log(LogLevel::Error,
                    "The schematics for a building are a null pointer.");
        error = "You don't have a valid schematics set.";
        return false;
    }
    for (auto iterator : ingredients)
    {
        // Check if the ingredient has been deleted.
        if (iterator.first == nullptr)
        {
            Logger::log(LogLevel::Error,
                        "One of the ingredients is a null pointer.");
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
    for (auto it : schematics->ingredients)
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
    for (auto iterator : tools)
    {
        // Update the condition of the involved objects.
        iterator->triggerDecay();
        if (iterator->condition < 0)
        {
            actor->sendMsg(iterator->getName(true) + " falls into pieces.");
        }
    }
    // Send conclusion message.
    actor->sendMsg("You have finished building %s.\n\n",
                   Formatter::yellow() + schematics->buildingModel->getName() +
                   Formatter::reset());
    return ActionStatus::Finished;
}

unsigned int BuildAction::getConsumedStamina(Character * character)
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

unsigned int BuildAction::getCooldown(Character *, Building * _schematics)
{
    return _schematics->time;
}
