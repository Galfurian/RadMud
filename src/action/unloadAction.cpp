/// @file   unloadAction.cpp
/// @brief  Implementation of the class for unloading activities.
/// @author Enrico Fraccaroli
/// @date   Oct 10 2016
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

#include "unloadAction.hpp"
#include "character.hpp"
#include "sqliteDbms.hpp"

UnloadAction::UnloadAction(Character * _actor, Item * _itemToBeUnloaded) :
    GeneralAction(_actor),
    itemToBeUnloaded(_itemToBeUnloaded)
{
    // Debugging message.
    Logger::log(LogLevel::Debug, "Created UnloadAction.");
    // Reset the cooldown of the action.
    this->resetCooldown(UnloadAction::getUnloadTime(_itemToBeUnloaded));
}

UnloadAction::~UnloadAction()
{
    Logger::log(LogLevel::Debug, "Deleted unload action.");
}

bool UnloadAction::check(std::string & error) const
{
    if (!GeneralAction::check(error))
    {
        return false;
    }
    if (itemToBeUnloaded == nullptr)
    {
        Logger::log(LogLevel::Error, "The item is a null pointer.");
        error = "The item you want to unload is missing.";
        return false;
    }
    if (itemToBeUnloaded->isEmpty())
    {
        error = itemToBeUnloaded->getNameCapital(true) + " is already empty.";
        return false;
    }
    if (!itemToBeUnloaded->isEmpty())
    {
        auto loadedItem = itemToBeUnloaded->content.front();
        if (loadedItem == nullptr)
        {
            Logger::log(LogLevel::Error, "The item does not contain any loaded item.");
            error = "Something is gone wrong while you were unloading " + itemToBeUnloaded->getName(true) + ".";
            return false;
        }
    }
    return true;
}

ActionType UnloadAction::getType() const
{
    return ActionType::Unload;
}

std::string UnloadAction::getDescription() const
{
    return "unloading";
}

std::string UnloadAction::stop()
{
    if (itemToBeUnloaded != nullptr)
    {
        return "You stop unloading " + itemToBeUnloaded->getName(true) + ".";
    }
    return "You stop unloading.";
}

ActionStatus UnloadAction::perform()
{
    // Check if the cooldown is ended.
    if (!this->checkElapsed())
    {
        return ActionStatus::Running;
    }
    std::string error;
    if (!this->check(error))
    {
        actor->sendMsg(error + "\n\n");
        return ActionStatus::Error;
    }
    auto loadedItem = itemToBeUnloaded->content.front();
    SQLiteDbms::instance().beginTransaction();
    itemToBeUnloaded->takeOut(loadedItem);
    actor->addInventoryItem(loadedItem);
    SQLiteDbms::instance().endTransaction();
    actor->sendMsg("You have finished unloading %s...\n\n", itemToBeUnloaded->getName(true));
    return ActionStatus::Finished;
}

unsigned int UnloadAction::getUnloadTime(Item * _itemToBeUnloaded)
{
    if (_itemToBeUnloaded->getType() == ModelType::Magazine)
    {
        auto loadedItem = _itemToBeUnloaded->toMagazineItem()->getAlreadyLoadedProjectile();
        if (loadedItem != nullptr)
        {
            return static_cast<unsigned int>(loadedItem->getWeight(false) * loadedItem->quantity);
        }
    }
    return 1;
}
