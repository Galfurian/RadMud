/// @file   ReloadAction.cpp
/// @brief  Implementation of the class for scouting an area.
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

ReloadAction::ReloadAction(RangedWeaponItem * _weapon, Character * _actor, unsigned int _cooldown) :
    GeneralAction(_actor, std::chrono::system_clock::now() + std::chrono::seconds(_cooldown)),
    weapon(_weapon)
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
    return "You stop reloading " + weapon->getName(true) + ".";
}

ActionStatus ReloadAction::perform()
{
    // Check if the cooldown is ended.
    if (!this->checkElapsed())
    {
        return ActionStatus::Running;
    }
    actor->sendMsg("You have finished reloading %s...\n", this->weapon->getName(true));
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
