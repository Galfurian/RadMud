/// @file   basicAttack.cpp
/// @brief  Contais the implementation of the functions used for executing basic attacks.
/// @author Enrico Fraccaroli
/// @date   Nov 2 2016
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

#include "basicAttack.hpp"
#include "rangedWeaponModel.hpp"
#include "rangedWeaponItem.hpp"
#include "meleeWeaponItem.hpp"
#include "formatter.hpp"
#include "room.hpp"
#include "area.hpp"

BasicAttack::BasicAttack(Character * _actor) :
    CombatAction(_actor)
{
    Logger::log(LogLevel::Debug, "Created BasicAttack.");
}

BasicAttack::~BasicAttack()
{
    Logger::log(LogLevel::Debug, "Deleted BasicAttack.");
}

bool BasicAttack::check(std::string & error) const
{
    return CombatAction::check(error);
}

ActionType BasicAttack::getType() const
{
    return ActionType::Combat;
}

std::string BasicAttack::getDescription() const
{
    return "fighting";
}

std::string BasicAttack::stop()
{
    return "You stop fighting.";
}

ActionStatus BasicAttack::perform()
{
    // Check if the cooldown is ended.
    if (!this->checkElapsed())
    {
        return ActionStatus::Running;
    }
    auto counter = 0;
    for (auto it : actor->actionQueue)
    {
        Logger::log(LogLevel::Debug, "[%s][%s] %s", actor->getName(), counter, it->getDescription());
        counter++;
    }
    // If there are no enemies, just stop fighting.
    if (actor->aggressionList.empty())
    {
        // Stop the combat.
        this->handleStop();
        // Return that the action is finished.
        return ActionStatus::Finished;
    }
    // Flag used to determine if the actor WAS able to attack the target.
    bool hasAttackedTheTarget = false;
    // Find a valid predefined target.
    if (this->setPredefinedTarget())
    {
        // Get the predefined target.
        Character * predefinedTarget = actor->aggressionList.getPredefinedTarget();
        // If the actor and the pred-target are in the same room, then use the melee weapons,
        //  otherwise use the ranged ones.
        if (actor->room->coord == predefinedTarget->room->coord)
        {
            // Since at melee weapon.
            hasAttackedTheTarget = true;
            // Retrieve all the melee weapons.
            auto meleeWeapons = actor->getActiveMeleeWeapons();
            // If there are no melee weapons available, use the natural weapons.
            if (meleeWeapons.empty())
            {
                // Perform the attack.
                this->performMeleeAttack(predefinedTarget, nullptr, false);
            }
            else
            {
                // Check if the actor is dual-wielding.
                bool dualWielding = (meleeWeapons.size() > 1);
                // Perform the attack for each weapon.
                for (auto weapon : meleeWeapons)
                {
                    // Perform the attack passing the melee weapon.
                    this->performMeleeAttack(predefinedTarget, weapon, dualWielding);
                }
            }
        }
        else
        {
            // Retrieve all the ranged weapons.
            auto rangedWeapons = actor->getActiveRangedWeapons();
            // Check if the actor is dual-wielding.
            bool dualWielding = (rangedWeapons.size() > 1);
            // Perform the attack for each weapon.
            for (auto weapon : rangedWeapons)
            {
                // Check if the target is at range of the weapon.
                if (actor->isAtRange(predefinedTarget, weapon->getRange()))
                {
                    // Set that the actor has actually attacked the target.
                    hasAttackedTheTarget = true;
                    // Perform the attack passing the ranged weapon.
                    this->performRangedAttack(predefinedTarget, weapon, dualWielding);
                }
            }
        }
    }
    // Check if the actor has not attacked anyone.
    if (!hasAttackedTheTarget)
    {
        actor->sendMsg("Try to get closer to your enemy.\n\n");
    }
    // Reset the cooldown.
    actor->getAction()->resetCooldown(CombatAction::getCooldown(actor));
    // Return that the action is still running.
    return ActionStatus::Running;
}

CombatActionType BasicAttack::getCombatActionType() const
{
    return CombatActionType::BasicAttack;
}

unsigned int BasicAttack::getConsumedStamina(Character * character, Item * weapon)
{
    // BASE     [+1.0]
    // STRENGTH [-0.0 to -1.40]
    // WEIGHT   [+1.6 to +2.51]
    // CARRIED  [+0.0 to +2.48]
    unsigned int consumedStamina = 1;
    consumedStamina -= character->getAbilityLog(Ability::Strength, 0.0, 1.0);
    consumedStamina = SafeSum(consumedStamina, SafeLog10(character->weight));
    consumedStamina = SafeSum(consumedStamina, SafeLog10(character->getCarryingWeight()));
    if (weapon == nullptr)
    {
        return consumedStamina;
    }
    if (weapon->getType() == ModelType::MeleeWeapon)
    {
        consumedStamina = SafeSum(consumedStamina, SafeLog10(weapon->getWeight(true)));
    }
    else if (weapon->getType() == ModelType::RangedWeapon)
    {
        if (weapon->model->toRangedWeapon()->rangedWeaponType != RangedWeaponType::Thrown)
        {
            return 0;
        }
        consumedStamina = SafeSum(consumedStamina, SafeLog10(weapon->getWeight(true)));
    }
    return consumedStamina;
}

bool BasicAttack::setPredefinedTarget()
{
    // If there is a predefined target, check if it is a valid target.
    if (actor->aggressionList.getPredefinedTarget() != nullptr)
    {
        Logger::log(LogLevel::Debug, "[%s] Has a predefined target.", actor->getNameCapital());
        if (this->checkTarget(actor->aggressionList.getPredefinedTarget()))
        {
            Logger::log(LogLevel::Debug, "[%s] Predefined target is a valid target.", actor->getNameCapital());
            return true;
        }
        Logger::log(LogLevel::Debug, "[%s] Predefined target is NOT a valid target.", actor->getNameCapital());
    }
    else
    {
        Logger::log(LogLevel::Debug, "[%s] Has no predefined target.", actor->getNameCapital());
    }
    // Take a valid target.
    for (auto it : actor->aggressionList)
    {
        if (this->checkTarget(it->aggressor))
        {
            Logger::log(LogLevel::Debug, "[%s] Has a new predefined target: %s",
                        actor->getNameCapital(),
                        it->aggressor->getNameCapital());
            actor->aggressionList.setPredefinedTarget(it->aggressor);
            return true;
        }
    }
    return false;
}

bool BasicAttack::checkTarget(Character * target)
{
    // Check characters.
    if ((actor == nullptr) || (target == nullptr))
    {
        Logger::log(LogLevel::Debug, "Either the actor or the target is a nullptr.");
        return false;
    }
    // Check their rooms.
    if ((actor->room == nullptr) || (target->room == nullptr))
    {
        Logger::log(LogLevel::Debug, "[%s] Either the actor or the target are in a nullptr room.",
                    actor->getNameCapital());
        return false;
    }
    // Check if they are at close range.
    if (actor->room->coord == target->room->coord)
    {
        Logger::log(LogLevel::Debug, "[%s] The actor and the target are in the same room.", actor->getNameCapital());
        return true;
    }
    // Check if there is no aimed character.
    if (actor->aimedCharacter == nullptr)
    {
        Logger::log(LogLevel::Debug, "[%s] The actor has no aimed character, so the target cannot be attacked.",
                    actor->getNameCapital());
        return false;
    }
    // If at long range, check if the target is the aimed character.
    if (actor->aimedCharacter->getName() == target->getName())
    {
        Logger::log(LogLevel::Debug, "[%s] The aimed character and the target are the same character.",
                    actor->getNameCapital());
        // Retrieve all the ranged weapons.
        auto rangedWeapons = actor->getActiveRangedWeapons();
        // Check if the actor has no ranged weapon equipped.
        if (rangedWeapons.empty())
        {
            Logger::log(LogLevel::Debug, "[%s] The actor has no ranged weapon equipped.", actor->getNameCapital());
            return false;
        }
        // Just check if AT LEAST one of the equipped ranged weapons can be used
        //  to attack the target.
        // TODO: This does not check if the weapon is USABLE!
        for (auto weapon : rangedWeapons)
        {
            if (actor->isAtRange(target, weapon->getRange()))
            {
                Logger::log(LogLevel::Debug, "[%s] The target is at range with %s.", actor->getNameCapital(),
                            weapon->getName(false));
                return true;
            }
        }
    }
    Logger::log(LogLevel::Debug, "[%s] No valid target has been found.", actor->getNameCapital());
    return false;
}

void BasicAttack::handleStop()
{
    // Send the stop message.
    actor->sendMsg(this->stop() + "\n\n");
    // Reset the list of aggressors.
    actor->aggressionList.resetList();
    // Clear the predefined target.
    actor->aggressionList.setPredefinedTarget(nullptr);
    // Remove the aimed character.
    actor->aimedCharacter = nullptr;
}

void BasicAttack::performMeleeAttack(Character * target, MeleeWeaponItem * weapon, const bool dualWielding)
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
    auto hitRoll = TRandInteger<unsigned int>(1, 20);
    // The amount of damage dealt.
    unsigned int damageRoll = 0;
    if (weapon != nullptr)
    {
        // Check if:
        //  1. The number of active weapons is more than 1, then we have to apply a penality to the hit roll.
        //  2. The value is NOT a natural 20 (hit).
        if (dualWielding && (hitRoll != 20))
        {
            // Evaluate the penalty to the hit roll.
            unsigned int penalty = 0;
            // On the RIGHT hand the penality is 6.
            // On the LEFT  hand the penality is 10.
            if (weapon->currentSlot == EquipmentSlot::RightHand) penalty = 6;
            if (weapon->currentSlot == EquipmentSlot::LeftHand) penalty = 10;
            // Safely apply the penality.
            hitRoll = (hitRoll < penalty) ? 0 : (hitRoll - penalty);
        }
        // Natural roll for the damage.
        damageRoll = weapon->rollDamage();
        // Check if the character is wielding a two-handed weapon.
        if (HasFlag(weapon->model->modelFlags, ModelFlag::TwoHand))
        {
            // Get the strenth modifier.
            auto strength = actor->getAbilityModifier(Ability::Strength);
            // Add to the damage rool one and half the strenth value.
            damageRoll += strength + (strength / 2);
        }
    }
    else
    {
        // Natural roll for the damage.
        damageRoll = TRandInteger<unsigned int>(1, 3 + actor->getAbilityModifier(Ability::Strength));
    }
    // Add effects modifier.
    hitRoll = SafeSum(hitRoll, actor->effects.getMeleeHitMod());
    // Evaluate the armor class of the enemy.
    auto armorClass = target->getArmorClass();
    // Flag used to check if the target is alive.
    bool targetIsAlive = true;
    // Check if:
    //  1. The hit roll is lesser than the armor class of the opponent.
    //  2. The hit roll is not a natural 20.
    if ((hitRoll < armorClass) && (hitRoll != 20))
    {
        // Show miss messages.
        this->handleMeleeMiss(target, weapon);
        // Consume half the stamina.
        actor->remStamina(consumedStamina / 2, true);
    }
    else
    {
        // Show hit messages.
        this->handleMeleeHit(target, weapon);
        // Consume the stamina.
        actor->remStamina(consumedStamina, true);
        // If the character has rolled a natural 20, then multiply the damage by two.
        if (hitRoll == 20)
        {
            damageRoll *= 2;
        }
        // Add effects modifier.
        hitRoll = SafeSum(hitRoll, actor->effects.getMeleeHitMod());
        // Procede and remove the damage from the health of the target.
        if (!target->remHealth(damageRoll))
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
            // Set to false the targetIsAlive flag.
            targetIsAlive = false;
        }
    }
    if (targetIsAlive)
    {
        if (!target->aggressionList.hasOpponent(actor))
        {
            target->aggressionList.addOpponent(actor);
            if (target->getAction()->getType() != ActionType::Combat)
            {
                target->actionQueue.push_front(std::make_shared<BasicAttack>(target));
            }
        }
        if (!actor->aggressionList.hasOpponent(target))
        {
            actor->aggressionList.addOpponent(target);
        }
    }
}

void BasicAttack::performRangedAttack(Character * target, RangedWeaponItem * weapon, const bool dualWielding)
{
    // Get the required stamina.
    auto consumedStamina = this->getConsumedStamina(actor, weapon);
    // Check if the actor has enough stamina to execute the action.
    if (consumedStamina > actor->getStamina())
    {
        actor->sendMsg("You are too tired to attack with %s.\n", weapon->getName(true));
        return;
    }
    // Natural roll for the hit rool.
    auto hitRoll = TRandInteger<unsigned int>(1, 20);
    // Check if:
    //  1. The number of active weapons is more than 1, then we have to apply a penality to the hit roll.
    //  2. The value is NOT a natural 20 (hit).
    if (dualWielding && (hitRoll != 20))
    {
        // Evaluate the penalty to the hit roll.
        unsigned int penalty = 0;
        // On the RIGHT hand the penality is 6.
        // On the LEFT  hand the penality is 10.
        if (weapon->currentSlot == EquipmentSlot::RightHand) penalty = 6;
        if (weapon->currentSlot == EquipmentSlot::LeftHand) penalty = 10;
        // Safely apply the penality.
        hitRoll = (hitRoll < penalty) ? 0 : (hitRoll - penalty);
    }
    // Add effects modifier.
    hitRoll = SafeSum(hitRoll, actor->effects.getMeleeHitMod());
    // Evaluate the armor class of the aimed character.
    auto armorClass = target->getArmorClass();
    // Flag used to check if the target is alive.
    bool targetIsAlive = true;
    // Check if:
    //  1. The hit roll is lesser than the armor class of the opponent.
    //  2. The hit roll is not a natural 20.
    if ((hitRoll < armorClass) && (hitRoll != 20))
    {
        this->handleRangedMiss(target, weapon);
        // Consume half the stamina.
        actor->remStamina(consumedStamina / 2, true);
    }
    else
    {
        this->handleRangedHit(target, weapon);
        // Consume the stamina.
        actor->remStamina(consumedStamina, true);
        // Natural roll for the damage rool.
        auto damageRoll = weapon->rollDamage();
        // If the character has rolled a natural 20, then multiply the damage by two.
        if (hitRoll == 20)
        {
            damageRoll *= 2;
        }
        // Add effects modifier.
        damageRoll = SafeSum(damageRoll, actor->effects.getRangedHitMod());
        if (!target->remHealth(damageRoll))
        {
            // Notify the others.
            target->room->sendToAll("%s%s screams in pain and then die!%s\n", {target},
                                    Formatter::red(), target->getNameCapital(), Formatter::reset());
            // The target has received the damage and now it is dead.
            target->kill();
            // Set to false the targetIsAlive flag.
            targetIsAlive = false;
        }
    }
    if (targetIsAlive)
    {
        if (!target->aggressionList.hasOpponent(actor))
        {
            target->aggressionList.addOpponent(actor);
            if (target->getAction()->getType() != ActionType::Combat)
            {
                target->actionQueue.push_front(std::make_shared<BasicAttack>(target));
            }
        }
        if (!actor->aggressionList.hasOpponent(target))
        {
            actor->aggressionList.addOpponent(target);
        }
    }
}

void BasicAttack::handleMeleeHit(Character * target, MeleeWeaponItem * weapon)
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

void BasicAttack::handleRangedHit(Character * target, RangedWeaponItem * weapon)
{
    // The enemy has received the damage but it is still alive.
    actor->sendMsg("You hit %s with %s.\n\n", target->getName(), weapon->getName(true));
    // Notify the other characters.
    if (actor->room == target->room)
    {
        target->sendMsg("%s fires a projectile which hits you.\n\n", actor->getNameCapital());
        actor->room->funcSendToAll("%s fires and hits %s with %s.\n",
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
        // Notify the enemy.
        if (target->aimedCharacter == actor)
        {
            target->sendMsg("%s fires a projectile which hits you.\n\n", actor->getNameCapital());
        }
        else
        {
            target->sendMsg("Someone fires a projectile that hits you.\n\n");
        }
        actor->room->funcSendToAll("%s fires and hits %s with %s.\n",
                                   [&](Character * character)
                                   {
                                       if ((character == actor) || (character == target)) return false;
                                       if (character->charactersInSight.containsCharacter(target)) return true;
                                       return character->aimedCharacter == target;
                                   },
                                   actor->getNameCapital(),
                                   target->getName(),
                                   weapon->getName(true));
        actor->room->funcSendToAll("%s fires at someone or something with %s.\n",
                                   [&](Character * character)
                                   {
                                       if ((character == actor) || (character == target)) return false;
                                       if (character->charactersInSight.containsCharacter(target)) return false;
                                       return character->aimedCharacter != target;
                                   },
                                   actor->getNameCapital(),
                                   weapon->getName(true));
        target->room->funcSendToAll("%s fires a projectile which hits %s.\n",
                                    [&](Character * character)
                                    {
                                        if ((character == actor) || (character == target)) return false;
                                        if (character->charactersInSight.containsCharacter(actor)) return true;
                                        return character->aimedCharacter == actor;
                                    },
                                    actor->getNameCapital(),
                                    target->getName(),
                                    weapon->getName(true));
        target->room->funcSendToAll("Someone fires a projectile which hits %s.\n",
                                    [&](Character * character)
                                    {
                                        if ((character == actor) || (character == target)) return false;
                                        if (character->charactersInSight.containsCharacter(actor)) return false;
                                        return character->aimedCharacter != actor;
                                    },
                                    target->getName());
    }
}

void BasicAttack::handleMeleeMiss(Character * target, MeleeWeaponItem * weapon)
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

void BasicAttack::handleRangedMiss(Character * target, RangedWeaponItem * weapon)
{
    // Notify the actor.
    actor->sendMsg("You fire and miss %s with %s.\n\n", target->getName(), weapon->getName(true));
    // Notify the other characters.
    if (actor->room == target->room)
    {
        target->sendMsg("%s fires an misses you.\n\n", actor->getName());
        actor->room->funcSendToAll("%s fires and misses %s with %s.\n",
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
        // Notify the enemy.
        if (target->aimedCharacter == actor)
        {
            target->sendMsg("%s fires an misses you.\n\n", actor->getName());
        }
        else
        {
            target->sendMsg("Someone fired at you, but missed.\n\n");
        }
        actor->room->funcSendToAll("%s fires and misses %s with %s.\n",
                                   [&](Character * character)
                                   {
                                       if ((character == actor) || (character == target)) return false;
                                       if (character->charactersInSight.containsCharacter(target)) return true;
                                       return character->aimedCharacter == target;
                                   },
                                   actor->getNameCapital(),
                                   target->getName(),
                                   weapon->getName(true));
        actor->room->funcSendToAll("%s fires at someone or something with %s.\n",
                                   [&](Character * character)
                                   {
                                       if ((character == actor) || (character == target)) return false;
                                       if (character->charactersInSight.containsCharacter(target)) return false;
                                       return character->aimedCharacter != target;
                                   },
                                   actor->getNameCapital(),
                                   weapon->getName(true));
        target->room->funcSendToAll("%s fires a projectile which whizzes nearby %s.\n",
                                    [&](Character * character)
                                    {
                                        if ((character == actor) || (character == target)) return false;
                                        if (character->charactersInSight.containsCharacter(actor)) return true;
                                        return character->aimedCharacter == actor;
                                    },
                                    actor->getNameCapital(),
                                    target->getName(),
                                    weapon->getName(true));
        target->room->funcSendToAll("Something whizzes nearby %s.\n",
                                    [&](Character * character)
                                    {
                                        if ((character == actor) || (character == target)) return false;
                                        if (character->charactersInSight.containsCharacter(actor)) return false;
                                        return character->aimedCharacter != actor;
                                    },
                                    target->getName());
    }
}
