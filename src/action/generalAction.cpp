/// @file   generalAction.cpp
/// @brief  Implements the base class for any kind of action.
/// @author Enrico Fraccaroli
/// @date   Jul 13 2016
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

#include "generalAction.hpp"
#include "../logger.hpp"

using namespace std::chrono;

GeneralAction::GeneralAction(Character * _actor) :
        actor(_actor),
        actionCooldown()
{
    Logger::log(LogLevel::Debug, "Created general action.");
}

GeneralAction::GeneralAction(Character * _actor, TimeClock _actionCooldown) :
        actor(_actor),
        actionCooldown(_actionCooldown)
{
    Logger::log(LogLevel::Debug, "Created general action.");
}

GeneralAction::~GeneralAction()
{
    Logger::log(LogLevel::Debug, "Deleted general action.");
}

bool GeneralAction::checkElapsed() const
{
    return (duration_cast<seconds>(actionCooldown - system_clock::now()).count() <= 0);
}

bool GeneralAction::check() const
{
    bool correct = true;
    if (this->checkElapsed())
    {
        Logger::log(LogLevel::Error, "Cooldown is already expired.");
        correct = false;
    }
    return correct;
}

ActionType GeneralAction::getType() const
{
    return ActionType::Wait;
}

std::string GeneralAction::getDescription() const
{
    return "";
}

std::string GeneralAction::stop()
{
    return "Excuse me sir, but you are doing nothing.";
}

ActionStatus GeneralAction::perform()
{
    return ActionStatus::Finished;
}

std::string GetActionTypeName(ActionType type)
{
    if (type == ActionType::Wait) return "Waiting";
    if (type == ActionType::Move) return "Moving";
    if (type == ActionType::Crafting) return "Crafting";
    if (type == ActionType::Building) return "Building";
    if (type == ActionType::Combat) return "Fighting";
    return "No Action";
}
