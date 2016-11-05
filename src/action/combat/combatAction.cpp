/// @file   combatAction.cpp
/// @brief  Contais the implementation of the virtual class for general combat actions.
/// @author Enrico Fraccaroli
/// @date   Jul 14 2016
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

#include "combatAction.hpp"
#include "room.hpp"

using namespace std::chrono;

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

unsigned int CombatAction::getCooldown(Character * character)
{
    // BASE     [+5.0]
    // STRENGTH [-0.0 to -1.40]
    // AGILITY  [-0.0 to -1.40]
    // WEIGHT   [+1.6 to +2.51]
    // CARRIED  [+0.0 to +2.48]
    // WEAPON   [+0.0 to +1.60]
    unsigned int cooldown = 5;
    cooldown -= character->getAbilityLog(Ability::Strength, 0.0, 1.0);
    cooldown -= character->getAbilityLog(Ability::Agility, 0.0, 1.0);
    cooldown = SafeSum(cooldown, SafeLog10(character->weight));
    cooldown = SafeSum(cooldown, SafeLog10(character->getCarryingWeight()));
    if (character->canAttackWith(EquipmentSlot::RightHand))
    {
        auto weapon = character->findEquipmentSlotItem(EquipmentSlot::RightHand);
        cooldown = SafeSum(cooldown, SafeLog10(weapon->getWeight(true)));
    }
    if (character->canAttackWith(EquipmentSlot::LeftHand))
    {
        auto weapon = character->findEquipmentSlotItem(EquipmentSlot::RightHand);
        cooldown = SafeSum(cooldown, SafeLog10(weapon->getWeight(true)));
    }
    return cooldown;
}
