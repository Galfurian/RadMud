/// @file   basicAttack.cpp
/// @brief  Contais the implementation of the functions used for executing
///          basic attacks.
/// @author Enrico Fraccaroli
/// @date   Nov 2 2016
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

#include "basicAttack.hpp"

#include "rangedWeaponModel.hpp"
#include "formatter.hpp"
#include "updater.hpp"
#include "logger.hpp"
#include "chase.hpp"
#include "room.hpp"
#include "area.hpp"

BasicAttack::BasicAttack(Character * _actor) :
    CombatAction(_actor)
{
    // Debugging message.
    Logger::log(LogLevel::Debug, "Created BasicAttack.");
    // Reset the cooldown of the action.
    this->resetCooldown(CombatAction::getCooldown(_actor));
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
    std::string error;
    if (!this->check(error))
    {
        actor->sendMsg(error + "\n\n");
        return ActionStatus::Error;
    }
    // If there are no enemies, just stop fighting.
    if (actor->combatHandler.empty())
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
        Character * predefined = actor->combatHandler.getPredefinedTarget();
        // If the actor and the pred-target are in the same room,
        //  then use the melee weapons, otherwise use the ranged ones.
        if (actor->room->coord == predefined->room->coord)
        {
            // Since at melee weapon.
            hasAttackedTheTarget = true;
            // Retrieve all the melee weapons.
            auto meleeWeapons = actor->getActiveMeleeWeapons();
            // If there are no melee weapons available,
            //  use the natural weapons.
            if (meleeWeapons.empty())
            {
                // Perform the attack.
                this->performMeleeAttack(predefined, nullptr, false);
            }
            else
            {
                // Set the progressive number of the attack to 0.
                unsigned int attackNumber = 0;
                // Perform the attack for each weapon.
                for (auto weapon : meleeWeapons)
                {
                    // Perform the attack passing the melee weapon.
                    this->performMeleeAttack(predefined, weapon, attackNumber);
                    // Increment the number of executed attacks.
                    attackNumber++;
                }
            }
        }
        else
        {
            // Retrieve all the ranged weapons.
            auto rangedWeapons = actor->getActiveRangedWeapons();
            // Set the progressive number of the attack to 0.
            unsigned int attackNumber = 0;
            // Perform the attack for each weapon.
            for (auto weapon : rangedWeapons)
            {
                // Check if the target is at range of the weapon.
                if (actor->isAtRange(predefined, weapon->getRange()))
                {
                    // Set that the actor has actually attacked the target.
                    hasAttackedTheTarget = true;
                    // Perform the attack passing the ranged weapon.
                    this->performRangedAttack(predefined, weapon, attackNumber);
                    // Increment the number of executed attacks.
                    attackNumber++;
                }
            }
        }
    }
    // Check if the actor has not attacked anyone.
    if (!hasAttackedTheTarget)
    {
        // Check if the actor is a mobile.
        if (actor->isMobile())
        {
            // Take the enemy with the higher value of aggro.
            auto topAggro = actor->combatHandler.getTopAggro();
            if (topAggro->aggressor != nullptr)
            {
                auto chaseAction = std::make_shared<Chase>(actor,
                                                           topAggro->aggressor);
                if (chaseAction->check(error))
                {
                    // Add the action.
                    actor->pushAction(chaseAction);
                    // Return that the action is still running.
                    return ActionStatus::Running;
                }
                else
                {
                    actor->sendMsg(error + "\n\n");
                }
            }
            else
            {
                Logger::log(LogLevel::Error, "Top aggro is a nullptr!");
            }
        }
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

unsigned int BasicAttack::getConsumedStamina(Character * character,
                                             Item * weapon)
{
    // BASE     [+1.0]
    // STRENGTH [-0.0 to -1.40]
    // WEIGHT   [+1.6 to +2.51]
    // CARRIED  [+0.0 to +2.48]
    unsigned int consumedStamina = 1;
    consumedStamina -= character->getAbilityLog(Ability::Strength);
    consumedStamina = SafeSum(consumedStamina, SafeLog10(character->weight));
    consumedStamina = SafeSum(consumedStamina,
                              SafeLog10(character->getCarryingWeight()));
    if (weapon == nullptr)
    {
        return consumedStamina;
    }
    if (weapon->getType() == ModelType::MeleeWeapon)
    {
        consumedStamina = SafeSum(consumedStamina,
                                  SafeLog10(weapon->getWeight(true)));
    }
    else if (weapon->getType() == ModelType::RangedWeapon)
    {
        if (weapon->model->toRangedWeapon()->rangedWeaponType !=
            RangedWeaponType::Thrown)
        {
            return 0;
        }
        consumedStamina = SafeSum(consumedStamina,
                                  SafeLog10(weapon->getWeight(true)));
    }
    return consumedStamina;
}

bool BasicAttack::setPredefinedTarget()
{
    // If there is a predefined target, check if it is a valid target.
    if (actor->combatHandler.getPredefinedTarget() != nullptr)
    {
        Logger::log(LogLevel::Debug,
                    "[%s] Has a predefined target.",
                    actor->getNameCapital());
        if (this->checkTarget(actor->combatHandler.getPredefinedTarget()))
        {
            Logger::log(LogLevel::Debug,
                        "[%s] Predefined target is a valid target.",
                        actor->getNameCapital());
            return true;
        }
        Logger::log(LogLevel::Debug,
                    "[%s] Predefined target is NOT a valid target.",
                    actor->getNameCapital());
    }
    else
    {
        Logger::log(LogLevel::Debug,
                    "[%s] Has no predefined target.",
                    actor->getNameCapital());
    }
    // Take a valid target.
    for (auto it : actor->combatHandler)
    {
        if (this->checkTarget(it->aggressor))
        {
            Logger::log(LogLevel::Debug,
                        "[%s] Has a new predefined target: %s",
                        actor->getNameCapital(),
                        it->aggressor->getNameCapital());
            actor->combatHandler.setPredefinedTarget(it->aggressor);
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
        Logger::log(LogLevel::Debug,
                    "Either the actor or the target is a nullptr.");
        return false;
    }
    // Check their rooms.
    if ((actor->room == nullptr) || (target->room == nullptr))
    {
        Logger::log(LogLevel::Debug,
                    "[%s] Either the actor or the target are in a nullptr room.",
                    actor->getNameCapital());
        return false;
    }
    // Check if they are at close range.
    if (actor->room->coord == target->room->coord)
    {
        Logger::log(LogLevel::Debug,
                    "[%s] The actor and the target are in the same room.",
                    actor->getNameCapital());
        return true;
    }
    // Check if there is no aimed character.
    if (actor->combatHandler.getAimedTarget() == nullptr)
    {
        Logger::log(LogLevel::Debug,
                    "[%s] The actor has no aimed character, so the target cannot be attacked.",
                    actor->getNameCapital());
        return false;
    }
    // If at long range, check if the target is the aimed character.
    if (actor->combatHandler.getAimedTarget() == target)
    {
        Logger::log(LogLevel::Debug,
                    "[%s] The aimed character and the target are the same character.",
                    actor->getNameCapital());
        // Retrieve all the ranged weapons.
        auto rangedWeapons = actor->getActiveRangedWeapons();
        // Check if the actor has no ranged weapon equipped.
        if (rangedWeapons.empty())
        {
            Logger::log(LogLevel::Debug,
                        "[%s] The actor has no ranged weapon equipped.",
                        actor->getNameCapital());
            return false;
        }
        // Just check if AT LEAST one of the equipped ranged weapons can be used
        //  to attack the target.
        // TODO: This does not check if the weapon is USABLE!
        for (auto weapon : rangedWeapons)
        {
            if (actor->isAtRange(target, weapon->getRange()))
            {
                Logger::log(LogLevel::Debug,
                            "[%s] The target is at range with %s.",
                            actor->getNameCapital(),
                            weapon->getName(false));
                return true;
            }
        }
    }
    Logger::log(LogLevel::Debug,
                "[%s] No valid target has been found.",
                actor->getNameCapital());
    return false;
}

void BasicAttack::handleStop()
{
    // Send the stop message.
    actor->sendMsg(this->stop() + "\n\n");
    // Reset the list of aggressors.
    actor->combatHandler.resetList();
    // Clear the predefined target.
    actor->combatHandler.setPredefinedTarget(nullptr);
    // Clear the aimed character.
    actor->combatHandler.setAimedTarget(nullptr);
}

void BasicAttack::performMeleeAttack(Character * target,
                                     MeleeWeaponItem * weapon,
                                     unsigned int attackNumber)
{
    // -------------------------------------------------------------------------
    // Preamble: Evaluate all the values which are required by the function.
    // -------------------------------------------------------------------------
    // Get the strength modifier.
    auto strengthMod = actor->getAbilityModifier(Ability::Strength);
    // Get the required stamina.
    auto consumedStamina = this->getConsumedStamina(actor, weapon);
    // -------------------------------------------------------------------------
    // Phase 1: Check if the actor has enough stamina to execute the action.
    // -------------------------------------------------------------------------
    if (consumedStamina > actor->stamina)
    {
        if (weapon != nullptr)
        {
            actor->sendMsg("You are too tired to attack with %s.\n",
                           weapon->getName(true));
        }
        else
        {
            actor->sendMsg("You are too tired to attack with your %s.\n",
                           actor->race->naturalWeapon);
        }
        return;
    }
    // -------------------------------------------------------------------------
    // Phase 2: Roll the hit and damage.
    // -------------------------------------------------------------------------
    // Evaluate the natural roll for the attack.
    unsigned int hitRoll = TRandInteger<unsigned int>(1, 20);
    // Boolean variable which identifies if the hit is a critical hit.
    bool isCritical = (hitRoll == 20);
    // If there is a weapon, roll the damage.
    if (weapon != nullptr)
    {
        // Apply the penalty due to multi-wield, check if:
        //  1. The number of already executed attacks is greater-than 0.
        //  2. The attack is not a critical hit.
        if ((attackNumber > 0) && (!isCritical))
        {
            // Evaluate the penalty to the hit roll.
            unsigned int penalty = (attackNumber * 3);
            // Safely apply the penalty.
            hitRoll = (hitRoll < penalty) ? 0 : (hitRoll - penalty);
        }
        // Apply the modifier to the hit roll with melee weapons.
        hitRoll = SafeSum(hitRoll,
                          actor->effects.getCombatModifier(
                              CombatModifier::MeleeWeaponHitRoll));
        // Improve the skills which has contributed to the hit roll.
        Skill::improveSkillCombatModifier(actor,
                                          CombatModifier::MeleeWeaponHitRoll);
    }
    else
    {
        // Apply the modifier to the hit roll when unarmed fighting.
        hitRoll = SafeSum(hitRoll,
                          actor->effects.getCombatModifier(
                              CombatModifier::UnarmedHitRoll));
        // Improve the skills which has contributed to the hit roll.
        Skill::improveSkillCombatModifier(actor,
                                          CombatModifier::UnarmedHitRoll);
    }
    // -------------------------------------------------------------------------
    // Phase 3: Check if the target is hit.
    // -------------------------------------------------------------------------
    // Evaluate the armor class of the enemy.
    auto armorClass = target->getArmorClass();
    // Check if:
    //  1. The hit roll is lesser than the armor class of the opponent.
    //  2. The hit roll is not a critical.
    if ((hitRoll < armorClass) && (!isCritical))
    {
        // Show miss messages.
        this->handleMeleeMiss(target, weapon);
        // Consume half the stamina.
        actor->remStamina(consumedStamina / 2, true);
    }
    else
    {
        // The amount of damage dealt.
        unsigned int damageRoll = 0;
        // If there is a weapon, roll the damage.
        if (weapon != nullptr)
        {
            // Natural roll for the damage.
            damageRoll = weapon->rollDamage();
            // Check if the character is wielding a two-handed weapon.
            if (HasFlag(weapon->model->modelFlags, ModelFlag::TwoHand))
            {
                // Add to the damage roll one and half the strength value.
                damageRoll += strengthMod + (strengthMod / 2);
            }
            // Apply the modifier to the damage roll with melee weapons.
            damageRoll = SafeSum(damageRoll,
                                 actor->effects.getCombatModifier(
                                     CombatModifier::MeleeWeaponDamage));
            // Improve the skills which has contributed to the damage roll.
            Skill::improveSkillCombatModifier(
                actor, CombatModifier::MeleeWeaponDamage);
        }
        else
        {
            // Natural roll for the damage.
            damageRoll = TRandInteger<unsigned int>(1, 3 + strengthMod);
            // Apply the modifier to the damage roll when unarmed fighting.
            damageRoll = SafeSum(damageRoll,
                                 actor->effects.getCombatModifier(
                                     CombatModifier::UnarmedDamage));
            // Improve the skills which has contributed to the damage roll.
            Skill::improveSkillCombatModifier(actor,
                                              CombatModifier::UnarmedDamage);
        }
        // Show hit messages.
        this->handleMeleeHit(target, weapon);
        // Consume the stamina.
        actor->remStamina(consumedStamina, true);
        // If the character has rolled a critical: multiply the damage by two.
        if (isCritical) damageRoll *= 2;
        // Proceed and remove the damage from the health of the target.
        if (!target->remHealth(damageRoll))
        {
            // Notify the others.
            target->room->sendToAll(
                "%s%s screams in pain and then die!%s\n", {target},
                Formatter::red(), target->getNameCapital(), Formatter::reset());
            // The target has received the damage and now it is dead.
            target->kill();
            // Interrupt the function.
            return;
        }
    }
    if (!target->combatHandler.hasOpponent(actor))
    {
        target->combatHandler.addOpponent(actor);
        if (target->getAction()->getType() != ActionType::Combat)
        {
            target->pushAction(std::make_shared<BasicAttack>(target));
        }
    }
    if (!actor->combatHandler.hasOpponent(target))
    {
        actor->combatHandler.addOpponent(target);
    }
}

void BasicAttack::performRangedAttack(Character * target,
                                      RangedWeaponItem * weapon,
                                      unsigned int attackNumber)
{
    // -------------------------------------------------------------------------
    // Preamble: Evaluate all the values which are required by the function.
    // -------------------------------------------------------------------------
    // Get the required stamina.
    auto consumedStamina = this->getConsumedStamina(actor, weapon);
    // -------------------------------------------------------------------------
    // Phase 1: Check if the actor has enough stamina to execute the action.
    // -------------------------------------------------------------------------
    if (consumedStamina > actor->stamina)
    {
        actor->sendMsg("You are too tired to attack with %s.\n",
                       weapon->getName(true));
        return;
    }
    // -------------------------------------------------------------------------
    // Phase 2: Check if the ranged weapon has enough projectiles.
    // -------------------------------------------------------------------------
    std::string error;
    auto projectile = weapon->retrieveProjectile(error);
    if (projectile == nullptr)
    {
        actor->sendMsg(error + ".\n\n");
        return;
    }
    // -------------------------------------------------------------------------
    // Phase 3: Roll the hit and damage.
    // -------------------------------------------------------------------------
    // Evaluate the natural roll for the attack.
    unsigned int hitRoll = TRandInteger<unsigned int>(1, 20);
    // Boolean variable which identifies if the hit is a critical hit.
    bool isCritical = (hitRoll == 20);
    // Apply the penalty due to multi-wield, check if:
    //  1. The number of already executed attacks is greater-than 0.
    //  2. The attack is not a critical hit.
    if ((attackNumber > 0) && (!isCritical))
    {
        // Evaluate the penalty to the hit roll.
        unsigned int penalty = (attackNumber * 3);
        // Safely apply the penalty.
        hitRoll = (hitRoll < penalty) ? 0 : (hitRoll - penalty);
    }
    // Apply the modifier to the hit roll with ranged weapons.
    hitRoll = SafeSum(hitRoll,
                      actor->effects.getCombatModifier(
                          CombatModifier::RangedWeaponHitRoll));
    // Improve the skills which has contributed to the hit roll.
    Skill::improveSkillCombatModifier(actor,
                                      CombatModifier::RangedWeaponHitRoll);
    // -------------------------------------------------------------------------
    // Phase 3: Check if the target is hit.
    // -------------------------------------------------------------------------
    // Evaluate the armor class of the aimed character.
    auto armorClass = target->getArmorClass();
    // Check if:
    //  1. The hit roll is lesser than the armor class of the opponent.
    //  2. The hit roll is not a natural 20.
    if ((hitRoll < armorClass) && (!isCritical))
    {
        this->handleRangedMiss(target, weapon);
        // Consume half the stamina.
        actor->remStamina(consumedStamina / 2, true);
    }
    else
    {
        // Natural roll for the damage.
        unsigned int damageRoll = weapon->rollDamage();
        // Apply the modifier to the damage roll with ranged weapons.
        damageRoll = SafeSum(damageRoll,
                             actor->effects.getCombatModifier(
                                 CombatModifier::RangedWeaponDamage));
        // Improve the skills which has contributed to the damage roll.
        Skill::improveSkillCombatModifier(actor,
                                          CombatModifier::RangedWeaponDamage);
        // Show the message.
        this->handleRangedHit(target, weapon);
        // Consume the stamina.
        actor->remStamina(consumedStamina, true);
        // If the character has rolled a critical: multiply the damage by two.
        if (isCritical) damageRoll *= 2;
        // Proceed and remove the damage from the health of the target.
        if (!target->remHealth(damageRoll))
        {
            // Notify the others.
            target->room->sendToAll(
                "%s%s screams in pain and then die!%s\n", {target},
                Formatter::red(), target->getNameCapital(), Formatter::reset());
            // The target has received the damage and now it is dead.
            target->kill();
        }
        else
        {
            if (!target->combatHandler.hasOpponent(actor))
            {
                target->combatHandler.addOpponent(actor);
                if (target->getAction()->getType() != ActionType::Combat)
                {
                    target->pushAction(std::make_shared<BasicAttack>(target));
                }
            }
            if (!actor->combatHandler.hasOpponent(target))
            {
                actor->combatHandler.addOpponent(target);
            }
        }
    }
    // Check if it is the last projectile.
    if (projectile->quantity == 1)
    {
        MudUpdater::instance().addItemToDestroy(projectile);
    }
    else
    {
        projectile->quantity -= 1;
    }
}

void BasicAttack::handleMeleeHit(Character * target,
                                 MeleeWeaponItem * weapon)
{
    if (weapon != nullptr)
    {
        // The target has received the damage but it is still alive.
        actor->sendMsg("You hit %s with %s.\n\n",
                       target->getName(), weapon->getName(true));
        // Notify the target.
        target->sendMsg("%s hits you with %s.\n\n",
                        actor->getNameCapital(), weapon->getName(true));
        // Notify the other characters.
        target->room->funcSendToAll(
            "%s hits %s with %s.\n",
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
        actor->sendMsg("You hit %s with your %s.\n\n",
                       target->getName(), actor->race->naturalWeapon);
        // Notify the target.
        target->sendMsg(
            "%s hits you with %s %s.\n\n",
            actor->getNameCapital(),
            actor->getPossessivePronoun(),
            actor->race->naturalWeapon);
        // Notify the other characters.
        target->room->funcSendToAll(
            "%s hits %s with %s %s.\n",
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
    actor->sendMsg("You hit %s with %s.\n\n",
                   target->getName(),
                   weapon->getName(true));
    // Notify the other characters.
    if (actor->room == target->room)
    {
        target->sendMsg("%s fires a projectile which hits you.\n\n",
                        actor->getNameCapital());
        actor->room->funcSendToAll(
            "%s fires and hits %s with %s.\n",
            [&](Character * character)
            {
                return !((character == actor) || (character == target));
            },
            actor->getNameCapital(),
            target->getName(),
            weapon->getName(true));
        return;
    }
    // Notify the enemy.
    if (target->combatHandler.getAimedTarget() == actor)
    {
        target->sendMsg("%s fires a projectile which hits you.\n\n",
                        actor->getNameCapital());
    }
    else
    {
        target->sendMsg("Someone fires a projectile that hits you.\n\n");
    }
    actor->room->funcSendToAll(
        "%s fires and hits %s with %s.\n",
        [&](Character * character)
        {
            if ((character == actor) || (character == target)) return false;
            if (character->combatHandler.charactersInSight.containsCharacter(
                target))
                return true;
            return character->combatHandler.getAimedTarget() == target;
        }, actor->getNameCapital(), target->getName(),
        weapon->getName(true));

    actor->room->funcSendToAll(
        "%s fires at someone or something with %s.\n",
        [&](Character * character)
        {
            if ((character == actor) || (character == target)) return false;
            if (character->combatHandler.charactersInSight.containsCharacter(
                target))
                return false;
            return character->combatHandler.getAimedTarget() != target;
        }, actor->getNameCapital(), weapon->getName(true));

    target->room->funcSendToAll(
        "%s fires a projectile which hits %s.\n",
        [&](Character * character)
        {
            if ((character == actor) || (character == target)) return false;
            if (character->combatHandler.charactersInSight.containsCharacter(
                actor))
                return true;
            return character->combatHandler.getAimedTarget() == actor;
        }, actor->getNameCapital(), target->getName(), weapon->getName(true));

    target->room->funcSendToAll(
        "Someone fires a projectile which hits %s.\n",
        [&](Character * character)
        {
            if ((character == actor) || (character == target)) return false;
            if (character->combatHandler.charactersInSight.containsCharacter(
                actor))
                return false;
            return character->combatHandler.getAimedTarget() != actor;
        }, target->getName());
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
        target->room->sendToAll("%s misses %s with %s.\n",
                                {actor, target},
                                actor->getNameCapital(),
                                target->getName(),
                                weapon->getName(true));
        return;
    }
    actor->sendMsg("You miss %s with your %s.\n\n", target->getName(),
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

void
BasicAttack::handleRangedMiss(Character * target, RangedWeaponItem * weapon)
{
    // Notify the actor.
    actor->sendMsg("You fire and miss %s with %s.\n\n",
                   target->getName(),
                   weapon->getName(true));
    // Notify the other characters.
    if (actor->room == target->room)
    {
        target->sendMsg("%s fires an misses you.\n\n",
                        actor->getName());
        actor->room->funcSendToAll(
            "%s fires and misses %s with %s.\n",
            [&](Character * character)
            {
                return !((character == actor) || (character == target));
            }, actor->getNameCapital(), target->getName(),
            weapon->getName(true));
        return;
    }
    // Notify the enemy.
    if (target->combatHandler.getAimedTarget() == actor)
    {
        target->sendMsg("%s fires an misses you.\n\n", actor->getName());
    }
    else
    {
        target->sendMsg("Someone fired at you, but missed.\n\n");
    }
    actor->room->funcSendToAll(
        "%s fires and misses %s with %s.\n",
        [&](Character * character)
        {
            if ((character == actor) || (character == target)) return false;
            if (character->combatHandler.charactersInSight.containsCharacter(
                target))
                return true;
            return character->combatHandler.getAimedTarget() == target;
        }, actor->getNameCapital(), target->getName(), weapon->getName(true));

    actor->room->funcSendToAll(
        "%s fires at someone or something with %s.\n",
        [&](Character * character)
        {
            if ((character == actor) || (character == target)) return false;
            if (character->combatHandler.charactersInSight.containsCharacter(
                target))
                return false;
            return character->combatHandler.getAimedTarget() != target;
        }, actor->getNameCapital(), weapon->getName(true));

    target->room->funcSendToAll(
        "%s fires a projectile which whizzes nearby %s.\n",
        [&](Character * character)
        {
            if ((character == actor) || (character == target)) return false;
            if (character->combatHandler.charactersInSight.containsCharacter(
                actor))
                return true;
            return character->combatHandler.getAimedTarget() == actor;
        }, actor->getNameCapital(), target->getName(), weapon->getName(true));

    target->room->funcSendToAll(
        "Something whizzes nearby %s.\n",
        [&](Character * character)
        {
            if ((character == actor) || (character == target)) return false;
            if (character->combatHandler.charactersInSight.containsCharacter(
                actor))
                return false;
            return character->combatHandler.getAimedTarget() != actor;
        }, target->getName());
}
