/// @file   reloadAction.cpp
/// @brief  Implementation of the class for reloading activities.
/// @author Enrico Fraccaroli
/// @date   Oct 9 2016
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

#include "reloadAction.hpp"
#include "../character/character.hpp"
#include "../sqlite/sqliteDbms.hpp"

ReloadAction::ReloadAction(RangedWeaponItem * _weapon, Item * _magazine, Character * _actor, unsigned int _cooldown) :
    GeneralAction(_actor, std::chrono::system_clock::now() + std::chrono::seconds(_cooldown)),
    weapon(_weapon),
    magazine(_magazine)
{
    Logger::log(LogLevel::Debug, "Created reload action.");
}

ReloadAction::~ReloadAction()
{
    Logger::log(LogLevel::Debug, "Deleted reload action.");
}

bool ReloadAction::check() const
{
    bool correct = GeneralAction::check();
    correct &= this->checkWeapon();
    correct &= this->checkMagazine();
    return correct;
}

ActionType ReloadAction::getType() const
{
    return ActionType::Reload;
}

std::string ReloadAction::getDescription() const
{
    return "reloading";
}

std::string ReloadAction::stop()
{
    return "You stop reloading " + weapon->getName(true) + " with " + magazine->getName(true) + ".";
}

ActionStatus ReloadAction::perform()
{
    // Check if the cooldown is ended.
    if (!this->checkElapsed())
    {
        return ActionStatus::Running;
    }
    // First check if there is already a magazine inside the weapon.
    if (!weapon->isEmpty())
    {
        actor->sendMsg("%s already contains a magazine...\n\n", weapon->getName(true));
        return ActionStatus::Error;
    }
    SQLiteDbms::instance().beginTransaction();
    if (!actor->remEquipmentItem(magazine))
    {
        if (!actor->remInventoryItem(magazine))
        {
            // Rollback the transaction.
            SQLiteDbms::instance().rollbackTransection();
            actor->sendMsg("Something is gone wrong while you were reloading %s...\n\n", weapon->getName(true));
            return ActionStatus::Error;
        }
    }
    weapon->putInside(magazine);
    SQLiteDbms::instance().endTransaction();
    actor->sendMsg("You have finished reloading %s...\n\n", this->weapon->getName(true));
    return ActionStatus::Finished;
}

bool ReloadAction::checkWeapon() const
{
    if (weapon == nullptr)
    {
        Logger::log(LogLevel::Error, "The weapon is a null pointer.");
        return false;
    }
    return true;
}

bool ReloadAction::checkMagazine() const
{
    if (magazine == nullptr)
    {
        Logger::log(LogLevel::Error, "The magazine is a null pointer.");
        return false;
    }
    return true;
}
