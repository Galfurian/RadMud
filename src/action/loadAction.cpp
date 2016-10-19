/// @file   loadAction.cpp
/// @brief  Implementation of the class for loading activities.
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

#include "loadAction.hpp"
#include "../character/character.hpp"
#include "../model/magazineModel.hpp"
#include "../sqlite/sqliteDbms.hpp"

LoadAction::LoadAction(Item * _itemToBeLoaded, Item * _projectile, Character * _actor, unsigned int _cooldown) :
    GeneralAction(_actor, std::chrono::system_clock::now() + std::chrono::seconds(_cooldown)),
    itemToBeLoaded(_itemToBeLoaded),
    projectile(_projectile)
{
    Logger::log(LogLevel::Debug, "Created load action.");
}

LoadAction::~LoadAction()
{
    Logger::log(LogLevel::Debug, "Deleted load action.");
}

bool LoadAction::check(std::string & error) const
{
    if (!GeneralAction::check(error))
    {
        return false;
    }
    if (itemToBeLoaded == nullptr)
    {
        Logger::log(LogLevel::Error, "The item is a null pointer.");
        error = "You don't have a valid item to load.";
        return false;
    }
    else
    {
        if (itemToBeLoaded->model->getType() != ModelType::Magazine)
        {
            Logger::log(LogLevel::Error, "The item is not a magazine.");
            error = "You cannot load " + itemToBeLoaded->getName(true);
            return false;
        }
    }
    if (projectile == nullptr)
    {
        Logger::log(LogLevel::Error, "The projectile is a null pointer.");
        error = "You don't have valid projectiles to load.";
        return false;
    }
    if (!itemToBeLoaded->isEmpty())
    {
        auto loaded = itemToBeLoaded->content.front();
        if (loaded == nullptr)
        {
            Logger::log(LogLevel::Error, "The item already contains an item which is a null pointer.");
            error = "Something is gone wrong while you were loading " + itemToBeLoaded->getName(true) + ".";
            return false;
        }
        // If there are projectiles inside, check if the two types of projectiles are compatible.
        if (!projectile->canStackWith(loaded))
        {
            error = "You cannot stack the item with the one inside the container.";
            return false;
        }
        if (itemToBeLoaded->model->toMagazine()->maxAmmount <= loaded->quantity)
        {
            error = "The item is already at full capacity.";
            return false;
        }
    }
    return true;
}

ActionType LoadAction::getType() const
{
    return ActionType::Load;
}

std::string LoadAction::getDescription() const
{
    return "loading";
}

std::string LoadAction::stop()
{
    return "You stop loading " + itemToBeLoaded->getName(true) + ".";
}

ActionStatus LoadAction::perform()
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
    auto ammountToLoad = itemToBeLoaded->model->toMagazine()->maxAmmount;
    // First check if there are already some projectiles inside the magazine.
    if (!itemToBeLoaded->isEmpty())
    {
        auto loaded = itemToBeLoaded->content.front();
        ammountToLoad -= loaded->quantity;
        SQLiteDbms::instance().beginTransaction();
        if (projectile->quantity < ammountToLoad)
        {
            itemToBeLoaded->putInside(projectile);
        }
        else
        {
            loaded->quantity += ammountToLoad;
            projectile->quantity -= ammountToLoad;
            loaded->updateOnDB();
            projectile->updateOnDB();
        }
        SQLiteDbms::instance().endTransaction();
    }
    else
    {
        SQLiteDbms::instance().beginTransaction();
        if (projectile->quantity < ammountToLoad)
        {
            itemToBeLoaded->putInside(projectile);
        }
        else
        {
            auto newProjectileStack = projectile->removeFromStack(actor, ammountToLoad);
            if (newProjectileStack == nullptr)
            {
                // Rollback the transaction.
                SQLiteDbms::instance().rollbackTransection();
                actor->sendMsg("Something is gone wrong while you were loading %s...\n\n",
                               itemToBeLoaded->getName(true));
                return ActionStatus::Error;
            }
            itemToBeLoaded->putInside(newProjectileStack);
        }
        SQLiteDbms::instance().endTransaction();
    }
    actor->sendMsg("You have finished loading %s with %s...\n\n",
                   itemToBeLoaded->getName(true),
                   projectile->getName(true));
    return ActionStatus::Finished;
}
