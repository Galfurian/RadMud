/// @file   combatAction.cpp
/// @brief  Contais the implementation of the virtual class for
///          general combat actions.
/// @author Enrico Fraccaroli
/// @date   Jul 14 2016
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

#include "action/combat/combatAction.hpp"

#include "utilities/logger.hpp"
#include "structure/room.hpp"
#include <cassert>

CombatAction::CombatAction(Character * _actor) :
    GeneralAction(_actor)
{
    Logger::log(LogLevel::Debug, "Created CombatAction.");
}

CombatAction::~CombatAction()
{
    Logger::log(LogLevel::Debug, "Deleted CombatAction.");
}

bool CombatAction::check(std::string & error) const
{
    return GeneralAction::check(error);
}

ActionType CombatAction::getType() const
{
    return ActionType::Combat;
}

std::string CombatAction::getDescription() const
{
    return "fighting";
}

std::string CombatAction::stop()
{
    return "You stop fighting.";
}

ActionStatus CombatAction::perform()
{
    return ActionStatus::Running;
}

unsigned int CombatAction::getCooldown()
{
    assert(actor && "Actor is nullptr");
    // BASE     [+5.0]
    // STRENGTH [-0.0 to -1.40]
    // AGILITY  [-0.0 to -1.40]
    // WEIGHT   [+1.6 to +2.51]
    // CARRIED  [+0.0 to +2.48]
    // WEAPON   [+0.0 to +1.60]
    unsigned int cooldown = 5;
    cooldown = SafeSum(cooldown, -actor->getAbilityLog(Ability::Strength));
    cooldown = SafeSum(cooldown, -actor->getAbilityLog(Ability::Agility));
    cooldown = SafeSum(cooldown, SafeLog10(actor->weight));
    cooldown = SafeSum(cooldown, SafeLog10(actor->getCarryingWeight()));
    for (auto bodyPart : actor->race->bodyParts)
    {
        if (!HasFlag(bodyPart->flags, BodyPartFlag::CanWield))
        {
            continue;
        }
        auto wpn = actor->findItemAtBodyPart(bodyPart);
        if (wpn == nullptr)
        {
            continue;
        }
        if ((wpn->getType() == ModelType::MeleeWeapon) ||
            (wpn->getType() == ModelType::RangedWeapon))
        {
            cooldown = SafeSum(cooldown, SafeLog10(wpn->getWeight(true)));
        }
    }
    return cooldown;
}


void CombatAction::handleStop()
{
    // Send the stop message.
    actor->sendMsg(this->stop() + "\n\n");
    // Reset the list of aggressors.
    actor->combatHandler.resetList();
}
