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
#include "../character/character.hpp"
#include "../model/magazineModel.hpp"
#include "../sqlite/sqliteDbms.hpp"

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

bool UnloadAction::check() const
{
    bool correct = GeneralAction::check();
    correct &= this->checkItem();
    return correct;
}

ActionType UnloadAction::getType() const
{
    return ActionType::Load;
}

std::string UnloadAction::getDescription() const
{
    return "unloading";
}

std::string UnloadAction::stop()
{
    return "You stop unloading " + itemToBeUnloaded->getName(true) + ".";
}

ActionStatus UnloadAction::perform()
{
    // Check if the cooldown is ended.
    if (!this->checkElapsed())
    {
        return ActionStatus::Running;
    }
    // First check if there are some projectiles inside the magazine.
    if (itemToBeUnloaded->isEmpty())
    {
        actor->sendMsg("%s is already empty...\n\n", itemToBeUnloaded->getNameCapital(true));
    }
    else
    {
        Item * loadedProjectile = itemToBeUnloaded->content.front();
        if (loadedProjectile == nullptr)
        {
            actor->sendMsg("Something is gone wrong while you were unloading %s...\n\n",
                           itemToBeUnloaded->getName(true));
            return ActionStatus::Error;
        }
        SQLiteDbms::instance().beginTransaction();
        itemToBeUnloaded->takeOut(loadedProjectile);
        actor->addInventoryItem(loadedProjectile);
        SQLiteDbms::instance().endTransaction();
        actor->sendMsg("You have finished unloading %s...\n\n", itemToBeUnloaded->getName(true));
    }
    return ActionStatus::Finished;
}

bool UnloadAction::checkItem() const
{
    if (itemToBeUnloaded == nullptr)
    {
        Logger::log(LogLevel::Error, "The item is a null pointer.");
        return false;
    }
    return true;
}
