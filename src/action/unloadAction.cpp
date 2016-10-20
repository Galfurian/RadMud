/// @file   unloadAction.cpp
/// @brief  Implementation of the class for unloading activities.
/// @author Enrico Fraccaroli
/// @date   Oct 10 2016
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

#include "unloadAction.hpp"
#include "character.hpp"
#include "sqliteDbms.hpp"

UnloadAction::UnloadAction(Item * _itemToBeUnloaded, Character * _actor, unsigned int _cooldown) :
    GeneralAction(_actor, std::chrono::system_clock::now() + std::chrono::seconds(_cooldown)),
    itemToBeUnloaded(_itemToBeUnloaded)
{
    Logger::log(LogLevel::Debug, "Created unload action.");
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
