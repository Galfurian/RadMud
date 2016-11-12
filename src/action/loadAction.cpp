/// @file   loadAction.cpp
/// @brief  Implementation of the class for loading activities.
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

#include "loadAction.hpp"
#include "character.hpp"
#include "magazineModel.hpp"
#include "sqliteDbms.hpp"

LoadAction::LoadAction(Character * _actor, Item * _itemToBeLoaded, Item * _projectile, const unsigned int & _ammount) :
    GeneralAction(_actor),
    itemToBeLoaded(_itemToBeLoaded),
    projectile(_projectile)
{
    // Debugging message.
    Logger::log(LogLevel::Debug, "Created LoadAction.");
    // Reset the cooldown of the action.
    this->resetCooldown(LoadAction::getLoadTime(_projectile, _ammount));
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
        if (projectile->quantity <= ammountToLoad)
        {
            actor->remInventoryItem(projectile);
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

unsigned int LoadAction::getLoadTime(Item * projectile, const unsigned int & ammountToLoad)
{
    // Evaluates the required time for loading the magazine.
    return static_cast<unsigned int>(projectile->getWeight(false) * ammountToLoad);
}