/// @file   aimAction.cpp
/// @brief  Implementation of the class for aiming a target.
/// @author Enrico Fraccaroli
/// @date   Oct 13 2016
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

#include "aimAction.hpp"
#include "character.hpp"
#include "area.hpp"
#include "room.hpp"

AimAction::AimAction(Character * _actor, Character * _target, unsigned int _cooldown) :
    GeneralAction(_actor, std::chrono::system_clock::now() + std::chrono::seconds(_cooldown)),
    target(_target)
{
    Logger::log(LogLevel::Debug, "Created aim action.");
}

AimAction::~AimAction()
{
    Logger::log(LogLevel::Debug, "Deleted aim action.");
}

bool AimAction::check(std::string & error) const
{
    if (!GeneralAction::check(error))
    {
        return false;
    }
    if (target == nullptr)
    {
        Logger::log(LogLevel::Error, "The target is a null pointer.");
        error = "You don't have a valid target.";
        return false;
    }
    if (!actor->isAtRange(target, actor->getViewDistance()))
    {
        error = target->getNameCapital() + " is out of sight.";
        return false;
    }
    return true;
}

ActionType AimAction::getType() const
{
    return ActionType::Aim;
}

std::string AimAction::getDescription() const
{
    return "aiming";
}

std::string AimAction::stop()
{
    if (target != nullptr)
    {
        return "You stop aiming at " + target->getName() + ".";
    }
    return "You stop aiming.";
}

ActionStatus AimAction::perform()
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
    if (actor->getActiveRangedWeapons().empty())
    {
        actor->sendMsg("You don't have a ranged weapon equipped.\n\n");
        return ActionStatus::Error;
    }
    // Send the message.
    actor->sendMsg("You are now aiming at %s...\n\n", target->getName());
    // Set the aimed character.
    actor->aimedCharacter = target;
    return ActionStatus::Finished;
}

int AimAction::getAimTime(Character * source, Character * target)
{
    int requiredTime = 2;
    if (source && target)
    {
        if (source->room && target->room)
        {
            requiredTime = 2 + Area::getDistance(source->room->coord, target->room->coord);
        }
    }
    return requiredTime;
}
