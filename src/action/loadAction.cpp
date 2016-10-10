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

bool LoadAction::check() const
{
    bool correct = GeneralAction::check();
    correct &= this->checkItem();
    correct &= this->checkProjectile();
    return correct;
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
    unsigned int ammountToLoad = itemToBeLoaded->model->toMagazine()->maxAmmount;
    unsigned int ammountAlreadyLoaded = 0;
    // First check if there are already some projectiles inside the magazine.
    if (!itemToBeLoaded->isEmpty())
    {
        Logger::log(LogLevel::Debug, "The magazine is NOT empty...");
        Item * alreadyLoadedProjectiles = itemToBeLoaded->content.front();
        if (alreadyLoadedProjectiles == nullptr)
        {
            actor->sendMsg("Something is gone wrong while you were loading %s...\n\n", itemToBeLoaded->getName(true));
            return ActionStatus::Error;
        }
        // If there are projectiles inside, check if the two types of projectiles are compatible.
        if (!projectile->canStackWith(alreadyLoadedProjectiles))
        {
            actor->sendMsg("The magazine already contains a different type of projectiles...\n\n");
            return ActionStatus::Error;
        }
        // Set the ammount of already loaded projectiles.
        ammountAlreadyLoaded = alreadyLoadedProjectiles->quantity;
        if (ammountToLoad <= ammountAlreadyLoaded)
        {
            actor->sendMsg("The magazine is already at full capacity...\n\n");
            return ActionStatus::Finished;
        }
        ammountToLoad -= ammountAlreadyLoaded;
        Logger::log(LogLevel::Debug, "To Load   : %s", ToString(ammountToLoad));
        Logger::log(LogLevel::Debug, "Loaded    : %s", ToString(ammountAlreadyLoaded));
        Logger::log(LogLevel::Debug, "Available : %s", ToString(projectile->quantity));
        SQLiteDbms::instance().beginTransaction();
        if (projectile->quantity < ammountToLoad)
        {
            itemToBeLoaded->putInside(projectile);
        }
        else
        {
            alreadyLoadedProjectiles->quantity += ammountToLoad;
            projectile->quantity -= ammountToLoad;
            alreadyLoadedProjectiles->updateOnDB();
            projectile->updateOnDB();
        }
        SQLiteDbms::instance().endTransaction();
    }
    else
    {
        Logger::log(LogLevel::Debug, "The magazine is empty...");
        SQLiteDbms::instance().beginTransaction();
        if (projectile->quantity < ammountToLoad)
        {
            itemToBeLoaded->putInside(projectile);
        }
        else
        {
            Item * newProjectileStack = projectile->removeFromStack(actor, ammountToLoad);
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

bool LoadAction::checkItem() const
{
    if (itemToBeLoaded == nullptr)
    {
        Logger::log(LogLevel::Error, "The item is a null pointer.");
        return false;
    }
    return true;
}

bool LoadAction::checkProjectile() const
{
    if (projectile == nullptr)
    {
        Logger::log(LogLevel::Error, "The projectile is a null pointer.");
        return false;
    }
    return true;
}
