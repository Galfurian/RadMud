/// @file   aimAction.cpp
/// @brief  Implementation of the class for aiming a target.
/// @author Enrico Fraccaroli
/// @date   Oct 13 2016
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

#include "aimAction.hpp"

#include "character.hpp"
#include "logger.hpp"
#include "area.hpp"
#include "room.hpp"

AimAction::AimAction(Character * _actor, Character * _target) :
    GeneralAction(_actor),
    target(_target)
{
    // Debugging message.
    //Logger::log(LogLevel::Debug, "Created aim action.");
    // Reset the cooldown of the action.
    this->resetCooldown(AimAction::getAimTime(_actor, _target));
}

AimAction::~AimAction()
{
    //Logger::log(LogLevel::Debug, "Deleted aim action.");
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
    actor->combatHandler.setAimedTarget(target);
    return ActionStatus::Finished;
}

unsigned int AimAction::getAimTime(Character * source, Character * target)
{
    unsigned int requiredTime = 2;
    if ((source != nullptr) && (target != nullptr))
    {
        if ((source->room != nullptr) && (target->room != nullptr))
        {
            requiredTime = SafeSum(requiredTime, Area::getDistance(source->room->coord, target->room->coord));
        }
    }
    return requiredTime;
}
