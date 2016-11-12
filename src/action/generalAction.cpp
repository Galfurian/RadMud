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
#include "combatAction.hpp"

using namespace std::chrono;

GeneralAction::GeneralAction(Character * _actor) :
    actor(_actor),
    actionCooldown()
{
    // Nothing to do.
}

GeneralAction::~GeneralAction()
{
    // Nothing to do.
}

bool GeneralAction::checkElapsed() const
{
    return (duration_cast<seconds>(actionCooldown - system_clock::now()).count() <= 0);
}

long int GeneralAction::getElapsed() const
{
    return duration_cast<seconds>(actionCooldown - system_clock::now()).count();
}

bool GeneralAction::check(std::string & error) const
{
    bool correct = true;
    if (actor == nullptr)
    {
        error = "You cannot begin the action.";
    }
    return correct;
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

void GeneralAction::resetCooldown(const unsigned int & _actionCooldown)
{
    this->actionCooldown = system_clock::now() + seconds(_actionCooldown);
}

unsigned int GeneralAction::getCooldown()
{
    return static_cast<unsigned int>(duration_cast<seconds>(actionCooldown - system_clock::now()).count());
}

std::shared_ptr<CombatAction> GeneralAction::toCombatAction()
{
    return std::static_pointer_cast<CombatAction>(this->shared_from_this());
}

std::string GetActionTypeName(ActionType type)
{
    if (type == ActionType::Wait) return "Waiting";
    if (type == ActionType::Move) return "Moving";
    if (type == ActionType::Crafting) return "Crafting";
    if (type == ActionType::Building) return "Building";
    if (type == ActionType::Combat) return "Fighting";
    if (type == ActionType::Scout) return "Scouting";
    if (type == ActionType::Reload) return "Reloading";
    if (type == ActionType::Load) return "Loading";
    if (type == ActionType::Unload) return "Unloading";
    if (type == ActionType::Aim) return "Aiming";
    return "No Action";
}
