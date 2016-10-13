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
#include "../character/character.hpp"

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

bool AimAction::check() const
{
    bool correct = GeneralAction::check();
    correct &= this->checkTarget();
    return correct;
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
    if (this->checkTarget())
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
    if (this->checkTarget())
    {
        // Check if the target is still in sight.
        if (actor->isAtRange(target, actor->getViewDistance()))
        {
            actor->sendMsg("You are now aiming at %s...\n\n", target->getName());
            // Set the aimed character.
            actor->aimedCharacter = target;
        }
        else
        {
            actor->sendMsg("%s is out of your line of sight...\n\n", target->getNameCapital());
            return ActionStatus::Error;
        }
    }
    return ActionStatus::Finished;
}

bool AimAction::checkTarget() const
{
    if (target == nullptr)
    {
        Logger::log(LogLevel::Error, "The target is a null pointer.");
        return false;
    }
    return true;
}
