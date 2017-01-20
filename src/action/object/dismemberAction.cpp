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
#include "logger.hpp"
#include "room.hpp"

DismemberAction::DismemberAction(Character * _actor, CorpseItem * _corpse) :
    GeneralAction(_actor),
    corpse(_corpse)
{
    // Debugging message.
    Logger::log(LogLevel::Debug, "Created DismemberAction.");
    // Reset the cooldown of the action.
    this->resetCooldown(DismemberAction::getCooldown(_actor));
}

DismemberAction::~DismemberAction()
{
    Logger::log(LogLevel::Debug, "Deleted DismemberAction.");
}

bool DismemberAction::check(std::string & error) const
{
    if (!GeneralAction::check(error))
    {
        return false;
    }
    if (corpse == nullptr)
    {
        error = "You cannot find the corpse you want to dismember.";
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
    // Get the first available body part.
    auto it = corpse->remainingBodyParts.begin();
    if (it == corpse->remainingBodyParts.end())
    {
        actor->sendMsg("You fail to dismember %s.",
                       corpse->getName(true));
        return ActionStatus::Error;
    }
    auto bodyPart = (*it);
    // Remove the body part.
    corpse->remainingBodyParts.erase(it);
    corpse->remainingBodyParts.shrink_to_fit();
    // Create the resources of the given body part.
    for (auto resources : bodyPart->resources)
    {
        auto item = resources.resource->createItem(
            actor->getName(),
            resources.material,
            false,
            ItemQuality::Normal,
            static_cast<unsigned int>(resources.quantity));
        if (actor->canCarry(item, item->quantity))
        {
            actor->addInventoryItem(item);
        }
        else if (actor->room != nullptr)
        {
            actor->room->addItem(item);
        }
        Skill::improveSkillKnowledge(actor, Knowledge::Butchery);
        actor->sendMsg("You successfully butcher %s and produce %s.",
                       corpse->getName(true),
                       item->getName(true));
    }
    return ActionStatus::Finished;
}

unsigned int DismemberAction::getCooldown(Character * /*character*/)
{
    return 3;
}
