/// @file   basicRangedAttack.cpp
/// @brief  Contais the implementation of the class for the basic ranged attacks.
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

#include "basicRangedAttack.hpp"
#include "rangedWeaponModel.hpp"
#include "rangedWeaponItem.hpp"
#include "formatter.hpp"
#include "room.hpp"

BasicRangedAttack::BasicRangedAttack(Character * _actor) :
    CombatAction(_actor)
{
    Logger::log(LogLevel::Debug, "Created BasicRangedAttack.");
}

BasicRangedAttack::~BasicRangedAttack()
{
    Logger::log(LogLevel::Debug, "Deleted BasicRangedAttack.");
}

bool BasicRangedAttack::check(std::string & error) const
{
    if (!CombatAction::check(error))
    {
        return false;
    }
    return true;
}

ActionType BasicRangedAttack::getType() const
{
    return ActionType::Combat;
}

std::string BasicRangedAttack::getDescription() const
{
    return "fighting";
}

std::string BasicRangedAttack::stop()
{
    return "You stop fighting.";
}

ActionStatus BasicRangedAttack::perform()
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
        return ActionStatus::Finished;
    }
    // Iterate trough all the ranged weapons.
    auto activeWeapons = actor->getActiveRangedWeapons();
    bool dualWielding = (activeWeapons.size() > 1);
    bool canAttackTarget = false;
    for (auto iterator : activeWeapons)
    {
        Character * enemy = actor->aimedCharacter;
        // Check if the target is at range of the weapon.
        if (!actor->isAtRange(enemy, iterator->getRange()))
        {
            // If the aimed character is not at range, switch to a closer target.
            enemy = actor->getNextOpponentAtRange(iterator->getRange());
            if (enemy == nullptr)
            {
                actor->sendMsg("%s is not at range for %s.\n", actor->getNameCapital(), iterator->getName(true));
                continue;
            }
        }
        canAttackTarget = true;
        this->performAttack(enemy, iterator, dualWielding);
    }
    if ((actor->aimedCharacter == nullptr) && actor->aggressionList.empty())
    {
        actor->sendMsg(this->stop() + "\n\n");
        return ActionStatus::Finished;
    }
    if (!canAttackTarget)
    {
        actor->sendMsg(this->stop() + "\n\n");
        return ActionStatus::Finished;
    }
    // By default set the next combat action to basic attack.
    if (!actor->setNextCombatAction(CombatActionType::BasicRangedAttack))
    {
        actor->sendMsg(this->stop() + "\n\n");
        return ActionStatus::Finished;
    }
    return ActionStatus::Running;
}

CombatActionType BasicRangedAttack::getCombatActionType() const
{
    return CombatActionType::BasicRangedAttack;
}

unsigned int BasicRangedAttack::getConsumedStamina(Character * character, RangedWeaponItem * weapon)
{
    // BASE     [+1.0]
    // STRENGTH [-0.0 to -2.80]
    // WEIGHT   [+1.6 to +2.51]
    // CARRIED  [+0.0 to +2.48]
    // WEAPON   [+0.0 to +1.60]
    if (weapon->model->toRangedWeapon()->rangedWeaponType == RangedWeaponType::Thrown)
    {
        return static_cast<unsigned int>(1.0
                                         - character->getAbilityLog(Ability::Strength, 0.0, 1.0)
                                         + SafeLog10(character->weight)
                                         + SafeLog10(character->getCarryingWeight())
                                         + SafeLog10(weapon->getWeight(true)));
    }
    return 0;
}

void BasicRangedAttack::performAttack(Character * target,
                                      RangedWeaponItem * weapon,
                                      const bool dualWielding)
{
    // Get the required stamina.
    auto consumedStamina = this->getConsumedStamina(actor, weapon);
    // Check if the actor has enough stamina to execute the action.
    if (consumedStamina > actor->getStamina())
    {
        actor->sendMsg("You are too tired to attack with %s.\n", weapon->getName(true));
        return;
    }
    // Natural roll for the attack.
    auto attack = TRandInteger<unsigned int>(1, 20);
    // Check if:
    //  1. The number of active weapons is more than 1, then we have to apply a penality to the attack roll.
    //  2. The value is NOT a natural 20 (hit).
    if (dualWielding && (attack != 20))
    {
        // Evaluate the penalty to the attack roll.
        unsigned int penalty = 0;
        // On the RIGHT hand the penality is 6.
        // On the LEFT  hand the penality is 10.
        if (weapon->currentSlot == EquipmentSlot::RightHand) penalty = 6;
        if (weapon->currentSlot == EquipmentSlot::LeftHand) penalty = 10;
        // Safely apply the penality.
        attack = (attack < penalty) ? 0 : (attack - penalty);
    }
    // Natural roll for the damage.
    //unsigned int damage = weapon->rollDamage();
    // Evaluate the armor class of the aimed character.
    auto armorClass = actor->aimedCharacter->getArmorClass();
    // Check if:
    //  1. The attack roll is lesser than the armor class of the opponent.
    //  2. The attack roll is not a natural 20.
    if ((attack < armorClass) && (attack != 20))
    {
        this->handleMiss(target, weapon);
        // Consume half the stamina.
        actor->remStamina(consumedStamina / 2, true);
    }
    else
    {
        this->handleHit(target, weapon);
        // Consume the stamina.
        actor->remStamina(consumedStamina, true);
        // Natural roll for the damage.
        auto damage = weapon->rollDamage();
        // If the character has rolled a natural 20, then multiply the damage by two.
        if (attack == 20)
        {
            damage *= 2;
        }
        if (!target->remHealth(damage))
        {
            // Notify the others.
            target->room->sendToAll("%s%s screams in pain and then die!%s\n", {target},
                                    Formatter::red(), target->getNameCapital(), Formatter::reset());
            // The target has received the damage and now it is dead.
            target->kill();
        }
    }
}

void BasicRangedAttack::handleHit(Character * target, RangedWeaponItem * weapon)
{
    // The enemy has received the damage but it is still alive.
    actor->sendMsg("You hit %s with %s.\n\n", target->getName(), weapon->getName(true));
    // Notify the enemy.
    if (target->aimedCharacter == actor)
    {
        target->sendMsg("%s fires a projectile which hits you.\n\n", actor->getNameCapital());
    }
    else
    {
        target->sendMsg("Someone fires a projectile that hits you.\n\n");
    }
    // Notify the others.
    actor->room->funcSendToAll("%s fires and hits %s with %s.\n",
                               [&](Character * character)
                               {
                                   if (character == nullptr) return false;
                                   if (character == actor) return false;
                                   if (character->aimedCharacter == nullptr) return false;
                                   return character->aimedCharacter == target;
                               },
                               actor->getNameCapital(),
                               target->getName(),
                               weapon->getName(true));
    actor->room->funcSendToAll("%s fires at someone or something with %s.\n",
                               [&](Character * character)
                               {
                                   if (character == nullptr) return false;
                                   if (character == actor) return false;
                                   if (character->aimedCharacter == nullptr) return true;
                                   return character->aimedCharacter != target;
                               },
                               actor->getNameCapital(),
                               weapon->getName(true));
    target->room->funcSendToAll("%s fires a projectile which hits %s.\n",
                                [&](Character * character)
                                {
                                    if (character == nullptr) return false;
                                    if (character == target) return false;
                                    if (character->aimedCharacter == nullptr) return false;
                                    return character->aimedCharacter == actor;
                                },
                                actor->getNameCapital(),
                                target->getName(),
                                weapon->getName(true));
    target->room->funcSendToAll("Someone fires a projectile which hits %s.\n",
                                [&](Character * character)
                                {
                                    if (character == nullptr) return false;
                                    if (character == target) return false;
                                    if (character->aimedCharacter == nullptr) return true;
                                    return character->aimedCharacter != actor;
                                },
                                target->getName());
}

void BasicRangedAttack::handleMiss(Character * target, RangedWeaponItem * weapon)
{
    // Notify the actor.
    actor->sendMsg("You fire and miss %s with %s.\n\n", target->getName(), weapon->getName(true));
    // Notify the enemy.
    if (target->aimedCharacter == actor)
    {
        target->sendMsg("%s fires an misses you.\n\n", actor->getName());
    }
    else
    {
        target->sendMsg("Someone fired at you, but missed.\n\n");
    }
    // Notify the others.
    actor->room->funcSendToAll("%s fires and misses %s with %s.\n",
                               [&](Character * character)
                               {
                                   if (character == nullptr) return false;
                                   if (character == actor) return false;
                                   if (character->aimedCharacter == nullptr) return false;
                                   return character->aimedCharacter == target;
                               },
                               actor->getNameCapital(),
                               target->getName(),
                               weapon->getName(true));
    actor->room->funcSendToAll("%s fires at someone or something with %s.\n",
                               [&](Character * character)
                               {
                                   if (character == nullptr) return false;
                                   if (character == actor) return false;
                                   if (character->aimedCharacter == nullptr) return true;
                                   return character->aimedCharacter != target;
                               },
                               actor->getNameCapital(),
                               weapon->getName(true));
    target->room->funcSendToAll("%s fires a projectile which whizzes nearby %s.\n",
                                [&](Character * character)
                                {
                                    if (character == nullptr) return false;
                                    if (character == target) return false;
                                    if (character->aimedCharacter == nullptr) return false;
                                    return character->aimedCharacter == actor;
                                },
                                actor->getNameCapital(),
                                target->getName(),
                                weapon->getName(true));
    target->room->funcSendToAll("Something whizzes nearby %s.\n",
                                [&](Character * character)
                                {
                                    if (character == nullptr) return false;
                                    if (character == target) return false;
                                    if (character->aimedCharacter == nullptr) return true;
                                    return character->aimedCharacter != actor;
                                },
                                target->getName());
}
