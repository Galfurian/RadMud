/// @file   generalAction.cpp
/// @brief  Implements the base class for any kind of action.
/// @author Enrico Fraccaroli
/// @date   Jul 13 2016
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

#include "generalAction.hpp"
#include "character.hpp"
#include "mobile.hpp"
#include "logger.hpp"
#include <lua.hpp>
#include <cassert>

GeneralAction::GeneralAction(Character * _actor,
                             const bool & _lastAction) :
    actor(_actor),
    lastAction(_lastAction),
    actionCooldown()
{
    // Debugging message.
    Logger::log(LogLevel::Debug, "Created GeneralAction.");
}

GeneralAction::~GeneralAction()
{
    Logger::log(LogLevel::Debug, "Deleted GeneralAction.");
}

bool GeneralAction::checkElapsed() const
{
    return (std::chrono::duration_cast<std::chrono::seconds>(
        actionCooldown - std::chrono::system_clock::now()).count() <= 0);
}

long int GeneralAction::getElapsed() const
{
    return std::chrono::duration_cast<std::chrono::seconds>(
        actionCooldown - std::chrono::system_clock::now()).count();
}

bool GeneralAction::check(std::string & error) const
{
    if (actor == nullptr)
    {
        error = "You cannot begin the action.";
        return false;
    }
    if (actor->room == nullptr)
    {
        error = "You are currently nowhere.";
        return false;
    }
    return true;
}

ActionType GeneralAction::getType() const
{
    return ActionType::Wait;
}

std::string GeneralAction::getDescription() const
{
    return "waiting";
}

std::string GeneralAction::stop()
{
    return "Excuse me sir, but you are doing nothing.";
}

ActionStatus GeneralAction::perform()
{
    return ActionStatus::Finished;
}

unsigned int GeneralAction::getCooldown()
{
    return static_cast<unsigned int>(
        std::chrono::duration_cast<std::chrono::seconds>(
            actionCooldown - std::chrono::system_clock::now()).count());
}

void GeneralAction::resetCooldown(const unsigned int & _actionCooldown)
{
    actionCooldown = std::chrono::system_clock::now();
    if (_actionCooldown == 0)
    {
        actionCooldown += std::chrono::seconds(this->getCooldown());
    }
    else
    {
        actionCooldown += std::chrono::seconds(_actionCooldown);
    }
}

std::shared_ptr<CombatAction> GeneralAction::toCombatAction()
{
    return std::static_pointer_cast<CombatAction>(this->shared_from_this());
}