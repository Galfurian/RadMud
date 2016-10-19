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

bool ReloadAction::check(std::string & error) const
{
    if (!GeneralAction::check(error))
    {
        return false;
    }
    if (weapon == nullptr)
    {
        Logger::log(LogLevel::Error, "The weapon is a null pointer.");
        error = "The weapon you want to reaload is missing.";
        return false;
    }
    if (magazine == nullptr)
    {
        Logger::log(LogLevel::Error, "The magazine is a null pointer.");
        error = "The magazine you want to use is missing.";
        return false;
    }
    if (!weapon->isEmpty())
    {
        Logger::log(LogLevel::Error, "The weapon already contains a magazine.");
        error = "The weapon already contains a magazine.";
        return false;
    }
    return true;
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
    if ((weapon != nullptr) && (magazine != nullptr))
    {
        return "You stop reloading " + weapon->getName(true) + " with " + magazine->getName(true) + ".";
    }
    return "You stop reloading.";
}

ActionStatus ReloadAction::perform()
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
    SQLiteDbms::instance().beginTransaction();
    if (!actor->remEquipmentItem(magazine) && !actor->remInventoryItem(magazine))
    {
        // Rollback the transaction.
        SQLiteDbms::instance().rollbackTransection();
        actor->sendMsg("Something is gone wrong while you were reloading %s...\n\n", weapon->getName(true));
        return ActionStatus::Error;
    }
    weapon->putInside(magazine);
    SQLiteDbms::instance().endTransaction();
    actor->sendMsg("You have finished reloading %s...\n\n", this->weapon->getName(true));
    return ActionStatus::Finished;
}
