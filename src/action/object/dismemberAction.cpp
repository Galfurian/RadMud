/// @file   dismemberAction.cpp
/// @author Enrico Fraccaroli
/// @date   Jan 13 2017
/// @copyright
/// Copyright (c) 2017 Enrico Fraccaroli <enrico.fraccaroli@gmail.com>
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

#include "dismemberAction.hpp"
#include "character.hpp"

DismemberAction::DismemberAction(Character * _actor,
                                 CorpseItem * _corpse,
                                 std::shared_ptr<BodyPart> _bodyPart) :
    GeneralAction(_actor),
    corpse(_corpse),
    bodyPart(_bodyPart)
{
    // Debugging message.
    //Logger::log(LogLevel::Debug, "Created DismemberAction.");
    // Reset the cooldown of the action.
    this->resetCooldown(DismemberAction::getCooldown(_actor, _bodyPart));
}

DismemberAction::~DismemberAction()
{
    //Logger::log(LogLevel::Debug, "Deleted move action.");
}

bool DismemberAction::check(std::string & error) const
{
    if (!GeneralAction::check(error))
    {
        return false;
    }
    return true;
}

ActionType DismemberAction::getType() const
{
    return ActionType::Dismember;
}

std::string DismemberAction::getDescription() const
{
    return "dismembering";
}

std::string DismemberAction::stop()
{
    return "You stop dismembering.";
}

ActionStatus DismemberAction::perform()
{
    // Check if the cooldown is ended.
    if (!this->checkElapsed())
    {
        return ActionStatus::Running;
    }
    return ActionStatus::Finished;
}

unsigned int DismemberAction::getCooldown(Character * /*character*/,
                                          std::shared_ptr<BodyPart>
                                          /*_bodyPart*/)
{
    // TODO: Evaluate the required time based on the skills of the character.
    return 3;
}
