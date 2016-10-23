/// @file   basicMeleeAttack.cpp
/// @brief  Contais the implementation of the class for the basic melee attacks.
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

#include "basicMeleeAttack.hpp"
#include "room.hpp"
#include "meleeWeaponItem.hpp"
#include "formatter.hpp"

BasicMeleeAttack::BasicMeleeAttack(Character * _actor) :
    CombatAction(_actor)
{
    Logger::log(LogLevel::Debug, "Created BasicMeleeAttack.");
}

BasicMeleeAttack::~BasicMeleeAttack()
{
    Logger::log(LogLevel::Debug, "Deleted BasicMeleeAttack.");
}

bool BasicMeleeAttack::check(std::string & error) const
{
    if (!CombatAction::check(error))
    {
        return false;
    }
    if (actor->getActiveMeleeWeapons().empty())
    {
        error = "You do not have a valid weapon equipped.";
    }
    return true;
}

ActionType BasicMeleeAttack::getType() const
{
    return ActionType::Combat;
}

std::string BasicMeleeAttack::getDescription() const
{
    return "fighting";
}

std::string BasicMeleeAttack::stop()
{
    return "You stop fighting.";
}

ActionStatus BasicMeleeAttack::perform()
{
    // Check if the cooldown is ended.
    if (!this->checkElapsed())
    {
        return ActionStatus::Running;
    }
    // If there are no enemies, just stop fighting.
    if (actor->aggressionList.empty())
    {
        return this->handleStop();
    }
    // Get a valid taret.
    Character * enemy = this->getValidTarget();
    if (enemy == nullptr)
    {
        actor->sendMsg("You do not have opponents at range.\n");
        return this->handleStop();
    }
    auto activeWeapons = actor->getActiveMeleeWeapons();
    // If there are no melee weapons available, use the natural weapons.
    if (activeWeapons.empty())
    {
        // Perform the attack.
        if (actor->isAtRange(enemy, 0))
        {
            this->performAttack(enemy, nullptr, false);
        }
        else
        {
            actor->sendMsg("You are too far away to attack.\n\n");
        }
    }
    else
    {
        bool dualWielding = (activeWeapons.size() > 1);
        for (auto iterator : activeWeapons)
        {
            // Perform the attack.
            if (actor->isAtRange(enemy, 0))
            {
                this->performAttack(enemy, iterator, dualWielding);
            }
            else
            {
                actor->sendMsg("You are too far away to attack.\n\n");
            }
        }
    }
    actor->getAction()->resetCooldown(actor->getCooldown(CombatActionType::BasicMeleeAttack));
    return ActionStatus::Running;
}

CombatActionType BasicMeleeAttack::getCombatActionType() const
{
    return CombatActionType::BasicMeleeAttack;
}

unsigned int BasicMeleeAttack::getConsumedStamina(Character * character, MeleeWeaponItem * weapon)
{
    // If the weapon is not specified.
    double weaponContribution;
    if (weapon == nullptr)
    {
        weaponContribution = 0.0;
    }
    else
    {
        weaponContribution = SafeLog10(weapon->getWeight(true));
    }
    // BASE     [+1.0]
    // STRENGTH [-0.0 to -2.80]
    // WEIGHT   [+1.6 to +2.51]
    // CARRIED  [+0.0 to +2.48]
    // WEAPON   [+0.0 to +1.60]
    return static_cast<unsigned int>(1.0
                                     - character->getAbilityLog(Ability::Strength, 0.0, 1.0)
                                     + SafeLog10(character->weight)
                                     + SafeLog10(character->getCarryingWeight())
                                     + weaponContribution);
}

void BasicMeleeAttack::performAttack(Character * target,
                                     MeleeWeaponItem * weapon,
                                     const bool dualWielding)
{
    // Get the required stamina.
    auto consumedStamina = this->getConsumedStamina(actor, weapon);
    // Check if the actor has enough stamina to execute the action.
    if (consumedStamina > actor->getStamina())
    {
        if (weapon != nullptr)
        {
            actor->sendMsg("You are too tired to attack with %s.\n", weapon->getName(true));
        }
        else
        {
            actor->sendMsg("You are too tired to attack with your %s.\n", actor->race->naturalWeapon);
        }
        return;
    }
    // Natural roll for the attack.
    auto attack = TRandInteger<unsigned int>(1, 20);
    // The ammount of damage dealt.
    unsigned int damage = 0;
    if (weapon != nullptr)
    {
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
        damage = weapon->rollDamage();
        // Check if the character is wielding a two-handed weapon.
        if (HasFlag(weapon->model->modelFlags, ModelFlag::TwoHand))
        {
            // Get the strenth modifier.
            auto strength = actor->getAbilityModifier(Ability::Strength);
            // Add to the damage rool one and half the strenth value.
            damage += strength + (strength / 2);
        }
    }
    else
    {
        // Natural roll for the damage.
        damage = TRandInteger<unsigned int>(1, 3 + actor->getAbilityModifier(Ability::Strength));
    }
    // Evaluate the armor class of the enemy.
    auto armorClass = target->getArmorClass();
    // Check if:
    //  1. The attack roll is lesser than the armor class of the opponent.
    //  2. The attack roll is not a natural 20.
    if ((attack < armorClass) && (attack != 20))
    {
        // Show miss messages.
        this->handleMissMessages(target, weapon);
        // Consume half the stamina.
        actor->remStamina(consumedStamina / 2, true);
        return;
    }
    else
    {
        // Show hit messages.
        this->handleHitMessages(target, weapon);
        // Consume the stamina.
        actor->remStamina(consumedStamina, true);
        // If the character has rolled a natural 20, then multiply the damage by two.
        if (attack == 20)
        {
            damage *= 2;
        }
        // Procede and remove the damage from the health of the target.
        if (!target->remHealth(damage))
        {
            // Notify the others.
            target->room->funcSendToAll("%s%s screams in pain and then die!%s\n",
                                        [&](Character * character)
                                        {
                                            return character != target;
                                        },
                                        Formatter::red(), target->getNameCapital(), Formatter::reset());
            // The target has received the damage and now it is dead.
            target->kill();
        }
    }
}

Character * BasicMeleeAttack::getValidTarget()
{
    Logger::log(LogLevel::Debug, "[%s] Trying to get a valid target...", actor->getName());
    // First try to get the next opponent at close combat range.
    Character * target = actor->getNextOpponentAtRange(0);
    // Otherwise, check if at least there are enemies in sight.
    if (target == nullptr)
    {
        Logger::log(LogLevel::Debug, "[%s] There is no target at close range.", actor->getName());
        for (auto iterator : actor->aggressionList)
        {
            if (actor->isAtRange(iterator->aggressor, actor->getViewDistance()))
            {
                target = iterator->aggressor;
                break;
            }
        }
    }
    return target;
}

ActionStatus BasicMeleeAttack::handleStop()
{
    actor->sendMsg(this->stop() + "\n\n");
    actor->aggressionList.resetList();
    return ActionStatus::Finished;
}

void BasicMeleeAttack::handleHitMessages(Character * target, MeleeWeaponItem * weapon)
{
    if (weapon != nullptr)
    {
        // The target has received the damage but it is still alive.
        actor->sendMsg("You hit %s with %s.\n\n", target->getName(), weapon->getName(true));
        // Notify the target.
        target->sendMsg("%s hits you with %s.\n\n", actor->getNameCapital(), weapon->getName(true));
        // Notify the other characters.
        target->room->funcSendToAll("%s hits %s with %s.\n",
                                    [&](Character * character)
                                    {
                                        return !((character == actor) || (character == target));
                                    },
                                    actor->getNameCapital(),
                                    target->getName(),
                                    weapon->getName(true));
    }
    else
    {
        // The target has received the damage but it is still alive.
        actor->sendMsg("You hit %s with your %s.\n\n", target->getName(), actor->race->naturalWeapon);
        // Notify the target.
        target->sendMsg("%s hits you with %s %s.\n\n",
                        actor->getNameCapital(),
                        actor->getPossessivePronoun(),
                        actor->race->naturalWeapon);
        // Notify the other characters.
        target->room->funcSendToAll("%s hits %s with %s %s.\n",
                                    [&](Character * character)
                                    {
                                        return !((character == actor) || (character == target));
                                    },
                                    actor->getNameCapital(),
                                    target->getName(),
                                    actor->getPossessivePronoun(),
                                    actor->race->naturalWeapon);
    }
}

void BasicMeleeAttack::handleMissMessages(Character * target, MeleeWeaponItem * weapon)
{
    // Notify the actor, enemy and others.
    if (weapon != nullptr)
    {
        actor->sendMsg("You miss %s with %s.\n\n",
                       target->getName(),
                       weapon->getName(true));
        target->sendMsg("%s misses you with %s.\n\n",
                        actor->getNameCapital(),
                        weapon->getName(true));
        target->room->sendToAll("%s misses %s with %s.\n", {actor, target},
                                actor->getNameCapital(),
                                target->getName(),
                                weapon->getName(true));
    }
    else
    {
        actor->sendMsg("You miss %s with your %s.\n\n",
                       target->getName(),
                       actor->race->naturalWeapon);
        target->sendMsg("%s misses you with %s %s.\n\n",
                        actor->getNameCapital(),
                        actor->getPossessivePronoun(),
                        actor->race->naturalWeapon);
        target->room->sendToAll("%s misses %s with %s %s.\n", {actor, target},
                                actor->getNameCapital(),
                                target->getName(),
                                actor->getPossessivePronoun(),
                                actor->race->naturalWeapon);
    }
}
